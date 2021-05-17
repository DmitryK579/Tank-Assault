#pragma once
#include <engine.h>
#include "sprite.h"
#include "network.h"

class main_menu {
public:
	main_menu(engine::ref<network> network_ref);
	~main_menu();


	void confirm_selection();
	void switch_menu(int menu_choice);
	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> image_shader, engine::ref<engine::shader> text_shader);
	void on_event(engine::event& event);

	void set_in_menu(bool state) { m_in_menu = state; }
	bool get_in_menu() { return m_in_menu; }

	static engine::ref<main_menu> create(engine::ref<network> network_ref);

private:
	void apply_pressed_key(std::string& target, std::string pressed_key, bool erase_character,int character_limit);

	engine::ref<engine::text_manager>				m_text_manager{};
	engine::ref<sprite>								m_selection_arrow{};
	engine::ref<sprite>								m_sprite{};
	std::vector<engine::ref<sprite>>				m_menu_sprites{};
	std::map<int32_t,std::string>					m_valid_name_keys{};
	std::map<int32_t, std::string>					m_valid_ip_keys{};
	std::map<int32_t, std::string>					m_valid_port_keys{};

	bool											m_in_menu;
	bool											m_entering_text;
	bool											m_lock_controls;
	bool											m_is_hostring_server;
	bool											m_is_joining_server;

	int												m_menu_state;
	int												m_current_menu_choice;
	std::vector<std::vector<glm::vec2>>				m_selection_arrow_positions;
	std::vector<int>								m_menu_choices;
	glm::vec3										m_text_colour_normal;
	glm::vec3										m_text_colour_entering;

	const int										state_title_screen = 0;
	const int										state_multiplayer_menu = 1;
	const int										state_multiplayer_join_menu = 2;
	const int										state_multiplayer_connection_status = 3;
	const int										state_multiplayer_lobby_host = 4;
	const int										state_multiplayer_lobby_client = 5;

	std::string										m_player_name;
	std::string										m_ip_address;
	std::string										m_port;

	engine::ref<network>							m_network_ref{};
};
