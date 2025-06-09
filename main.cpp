#include <iostream>
#include <sstream>
#include "src/sys/ChatSystem.h"
#include "src/storage/FileManager.h"
#include "src/chats/GroupChat.h"

int main() {
    ChatSystem app;

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
            iss >> u >> p >> role;
            if (role == "admin") {
                std::string code;
                std::cout << "Enter admin code: ";
                std::getline(std::cin, code);
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
            iss >> u >> p;
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
                        bool ok2 = app.registerUser(MyString(u.c_str()), MyString(p.c_str()));
                        std::cout << (ok2
                            ? "User registered. Please login again.\n"
                            : "Registration failed.\n");
                    }
                } else {
                    std::cout << "Invalid credentials.\n";
                }
            }
        }

        else if (cmd == "logout") {
            app.logout();
            std::cout << "Logged out.\n";
        }

        else if (cmd == "create-individual") {
            std::string other;
            iss >> other;
            bool ok = app.createIndividualChat(MyString(other.c_str()));
            std::cout << (ok ? "Chat created.\n" : "Chat could not be created.\n");
        }

        else if (cmd == "create-group") {
            std::string g;
            iss >> g;
            bool ok = app.createGroupChat(MyString(g.c_str()));
            std::cout << (ok ? "Group created.\n" : "Group could not be created.\n");
        }

        else if (cmd == "send") {
            std::string chatID;
            iss >> chatID;
            std::string msg;
            std::getline(iss, msg);
            if (!msg.empty() && msg.front() == ' ')
                msg.erase(0,1);
            app.sendMessage(MyString(chatID.c_str()), MyString(msg.c_str()));
        }

        else if (cmd == "view-messages") {
            std::string chatID;
            iss >> chatID;
            app.viewMessages(MyString(chatID.c_str()));
        }

        else if (cmd == "view-chats") {
            app.viewMyChats();
        }

        else if (cmd == "view-members") {
            std::string cid; iss >> cid;
            app.viewGroupMembers(MyString(cid.c_str()));
        }

        else if (cmd == "add-member") {
            std::string chatID, user;
            iss >> chatID >> user;

            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i) {
                if (app.getChats()[i]->getID() == MyString(chatID.c_str())) {
                    c = app.getChats()[i];
                    break;
                }
            }
            if (!c || dynamic_cast<GroupChat*>(c) == nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }

            User* u = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i) {
                if (app.getUsers()[i]->getUsername() == MyString(user.c_str())) {
                    u = app.getUsers()[i];
                    break;
                }
            }
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

        else if (cmd == "remove-member") {
            std::string chatID, user;
            iss >> chatID >> user;

            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i) {
                if (app.getChats()[i]->getID() == MyString(chatID.c_str())) {
                    c = app.getChats()[i];
                    break;
                }
            }
            if (!c || dynamic_cast<GroupChat*>(c) == nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }

            User* u = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i) {
                if (app.getUsers()[i]->getUsername() == MyString(user.c_str())) {
                    u = app.getUsers()[i];
                    break;
                }
            }
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

        else if (cmd == "delete-user") {
            std::string u; iss >> u;
            bool ok = app.deleteUser(MyString(u.c_str()));
            std::cout << (ok ? "User deleted.\n" : "Failed. Require system-admin.\n");
        }

        else if (cmd == "delete-chat") {
            std::string c; iss >> c;
            bool ok = app.deleteChat(MyString(c.c_str()));
            std::cout << (ok ? "Chat deleted.\n" : "Failed. Require system-admin.\n");
        }

        else if (cmd == "view-all-users") {
            app.viewAllUsers();
        }

        else if (cmd == "view-all-chats") {
            app.viewAllChats();
        }

        else if (cmd == "help") {
            std::cout <<
"Available commands:\n"
"  register|create-account <user> <pass> [admin]\n"
"  login <user> <pass>\n"
"  logout\n"
"  create-individual <user>\n"
"  create-group <group_name>\n"
"  send <chat_id> <message…>\n"
"  view-messages <chat_id>\n"
"  view-chats\n"
"  view-members <chat_id>\n"
"  add-member <chat_id> <user>\n"
"  remove-member <chat_id> <user>\n"
"  delete-user <user>             (system-admin only)\n"
"  delete-chat <chat_id>          (system-admin only)\n"
"  view-all-users                 (system-admin only)\n"
"  view-all-chats                 (system-admin only)\n"
"  help\n"
"  exit\n";
        }

        else {
            std::cout << "Unknown command. Type help.\n";
        }
    }

    FileManager::saveChats         (app.getChats(),         "data/chats.txt");
    FileManager::saveMessages      (app.getChats(),         "data/messages.txt");
    FileManager::saveChatsBinary   (app.getChats(),         "data/chats.dat");
    FileManager::saveMessagesBinary(app.getChats(),         "data/messages.dat");
    FileManager::saveUsers         (app.getUsers(),         "data/users.txt");

    std::cout << "Exiting...\n";
    return 0;
}
