//
// Created by arehal on 1/17/19.
//

#include <iostream>

#include "World.h"

using model::World;
using model::Player;

namespace model {

    World::World()
      : areas({})
        { }

    World::World(std::vector<Area> areas)
      : areas(std::move(areas))
        { }

    //getters and setters
    std::vector<Area> World::getAreas() {
      return areas;
    }

    void World::setAreas(std::vector<Area> areas) {
      this->areas = std::move(areas);
    }

    void World::addArea(Area area) {
      areas.push_back(std::move(area));
    }

    std::map<model::ID, Player> World::getUserMap(){
        return this->usersMap;
    }

    void World::insertUser(Player player){
        usersMap.insert(std::pair<model::ID,Player>(player.getId(), player));
    }

//    std::map<model::ID, Area> World::getAreaMap(){
//        return this->areasMap;
//    }

    void World::printUsers() {
        std::cout << "Users:" << std::endl;

        for (auto& [key, value]: usersMap) {
            std::cout << value.getId() << ". " <<  value.getUsername() << std::endl;
        }
    }
};