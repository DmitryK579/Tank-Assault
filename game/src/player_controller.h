#pragma once
#include <engine.h>
#include "tank_commands.h"

class player_controller {
public:
	player_controller();
	~player_controller();

	void on_update();
	bool new_command();

	char get_command() { return m_command; }

	static engine::ref<player_controller> create();
private:
	char m_command;
	char m_old_command;
};
