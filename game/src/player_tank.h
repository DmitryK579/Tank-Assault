#pragma once
#include <engine.h>
#include "sprite.h"

class player_tank {
public:
	player_tank();
	~player_tank();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void on_event(engine::event& event);

	static engine::ref<player_tank> create();
private:
	engine::ref<sprite> m_chassis_sprite;
	engine::ref<sprite> m_turret_sprite;

	glm::vec2 m_position;
	glm::vec2 m_old_position;
	glm::vec2 m_velocity;
	float m_speed;
	float m_angle;
	bool m_is_moving;
};
