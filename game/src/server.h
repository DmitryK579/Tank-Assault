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
	void start_game();

	bool is_active() { return m_is_active; }
	bool all_players_ready() { return m_all_players_ready; }
	std::vector<std::string> get_player_names() { return m_player_names; }
	int get_number_of_players() { return m_valid_connections.size() + 1; }

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
	void start_sync_response();

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
	bool m_all_players_ready;

	sf::UdpSocket m_socket;

	std::mutex m_mutex;
	std::thread* m_communication_thread;

	static const int step_request_name = 1;
	static const int step_in_lobby = 2;
	static const int step_start_sync_request = 3;
	static const int step_start_sync_confirmed = 4;
	static const int step_in_game = 5;

};
