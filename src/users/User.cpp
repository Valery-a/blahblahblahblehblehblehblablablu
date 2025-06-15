#include "User.h"

User::User() : username(""), password("") {}

User::User(const MyString& username, const MyString& password)
    : username(username), password(password) {}

const MyString& User::getUsername() const {
    return username;
}

const MyString& User::getPassword() const {
    return password;
}

bool User::checkPassword(const MyString& pass) const {
    return password == pass;
}

void User::addChat(const MyString& chatID) {
    for (size_t i = 0; i < chatData.size(); ++i) {
        if (chatData[i].chatID == chatID)
            return;
    }
    UserChat chat{chatID, MyVector<Message>()};
    chatData.push_back(chat);
}

void User::removeChat(const MyString& chatID) {
    for (size_t i = 0; i < chatData.size(); ++i) {
        if (chatData[i].chatID == chatID) {
            chatData.removeAt(i);
            return;
        }
    }
}

const MyVector<UserChat>& User::getChats() const {
    return chatData;
}

bool User::isAdmin() const {
    return false;
}

MyVector<UserChat>& User::accessChats() {
    return chatData;
}