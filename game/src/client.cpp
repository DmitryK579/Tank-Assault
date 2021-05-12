#include "client.h"

client::client(unsigned short server_port) {
	m_server_port = server_port;
	m_server_ip = "127.0.0.1";
	m_user_port = 0;
	m_user_id = 255;
	m_player_name = "!";
	m_is_active = false;
}

client::~client() {

}

void client::recieve_messages() {
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

void client::process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port) {

	if (message.sender_id != 255){
		switch (message.message_id) {
		case network_message::id_request_join_response:
			if (message.message_body == "y") {
				m_server_ip = sender;
				m_connection_step = step_request_id;
			}
			else {
				leave_server();
			}
			break;

		case network_message::id_user_id_assignment:
		{
			m_user_id = stoi(message.message_body);
			if (m_connection_step != step_ready) {
				send_user_name();
				m_connection_step = step_request_all_names;
			}
			break;
		}

		case network_message::id_all_player_names:
		{
			std::vector<std::string> all_names = network_message::split_message(message.message_body);
			m_player_names = all_names;
			m_connection_step = step_ready;
			break;
		}
		case network_message::id_ping:
			break;

		case network_message::id_leave:
			if (message.sender_id == 0) {
				m_server_ip = "127.0.0.1";
				leave_server();
			}
			break;

		case network_message::id_kick:
			break;
		}
	}
}

void client::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet = write_to_sfml_packet(message);
	m_socket.send(packet, ip, port);
}

void client::send_user_name() {
	network_message::message response = { m_user_id,network_message::id_player_name,m_player_name };
	send_message(response, m_server_ip, m_server_port);
}

void client::join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name) {
	m_player_name = player_name;
	m_user_port = port;
	m_socket.bind(port);
	m_is_active = true;
	m_connection_step = step_request_connect;
	m_communication_thread = new std::thread(&client::recieve_messages, this);
	network_message::message message = { m_user_id,network_message::id_request_join,"" };
	send_message(message, ip_address, m_server_port);
}

void client::leave_server() {
	network_message::message message = { m_user_id, network_message::id_leave, "" };
	send_message(message, m_server_ip, m_server_port);
	m_connection_step = 0;
	m_user_id = 255;
	m_is_active = false;
	m_socket.unbind();
	m_server_ip = "127.0.0.1";
	m_player_names.clear();
}

sf::Packet client::write_to_sfml_packet(const network_message::message& message) {
	sf::Packet packet;
	packet << message.sender_id << message.message_id << message.message_body;
	return packet;
}

network_message::message client::read_message_from_sfml_packet(sf::Packet& packet) {
	network_message::message message;
	packet >> message.sender_id >> message.message_id >> message.message_body;
	return message;
}

engine::ref<client> client::create(unsigned short server_port)
{
	return std::make_shared<client>(server_port);
}
