#include "network_message.h"

// Split a string by a marker and return the vector of those split strings.
std::vector<std::string> network_message::split_message(const std::string& string) {
	std::vector<std::string> string_vector;
	std::stringstream stringstream(string);
	std::string segment;
	while (std::getline(stringstream, segment, separation_symbol)) {
		string_vector.push_back(segment);
	}
	return string_vector;
}
