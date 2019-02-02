//
// Created by jnhkm on 2019-01-25.
//

#include "lib/gtest/gtest.h"
#include "lib/gmock/gmock.h"
#include "Player.h"
#include "Object.h"
#include <stdlib.h>
#include <iostream>

using model::Player;
using model::Slot;
using model::Object;

namespace {
    TEST(PlayerTestSuite, canConstructPlayer) {
        int expected_id = 12345;
        std::string expected_username = "hello";
        std::string expected_password = "zombie";

        Player player{expected_id, expected_username, expected_password};

        EXPECT_EQ(player.getId(), expected_id);
        EXPECT_EQ(player.getUsername(), expected_username);
        EXPECT_EQ(player.getPassword(), expected_password);
    }

    TEST(PlayerTestSuite, canAddItemToInventory) {
        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        Slot expected_slot = Slot::Head;

        Player player{152, "hello", "20000"};
        Object item{expected_id, expected_name, expected_description, {}, {}, expected_slot};

        player.addToInventoryItems(item);
        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(player.isItemInInventory(item), true);
    }

    TEST(PlayerTestSuite, canEquipItemFromInventoryWhenSlotIsEmpty) {
        Player player{152, "hello", "20000"};

        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        Slot expected_slot = Slot::Head;
        Object item{expected_id, expected_name, expected_description, {}, {}, expected_slot};

        player.addToInventoryItems(item);

        ASSERT_EQ(player.isItemInInventory(item), true);

        std::vector<Object> items = player.getInventoryItems();

        player.equipItem(item);

        EXPECT_EQ(player.isItemInInventory(item), false);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), true);
    }

    TEST(PlayerTestSuite, canEquipItemFromInventoryWhenSlotIsOccupied) {
        Player player{152, "hello", "20000"};

        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        Slot expected_slot = Slot::Head;
        Object item{expected_id, expected_name, "", {}, {}, expected_slot};

        int expected_equipped_id = 15;
        std::string expected_equipped_name = "The Punisher";
        Object equippedItem{expected_equipped_id, expected_equipped_name, "", {}, {}, expected_slot};

        player.addToInventoryItems(equippedItem);
        player.equipItem(item);

        player.addToInventoryItems(item);

        std::vector<Object> items = player.getInventoryItems();

        player.equipItem(item);

        EXPECT_FALSE(player.isItemInInventory(item));
        EXPECT_TRUE(player.isItemInInventory(equippedItem));
        EXPECT_TRUE(player.isSlotOccupied(item.getSlot()));
    }

    TEST(PlayerTestSuite, canDropItemFromInventory) {
        Player player{152, "hello", "20000"};

        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        Slot expected_slot = Slot::Head;
        Object item{expected_id, expected_name, expected_description, {}, {}, expected_slot};

        player.addToInventoryItems(item);

        player.dropItemFromInventory(item);

        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(player.isItemInInventory(item), false);
    }

    TEST(PlayerTestSuite, canDropEquippedItem) {
        Player player{152, "hello", "20000"};

        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        Slot expected_slot = Slot::Head;
        Object item{expected_id, expected_name, expected_description, {}, {}, expected_slot};

        player.addToInventoryItems(item);

        player.equipItem(item);

        player.dropItemFromEquipped(item.getSlot());

        EXPECT_EQ(player.isItemInInventory(item), false);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), false);
    }

    TEST(PlayerTestSuite, canUnequipItem) {
        Player player{152, "hello", "20000"};

        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        Slot expected_slot = Slot::Head;
        Object item{expected_id, expected_name, expected_description, {}, {}, expected_slot};

        player.addToInventoryItems(item);

        player.equipItem(item);

        player.unequipItem(item.getSlot());

        EXPECT_EQ(player.isItemInInventory(item), true);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), false);
    }

    TEST(PlayerTestSuite, canReturnCollectionOfItems) {
        Player player{1294, "Cindy", "iPoopOnPizzas"};

        unsigned int itemsToCreate = 10;
        for (unsigned int i = 0; i < itemsToCreate; i++) {
            Object item{rand()%220, "test", "test", {}, {}, Slot::Head};
            player.addToInventoryItems(item);
        }

        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(items.size(), itemsToCreate);

    }
}