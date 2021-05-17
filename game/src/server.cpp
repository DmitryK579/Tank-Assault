#include "server.h"

server::server(unsigned short server_port, int max_players) {
	// Initialize class variables
	m_server_port = server_port;
	m_accepting_clients = false;
	m_is_active = false;
	m_is_in_game = false;
	m_all_players_ready = false;
	m_player_in_game_disconnected = false;
	m_user_id = 0;
	m_max_players = 4;
	m_max_reconnect_attempts = 10;
	m_empty_name = "<Empty>";
	// Create placeholder names to display in the lobby when clients are not connected.
	for (int i = 0; i < m_max_players; i++) {
		m_player_names.push_back(m_empty_name);
	}
}

server::~server() {

}

// Recieve packets sent to server
void server::receive_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet received_packet;
		if (m_socket.receive(received_packet, sender, port) != sf::Socket::Done) {
			//error
		}
		else {
			network_message::message message = read_message_from_sfml_packet(received_packet);
			process_message(message, sender, port);
		}
	}
}
// Check message types and respond to them.
void server::process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	// New client is attempting to join
	if (message.sender_id == 255) {
		respond_to_join_request(message, sender, port);
	}
	else {
		// Check if a client that was removed from valid connections is attempting to contact the server.
		bool invalid_connection = true;
		for (int i = 0; i < m_valid_connections.size(); i++) {
			if (sender == m_valid_connections[i].first) {
				invalid_connection = false;
				break;
			}
		}
		if (!invalid_connection) {
			switch (message.message_id) {

				// Received client's name
			case network_message::id_player_name:
				m_player_names[message.sender_id] = message.message_body;
				send_all_player_names();
				switch_client_connection_step((message.sender_id - 1), step_in_lobby);
				break;

				// Received ping request
			case network_message::id_ping:
				m_client_timeout_info[message.sender_id - 1].first = 0;
				m_client_timeout_info[message.sender_id - 1].second = 0;
				respond_to_ping(sender, port);
				break;

				// Ping request returned
			case network_message::id_ping_return:
				m_client_timeout_info[message.sender_id - 1].first = 0;
				m_client_timeout_info[message.sender_id - 1].second = 0;
				break;

				// Client left the server
			case network_message::id_leave:
				disconnect_player(message.sender_id);
				break;

				// Received confirmation from client that they can start the game.
			case network_message::id_start_sync:
				switch_client_connection_step((message.sender_id - 1), step_start_sync_confirmed);
				start_sync_response();
				break;

				// Received tank object state.
			case network_message::id_tank_state:
				store_tank_state(message);
				break;
			}
		}
		else {
			kick(sender, port);
		}
	}
}

// Call every frame
void server::on_update(const engine::timestep& time_step) {

	if (m_is_active) {
		// If no message is received from a client for too long, contact them. If they cannot be
		// reached, disconnect them from this session.
		for (int i = 0; i < m_valid_connections.size(); i++) {
			m_client_timeout_info[i].first += (float)time_step;
			if (m_client_timeout_info[i].first > 2) {
				if (m_client_timeout_info[i].second >= m_max_reconnect_attempts) {
					disconnect_player(i + 1);
					break;
				}
				retry_send(i);
			}
		}
	}
}

// Send message to target client.
void server::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet = write_to_sfml_packet(message);
	m_socket.send(packet, ip, port);
}

// Send a message to a client based on that client's connection step.
void server::retry_send(int id) {
	m_client_timeout_info[id].first = 0;
	m_client_timeout_info[id].second += 1;

	network_message::message message = { m_user_id, 0, "" };

	switch (m_client_connection_steps[id]) {
	case step_request_name:
		message.message_id = network_message::id_player_name_request;
		break;
	case step_in_lobby:
		message.message_id = network_message::id_ping;
		break;
	case step_start_sync_request:
		message.message_id = network_message::id_start_game;
		break;
	case step_start_sync_confirmed:
		message.message_id = network_message::id_ping;
		break;
	case step_in_game:
		message.message_id = network_message::id_ping;
		break;
	}

	send_message(message, m_valid_connections[id].first, m_valid_connections[id].second);
}

