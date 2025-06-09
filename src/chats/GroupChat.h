#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include "Chat.h"

class GroupChat : public Chat {
private:
    MyString groupName;
    MyVector<MyString> admins;

public:
    GroupChat(const MyString& id, const MyString& groupName, const MyString& creator);

    void addParticipant(const MyString& username) override;
    void removeParticipant(const MyString& username) override;

    void addAdmin(const MyString& username);
    bool isAdmin(const MyString& username) const;

    void printChatInfo() const override;
    const MyString& getGroupName() const;
};

#endif
