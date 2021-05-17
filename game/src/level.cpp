#include "level.h"
#include "engine/events/key_event.h"

level::level(engine::ref<network> network_ref) {
	// Initialize class variables
	m_player_id = 0;
	m_new_object_id = 0;
	m_network_ref = network_ref;
	m_player_controller = player_controller::create();
	m_is_active = false;

	// Create level terrain
	m_terrain = terrain::create("assets/textures/gametiles.png");
	m_level_1_terrain_sequence = {
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

	m_terrain->build_layout(28, 16, m_level_1_terrain_sequence);

}

level::~level() {

}

// Spawn tanks on the playing field
void level::initialize_tanks() {
	m_is_active = true;
	std::vector<float>terrain_boundaries = m_terrain->get_terrain_boundaries();
	float up_boundary = terrain_boundaries[0];
	float down_boundary = terrain_boundaries[1];
	float right_boundary = terrain_boundaries[2];
	float left_boundary = terrain_boundaries[3];
	// If single player
	if (!m_network_ref->is_active()) {
		engine::ref<tank> tank = tank::create(0);
		tank->set_level_boundaries(up_boundary, down_boundary, right_boundary, left_boundary);
		m_tanks.push_back(tank);
		m_new_object_id += 1;
	}
	// If multi player
	else {
		int players = m_network_ref->get_number_of_players();
		m_player_id = m_network_ref->get_user_id();
		for (int i = 0; i < players; i++) {
			engine::ref<tank> tank = tank::create(i);
			tank->set_position((-80 * i), (-80 * i));
			tank->set_level_boundaries(up_boundary, down_boundary, right_boundary, left_boundary);
			m_tanks.push_back(tank);
			m_new_object_id += 1;
		}
	}
}

// Check if all clients are ready to start the level
void level::synchronization_check() {
	if (m_network_ref->all_players_ready()) {
		initialize_tanks();
	}
}
// Call each frame
void level::on_update(const engine::timestep& time_step) {
	// Update tank states from network messages
	if (m_network_ref->is_active()) {
		std::vector<network_message::object_states> network_tank_states = m_network_ref->get_received_tank_states();
		for (int i = 0; i < network_tank_states.size(); i++) {
			for (int j = 0; j < m_tanks.size(); j++) {
				if (network_tank_states[i].object_id == m_tanks[j]->get_id()) {
					m_tanks[j]->set_position(network_tank_states[i].x_coords, network_tank_states[i].y_coords);
					m_tanks[j]->change_movement_direction(network_tank_states[i].movement_command);
					if (network_tank_states[i].connected == false) {
						m_tanks[j]->set_active(false);
					}
					break;
				}
			}
		}
	}
	else {
		// This can only happen if the server disconnected or client's connection timed out.
		if (m_player_id != 0) {
			quit_to_menu();
		}
	}
	if (m_is_active) {
		// Update player's commands
		m_player_controller->on_update();
		// If player pressed a different key, update player's tank behaviour
		if (m_player_controller->new_move_command()) {
			m_tanks[m_player_id]->change_movement_direction(m_player_controller->get_move_command());
		}

		if (m_network_ref->is_active()) {

			//Send states of all objects if they changed their behaviour or if a client disconnected from the game.
			if (m_network_ref->is_hosting()) {
				for (int i = 0; i < m_tanks.size(); i++) {
					if (m_tanks[i]->was_move_command_updated() || m_network_ref->player_in_game_disconnected()) {
						glm::vec2 tank_position = m_tanks[i]->get_position();
						network_message::object_states tank_state =
						{ m_tanks[i]->get_id(),tank_position.x,tank_position.y,m_tanks[i]->get_move_command(), false, true };
						m_network_ref->send_tank_state(tank_state);
					}
				}
				// Prevent the disconnected client from affecting the game.
				if (m_network_ref->player_in_game_disconnected()) {
					for (int i = 0; i < m_tanks.size(); i++) {
						if (i < m_network_ref->get_max_players()){
							if (m_network_ref->get_player_name(i) == "<Empty>") {
								m_tanks[i]->set_active(false);
							}
						}
						else {
							break;
						}
					}
				}
				// Confirm to the network class that all object states have been sent.
				m_network_ref->object_states_sent();
			}
			// If client, send only the state of own tank if its state was updated.
			else {
				if (m_tanks[m_player_id]->was_move_command_updated()) {
					glm::vec2 tank_position = m_tanks[m_player_id]->get_position();
					network_message::object_states tank_state =
					{ m_player_id,tank_position.x,tank_position.y,m_tanks[m_player_id]->get_move_command(), false, true };
					m_network_ref->send_tank_state(tank_state);
				}
			}
		}

		// Update the position of all tanks on the field
		for (int i = 0; i < m_tanks.size(); i++) {
			m_tanks[i]->on_update(time_step);
		}
	}
	else {
		// Whenever a new level loads: check if all clients are ready to begin
		if (m_network_ref->is_active()) {
			synchronization_check();
		}
	}
}

// Render objects in level
void level::on_render(engine::ref<engine::shader> shader) {
	m_terrain->on_render(shader);
	for (int i = 0; i < m_tanks.size(); i++) {
		m_tanks[i]->on_render(shader);
	}
}

// Reset class variables and prepare to switch to menu.
void level::quit_to_menu() {
	m_network_ref->leave_server();
	m_player_id = 0;
	m_is_active = false;
	m_tanks.clear();
	m_in_menu = true;
}

void level::on_event(engine::event& event) {

	auto& e = dynamic_cast<engine::key_pressed_event&>(event);
	if (e.key_code() == engine::key_codes::KEY_ESCAPE)
	{
		quit_to_menu();
	}

}

// Return pointer to class
engine::ref<level> level::create(engine::ref<network> network_ref)
{
	return std::make_shared<level>(network_ref);
}
