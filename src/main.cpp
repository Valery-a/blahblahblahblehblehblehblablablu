#include <iostream>
#include <sstream>
#include "sys/ChatSystem.h"
#include "storage/FileManager.h"
#include "chats/GroupChat.h"

int main() {
    ChatSystem app;

    // --- Load persistent state ---
    FileManager::loadUsers(app.getUsers(), "data/users.txt");
    if (FileManager::loadChatsBinary(app.getChats(), "data/chats.dat")) {
        FileManager::loadMessagesBinary(app.getChats(), "data/messages.dat");
    } else {
        FileManager::loadChats(app.getChats(), "data/chats.txt");
        FileManager::loadMessages(app.getChats(), "data/messages.txt");
    }

    std::string line;
    while (std::cout << "> ", std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            break;
        }

        else if (cmd == "register" || cmd == "create-account") {
            std::string u, p, role;
            if (!(iss >> u >> p)) {
                std::cout << "Usage: register <username> <password> [admin]\n";
                continue;
            }
            if (u.empty() || p.empty()) {
                std::cout << "Error: username and password cannot be empty.\n";
                continue;
            }
            iss >> role;
            if (role == "admin") {
                std::string code;
                std::cout << "Enter admin code: ";
                std::getline(std::cin, code);
                if (code.empty()) {
                    std::cout << "Error: admin code cannot be empty.\n";
                    continue;
                }
                bool ok = app.registerUser(
                    MyString(u.c_str()),
                    MyString(p.c_str()),
                    true,
                    MyString(code.c_str())
                );
                std::cout << (ok ? "Admin registered.\n" : "Registration failed.\n");
            } else {
                bool ok = app.registerUser(
                    MyString(u.c_str()),
                    MyString(p.c_str())
                );
                std::cout << (ok ? "User registered.\n" : "Username already taken.\n");
            }
        }

        else if (cmd == "login") {
            std::string u, p;
            if (!(iss >> u)) {
                std::cout << "Usage: login <username> [password]\n";
                continue;
            }
            if (!(iss >> p)) {
                std::cout << "Enter password: ";
                std::getline(std::cin, p);
            }
            if (p.empty()) {
                std::cout << "Error: password cannot be empty.\n";
                continue;
            }

            bool ok = app.login(MyString(u.c_str()), MyString(p.c_str()));
            if (ok) {
                std::cout << "Logged in as " << u << ".\n";
            } else {
                bool exists = false;
                for (size_t i = 0; i < app.getUsers().size(); ++i) {
                    if (app.getUsers()[i]->getUsername() == MyString(u.c_str())) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    std::cout << "User \"" << u << "\" not found. Register? (y/n): ";
                    std::string ans;
                    std::getline(std::cin, ans);
                    if (!ans.empty() && (ans[0]=='y' || ans[0]=='Y')) {
                        std::string newpass;
                        std::cout << "Enter password for new account: ";
                        std::getline(std::cin, newpass);
                        if (newpass.empty()) {
                            std::cout << "Error: password cannot be empty.\n";
                            continue;
                        }
                        bool ok2 = app.registerUser(
                            MyString(u.c_str()),
                            MyString(newpass.c_str())
                        );
                        std::cout << (ok2
                            ? "User registered. Please login again.\n"
                            : "Registration failed.\n");
                    }
                } else {
                    std::cout << "Invalid credentials.\n";
                }
            }
        }

        // LOGOUT
        else if (cmd == "logout") {
            app.logout();
            std::cout << "Logged out.\n";
        }

        // CREATE-INDIVIDUAL
        else if (cmd == "create-individual") {
            std::string other;
            if (!(iss >> other) || other.empty()) {
                std::cout << "Usage: create-individual <user>\n";
                continue;
            }
            bool ok = app.createIndividualChat(MyString(other.c_str()));
            std::cout << (ok ? "Chat created.\n" : "Chat could not be created.\n");
        }

        // CREATE-GROUP
        else if (cmd == "create-group") {
            std::string g;
            if (!(iss >> g) || g.empty()) {
                std::cout << "Usage: create-group <group_name>\n";
                continue;
            }
            bool ok = app.createGroupChat(MyString(g.c_str()));
            std::cout << (ok ? "Group created.\n" : "Group could not be created.\n");
        }

        else if (cmd == "request-join") {
        std::string cid; 
        if (!(iss >> cid)) { std::cout << "Usage: request-join <chat_id>\n"; continue; }
        bool ok = app.requestJoin(MyString(cid.c_str()));
        std::cout << (ok ? "Request sent or joined.\n" : "Cannot request join.\n");
    }

    else if (cmd == "list-requests") {
        std::string cid;
        if (!(iss >> cid)) { std::cout << "Usage: list-requests <chat_id>\n"; continue; }
        if (!app.listJoinRequests(MyString(cid.c_str())))
            std::cout << "Unauthorized or no such group.\n";
    }

    else if (cmd == "approve-join") {
        std::string cid, user;
        if (!(iss >> cid >> user)) { std::cout << "Usage: approve-join <chat_id> <username>\n"; continue; }
        bool ok = app.approveJoin(MyString(cid.c_str()), MyString(user.c_str()));
        std::cout << (ok ? "User approved.\n" : "Cannot approve.\n");
    }

    else if (cmd == "reject-join") {
        std::string cid, user;
        if (!(iss >> cid >> user)) { std::cout << "Usage: reject-join <chat_id> <username>\n"; continue; }
        bool ok = app.rejectJoin(MyString(cid.c_str()), MyString(user.c_str()));
        std::cout << (ok ? "Request rejected.\n" : "Cannot reject.\n");
    }

    else if (cmd == "set-membership") {
        std::string cid, mode;
        if (!(iss >> cid >> mode) || (mode != "open" && mode != "closed")) {
            std::cout << "Usage: set-membership <chat_id> <open|closed>\n";
            continue;
        }
        bool open = (mode == "open");
        bool ok = app.setGroupOpen(MyString(cid.c_str()), open);
        std::cout << (ok
            ? std::string("Membership now ") + mode + ".\n"
            : "Cannot change membership mode.\n");
    }

        // SELECT-CHAT
        else if (cmd == "select-chat") {
            std::string cid;
            if (!(iss >> cid) || cid.empty()) {
                std::cout << "Usage: select-chat <chat_id>\n";
                continue;
            }
            app.selectChat(MyString(cid.c_str()));
        }

        // SEND
        else if (cmd == "send") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: send <chat_id> <message>\n";
                continue;
            }
            std::string msg;
            std::getline(iss, msg);
            if (msg.empty() || (msg.size() == 1 && msg[0] == ' ')) {
                std::cout << "Error: message cannot be empty.\n";
                continue;
            }
            if (msg.front() == ' ') msg.erase(0,1);
            app.sendMessage(MyString(chatID.c_str()), MyString(msg.c_str()));
        }

        // VIEW-MESSAGES
        else if (cmd == "view-messages") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: view-messages <chat_id>\n";
                continue;
            }
            app.viewMessages(MyString(chatID.c_str()));
        }

        // VIEW-CHATS
        else if (cmd == "view-chats") {
            if (!app.getLoggedInUser()) {
                std::cout << "Please login first.\n";
                continue;
            }
            app.viewMyChats();
        }

        // VIEW-MEMBERS
        else if (cmd == "view-members") {
            std::string cid;
            if (!(iss >> cid) || cid.empty()) {
                std::cout << "Usage: view-members <chat_id>\n";
                continue;
            }
            app.viewGroupMembers(MyString(cid.c_str()));
        }

        // ADD-MEMBER
        else if (cmd == "add-member") {
            std::string chatID, user;
            if (!(iss >> chatID >> user) || chatID.empty() || user.empty()) {
                std::cout << "Usage: add-member <chat_id> <user>\n";
                continue;
            }
            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i)
                if (app.getChats()[i]->getID() == MyString(chatID.c_str()))
                    c = app.getChats()[i];
            if (!c || dynamic_cast<GroupChat*>(c) == nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }
            User* u = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i)
                if (app.getUsers()[i]->getUsername() == MyString(user.c_str()))
                    u = app.getUsers()[i];
            if (!u) {
                std::cout << "User \"" << user << "\" does not exist.\n";
                continue;
            }
            if (c->hasParticipant(MyString(user.c_str()))) {
                std::cout << "User \"" << user << "\" is already in group.\n";
                continue;
            }
            bool ok = app.addMemberToGroup(MyString(chatID.c_str()), MyString(user.c_str()));
            std::cout << (ok
                ? "Member added.\n"
                : "Failed. Must be group-admin or system-admin.\n");
        }

        // REMOVE-MEMBER
        else if (cmd == "remove-member") {
            std::string chatID, user;
            if (!(iss >> chatID >> user) || chatID.empty() || user.empty()) {
                std::cout << "Usage: remove-member <chat_id> <user>\n";
                continue;
            }
            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i)
                if (app.getChats()[i]->getID() == MyString(chatID.c_str()))
                    c = app.getChats()[i];
            if (!c || dynamic_cast<GroupChat*>(c) == nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }
            User* u = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i)
                if (app.getUsers()[i]->getUsername() == MyString(user.c_str()))
                    u = app.getUsers()[i];
            if (!u) {
                std::cout << "User \"" << user << "\" does not exist.\n";
                continue;
            }
            if (!c->hasParticipant(MyString(user.c_str()))) {
                std::cout << "User \"" << user << "\" is not in group.\n";
                continue;
            }
            bool ok = app.removeMemberFromGroup(MyString(chatID.c_str()), MyString(user.c_str()));
            std::cout << (ok
                ? "Member removed.\n"
                : "Failed. Must be group-admin or system-admin.\n");
        }

        // ADD-TO-GROUP
        else if (cmd == "add-to-group") {
            std::string chatID, user;
            if (!(iss >> chatID >> user) || chatID.empty() || user.empty()) {
                std::cout << "Usage: add-to-group <chat_id> <user>\n";
                continue;
            }
            bool ok = app.addToGroup(MyString(chatID.c_str()), MyString(user.c_str()));
            std::cout << (ok
                ? user + std::string(" added to group!\n")
                : "Failed to add to group.\n");
        }

        // KICK-FROM-GROUP
        else if (cmd == "kick-from-group") {
            std::string chatID, user;
            if (!(iss >> chatID >> user) || chatID.empty() || user.empty()) {
                std::cout << "Usage: kick-from-group <chat_id> <user>\n";
                continue;
            }
            bool ok = app.kickFromGroup(MyString(chatID.c_str()), MyString(user.c_str()));
            std::cout << (ok
                ? user + std::string(" removed from group!\n")
                : "Failed to remove from group.\n");
        }

        // SET-GROUP-ADMIN
        else if (cmd == "set-group-admin") {
            std::string chatID, user;
            if (!(iss >> chatID >> user) || chatID.empty() || user.empty()) {
                std::cout << "Usage: set-group-admin <chat_id> <user>\n";
                continue;
            }
            bool ok = app.setGroupAdmin(MyString(chatID.c_str()), MyString(user.c_str()));
            std::cout << (ok
                ? user + std::string(" is now an admin!\n")
                : "Failed to set admin.\n");
        }

        // GROUP-STATS
        else if (cmd == "group-stats") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: group-stats <chat_id>\n";
                continue;
            }
            app.groupStats(MyString(chatID.c_str()));
        }

        // LEAVE-GROUP
        else if (cmd == "leave-group") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: leave-group <chat_id>\n";
                continue;
            }
            bool ok = app.leaveGroup(MyString(chatID.c_str()));
            std::cout << (ok
                ? "You left the group.\n"
                : "Could not leave group.\n");
        }

        // DELETE-USER
        else if (cmd == "delete-user") {
            std::string u;
            if (!(iss >> u) || u.empty()) {
                std::cout << "Usage: delete-user <username>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteUser(MyString(u.c_str()));
            std::cout << (ok
                ? "User " + u + " deleted!\n"
                : "Failed.\n");
        }

        // DELETE-CHAT
        else if (cmd == "delete-chat") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: delete-chat <chat_id>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteChat(MyString(chatID.c_str()));
            std::cout << (ok
                ? "Chat deleted.\n"
                : "Failed.\n");
        }

        // DELETE-GROUP
        else if (cmd == "delete-group") {
            std::string chatID;
            if (!(iss >> chatID) || chatID.empty()) {
                std::cout << "Usage: delete-group <chat_id>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteChat(MyString(chatID.c_str()));
            std::cout << (ok
                ? "Group deleted!\n"
                : "Failed.\n");
        }

        // VIEW-ALL-USERS
        else if (cmd == "view-all-users") {
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            app.viewAllUsers();
        }

        // VIEW-ALL-CHATS
        else if (cmd == "view-all-chats") {
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            app.viewAllChats();
        }

        // HELP
        else if (cmd == "help") {
            std::cout <<
"Available commands:\n"
"  register|create-account <username> <password> [admin]\n"
"  login <username> [password]\n"
"  logout\n"
"  create-individual <user>\n"
"  create-group <group_name>\n"
"  request-join <chat_id>\n"
"  list-requests <chat_id>\n"
"  approve-join <chat_id> <username>\n"
"  reject-join <chat_id> <username>\n"
"  set-membership <chat_id> <open|closed>\n"
"  select-chat <chat_id>\n"
"  send <chat_id> <message…>\n"
"  view-messages <chat_id>\n"
"  view-chats\n"
"  view-members <chat_id>\n"
"  add-member <chat_id> <user>\n"
"  remove-member <chat_id> <user>\n"
"  add-to-group <chat_id> <user>\n"
"  kick-from-group <chat_id> <user>\n"
"  set-group-admin <chat_id> <user>\n"
"  group-stats <chat_id>\n"
"  leave-group <chat_id>\n"
"  delete-user <username> (system-admin only)\n"
"  delete-chat <chat_id> (system-admin only)\n"
"  delete-group <chat_id> (system-admin only)\n"
"  view-all-users (system-admin only)\n"
"  view-all-chats (system-admin only)\n"
"  help\n"
"  exit\n";
        }

        // UNKNOWN
        else {
            std::cout << "Unknown command. Type help.\n";
        }
    }
    
    // --- Persist state ---
    FileManager::saveChats         (app.getChats(),         "data/chats.txt");
    FileManager::saveMessages      (app.getChats(),         "data/messages.txt");
    FileManager::saveChatsBinary   (app.getChats(),         "data/chats.dat");
    FileManager::saveMessagesBinary(app.getChats(),         "data/messages.dat");
    FileManager::saveUsers         (app.getUsers(),         "data/users.txt");

    std::cout << "Exiting...\n";
    return 0;
}
