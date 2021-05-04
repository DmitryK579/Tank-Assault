#include "player_tank.h"

player_tank::player_tank() {
	m_chassis_sprite = sprite::create("assets/textures/lightvehicles.png", 26, 26);
}

player_tank::~player_tank() {

}

void player_tank::on_update(const engine::timestep& time_step) {

}

void player_tank::on_render(engine::ref<engine::shader> shader) {

}

void player_tank::on_event(engine::event& event) {

}

engine::ref<player_tank> player_tank::create()
{
	return std::make_shared<player_tank>();
}
