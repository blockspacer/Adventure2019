//
// Created by Brittany Ryan on 2019-01-21.
//

#ifndef RESET_H
#define RESET_H

#include <string>
#include "Types.h"
#include "json.hpp"

using json = nlohmann::json;

namespace model {
    /**
    *  @class Reset
    *
    *  @brief A class designed to represent reset information.
    *
    *  The Reset class contains all information needed to reset
    *  objects and npcs into rooms
    */
    class Reset {

    private:
        std::string action;
        model::ID id;
        int limit;
        int room;
        int slot;

        friend std::ostream& operator<<(std::ostream& os, const Reset& r);

    public:
      	//constructors
      	Reset();
        Reset(std::string action, model::ID id, int limit, int room, int slot);

        //getters and setters
        std::string getAction() const;
        model::ID getId() const;
        int getLimit() const;
        int getRoom() const;
        int getSlot() const;
        void setAction(const std::string &action);
        void setId(const model::ID &id);
        void setLimit(int limit);
        void setRoom(int room);
        void setSlot(int slot);
    };

    inline void from_json(const json &j, Reset &r) {
        r.setId(j.at("id").get<model::ID>());
        r.setAction(j.at("action").get<std::string>());
        if(j.find("limit") != j.end()) {
            r.setLimit(j.at("limit").get<int>());
        }
        if(j.find("room") != j.end()) {
            r.setRoom(j.at("room").get<int>());
        }
        if(j.find("slot") != j.end()){
            r.setSlot(j.at("slot").get<int>());
        }

    }

}

#endif //RESET_H
