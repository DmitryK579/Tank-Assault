#pragma once
#include <engine.h>
#include "terrain.h"
#include "tank.h"
#include "network.h"
#include "player_controller.h"

class level {
public:
	level(engine::ref<network> network_ref);
	~level();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void on_event(engine::event& event);

	void initialize_tanks();
	void synchronization_check();

	bool get_in_menu() { return m_in_menu; }
	void set_in_menu(bool state) { m_in_menu = state; }

	static engine::ref<level> create(engine::ref<network> network_ref);

private:

	void quit_to_menu();

	int m_player_id;
	int m_new_object_id;
	bool m_is_active;
	bool m_in_menu;

	engine::ref<terrain> m_terrain;
	std::vector<std::pair<int,int>> m_level_1_terrain_sequence;

	std::vector<engine::ref<tank>> m_tanks{};

	engine::ref<network> m_network_ref{};
	engine::ref<player_controller> m_player_controller{};
};
