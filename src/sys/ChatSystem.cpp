#include "ChatSystem.h"
#include "../storage/FileManager.h"
#include <iostream>

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

bool ChatSystem::registerUser(const MyString& u, const MyString& p) {
    return registerUser(u, p, false, "");
}
bool ChatSystem::registerUser(const MyString& u, const MyString& p, bool isAdmin) {
    return registerUser(u, p, isAdmin, "");
}
bool ChatSystem::registerUser(const MyString& u,
                             const MyString& p,
                             bool isAdmin,
                             const MyString& code)
{
    if (findUser(u)) 
        return false;

    if (isAdmin) {
        if (code != Admin::MASTER_CODE) {
            return false;
        }
        users.push_back(new Admin(u, p, code));
    }
    else {
        users.push_back(new User(u, p));
    }
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

void ChatSystem::selectChat(const MyString& id) {
    viewMessages(id);
    if (!loggedInUser) return;
    std::cout << "Enter message: ";
    char buf[2048];
    if (std::cin.getline(buf, sizeof(buf)) && buf[0] != '\0')
        sendMessage(id, MyString(buf));
}


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

bool ChatSystem::removeMemberFromGroup(const MyString& chatID, const MyString& username) {
    if (!loggedInUser) 
        return false;

    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    User* target = findUser(username);

    if (!g || !target) 
        return false;

    if (!g->hasParticipant(username)) 
        return false;

    bool execIsSysAdmin   = loggedInUser->isAdmin();
    bool execIsGroupAdmin = g->isAdmin(loggedInUser->getUsername());
    if (!execIsSysAdmin && !execIsGroupAdmin) 
        return false;

    if (target->isAdmin() && !execIsSysAdmin) 
        return false;

    const MyString& creator     = g->getCreator();
    bool execIsCreator          = (creator == loggedInUser->getUsername());
    bool tgtIsCreator           = (creator == username);

    if (execIsGroupAdmin && !execIsCreator) {
        if (tgtIsCreator)         return false;
        if (g->isAdmin(username)) return false;
    }

    g->removeParticipant(username);
    target->removeChat(chatID);
    return true;
}

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

void ChatSystem::groupStats(const MyString& chatID) const {
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) {
        std::cout << "Group not found.\n";
        return;
    }

    size_t memberCount = g->getParticipants().size();
    size_t msgCount    = g->getMessages().size();

    MyVector<MyString>  senders;
    MyVector<size_t>    counts;

    const auto& msgs = g->getMessages();
    for (size_t i = 0; i < msgs.size(); ++i) {
        MyString sender = msgs[i].getSender();
        bool found = false;
        for (size_t j = 0; j < senders.size(); ++j) {
            if (senders[j] == sender) {
                counts[j] += 1;
                found = true;
                break;
            }
        }
        if (!found) {
            senders.push_back(sender);
            counts.push_back(1);
        }
    }

    size_t topIdx = 0;
    for (size_t j = 1; j < counts.size(); ++j) {
        if (counts[j] > counts[topIdx]) {
            topIdx = j;
        }
    }

    if (counts.empty()) {
        std::cout
            << g->getGroupName().c_str() << ": "
            << memberCount << " members, "
            << msgCount    << " msg, "
            << "Top: N/A\n";
    } else {
        std::cout
            << g->getGroupName().c_str() << ": "
            << memberCount << " members, "
            << msgCount    << " msg, Top: "
            << senders[topIdx].c_str()
            << " (" << counts[topIdx] << " msg)\n";
    }
}


bool ChatSystem::deleteUser(const MyString& username) {
    if (!loggedInUser || !loggedInUser->isAdmin()) return false;
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->getUsername() == username) {
            delete users[i];
            users.removeAt(i);
            break;
        }
    }
    for (size_t i = 0; i < chats.size(); ++i)
        chats[i]->removeParticipant(username);
    return true;
}

bool ChatSystem::deleteChat(const MyString& chatID) {
    if (!loggedInUser || !loggedInUser->isAdmin()) return false;
    for (size_t i = 0; i < chats.size(); ++i) {
        if (chats[i]->getID() == chatID) {
            delete chats[i];
            chats.removeAt(i);
            break;
        }
    }
    for (size_t i = 0; i < users.size(); ++i)
        users[i]->removeChat(chatID);
    return true;
}

void ChatSystem::viewAllUsers() const {
    MyVector<MyString> seen;

    for (size_t i = 0; i < users.size(); ++i) {
        User* u = users[i];
        MyString name = u->getUsername();

        bool already = false;
        for (size_t j = 0; j < seen.size(); ++j) {
            if (seen[j] == name) {
                already = true;
                break;
            }
        }

        if (!already) {
            seen.push_back(name);
            std::cout << "- " << name.c_str();
            if (u->isAdmin())
                std::cout << " (admin)";
            std::cout << "\n";
        }
    }
}

