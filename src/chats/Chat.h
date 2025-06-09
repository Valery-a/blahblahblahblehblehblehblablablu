#ifndef CHAT_H
#define CHAT_H

#include "MyVector.hpp"
#include "MyString.h"
#include "Message.h"

class Chat {
protected:
    MyString chatID;
    MyVector<MyString> participants;
    MyVector<Message> messages;

public:
    Chat(const MyString& chatID);
    virtual ~Chat() = default;

    const MyString& getID() const;
    const MyVector<MyString>& getParticipants() const;
    const MyVector<Message>& getMessages() const;

    virtual void addParticipant(const MyString& username);
    virtual void removeParticipant(const MyString& username);
    virtual bool hasParticipant(const MyString& username) const;

    void addMessage(const Message& message);
    void printMessages() const;

    virtual void printChatInfo() const = 0;
};

#endif
