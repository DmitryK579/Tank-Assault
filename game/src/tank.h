#pragma once
#include <engine.h>
#include "sprite.h"

class tank {
public:
	tank(int player_id);
	~tank();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void change_movement_direction(char command);

	static engine::ref<tank> create(int id);

private:
	void stop();

	engine::ref<sprite> m_chassis_sprite;
	engine::ref<sprite> m_turret_sprite;

	glm::vec2 m_position;
	glm::vec2 m_old_position;
	glm::vec2 m_velocity;
	int m_id;
	float m_speed;
	float m_angle;
	int m_hit_points;
	bool m_is_moving_up;
	bool m_is_moving_down;
	bool m_is_moving_left;
	bool m_is_moving_right;
};
