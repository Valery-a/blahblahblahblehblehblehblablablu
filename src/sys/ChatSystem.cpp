// src/sys/ChatSystem.cpp
#include "ChatSystem.h"
#include "../storage/FileManager.h"
#include <algorithm>
#include <iostream>
#include <map>

ChatSystem::ChatSystem() : loggedInUser(nullptr) {}
ChatSystem::~ChatSystem() {
    for (size_t i = 0; i < users.size(); ++i) delete users[i];
    for (size_t i = 0; i < chats.size(); ++i) delete chats[i];
}

User* ChatSystem::findUser(const MyString& username) {
    for (size_t i = 0; i < users.size(); ++i)
        if (users[i]->getUsername() == username)
            return users[i];
    return nullptr;
}

Chat* ChatSystem::findChat(const MyString& chatID) const {
    for (size_t i = 0; i < chats.size(); ++i)
        if (chats[i]->getID() == chatID)
            return chats[i];
    return nullptr;
}

// -- register / login / logout ----------------------------------

bool ChatSystem::registerUser(const MyString& u, const MyString& p) {
    return registerUser(u, p, false, "");
}
bool ChatSystem::registerUser(const MyString& u, const MyString& p, bool isAdmin) {
    return registerUser(u, p, isAdmin, "");
}
bool ChatSystem::registerUser(const MyString& u, const MyString& p, bool isAdmin, const MyString& code) {
    if (findUser(u)) return false;
    if (isAdmin) users.push_back(new Admin(u, p, code));
    else         users.push_back(new User(u, p));
    return true;
}

bool ChatSystem::login(const MyString& u, const MyString& p) {
    User* usr = findUser(u);
    if (usr && usr->checkPassword(p)) {
        loggedInUser = usr;
        return true;
    }
    return false;
}

void ChatSystem::logout() {
    loggedInUser = nullptr;
}

// -- create chats -----------------------------------------------

bool ChatSystem::createIndividualChat(const MyString& other) {
    if (!loggedInUser) return false;
    User* o = findUser(other);
    if (!o) return false;
    MyString id = loggedInUser->getUsername() + "_" + other;
    if (findChat(id)) return false;
    Chat* c = new IndividualChat(loggedInUser->getUsername(), other);
    chats.push_back(c);
    loggedInUser->addChat(id);
    o->addChat(id);
    return true;
}

bool ChatSystem::createGroupChat(const MyString& groupName) {
    if (!loggedInUser) return false;
    MyString id = groupName + "_group";
    if (findChat(id)) return false;
    GroupChat* g = new GroupChat(id, groupName, loggedInUser->getUsername());
    chats.push_back(g);
    loggedInUser->addChat(id);
    return true;
}

// -- send & view ------------------------------------------------

void ChatSystem::sendMessage(const MyString& id, const MyString& msg) {
    if (!loggedInUser) return;
    Chat* c = findChat(id);
    if (!c || !c->hasParticipant(loggedInUser->getUsername())) return;
    c->addMessage(Message(loggedInUser->getUsername(), msg));
}

void ChatSystem::viewMessages(const MyString& id) const {
    if (!loggedInUser) return;
    Chat* c = findChat(id);
    if (c && c->hasParticipant(loggedInUser->getUsername()))
        c->printMessages();
}

// -- “select-chat”: view + prompt for a new message ----------

void ChatSystem::selectChat(const MyString& id) {
    viewMessages(id);
    if (!loggedInUser) return;
    std::cout << "Enter message: ";
    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        sendMessage(id, MyString(line.c_str()));
    }
}

// -- listing chats ---------------------------------------------

void ChatSystem::viewMyChats() const {
    if (!loggedInUser) return;
    const auto& uc = loggedInUser->getChats();
    for (size_t i = 0; i < uc.size(); ++i) {
        std::cout << "- " << uc[i].chatID.c_str() << "\n";
    }
}

