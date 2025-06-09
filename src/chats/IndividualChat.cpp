#include "IndividualChat.h"
#include <iostream>

IndividualChat::IndividualChat(const MyString& user1, const MyString& user2)
    : Chat(user1 + "_" + user2)
{
    addParticipant(user1);
    addParticipant(user2);
}

void IndividualChat::printChatInfo() const {
    std::cout << "Individual Chat: " << participants[0] << " and " << participants[1] << std::endl;
}
