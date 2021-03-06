//
// Created by jnhkm on 2019-03-19.
//

#include "Slot.h"

using model::Slot;

namespace model {
    std::unordered_map<std::string, Slot> slots = {
            {"head", Slot::Head},
            {"shoulders", Slot::Shoulders},
            {"back", Slot::Back},
            {"chest", Slot::Chest},
            {"hands", Slot::Hands},
            {"legs", Slot::Legs},
            {"feet", Slot::Feet},
            {"weapon", Slot::Weapon},
            {"misc", Slot::Misc},
    };

    Slot getSlotFromString(std::string slot) {
        Slot result;

        std::transform(slot.begin(), slot.end(), slot.begin(), ::tolower);

        try {
            result = slots.at(slot);
        } catch (const std::out_of_range &e) {
            result = Slot::Misc;
        }

        return result;
    }

    std::string getStringFromSlot(const Slot &slot) {
        std::string result = "misc";

        for (const auto &it : slots) {
            if (it.second == slot) {
                result = it.first;
                break;
            }
        }

        return result;
    }

    Slot getSlotFromJsonInt(int slot) {
        switch (slot) {
            case 0:
                return Slot::Weapon;

            case 1:
                return Slot::Misc;

            case 3:
                return Slot::Back;

            case 5:
                return Slot::Chest;

            case 6:
                return Slot::Head;

            case 8:
                return Slot::Feet;

            case 12:
                return Slot::Back;

            case 16:
                return Slot::Weapon;

            default:
                return Slot::Misc;
        }
    }
}
