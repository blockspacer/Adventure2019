//
// Created by louis on 21/02/19.
//

#include "lib/gtest/gtest.h"
#include "lib/gmock/gmock.h"
#include "MagicHandler.h"
#include "AccountHandler.h"
#include "Server.h"

using model::MagicHandler;
using model::AccountHandler;
using networking::Connection;

/*
 *  Cases to test:
 *  1. Reject invalid spell names
 *  2. A player can cast Confuse on themselves
 *  3. A player can cast Confuse on another player in the same room
 *  4. A player cannot cast Confuse on another player in a different room
 *  5. A player cannot cast Confuse on themselves if they are already Confused
 *  6. A player cannot cast Confuse on an already Confused player
 *  7. The confuseSpeech() method can convert a string into Pig Latin
 *  8. Confuse spell instance will expire after a certain number of cycles
 *  9. A player cannot cast Body Swap on themselves
 * 10. A player can cast Body Swap on another player in the same room
 * 11. A player cannot cast Body Swap on another player in a different room
 * 12. A player cannot cast Body Swap if they are under the Body Swap spell's effects
 * 13. A player cannot cast Body Swap on an already Body Swapped player
 * 14. Body Swap spell instance will expire after a certain number of cycles
 */

constexpr Connection CLIENT_A = {100};
constexpr Connection CLIENT_B = {200};
constexpr Connection CLIENT_C = {300};

constexpr auto USERNAME_A = "Able";
constexpr auto USERNAME_B = "Baker";
constexpr auto USERNAME_C = "Charlie";

constexpr auto CONFUSE_SPELL_NAME = "confuse";
constexpr auto BODY_SWAP_SPELL_NAME = "swap";

constexpr unsigned int CONFUSE_DURATION = 50;
constexpr unsigned int BODY_SWAP_DURATION = 50;

constexpr auto VALID_LENGTH_STRING = "Valid Input";

AccountHandler createAccountHandler() {
    AccountHandler accountHandler;

    // Register client A
    accountHandler.processRegistration(CLIENT_A);
    accountHandler.processRegistration(CLIENT_A, USERNAME_A);
    accountHandler.processRegistration(CLIENT_A, VALID_LENGTH_STRING);
    accountHandler.processRegistration(CLIENT_A, VALID_LENGTH_STRING);

    // Register client B
    accountHandler.processRegistration(CLIENT_B);
    accountHandler.processRegistration(CLIENT_B, USERNAME_B);
    accountHandler.processRegistration(CLIENT_B, VALID_LENGTH_STRING);
    accountHandler.processRegistration(CLIENT_B, VALID_LENGTH_STRING);

    return accountHandler;
}


TEST(MagicHandlerTestSuite, rejectInvalidSpellName) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    auto spellName = "invalid_spell_name";
    auto result = magicHandler.castSpell(CLIENT_A, spellName).back();

    std::ostringstream casterExpected;
    casterExpected << "There are no spells with the name of \"" << spellName << "\"\n";

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);
}


TEST(MagicHandlerTestSuite, canCastConfuseOnSelf) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_A;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto result = results.back();

    std::ostringstream casterExpected;
    casterExpected << "You cast Confuse on yourself.\n";

    EXPECT_TRUE(magicHandler.isConfused(CLIENT_A));
    EXPECT_FALSE(magicHandler.isConfused(CLIENT_B));

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);
}


TEST(MagicHandlerTestSuite, canCastConfuseOnOtherPlayerInSameRoom) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_B;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(2), results.size());

    auto casterResult = results.front();
    auto targetResult = results.back();

    std::ostringstream casterExpected;
    casterExpected << "You cast Confuse on " << USERNAME_B << "\n";

    std::ostringstream targetExpected;
    targetExpected<< USERNAME_A << " cast Confuse on you!" << "\n";

    EXPECT_FALSE(magicHandler.isConfused(CLIENT_A));
    EXPECT_TRUE(magicHandler.isConfused(CLIENT_B));

    EXPECT_EQ(CLIENT_A.id, casterResult.connection.id);
    EXPECT_EQ(casterExpected.str(), casterResult.text);

    EXPECT_EQ(CLIENT_B.id, targetResult.connection.id);
    EXPECT_EQ(targetExpected.str(), targetResult.text);
}


