#include "player_controller.h"

player_controller::player_controller() {
	m_move_command = 'N';
	m_old_move_command = 'N';
	m_controls_locked = false;
}

player_controller::~player_controller() {

}

void player_controller::on_update() {
	m_old_move_command = m_move_command;
	m_move_command = tank_commands::stop;
	if (!m_controls_locked) {
		if (engine::input::key_pressed(engine::key_codes::KEY_UP)) {
			m_move_command = tank_commands::move_up;
		}
		else if (engine::input::key_pressed(engine::key_codes::KEY_DOWN)) {
			m_move_command = tank_commands::move_down;
		}
		else if (engine::input::key_pressed(engine::key_codes::KEY_RIGHT)) {
			m_move_command = tank_commands::move_right;
		}
		else if (engine::input::key_pressed(engine::key_codes::KEY_LEFT)) {
			m_move_command = tank_commands::move_left;
		}
	}
}

bool player_controller::new_move_command() {
	if (m_old_move_command == m_move_command) {
		return false;
	}
	else {
		return true;
	}
}

engine::ref<player_controller> player_controller::create() {
	return std::make_shared<player_controller>();
}
