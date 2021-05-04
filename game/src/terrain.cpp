#include "terrain.h"

terrain::terrain(const std::string& path) {
	m_terrain_spritesheet = engine::texture_2d::create(path, true);
	m_tile_size_x = 40;
	m_tile_size_y = 40;
}

terrain::~terrain() {

}
// Create quads of ground tiles based on tile positions in the spritesheet. Uses square positions in horizontal 
// and vertical axis instead of precise pixel coordinates.
void terrain::build_layout(int horizontal_tile_number, int vertical_tile_number, const std::vector<std::pair<int,int>>& terrain_sequence) {
	m_horizontal_tile_number = horizontal_tile_number;
	m_vertical_tile_number = vertical_tile_number;

	float image_step_x = 1.0f/(m_terrain_spritesheet->width() / m_tile_size_x);
	float image_step_y = 1.0f/(m_terrain_spritesheet->height() / m_tile_size_y);

	for (int i = 0; i < terrain_sequence.size(); i++) {
		//y texture coordinate y is inversed so quad texture appears from top to bottom of the image instead from bottom to top.
		engine::ref<quad> quad = quad::create(glm::vec2(m_tile_size_x, m_tile_size_y),
			glm::vec2(terrain_sequence[i].first * image_step_x, terrain_sequence[i].first * image_step_x + image_step_x),
			glm::vec2(1.f-(terrain_sequence[i].second * image_step_y + image_step_y), 1.f - (terrain_sequence[i].second * image_step_y)));
		m_terrain_quads.push_back(quad);
	}
}

// Call to render all ground tiles.
void terrain::on_render(engine::ref<engine::shader> shader) {
	m_terrain_spritesheet->bind();
	int quad_index = 0;
	for (int i = 0; i < m_vertical_tile_number; i++) {
		for (int j = 0; j < m_horizontal_tile_number; j++) {
			glm::mat4 transform(1.0f);
			transform = glm::translate(transform, glm::vec3(
				m_tile_size_x * j * 2 - ((float)engine::application::window().width() - 80.0f),
				-m_tile_size_y * i * 2 + ((float)engine::application::window().height() - 80.0f),
				-.1f));
			engine::renderer::submit(shader, m_terrain_quads[quad_index]->mesh(), transform);
			quad_index += 1;

		}
	}
}

//Create pointer to class.
engine::ref<terrain> terrain::create(const std::string& path)
{
	return std::make_shared<terrain>(path);
}
