#include "client.h"

client::client(unsigned short server_port) {
	// Initialize class variables
	m_server_port = server_port;
	m_server_ip = "127.0.0.1";
	m_user_port = 0;
	m_user_id = 255;
	m_player_name = "!";
	m_is_active = false;
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
			break;
		}
		network_message::message message = read_message_from_sfml_packet(recieved_packet);
		process_message(message, sender, port);
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
				m_connection_step = step_request_id;
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
			if (m_connection_step != step_ready) {
				send_user_name();
				m_connection_step = step_request_all_names;
			}
			break;
		}

		// Recieved name of every player from server
		case network_message::id_all_player_names:
		{
			std::vector<std::string> all_names = network_message::split_message(message.message_body);
			m_player_names = all_names;
			m_connection_step = step_ready;
			break;
		}

		// Recieved ping
		case network_message::id_ping:
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
		}
	}
}

// Send message to target server
void client::send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port) {
	sf::Packet packet = write_to_sfml_packet(message);
	m_socket.send(packet, ip, port);
}

// Send the player's name to server
void client::send_user_name() {
	network_message::message response = { m_user_id,network_message::id_player_name,m_player_name };
	send_message(response, m_server_ip, m_server_port);
}

// Attempt to join an existing server by given IP. Also sets the client's port to listen for replies.
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

// Leave the server.
void client::leave_server() {
	if (m_is_active) {
		network_message::message message = { m_user_id, network_message::id_leave, "" };
		send_message(message, m_server_ip, m_server_port);
		m_connection_step = 0;
		m_user_id = 255;
		m_is_active = false;
		m_socket.unbind();
		m_server_ip = "127.0.0.1";
		m_player_names.clear();
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
