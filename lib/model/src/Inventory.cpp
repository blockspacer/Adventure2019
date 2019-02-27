//
// Created by jnhkm on 2019-02-23.
//

#include "Inventory.h"

using model::Inventory;

namespace model {
    Inventory::Inventory() {}

    std::map<Object, int> Inventory::getMappedInventory() {
        return this->inventory;
    }

    std::vector<Object> Inventory::getVectorInventory(){
        std::vector<Object> container;
        container.reserve(this->inventory.size());

        for (auto const& [key, value] : this->inventory) {
            for (int i = 0; i < value; i++) {
                container.push_back(key);
            }
        }

        return container;
    }

    void Inventory::mapInventory(std::vector<Object> items) {
        this->inventory.clear();
        for (Object item : items) {
            addItemToInventory(item);
        }
    }

    void Inventory::addItemToInventory(const Object &item) {
        if (inventory.count(item) > 0) {
            this->inventory.at(item)++;
        }
        else {
            this->inventory.insert(std::pair<Object, int>(item, 1));
        }
    }

    Object Inventory::removeItemFromInventory(const Object &item) {
        int newQuantity = -1;
        Object temp;

        if (inventory.count(item) > 0) {
            newQuantity = this->inventory.at(item) - 1;
            temp = item;

            if (newQuantity == 0) {
                this->inventory.erase(item);
            }
        }

        return temp;
    }

    bool Inventory::isItemInInventory(const Object &item) {
        return static_cast<bool>(this->inventory.count(item));
    }
}
