#ifndef INDIVIDUALCHAT_H
#define INDIVIDUALCHAT_H

#include "Chat.h"

class IndividualChat : public Chat {
public:
    IndividualChat(const MyString& user1, const MyString& user2);

    void printChatInfo() const override;
};

#endif
