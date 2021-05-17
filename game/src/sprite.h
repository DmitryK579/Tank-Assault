#pragma once
#include <engine.h>

class quad;

class sprite
{
public:
	sprite(const std::string& path, float width, float height);
	~sprite();

	void on_update(const engine::timestep& time_step);
	void on_render(glm::mat4 transform, engine::ref<engine::shader> shader);
	void add_frame_quad(int sprite_size_x, int sprite_size_y, int horizontal_frame_square, int vertical_frame_square);

	void set_scale(glm::vec2 scale) { m_scale = scale; }
	glm::vec2 get_scale() { return m_scale; }

	void set_transparency(float value) { m_transparency = value; }
	float get_transparency() { return m_transparency; }

	void set_current_frame(int frame);
	int get_current_frame() { return m_current_frame; }

	int get_total_frames() { return m_total_frames; }

	void set_frame_time(float time) { m_frame_time = time; }
	float get_frame_time() { return m_frame_time; }

	void set_animating(bool state) { m_is_animating = state; }
	bool get_animating() { return m_is_animating; }

	static engine::ref<sprite> create(const std::string& path, float width, float height);

private:

	float								m_transparency;
	float								m_timer;
	float								m_frame_time;
	int									m_total_frames;
	int									m_current_frame;
	bool								m_is_animating;
	glm::vec2							m_scale;

	engine::ref<engine::texture_2d>		m_main_image;
	engine::ref<quad>					m_main_image_quad;
	std::vector<engine::ref<quad>>		m_frame_quads;
};
