#pragma once
#include <engine.h>
#include "SFML/Network.hpp"

class network {
public:

	network();
	~network();

	struct message_struct {
		sf::Uint8 sender_id;
		sf::Uint8 message_id;
		std::string message_body;
	};

	void recieve_messages();
	void create_server(std::string player_name);
	void join_server(sf::IpAddress ip_address, unsigned short port);
	void leave_server();

	void on_update(const engine::timestep& time_step);

	static engine::ref<network> create();

	void set_user_id(int id) { m_user_id = id; }
	int	 get_user_id() { return m_user_id; }

	void set_port(unsigned short port) { m_port = port; }
	unsigned short get_port() { return m_port; }

	void set_is_host(bool state) { m_is_host = state; }
	bool get_is_host() { return m_is_host; }

	std::string get_player_name(int index) { return m_player_names[index]; }

private:

	void send_message(const message_struct& message, const sf::IpAddress& ip, const unsigned short& port);

	void assign_new_user_id();
	void send_all_player_names();
	void reset_player_names();
	std::vector<std::string> split_string(const std::string& string);

	int m_max_players;
	int m_user_id;
	unsigned short m_port;
	std::string m_empty_name;
	std::string m_player_name;
	char m_separation_marker;

	std::string m_public_ip_address;
	std::vector <sf::IpAddress> m_new_ip_address_queue;
	std::vector <sf::IpAddress> m_session_ip_addresses;
	std::vector <std::string> m_player_names;
	
	bool m_is_host;
	bool m_accepting_clients;
	bool m_is_active;

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	void process_message(const message_struct& message, const sf::IpAddress& sender, const unsigned short& port);

	static const int id_request_join = 1;
	static const int id_request_join_response = 2;
	static const int id_user_id_assignment = 3;
	static const int id_player_name = 4;
	static const int id_all_player_names = 5;
	static const int id_ping = 6;
};
