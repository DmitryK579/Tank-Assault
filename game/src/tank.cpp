#include "tank.h"
#include "tank_commands.h"

tank::tank(int id) {

	// Initialize tank characteristics
	m_id = id;
	m_hit_points = 3;
	m_position = glm::vec2(.0f, .0f);
	m_velocity = glm::vec2(.0f, 1.0f);
	m_speed = 120.0f;
	m_angle = 0.0f;
	stop();

	// Initialize tank sprites and animation
	if (id > 4) {
		id = 4;
	}
	m_chassis_sprite = sprite::create("assets/textures/lightvehicles.png", 26, 26);
	for (int i = 0; i < 4; i++) {
		m_chassis_sprite->add_frame_quad(26, 26, i, id);
	}
	m_chassis_sprite->set_frame_time(0.05f);
	m_chassis_sprite->set_animating(true);

	m_turret_sprite = sprite::create("assets/textures/turrets.png", 42, 42);
	m_turret_sprite->add_frame_quad(42, 42, 2, id);

}

tank::~tank() {

}

//Call every frame
void tank::on_update(const engine::timestep& time_step) {
	bool moving = false;
	
	if (m_is_moving_up) {
		m_velocity.y = 1;
		m_velocity.x = 0;
		moving = true;
		m_angle = glm::radians(0.0f);
	}
	else if (m_is_moving_down) {
		m_velocity.y = -1;
		m_velocity.x = 0;
		moving = true;
		m_angle = glm::radians(-180.0f);
	}
	else if (m_is_moving_right) {
		m_velocity.x = 1;
		m_velocity.y = 0;
		moving = true;
		m_angle = glm::radians(-90.0f);
	}
	else if (m_is_moving_left) {
		m_velocity.x = -1;
		m_velocity.y = 0;
		moving = true;
		m_angle = glm::radians(-270.0f);
	}
	m_old_position = m_position;
	if (moving) {
		m_position = m_position + (m_velocity * m_speed * (float)time_step);
	}

	m_chassis_sprite->set_animating(moving);
	m_chassis_sprite->on_update(time_step);
	m_turret_sprite->on_update(time_step);

}
// Call to render the tank.
void tank::on_render(engine::ref<engine::shader> shader) {
	glm::mat4 chassis_transform(1.0f);
	chassis_transform = glm::translate(chassis_transform, glm::vec3(m_position.x, m_position.y, (0.0f+(float)m_id)));
	chassis_transform = glm::rotate(chassis_transform, m_angle, glm::vec3(0, 0, 1));
	m_chassis_sprite->on_render(chassis_transform, shader);

	glm::mat4 turret_transform(1.0f);
	glm::vec3 turret_offset = glm::vec3(10.0f * m_velocity.x, 10.0f * m_velocity.y, (0.2f+(float)m_id));
	turret_transform = glm::translate(turret_transform, glm::vec3(m_position.x + turret_offset.x, m_position.y + turret_offset.y, turret_offset.z));
	turret_transform = glm::rotate(turret_transform, m_angle, glm::vec3(0, 0, 1));
	m_turret_sprite->on_render(turret_transform, shader);
}

void tank::change_movement_direction(char command) {
	if (command == tank_commands::stop) {
		stop();
	}
	else if (command == tank_commands::move_up) {
		m_is_moving_up = true;
		m_is_moving_down = false;
		m_is_moving_right = false;
		m_is_moving_left = false;
	}
	else if (command == tank_commands::move_down) {
		m_is_moving_up = false;
		m_is_moving_down = true;
		m_is_moving_right = false;
		m_is_moving_left = false;
	}
	else if (command == tank_commands::move_right) {
		m_is_moving_up = false;
		m_is_moving_down = false;
		m_is_moving_right = true;
		m_is_moving_left = false;
	}
	else if (command == tank_commands::move_left) {
		m_is_moving_up = false;
		m_is_moving_down = false;
		m_is_moving_right = false;
		m_is_moving_left = true;
	}
}

void tank::stop() {
	m_is_moving_up = false;
	m_is_moving_down = false;
	m_is_moving_right = false;
	m_is_moving_left = false;
}
// Create pointer to class
engine::ref<tank> tank::create(int id)
{
	return std::make_shared<tank>(id);
}
