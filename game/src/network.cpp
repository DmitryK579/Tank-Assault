#include "network.h"

sf::Packet& operator <<(sf::Packet& packet, const network::message_struct& message) {
	return packet << message.sender_id << message.message_id << message.message_body;
}

sf::Packet& operator >>(sf::Packet& packet, network::message_struct& message) {
	return packet >> message.sender_id >> message.message_id >> message.message_body;
}

network::network() {
	m_server_port = 5029;
	m_is_host = false;
	m_connection_step = 0;
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
	m_local_ip_address = sf::IpAddress::getLocalAddress().toString();
}
network::~network() {

}

void network::recieve_messages() {
	while (m_is_active) {
		sf::IpAddress sender;
		unsigned short port;
		message_struct message;
		sf::Packet recieved_packet;
		if (m_socket.receive(recieved_packet, sender, port) != sf::Socket::Done) {
			//error
			break;
		}
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
				m_new_client_queue.push_back({ sender,port });
				response.message_body = "y";
				m_connection_step = step_request_name;
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
				m_valid_connections.push_back({ sender,port });
				m_connection_step = step_request_id;
			}
			else {
				leave_server();
			}
			break;

		case id_user_id_assignment:
		{
			m_user_id = stoi(message.message_body);
			if (m_connection_step != step_ready) {
				send_user_name(sender, port);
				m_connection_step = step_request_all_names;
			}
			break;
		}

		case id_player_name:
			m_player_names[message.sender_id] = message.message_body;
			send_all_player_names();
			m_connection_step = step_ready;
			break;

		case id_all_player_names:
		{
			std::vector<std::string> all_names = split_string(message.message_body);
			for (int i = 0; i < all_names.size(); i++) {
				m_player_names[i] = all_names[i];
			}
			m_connection_step = step_ready;
			break;
		}
		case id_ping:
			break;

		case id_leave:
			if (message.sender_id == 0) {
				m_valid_connections.clear();
				leave_server();
			}
			else {
				disconnect_player(message.sender_id);
			}
			break;

		case id_kick:

			break;
		}
	}
}

void network::create_server(std::string player_name) {
	m_user_id = 0;
	m_user_port = m_server_port;
	m_is_host = true;
	m_is_active = true;
	m_accepting_clients = true;
	m_socket.bind(m_server_port);
	m_player_name = player_name;
	m_player_names[0] = m_player_name;
	m_communication_thread = new std::thread (&network::recieve_messages,this);
}

void network::join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name) {
	m_player_name = player_name;
	m_socket.bind(port);
	m_is_active = true;
	m_connection_step = step_request_connect;
	m_communication_thread = new std::thread (&network::recieve_messages,this);
	message_struct message = { m_user_id,id_request_join,""};
	send_message(message, ip_address, m_server_port);
}

void network::leave_server() {
	message_struct message = { m_user_id, id_leave, "" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
	m_connection_step = 0;
	m_user_id = 255;
	m_is_host = false;
	m_is_active = false;
	m_accepting_clients = false;
	m_socket.unbind();
	m_valid_connections.clear();
	reset_player_names();
}

void network::on_update(const engine::timestep& time_step) {
	if (m_is_active) {
		if (m_new_client_queue.size() > 0 && m_valid_connections.size() < m_max_players) {
			assign_new_user_id();
		}
	}
}

void network::assign_new_user_id() {
	auto iterator = std::find(m_player_names.begin(), m_player_names.end(), m_empty_name);

	// If empty name found
	if (iterator != m_player_names.end())
	{
		m_valid_connections.push_back(m_new_client_queue[0]);
		int index = iterator - m_player_names.begin();
		message_struct message = { m_user_id, id_user_id_assignment, std::to_string(index) };
		send_message(message, m_new_client_queue[0].first, m_new_client_queue[0].second);

		m_new_client_queue.erase(m_new_client_queue.begin());

		if (m_valid_connections.size() == m_max_players) {
			m_accepting_clients = false;
		}
	}
}

void network::reassign_id_of_existing_users() {
	message_struct message = { m_user_id,id_user_id_assignment,"" };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		message.message_body = std::to_string(i + 1);
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
	}
}

void network::send_message(const message_struct& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet;
	packet << message;
	m_socket.send(packet, ip, port);
}

void network::send_user_name(sf::IpAddress ip, unsigned short port) {
	message_struct response = { m_user_id,id_player_name,m_player_name };
	send_message(response, ip, port);
}

void network::send_all_player_names() {
	std::string body = "";
	for (int i = 0; i < m_player_names.size(); i++) {
		body += m_player_names[i];
		body += m_separation_marker;
	}
	message_struct message = { m_user_id,id_all_player_names,body };
	for (int i = 0; i < m_valid_connections.size(); i++) {
		send_message(message, m_valid_connections[i].first, m_valid_connections[i].second);
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

void network::disconnect_player(int id) {
	m_valid_connections.erase(m_valid_connections.begin()+id-1);
	for (int i = id; i < m_player_names.size()-1; i++) {
		m_player_names[i] = m_player_names[i + 1];
	}
	m_player_names[m_max_players - 1] = m_empty_name;
	if (m_valid_connections.size() > 0) {
		send_all_player_names();
		reassign_id_of_existing_users();
	}
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


