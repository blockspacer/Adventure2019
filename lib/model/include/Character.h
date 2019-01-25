//
// Created by jnhkm on 2019-01-16.
//

#ifndef CHARACTER_H
#define CHARACTER_H

#include "Object.h"
#include "Role.h"

namespace model {
    /**
    *  @struct Character
    *
    *  @brief A struct describing a character.
    *
    *  The Character struct contains all necessary information
    *  that must be contained in a character object in the application.
    */

    struct Character {
    public:
        explicit Character(int id);

        int getId() const;

        void setId(int id);

        double getHealth() const;

        void setHealth(double health);

        static constexpr double STARTING_HEALTH = 100.00;

        static constexpr double MAX_HEALTH = 100.00;

    private:
        int id;

        Role role;

        double health;
    };
}

#endif //CHARACTER_H