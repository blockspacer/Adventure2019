//
// Created by louis on 26/01/19.
//

#include "lib/gtest/gtest.h"
#include "lib/gmock/gmock.h"
#include "PlayerHandler.h"
#include "Response.h"
#include "Server.h"

using model::PlayerHandler;
using model::Response;
using networking::Connection;

/*
 *  Cases to test:
 *  1.  Start register and prompt for username
 *  2.  Enter long username
 *  3.  Enter valid username
 *  4.  Enter short password
 *  5.  Enter long password
 *  6.  Enter valid password
 *  7.  Enter non-matching re-entered-password
 *  8.  Enter matching re-entered-password / Successful registration
 *  9.  Logged in after registering
 *  10. Prevent registration with taken username on username input
 *  11. Prevent registration with taken username on second password input
 *  12. Remove appropriate 'reg' states if client disconnects while in registration process
 *  13. Start login and prompt for username
 *  14. Login prompts for password after entering username
 *  15. Enter invalid credentials in login
 *  16. Enter valid credentials in login
 *  17. Logout other client if same Player logged in by a client
 *  18. Remove appropriate 'login' states if client disconnects while in login process
 */
const Connection CLIENT_A = {100};
const Connection CLIENT_B = {200};

const unsigned short EXPECTED_MIN_PASSWORD_LENGTH = 4;
const unsigned short EXPECTED_MAX_USERNAME_AND_PASSWORD_LENGTH = 16;

const std::string validLengthString = "Valid Input";
const std::string longLengthString  = "Very very very long input";
const std::string shortLengthString = "SLS";


TEST(PlayerHandlerTestSuite, canStartRegistration) {
    PlayerHandler playerHandler{};

    auto result = playerHandler.processRegistration(CLIENT_A);

    std::ostringstream expect;
    expect << "\n"
           << "Register\n"
           << "--------\n"
           << "Enter your username (maximum of " << EXPECTED_MAX_USERNAME_AND_PASSWORD_LENGTH << " characters)\n";

    EXPECT_EQ(expect.str(), result);
    EXPECT_EQ(playerHandler.isRegistering(CLIENT_A), true);
}

TEST(PlayerHandlerTestSuite, canPreventLongUsername) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    auto result = playerHandler.processRegistration(CLIENT_A, longLengthString);

    EXPECT_EQ("The username you entered is too long. Registration process cancelled.\n", result);
}

TEST(PlayerHandlerTestSuite, canUseValidUsername) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    auto result = playerHandler.processRegistration(CLIENT_A, validLengthString);

    std::ostringstream expect;
    expect << validLengthString << "\n"
           << "Enter your password (minimum of " << EXPECTED_MIN_PASSWORD_LENGTH << " characters,"
           << " maximum of " << EXPECTED_MAX_USERNAME_AND_PASSWORD_LENGTH << " characters)\n";

    EXPECT_EQ(expect.str(), result);
}

TEST(PlayerHandlerTestSuite, canPreventShortPassword) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result = playerHandler.processRegistration(CLIENT_A, shortLengthString);

    EXPECT_EQ("The password you entered is too short. Registration process cancelled.\n", result);
}

TEST(PlayerHandlerTestSuite, canPreventLongPassword) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result = playerHandler.processRegistration(CLIENT_A, longLengthString);

    EXPECT_EQ("The password you entered is too long. Registration process cancelled.\n", result);
}

TEST(PlayerHandlerTestSuite, canUseValidPassword) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result =  playerHandler.processRegistration(CLIENT_A, validLengthString);

    EXPECT_EQ("Re-enter your password\n", result);
}

TEST(PlayerHandlerTestSuite, canDetectNonMatchingPassword) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result = playerHandler.processRegistration(CLIENT_A, "notMatch");

    EXPECT_EQ("The passwords you entered do not match. Registration process cancelled.\n", result);
}

