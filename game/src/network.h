#pragma once
#include <engine.h>
#include "network_message.h"
#include "server.h"
#include "client.h"
#include "SFML/Network.hpp"

class network {
public:

	network();
	~network();

	void create_server(std::string player_name);
	void join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name);
	void leave_server();

	void on_update(const engine::timestep& time_step);

	static engine::ref<network> create();

	bool get_is_active() { return m_is_active; }

	std::string get_public_ip() { return m_public_ip_address; }
	std::string get_local_ip() { return m_local_ip_address; }

	std::string get_player_name(int index);

private:

	int m_max_players;
	unsigned short m_server_port;
	unsigned short m_user_port;
	std::string m_player_name;

	std::string m_public_ip_address;
	std::string m_local_ip_address;
	
	bool m_is_host;
	bool m_is_active;

	engine::ref<server> m_server{};
	engine::ref<client> m_client{};
};
