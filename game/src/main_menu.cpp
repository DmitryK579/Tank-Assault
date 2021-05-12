#include "main_menu.h"
#include "engine/events/key_event.h"

main_menu::main_menu(engine::ref<network> network_ref) {
	m_network_ref = network_ref;

	// Create text manager
	m_text_manager = engine::text_manager::create();

	// Initialize menu variables
	m_player_name = "PLAYER";
	m_ip_address = "127.0.0.1";
	m_port = "5020";
	m_text_colour_normal = glm::vec3(1.0f, 1.0f, 1.0f);
	m_text_colour_entering = glm::vec3(0.5f, 0.f, 1.0f);
	m_in_menu = true;
	m_entering_text = false;
	m_is_hostring_server = false;
	m_is_joining_server = false;
	m_menu_state = state_title_screen;
	m_current_menu_choice = 0;

	// Set keys that can be entered into fields in the menu
	m_valid_name_keys = {
		{engine::key_codes::KEY_0, "0"},
		{engine::key_codes::KEY_1, "1"},
		{engine::key_codes::KEY_2, "2"},
		{engine::key_codes::KEY_3, "3"},
		{engine::key_codes::KEY_4, "4"},
		{engine::key_codes::KEY_5, "5"},
		{engine::key_codes::KEY_6, "6"},
		{engine::key_codes::KEY_7, "7"},
		{engine::key_codes::KEY_8, "8"},
		{engine::key_codes::KEY_9, "9"},
		{engine::key_codes::KEY_PERIOD, "."},
		{engine::key_codes::KEY_A, "A"},
		{engine::key_codes::KEY_B, "B"},
		{engine::key_codes::KEY_C, "C"},
		{engine::key_codes::KEY_D, "D"},
		{engine::key_codes::KEY_E, "E"},
		{engine::key_codes::KEY_F, "F"},
		{engine::key_codes::KEY_G, "G"},
		{engine::key_codes::KEY_H, "H"},
		{engine::key_codes::KEY_I, "I"},
		{engine::key_codes::KEY_J, "J"},
		{engine::key_codes::KEY_K, "K"},
		{engine::key_codes::KEY_L, "L"},
		{engine::key_codes::KEY_M, "M"},
		{engine::key_codes::KEY_N, "N"},
		{engine::key_codes::KEY_O, "O"},
		{engine::key_codes::KEY_P, "P"},
		{engine::key_codes::KEY_Q, "Q"},
		{engine::key_codes::KEY_R, "R"},
		{engine::key_codes::KEY_S, "S"},
		{engine::key_codes::KEY_T, "T"},
		{engine::key_codes::KEY_U, "U"},
		{engine::key_codes::KEY_V, "V"},
		{engine::key_codes::KEY_W, "W"},
		{engine::key_codes::KEY_X, "X"},
		{engine::key_codes::KEY_Y, "Y"},
		{engine::key_codes::KEY_Z, "Z"},
	};

	m_valid_ip_keys = {
		{engine::key_codes::KEY_0, "0"},
		{engine::key_codes::KEY_1, "1"},
		{engine::key_codes::KEY_2, "2"},
		{engine::key_codes::KEY_3, "3"},
		{engine::key_codes::KEY_4, "4"},
		{engine::key_codes::KEY_5, "5"},
		{engine::key_codes::KEY_6, "6"},
		{engine::key_codes::KEY_7, "7"},
		{engine::key_codes::KEY_8, "8"},
		{engine::key_codes::KEY_9, "9"},
		{engine::key_codes::KEY_PERIOD, "."}
	};

	m_valid_port_keys = {
		{engine::key_codes::KEY_0, "0"},
		{engine::key_codes::KEY_1, "1"},
		{engine::key_codes::KEY_2, "2"},
		{engine::key_codes::KEY_3, "3"},
		{engine::key_codes::KEY_4, "4"},
		{engine::key_codes::KEY_5, "5"},
		{engine::key_codes::KEY_6, "6"},
		{engine::key_codes::KEY_7, "7"},
		{engine::key_codes::KEY_8, "8"},
		{engine::key_codes::KEY_9, "9"},
	};

	// Create menu sprites and positions

	m_selection_arrow = sprite::create("assets/textures/arrow.png", 40, 40);
	engine::ref<sprite> main_menu = sprite::create("assets/textures/MainMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> options_menu = sprite::create("assets/textures/OptionsMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> controls_menu = sprite::create("assets/textures/ControlsMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> volume_menu = sprite::create("assets/textures/VolumeMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> multiplayer_menu = sprite::create("assets/textures/MultiplayerMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> multiplayer_join_menu = sprite::create("assets/textures/MultiplayerJoinMenu.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> multiplayer_connection_status_menu = sprite::create("assets/textures/MultiplayerConnectionStatus.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> multiplayer_lobby_host_menu = sprite::create("assets/textures/MultiplayerLobbyHost.png", (float)engine::application::window().width(), (float)engine::application::window().height());
	engine::ref<sprite> multiplayer_lobby_client_menu = sprite::create("assets/textures/MultiplayerLobbyClient.png", (float)engine::application::window().width(), (float)engine::application::window().height());

	m_menu_sprites.push_back(main_menu);
	m_menu_sprites.push_back(options_menu);
	m_menu_sprites.push_back(controls_menu);
	m_menu_sprites.push_back(volume_menu);
	m_menu_sprites.push_back(multiplayer_menu);
	m_menu_sprites.push_back(multiplayer_join_menu);
	m_menu_sprites.push_back(multiplayer_connection_status_menu);
	m_menu_sprites.push_back(multiplayer_lobby_host_menu);
	m_menu_sprites.push_back(multiplayer_lobby_client_menu);

	int menu_states = m_menu_sprites.size();

	for (int i = 0; i < menu_states; i++) {
		m_menu_choices.push_back(0);
	}
	m_menu_choices[state_title_screen] = 4;
	m_menu_choices[state_options_menu] = 3;
	m_menu_choices[state_controls_menu] = 2;
	m_menu_choices[state_volume_menu] = 3;
	m_menu_choices[state_multiplayer_menu] = 4;
	m_menu_choices[state_multiplayer_join_menu] = 4;
	m_menu_choices[state_multiplayer_connection_status] = 1;
	m_menu_choices[state_multiplayer_lobby_host] = 2;
	m_menu_choices[state_multiplayer_lobby_client] = 1;

	for (int i = 0; i < menu_states; i++) {
		std::vector<glm::vec2> position_vector;
		for (int j = 0; j < m_menu_choices[i]; j++) {
			float x = -330;
			float y = 13 - (178 * j);
			position_vector.push_back(glm::vec2(x, y));
		}
		m_selection_arrow_positions.push_back(position_vector);
	}
	m_selection_arrow_positions[state_multiplayer_lobby_host][0].y -= 178 * 2;
	m_selection_arrow_positions[state_multiplayer_lobby_host][1].y -= 178 * 2;
	m_selection_arrow_positions[state_multiplayer_lobby_client][0].y -= 178 * 3;
}

main_menu::~main_menu() {

}

//Call each frame
bool main_menu::on_update(const engine::timestep& time_step) {
	if (m_menu_state == state_multiplayer_lobby_host || m_menu_state == state_multiplayer_lobby_client) {
		if (m_network_ref->get_is_active() == false) {
			switch_menu(state_multiplayer_menu);
		}
	}
	return m_in_menu;
}

//Call during a key press event
void main_menu::on_event(engine::event& event) {
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (!m_entering_text) {
			// Change selected option in menu
			if (e.key_code() == engine::key_codes::KEY_UP) {
				m_current_menu_choice -= 1;
			}
			if (e.key_code() == engine::key_codes::KEY_DOWN) {
				m_current_menu_choice += 1;
			}

			// Wrap selection if it is out of bounds
			if (m_current_menu_choice < 0) {
				m_current_menu_choice = m_menu_choices[m_menu_state] - 1;
			}
			else if (m_current_menu_choice > m_menu_choices[m_menu_state] - 1) {
				m_current_menu_choice = 0;
			}

			// Move to a different menu or allow user to enter text
			if (e.key_code() == engine::key_codes::KEY_ENTER) {
				confirm_selection();
			}
		}
		else {
			std::string pressed_key = "";
			bool erase_character = false;
			if (e.key_code() != engine::key_codes::KEY_ENTER) {
				if (e.key_code() != engine::key_codes::KEY_BACKSPACE) {
					// Entering name
					if (m_menu_state == state_multiplayer_menu && m_current_menu_choice == 0) {
						if (m_valid_name_keys.find(e.key_code()) != m_valid_name_keys.end()) {
							pressed_key = m_valid_name_keys[e.key_code()];
						}
					}
					// Entering IP
					else if (m_menu_state == state_multiplayer_join_menu && m_current_menu_choice == 0) {
						if (m_valid_ip_keys.find(e.key_code()) != m_valid_ip_keys.end()) {
							pressed_key = m_valid_ip_keys[e.key_code()];
						}
					}
					// Entering port
					else if (m_menu_state == state_multiplayer_join_menu && m_current_menu_choice == 1) {
						if (m_valid_port_keys.find(e.key_code()) != m_valid_port_keys.end()) {
							pressed_key = m_valid_port_keys[e.key_code()];
						}
					}
				}
				else {
					erase_character = true;
				}
			}
			else {
				m_entering_text = false;
			}
			// Add pressed key to name
			if (m_menu_state == state_multiplayer_menu && m_current_menu_choice == 0) {
				apply_pressed_key(m_player_name, pressed_key, erase_character,20);
			}
			// Add pressed key to IP
			else if (m_menu_state == state_multiplayer_join_menu && m_current_menu_choice == 0) {
				apply_pressed_key(m_ip_address, pressed_key, erase_character,20);
			}
			// Add pressed key to port
			else if (m_menu_state == state_multiplayer_join_menu && m_current_menu_choice == 1) {
				apply_pressed_key(m_port, pressed_key, erase_character,5);
			}
		}
	}
	
}
//Add one of the valid keys to a target string.
void main_menu::apply_pressed_key(std::string& target, std::string pressed_key, bool erase_character, int character_limit) {
	if (erase_character == false) {
		if (target.size() < character_limit) {
			target += pressed_key;
		}
	}
	else {
		if (target.size() != 0) {
			target.erase(target.size() - 1);
		}
	}
}
//Handle menu state switching upon pressing the enter key
void main_menu::confirm_selection() {
	if (m_menu_state == state_title_screen) {
		// Start single player game
		if (m_current_menu_choice == 0) {
			m_in_menu = false;
		}
		// Move to multi player menu
		if (m_current_menu_choice == 1) {
			switch_menu(state_multiplayer_menu);
		}
		// Move to options menu
		if (m_current_menu_choice == 2) {
			switch_menu(state_options_menu);
		}
		// Exit the application
		if (m_current_menu_choice == 3) {
			engine::application::exit();
		}
	}
	// Options menu currently not functional
	else if (m_menu_state == state_options_menu) {
		// Change controls
		if (m_current_menu_choice == 0) {
			switch_menu(state_controls_menu);
		}
		// Change volume
		if (m_current_menu_choice == 1) {
			switch_menu(state_volume_menu);
		}
		// Back to title screen
		if (m_current_menu_choice == 2) {
			switch_menu(state_title_screen);
		}
	}
	else if (m_menu_state == state_controls_menu) {
		// Change keys
		if (m_current_menu_choice == 0) {

		}
		// Back to options menu
		if (m_current_menu_choice == 1) {
			switch_menu(state_options_menu);
		}
	}
	else if (m_menu_state == state_volume_menu) {
		// Change music volume
		if (m_current_menu_choice == 0) {

		}
		// Change sound volume
		if (m_current_menu_choice == 1) {

		}
		// Back to options menu
		if (m_current_menu_choice == 2) {
			switch_menu(state_options_menu);
		}
	}
	else if (m_menu_state == state_multiplayer_menu) {
		// Enter player name
		if (m_current_menu_choice == 0) {
			m_entering_text = true;
		}
		// Host new lobby
		if (m_current_menu_choice == 1) {
			switch_menu(state_multiplayer_lobby_host);
			m_network_ref->create_server(m_player_name);
		}
		// Join an existing lobby
		if (m_current_menu_choice == 2) {
			switch_menu(state_multiplayer_join_menu);
		}
		// Back to title screen
		if (m_current_menu_choice == 3) {
			switch_menu(state_title_screen);
		}
	}
	else if (m_menu_state == state_multiplayer_join_menu) {
		// Enter IP
		if (m_current_menu_choice == 0) {
			m_entering_text = true;
		}
		// Enter port
		if (m_current_menu_choice == 1) {
			m_entering_text = true;
		}
		// Connect to a server
		if (m_current_menu_choice == 2) {
			sf::IpAddress ip = m_ip_address;
			unsigned short port = static_cast<unsigned short>(std::stoul(m_port));
			m_network_ref->join_server(ip,port,m_player_name);
			switch_menu(state_multiplayer_lobby_client);
		}
		// Back to multiplayer menu
		if (m_current_menu_choice == 3) {
			switch_menu(state_multiplayer_menu);
		}
	}
	else if (m_menu_state == state_multiplayer_lobby_host) {
		// Begin multiplayer game
		if (m_current_menu_choice == 0) {

		}
		// Back to multiplayer menu
		if (m_current_menu_choice == 1) {
			switch_menu(state_multiplayer_menu);
			m_network_ref->leave_server();
		}
	}
	else if (m_menu_state == state_multiplayer_lobby_client) {
		// Back to multiplayer menu
		if (m_current_menu_choice == 0) {
			switch_menu(state_multiplayer_menu);
			m_network_ref->leave_server();
		}
	}
}
// Switch menu state and reset the position of selection cursor
void main_menu::switch_menu(int menu_choice) {
	m_menu_state = menu_choice;
	m_current_menu_choice = 0;
}

// Call to render menu images and text
void main_menu::on_render(engine::ref<engine::shader> image_shader, engine::ref<engine::shader> text_shader) {

	glm::mat4 menu_transform(1.0f);
	menu_transform = glm::translate(menu_transform, glm::vec3(0, 0, -0.2f));

	glm::mat4 selection_cursor_transform(1.0f);
	selection_cursor_transform = glm::translate(selection_cursor_transform, glm::vec3(m_selection_arrow_positions[m_menu_state][m_current_menu_choice].x, m_selection_arrow_positions[m_menu_state][m_current_menu_choice].y, 0.0f));

	m_menu_sprites[m_menu_state]->on_render(menu_transform, image_shader);
	m_selection_arrow->on_render(selection_cursor_transform, image_shader);

	// Render player name in multiplayer menu
	if (m_menu_state == state_multiplayer_menu) {
		glm::vec3 colour = m_text_colour_normal;
		if (m_entering_text) {
			colour = m_text_colour_entering;
		}
		m_text_manager->render_text(text_shader, m_player_name, (float)engine::application::window().width()/2 + 50.f,
			(float)engine::application::window().height()/2 -5.f, 0.75f, glm::vec4(1.f, colour.x,colour.y,colour.z));
	}

	// Render IP address and port in multiplayer join menu
	else if (m_menu_state == state_multiplayer_join_menu) {
		glm::vec3 ip_colour = m_text_colour_normal;
		glm::vec3 port_colour = m_text_colour_normal;
		if (m_entering_text) {
			// Entering IP address
			if (m_current_menu_choice == 0) {
				ip_colour = m_text_colour_entering;
			}
			// Entering port
			else if (m_current_menu_choice == 1) {
				port_colour = m_text_colour_entering;
			}
		}
		m_text_manager->render_text(text_shader, m_ip_address, (float)engine::application::window().width() / 2 + 100.f,
			(float)engine::application::window().height() / 2 - 5.f, 0.75f, glm::vec4(1.f, ip_colour.x, ip_colour.y, ip_colour.z));
		m_text_manager->render_text(text_shader, m_port, (float)engine::application::window().width() / 2 + 100.f,
			(float)engine::application::window().height() / 2 - 95.f, 0.75f, glm::vec4(1.f, port_colour.x, port_colour.y, port_colour.z));

	}
	// Render player names and IP addresses in multiplayer lobby
	else if (m_menu_state == state_multiplayer_lobby_host || m_menu_state == state_multiplayer_lobby_client) {
		glm::vec3 colour = m_text_colour_normal;

		m_text_manager->render_text(text_shader, m_network_ref->get_player_name(0), (float)engine::application::window().width() / 2 - 230.f,
			(float)engine::application::window().height() / 2 + 115.f, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));

		m_text_manager->render_text(text_shader, m_network_ref->get_player_name(1), (float)engine::application::window().width() / 2 - 230.f,
			(float)engine::application::window().height() / 2 + 60.f, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));

		m_text_manager->render_text(text_shader, m_network_ref->get_player_name(2), (float)engine::application::window().width() / 2 - 230.f,
			(float)engine::application::window().height() / 2 + 5.f, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));

		m_text_manager->render_text(text_shader, m_network_ref->get_player_name(3), (float)engine::application::window().width() / 2 - 230.f,
			(float)engine::application::window().height() / 2 - 50.f, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));

		m_text_manager->render_text(text_shader, "Public IP: " + m_network_ref->get_public_ip(), (float)engine::application::window().width() / 40,
			(float)engine::application::window().height() / 10, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));

		m_text_manager->render_text(text_shader, "Local IP : " + m_network_ref->get_local_ip(), (float)engine::application::window().width() / 40,
			(float)engine::application::window().height() / 20, 0.75f, glm::vec4(1.f, colour.x, colour.y, colour.z));
	}
}

// Create pointer to class
engine::ref<main_menu> main_menu::create(engine::ref<network> network_ref) {
	return std::make_shared<main_menu>(network_ref);
}
