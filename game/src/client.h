#pragma once
#include <engine.h>
#include "network_message.h"
#include "SFML/Network.hpp"

class client {
public:
	client(unsigned short server_port);
	~client();

	void on_update(const engine::timestep& time_step);

	void recieve_messages();
	void join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name);
	void leave_server();

	static engine::ref<client> create(unsigned short server_port);

private:
	void send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port);

	void send_user_name();

	int m_user_id;
	int m_connection_step;
	unsigned short m_server_port;
	sf::IpAddress m_server_ip;
	unsigned short m_user_port;
	std::string m_player_name;
	char m_separation_marker;

	std::string m_public_ip_address;
	std::string m_local_ip_address;
	std::vector <std::string> m_player_names;

	bool m_is_active;

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	void process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);

	static const int step_request_connect = 1;
	static const int step_request_id = 2;
	static const int step_request_all_names = 3;
	static const int step_ready = 4;
	static const int step_in_game = 5;
};
