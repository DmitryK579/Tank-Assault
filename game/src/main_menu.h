#pragma once
#include <engine.h>
#include "sprite.h"

class main_menu {
public:
	main_menu();
	~main_menu();


	void confirm_selection();
	void switch_menu(int menu_choice);
	bool on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> image_shader, engine::ref<engine::shader> text_shader);
	void on_event(engine::event& event);
	static engine::ref<main_menu> create();

private:
	void apply_pressed_key(std::string& target, std::string pressed_key, bool erase_character);
	void reset_multiplayer_names();
	engine::ref<engine::text_manager>				m_text_manager{};
	engine::ref<sprite>								m_selection_arrow{};
	engine::ref<sprite>								m_sprite{};
	std::vector<engine::ref<sprite>>				m_menu_sprites{};
	std::map<int32_t,std::string>					m_valid_keys{};

	bool											m_in_menu = true;
	bool											m_entering_text = false;
	int												m_menu_state = 0;
	int												m_current_menu_choice = 0;
	std::vector<std::vector<glm::vec2>>				m_selection_arrow_positions;
	std::vector<int>								m_menu_choices;
	glm::vec3										m_text_colour_normal;
	glm::vec3										m_text_colour_entering;

	const int										state_title_screen = 0;
	const int										state_options_menu = 1;
	const int										state_controls_menu = 2;
	const int										state_volume_menu = 3;
	const int										state_multiplayer_menu = 4;
	const int										state_multiplayer_join_menu = 5;
	const int										state_multiplayer_session_menu = 6;

	std::string										m_player_name;
	std::string										m_ip_address;
	std::string										m_port;

	std::string										m_p1_name;
	std::string										m_p2_name;
	std::string										m_p3_name;
	std::string										m_p4_name;
};
