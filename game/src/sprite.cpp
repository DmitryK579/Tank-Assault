#include "pch.h"
#include "sprite.h"
#include "quad.h"

sprite::sprite(const std::string& path, float width, float height)
{
	// Initialise class variables
	m_main_image = engine::texture_2d::create(path, true);
	m_transparency = 1.0f;
	m_scale = glm::vec2(1.f, 1.f);
	m_current_frame = 1;
	// Keep total frames at 0 to render the entire image.
	m_total_frames = 0;
	m_timer = 0.f;
	m_frame_time = 0.0f;
	m_is_animating = false;

	m_main_image_quad = quad::create(glm::vec2(width, height), glm::vec2(0.f, 1.f), glm::vec2(0.f, 1.f));	
}

sprite::~sprite(){

}

//Call each frame
void sprite::on_update(const engine::timestep& time_step)
{
	// Add time passed between frames to timer
	if (m_is_animating == true) {
		m_timer += (float)time_step;
	}

	// Switch current frame based on timer
	if (m_timer > m_frame_time) {
		m_timer -= m_frame_time;
		m_current_frame += 1;
		if (m_current_frame > m_total_frames) {
			m_current_frame -= m_total_frames;
		}
	}
}

//Call to render the sprite
void sprite::on_render(glm::mat4 transform, engine::ref<engine::shader> shader)
{
	transform = glm::scale(transform, glm::vec3(m_scale.x, m_scale.y, 1.f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	m_main_image->bind();
	// Render the whole image
	if (m_total_frames == 0) {
		engine::renderer::submit(shader, m_main_image_quad->mesh(), transform);
	}
	// Render a specific frame from the image
	else if (m_total_frames > 0) {
		int index = m_current_frame - 1;
		engine::renderer::submit(shader, m_frame_quads[index]->mesh(), transform);
	}
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

//Add a specific point from the image to vector of all frames. This function will prevent the whole image from being
//rendered and can be used even if the sprite is not intended to be animated.
void sprite::add_frame_quad(int sprite_size_x, int sprite_size_y, int horizontal_frame_square, int vertical_frame_square) {
	float image_step_x = 1.0f/(m_main_image->width() / sprite_size_x);
	float image_step_y = 1.0f/(m_main_image->height() / sprite_size_y);

	//texture coordinate y is inversed so quad texture appears from top to bottom of the image instead from bottom to top.
	engine::ref<quad> quad = quad::create(glm::vec2(sprite_size_x, sprite_size_y),
		glm::vec2(horizontal_frame_square * image_step_x, horizontal_frame_square * image_step_x + image_step_x),
		glm::vec2(1.0f-(vertical_frame_square * image_step_y + image_step_y), 1.0f - (vertical_frame_square * image_step_y)));

	m_frame_quads.push_back(quad);
	m_total_frames += 1;
}

// Change the current frame, as long as it exists.
void sprite::set_current_frame(int frame) {
	if (frame <= m_total_frames && frame > 0) {
		m_current_frame = frame;
	}
	else {
		m_current_frame = 1;
	}
}
//Create pointer to class
engine::ref<sprite> sprite::create(const std::string& path, float width, float height)
{
	return std::make_shared<sprite>(path, width, height);
}
