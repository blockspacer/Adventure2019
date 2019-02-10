//
// Created by louis on 19/01/19.
//

#include "Game.h"

#include <map>
#include <sstream>
#include <iostream>

using model::Game;
using model::PlayerHandler;
using model::WorldHandler;

std::string
lowercase(std::string string) {
    std::transform(string.begin(), string.end(), string.begin(), ::tolower);
    return string;
}

std::string
trimWhitespace(std::string string) {
    std::string whitespace = " \t";
    auto start = string.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    auto end = string.find_last_not_of(whitespace);
    auto size = end - start + 1;
    return string.substr(start, size);
}

namespace model {
    Game::Game(std::vector<Connection> &clients,
               std::vector<Connection> &newClients,
               std::vector<Connection> &disconnectedClients,
               std::function<void(Connection)> &disconnect,
               std::function<void()> &shutdown) {
        this->clients = &clients;
        this->newClients = &newClients;
        this->disconnectedClients = &disconnectedClients;

        this->disconnect = disconnect;
        this->shutdown = shutdown;

        this->playerHandler = std::make_unique<PlayerHandler>();
        this->worldHandler = std::make_unique<WorldHandler>();
    }

    void
    Game::handleConnects(std::deque<Response> &responses) {
        for (auto &newClient : *this->newClients) {
            std::ostringstream introduction;

            introduction << "Welcome to Adventure 2019!\n"
                         << "\n"
                         << "Enter \"login\" to login to an existing account\n"
                         << "Enter \"register\" to create a new account\n";

            responses.push_back({newClient, introduction.str()});
        }

        this->newClients->clear();
    }

    void
    Game::handleDisconnects(std::deque<Response> &responses) {

        for (auto &disconnectedClient : *this->disconnectedClients) {
            if (this->playerHandler->isLoggingIn(disconnectedClient)) {
                this->playerHandler->exitLogin(disconnectedClient);
                std::cout << disconnectedClient.id << " has been removed from login due to disconnect\n";
            }

            if (this->playerHandler->isRegistering(disconnectedClient)) {
                this->playerHandler->exitRegistration(disconnectedClient);
                std::cout << disconnectedClient.id << " has been removed from registration due to disconnect\n";
            }

            if (this->playerHandler->isLoggedIn(disconnectedClient)) {
                removeClientFromGame(disconnectedClient);
                this->playerHandler->logoutPlayer(disconnectedClient);
                std::cout << disconnectedClient.id << " has been logged out of the game due to disconnect\n";
            }
        }

        this->disconnectedClients->clear();
    }

    void
    Game::handleIncoming(const std::deque<Message> &incoming, std::deque<Response> &responses) {
        for (const auto& input : incoming) {
            auto client = input.connection;
            auto incomingInput = trimWhitespace(input.text);
            std::ostringstream tempMessage;

            if (this->playerHandler->isLoggingIn(client)) {
                responses.push_back({
                    client,
                    this->playerHandler->processLogin(client, incomingInput.substr(0, incomingInput.find(' ')))
                });
                if (this->playerHandler->isLoggedIn(client)) {
                    addClientToGame(client);
                    auto roomID = this->playerHandler->getRoomIdByClient(client);
                    tempMessage << this->worldHandler->findRoom(roomID);
                    responses.push_back({client, tempMessage.str()});
                }
                continue;

            } else if (this->playerHandler->isRegistering(client)) {
                responses.push_back({
                    client,
                    this->playerHandler->processRegistration(client, incomingInput.substr(0, incomingInput.find(' ')))
                });
                if (this->playerHandler->isLoggedIn(client)) {
                    addClientToGame(client);
                    auto roomID = this->playerHandler->getRoomIdByClient(client);
                    tempMessage << this->worldHandler->findRoom(roomID);
                    responses.push_back({client, tempMessage.str()});
                }
                continue;
            }

            std::string commandString = lowercase(incomingInput.substr(0, incomingInput.find(' ')));

            if (!this->commandMap.count(commandString)) {
                tempMessage << "The word \"" << commandString << "\" is not a valid command.\n";
                responses.push_back({client, tempMessage.str()});
                continue;
            }

            std::string parameters = "";

            if (incomingInput.find(' ') != std::string::npos) {
                parameters = trimWhitespace(incomingInput.substr(incomingInput.find(' ') + 1));
            }

            Command command = this->commandMap.at(commandString);

            switch (command) {
                case Command::Quit: {
                    removeClientFromGame(client);
                    this->disconnect(input.connection);
                    continue;
                }

                case Command::Shutdown: {
                    std::cout << "Shutting down.\n";
                    this->shutdown();
                    return;
                }

                default:
                    break;
            }

            if (!this->playerHandler->isLoggedIn(client)) {
                responses.push_back(this->executeMenuAction(client, command, parameters));

            } else {
                if (isInvalidFormat(command, parameters)) {
                    tempMessage << "Invalid format for command \"" << commandString << "\".\n";
                    responses.push_back({client, tempMessage.str()});
                    continue;
                }
                auto responseList = this->executeInGameAction(client, command, parameters);
                for (auto response : responseList) {
                    responses.push_back(response);
                }
            }
        }
    }

