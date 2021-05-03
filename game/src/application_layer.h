#pragma once
#include <engine.h>
#include "cross_fade.h"
#include "sprite.h"
#include "main_menu.h"

class application_layer : public engine::layer
{
public:
    application_layer();
	~application_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	void render_menu();
	void switch_menu(int menu_choice);
	void confirm_selection();
	//engine::ref<engine::skybox>			m_skybox{};
	//engine::ref<engine::game_object>	m_terrain{};
	//engine::ref<engine::game_object>	m_cow{};
	//engine::ref<engine::game_object>	m_tree{};
	//engine::ref<engine::game_object>	m_ball{};
	//engine::ref<engine::game_object>	m_mannequin{};

	//engine::ref<engine::material>		m_material{};

	//engine::DirectionalLight            m_directionalLight;

	//std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	//engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	//float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera       m_2d_camera; 
    //engine::perspective_camera        m_3d_camera;

	engine::ref<main_menu>							m_main_menu{};

	bool m_in_menu;

};
