//
// Created by jnhkm on 2019-03-11.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdlib.h>
#include "CombatHandler.h"
#include "Player.h"
#include "NPC.h"

using handler::CombatHandler;
using handler::AccountHandler;
using handler::WorldHandler;
using model::Player;
using model::NPC;
using model::World;

constexpr Connection CLIENT_A = {100};
constexpr Connection CLIENT_B = {200};

constexpr auto USERNAME_A = "Able";
constexpr auto USERNAME_B = "Baker";
constexpr auto VALID_PASSWORD_STRING = "Valid Pass";

constexpr model::ID TEST_ROOM_1_ID = 1000;
constexpr model::ID TEST_ROOM_2_ID = 2000;
constexpr auto TEST_ROOM_1_EXIT = "east";
constexpr auto TEST_ROOM_2_EXIT = "west";

constexpr model::ID NPC_A_ID = 100;
constexpr model::ID NPC_B_ID = 200;
constexpr auto NPC_A_KEYWORD = "alpha";
constexpr auto NPC_B_KEYWORD = "bravo";

constexpr auto EXPECTED_ROUND_CYCLES = 7;

namespace {
    class CombatHandlerTestSuite : public ::testing::Test {
    protected:
        AccountHandler accountHandler;
        WorldHandler worldHandler;
        CombatHandler combatHandler{accountHandler, worldHandler};
        World world;

        virtual void SetUp() override {
            // Login client A
            accountHandler.processLogin(CLIENT_A);
            accountHandler.processLogin(CLIENT_A, USERNAME_A);
            accountHandler.processLogin(CLIENT_A, VALID_PASSWORD_STRING);
            accountHandler.setRoomIdByClient(CLIENT_A, TEST_ROOM_1_ID);

            // Login client B
            accountHandler.processLogin(CLIENT_B);
            accountHandler.processLogin(CLIENT_B, USERNAME_B);
            accountHandler.processLogin(CLIENT_B, VALID_PASSWORD_STRING);
            accountHandler.setRoomIdByClient(CLIENT_B, TEST_ROOM_1_ID);

            // Construct World
            Room room1 = {TEST_ROOM_1_ID, "Test room 1", {"Test room 1 description"}};
            std::string uniqueIdA = std::to_string(NPC_A_ID) + std::to_string(TEST_ROOM_1_ID);
            NPC npcA {
                NPC_A_ID,
                std::stoi(uniqueIdA),
                {NPC_A_KEYWORD},
                {"Long description."},
                "Test NPC 1",
                {"Interaction text."}
            };

            std::string uniqueIdB = std::to_string(NPC_B_ID) + std::to_string(TEST_ROOM_1_ID);
            NPC npcB {
                NPC_B_ID,
                std::stoi(uniqueIdB),
                {NPC_B_KEYWORD},
                {"Long description."},
                "Test NPC 1",
                {"Interaction text."}
            };

            room1.addNPC(npcA);
            room1.addNPC(npcB);
            room1.addDoor({TEST_ROOM_1_EXIT, TEST_ROOM_2_ID, {"Goes to Test room 2"}});

            Room room2 = {TEST_ROOM_2_ID, "Test room 2", {"Test room 2 description"}};
            room2.addDoor({TEST_ROOM_2_EXIT, TEST_ROOM_1_ID, {"Goes to Test room 1"}});

            Area area = Area("Testing area");
            area.addRoom(room1);
            area.addRoom(room2);

            world.addArea(area);
            worldHandler.setWorld(world);
        }
    };

    /*
     *  Cases to test:
     *  1.  Message on attacking with no keyword
     *  2.  Message on attacking with invalid keyword
     *  3.  Can attack an NPC while player and NPC are not in combat
     *  4.  Can attack NPC while player and NPC are in combat
     *  5.  Cannot attack NPC while player is already in combat with another NPC
     *  6.  Cannot attack NPC while NPC is already in combat with another player
     *  7.  Message on fleeing while not in combat
     *  8.  Can flee to random room while in combat
     *  9.  Can flee in room with no doors while in combat
     *  10. Can get opponent ID
     *  11. Can replace player with decoy in combat
     *  12. Can remove player from combat on client logout
     *  13. Combat rounds end after a number of game cycles
     *  14. Can exit combat
     */


