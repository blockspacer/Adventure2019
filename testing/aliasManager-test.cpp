//
// Created by Stephen Wanhella on 2019-02-14.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AliasManager.h"
#include "CommandParser.h"
#include <boost/algorithm/string/case_conv.hpp>

using game::Command;
using game::AliasManager;
using game::CommandParser;

namespace {
    TEST(AliasManagerTestSuite, canGetAliasedCommands) {
        AliasManager aliasManager;
        std::string testUser = "testuser";

        std::string testHelp = "test_help";
        std::string testLogin = "test_login";
        std::string testLogout = "test_logout";
        std::string testLook = "test_look";
        std::string testMove = "test_move";
        std::string testQuit = "test_quit";
        std::string registerAlias = "test_register";
        std::string sayAlias = "test_say";
        std::string shutdownAlias = "test_shutdown";
        std::string tellAlias = "test_tell";
        std::string yellAlias = "test_yell";

        aliasManager.setUserAlias(Command::Help, testHelp, testUser);
        aliasManager.setUserAlias(Command::Login, testLogin, testUser);
        aliasManager.setUserAlias(Command::Logout, testLogout, testUser);
        aliasManager.setUserAlias(Command::Look, testLook, testUser);
        aliasManager.setUserAlias(Command::Move, testMove, testUser);
        aliasManager.setUserAlias(Command::Quit, testQuit, testUser);
        aliasManager.setUserAlias(Command::Register, registerAlias, testUser);
        aliasManager.setUserAlias(Command::Say, sayAlias, testUser);
        aliasManager.setUserAlias(Command::Shutdown, shutdownAlias, testUser);
        aliasManager.setUserAlias(Command::Tell, tellAlias, testUser);
        aliasManager.setUserAlias(Command::Yell, yellAlias, testUser);

        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(testHelp, testUser));
        EXPECT_EQ(Command::Login, aliasManager.getCommandForUser(testLogin, testUser));
        EXPECT_EQ(Command::Logout, aliasManager.getCommandForUser(testLogout, testUser));
        EXPECT_EQ(Command::Look, aliasManager.getCommandForUser(testLook, testUser));
        EXPECT_EQ(Command::Move, aliasManager.getCommandForUser(testMove, testUser));
        EXPECT_EQ(Command::Quit, aliasManager.getCommandForUser(testQuit, testUser));
        EXPECT_EQ(Command::Register, aliasManager.getCommandForUser(registerAlias, testUser));
        EXPECT_EQ(Command::Say, aliasManager.getCommandForUser(sayAlias, testUser));
        EXPECT_EQ(Command::Shutdown, aliasManager.getCommandForUser(shutdownAlias, testUser));
        EXPECT_EQ(Command::Tell, aliasManager.getCommandForUser(tellAlias, testUser));
        EXPECT_EQ(Command::Yell, aliasManager.getCommandForUser(yellAlias, testUser));

        aliasManager.clearUserAlias(Command::Help, testUser);
        aliasManager.clearUserAlias(Command::Login, testUser);
        aliasManager.clearUserAlias(Command::Logout, testUser);
        aliasManager.clearUserAlias(Command::Look, testUser);
        aliasManager.clearUserAlias(Command::Move, testUser);
        aliasManager.clearUserAlias(Command::Quit, testUser);
        aliasManager.clearUserAlias(Command::Register, testUser);
        aliasManager.clearUserAlias(Command::Say, testUser);
        aliasManager.clearUserAlias(Command::Shutdown, testUser);
        aliasManager.clearUserAlias(Command::Tell, testUser);
        aliasManager.clearUserAlias(Command::Yell, testUser);

        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testHelp, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testLogin, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testLogout, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testLook, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testMove, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(testQuit, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(registerAlias, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(sayAlias, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(shutdownAlias, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(tellAlias, testUser));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(yellAlias, testUser));
    }

    TEST(AliasManagerTestSuite, aliasedCommandsFallBackToDefault) {
        AliasManager aliasManager;
        std::string testUser = "testuser_no_aliases";
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser("help", testUser));
        EXPECT_EQ(Command::Login, aliasManager.getCommandForUser("login", testUser));
        EXPECT_EQ(Command::Logout, aliasManager.getCommandForUser("logout", testUser));
        EXPECT_EQ(Command::Look, aliasManager.getCommandForUser("look", testUser));
        EXPECT_EQ(Command::Move, aliasManager.getCommandForUser("move", testUser));
        EXPECT_EQ(Command::Quit, aliasManager.getCommandForUser("quit", testUser));
        EXPECT_EQ(Command::Register, aliasManager.getCommandForUser("register", testUser));
        EXPECT_EQ(Command::Say, aliasManager.getCommandForUser("say", testUser));
        EXPECT_EQ(Command::Shutdown, aliasManager.getCommandForUser("shutdown", testUser));
        EXPECT_EQ(Command::Tell, aliasManager.getCommandForUser("tell", testUser));
        EXPECT_EQ(Command::Yell, aliasManager.getCommandForUser("yell", testUser));
    }

