#pragma once
#include <engine.h>
#include "quad.h"

class terrain {
public:
	terrain(const std::string& path);
	~terrain();

	void on_render(engine::ref<engine::shader> shader);
	void build_layout(int horizontal_tile_number, int vertical_tile_number, const std::vector<std::pair<int,int>>& terrain_sequence);

	static engine::ref<terrain> create(const std::string& path);
private:
	engine::ref<engine::texture_2d> m_terrain_spritesheet;
	int m_tile_size_x;
	int m_tile_size_y;
	int m_horizontal_tile_number;
	int m_vertical_tile_number;

	std::vector<engine::ref<quad>> m_terrain_quads;
};
