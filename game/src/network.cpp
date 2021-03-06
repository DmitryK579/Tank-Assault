#include "network.h"

network::network() {
	// Initialize class variables
	m_server_port = 5029;
	m_is_host = false;
	m_is_active = false;
	m_max_players = 4;
	m_public_ip_address = sf::IpAddress::getPublicAddress().toString();
	m_local_ip_address = sf::IpAddress::getLocalAddress().toString();

	// Create pointers to server and client classes
	m_server = server::create(m_server_port, m_max_players);
	m_client = client::create(m_server_port);

}
network::~network() {

}

// Launch a new server with provided player name as host.
void network::create_server(std::string player_name) {
	m_user_port = m_server_port;
	m_is_host = true;
	m_is_active = true;
	m_player_name = player_name;

	m_server->launch_server(player_name);
}

// Attempt to join an existing server on given ip address. Also sets the port that the user will be listening on
// (server port already known).
void network::join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name) {
	m_player_name = player_name;
	m_is_active = true;

	m_client->join_server(ip_address, port, player_name);
}

// If host, closes the server. If client, disconnects from a server.
void network::leave_server() {
	if (m_is_active) {
		if (m_is_host) {
			m_is_host = false;
			m_server->close_server();
		}
		else {
			m_client->leave_server();
		}
		m_is_active = false;
	}
}
// Start game as host
void network::server_start_game() {
	if (m_is_host) {
		m_server->start_game();
	}
}
// Call every frame
void network::on_update(const engine::timestep& time_step) {
	if (m_is_active) {
		if (m_is_host) {
			m_server->on_update(time_step);
			m_is_active = m_server->is_active();
		}
		else {
			m_client->on_update(time_step);
			m_is_active = m_client->is_active();
		}
	}
}

// Return name in specified index
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

int network::get_user_id() {
	if (m_is_host || !m_is_active) {
		return 0;
	}
	else {
		return m_client->get_id();
	}
}

int network::get_number_of_players() {
	if (m_is_host) {
		return m_server->get_number_of_players();
	}
	else {
		return m_client->get_number_of_players();
	}
}

// Takes tank states from the vector acting as a first in first out queue, and erase the states
// that were obtained from the queue.
std::vector<network_message::object_states> network::get_received_tank_states() {
	std::vector<network_message::object_states> received_tank_states;
	if (m_is_host) {
		received_tank_states = m_server->get_received_tank_states();
		m_server->erase_received_tank_states(received_tank_states.size());
	}
	else {
		received_tank_states = m_client->get_received_tank_states();
		m_client->erase_received_tank_states(received_tank_states.size());
	}
	return received_tank_states;
}

// Send tank object state to network
void network::send_tank_state(network_message::object_states& tank_state) {
	if (m_is_host) {
		m_server->send_tank_state(tank_state);
	}
	else {
		m_client->send_tank_state(tank_state);
	}
}

bool network::all_players_ready() {
	if (m_is_host) {
		return m_server->all_players_ready();
	}
	else {
		return m_client->all_players_ready();
	}
}

// Checks if any player has disconnected from the game.
bool network::player_in_game_disconnected() {
	if (m_is_host) {
		return m_server->player_in_game_disconnected();
	}
	return false;
}

// Confirms to server that all objects states in level were sent.
void network::object_states_sent() {
	if (m_is_host) {
		m_server->object_states_sent();
	}
}

// Return pointer to class.
engine::ref<network> network::create()
{
	return std::make_shared<network>();
}