    TEST(AliasManagerTestSuite, canMakeAndDeleteGlobalAliases) {
        AliasManager aliasManager;
        std::string testUser = "testuser";
        std::string testUser_noAliases = "asdf_user";
        std::string global_help_alias = "hAlP";
        std::string helpAlias = "test_help";
        CommandParser commandParser;
        std::string helpDefault = commandParser.getStringForCommand(Command::Help);

        aliasManager.setGlobalAlias(Command::Help, global_help_alias);
        aliasManager.setUserAlias(Command::Help, helpAlias, testUser);

        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(helpDefault, testUser));
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(helpAlias, testUser));
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(global_help_alias, testUser));

        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(helpDefault, testUser_noAliases));
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(helpAlias, testUser_noAliases));
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(global_help_alias, testUser_noAliases));
        aliasManager.clearGlobalAlias(Command::Help);
        aliasManager.clearUserAlias(Command::Help, testUser);
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(global_help_alias, testUser_noAliases));
    }

    TEST(AliasManagerTestSuite, canMakeAndDeleteUserAliases) {
        AliasManager aliasManager;
        std::string testUser = "new_test_user";
        std::string testUser_noAliases = "asdf_user";
        std::string user_help_alias = "hAlP";

        aliasManager.setUserAlias(Command::Help, user_help_alias, testUser);
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser(user_help_alias, testUser));
        EXPECT_NE(Command::Help, aliasManager.getCommandForUser(user_help_alias, testUser_noAliases));
        EXPECT_EQ(Command::Help, aliasManager.getCommandForUser("help", testUser));
        aliasManager.clearUserAlias(Command::Help, testUser);
        EXPECT_EQ(Command::InvalidCommand, aliasManager.getCommandForUser(user_help_alias, testUser));
    }

    TEST(AliasManagerTestSuite, canThrowExceptionsOnBadFilePath) {
        AliasManager aliasManager{"asdf.json"};
        std::string username = "asdf_user";
        std::string alias = "asdf";
        Command command = Command::Logout;
        ASSERT_ANY_THROW(aliasManager.setUserAlias(command, alias, username));
        ASSERT_ANY_THROW(aliasManager.clearUserAlias(command, username));
        ASSERT_ANY_THROW(aliasManager.setGlobalAlias(command, alias));
        ASSERT_ANY_THROW(aliasManager.clearGlobalAlias(command));
        ASSERT_ANY_THROW(aliasManager.getCommandForUser(alias, username));
    }

    TEST(AliasManagerTestSuite, canCatchInvalidAliases) {
        AliasManager aliasManager;
        CommandParser commandParser;

        std::string invalidAlias = commandParser.getStringForCommand(Command::Say);
        std::string uppercaseInvalidAlias = boost::algorithm::to_upper_copy(invalidAlias);

        std::string validAlias = "ayo";
        std::string uppercaseValidAlias = boost::algorithm::to_upper_copy(validAlias);

        EXPECT_TRUE(aliasManager.isValidAlias(validAlias));
        EXPECT_TRUE(aliasManager.isValidAlias(uppercaseValidAlias));
        EXPECT_FALSE(aliasManager.isValidAlias(invalidAlias));
        EXPECT_FALSE(aliasManager.isValidAlias(uppercaseInvalidAlias));
    }

    TEST(AliasManagerTestSuite, canGetAliasesForUser) {
        AliasManager aliasManager;
        CommandParser commandParser;
        std::string username = "TestUser_asldfkjasldfkjas";
        auto command = Command::Help;

        aliasManager.clearUserAlias(command, username);
        auto aliases = aliasManager.getAliasesForUser(username);
        EXPECT_TRUE(aliases.empty());

        auto alias = "yeet";
        aliasManager.setUserAlias(command, alias, username);
        aliases = aliasManager.getAliasesForUser(username);
        EXPECT_FALSE(aliases.empty());
        aliasManager.clearUserAlias(command, username);
    }

    TEST(AliasManagerTestSuite, canGetAliasesForUserBadFilename) {
        AliasManager aliasManager{"fake_file.json"};
        std::string username = "stephen";
        ASSERT_ANY_THROW(aliasManager.getAliasesForUser(username));
    }

    TEST(AliasManagerTestSuite, wontSetDuplicateAlias) {
        AliasManager aliasManager;
        std::string username = "stephen";
        Command command = Command::Help;
        std::string alias = "yeet";
        aliasManager.clearUserAlias(command, username);
        EXPECT_TRUE(aliasManager.setUserAlias(command, alias, username));
        EXPECT_FALSE(aliasManager.setUserAlias(command, alias, username));
        aliasManager.clearUserAlias(command, username);
    }
}
