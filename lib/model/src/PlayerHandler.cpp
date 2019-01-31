//
// Created by Stephen Wanhella on 2019-01-22.
//

#include <PlayerHandler.h>
#include <iostream>
#include <sstream>

#include "PlayerHandler.h"

using json = nlohmann::json;
using player = model::Player;

namespace model {
    const unsigned short PlayerHandler::MIN_PASSWORD_LENGTH = 4;
    const unsigned short PlayerHandler::MAX_USERNAME_AND_PASSWORD_LENGTH = 16;

    PlayerHandler::PlayerHandler() {
        this->nextId = 1;
        this->allPlayers = {};
        this->usernameToPlayer = {};
        this->activeIdToClient = {};
        this->activeClientToId = {};

        this->regUsernamePrompt = {};
        this->regPasswordFirstPrompt = {};
        this->regPasswordSecondPrompt = {};

        this->loginUsernameInput = {};
        this->loginUsernamePrompt = {};
        this->loginPasswordPrompt = {};

        this->bootedClients = {};
    }

    bool
    PlayerHandler::isLoggedIn(const uintptr_t &clientId) {
        return (bool) activeClientToId.count(clientId);
    }

    bool
    PlayerHandler::isRegistering(const uintptr_t &clientId) {
        auto inUsernamePrompt       = (bool) this->regUsernamePrompt.count(clientId);
        auto inFirstPasswordPrompt  = (bool) this->regPasswordFirstPrompt.count(clientId);
        auto inSecondPasswordPrompt = (bool) this->regPasswordSecondPrompt.count(clientId);

        return (inUsernamePrompt || inFirstPasswordPrompt || inSecondPasswordPrompt);
    }

    std::string
    PlayerHandler::startRegistration(const uintptr_t &clientId) {
        this->regUsernamePrompt.insert(clientId);
        std::ostringstream response;
        response << "\n"
                 << "Register\n"
                 << "--------\n"
                 << "Enter your username (maximum of " << MAX_USERNAME_AND_PASSWORD_LENGTH << " characters)\n";

        return response.str();
    }

    std::string
    PlayerHandler::processRegistration(const uintptr_t &clientId, const std::string &input) {
        auto inUsernamePrompt       = (bool) this->regUsernamePrompt.count(clientId);
        auto inFirstPasswordPrompt  = (bool) this->regPasswordFirstPrompt.count(clientId);

        if (inUsernamePrompt) {
            if (input.length() == 0) {
                this->regUsernamePrompt.erase(clientId);
                return "No username entered. Registration process cancelled.\n";
            }

            if (input.length() > MAX_USERNAME_AND_PASSWORD_LENGTH) {
                this->regUsernamePrompt.erase(clientId);
                return "The username you entered is too long. Registration process cancelled.\n";
            }

            if (this->usernameToPlayer.count(input)) {
                return "The username \"" + input + "\" has already been taken, please use a different username.\n";
            }

            this->regUsernameInput.emplace(clientId, input);

            this->regPasswordFirstPrompt.insert(clientId);
            this->regUsernamePrompt.erase(clientId);

            std::ostringstream response;
            response << input << "\n"
                     << "Enter your password (minimum of " << MIN_PASSWORD_LENGTH << " characters,"
                     << " maximum of " << MAX_USERNAME_AND_PASSWORD_LENGTH << " characters)\n";

            return response.str();

        } else if (inFirstPasswordPrompt) {
            if (input.length() < MIN_PASSWORD_LENGTH) {
                this->regPasswordFirstPrompt.erase(clientId);
                return "The password you entered is too short. Registration process cancelled.\n";
            }

            if (input.length() > MAX_USERNAME_AND_PASSWORD_LENGTH) {
                this->regPasswordFirstPrompt.erase(clientId);
                return "The password you entered is too long. Registration process cancelled.\n";
            }

            this->regPasswordInput.emplace(clientId, input);
            this->regPasswordSecondPrompt.insert(clientId);
            this->regPasswordFirstPrompt.erase(clientId);

            return "Re-enter your password\n";

        } else {
            this->regPasswordSecondPrompt.erase(clientId);

            if (this->regPasswordInput.at(clientId) != input) {
                return "The passwords you entered do not match. Registration process cancelled.\n";
            }

            auto inputUsername = this->regUsernameInput.at(clientId);
            auto inputPassword = this->regPasswordInput.at(clientId);
            this->regUsernameInput.erase(clientId);
            this->regPasswordInput.erase(clientId);

            if (this->usernameToPlayer.count(inputUsername)) {
                return "The username \"" + inputUsername + "\" has already been taken, please use a different username.\n";
            }

            auto playerId = this->nextId++;
            this->allPlayers.emplace(playerId, Player(playerId, inputUsername, inputPassword));
            this->usernameToPlayer.emplace(inputUsername, &this->allPlayers.at(playerId));

            this->activeClientToId.emplace(clientId, playerId);
            this->activeIdToClient.emplace(playerId, clientId);

            std::cout << inputUsername << " has been registered to the game\n";
            return "Your account has been successfully registered and you are now logged in.\n\n";
        }
    }

