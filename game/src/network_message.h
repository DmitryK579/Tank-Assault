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

	struct object_states {
		int object_id;
		float x_coords;
		float y_coords;
		char movement_command;
		bool fire;
		bool connected;
	};

	static std::vector<std::string> split_message(const std::string& string);

	static constexpr char separation_symbol = ';';

	static constexpr int id_request_join = 1; // Client message
	static constexpr int id_request_join_response = 2; // Server message
	static constexpr int id_user_id_request = 3; // Client message
	static constexpr int id_user_id_assignment = 4; // Server message
	static constexpr int id_player_name_request = 5; // Server message
	static constexpr int id_player_name = 6; // Client message
	static constexpr int id_all_player_names_request = 7; // Client message
	static constexpr int id_all_player_names = 8; // Server message
	static constexpr int id_ping = 9; // Common message
	static constexpr int id_ping_return = 10;
	static constexpr int id_leave = 11; // Common message
	static constexpr int id_kick = 12; // Server message
	static constexpr int id_start_game = 13; // Server message
	static constexpr int id_start_sync = 14; // Client message
	static constexpr int id_start_sync_confirmed = 15; // Server message
	static constexpr int id_tank_state = 16; // Common message


};
