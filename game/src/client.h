#pragma once
#include <engine.h>
#include "network_message.h"
#include "SFML/Network.hpp"

class client {
public:
	client(unsigned short server_port);
	~client();

	void on_update(const engine::timestep& time_step);

	void receive_messages();
	void join_server(sf::IpAddress ip_address, unsigned short port, std::string player_name);
	void leave_server();
	void send_tank_state(network_message::object_states& tank_state);

	bool is_active() { return m_is_active; }
	bool all_players_ready() { return m_all_players_ready; }
	std::vector<std::string> get_player_names() { return m_player_names; }
	int get_id() { return m_user_id; }
	int get_number_of_players();

	std::vector <network_message::object_states> get_received_tank_states() { return m_received_tank_states; }
	void erase_received_tank_states(int entries_to_delete);

	static engine::ref<client> create(unsigned short server_port);

private:

	void process_message(const network_message::message& message, const sf::IpAddress& sender, const unsigned short& port);
	void send_message(const network_message::message& message, const sf::IpAddress& ip, const unsigned short& port);

	void send_user_name();
	void game_start_response();
	void retry_send();
	void respond_to_ping(const sf::IpAddress& ip, const unsigned short& port);
	void store_tank_state(const network_message::message& message);

	void switch_connection_step(int step);

	sf::Packet write_to_sfml_packet(const network_message::message& message);
	network_message::message read_message_from_sfml_packet(sf::Packet& packet);

	int												m_user_id;
	int												m_connection_step;
	int												m_max_reconnection_attempts;
	int												m_current_reconnection_attempt;
	float											m_timeout_timer;
	bool											m_is_active;
	bool											m_all_players_ready;

	sf::IpAddress									m_server_ip;
	unsigned short									m_server_port;
	unsigned short									m_user_port;
	sf::UdpSocket									m_socket;

	std::string										m_player_name;
	std::string										m_public_ip_address;
	std::string										m_local_ip_address;
	std::vector <std::string>						m_player_names;
	std::vector<network_message::object_states>		m_received_tank_states;

	std::thread*									m_communication_thread;

	static const int								step_request_connect = 1;
	static const int								step_request_id = 2;
	static const int								step_request_all_names = 3;
	static const int								step_in_lobby = 4;
	static const int								step_start_sync_check = 5;
	static const int								step_in_game = 6;
};
