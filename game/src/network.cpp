#include "network.h"

network::network() {
	m_server_port = 5029;
	m_is_host = false;
	m_is_active = false;
	m_max_players = 4;

	m_server = server::create(m_server_port, m_max_players);
	m_client = client::create(m_server_port);

	m_public_ip_address = sf::IpAddress::getPublicAddress().toString();
	m_local_ip_address = sf::IpAddress::getLocalAddress().toString();

}
network::~network() {

}

void network::create_server(std::string player_name) {
	m_user_port = m_server_port;
	m_is_host = true;
	m_is_active = true;
	m_player_name = player_name;

	m_server->launch_server(player_name);
}

void network::join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name) {
	m_player_name = player_name;
	m_is_active = true;

	m_client->join_server(ip_address, port, player_name);
}

void network::leave_server() {
	if (m_is_host) {
		m_is_host = false;
		m_server->close_server();
	}
	else {
		m_client->leave_server();
	}
	m_is_active = false;
}

void network::on_update(const engine::timestep& time_step) {
	
}

std::string network::get_player_name(int index) {
	if (m_is_host) {
		return m_server->get_player_names()[index];
	}
	else {
		if (m_client->get_player_names().size() != m_max_players) {
			return m_server->get_player_names()[index];
		}
		else {
			return m_client->get_player_names()[index];
		}
	}
}

engine::ref<network> network::create()
{
	return std::make_shared<network>();
}


