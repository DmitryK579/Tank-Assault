#pragma once
#include <engine.h>
#include "terrain.h"
#include "player_tank.h"

class level {
public:
	level();
	~level();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void on_event(engine::event& event);

	static engine::ref<level> create();

private:
	engine::ref<terrain> m_terrain;
	std::vector<std::pair<int,int>> m_level_1_terrain_sequence;

	engine::ref<player_tank> m_player_tank;
};
