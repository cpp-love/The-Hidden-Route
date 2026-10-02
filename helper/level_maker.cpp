/**
 * @file level_maker.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 关卡制作器。
 * @version 0.1.0-3
 * @date 2026-10-01
 * 
 * @copyright cpp-love
 * 
 */

// 放在最前面，防止 `tgui::MessageBox` 的 `MessageBox` 被替换。
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif // _WIN32

#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/level_render_system.hpp"
#include "thr/ecs/systems/level_serialization_system.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System.hpp>
#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/FileDialog.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <algorithm>
#include <array>
#include <entt/entity/fwd.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

// 此部分用于生成路径段的轮廓。
// 此部分使用 AI 但已检查过正确性。
namespace {

    /// @brief 轮廓生成所需的路径段数据。
    struct segment_geometry {
        sf::FloatRect              bounds; ///< 段占据的轴对齐包围盒。
        int                        layer;  ///< 显示层级，数值越小越靠前。
        std::optional<std::size_t> prev;   ///< 前一段索引。
        std::optional<std::size_t> next;   ///< 后一段索引。
    };

    /// @brief 矩形的一条轴对齐边及其坐标范围。
    struct edge {
        bool  horizontal; ///< 线段是否水平。
        float fixed;      ///< 固定坐标：水平边为 y，竖直边为 x。
        float low;        ///< 可变坐标范围的起点。
        float high;       ///< 可变坐标范围的终点。
    };

    /**
     * @brief 查询当前点所在区域的最前层级。
     * @param [in] segments 所有的路径段。
     * @param [in] point 要查询的点。
     * @return std::optional<int> 最前面的段的层数。
     * @retval std::nullopt 当前点没有所属的段。
     * @details 时间复杂度为 $O(n)$，其中 $n$ 为路径段数量。
     */
    std::optional<int> frontmost_layer(const std::vector<segment_geometry> &segments,
                                       sf::Vector2f                         point) {
        std::optional<int> frontmost; //< 当前最前面的段的层数。
        for (const auto &cursegment : segments) {
            if (!cursegment.bounds.contains(point)) {
                continue;
            }
            if (!frontmost.has_value() || cursegment.layer < *frontmost) {
                frontmost = cursegment.layer;
            }
        }
        return frontmost;
    }

    /**
     * @brief 判断当前边区间外侧的采样点是否处于通过 `prev/next` 连接的段内。
     * @param [in] index 当前段在 `segments` 中的索引。
     * @param [in] segments 所有关卡路径段。
     * @param [in] outside 当前边区间外侧的采样点。
     * @return true 外侧采样点处于通过 `prev/next` 连接的段内。
     * @return false 外侧采样点不处于通过 `prev/next` 连接的段内。
     * @details 时间复杂度为 $O(1)$。
     */
    bool has_connected_neighbor(std::size_t index, const std::vector<segment_geometry> &segments,
                                sf::Vector2f outside) {
        const auto &segment = segments[index];
        auto        check_segment = [&](std::size_t curindex) {
            const auto &cursegment = segments[curindex];

            if (curindex == index) {
                return false;
            }

            const bool connected = segment.prev == curindex || segment.next == curindex
                                   || cursegment.prev == index || cursegment.next == index;
            if (!connected) {
                return false;
            }

            const auto &bounds = cursegment.bounds;
            return bounds.contains(outside);
        };

        return (segment.prev.has_value() && check_segment(*segment.prev))
               || (segment.next.has_value() && check_segment(*segment.next));
    }