TEST(PlayerHandlerTestSuite, canRegisterSuccessfully) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result = playerHandler.processRegistration(CLIENT_A, validLengthString);

    EXPECT_EQ("Your account has been successfully registered and you are now logged in.\n\n", result);
}

TEST(PlayerHandlerTestSuite, isLoggedInAfterRegister) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);

    EXPECT_EQ(playerHandler.isLoggingIn(CLIENT_A), false);
    EXPECT_EQ(playerHandler.isLoggedIn(CLIENT_A), true);
}

TEST(PlayerHandlerTestSuite, canDetectUsernameTakenOnUsernameEntry) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);

    playerHandler.processRegistration(CLIENT_B);
    auto result = playerHandler.processRegistration(CLIENT_B, validLengthString);

    EXPECT_EQ("The username \"" + validLengthString + "\" has already been taken, please use a different username.\n", result);
}

TEST(PlayerHandlerTestSuite, canDetectUsernameTakenOnPasswordReEntry) {
    PlayerHandler playerHandler{};

    // Start registration process
    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_B);

    // Enters usernames
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_B, validLengthString);

    // Enters passwords
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_B, validLengthString);

    // Re-enters passwords
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    auto result = playerHandler.processRegistration(CLIENT_B, validLengthString);

    EXPECT_EQ("The username \"" + validLengthString + "\" has already been taken, please use a different username.\n", result);
}

TEST(PlayerHandlerTestSuite, canRemoveClientFromRegisteringOnDisconnect) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.exitRegistration(CLIENT_A);
    auto result = playerHandler.isRegistering(CLIENT_A);

    EXPECT_EQ(result, false);
}

TEST(PlayerHandlerTestSuite, canStartLogin) {
    PlayerHandler playerHandler{};

    auto result = playerHandler.processLogin(CLIENT_A);

    EXPECT_EQ("\nLogin\n-----\nEnter your username\n", result);
    EXPECT_EQ(playerHandler.isLoggingIn(CLIENT_A), true);
}

TEST(PlayerHandlerTestSuite, canDetectFailedLogin) {
    PlayerHandler playerHandler{};

    playerHandler.processLogin(CLIENT_A);
    playerHandler.processLogin(CLIENT_A, validLengthString);
    auto result = playerHandler.processLogin(CLIENT_A, validLengthString);

    EXPECT_EQ("Invalid username or password.\n", result);
}

TEST(PlayerHandlerTestSuite, canLogInSuccessfully) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.logoutPlayer(CLIENT_A);

    playerHandler.processLogin(CLIENT_A);
    playerHandler.processLogin(CLIENT_A, validLengthString);
    auto result = playerHandler.processLogin(CLIENT_A, validLengthString);

    EXPECT_EQ("Logged in successfully!\n", result);
}

TEST(PlayerHandlerTestSuite, canLogoutClientOnOtherClientLogin) {
    PlayerHandler playerHandler{};

    playerHandler.processRegistration(CLIENT_A);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);
    playerHandler.processRegistration(CLIENT_A, validLengthString);

    playerHandler.processLogin(CLIENT_B);
    playerHandler.processLogin(CLIENT_B, validLengthString);
    playerHandler.processLogin(CLIENT_B, validLengthString);
    std::deque<Response> results = {};
    playerHandler.notifyBootedClients(results);

    EXPECT_EQ(CLIENT_A, results.front().client);
    EXPECT_EQ("You have been logged out due to being logged in elsewhere.\n", results.front().message);
    EXPECT_EQ(playerHandler.isLoggedIn(CLIENT_A), false);
    EXPECT_EQ(playerHandler.isLoggedIn(CLIENT_B), true);
}

TEST(PlayerHandlerTestSuite, canRemoveClientFromLoginOnDisconnect) {
    PlayerHandler playerHandler{};

    playerHandler.processLogin(CLIENT_A);
    playerHandler.processLogin(CLIENT_A, validLengthString);
    playerHandler.exitLogin(CLIENT_A);

    EXPECT_EQ(playerHandler.isLoggingIn(CLIENT_A), false);
}