// Change a client's connection step and reset their timeout variables.
void server::switch_client_connection_step(int id, int step) {
	m_client_connection_steps[id] = step;
	m_client_timeout_info[id].first = 0;
	m_client_timeout_info[id].second = 0;
}

// Send response to a client's request to join the server
void server::respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	bool existing_connection = false;
	// Check if a client was added as a valid connection, but the client did not receive a new ID.
	for (int i = 0; i < m_valid_connections.size(); i++) {
		if (m_valid_connections[i].first == sender) {
			existing_connection = true;
		}
	}
	if (!existing_connection) {
		if (message.message_id == network_message::id_request_join) {
			bool send_new_id = false;
			std::string body = "";
			if (m_accepting_clients) {
				// Accept join request
				m_valid_connections.push_back({ sender,port });
				m_client_connection_steps.push_back(step_request_name);
				m_client_timeout_info.push_back({ 0.0f, 0 });
				body = "y";
				send_new_id = true;

				// Maximum players reached; deny any further requests
				if (m_valid_connections.size() == m_max_players - 1) {
					m_accepting_clients = false;
				}
			}
			else {
				// Deny join request
				body = "n";
			}
			network_message::message response = { m_user_id, network_message::id_request_join_response, body };
			send_message(response, sender, port);
			// Assign new id to valid client
			if (send_new_id) {
				assign_id_to_new_user();
			}
		}
	}
	else {
		reassign_id_of_existing_users();
	}
}

// Send the name of every player to every client.
void server::send_all_player_names() {
	std::string body = "";
	for (int i = 0; i < m_player_names.size(); i++) {
		body += m_player_names[i];
		body += network_message::separation_symbol;
	}
	network_message::message message = { m_user_id,network_message::id_all_player_names,body };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
}

// Disconnect a client from the server
void server::disconnect_player(int id) {
	if (!m_is_in_game) {
		// Erase connection details
		m_valid_connections.erase(m_valid_connections.begin() + id - 1);
		m_client_connection_steps.erase(m_client_connection_steps.begin() + id - 1);
		m_client_timeout_info.erase(m_client_timeout_info.begin() + id - 1);

		// Move names up one position if client that disconnected is not the most recent client.
		for (int i = id; i < m_player_names.size() - 1; i++) {
			m_player_names[i] = m_player_names[i + 1];
		}
		m_player_names[m_max_players - 1] = m_empty_name;

		// Send updated information to remaining clients
		if (m_valid_connections.size() > 0) {
			send_all_player_names();
			reassign_id_of_existing_users();
		}
		else {
			// Begin solo game if clients cannot be reached.
			if (m_is_in_game) {
				m_all_players_ready = true;
			}
		}
		// Accept new clients
		if (!m_is_in_game) {
			m_accepting_clients = true;
		}
	}
	// Connection structure should not change during a game.
	else {
		m_player_names[id] = m_empty_name;
		send_all_player_names();
		m_player_in_game_disconnected = true;
	}
}

// Assign a new ID to client
void server::assign_id_to_new_user() {
	size_t connection_size = m_valid_connections.size();
	network_message::message message = { m_user_id,network_message::id_user_id_assignment,""};
	message.message_body = std::to_string(connection_size);
	send_message(message, m_valid_connections[connection_size-1].first, m_valid_connections[connection_size-1].second);
}

