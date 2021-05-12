#include "server.h"

sf::Packet& operator <<(sf::Packet& packet, const network_message::message& message) {
	return packet << message.sender_id << message.message_id << message.message_body;
}

sf::Packet& operator >>(sf::Packet& packet, network_message::message& message) {
	return packet >> message.sender_id >> message.message_id >> message.message_body;
}

server::server(unsigned short port) {
	m_server_port = port;
	m_accepting_clients = false;
	m_is_active = false;
	m_user_id = 0;
	m_max_players = 4;
	m_empty_name = "<Empty>";
	for (int i = 0; i < m_max_players; i++) {
		m_player_names.push_back(m_empty_name);
	}
}

server::~server() {

}

void server::recieve_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		network_message::message message;
		sf::Packet recieved_packet;
		if (m_socket.receive(recieved_packet, sender, port) != sf::Socket::Done) {
			//error
			break;
		}
		recieved_packet >> message;
		process_message(message, sender, port);
	}
}

void server::process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	if (message.sender_id == 255) {
		respond_to_join_request(message, sender, port);
	}
	else {
		switch (message.message_id) {

		case network_message::id_player_name:
			m_player_names[message.sender_id] = message.message_body;
			send_all_player_names();
			m_client_connection_steps[message.sender_id] = step_ready;
			break;

		case network_message::id_ping:
			break;

		case network_message::id_leave:
			disconnect_player(message.sender_id);
			break;

		case network_message::id_kick:
			break;
		}
	}
}

void server::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet;
	packet << message;
	m_socket.send(packet, ip, port);
}

void server::respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {
	if (message.message_id == network_message::id_request_join) {
		network_message::message response;
		response.sender_id = m_user_id;
		response.message_id = network_message::id_request_join_response;
		if (m_accepting_clients) {
			m_valid_connections.push_back({ sender,port });
			m_client_connection_steps.push_back(step_request_name);
			response.message_body = "y";

			if (m_valid_connections.size() == m_max_players) {
				m_accepting_clients = false;
			}
		}
		else {
			response.message_body = "n";
		}
		send_message(response, sender, port);
	}
}

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

void server::disconnect_player(int id) {
	m_valid_connections.erase(m_valid_connections.begin() + id - 1);
	m_client_connection_steps.erase(m_client_connection_steps.begin() + id - 1);
	for (int i = id; i < m_player_names.size() - 1; i++) {
		m_player_names[i] = m_player_names[i + 1];
	}
	m_player_names[m_max_players - 1] = m_empty_name;
	if (m_valid_connections.size() > 0) {
		send_all_player_names();
		reassign_id_of_existing_users();
	}
}

void server::reassign_id_of_existing_users() {
	network_message::message message = { m_user_id,network_message::id_user_id_assignment,"" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		message.message_body = std::to_string(i + 1);
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
}

void server::reset_player_names() {
	for (int i = 0; i < m_player_names.size(); i++) {
		m_player_names[i] = m_empty_name;
	}
}

void server::launch_server(std::string player_name) {
	m_is_active = true;
	m_accepting_clients = true;
	m_socket.bind(m_server_port);
	m_player_name = player_name;
	m_player_names[0] = m_player_name;
	m_communication_thread = new std::thread(&server::recieve_messages, this);
}

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

engine::ref<server> server::create(unsigned short port)
{
	return std::make_shared<server>(port);
}
