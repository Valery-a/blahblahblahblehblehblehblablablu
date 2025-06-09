#ifndef USER_H
#define USER_H

#include "../util/MyString.h"
#include "../util/MyVector.hpp"
#include "../chats/Message.h"

struct UserChat {
    MyString chatID;
    MyVector<Message> messages;
};

class User {
protected:
    MyString username;
    MyString password;
    MyVector<UserChat> chatData;

public:
    User();
    User(const MyString& username, const MyString& password);

    const MyString& getUsername() const;
    const MyString& getPassword() const;
    bool checkPassword(const MyString& pass) const;

    void addChat(const MyString& chatID);
    void removeChat(const MyString& chatID);
    const MyVector<UserChat>& getChats() const;

    virtual bool isAdmin() const;
    virtual ~User() = default;
};

#endif
// използваме композиция защото User притежава и управлява chatData и не може chatData да съществува без user-a
//Когато потребителят бъде унищожен, и списъкът с неговите чатове (референции към ID-та или локални данни) изчезва.
