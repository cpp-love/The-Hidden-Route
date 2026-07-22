/**
 * @file game_states.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了一些具体的游戏状态。
 * @version 0.1.0-6
 * @date 2026-07-14
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_MAIN_GAME_STATES_HPP
#define THR_MAIN_GAME_STATES_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/global/game_state_components.hpp"
#include "thr/ecs/lua_bindings/lua_manager.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <optional>

namespace mainhelper {

    /// @brief 关卡完成的事件组件。
    struct level_finished_event {};

    /// @brief 设置状态类。
    class settings_menu : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        settings_menu() noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        settings_menu(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        settings_menu(settings_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        settings_menu &operator=(const settings_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        settings_menu &operator=(settings_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~settings_menu() noexcept override;
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
        main_menu() noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        main_menu(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        main_menu(main_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        main_menu &operator=(const main_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        main_menu &operator=(main_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~main_menu() noexcept override;
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

    /// @brief 关卡图界面状态类。
    class level_graph_screen : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        level_graph_screen() noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        level_graph_screen(const level_graph_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        level_graph_screen(level_graph_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        level_graph_screen &operator=(const level_graph_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        level_graph_screen &operator=(level_graph_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~level_graph_screen() noexcept override;
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
        /**
         * @brief 响应关卡完成的函数。
         * @param [in] event 事件组件。
         */
        void           on_level_finished(level_finished_event event) noexcept;
        /// @brief 连接调度器。
        void           connect_dispatcher() noexcept;
        /// @brief 断开连接调度器。
        void           disconnect_dispatcher() noexcept;
        bool           m_is_paused = false;                ///< 是否暂停。
        entt::entity   m_current_level_entity{entt::null}; ///< 当前在玩的关卡对应的实体。
        entt::registry m_registry;                         ///< 注册表。
    };

    /// @brief 游戏界面状态类。
    class game_screen : public thr::ecs::game_state_base {
      public:
        /**
         * @brief 构建一个 game_screen 对象。
         * @param [in] level_name 关卡名称。
         */
        explicit game_screen(std::string_view level_name) noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        game_screen(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        game_screen(game_screen &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        game_screen &operator=(const game_screen &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        game_screen &operator=(game_screen &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~game_screen() noexcept override;
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
        void                                               connect_dispatcher() noexcept;
        /// @brief 断开连接调度器。
        void                                               disconnect_dispatcher() noexcept;
        bool                                               m_is_paused = false;         ///< 是否暂停。
        std::optional<thr::ecs::clock::time_point>         m_winned_time;               ///< 胜利时间。
        std::optional<thr::ecs::lua_bindings::lua_manager> m_lua_manager;               ///< Lua 管理器。
        entt::registry                                     m_registry;                  ///< 注册表。
        entt::entity                                       m_player_entity{entt::null}; ///< 玩家实体。
    };

    /// @brief 暂停界面状态类。
    class pause_menu : public thr::ecs::game_state_base {
      public:
        /// @copydoc game_state_base::game_state_base()
        pause_menu() noexcept;
        /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
        pause_menu(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
        pause_menu(pause_menu &&rhs) noexcept = default;
        /// @copydoc game_state_base::operator=(const game_state_base &rhs)
        pause_menu &operator=(const pause_menu &rhs) noexcept = delete;
        /// @copydoc game_state_base::operator=(game_state_base &&rhs)
        pause_menu &operator=(pause_menu &&rhs) noexcept = delete;
        /// @copydoc game_state_base::~game_state_base
        ~pause_menu() noexcept override;
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