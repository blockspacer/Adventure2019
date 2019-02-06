//
// Created by louis on 19/01/19.
//

#ifndef GAME_H
#define GAME_H

#include "Server.h"
#include "Player.h"
#include "Response.h"
#include "PlayerHandler.h"

#include <functional>
#include <deque>
#include <map>

using networking::Connection;
using networking::Message;
using model::PlayerHandler;

namespace model {
    /**
     *  @class Game
     *
     *  @brief A class for containing the game's logic
     *
     *  This class manages the components necessary to gather, process, and output
     *  information that is to be exchanged between the server and clients.
     */
    class Game {
    private:
        std::vector<Connection>* clients;
        std::vector<Connection>* newClients;
        std::vector<Connection>* disconnectedClients;
        std::function<void(Connection action)> disconnect;
        std::function<void()> shutdown;

        std::unique_ptr<PlayerHandler> playerHandler;

        enum class Command {
            HELP,
            LOGIN,
            LOGOUT,
            LOOK,
            QUIT,
            REGISTER,
            SAY,
            SHUTDOWN,
            TELL
        };

        std::map<std::string, Command> commandMap = {
            {"help", Command::HELP},
            {"login", Command::LOGIN},
            {"logout", Command::LOGOUT},
            {"look", Command::LOOK},
            {"info", Command::LOOK},
            {"quit", Command::QUIT},
            {"register", Command::REGISTER},
            {"say", Command::SAY},
            {"shutdown", Command::SHUTDOWN},
            {"tell", Command::TELL}
        };

        std::map<Command, std::vector<std::string>> commandWordsMap = {
            {Command::HELP, {"help"}},
            {Command::LOGIN, {"login"}},
            {Command::LOGOUT, {"logout"}},
            {Command::LOOK, {"look"}},
            {Command::QUIT, {"quit"}},
            {Command::REGISTER, {"register"}},
            {Command::SAY, {"say"}},
            {Command::SHUTDOWN, {"shutdown"}},
            {Command::TELL, {"tell"}}
        };

        /**
         *  Calls handler class methods that manage newly connected clients. Empties new client IDs from the associated
         *  vector on completion.
         */
        void
        handleConnects(std::deque<Response> &responses);

        /**
         *  Calls handler class methods that manage disconnected users here. Empties disconnected client IDs from the
         *  associated vector on completion.
         */
        void
        handleDisconnects(std::deque<Response> &responses);

        /**
         *  Processes client input, calling class methods based on client input and formulating appropriate responses in
         *  the form of Response objects.
         */
        void
        handleIncoming(const std::deque<Message> &incoming, std::deque<Response> &responses);

        /**
         *  Creates a Response to commands when the client is not logged in
         */
        Response
        executeMenuAction(const Connection &client, const Command &command, const std::string &param);

        /**
         *  Creates a Response to commands when the client is logged in
         */
        std::vector<Response>
        executeInGameAction(const Connection &client, const Command &command, const std::string &param);

        /**
         *  Calls handler class methods that return responses and are not dependent on user input.
         */
        void
        handleOutgoing(std::deque<Response> &responses);

        /**
         *  Converts a Response deque into a Message deque.
         */
        std::deque<Message>
        formMessages(std::deque<Response> &responses);

        /**
         *  Returns the words associated with a command in the form of a comma-separated string.
         *  Used to display commands in the help display.
         */
         std::string
         getCommandWords(Command command);

    public:
        /**
         *  Constructs a Game instance with references to connected clients, new client IDs, and disconnected client IDs.
         *  The game class' handleConnects() and handleDisconnects() methods are responsible for removing IDs from the
         *  new client ID and disconnected client ID vectors, respectively.
         */
        Game(std::vector<Connection> &clients,
             std::vector<Connection> &newClients,
             std::vector<Connection> &disconnectedClients,
             std::function<void(Connection)> &disconnect,
             std::function<void()> &shutdown);

        /**
         *  Runs a game cycle. Performs handleConnects(), handleDisconnects(), handleIncoming(), handleOutgoing(),
         *  and finally formMessages(). This Returns a deque of Message objects to be sent by the game server.
         */
        std::deque<Message>
        processCycle(std::deque<Message> &incoming);
    };
}

#endif //GAME_H
