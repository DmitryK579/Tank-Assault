#include "client.h"

client::client(unsigned short server_port) {
	// Initialize class variables
	m_server_port = server_port;
	m_server_ip = "127.0.0.1";
	m_user_port = 0;
	m_user_id = 255;
	m_max_reconnection_attempts = 10;
	m_current_reconnection_attempt = 0;
	m_timeout_timer = 0;
	m_player_name = "!";
	m_is_active = false;
	m_all_players_ready = false;
}

client::~client() {

}
// Recieve packets sent to client
void client::recieve_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet recieved_packet;
		if (m_socket.receive(recieved_packet, sender, port) != sf::Socket::Done) {
			//error
		}
		else {
			network_message::message message = read_message_from_sfml_packet(recieved_packet);
			process_message(message, sender, port);
		}
		
	}
}

// Check message types and respond to them.
void client::process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	// Clients should not contact other clients
	if (message.sender_id != 255){
		switch (message.message_id) {

		// Response from server to this client's join request
		case network_message::id_request_join_response:
			// Join request accepted
			if (message.message_body == "y") {
				m_server_ip = sender;
				switch_connection_step(step_request_id);
			}
			// Join request denied
			else {
				leave_server();
			}
			break;

		// Server's assigned ID to this client
		case network_message::id_user_id_assignment:
		{
			m_user_id = stoi(message.message_body);
			if (m_connection_step == step_request_id || m_connection_step == step_request_connect) {
				// Server sends two messages as reply in case of a successful connection
				// so this statement is a safeguard if messages arrive out of order
				// since the second message will arrive only if connection has been accepted
				if (m_server_ip != sender) {
					m_server_ip = sender;
				}
				send_user_name();
				switch_connection_step(step_request_all_names);
			}
			break;
		}
		case network_message::id_player_name_request:
			send_user_name();
			break;

		// Recieved name of every player from server
		case network_message::id_all_player_names:
		{
			std::vector<std::string> all_names = network_message::split_message(message.message_body);
			m_player_names = all_names;
			switch_connection_step(step_in_lobby);
			break;
		}

		// Recieved ping
		case network_message::id_ping:
			m_timeout_timer = 0;
			m_current_reconnection_attempt = 0;
			respond_to_ping(sender, port);
			break;

		case network_message::id_ping_return:
			m_timeout_timer = 0;
			m_current_reconnection_attempt = 0;
			break;

		// Server closed by host
		case network_message::id_leave:
			if (message.sender_id == 0) {
				leave_server();
			}
			break;

		// This client got kicked by the server
		case network_message::id_kick:
			leave_server();
			break;

		// Server started the game
		case network_message::id_start_game:
			switch_connection_step(step_start_sync_check);
			game_start_response();
			break;

		// Server confirmed every client can start the game
		case network_message::id_start_sync_confirmed:
			switch_connection_step(step_in_game);
			m_all_players_ready = true;
			break;

		case network_message::id_tank_state:
			store_tank_state(message);
			break;
		}
	}
}

void client::on_update(const engine::timestep& time_step) {
	if (m_is_active) {
		m_timeout_timer += (float)time_step;
		if (m_timeout_timer > 2) {
			if (m_current_reconnection_attempt >= m_max_reconnection_attempts) {
				leave_server();
			}
			retry_send();
		}
	}
}

void client::retry_send() {
	m_timeout_timer = 0;
	m_current_reconnection_attempt += 1;

	network_message::message message = { m_user_id, 0, "" };

	switch (m_connection_step) {
	case step_request_connect:
		message.message_id = network_message::id_request_join;
		break;
	case step_request_id:
		message.message_id = network_message::id_user_id_request;
		break;
	case step_request_all_names:
		message.message_id = network_message::id_all_player_names_request;
		break;
	case step_in_lobby:
		message.message_id = network_message::id_ping;
		break;
	case step_start_sync_check:
		message.message_id = network_message::id_start_sync;
		break;
	case step_in_game:
		message.message_id = network_message::id_ping;
		break;
	}

	send_message(message, m_server_ip, m_server_port);

}

