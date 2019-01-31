/*
* Reset.h
*
* Class Description: A class designed to represent reset information.
*
* Created on: January 21, 2019
* Author: Brittany Ryan, 301217765
*/

#ifndef WEBSOCKETNETWORKING_RESET_H
#define WEBSOCKETNETWORKING_RESET_H

#include <string>
#include "Types.h"

namespace model {

    class Reset {

    private:
        std::string action;
        model::ID id;
        int limit;
        int room;
        int slot;

    public:
      	//constructors
      	Reset();
        Reset(std::string action, model::ID id, int limit, int room, int slot);

        //getters and setters
        std::string getAction();
        model::ID getId();
        int getLimit();
        int getRoom();
        int getSlot();
        void setAction(std::string action);
        void setId(model::ID id);
        void setLimit(int limit);
        void setRoom(int room);
        void setSlot(int slot);

    };
}

#endif