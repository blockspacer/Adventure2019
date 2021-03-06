//
// Created by arehal on 1/29/19.
//

#ifndef WEBSOCKETNETWORKING_FILEPARSER_H
#define WEBSOCKETNETWORKING_FILEPARSER_H

#include <string>

#include "json.hpp"
#include "World.h"

using model::World;
using model::Player;

constexpr auto JSON_EXTENSION = ".json";
constexpr auto SAVE_FILE_PATH = "lib/data/saveFile.json";
constexpr auto REGISTERED_USERS_PATH = "lib/data/users.json";

namespace DataManager {

    enum FileType {JSON};

    Area ParseDataFile(const std::string& filePath);
    std::vector<Player> ParseUsersFile(const std::string& filePath);
    std::vector<Area> ParseWorldFile(const std::string& filePath);
    void writeJson(Json json, std::string filePath);
    void writeWorldToFile(World& world, FileType type);
    void saveRegisteredUsers(const std::map<model::ID, Player>& players);
    std::vector<Player> loadRegisteredPlayers();
}
#endif //WEBSOCKETNETWORKING_FILEPARSER_H