    void
    PlayerHandler::exitRegistration(const uintptr_t &clientId) {
        if (this->regUsernamePrompt.count(clientId)) {
            this->regUsernamePrompt.erase(clientId);
        }

        if (this->regPasswordFirstPrompt.count(clientId)) {
            this->regUsernameInput.erase(clientId);
            this->regPasswordFirstPrompt.erase(clientId);
        }

        if (this->regPasswordSecondPrompt.count(clientId)) {
            this->regPasswordInput.erase(clientId);
            this->regPasswordSecondPrompt.erase(clientId);
        }
    }

    bool
    PlayerHandler::isLoggingIn(const uintptr_t &clientId) {
        auto inUsernamePrompt = (bool) this->loginUsernamePrompt.count(clientId);
        auto inPasswordPrompt = (bool) this->loginPasswordPrompt.count(clientId);

        return (inUsernamePrompt || inPasswordPrompt);
    }

    std::string
    PlayerHandler::startLogin(const uintptr_t &clientId) {
        this->loginUsernamePrompt.insert(clientId);

        return "\nLogin\n"
               "-----\n"
               "Enter your username\n";
    }

    std::string
    PlayerHandler::processLogin(const uintptr_t &clientId, const std::string &input) {
        auto inUsernamePrompt = (bool) this->loginUsernamePrompt.count(clientId);
        auto inPasswordPrompt = (bool) this->loginPasswordPrompt.count(clientId);

        if (input.length() == 0) {
            this->loginUsernamePrompt.erase(clientId);
            return "No username entered. Login process cancelled.\n";
        }

        if (inUsernamePrompt) {
            this->loginUsernameInput.emplace(clientId, input);
            this->loginUsernamePrompt.erase(clientId);
            this->loginPasswordPrompt.insert(clientId);
            return input + "\nEnter your password\n";

        } else {
            if (!inPasswordPrompt) {
                return "A login error has occurred\n";
            }

            this->loginPasswordPrompt.erase(clientId);

            auto successfulLogin = false;
            auto inputUsername = this->loginUsernameInput.at(clientId);
            this->loginUsernameInput.erase(clientId);

            if (this->usernameToPlayer.count(inputUsername)) {
                auto selectedPlayer = this->usernameToPlayer.at(inputUsername);
                auto playerId = selectedPlayer->getId();

                if ((selectedPlayer->getPassword() == input) && !this->activeIdToClient.count(playerId)) {
                    this->activeClientToId.emplace(clientId, playerId);
                    this->activeIdToClient.emplace(playerId, clientId);
                    successfulLogin = true;

                } else if ((selectedPlayer->getPassword() == input) && this->activeIdToClient.count(playerId)) {
                    // Player is already being used by a client, logout associated client
                    // and login with new client
                    auto otherClientId = this->activeIdToClient.at(playerId);
                    this->bootedClients.push_back(otherClientId);

                    this->activeClientToId.erase(otherClientId);
                    this->activeIdToClient.erase(playerId);

                    this->activeClientToId.emplace(clientId, playerId);
                    this->activeIdToClient.emplace(playerId, clientId);

                    successfulLogin = true;
                    std::cout << inputUsername << " is now being used by " << clientId << "\n";
                }
            }

            if (successfulLogin) {
                return "Logged in successfully!\n";

            } else {
                return "Invalid username or password.\n";
            }
        }
    }

    void
    PlayerHandler::exitLogin(const uintptr_t &clientId) {
        if (this->loginUsernamePrompt.count(clientId)) {
            this->loginUsernamePrompt.erase(clientId);
        }

        if (this->loginPasswordPrompt.count(clientId)) {
            this->loginUsernameInput.erase(clientId);
            this->loginPasswordPrompt.erase(clientId);
        }
    }

    std::string
    PlayerHandler::logoutPlayer(const uintptr_t &clientId) {
        this->activeIdToClient.erase(this->activeClientToId.at(clientId));
        this->activeClientToId.erase(clientId);

        return "Logged out successfully.\n";
    }

    std::string
    PlayerHandler::getUsernameByClientId(const uintptr_t &clientId) {
        return this->allPlayers.at(this->activeClientToId.at(clientId)).getUsername();
    }

    void
    PlayerHandler::notifyBootedClients(std::deque<Response> &responses) {
        auto itr = this->bootedClients.begin();

        while (itr != this->bootedClients.end()) {
            auto clientId = *itr;
            responses.push_back({clientId, "You have been logged out due to being logged in elsewhere.\n", true});

            this->bootedClients.erase(itr);
        }
    }

    std::vector<Player>
    PlayerHandler::parseJsonUsers(json users) {
        std::vector<Player> players;

        for (json::iterator it = users.begin(); it != users.end(); ++it) {
            Player p (it.value().at("id"), it.value().at("username"), "");
            players.push_back(p);
        }

        return players;
    }
}