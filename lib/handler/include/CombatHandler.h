//
// Created by jnhkm on 2019-03-10.
//

#ifndef COMBATHANDLER_H
#define COMBATHANDLER_H

#include "CombatLogic.h"
#include "Character.h"

using logic::CombatLogic;
using model::Character;

namespace handler {
    class CombatHandler {
    private:
        std::vector<CombatState> active_combats;

        CombatLogic logic;

        constexpr static int BASE_DAMAGE = 10;
        constexpr static int BASE_HEAL = 5;
    public:
        CombatHandler();

        /**
         *
         * @param attacker that will enter Combat
         * @param defender that will enter Combat
         */
        void enterCombat(const Character &attacker, const Character &defender);

        /**
         * The arguments are attacker and defender for name consistency sake,
         * it doesn't matter in which order the Characters are passed in
         * @param attacker: First character that is part of the Combat
         * @param defender: Second character that is part of the Combat
         */
        void exitCombat(const Character &attacker, const Character &defender);

        /**
         *
         * @param attacker is the person dealing damage
         * @param defender is the perosn being dealt the damage
         */
        void attack(const Character &attacker, Character &defender);

        /**
         *
         * @param healer that will use their ability to heal someone
         * @param target is the person being healed
         */
        void heal(const Character &healer, Character &target);

        /**
         *
         * @param attacker that is ataccking the target
         * @param defender that is taking the damage
         * @return True if there is an active combat state, or False if there isn't
         */
        bool isInCombat(const Character &attacker, const Character &defender);
    };
}



#endif //COMBATHANDLER_H
