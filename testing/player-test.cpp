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
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;


        Player player{152, "hello", "20000"};
        Object item{
            expected_id,
            expected_name,
            expected_short_description,
            expected_long_description,
            expected_keywords,
            expected_slot
        };

        player.addToInventoryItems(item);
        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(player.isItemInInventory(item), true);
    }

    TEST(PlayerTestSuite, canEquipItemFromInventory) {
        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;

        Player player{152, "hello", "20000"};
        Object item{
                expected_id,
                expected_name,
                expected_short_description,
                expected_long_description,
                expected_keywords,
                expected_slot
        };

        player.addToInventoryItems(item);

        ASSERT_EQ(player.isItemInInventory(item), true);

        std::vector<Object> items = player.getInventoryItems();

        player.equipItem(item);

        EXPECT_EQ(player.isItemInInventory(item), false);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), true);
    }

    TEST(PlayerTestSuite, canDropItemFromInventory) {
        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;

        Player player{152, "hello", "20000"};
        Object item{
            expected_id,
            expected_name,
            expected_short_description,
            expected_long_description,
            expected_keywords,
            expected_slot,
        };

        player.addToInventoryItems(item);

        player.dropItemFromInventory(item);

        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(player.isItemInInventory(item), false);
    }

    TEST(PlayerTestSuite, canDropEquippedItem) {
        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;

        Player player{152, "hello", "20000"};
        Object item{
            expected_id,
            expected_name,
            expected_short_description,
            expected_long_description,
            expected_keywords,
            expected_slot
        };

        player.addToInventoryItems(item);

        player.equipItem(item);

        player.dropItemFromEquipped(item.getSlot());

        EXPECT_EQ(player.isItemInInventory(item), false);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), false);
    }

    TEST(PlayerTestSuite, canUnequipItem) {
        int expected_id = 12345;
        std::string expected_name = "The Executioner";
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;

        Player player{152, "hello", "20000"};
        Object item{
            expected_id,
            expected_name,
            expected_short_description,
            expected_long_description,
            expected_keywords,
            expected_slot
        };

        player.addToInventoryItems(item);

        player.equipItem(item);

        player.unequipItem(item.getSlot());

        EXPECT_EQ(player.isItemInInventory(item), true);
        EXPECT_EQ(player.isSlotOccupied(item.getSlot()), false);
    }

    TEST(PlayerTestSuite, canReturnCollectionOfItems) {
        Player player{1294, "Cindy", "iPoopOnPizzas"};
        std::string expected_name = "The Executioner";
        std::string expected_short_description = "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back";
        std::vector<std::string> expected_long_description = {
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back",
                "Assigns the player with the title 'Sumner' and immediately assigns the enemy 2 readings back to back"
        };
        std::vector<std::string> expected_keywords = {
                "Apple",
                "Facebook",
                "Microsoft"
        };
        Slot expected_slot = Slot::Head;

        unsigned int itemsToCreate = 10;
        for (unsigned int i = 0; i < itemsToCreate; i++) {
            Object item{
                    rand()%220,
                    expected_name,
                    expected_short_description,
                    expected_long_description,
                    expected_keywords,
                    expected_slot
            };
            player.addToInventoryItems(item);
        }

        std::vector<Object> items = player.getInventoryItems();

        EXPECT_EQ(items.size(), itemsToCreate);

    }
}