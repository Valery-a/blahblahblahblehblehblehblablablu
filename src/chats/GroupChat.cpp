#include "GroupChat.h"
#include <iostream>

GroupChat::GroupChat(const MyString& id, const MyString& groupName, const MyString& creator)
    : Chat(id), groupName(groupName) {
    addParticipant(creator);
    addAdmin(creator);
}

void GroupChat::addParticipant(const MyString& username) {
    if (!hasParticipant(username)) {
        participants.push_back(username);
    }
}

void GroupChat::removeParticipant(const MyString& username) {
    for (size_t i = 0; i < participants.size(); ++i) {
        if (participants[i] == username) {
            participants.removeAt(i);
            break;
        }
    }

    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i] == username) {
            admins.removeAt(i);
            break;
        }
    }
}

void GroupChat::addAdmin(const MyString& username) {
    if (!isAdmin(username)) {
        admins.push_back(username);
    }
}

bool GroupChat::isAdmin(const MyString& username) const {
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i] == username) {
            return true;
        }
    }
    return false;
}

void GroupChat::printChatInfo() const {
    std::cout << "Group Chat: " << groupName << " (" << chatID << ")" << std::endl;
    std::cout << "Participants: ";
    for (size_t i = 0; i < participants.size(); ++i) {
        std::cout << participants[i];
        if (i < participants.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

const MyString& GroupChat::getGroupName() const {
    return groupName;
}
