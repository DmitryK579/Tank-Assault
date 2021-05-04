#pragma once
#include <engine.h>
#include "cross_fade.h"
#include "sprite.h"
#include "main_menu.h"
#include "level.h"

class application_layer : public engine::layer
{
public:
    application_layer();
	~application_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera			m_2d_camera; 

	engine::ref<main_menu>				m_main_menu{};
	bool								m_in_menu;

	engine::ref<level>					m_level{};
};
