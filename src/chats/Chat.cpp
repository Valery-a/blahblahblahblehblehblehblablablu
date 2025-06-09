#include "Chat.h"
#include <iostream>

Chat::Chat(const MyString& chatID) : chatID(chatID) {}

const MyString& Chat::getID() const {
    return chatID;
}

const MyVector<MyString>& Chat::getParticipants() const {
    return participants;
}

void Chat::addParticipant(const MyString& username) {
    for (size_t i = 0; i < participants.size(); ++i) {
        if (participants[i] == username) return;
    }
    participants.push_back(username);
}

void Chat::removeParticipant(const MyString& username) {
    for (size_t i = 0; i < participants.size(); ++i) {
        if (participants[i] == username) {
            participants.removeAt(i);
            return;
        }
    }
}

const MyVector<Message>& Chat::getMessages() const {
    return messages;
}

bool Chat::hasParticipant(const MyString& username) const {
    for (size_t i = 0; i < participants.size(); ++i) {
        if (participants[i] == username) return true;
    }
    return false;
}

void Chat::addMessage(const Message& message) {
    messages.push_back(message);
}

void Chat::printMessages() const {
    for (size_t i = 0; i < messages.size(); ++i) {
        messages[i].print();
    }
}
