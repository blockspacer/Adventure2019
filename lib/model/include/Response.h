//
// Created by louis on 25/01/19.
//

#ifndef RESPONSE_H
#define RESPONSE_H

#include <cstdint>
#include <string>

namespace model {
    /**
     *  A message to be sent from the server to client(s). Stores a boolean value for
     *  determining if the message should be sent to only the sender or to all clients
     */
    struct Response {
        uintptr_t clientId;
        std::string message;
        bool isLocal;
    };
}

#endif //RESPONSE_H
