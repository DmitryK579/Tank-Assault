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

	bool was_move_command_updated();

	void set_position(float x, float y) { m_position = glm::vec2(x, y); }
	void set_level_boundaries(float up, float down, float right, float left);
	void set_active(bool state) { m_active = state; }
	glm::vec2 get_position() { return m_position; }
	int get_id() { return m_id; }
	char get_move_command() { return m_current_move_command; }

	static engine::ref<tank> create(int id);

private:
	void stop();

	engine::ref<sprite> m_chassis_sprite;
	engine::ref<sprite> m_turret_sprite;

	glm::vec2 m_position;
	glm::vec2 m_old_position;
	glm::vec2 m_velocity;
	char m_current_move_command;
	char m_old_move_command;
	int m_id;
	float m_speed;
	float m_angle;
	float m_up_boundary;
	float m_down_boundary;
	float m_left_boundary;
	float m_right_boundary;
	bool m_active;
	bool m_is_moving_up;
	bool m_is_moving_down;
	bool m_is_moving_left;
	bool m_is_moving_right;
};
