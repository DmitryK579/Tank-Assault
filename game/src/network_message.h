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

	static constexpr int id_request_join = 1;
	static constexpr int id_request_join_response = 2;
	static constexpr int id_user_id_assignment = 3;
	static constexpr int id_player_name = 4;
	static constexpr int id_all_player_names = 5;
	static constexpr int id_ping = 6;
	static constexpr int id_leave = 7;
	static constexpr int id_kick = 8;


};
