#pragma once
#include <engine.h>
#include "network_message.h"
#include "SFML/Network.hpp"

class server {
public:
	server(unsigned short server_port, int max_players);
	~server();

	void on_update(const engine::timestep& time_step);

	void recieve_messages();
	void launch_server(std::string player_name);
	void close_server();

	std::vector<std::string> get_player_names() { return m_player_names; }

	static engine::ref<server> create(unsigned short server_port, int max_players);

private:
	void send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port);

	void process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void assign_id_to_new_user();
	void reassign_id_of_existing_users();
	void send_all_player_names();
	void reset_player_names();
	void disconnect_player(int id);

	sf::Packet write_to_sfml_packet(const network_message::message& message);
	network_message::message read_message_from_sfml_packet(sf::Packet& packet);

	int m_max_players;
	int m_user_id;
	unsigned short m_server_port;
	std::string m_empty_name;
	std::string m_player_name;

	std::string m_public_ip_address;
	std::string m_local_ip_address;
	std::vector < std::pair <sf::IpAddress, unsigned short>> m_valid_connections;
	std::vector <std::string> m_player_names;
	std::vector <int> m_client_connection_steps;

	bool m_accepting_clients;
	bool m_is_active;
	bool m_is_in_game;

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	static const int step_request_name = 1;
	static const int step_ready = 2;
	static const int step_in_game = 3;

};
