//
// Created by Stephen Wanhella on 2019-02-14.
//

#include <WorldHandler.h>
#include "gtest/gtest.h"
#include "Character.h"

namespace {
    model::ID getFirstRoom(const model::WorldHandler &worldHandler) {
        EXPECT_FALSE(worldHandler.getWorld().getAreas().empty());
        auto area = worldHandler.getWorld().getAreas()[0];
        EXPECT_FALSE(area.getRooms().empty());
        auto room = area.getRooms()[0];
        return room.getId();
    }

    TEST(WorldHandlerTestSuite, canMakeWorld) {
        model::WorldHandler worldHandler;
    }

    TEST(WorldHandlerTestSuite, canLoadAreas) {
        model::WorldHandler worldHandler;
        EXPECT_FALSE(worldHandler.getWorld().getAreas().empty());
    }

    TEST(WorldHandlerTestSuite, canAddAndRemovePlayerFromRoom) {
        model::WorldHandler worldHandler;
        auto roomId = getFirstRoom(worldHandler);
        model::ID playerId = 1234;
        worldHandler.addPlayer(playerId, roomId);

        auto room = worldHandler.findRoom(roomId);
        auto playersInRoom = room.getPlayersInRoom();
        bool isPlayerInRoom = std::find(playersInRoom.begin(), playersInRoom.end(), playerId) != playersInRoom.end();
        EXPECT_TRUE(isPlayerInRoom);

        worldHandler.removePlayer(playerId, roomId);

        // have to re-find room after making changes because it's not returned by reference
        room = worldHandler.findRoom(roomId);
        playersInRoom = room.getPlayersInRoom();
        isPlayerInRoom = std::find(playersInRoom.begin(), playersInRoom.end(), playerId) != playersInRoom.end();
        EXPECT_FALSE(isPlayerInRoom);
    }

    TEST(WorldHandlerTestSuite, canGetAdjacentRooms) {
        model::WorldHandler worldHandler;
        auto roomId = getFirstRoom(worldHandler);
        auto doors = worldHandler.findRoom(roomId).getDoors();
        std::unordered_map<std::string, model::ID> destinations;
        for (const auto &door: doors) {
            destinations.emplace(door.dir, door.leadsTo);
        }
        for (const auto &destinationPair: destinations) {
            auto direction = destinationPair.first;
            EXPECT_TRUE(worldHandler.isValidDirection(roomId, direction));
            auto destinationId = destinationPair.second;
            EXPECT_EQ(worldHandler.getDestination(roomId, direction), destinationId);
        }
    }

    TEST(WorldHandlerTestSuite, canMovePlayer) {
        model::WorldHandler worldHandler;
        auto roomId = getFirstRoom(worldHandler);
        auto room = worldHandler.findRoom(roomId);
        auto adjacentRoomId = room.getDoors()[0].leadsTo;
        model::ID playerId = 1234;

        worldHandler.addPlayer(playerId, roomId);

        auto playersInRoom = worldHandler.findRoom(roomId).getPlayersInRoom();
        auto playersInAdjacentRoom = worldHandler.findRoom(adjacentRoomId).getPlayersInRoom();
        auto nearbyPlayers = worldHandler.getNearbyPlayerIds(roomId);

        bool isPlayerInRoom = std::find(playersInRoom.begin(), playersInRoom.end(), playerId) != playersInRoom.end();
        bool isPlayerInAdjacentRoom = std::find(playersInAdjacentRoom.begin(), playersInAdjacentRoom.end(), playerId) !=
                                      playersInAdjacentRoom.end();
        bool isPlayerNearbyRoom =
                std::find(nearbyPlayers.begin(), nearbyPlayers.end(), playerId) != nearbyPlayers.end();
        EXPECT_TRUE(isPlayerInRoom);
        EXPECT_FALSE(isPlayerInAdjacentRoom);
        EXPECT_TRUE(isPlayerNearbyRoom);

        worldHandler.movePlayer(playerId, roomId, adjacentRoomId);

        playersInRoom = worldHandler.findRoom(roomId).getPlayersInRoom();
        playersInAdjacentRoom = worldHandler.findRoom(adjacentRoomId).getPlayersInRoom();
        nearbyPlayers = worldHandler.getNearbyPlayerIds(roomId);

        isPlayerInRoom = std::find(playersInRoom.begin(), playersInRoom.end(), playerId) != playersInRoom.end();
        isPlayerInAdjacentRoom = std::find(playersInAdjacentRoom.begin(), playersInAdjacentRoom.end(), playerId) !=
                                 playersInAdjacentRoom.end();
        isPlayerNearbyRoom = std::find(nearbyPlayers.begin(), nearbyPlayers.end(), playerId) != nearbyPlayers.end();
        EXPECT_FALSE(isPlayerInRoom);
        EXPECT_TRUE(isPlayerInAdjacentRoom);
        EXPECT_TRUE(isPlayerNearbyRoom);
    }

    TEST(WorldHandlerTestSuite, willThrowExceptionOnInvalidRoom) {
        model::WorldHandler worldHandler;
        model::ID nonExistentRoomId = 12345678;
        ASSERT_THROW(worldHandler.findRoom(nonExistentRoomId), std::runtime_error);
    }
}