    /**
     * @brief 将候选边切分并筛选出可见线段。
     * @param [in] edge 待处理的矩形边。
     * @param [in] entity 该边所属段在 `registry` 中的位置。
     * @param [in] registry 持有段内容的注册表。
     * @param [out] outline 追加可见线段的列表的地方。
     * @details 在其他矩形边界处切分候选边，采样各区间的内外两侧，再结合连接关系和层级筛选。
     */
    void visible_edge_points(const edge &edge, entt::entity entity, const entt::registry &registry,
                             thr::ecs : segment::outline &outline) {
        auto               segments = registry.view<thr::ecs::segment>();
        // 分割当前边，不管它的层数。
        std::vector<float> splits{edge.low, edge.high}; ///< 分割的点。
        for (const auto &[entity, segment] : segments.each()) {
            const auto &bounds = segment.get_bounds();
            const float perpendicular_low = edge.horizontal ? bounds.position.y : bounds.position.x;
            const float perpendicular_high =
                edge.horizontal ? bounds.position.y + bounds.size.y : bounds.position.x + bounds.size.x;
            if (edge.fixed < perpendicular_low || edge.fixed > perpendicular_high) {
                continue;
            }
            const float low = edge.horizontal ? bounds.position.x : bounds.position.y;
            const float high =
                edge.horizontal ? bounds.position.x + bounds.size.x : bounds.position.y + bounds.size.y;
            if (edge.low < low && low < edge.high) {
                splits.push_back(low);
            }
            if (edge.low < high && high < edge.high) {
                splits.push_back(high);
            }
        }
        std::ranges::sort(splits);

        // 根据线段层数确定可见部分。
        const auto &segment = registry.get<thr::ecs::segment>(entity);
        const auto &vertexs = outline.vertexs;
        const auto &bounds = segment.get_bounds();
        const float epsilon = std::max(1e-4f, thr::ecs::segment::width() * 1e-4f);
        for (const auto &[low, high] : splits | std::views::pairwise) {
            if (high - low <= epsilon) {
                continue;
            }

            const float  middle = (low + high) / 2;
            sf::Vector2f inside;
            sf::Vector2f outside;
            // 沿边法向偏移采样，判断边的迷宫内侧由哪一层占据。
            if (edge.horizontal) {
                const bool top_edge = edge.fixed == bounds.position.y;
                inside = {middle, edge.fixed + (top_edge ? epsilon : -epsilon)};
                outside = {middle, edge.fixed + (top_edge ? -epsilon : epsilon)};
            } else {
                const bool left_edge = edge.fixed == bounds.position.x;
                inside = {edge.fixed + (left_edge ? epsilon : -epsilon), middle};
                outside = {edge.fixed + (left_edge ? -epsilon : epsilon), middle};
            }

            const auto inside_layer = frontmost_layer(registry, inside);
            // 如果这条边是连接两条段的边或被其他段覆盖，删除这条边。
            if (has_connected_neighbor(registry, entity, outside) || !inside_layer.has_value()
                || *inside_layer != segment.layer) {
                continue;
            }

            if (edge.horizontal) {
                if (low != splits.front() && low - vertexs.back().back().x <= epsilon) {
                    vertexs.back().back().x = high;
                } else {
                    vertexs.emplace_back(
                        {sf::Vector2f{low, edge.fixed}, sf::Vector2f{high, edge.fixed}});
                }
            } else {
                if (low != splits.front() && low - vertexs.back().back().y <= epsilon) {
                    vertexs.back().back().y = high;
                } else {
                    vertexs.emplace_back(
                        {sf::Vector2f{edge.fixed, low}, sf::Vector2f{edge.fixed, high}});
                }
            }
        }
    }

    /**
     * @brief 为关卡中的每个路径段生成可见轮廓。
     * @param [in,out] registry 持有关卡内容的注册表；各段的 `outline.vertexs` 会被替换，其余字段保留。
     * @details 根据段包围盒、`prev`/`next` 连接关系和 `layer` 计算轮廓，不依赖原有 `outline.vertexs`。
     */
    void generate_outlines(entt::registry &registry) {
        auto segments = registry.view<thr::ecs::segment>();

        // 逐一添加每个矩形的边。
        for (auto [entity, segment] : segments.each()) {
            const auto               &bounds = segment.get_bounds();
            const std::array<edge, 4> edges{{
                {.horizontal = true,
                 .fixed = bounds.position.y,
                 .low = bounds.position.x,
                 .high = bounds.position.x + bounds.size.x},
                {.horizontal = false,
                 .fixed = bounds.position.x + bounds.size.x,
                 .low = bounds.position.y,
                 .high = bounds.position.y + bounds.size.y},
                {.horizontal = true,
                 .fixed = bounds.position.y + bounds.size.y,
                 .low = bounds.position.x,
                 .high = bounds.position.x + bounds.size.x},
                {.horizontal = false,
                 .fixed = bounds.position.x,
                 .low = bounds.position.y,
                 .high = bounds.position.y + bounds.size.y},
            }};
            auto                     &vertexs = segment.outline.vertexs;
            for (const auto &edge : edges) {
                visible_edge_points(edge, entity, registry, lines);
            }
        }
    }

} // namespace

