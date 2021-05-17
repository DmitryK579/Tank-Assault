#pragma once
#include <engine.h>
#include "tank_commands.h"

class player_controller {
public:
	player_controller();
	~player_controller();

	void on_update();
	bool new_move_command();

	char get_move_command() { return m_move_command; }

	static engine::ref<player_controller> create();
private:
	char m_move_command;
	char m_old_move_command;
	bool m_controls_locked;
};
