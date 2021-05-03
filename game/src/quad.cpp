#include "pch.h"
#include "quad.h"

quad::quad(glm::vec2 half_extents, glm::vec2 x_tex_coords, glm::vec2 y_tex_coords) : m_half_extents(half_extents)
{

	std::vector<engine::mesh::vertex> quad_vertices
	{
		//                   position												normal				 tex coord       
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ x_tex_coords[0], y_tex_coords[0] } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ x_tex_coords[1], y_tex_coords[0] } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ x_tex_coords[1], y_tex_coords[1] } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  0.0f },		{ 0.0f, 0.0f,  -1.0f },		{ x_tex_coords[0], y_tex_coords[1] } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	m_mesh = engine::mesh::create(quad_vertices, quad_indices);
}

quad::~quad() {}

engine::ref<quad> quad::create(glm::vec2 half_extents, glm::vec2 x_tex_coords,glm::vec2 y_tex_coords)
{
	return std::make_shared<quad>(half_extents,x_tex_coords,y_tex_coords);
}
