#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include "Chat.h"

class GroupChat : public Chat {
private:
    MyString groupName;
    MyString creator;
    bool membershipOpen;
    MyVector<MyString> pendingRequests;
    MyVector<MyString> admins;

public:
    GroupChat(const MyString& id,
              const MyString& groupName,
              const MyString& creator);

    const MyString& getCreator() const;
    const MyString& getGroupName() const;

    void addParticipant(const MyString& username) override;
    void removeParticipant(const MyString& username) override;

    void addAdmin(const MyString& username);
    bool isAdmin(const MyString& username) const;

    bool isMembershipOpen() const;
    void setMembershipOpen(bool open);

    const MyVector<MyString>& getPendingRequests() const;
    bool addJoinRequest(const MyString& username);
    bool approveJoinRequest(const MyString& username);
    bool rejectJoinRequest(const MyString& username);

    void printChatInfo() const override;
};

#endif
