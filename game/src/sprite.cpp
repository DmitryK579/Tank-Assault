#include "pch.h"
#include "sprite.h"
#include "quad.h"

sprite::sprite(const std::string& path, float width, float height, int frames, float frame_time)
{
	m_texture = engine::texture_2d::create(path, true);
	m_transparency = 1.0f;
	m_scale = glm::vec2(1.f, 1.f);
	m_current_frame = 1;
	m_total_frames = frames;
	m_timer = 0.f;
	m_frame_time = frame_time;
	if (frame_time > 0) {
		m_is_animating = true;
	}
	else {
		m_is_animating = false;
	}

	float image_step = 1.0f / (float)frames;
	for (int i = 0; i < frames; i++) {
		engine::ref<quad> quad = quad::create(glm::vec2(width, height), glm::vec2(i * image_step, i * image_step + image_step), glm::vec2(0.f, 1.f));
		m_quads.push_back(quad);
	}
}

sprite::~sprite()
{}

void sprite::on_update(const engine::timestep& time_step)
{
	if (m_is_animating == true) {
		m_timer += (float)time_step;
	}

	if (m_timer > m_frame_time) {
		m_timer -= m_frame_time;
		m_current_frame += 1;
		if (m_current_frame > m_total_frames) {
			m_current_frame -= m_total_frames;
		}
	}
}

void sprite::on_render(glm::mat4 transform, engine::ref<engine::shader> shader)
{
	transform = glm::scale(transform, glm::vec3(m_scale.x, m_scale.y, 1.f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	m_texture->bind();
	int index = m_current_frame - 1;
	engine::renderer::submit(shader, m_quads[index]->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void sprite::create_sprite_quad(int sprite_size_x, int sprite_size_y, int horizontal_sprite_square, int vertical_sprite_square) {
	float image_step_x = m_texture->width() / sprite_size_x;
	float image_step_y = m_texture->height() / sprite_size_y;

	engine::ref<quad> quad = quad::create(glm::vec2(sprite_size_x, sprite_size_y),
		glm::vec2(horizontal_sprite_square * image_step_x, horizontal_sprite_square * image_step_x + image_step_x),
		glm::vec2(vertical_sprite_square * image_step_y, vertical_sprite_square * image_step_y + image_step_y));

	m_quads.push_back(quad);

}

engine::ref<sprite> sprite::create(const std::string& path, float width, float height, int frames, float frame_time)
{
	return std::make_shared<sprite>(path, width, height, frames, frame_time);
}