TEST(MagicHandlerTestSuite, cannotCastConfuseOnOtherPlayerInDifferentRoom) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    accountHandler.setRoomIdByClient(CLIENT_B, 42);
    ASSERT_NE(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_B;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto result = results.front();

    std::ostringstream casterExpected;
    casterExpected << "There is no player here with the name \"" << USERNAME_B << "\"\n";

    EXPECT_FALSE(magicHandler.isConfused(CLIENT_A));
    EXPECT_FALSE(magicHandler.isConfused(CLIENT_B));

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);
}


TEST(MagicHandlerTestSuite, cannotCastConfuseOnSelfWhileConfused) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_A;
    magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_TRUE(magicHandler.isConfused(CLIENT_A));
    ASSERT_FALSE(magicHandler.isConfused(CLIENT_B));

    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto result = results.front();

    std::ostringstream casterExpected;
    casterExpected << "You are already under the effects of the Confuse spell!\n";

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);
}


TEST(MagicHandlerTestSuite, cannotCastConfuseOnConfusedPlayer) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_B;
    magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_FALSE(magicHandler.isConfused(CLIENT_A));
    ASSERT_TRUE(magicHandler.isConfused(CLIENT_B));

    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto casterResult = results.front();

    std::ostringstream casterExpected;
    casterExpected<< USERNAME_B << " is already under the effects of the Confuse spell!\n";

    ASSERT_EQ(CLIENT_A.id, casterResult.connection.id);
    ASSERT_EQ(casterExpected.str(), casterResult.text);
}


TEST(MagicHandlerTestSuite, canConvertMessageToPigLatin) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    auto result = magicHandler.confuseSpeech("Hello, how are you?");

    std::ostringstream expected;
    expected << "elloHay, owhay areway ouyay?";

    EXPECT_EQ(expected.str(), result);
}


TEST(MagicHandlerTestSuite, canWaitUntilConfuseExpires) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    std::ostringstream argument;
    argument << CONFUSE_SPELL_NAME << " " << USERNAME_A;
    magicHandler.castSpell(CLIENT_A, argument.str());

    std::deque<Message> messages;
    for (int i = CONFUSE_DURATION; i >= 0; --i) {
        ASSERT_TRUE(magicHandler.isConfused(CLIENT_A));
        magicHandler.processCycle(messages);
    }

    EXPECT_FALSE(magicHandler.isConfused(CLIENT_A));
}


TEST(MagicHandlerTestSuite, cannotCastBodySwapOnSelf) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_A;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto result = results.back();

    std::ostringstream casterExpected;
    casterExpected << "You can't cast Body Swap on yourself!\n";

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);
}


TEST(MagicHandlerTestSuite, canCastBodySwapOnOtherPlayerInSameRoom) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_B;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(2), results.size());

    auto targetResult = results.front();
    auto casterResult = results.back();

    std::ostringstream casterExpected;
    casterExpected << "You have successfully swapped bodies with " << USERNAME_B << "\n";

    std::ostringstream targetExpected;
    targetExpected << USERNAME_A << " cast swap on you!\n";

    EXPECT_EQ(CLIENT_A.id, casterResult.connection.id);
    EXPECT_EQ(casterExpected.str(), casterResult.text);

    EXPECT_EQ(CLIENT_B.id, targetResult.connection.id);
    EXPECT_EQ(targetExpected.str(), targetResult.text);

    EXPECT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
    EXPECT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_B));
}


