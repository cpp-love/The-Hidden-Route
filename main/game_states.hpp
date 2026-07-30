/**
 * @file game_states.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了一些具体的游戏状态。
 * @version 0.1.0-7
 * @date 2026-07-22
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_MAIN_GAME_STATES_HPP
#define THR_MAIN_GAME_STATES_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/global/game_state_components.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <optional>
#include <sol/state.hpp>
#include <vector>

namespace mainhelper {

    /// @brief 关卡完成的事件组件。
    struct level_finished_event {
        bool should_unlock_relative_levels{true}; ///< 是否应该解锁相邻的关卡，默认不包含隐藏关卡。
        std::vector<std::string> extra_levels;    ///< 额外解锁的关卡。
    };

    /// @brief 设置状态类。
    class settings_menu : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        settings_menu();
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        settings_menu(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        settings_menu(settings_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        settings_menu &operator=(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        settings_menu &operator=(settings_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~settings_menu() override;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(thr::ecs::milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void draw() noexcept override;

      protected:
        /// @copydoc game_state_base::init
        void init() noexcept override;

      private:
        /// @brief 连接调度器。
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接调度器。
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停。
        entt::registry m_registry;          ///< 注册表。
    };

    /// @brief 主页状态类。
    class main_menu : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        main_menu();
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        main_menu(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        main_menu(main_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        main_menu &operator=(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        main_menu &operator=(main_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~main_menu() override;
        /// @copydoc game_state_base::on_pause
        void on_pause() override;
        /// @copydoc game_state_base::on_resume
        void on_resume() override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void update(thr::ecs::milliseconds_f delta_time) override;
        /// @copydoc game_state_base::draw
        void draw() override;

      protected:
        /// @copydoc game_state_base::init
        void init() override;

      private:
        /// @brief 连接调度器。
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接调度器。
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停。
        entt::registry m_registry;          ///< 注册表。
        sol::state     m_lua;               ///< Lua 状态。
    };

    /// @brief 关卡图界面状态类。
    class level_graph_screen : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        level_graph_screen();
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        level_graph_screen(const level_graph_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        level_graph_screen(level_graph_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        level_graph_screen &operator=(const level_graph_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        level_graph_screen &operator=(level_graph_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~level_graph_screen() override;
        /// @copydoc game_state_base::on_pause
        void on_pause() override;
        /// @copydoc game_state_base::on_resume
        void on_resume() override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) override;
        /// @copydoc game_state_base::update
        void update(thr::ecs::milliseconds_f delta_time) override;
        /// @copydoc game_state_base::draw
        void draw() override;

      protected:
        /// @copydoc game_state_base::init
        void init() override;

      private:
        /**
         * @brief 响应关卡完成的函数。
         * @param [in] event 事件组件。
         */
        void           on_level_finished(const level_finished_event &event);
        /// @brief 连接调度器。
        void           connect_dispatcher();
        /// @brief 断开连接调度器。
        void           disconnect_dispatcher();
        bool           m_is_paused = false;                ///< 是否暂停。
        entt::entity   m_current_level_entity{entt::null}; ///< 当前在玩的关卡对应的实体。
        entt::registry m_registry;                         ///< 注册表。
        sol::state     m_lua;                              ///< Lua 状态。
    };

    /// @brief 游戏界面状态类。
    class game_screen : public thr::ecs::game_state_base {
      public:
        /**
         * @brief 构造 game_screen 对象。
         * @param [in] level_name 关卡名称。
         */
        explicit game_screen(std::string_view level_name);
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        game_screen(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        game_screen(game_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        game_screen &operator=(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        game_screen &operator=(game_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~game_screen() override;
        /// @copydoc game_state_base::on_pause
        void on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool handle_event(const sf::Event &event) override;
        /// @copydoc game_state_base::update
        void update(thr::ecs::milliseconds_f delta_time) override;
        /// @copydoc game_state_base::draw
        void draw() override;

      protected:
        /// @copydoc game_state_base::init
        void init() override;

      private:
        /// @brief 连接调度器。
        void connect_dispatcher();
        /// @brief 断开连接调度器。
        void disconnect_dispatcher();
        bool m_is_paused = false; ///< 是否暂停。
        std::optional<thr::ecs::milliseconds_f>
                                  m_remaining_time_after_winning; ///< 胜利后等待的剩余时间。
        std::optional<sol::state> m_lua;                          ///< Lua 状态。
        entt::registry            m_registry;                     ///< 注册表。
        entt::entity              m_player_entity{entt::null};    ///< 玩家实体。
    };

    /// @brief 暂停界面状态类。
    class pause_menu : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        pause_menu();
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        pause_menu(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        pause_menu(pause_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        pause_menu &operator=(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        pause_menu &operator=(pause_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~pause_menu() override;
        /// @copydoc game_state_base::on_pause
        void               on_pause() noexcept override;
        /// @copydoc game_state_base::on_resume
        void               on_resume() noexcept override;
        /// @copydoc game_state_base::on_handle_event
        bool               handle_event(const sf::Event &event) noexcept override;
        /// @copydoc game_state_base::update
        void               update(thr::ecs::milliseconds_f delta_time) noexcept override;
        /// @copydoc game_state_base::draw
        void               draw() noexcept override;
        /// @copybrief game_state_base::should_block_passing_down
        [[nodiscard]] bool should_block_passing_down() noexcept override { return false; }

      protected:
        /// @copydoc game_state_base::init
        void init() noexcept override;

      private:
        /// @brief 连接调度器。
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接调度器。
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false; ///< 是否暂停。
        entt::registry m_registry;          ///< 注册表。
    };

} // namespace mainhelper

#endif // THR_MAIN_GAME_STATES_HPP