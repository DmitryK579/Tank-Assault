#include "server.h"

server::server(unsigned short server_port, int max_players) {
	// Initialize class variables
	m_server_port = server_port;
	m_accepting_clients = false;
	m_is_active = false;
	m_is_in_game = false;
	m_all_players_ready = false;
	m_user_id = 0;
	m_max_players = 4;
	m_empty_name = "<Empty>";
	// Create placeholder names to display in the lobby when clients are not connected.
	for (int i = 0; i < m_max_players; i++) {
		m_player_names.push_back(m_empty_name);
	}
}

server::~server() {

}

// Recieve packets sent to server
void server::recieve_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet recieved_packet;
		if (m_socket.receive(recieved_packet, sender, port) != sf::Socket::Done) {
			//error
			break;
		}
		network_message::message message = read_message_from_sfml_packet(recieved_packet);
		process_message(message, sender, port);
	}
}
// Check message types and respond to them.
void server::process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	// New client is attempting to join
	if (message.sender_id == 255) {
		respond_to_join_request(message, sender, port);
	}
	else {
		switch (message.message_id) {

		// Recieved client's name
		case network_message::id_player_name:
			m_player_names[message.sender_id] = message.message_body;
			send_all_player_names();
			m_client_connection_steps[message.sender_id-1] = step_in_lobby;
			break;

		// Recieved ping
		case network_message::id_ping:
			break;

		// Client left the server
		case network_message::id_leave:
			disconnect_player(message.sender_id);
			break;

		case network_message::id_start_sync:
			m_client_connection_steps[message.sender_id - 1] = step_start_sync_confirmed;
			start_sync_response();
			break;
		}
	}
}

// Send message to target client.
void server::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet = write_to_sfml_packet(message);
	m_socket.send(packet, ip, port);
}

// Send response to a client's request to join the server
void server::respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {
	if (message.message_id == network_message::id_request_join) {
		bool send_new_id = false;
		std::string body = "";
		if (m_accepting_clients) {
			// Accept join request
			m_valid_connections.push_back({ sender,port });
			m_client_connection_steps.push_back(step_request_name);
			body = "y";
			send_new_id = true;

			// Maximum players reached; deny any further requests
			if (m_valid_connections.size() == m_max_players-1) {
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
	// Erase connection details
	m_valid_connections.erase(m_valid_connections.begin() + id - 1);
	m_client_connection_steps.erase(m_client_connection_steps.begin() + id - 1);

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
	// Accept new clients
	if (!m_is_in_game) {
		m_accepting_clients = true;
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
		network_message::message message = { m_user_id, network_message::id_start_sync_confirmed,"" };
		for (int i = 0; i < m_valid_connections.size(); i++) {
			send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
			m_client_connection_steps[i] = step_in_game;
		}
		m_all_players_ready = true;
	}
}

// Create new server and begin listening for messages.
void server::launch_server(std::string player_name) {
	m_is_active = true;
	m_accepting_clients = true;
	m_socket.bind(m_server_port);
	m_player_name = player_name;
	m_player_names[0] = m_player_name;
	m_communication_thread = new std::thread(&server::recieve_messages, this);
}

// Disconnect all clients and close the server.
void server::close_server() {
	network_message::message message = { m_user_id, network_message::id_leave, "" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
	m_is_active = false;
	m_accepting_clients = false;
	m_socket.unbind();
	m_valid_connections.clear();
	m_client_connection_steps.clear();
	reset_player_names();
}

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
			m_client_connection_steps[i] = step_start_sync_request;
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