namespace {
    /// @brief 关卡制作器的主界面。
    class maker_screen : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        maker_screen();
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        maker_screen(const maker_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        maker_screen(maker_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        maker_screen &operator=(const maker_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        maker_screen &operator=(maker_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~maker_screen() override;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(thr::ecs::milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw() override;

      protected:
        /// @copydoc game_state_base::init
        void init() noexcept override;

      private:
        /// @brief 连接调度器。
        void                                 connect_dispatcher();
        /// @brief 断开连接调度器。
        void                                 disconnect_dispatcher();

        /**
         * @brief 加载关卡。
         * @param [in] path 关卡的位置。
         * @return std::optional<std::string> 可能的错误。
         */
        std::optional<std::string>           load_level(std::filesystem::path path);
        /**
         * @brief 保存关卡。
         * @return std::optional<std::string> 可能的错误。
         */
        std::optional<std::string>           save_level();

        bool                                 m_is_paused = false; ///< 是否暂停。
        entt::registry                       m_registry;          ///< 注册表。
        std::optional<std::filesystem::path> m_current_level;     ///< 当前要编辑的关卡。
    };
    /// @todo 添加在未保存时退出和重新加载关卡的警告。

    maker_screen::maker_screen() { connect_dispatcher(); }
    maker_screen::~maker_screen() { disconnect_dispatcher(); }

    void maker_screen::init() noexcept {
        auto group =
            m_global_gui->get<tgui::Group>(thr::ecs::game_state_manager::game_screen_group_name);

        auto menu = tgui::MenuBar::create();
        menu->setSize({"100%", "4.5%"});
        menu->getRenderer()->setBackgroundColor(tgui::Color{230, 233, 239});
        menu->addMenu("File");
        menu->addMenuItem("File", "Load...");
        menu->addMenuItem("File", "Save");
        menu->connectMenuItem({"File", "Load..."}, [this, group] {
            auto file_dialog = tgui::FileDialog::create();
            file_dialog->setFileMustExist(true);
            file_dialog->setMultiSelect(false);
            file_dialog->setTitle("Load File");
            file_dialog->setPosition({"(parent.innersize - size) / 2"});
            file_dialog->onFileSelect([this, file_dialog, group] {
                if (std::optional err = load_level(file_dialog->getSelectedPaths()[0])) {
                    auto message_box = tgui::MessageBox::create("Error", *err);
                    message_box->setPosition({"(parent.innersize - size) / 2"});
                    message_box->addButton("OK");
                    message_box->onButtonPress([message_box] { message_box->close(); });
                    group->add(message_box);
                }
                file_dialog->close();
            });
            group->add(file_dialog);
        });
        menu->connectMenuItem({"File", "Save"}, [this, group] {
            if (std::optional err = save_level()) {
                auto message_box = tgui::MessageBox::create("Error", *err);
                message_box->setPosition({"(parent.innersize - size) / 2"});
                message_box->addButton("OK");
                message_box->onButtonPress([message_box] { message_box->close(); });
                group->add(message_box);
            }
        });
        group->add(menu, "menu");

        auto tool_panel = tgui::Panel::create();
        tool_panel->setPosition({0, "5%"});
        tool_panel->setSize({"6.25%", "95%"});
        tool_panel->getRenderer()->setBackgroundColor(tgui::Color{230, 233, 239});
        group->add(tool_panel, "tool_panel");

        auto level_screen = tgui::CanvasSFML::create();
        level_screen->setPosition({"6.75%", "5%"});
        level_screen->setSize({"62.75%", "95%"});
        sf::Vector2u size = thr::ecs::configs::singleton().game_screen_size;
        sf::Vector2f float_size{static_cast<float>(size.x), static_cast<float>(size.y)};
        level_screen->setView({float_size / 2.f, float_size});
        group->add(level_screen, "level_screen");

        auto segment_information =
            tgui::ChildWindow::create("Segment Information", tgui::ChildWindow::TitleButton::None);
        segment_information->setPosition({"70%", "5%"});
        segment_information->setSize({"30%", "95%"});
        group->add(segment_information, "segment_information");
    }
    void maker_screen::on_pause() noexcept { m_is_paused = true; }
    void maker_screen::on_resume() noexcept { m_is_paused = false; }
    bool maker_screen::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void maker_screen::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void maker_screen::draw() {
        auto level_screen =
            m_global_gui->get<tgui::Group>(thr::ecs::game_state_manager::game_screen_group_name)
                ->get<tgui::CanvasSFML>("level_screen");
        // adapted from `game_state_manager.cpp`
        // 更新视图。
        float         window_ratio = level_screen->getSize().x / level_screen->getSize().y;
        sf::View      view{level_screen->getView()};
        float         old_view_ratio = view.getSize().x / view.getSize().y;
        sf::FloatRect viewport{{0, 0}, {1, 1}};

        // 根据长宽比计算视口位置和大小，确保黑边方向正确。
        if (window_ratio > old_view_ratio) {
            // 窗口更扁，黑边在左右。
            viewport.size.x = old_view_ratio / window_ratio;
            viewport.position.x = (1 - viewport.size.x) / 2;
        } else {
            // 窗口更瘦高，黑边在上下。
            viewport.size.y = window_ratio / old_view_ratio;
            viewport.position.y = (1 - viewport.size.y) / 2;
        }

        // 创建并应用新的视口。
        view.setViewport(viewport);
        level_screen->setView(view);

        level_screen->clear(thr::ecs::configs::singleton().background_color);
        thr::ecs::level_render_system::draw(m_registry, level_screen->getRenderTexture());
        level_screen->display();
    }

    void maker_screen::connect_dispatcher() {
        thr::ecs::segment::connect_listener(m_registry);
        thr::ecs::node::connect_listener(m_registry);
        thr::ecs::player::connect_listener(m_registry);
    }
    void maker_screen::disconnect_dispatcher() {
        thr::ecs::segment::disconnect_listener(m_registry);
        thr::ecs::node::disconnect_listener(m_registry);
        thr::ecs::player::disconnect_listener(m_registry);
    }

    std::optional<std::string> maker_screen::load_level(std::filesystem::path path) {
        disconnect_dispatcher();
        m_registry.ctx().clear();
        m_registry.clear();
        connect_dispatcher();
        try {
            std::ifstream fin(path);
            // NOLINTNEXTLINE(hicpp-signed-bitwise)
            fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            nlohmann::json json;
            fin >> json;
            thr::ecs::level_serialization_system::deserialize_from_json(m_registry, json);

            m_current_level = std::move(path);
            return std::nullopt;
        } catch (std::exception &e) {
            m_current_level = std::nullopt;
            return e.what();
        }
    }
    std::optional<std::string> maker_screen::save_level() {
        if (!m_current_level.has_value()) {
            return "There is no file in the level maker, so it cannot be saved.";
        }

        try {
            std::ofstream fout(*m_current_level);
            // NOLINTNEXTLINE(hicpp-signed-bitwise)
            fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fout << thr::ecs::level_serialization_system::serialize_to_json(m_registry).dump(4);

            return std::nullopt;
        } catch (std::exception &e) {
            return e.what();
        }
    }
} // namespace

// copied from `main.cpp`
int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    // spdlog config
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);

    sf::RenderWindow             window(sf::VideoMode(thr::ecs::configs::singleton().game_screen_size),
                                        "The Hidden Route - Level Maker");
    thr::ecs::game_state_manager manager(window);
    manager.push_state(std::make_unique<maker_screen>());

    auto prev = thr::ecs::clock::now();
    while (true) {
        // handle event
        while (const std::optional event = window.pollEvent()) {
            manager.handle_event(*event);
        }
        if (!window.isOpen()) {
            break;
        }

        // update
        auto cur = thr::ecs::clock::now();
        manager.update(cur - prev);
        prev = cur;

        // render
        window.clear(thr::ecs::configs::singleton().background_color);
        manager.draw();
        window.display();
    }

    return 0;
}