// Send message to target server
void client::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet = write_to_sfml_packet(message);
	m_socket.send(packet, ip, port);
}

void client::switch_connection_step(int step) {
	m_connection_step = step;
	m_timeout_timer = 0;
	m_current_reconnection_attempt = 0;
}

// Send the player's name to server
void client::send_user_name() {
	network_message::message response = { m_user_id,network_message::id_player_name,m_player_name };
	send_message(response, m_server_ip, m_server_port);
}

void client::game_start_response() {
	network_message::message response = { m_user_id,network_message::id_start_sync,"" };
	send_message(response, m_server_ip, m_server_port);
}

void client::send_tank_state(network_message::object_states& tank_state) {
	network_message::message message = { m_user_id,network_message::id_tank_state,"" };
	std::string body = "";

	body += std::to_string(tank_state.object_id);
	body += network_message::separation_symbol;

	body += std::to_string(tank_state.x_coords);
	body += network_message::separation_symbol;

	body += std::to_string(tank_state.y_coords);
	body += network_message::separation_symbol;

	body += tank_state.movement_command;
	body += network_message::separation_symbol;

	body += std::to_string(tank_state.fire);
	body += network_message::separation_symbol;

	body += std::to_string(tank_state.connected);
	body += network_message::separation_symbol;

	message.message_body = body;

	send_message(message, m_server_ip, m_server_port);
	
}

void client::respond_to_ping(const sf::IpAddress& ip, const unsigned short& port) {
	network_message::message message = { m_user_id,network_message::id_ping_return,"" };
	send_message(message, ip, port);
}

void client::store_tank_state(const network_message::message& message) {
	m_timeout_timer = 0;
	m_current_reconnection_attempt = 0;

	network_message::object_states tank;
	std::vector<std::string> tank_states = network_message::split_message(message.message_body);
	tank.object_id = stoi(tank_states[0]);
	tank.x_coords = stof(tank_states[1]);
	tank.y_coords = stof(tank_states[2]);
	tank.movement_command = tank_states[3][0];
	tank.fire = stoi(tank_states[4]);
	tank.connected = stoi(tank_states[5]);

	m_received_tank_states.push_back(tank);
}

void client::erase_received_tank_states(int entries_to_delete) {
	for (int i = 0; i < entries_to_delete; i++) {
		m_received_tank_states.erase(m_received_tank_states.begin());
	}
}

// Attempt to join an existing server by given IP. Also sets the client's port to listen for replies.
void client::join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name) {
	m_player_name = player_name;
	m_user_port = port;
	m_socket.bind(port);
	m_is_active = true;
	m_server_ip = ip_address;
	switch_connection_step(step_request_connect);
	m_communication_thread = new std::thread(&client::recieve_messages, this);
	network_message::message message = { m_user_id,network_message::id_request_join,"" };
	send_message(message, ip_address, m_server_port);
}

// Leave the server.
void client::leave_server() {
	if (m_is_active) {
		network_message::message message = { m_user_id, network_message::id_leave, "" };
		send_message(message, m_server_ip, m_server_port);
		switch_connection_step(0);
		m_user_id = 255;
		m_is_active = false;
		m_all_players_ready = false;
		m_socket.unbind();
		m_server_ip = "127.0.0.1";
		m_player_names.clear();
		m_received_tank_states.clear();
	}
}

int client::get_number_of_players() {
	int players = 0;
	for (int i = 0; i < m_player_names.size(); i++) {
		if (m_player_names[i] != "<Empty>") {
			players += 1;
		}
	}
	return players;
}

// Write message to SFML packet
sf::Packet client::write_to_sfml_packet(const network_message::message& message) {
	sf::Packet packet;
	packet << message.sender_id << message.message_id << message.message_body;
	return packet;
}

// Read message from SFML packet
network_message::message client::read_message_from_sfml_packet(sf::Packet& packet) {
	network_message::message message;
	packet >> message.sender_id >> message.message_id >> message.message_body;
	return message;
}

// Create pointer to class
engine::ref<client> client::create(unsigned short server_port)
{
	return std::make_shared<client>(server_port);
}
