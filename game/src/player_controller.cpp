#include "player_controller.h"

player_controller::player_controller() {
	m_command = 'N';
	m_old_command = 'N';
}

player_controller::~player_controller() {

}

void player_controller::on_update() {
	m_old_command = m_command;
	m_command = tank_commands::stop;
	if (engine::input::key_pressed(engine::key_codes::KEY_UP)) {
		m_command = tank_commands::move_up;
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_DOWN)) {
		m_command = tank_commands::move_down;
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_RIGHT)) {
		m_command = tank_commands::move_right;
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_LEFT)) {
		m_command = tank_commands::move_left;
	}
}

bool player_controller::new_command() {
	if (m_old_command == m_command) {
		return false;
	}
	else {
		return true;
	}
}

engine::ref<player_controller> player_controller::create() {
	return std::make_shared<player_controller>();
}
