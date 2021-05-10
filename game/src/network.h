#pragma once
#include <engine.h>
#include "SFML/Network.hpp"

class network {
public:
	network();
	~network();

	void process_communication();
	void create_server();
	void join_server();
	void leave_server();

	void on_update(const engine::timestep& time_step);

	static engine::ref<network> create();

	void set_user_id(int id) { m_user_id = id; }
	int	 get_user_id() { return m_user_id; }

	void set_port(int port) { m_port = port; }
	int get_port() { return m_port; }

	void set_is_host(bool state) { m_is_host = state; }
	bool get_is_host() { return m_is_host; }

private:
	struct message {
		sf::Uint8 message_id;
		std::string message_body;
	};

	int m_user_id;
	int m_port;
	std::vector<std::string> m_session_ip_addresses;

	bool m_is_host;
	bool m_is_active;

	sf::UdpSocket m_socket;

	std::mutex mutex;
	std::thread communication_thread;
};
