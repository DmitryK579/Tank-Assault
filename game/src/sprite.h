#pragma once
#include <engine.h>

class quad;

class sprite
{
public:
	sprite(const std::string& path, float width, float height, int frames, float frame_time);
	~sprite();

	void on_update(const engine::timestep& time_step);
	void on_render(glm::mat4 transform, engine::ref<engine::shader> shader);
	void create_sprite_quad(int sprite_size_x, int sprite_size_y, int horizontal_sprite_square, int vertical_sprite_square);

	static engine::ref<sprite> create(const std::string& path, float width, float height, int frames, float frame_time);

private:

	glm::vec2 m_scale;
	float m_timer;
	float m_frame_time;
	int m_total_frames;
	int m_current_frame;
	bool m_is_animating;

	engine::ref<engine::texture_2d> m_texture;
	float m_transparency;
	std::vector<engine::ref<quad>> m_quads;
};
