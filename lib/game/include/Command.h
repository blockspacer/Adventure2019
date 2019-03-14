//
// Created by Stephen Wanhella on 2019-02-14.
//

#ifndef COMMAND_H
#define COMMAND_H
namespace game {
    enum class Command {
        Cast,
        Debug,
        Drop,
        Equipment,
        Examine,
        Exits,
        Help,
        Inventory,
        Login,
        Logout,
        Look,
        Move,
        Quit,
        Register,
        Remove,
        Say,
        Shutdown,
        Spells,
        Talk,
        Take,
        Tell,
        Wear,
        Yell,
        InvalidCommand
    };
}

#endif //COMMAND_H