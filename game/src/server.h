#pragma once
#include <engine.h>
#include "network_message.h"
#include "SFML/Network.hpp"

class server {
public:
	server(unsigned short port);
	~server();

	void on_update(const engine::timestep& time_step);

	void recieve_messages();
	void launch_server(std::string player_name);
	void close_server();

	static engine::ref<server> create(unsigned short port);

private:
	void send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port);

	void process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void assign_new_user_id();
	void reassign_id_of_existing_users();
	void send_all_player_names();
	void reset_player_names();
	void disconnect_player(int id);
	std::vector<std::string> split_string(const std::string& string);

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

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	static const int step_request_name = 1;
	static const int step_ready = 2;
	static const int step_in_game = 3;

};
