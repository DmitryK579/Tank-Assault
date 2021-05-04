#include "application_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

application_layer::application_layer() 
    :m_2d_camera(-(float)engine::application::window().width(), (float)engine::application::window().width(), -(float)engine::application::window().height(), (float)engine::application::window().height())
{

    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::event, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	//m_audio_manager->play("music");


	// Initialise the shaders, materials and lights
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));

	// Load the tree model. Create a tree object. Set its properties
	/*
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);
	*/
	
	m_text_manager = engine::text_manager::create();

	//m_sprite = sprite::create("assets/textures/base.png", 80, 80, 4, 1.f);

	// Initialize main menu
	m_main_menu = main_menu::create();
	m_in_menu = true;

	m_level = level::create();
}

application_layer::~application_layer() {}

void application_layer::on_update(const engine::timestep& time_step) 
{
    //m_3d_camera.on_update(time_step);

	//m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	//m_mannequin->animated_mesh()->on_update(time_step);

	//m_sprite->on_update(time_step);

	m_in_menu = m_main_menu->on_update();
} 

void application_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	//2d scene using the material shader
	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);

	//glm::mat4 transform(1.0f);
	//transform = glm::translate(transform, glm::vec3(0.f, 0.f, -0.1f));
	//m_sprite->on_render(transform, textured_lighting_shader);

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	if (m_in_menu) {
		m_main_menu->on_render(textured_lighting_shader, text_shader);

	}
	else {
		m_level->on_render(textured_lighting_shader);
	}
	engine::renderer::end_scene();
	// Render text
	// m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height()-25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
} 

void application_layer::on_event(engine::event& event) 
{
    if(event.event_type() == engine::event_type_e::key_pressed) 
    {
		if (m_in_menu) {
			m_main_menu->on_event(event);
		}
		else {
			auto& e = dynamic_cast<engine::key_pressed_event&>(event);
			if (e.key_code() == engine::key_codes::KEY_TAB)
			{
				engine::render_command::toggle_wireframe();
			}
		}
    } 
}
