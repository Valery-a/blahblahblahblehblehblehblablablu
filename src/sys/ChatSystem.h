// src/sys/ChatSystem.h
#ifndef CHATSYSTEM_H
#define CHATSYSTEM_H

#include "../util/MyString.h"
#include "../util/MyVector.hpp"
#include "../users/User.h"
#include "../users/Admin.h"
#include "../chats/Chat.h"
#include "../chats/IndividualChat.h"
#include "../chats/GroupChat.h"

class ChatSystem {
private:
    MyVector<User*> users;
    MyVector<Chat*> chats;
    User* loggedInUser;

    User* findUser(const MyString& username);
    Chat* findChat(const MyString& chatID) const;

public:
    ChatSystem();
    ~ChatSystem();

    // user/admin management
    bool registerUser(const MyString& username, const MyString& password);
    bool registerUser(const MyString& username, const MyString& password, bool isAdmin);
    bool registerUser(const MyString& username, const MyString& password, bool isAdmin, const MyString& adminCode);
    bool login(const MyString& username, const MyString& password);
    void logout();

    // chat creation
    bool createIndividualChat(const MyString& user2);
    bool createGroupChat(const MyString& groupName);

    // messaging
    void sendMessage(const MyString& chatID, const MyString& message);
    void viewMessages(const MyString& chatID) const;

    // this is the “select-chat” entrypoint
    void selectChat(const MyString& chatID);

    // list chats
    void viewMyChats() const;
    void viewAllChats() const;    // richer output for admins

    // group membership
    bool addMemberToGroup(const MyString& chatID, const MyString& username);
    bool removeMemberFromGroup(const MyString& chatID, const MyString& username);

    // the “leave-group” command
    bool leaveGroup(const MyString& chatID);

    // aliases for add/remove
    bool addToGroup(const MyString& chatID, const MyString& username);
    bool kickFromGroup(const MyString& chatID, const MyString& username);

    // new: set someone as group-admin
    bool setGroupAdmin(const MyString& chatID, const MyString& username);

    // new: show “group-stats”
    void groupStats(const MyString& chatID) const;

    // administration (system-admin only)
    bool deleteUser(const MyString& username); // also remove from all chats
    bool deleteChat(const MyString& chatID);   // aka delete-group, also remove from users

    void viewAllUsers() const;

    User* getLoggedInUser() const;
};

#endif // CHATSYSTEM_H
