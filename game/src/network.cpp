#include "network.h"

sf::Packet& operator <<(sf::Packet& packet, const network::message_struct& message) {
	return packet << message.sender_id << message.message_id << message.message_body;
}

sf::Packet& operator >>(sf::Packet& packet, network::message_struct& message) {
	return packet >> message.sender_id >> message.message_id >> message.message_body;
}

network::network() {
	m_port = 5029;
	m_is_host = false;
	m_accepting_clients = false;
	m_is_active = false;
	m_user_id = 255;
	m_max_players = 4;
	m_empty_name = "<Empty>";
	m_separation_marker = ';';
	for (int i = 0; i < m_max_players; i++) {
		m_player_names.push_back(m_empty_name);
	}
	m_public_ip_address = sf::IpAddress::getPublicAddress().toString();
}
network::~network() {

}

void network::recieve_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		message_struct message;
		sf::Packet recieved_packet;
		m_socket.receive(recieved_packet,sender,port);
		recieved_packet >> message;
		process_message(message,sender,port);
	}
}

void network::process_message(const message_struct& message, const sf::IpAddress& sender, const unsigned short& port) {

	if (message.sender_id == 255) {
		message_struct response;
		response.sender_id = m_user_id;
		if (message.message_id == id_request_join) {
			response.message_id = id_request_join_response;
			if (m_accepting_clients) {
				m_new_ip_address_queue.push_back(sender);
				response.message_body = "y";
			}
			else {
				response.message_body = "n";
			}
			send_message(response, sender, port);
		}
	}
	else {
		switch (message.message_id) {
		case id_request_join_response:
			if (message.message_body == "y") {
				m_session_ip_addresses.push_back(sender);
			}
			break;

		case id_user_id_assignment:
		{
			m_user_id = stoi(message.message_body);
			message_struct response = { m_user_id,id_player_name,m_player_name };
			send_message(response, sender, port);
			break;
		}

		case id_player_name:
			m_player_names[message.sender_id] = message.message_body;
			send_all_player_names();
			break;

		case id_all_player_names:
		{
			std::vector<std::string> all_names = split_string(message.message_body);
			for (int i = 0; i < all_names.size(); i++) {
				m_player_names[i] = all_names[i];
			}
			break;
		}
		case id_ping:
			break;
		}
	}
}

void network::create_server(std::string player_name) {
	m_user_id = 0;
	m_is_host = true;
	m_is_active = true;
	m_accepting_clients = true;
	m_socket.bind(m_port);
	m_player_names[0] = player_name;
	m_communication_thread = new std::thread (&network::recieve_messages,this);
}

void network::join_server(sf::IpAddress ip_address, unsigned short port) {
	m_socket.bind(m_port);
	m_is_active = true;
	m_communication_thread = new std::thread (&network::recieve_messages,this);
	message_struct message = { m_user_id,id_request_join,""};
	send_message(message, ip_address, port);
}

void network::leave_server() {
	m_user_id = 255;
	m_is_host = false;
	m_is_active = false;
	m_accepting_clients = false;
	m_socket.unbind();
	m_session_ip_addresses.clear();
	reset_player_names();
}

void network::on_update(const engine::timestep& time_step) {
	if (m_is_active) {
		if (m_new_ip_address_queue.size() > 0 && m_session_ip_addresses.size() < m_max_players) {
			assign_new_user_id();
		}
	}
}

void network::assign_new_user_id() {
	auto iterator = std::find(m_player_names.begin(), m_player_names.end(), m_empty_name);

	// If empty name found
	if (iterator != m_player_names.end())
	{
		m_session_ip_addresses.push_back(m_new_ip_address_queue[0]);
		int index = iterator - m_player_names.begin();
		message_struct message = { m_user_id, id_user_id_assignment, std::to_string(index) };
		send_message(message, m_new_ip_address_queue[0], m_port);

		m_new_ip_address_queue.erase(m_new_ip_address_queue.begin());

		if (m_session_ip_addresses.size() == m_max_players) {
			m_accepting_clients = false;
		}
	}
}

void network::send_message(const message_struct& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet;
	packet << message;
	m_socket.send(packet, ip, port);
}

void network::send_all_player_names() {
	std::string body = "";
	for (int i = 0; i < m_player_names.size(); i++) {
		body += m_player_names[i];
		body += m_separation_marker;
	}
	message_struct message = { m_user_id,id_all_player_names,body };
	for (int i = 0; i < m_session_ip_addresses.size(); i++) {
		send_message(message, m_session_ip_addresses[i], m_port);
	}
}

std::vector<std::string> network::split_string(const std::string& string) {
	std::vector<std::string> string_vector;
	std::stringstream stringstream(string);
	std::string segment;
	while (std::getline(stringstream, segment, m_separation_marker)) {
		string_vector.push_back(segment);
	}
	return string_vector;
}

void network::reset_player_names() {
	for (int i = 0; i < m_player_names.size(); i++) {
		m_player_names[i] = m_empty_name;
	}
}

engine::ref<network> network::create()
{
	return std::make_shared<network>();
}