    Response
    Game::executeMenuAction(const Connection &client,
                            const Command &command,
                            const std::string &param) {
        std::ostringstream tempMessage;

        switch (command) {
            case Command::Register:
                tempMessage << this->playerHandler->processRegistration(client);
                break;

            case Command::Login:
                tempMessage << this->playerHandler->processLogin(client);
                break;

            case Command::Help:
                tempMessage << "\n"
                            << "********\n"
                            << "* HELP *\n"
                            << "********\n"
                            << "\n"
                            << "COMMANDS:\n"
                            << "  - " << this->getCommandWords(Command::Help) << " (shows this help interface)\n"
                            << "  - " << this->getCommandWords(Command::Register) << " (create a new account)\n"
                            << "  - " << this->getCommandWords(Command::Login) << " (login to an existing account)\n"
                            << "  - " << this->getCommandWords(Command::Quit) << " (disconnects you from the game server)\n"
                            << "  - " << this->getCommandWords(Command::Shutdown) << " (shuts down the game server)\n";
                break;

            default:
                tempMessage << "\nEnter " << "\"" << this->getCommandWords(Command::Login) << "\" to login to an existing account\n"
                            << "Enter " << "\"" << this->getCommandWords(Command::Register) << "\" to create a new account\n"
                            << "Enter " << "\"" << this->getCommandWords(Command::Help) << "\" for a full list of commands\n";
                break;
        }

        return {client, tempMessage.str()};
    }


    std::vector<Response>
    Game::executeInGameAction(const Connection &client,
                              const Command &command,
                              const std::string &param) {
        std::ostringstream tempMessage;
        bool isLocal = true;

        switch (command) {
            case Command::Logout: {
                removeClientFromGame(client);
                tempMessage << this->playerHandler->logoutPlayer(client);
                break;
            }

            case Command::Help:
                tempMessage << "\n"
                            << "********\n"
                            << "* HELP *\n"
                            << "********\n"
                            << "\n"
                            << "COMMANDS:\n"
                            << "  - " << this->getCommandWords(Command::Help) << " (shows this help interface)\n"
                            << "  - " << this->getCommandWords(Command::Say) << " [message] (sends [message] to close by players in the game)\n"
                            << "  - " << this->getCommandWords(Command::Tell) << " [username] [message] (sends [message] to [username] in the game)\n"
                            << "  - " << this->getCommandWords(Command::Yell) << " [message] (sends [message] to other players in the game)\n"
                            << "  - " << this->getCommandWords(Command::Look) << " (displays current location information)\n"
                            << "  - " << this->getCommandWords(Command::Move) << " [direction] (moves you in the direction specified)\n"
                            << "  - " << this->getCommandWords(Command::Logout) << " (logs you out of the game)\n"
                            << "  - " << this->getCommandWords(Command::Quit) << " (disconnects you from the game server)\n"
                            << "  - " << this->getCommandWords(Command::Shutdown) << " (shuts down the game server)\n";
                break;

            case Command::Say: {
                std::vector<Response> responses;
                auto roomId = this->playerHandler->getRoomIdByClient(client);
                auto playerIds = this->worldHandler->getNearbyPlayerIds(roomId);
                for (auto playerId : playerIds) {
                    auto connection = this->playerHandler->getClientByPlayerId(playerId);
                    std::ostringstream sayMessage;
                    sayMessage << this->playerHandler->getUsernameByClient(client) << "> " << param << "\n";
                    responses.push_back({connection, sayMessage.str(), isLocal});
                }
                return responses;
            }

            case Command::Tell: {
                std::vector<Response> responses;
                auto username = param.substr(0, param.find(' '));
                auto message = trimWhitespace(param.substr(param.find(' ') + 1));
                for (auto connection: *this->clients) {
                    auto receiver = this->playerHandler->getUsernameByClient(connection);
                    if (receiver == username)  {
                        auto sender = this->playerHandler->getUsernameByClient(client);
                        std::ostringstream toMessage, fromMessage;
                        toMessage << "To " << receiver << "> " << message << "\n";
                        fromMessage << "From " + sender + "> " << message << "\n";;
                        responses.push_back({client, toMessage.str(), isLocal});
                        responses.push_back({connection, fromMessage.str(), isLocal});
                        return responses;
                    }
                }
                tempMessage << "Unable to find online user \"" << username << "\".\n";
                break;
            }

            case Command::Yell: {
                isLocal = false;
                tempMessage << this->playerHandler->getUsernameByClient(client) << "> " << param << "\n";
                break;
            }

            case Command::Look: {
                auto roomID = this->playerHandler->getRoomIdByClient(client);
                tempMessage << this->worldHandler->findRoom(roomID);
                break;
            }

            case Command::Move: {
                auto roomID = this->playerHandler->getRoomIdByClient(client);
                if (this->worldHandler->isValidDirection(roomID, param)) {
                    auto playerID = this->playerHandler->getPlayerIdByClient(client);
                    auto destinationID = this->worldHandler->getDestination(roomID, param);
                    this->worldHandler->movePlayer(playerID, roomID, destinationID);
                    this->playerHandler->setRoomIdByClient(client, destinationID);
                    tempMessage << this->worldHandler->findRoom(destinationID);
                } else {
                    tempMessage << "You can't move that way!\n";
                }
                break;
            }

            default:
                tempMessage << "\nEnter " << "\"" << this->getCommandWords(Command::Help) << "\" for a full list of commands\n";
                break;
        }

        return {{client, tempMessage.str(), isLocal}};
    }

