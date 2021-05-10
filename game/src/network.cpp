#include "network.h"

network::network() {
	m_port = 5029;
	m_is_host = false;
	m_is_active = false;
	m_user_id = -1;
}
network::~network() {

}

void network::process_communication() {
	while (m_is_active) {

	}
}

void network::create_server() {
	m_user_id = 1;
	m_is_host = true;
}

void network::join_server() {

}

void network::leave_server() {
	m_user_id = -1;
	m_is_host = false;
}

void network::on_update(const engine::timestep& time_step) {

}

engine::ref<network> network::create()
{
	return std::make_shared<network>();
}