// Re-assign the IDs of existing clients
void server::reassign_id_of_existing_users() {
	network_message::message message = { m_user_id,network_message::id_user_id_assignment,"" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		message.message_body = std::to_string(i + 1);
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
}

// Set all player names to "<Empty>"
void server::reset_player_names() {
	for (int i = 0; i < m_player_names.size(); i++) {
		m_player_names[i] = m_empty_name;
	}
}

// Check if all clients are ready to begin the game
void server::start_sync_response() {
	bool wait = false;
	for (int i = 0; i < m_client_connection_steps.size(); i++) {
		if (m_client_connection_steps[i] != step_start_sync_confirmed) {
			wait = true;
		}
	}
	if (!wait) {
		// Send a message that all clients have confirmed they are ready to begin the game.
		network_message::message message = { m_user_id, network_message::id_start_sync_confirmed,"" };
		for (int i = 0; i < m_valid_connections.size(); i++) {
			send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
			switch_client_connection_step(i, step_in_game);
		}
		m_all_players_ready = true;
	}
}

// Send tank object state to all clients.
void server::send_tank_state(network_message::object_states& tank_state) {
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

	if (m_player_names[tank_state.object_id] != m_empty_name) {
		tank_state.connected = true;
	}
	else {
		tank_state.connected = false;
	}

	body += std::to_string(tank_state.connected);
	body += network_message::separation_symbol;

	message.message_body = body;

	for (int i = 0; i < m_valid_connections.size(); i++) {
		// Clients should not receive an update to their own position.
		if (tank_state.object_id - 1 != i) {
			send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
		}
	}
}

// Store received tank state in a vector acting as a first in first out queue.
void server::store_tank_state(const network_message::message& message) {
	m_client_timeout_info[message.sender_id - 1].first = 0;
	m_client_timeout_info[message.sender_id - 1].second = 0;

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

// Erase entries from the start of the tank state vector
void server::erase_received_tank_states(int entries_to_delete) {
	for (int i = 0; i < entries_to_delete; i++) {
		m_received_tank_states.erase(m_received_tank_states.begin());
	}
}

// Confirmation from level class that all object states have been sent.
void server::object_states_sent() {
	m_player_in_game_disconnected = false;
}

// Remove a client from this session
void server::kick(const sf::IpAddress& ip, const unsigned short& port) {
	network_message::message message = { m_user_id, network_message::id_kick,"" };
	send_message(message, ip, port);
}

// Respond to a ping request from a client
void server::respond_to_ping(const sf::IpAddress& ip, const unsigned short& port) {
	network_message::message message = { m_user_id,network_message::id_ping_return,"" };
	send_message(message, ip, port);
}

// Create new server and begin listening for messages.
void server::launch_server(std::string player_name) {
	m_is_active = true;
	m_accepting_clients = true;
	m_socket.bind(m_server_port);
	m_player_name = player_name;
	m_player_names[0] = m_player_name;
	m_communication_thread = new std::thread(&server::receive_messages, this);
}

// Disconnect all clients, close the server, and reset class variables.
void server::close_server() {
	network_message::message message = { m_user_id, network_message::id_leave, "" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
	m_is_active = false;
	m_accepting_clients = false;
	m_all_players_ready = false;
	m_socket.unbind();
	m_valid_connections.clear();
	m_client_connection_steps.clear();
	m_client_timeout_info.clear();
	m_received_tank_states.clear();
	reset_player_names();
}

// Send a message to all clients that the server is ready to start the game.
void server::start_game() {
	m_accepting_clients = false;
	m_is_in_game = true;
	if (m_valid_connections.size() == 0) {
		m_all_players_ready = true;
	}
	else {
		network_message::message message = { m_user_id, network_message::id_start_game,"" };
		for (int i = 0; i < m_valid_connections.size(); i++) {
			send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
			switch_client_connection_step(i, step_start_sync_request);
		}
	}
}

// Write message to SFML packet
sf::Packet server::write_to_sfml_packet(const network_message::message& message) {
	sf::Packet packet;
	packet << message.sender_id << message.message_id << message.message_body;
	return packet;
}

// Read message from SFML packet
network_message::message server::read_message_from_sfml_packet(sf::Packet& packet) {
	network_message::message message;
	packet >> message.sender_id >> message.message_id >> message.message_body;
	return message;
}

// Create pointer to class
engine::ref<server> server::create(unsigned short server_port, int max_players)
{
	return std::make_shared<server>(server_port, max_players);
}
