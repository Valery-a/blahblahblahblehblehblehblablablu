#include "GroupChat.h"
#include <iostream>

GroupChat::GroupChat(const MyString& id,
                     const MyString& groupName,
                     const MyString& creator)
  : Chat(id),
    groupName(groupName),
    creator(creator),
    membershipOpen(false)
{
    addParticipant(creator);
    addAdmin(creator);
}

const MyString& GroupChat::getCreator() const {
    return creator;
}

const MyString& GroupChat::getGroupName() const {
    return groupName;
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
    if (hasParticipant(username) && !isAdmin(username)) {
        admins.push_back(username);
    }
}

bool GroupChat::isAdmin(const MyString& username) const {
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i] == username) return true;
    }
    return false;
}

bool GroupChat::isMembershipOpen() const {
    return membershipOpen;
}

void GroupChat::setMembershipOpen(bool open) {
    membershipOpen = open;
}

const MyVector<MyString>& GroupChat::getPendingRequests() const {
    return pendingRequests;
}

bool GroupChat::addJoinRequest(const MyString& username) {
    if (hasParticipant(username)) return false;
    for (size_t i = 0; i < pendingRequests.size(); ++i)
        if (pendingRequests[i] == username)
            return false;
    pendingRequests.push_back(username);
    return true;
}

bool GroupChat::approveJoinRequest(const MyString& username) {
    for (size_t i = 0; i < pendingRequests.size(); ++i) {
        if (pendingRequests[i] == username) {
            pendingRequests.removeAt(i);
            addParticipant(username);
            return true;
        }
    }
    return false;
}

bool GroupChat::rejectJoinRequest(const MyString& username) {
    for (size_t i = 0; i < pendingRequests.size(); ++i) {
        if (pendingRequests[i] == username) {
            pendingRequests.removeAt(i);
            return true;
        }
    }
    return false;
}

void GroupChat::printChatInfo() const {
    std::cout << "Group Chat: " << groupName
              << " (" << getID().c_str() << ")\n";

    std::cout << "  Creator:  " << creator.c_str() << "\n";

    std::cout << "  Co-leaders: ";
    if (admins.empty()) std::cout << "None";
    else {
        for (size_t i = 0; i < admins.size(); ++i) {
            std::cout << admins[i].c_str();
            if (i+1 < admins.size()) std::cout << ", ";
        }
    }
    std::cout << "\n";

    std::cout << "  Membership: "
              << (membershipOpen ? "open" : "closed")
              << "\n";

    std::cout << "  Members (" << participants.size() << "): ";
    for (size_t i = 0; i < participants.size(); ++i) {
        std::cout << participants[i].c_str();
        if (i+1 < participants.size()) std::cout << ", ";
    }
    std::cout << "\n";

    if (!pendingRequests.empty()) {
        std::cout << "  Pending Requests: ";
        for (size_t i = 0; i < pendingRequests.size(); ++i) {
            std::cout << pendingRequests[i].c_str();
            if (i+1 < pendingRequests.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }
}