User* ChatSystem::getLoggedInUser() const {
    return loggedInUser;
}

MyVector<User*>& ChatSystem::getUsers() {
    return users;
}

MyVector<Chat*>& ChatSystem::getChats() {
    return chats;
}

const MyVector<User*>& ChatSystem::getUsers() const {
    return users;
}

const MyVector<Chat*>& ChatSystem::getChats() const {
    return chats;
}

void ChatSystem::viewGroupMembers(const MyString& chatID) const {
    if (!loggedInUser) {
        std::cout << "Please login first.\n";
        return;
    }

    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) {
        std::cout << "Group not found.\n";
        return;
    }

    const MyString& me = loggedInUser->getUsername();
    bool isMember   = g->hasParticipant(me);
    bool isSysAdmin = loggedInUser->isAdmin();
    if (!isMember && !isSysAdmin) {
        std::cout << "Unauthorized or not a member of this group.\n";
        return;
    }

    std::cout << "Members of " << chatID.c_str() << ":\n";
    const MyString& creator = g->getCreator();
    for (size_t i = 0; i < g->getParticipants().size(); ++i) {
        const auto& m = g->getParticipants()[i];
        std::cout << "- " << m.c_str();
        if (m == creator) {
            std::cout << " (creator)";
        } else if (g->isAdmin(m)) {
            std::cout << " (admin)";
        }
        std::cout << "\n";
    }
}

bool ChatSystem::requestJoin(const MyString& chatID) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) return false;

    const MyString& me = loggedInUser->getUsername();
    if (g->hasParticipant(me))               return false;
    if (g->isMembershipOpen()) {
        g->addParticipant(me);
        loggedInUser->addChat(chatID);
        return true;
    } else {
        return g->addJoinRequest(me);
    }
}

bool ChatSystem::listJoinRequests(const MyString& chatID) const {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) return false;

    const MyString& me = loggedInUser->getUsername();
    bool amSysAdmin   = loggedInUser->isAdmin();
    bool amGroupAdmin = g->isAdmin(me);
    bool amCreator    = (g->getCreator() == me);
    if (!amSysAdmin && !amGroupAdmin && !amCreator) return false;

    const auto& reqs = g->getPendingRequests();
    std::cout << "Pending for " << chatID.c_str() << ":\n";
    for (size_t i = 0; i < reqs.size(); ++i) {
        std::cout << "- " << reqs[i].c_str() << "\n";
    }
    return true;
}

bool ChatSystem::approveJoin(const MyString& chatID, const MyString& username) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    User* u = findUser(username);
    if (!g || !u) return false;

    const MyString& me = loggedInUser->getUsername();
    bool amSysAdmin   = loggedInUser->isAdmin();
    bool amGroupAdmin = g->isAdmin(me);
    bool amCreator    = (g->getCreator() == me);
    if (!amSysAdmin && !amGroupAdmin && !amCreator) return false;

    if (g->approveJoinRequest(username)) {
        u->addChat(chatID);
        return true;
    }
    return false;
}

bool ChatSystem::rejectJoin(const MyString& chatID, const MyString& username) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) return false;

    const MyString& me = loggedInUser->getUsername();
    bool amSysAdmin   = loggedInUser->isAdmin();
    bool amGroupAdmin = g->isAdmin(me);
    bool amCreator    = (g->getCreator() == me);
    if (!amSysAdmin && !amGroupAdmin && !amCreator) return false;

    return g->rejectJoinRequest(username);
}

bool ChatSystem::setGroupOpen(const MyString& chatID, bool open) {
    if (!loggedInUser) return false;
    Chat* c = findChat(chatID);
    auto* g = dynamic_cast<GroupChat*>(c);
    if (!g) return false;

    const MyString& me = loggedInUser->getUsername();
    bool amSysAdmin   = loggedInUser->isAdmin();
    bool amCreator    = (g->getCreator() == me);
    if (!amSysAdmin && !amCreator) return false;

    g->setMembershipOpen(open);
    return true;
}

void ChatSystem::initializeUserChats() {
    for (size_t ci = 0; ci < chats.size(); ++ci) {
        Chat* c = chats[ci];
        const MyString& chatID = c->getID();
        const auto& parts = c->getParticipants();
        for (size_t pi = 0; pi < parts.size(); ++pi) {
            const MyString& uname = parts[pi];
            User* u = findUser(uname);
            if (u) {
                u->addChat(chatID);
            }
        }
    }
}

char* ChatSystem::getArg(char*& p) {
    if (!p) return nullptr;
    while (*p == ' ') ++p;
    if (*p == '\0') return nullptr;
    char* start = p;
    while (*p != ' ' && *p != '\0') ++p;
    if (*p == ' ') { *p = '\0'; ++p; }
    return start;
}