TEST(MagicHandlerTestSuite, cannotCastBodySwapOnOtherPlayerInDifferentRoom) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    accountHandler.setRoomIdByClient(CLIENT_B, 42);
    ASSERT_NE(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_B;
    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto result = results.front();

    std::ostringstream casterExpected;
    casterExpected << "There is no one here with the name \"" << USERNAME_B << "\"\n";

    EXPECT_EQ(CLIENT_A.id, result.connection.id);
    EXPECT_EQ(casterExpected.str(), result.text);

    EXPECT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_A).id);
    EXPECT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_B).id);
    EXPECT_FALSE(magicHandler.isBodySwapped(CLIENT_A));
    EXPECT_FALSE(magicHandler.isBodySwapped(CLIENT_B));
}


TEST(MagicHandlerTestSuite, cannotCastBodySwapOnWhileSelfIsSwapped) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_B;
    magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
    ASSERT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
    ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
    ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_B));

    auto results = magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto casterResult = results.front();

    std::ostringstream casterExpected;
    casterExpected << "You can't cast Body Swap while already under the effects of the spell!\n";

    EXPECT_EQ(CLIENT_A.id, casterResult.connection.id);
    EXPECT_EQ(casterExpected.str(), casterResult.text);

    EXPECT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
    EXPECT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_B));
}

TEST(MagicHandlerTestSuite, cannotCastBodySwapOnWhileOtherPlayerIsSwapped) {
    AccountHandler accountHandler = createAccountHandler();
    // Register client C
    accountHandler.processRegistration(CLIENT_C);
    accountHandler.processRegistration(CLIENT_C, USERNAME_C);
    accountHandler.processRegistration(CLIENT_C, VALID_LENGTH_STRING);
    accountHandler.processRegistration(CLIENT_C, VALID_LENGTH_STRING);

    MagicHandler magicHandler(accountHandler);

    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_B));
    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_A), accountHandler.getRoomIdByClient(CLIENT_C));
    ASSERT_EQ(accountHandler.getRoomIdByClient(CLIENT_B), accountHandler.getRoomIdByClient(CLIENT_C));

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_B;
    magicHandler.castSpell(CLIENT_A, argument.str());

    ASSERT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
    ASSERT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
    ASSERT_EQ(CLIENT_C.id, accountHandler.getClientByUsername(USERNAME_C).id);
    ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
    ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_B));
    ASSERT_FALSE(magicHandler.isBodySwapped(CLIENT_C));

    auto results = magicHandler.castSpell(CLIENT_C, argument.str());

    ASSERT_EQ(static_cast<unsigned int>(1), results.size());

    auto casterResult = results.front();

    std::ostringstream casterExpected;
    casterExpected << USERNAME_B << " is already under the effects of the Body Swap spell!\n";

    EXPECT_EQ(CLIENT_C.id, casterResult.connection.id);
    EXPECT_EQ(casterExpected.str(), casterResult.text);

    EXPECT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
    EXPECT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
    EXPECT_EQ(CLIENT_C.id, accountHandler.getClientByUsername(USERNAME_C).id);
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
    EXPECT_TRUE(magicHandler.isBodySwapped(CLIENT_B));
    EXPECT_FALSE(magicHandler.isBodySwapped(CLIENT_C));
}


TEST(MagicHandlerTestSuite, canWaitUntilBodySwapExpires) {
    AccountHandler accountHandler = createAccountHandler();
    MagicHandler magicHandler(accountHandler);

    std::ostringstream argument;
    argument << BODY_SWAP_SPELL_NAME << " " << USERNAME_B;
    magicHandler.castSpell(CLIENT_A, argument.str());

    std::deque<Message> messages;
    for (int i = BODY_SWAP_DURATION; i >= 0; --i) {
        ASSERT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_B).id);
        ASSERT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_A).id);
        ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_A));
        ASSERT_TRUE(magicHandler.isBodySwapped(CLIENT_B));
        magicHandler.processCycle(messages);
    }

    EXPECT_EQ(CLIENT_A.id, accountHandler.getClientByUsername(USERNAME_A).id);
    EXPECT_EQ(CLIENT_B.id, accountHandler.getClientByUsername(USERNAME_B).id);
    EXPECT_FALSE(magicHandler.isBodySwapped(CLIENT_A));
    EXPECT_FALSE(magicHandler.isBodySwapped(CLIENT_B));
}