    TEST_F(CombatHandlerTestSuite, notifyIfNoKeyword) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        auto result = combatHandler.attack(CLIENT_A, "");

        std::ostringstream expected;
        expected << "You need to specify the name of the person (NPC) to attack.\n";

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));
        EXPECT_EQ(expected.str(), result);
    }


    TEST_F(CombatHandlerTestSuite, notifyIfInvalidKeyword) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        std::string invalidKeyword = "invalidKeyword";
        auto result = combatHandler.attack(CLIENT_A, invalidKeyword);

        std::ostringstream expected;
        expected << "Invalid keyword.\n";

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));
        EXPECT_EQ(expected.str(), result);
    }


    TEST_F(CombatHandlerTestSuite, canAttackNpcWithKeywordWhileNeitherAreInCombat) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));
    }


    TEST_F(CombatHandlerTestSuite, canAttackNpcWithKeywordWhileBothAreInCombat) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);
        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));

        EXPECT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));
    }


    TEST_F(CombatHandlerTestSuite, cannotAttackWhileInCombatWithAnotherNpc) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        auto result = combatHandler.attack(CLIENT_A, NPC_B_KEYWORD);

        std::ostringstream expected;
        expected << "You are already engaged in combat with someone else!\n";

        EXPECT_EQ(expected.str(), result);
    }


    TEST_F(CombatHandlerTestSuite, cannotAttackWhileNPCInCombatWithAnotherPlayer) {
        auto playerA = accountHandler.getPlayerByClient(CLIENT_A);
        auto playerB = accountHandler.getPlayerByClient(CLIENT_B);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(playerB.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(playerB.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_B, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(playerB.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(playerB.getId(), npcUniqueId));

        auto result = combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        std::ostringstream expected;
        expected << NPC_A_KEYWORD << " is already engaged in combat with someone else!\n";

        ASSERT_FALSE(combatHandler.isInCombat(playerA.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_FALSE(combatHandler.areInCombat(playerA.getId(), npcUniqueId));
        EXPECT_EQ(expected.str(), result);
    }


    TEST_F(CombatHandlerTestSuite, notifyFleeWhileNotInCombat) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        auto result = combatHandler.flee(CLIENT_A);

        std::ostringstream expected;
        expected << "You are in no danger to flee from.\n";

        EXPECT_EQ(expected.str(), result);
    }


    TEST_F(CombatHandlerTestSuite, canFleeWhileInCombat) {
        auto &player = accountHandler.getPlayerByClient(CLIENT_A);
        auto &room = worldHandler.findRoom(TEST_ROOM_1_ID);
        room.addPlayerToRoom(player.getId());
        auto npcUniqueId = room.getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();
        auto doors = room.getDoors();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        bool successfullyEscaped = false;
        const int maximumTries = 10;
        int tries = 0;
        while (!successfullyEscaped) {
            ASSERT_LT(tries, maximumTries);
            ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
            combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);
            ASSERT_EQ(TEST_ROOM_1_ID, accountHandler.getRoomIdByClient(CLIENT_A));
            ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
            ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
            ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

            while (combatHandler.isInCombat(player.getId())) {
                auto result = combatHandler.flee(CLIENT_A);
                if (!combatHandler.isInCombat(player.getId()) && (result.find("successfully") != std::string::npos)) {
                    successfullyEscaped = true;

                } else if (!combatHandler.isInCombat(player.getId())) {
                    player.setCurrRoomID(TEST_ROOM_1_ID);
                    room.addPlayerToRoom(player.getId());
                }
            }

            ++tries;
        }

        ASSERT_TRUE(successfullyEscaped);
        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));

        EXPECT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));
        EXPECT_EQ(TEST_ROOM_2_ID, accountHandler.getRoomIdByClient(CLIENT_A));
    }


    TEST_F(CombatHandlerTestSuite, canFleeWhileInCombatWithNoDoors) {
        // Construct World
        World newWorld{};
        Room createdRoom = {TEST_ROOM_1_ID, "Test room 1", {"Test room 1 description"}};
        createdRoom.addNPC({
            NPC_A_ID,
            {NPC_A_KEYWORD},
            {"Long description."},
            "Test NPC 1",
            {"Interaction text."}
        });

        Area area = Area("Testing area");
        area.addRoom(createdRoom);
        newWorld.addArea(area);
        worldHandler.setWorld(newWorld);

        auto &player = accountHandler.getPlayerByClient(CLIENT_A);
        auto &room = worldHandler.findRoom(TEST_ROOM_1_ID);
        room.addPlayerToRoom(player.getId());
        auto npcUniqueId = room.getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();
        auto doors = room.getDoors();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        bool successfullyEscaped = false;
        const int maximumTries = 10;
        int tries = 0;
        while (!successfullyEscaped) {
            ASSERT_LT(tries, maximumTries);
            ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
            combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);
            ASSERT_EQ(TEST_ROOM_1_ID, accountHandler.getRoomIdByClient(CLIENT_A));
            ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
            ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
            ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

            while (combatHandler.isInCombat(player.getId())) {
                auto result = combatHandler.flee(CLIENT_A);
                if (!combatHandler.isInCombat(player.getId()) && (result.find("successfully") != std::string::npos)) {
                    successfullyEscaped = true;

                } else if (!combatHandler.isInCombat(player.getId())) {
                    player.setCurrRoomID(TEST_ROOM_1_ID);
                    room.addPlayerToRoom(player.getId());
                }
            }

            ++tries;
        }

        ASSERT_TRUE(successfullyEscaped);
        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));

        EXPECT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));
        EXPECT_EQ(TEST_ROOM_1_ID, accountHandler.getRoomIdByClient(CLIENT_A));
    }


    TEST_F(CombatHandlerTestSuite, canGetOpponentId) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        EXPECT_EQ(npcUniqueId, combatHandler.getOpponentId(player.getId()));
        EXPECT_EQ(player.getId(), combatHandler.getOpponentId(npcUniqueId));
    }


    TEST_F(CombatHandlerTestSuite, canReplacePlayerWithDecoy) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.replaceWithDecoy(player);
        EXPECT_NO_THROW(worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(USERNAME_A));
        EXPECT_FALSE(combatHandler.isInCombat(player.getId()));
    }


    TEST_F(CombatHandlerTestSuite, canRemoveCombatInstanceOnClientLogout) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.handleLogout(CLIENT_A);

        EXPECT_FALSE(combatHandler.isInCombat(player.getId()));
        EXPECT_FALSE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));
        EXPECT_NO_THROW(combatHandler.handleLogout(CLIENT_B));
    }


    TEST_F(CombatHandlerTestSuite, canEndCombatRound) {
        auto player = accountHandler.getPlayerByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(player.getId()));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(player.getId()));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(player.getId(), npcUniqueId));

        std::deque<Message> messages;
        for (int i = 0; i <= EXPECTED_ROUND_CYCLES; ++i) {
            combatHandler.processCycle(messages);
        }

        ASSERT_EQ(1u, messages.size());
    }


    TEST_F(CombatHandlerTestSuite, canExitCombat) {
        auto playerId = accountHandler.getPlayerIdByClient(CLIENT_A);
        auto npcUniqueId = worldHandler.findRoom(TEST_ROOM_1_ID).getNpcByKeyword(NPC_A_KEYWORD).getUniqueId();

        ASSERT_FALSE(combatHandler.isInCombat(playerId));
        ASSERT_FALSE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_FALSE(combatHandler.areInCombat(playerId, npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(playerId));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(playerId, npcUniqueId));

        combatHandler.exitCombat(playerId);

        EXPECT_FALSE(combatHandler.isInCombat(playerId));
        EXPECT_FALSE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_FALSE(combatHandler.areInCombat(playerId, npcUniqueId));

        combatHandler.attack(CLIENT_A, NPC_A_KEYWORD);

        ASSERT_TRUE(combatHandler.isInCombat(playerId));
        ASSERT_TRUE(combatHandler.isInCombat(npcUniqueId));
        ASSERT_TRUE(combatHandler.areInCombat(playerId, npcUniqueId));

        combatHandler.exitCombat(playerId, npcUniqueId);

        EXPECT_FALSE(combatHandler.isInCombat(playerId));
        EXPECT_FALSE(combatHandler.isInCombat(npcUniqueId));
        EXPECT_FALSE(combatHandler.areInCombat(playerId, npcUniqueId));
    }
}
