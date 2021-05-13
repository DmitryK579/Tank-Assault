#pragma once
#include <engine.h>
#include "SFML/Network.hpp"

class network_message {
public:
	struct message {
		sf::Uint8 sender_id;
		sf::Uint8 message_id;
		std::string message_body;
	};

	static std::vector<std::string> split_message(const std::string& string);

	static constexpr char separation_symbol = ';';

	static constexpr int id_request_join = 1; // Client message
	static constexpr int id_request_join_response = 2; // Server message
	static constexpr int id_user_id_assignment = 3; // Server message
	static constexpr int id_player_name = 4; // Client message
	static constexpr int id_all_player_names = 5; // Server message
	static constexpr int id_ping = 6; // Common message
	static constexpr int id_leave = 7; // Common message
	static constexpr int id_kick = 8; // Server message
	static constexpr int id_start_game = 9; // Server message
	static constexpr int id_start_sync = 10; // Client message
	static constexpr int id_start_sync_confirmed = 11; // Server message
	static constexpr int id_tank_state = 12; // Common message


};
