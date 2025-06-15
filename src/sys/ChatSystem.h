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

    bool registerUser(const MyString& username, const MyString& password);
    bool registerUser(const MyString& username, const MyString& password, bool isAdmin);
    bool registerUser(const MyString& username, const MyString& password, bool isAdmin, const MyString& adminCode);
    bool login(const MyString& username, const MyString& password);
    void logout();

    MyVector<User*>&   getUsers();
    MyVector<Chat*>&   getChats();

    const MyVector<User*>& getUsers() const;
    const MyVector<Chat*>& getChats() const;

    User* getLoggedInUser() const;
    void viewGroupMembers(const MyString& chatID) const;

    bool createIndividualChat(const MyString& user2);
    bool createGroupChat(const MyString& groupName);
    
    void sendMessage(const MyString& chatID, const MyString& message);
    void viewMessages(const MyString& chatID);

    void selectChat(const MyString& chatID);

    void viewMyChats() const;
    void viewAllChats() const;

    bool addMemberToGroup(const MyString& chatID, const MyString& username);
    bool removeMemberFromGroup(const MyString& chatID, const MyString& username);

    bool leaveGroup(const MyString& chatID);

    bool addToGroup(const MyString& chatID, const MyString& username);
    bool kickFromGroup(const MyString& chatID, const MyString& username);

    bool setGroupAdmin(const MyString& chatID, const MyString& username);

    void groupStats(const MyString& chatID) const;

    bool deleteUser(const MyString& username);
    bool deleteChat(const MyString& chatID);

    void viewAllUsers() const;

    bool requestJoin(const MyString& chatID);
    bool listJoinRequests(const MyString& chatID) const;
    bool approveJoin(const MyString& chatID, const MyString& username);
    bool rejectJoin(const MyString& chatID, const MyString& username);
    bool setGroupOpen(const MyString& chatID, bool open);

    void initializeUserChats();

    static char* getArg(char*& p);
};

#endif