void ChatSystem::viewAllChats() const {
    for (size_t i = 0; i < chats.size(); ++i) {
        Chat* c = chats[i];
        const auto& id = c->getID();
        size_t msgCount = c->getMessages().size();
        if (auto* g = dynamic_cast<GroupChat*>(c)) {
            std::cout << id.c_str() << ": "
                      << g->getGroupName().c_str()
                      << " (" << msgCount << " msg)\n";
        } else {
            std::cout << id.c_str()
                      << " (" << msgCount << " msg)\n";
        }
    }
}

// -- group membership ------------------------------------------

bool ChatSystem::addMemberToGroup(const MyString& chatID, const MyString& user) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    User* u = findUser(user);
    if (!g || !u) return false;
    if (!g->isAdmin(loggedInUser->getUsername()) && !loggedInUser->isAdmin())
        return false;
    g->addParticipant(user);
    u->addChat(chatID);
    return true;
}

bool ChatSystem::removeMemberFromGroup(const MyString& chatID, const MyString& user) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    User* u = findUser(user);
    if (!g || !u) return false;
    if (!g->isAdmin(loggedInUser->getUsername()) && !loggedInUser->isAdmin())
        return false;
    g->removeParticipant(user);
    u->removeChat(chatID);
    return true;
}

// -- leave-group, add-to-group, kick-from-group aliases --------

bool ChatSystem::leaveGroup(const MyString& chatID) {
    if (!loggedInUser) return false;
    return removeMemberFromGroup(chatID, loggedInUser->getUsername());
}

bool ChatSystem::addToGroup(const MyString& chatID, const MyString& username) {
    return addMemberToGroup(chatID, username);
}

bool ChatSystem::kickFromGroup(const MyString& chatID, const MyString& username) {
    return removeMemberFromGroup(chatID, username);
}

// -- set someone as group-admin --------------------------------

bool ChatSystem::setGroupAdmin(const MyString& chatID, const MyString& username) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g || !g->hasParticipant(username)) return false;
    if (!g->isAdmin(loggedInUser->getUsername()) && !loggedInUser->isAdmin())
        return false;
    g->addAdmin(username);
    return true;
}

// -- group-stats ------------------------------------------------

void ChatSystem::groupStats(const MyString& chatID) const {
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) {
        std::cout << "Group not found.\n";
        return;
    }
    size_t memberCount = g->getParticipants().size();
    size_t msgCount    = g->getMessages().size();

    // tally per-sender counts
    std::map<std::string,int> freq;
    const auto& msgs = g->getMessages();
    for (size_t i = 0; i < msgs.size(); ++i)
        freq[ msgs[i].getSender().c_str() ]++;

    // find top
    auto top = std::max_element(freq.begin(), freq.end(),
        [](auto& a, auto& b){ return a.second < b.second; });

    std::cout << g->getGroupName().c_str() << ": "
              << memberCount << " members, "
              << msgCount    << " msg, Top: "
              << top->first << " (" << top->second << " msg)\n";
}

// -- delete-user / delete-chat enhancements --------------------

bool ChatSystem::deleteUser(const MyString& username) {
    if (!loggedInUser || !loggedInUser->isAdmin()) return false;
    // remove from users[]
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->getUsername() == username) {
            delete users[i];
            users.removeAt(i);
            break;
        }
    }
    // scrub from all chats
    for (size_t i = 0; i < chats.size(); ++i)
        chats[i]->removeParticipant(username);
    return true;
}

bool ChatSystem::deleteChat(const MyString& chatID) {
    if (!loggedInUser || !loggedInUser->isAdmin()) return false;
    // remove from chats[]
    for (size_t i = 0; i < chats.size(); ++i) {
        if (chats[i]->getID() == chatID) {
            delete chats[i];
            chats.removeAt(i);
            break;
        }
    }
    // scrub from every user’s chatData
    for (size_t i = 0; i < users.size(); ++i)
        users[i]->removeChat(chatID);
    return true;
}

void ChatSystem::viewAllUsers() const {
    for (size_t i = 0; i < users.size(); ++i) {
        User* u = users[i];
        std::cout << "- " << u->getUsername().c_str();
        if (u->isAdmin()) std::cout << " (admin)";
        std::cout << "\n";
    }
}

User* ChatSystem::getLoggedInUser() const {
    return loggedInUser;
}
