#pragma once
#include <engine.h>
#include "network_message.h"
#include "SFML/Network.hpp"

class server {
public:
	server(unsigned short server_port, int max_players);
	~server();

	void on_update(const engine::timestep& time_step);

	void receive_messages();
	void launch_server(std::string player_name);
	void close_server();
	void start_game();
	void send_tank_state(network_message::object_states& tank_state);

	bool is_active() { return m_is_active; }
	bool all_players_ready() { return m_all_players_ready; }
	bool player_in_game_disconnected() { return m_player_in_game_disconnected; }
	void object_states_sent();
	std::vector<std::string> get_player_names() { return m_player_names; }
	int get_number_of_players() { return m_valid_connections.size() + 1; }
	std::vector <network_message::object_states> get_received_tank_states() { return m_received_tank_states; }
	void erase_received_tank_states(int entries_to_delete);

	static engine::ref<server> create(unsigned short server_port, int max_players);

private:
	void send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port);
	void retry_send(int id);

	void process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void switch_client_connection_step(int id, int step);
	void respond_to_join_request(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void assign_id_to_new_user();
	void reassign_id_of_existing_users();
	void send_all_player_names();
	void reset_player_names();
	void disconnect_player(int id);
	void start_sync_response();
	void store_tank_state(const network_message::message& message);
	void kick(const sf::IpAddress& ip, const unsigned short& port);
	void respond_to_ping(const sf::IpAddress& ip, const unsigned short& port);

	sf::Packet write_to_sfml_packet(const network_message::message& message);
	network_message::message read_message_from_sfml_packet(sf::Packet& packet);

	int															m_max_players;
	int															m_user_id;
	int															m_max_reconnect_attempts;
	unsigned short												m_server_port;
	std::string													m_empty_name;
	std::string													m_player_name;

	bool														m_accepting_clients;
	bool														m_is_active;
	bool														m_is_in_game;
	bool														m_all_players_ready;
	bool														m_player_in_game_disconnected;

	std::string													m_public_ip_address;
	std::string													m_local_ip_address;
	std::vector < std::pair <sf::IpAddress, unsigned short>>	m_valid_connections;
	std::vector <std::string>									m_player_names;
	std::vector <int>											m_client_connection_steps;
	sf::UdpSocket												m_socket;

	//first - timer, second - number of reconnect attempts
	std::vector<std::pair<float, int>>							m_client_timeout_info;

	std::vector<network_message::object_states>					m_received_tank_states;

	std::thread*												m_communication_thread;

	static const int											step_request_name = 1;
	static const int											step_in_lobby = 2;
	static const int											step_start_sync_request = 3;
	static const int											step_start_sync_confirmed = 4;
	static const int											step_in_game = 5;

};