    void
    Game::handleOutgoing(std::deque<Response> &responses) {
        this->playerHandler->notifyBootedClients(responses);
    }


    std::deque<Message>
    Game::formMessages(std::deque<Response> &responses) {
        std::deque<Message> outgoing;
        std::map<Connection, std::ostringstream> clientMessages;

        for (const auto &entry : responses) {
            if (entry.isLocal) {
                clientMessages[entry.client] << entry.message;

            } else {
                for (const auto &client : *this->clients) {
                    // Send to public messages to logged in users only
                    if (this->playerHandler->isLoggedIn(client)) {
                        clientMessages[client] << entry.message;
                    }
                }
            }
        }

        for (auto const& [client, message] : clientMessages) {
            outgoing.push_back({client, message.str()});
        }

        return outgoing;
    }


    std::string
    Game::getCommandWords(Command command) {
        std::vector<std::string> words = this->commandWordsMap.at(command);
        std::ostringstream tempMessage;

        for (unsigned int i = 0; i < words.size(); ++i) {
            tempMessage << words[i];

            if (i < (words.size() - 1)) {
                tempMessage << ", ";
            }
        }

        return tempMessage.str();
    }

    void
    Game::addClientToGame(Connection client) {
        auto playerID = this->playerHandler->getPlayerIdByClient(client);
        auto roomID = this->playerHandler->getRoomIdByClient(client);
        this->worldHandler->addPlayer(playerID, roomID);
    }

    void
    Game::removeClientFromGame(Connection client) {
        auto playerID = this->playerHandler->getPlayerIdByClient(client);
        auto roomID = this->playerHandler->getRoomIdByClient(client);
        this->worldHandler->removePlayer(playerID, roomID);
    }

    bool
    Game::isInvalidFormat(const Command &command, const std::string &parameters) {
        bool wrongTellFormat = (command == Command::Tell && parameters.find(' ') == std::string::npos);
        bool isCommandWithParam = (command == Command::Move || command == Command::Say  || command == Command::Yell);
        return (wrongTellFormat || (isCommandWithParam && parameters.empty()));
    }

    std::deque<Message>
    Game::processCycle(std::deque<Message> &incoming) {
        std::deque<Response> responses;

        this->handleConnects(responses);
        this->handleDisconnects(responses);
        this->handleIncoming(incoming, responses);
        this->handleOutgoing(responses);

        return this->formMessages(responses);
    }
}
