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
	void join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name);
	void leave_server();

	void on_update(const engine::timestep& time_step);

	static engine::ref<network> create();

	void set_user_id(int id) { m_user_id = id; }
	int	 get_user_id() { return m_user_id; }

	void set_user_port(unsigned short port) { m_user_port = port; }
	unsigned short get_port() { return m_user_port; }

	void set_is_host(bool state) { m_is_host = state; }
	bool get_is_host() { return m_is_host; }

	bool get_is_active() { return m_is_active; }

	std::string get_public_ip() { return m_public_ip_address; }

	std::string get_player_name(int index) { return m_player_names[index]; }

private:

	void send_message(const message_struct& message, const sf::IpAddress& ip, const unsigned short& port);

	void assign_new_user_id();
	void reassign_id_of_existing_users();
	void send_user_name(sf::IpAddress ip, unsigned short port);
	void send_all_player_names();
	void reset_player_names();
	void disconnect_player(int id);
	std::vector<std::string> split_string(const std::string& string);

	int m_max_players;
	int m_user_id;
	int m_connection_step;
	unsigned short m_server_port;
	unsigned short m_user_port;
	std::string m_empty_name;
	std::string m_player_name;
	char m_separation_marker;


	std::string m_public_ip_address;
	std::string m_local_ip_address;
	std::vector < std::pair <sf::IpAddress,unsigned short>> m_new_client_queue;
	std::vector < std::pair <sf::IpAddress, unsigned short>> m_valid_connections;
	std::vector <std::string> m_player_names;
	
	bool m_is_host;
	bool m_accepting_clients;
	bool m_is_active;

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	void process_message(const message_struct& message, const sf::IpAddress& sender, const unsigned short& port);

	static const int step_request_connect = 100;
	static const int step_request_id = 101;
	static const int step_request_name = 102;
	static const int step_request_all_names = 103;
	static const int step_ready = 104;
	static const int step_in_game = 105;

	static const int id_request_join = 1;
	static const int id_request_join_response = 2;
	static const int id_user_id_assignment = 3;
	static const int id_player_name = 4;
	static const int id_all_player_names = 5;
	static const int id_ping = 6;
	static const int id_leave = 7;
	static const int id_kick = 8;
};
