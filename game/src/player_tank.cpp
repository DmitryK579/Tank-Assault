#include "player_tank.h"
#include "engine/events/key_event.h"

player_tank::player_tank(int player_id) {

	// Initialize tank characteristics
	m_position = glm::vec2(.0f, .0f);
	m_velocity = glm::vec2(.0f, 1.0f);
	m_speed = 120.0f;
	m_angle = 0.0f;
	m_is_moving = false;

	// Initialize tank sprites and animation
	m_chassis_sprite = sprite::create("assets/textures/lightvehicles.png", 26, 26);
	for (int i = 0; i < 4; i++) {
		m_chassis_sprite->add_frame_quad(26, 26, i, player_id);
	}
	m_chassis_sprite->set_frame_time(0.05f);
	m_chassis_sprite->set_animating(true);

	m_turret_sprite = sprite::create("assets/textures/turrets.png", 42, 42);
	m_turret_sprite->add_frame_quad(42, 42, 2, player_id);

}

player_tank::~player_tank() {

}

//Call every frame
void player_tank::on_update(const engine::timestep& time_step) {
	m_is_moving = false;

	if (engine::input::key_pressed(engine::key_codes::KEY_UP)) {
		m_is_moving = true;
		m_angle = glm::radians(0.0f);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_DOWN)) {
		m_is_moving = true;
		m_angle = glm::radians(-180.0f);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_RIGHT)) {
		m_is_moving = true;
		m_angle = glm::radians(-90.0f);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_LEFT)) {
		m_is_moving = true;
		m_angle = glm::radians(-270.0f);
	}

	if (m_is_moving) {
		float x = 0 * glm::cos(m_angle) - 1 * glm::sin(m_angle);
		float y = 0 * glm::sin(m_angle) + 1 * glm::cos(m_angle);
		m_velocity = glm::vec2(x, y);

		m_old_position = m_position;
		m_position = m_position + (m_velocity * m_speed * (float)time_step);
	}

	m_chassis_sprite->set_animating(m_is_moving);
	m_chassis_sprite->on_update(time_step);
	m_turret_sprite->on_update(time_step);

}
// Call to render the tank.
void player_tank::on_render(engine::ref<engine::shader> shader) {
	glm::mat4 chassis_transform(1.0f);
	chassis_transform = glm::translate(chassis_transform, glm::vec3(m_position.x,m_position.y,.0f));
	chassis_transform = glm::rotate(chassis_transform, m_angle, glm::vec3(0, 0, 1));
	m_chassis_sprite->on_render(chassis_transform, shader);

	glm::mat4 turret_transform(1.0f);
	glm::vec3 turret_offset = glm::vec3(10.0f * m_velocity.x, 10.0f * m_velocity.y,0.2f);
	turret_transform = glm::translate(turret_transform, glm::vec3(m_position.x + turret_offset.x, m_position.y + turret_offset.y, turret_offset.z));
	turret_transform = glm::rotate(turret_transform, m_angle, glm::vec3(0, 0, 1));
	m_turret_sprite->on_render(turret_transform, shader);
}

void player_tank::on_event(engine::event& event) {
	
}

// Create pointer to class
engine::ref<player_tank> player_tank::create(int player_id)
{
	return std::make_shared<player_tank>(player_id);
}
