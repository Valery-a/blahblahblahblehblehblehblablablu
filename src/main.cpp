#include <iostream>
#include <filesystem>
#include <cstring>
#include "sys/ChatSystem.h"
#include "storage/FileManager.h"

int main() {
    ChatSystem app;
    std::filesystem::create_directories("data");
    if (!FileManager::loadUsersBinary(app.getUsers(), "data/users.dat.bin")) {
        FileManager::loadUsers(app.getUsers(), "data/users.txt");
    }
        bool binFound = false;
    for (auto& entry : std::filesystem::directory_iterator("data")) {
        std::filesystem::path p         = entry.path();
        std::filesystem::path fnamePath = p.filename();
        const char*       fullPath     = p.c_str();
        const char*       fname        = fnamePath.c_str();

        if (std::strncmp(fname, "chats_", 6) == 0 && std::strstr(fname, ".dat.bin")) {
            FileManager::loadChatsBinary   (app.getChats(), fullPath);
            MyString msgFile = MyString("data/messages_") + MyString(fname + 6);
            FileManager::loadMessagesBinary(app.getChats(), msgFile.c_str());
            binFound = true;
        }
    }

    if (!binFound) {
        for (auto& entry : std::filesystem::directory_iterator("data")) {
            std::filesystem::path p         = entry.path();
            std::filesystem::path fnamePath = p.filename();
            const char*       fullPath     = p.c_str();
            const char*       fname        = fnamePath.c_str();

            if (std::strncmp(fname, "chats_", 6) == 0 && std::strstr(fname, ".txt")) {
                FileManager::loadChats   (app.getChats(), fullPath);
                MyString msgFile = MyString("data/messages_") + MyString(fname + 6);
                FileManager::loadMessages(app.getChats(), msgFile.c_str());
            }
        }
    }
    app.initializeUserChats();


    char line[1024];
    while (true) {
        std::cout << "\n> ";
        if (!std::cin.getline(line, sizeof(line))) break;
        if (line[0] == '\0') continue;

        char* cmd;
        char* args;
        char* sp = line;
        while (*sp != ' ' && *sp != '\0') ++sp;
        if (*sp == ' ') {
            *sp = '\0';
            cmd = line;
            args = sp + 1;
        } else {
            cmd = line;
            args = nullptr;
        }

        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "register") == 0 || strcmp(cmd, "create-account") == 0) {
            char* pos = args;
            char* u = ChatSystem::getArg(pos);
            char* pwd = ChatSystem::getArg(pos);
            if (!u || !pwd) {
                std::cout << "Usage: register <username> <password> [admin]\n";
                continue;
            }
            char* role = ChatSystem::getArg(pos);
            if (role && strcmp(role, "admin") == 0) {
                std::cout << "Enter admin code: ";
                char code[256];
                std::cin.getline(code, sizeof(code));
                if (code[0] == '\0') {
                    std::cout << "Error: admin code cannot be empty.\n";
                    continue;
                }
                bool ok = app.registerUser(MyString(u), MyString(pwd), true, MyString(code));
                std::cout << (ok ? "Admin registered.\n" : "Registration failed.\n");
            } else {
                bool ok = app.registerUser(MyString(u), MyString(pwd));
                std::cout << (ok ? "User registered.\n" : "Username already taken.\n");
            }
        } else if (strcmp(cmd, "login") == 0) {
            char* pos = args;
            char* u = ChatSystem::getArg(pos);
            if (!u) {
                std::cout << "Usage: login <username> [password]\n";
                continue;
            }
            char pwdBuf[256];
            char* pwd = ChatSystem::getArg(pos);
            if (!pwd) {
                std::cout << "Enter password: ";
                std::cin.getline(pwdBuf, sizeof(pwdBuf));
                pwd = pwdBuf;
            }
            if (!pwd || pwd[0] == '\0') {
                std::cout << "Error: password cannot be empty.\n";
                continue;
            }
            bool ok = app.login(MyString(u), MyString(pwd));
            if (ok) {
                std::cout << "Logged in as " << u << ".\n";
            } else {
                bool exists = false;
                for (size_t i = 0; i < app.getUsers().size(); ++i) {
                    if (app.getUsers()[i]->getUsername() == MyString(u)) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    std::cout << "User \"" << u << "\" not found. Register? (y/n): ";
                    char ans[8];
                    std::cin.getline(ans, sizeof(ans));
                    if (ans[0]=='y' || ans[0]=='Y') {
                        std::cout << "Enter password for new account: ";
                        char newpass[256];
                        std::cin.getline(newpass, sizeof(newpass));
                        if (newpass[0]=='\0') {
                            std::cout << "Error: password cannot be empty.\n";
                            continue;
                        }
                        bool ok2 = app.registerUser(MyString(u), MyString(newpass));
                        std::cout << (ok2 ? "User registered. Please login again.\n" : "Registration failed.\n");
                    }
                } else {
                    std::cout << "Invalid credentials.\n";
                }
            }
        } else if (strcmp(cmd, "logout") == 0) {
            app.logout();
            std::cout << "Logged out.\n";
        } else if (strcmp(cmd, "create-individual") == 0) {
            char* pos = args;
            char* other = ChatSystem::getArg(pos);
            if (!other) {
                std::cout << "Usage: create-individual <user>\n";
                continue;
            }
            bool ok = app.createIndividualChat(MyString(other));
            std::cout << (ok ? "Chat created.\n" : "Chat could not be created.\n");
        } else if (strcmp(cmd, "create-group") == 0) {
            char* pos = args;
            char* g = ChatSystem::getArg(pos);
            if (!g) {
                std::cout << "Usage: create-group <group_name>\n";
                continue;
            }
            bool ok = app.createGroupChat(MyString(g));
            std::cout << (ok ? "Group created.\n" : "Group could not be created.\n");
        } else if (strcmp(cmd, "request-join") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            if (!cid) {
                std::cout << "Usage: request-join <chat_id>\n";
                continue;
            }
            bool ok = app.requestJoin(MyString(cid));
            std::cout << (ok ? "Request sent or joined.\n" : "Cannot request join.\n");
        } else if (strcmp(cmd, "list-requests") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            if (!cid) {
                std::cout << "Usage: list-requests <chat_id>\n";
                continue;
            }
            if (!app.listJoinRequests(MyString(cid))) {
                std::cout << "Unauthorized or no such group.\n";
            }
        } else if (strcmp(cmd, "approve-join") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!cid || !user) {
                std::cout << "Usage: approve-join <chat_id> <username>\n";
                continue;
            }
            bool ok = app.approveJoin(MyString(cid), MyString(user));
            std::cout << (ok ? "User approved.\n" : "Cannot approve.\n");
        } else if (strcmp(cmd, "reject-join") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!cid || !user) {
                std::cout << "Usage: reject-join <chat_id> <username>\n";
                continue;
            }
            bool ok = app.rejectJoin(MyString(cid), MyString(user));
            std::cout << (ok ? "Request rejected.\n" : "Cannot reject.\n");
        } else if (strcmp(cmd, "set-membership") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            char* mode = ChatSystem::getArg(pos);
            if (!cid || !mode || (strcmp(mode,"open")!=0 && strcmp(mode,"closed")!=0)) {
                std::cout << "Usage: set-membership <chat_id> <open|closed>\n";
                continue;
            }
            bool ok = app.setGroupOpen(MyString(cid), strcmp(mode,"open")==0);
            if (ok) std::cout << "Membership now " << mode << ".\n";
            else std::cout << "Cannot change membership mode.\n";
        } else if (strcmp(cmd, "select-chat") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            if (!cid) {
                std::cout << "Usage: select-chat <chat_id>\n";
                continue;
            }
            app.selectChat(MyString(cid));
        } else if (strcmp(cmd, "send") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            if (!chatID) {
                std::cout << "Usage: send <chat_id> <message>\n";
                continue;
            }
            char* msg = pos;
            while (msg && *msg==' ') ++msg;
            if (!msg || *msg=='\0') {
                std::cout << "Error: message cannot be empty.\n";
                continue;
            }
            app.sendMessage(MyString(chatID), MyString(msg));
        } else if (strcmp(cmd, "view-messages") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            if (!chatID) {
                std::cout << "Usage: view-messages <chat_id>\n";
                continue;
            }
            app.viewMessages(MyString(chatID));
        } else if (strcmp(cmd, "view-chats") == 0) {
            if (!app.getLoggedInUser()) {
                std::cout << "Please login first.\n";
                continue;
            }
            app.viewMyChats();
        } else if (strcmp(cmd, "view-members") == 0) {
            char* pos = args;
            char* cid = ChatSystem::getArg(pos);
            if (!cid) {
                std::cout << "Usage: view-members <chat_id>\n";
                continue;
            }
            app.viewGroupMembers(MyString(cid));
        } else if (strcmp(cmd, "add-member") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!chatID || !user) {
                std::cout << "Usage: add-member <chat_id> <user>\n";
                continue;
            }
            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i) {
                if (app.getChats()[i]->getID() == MyString(chatID)) {
                    c = app.getChats()[i];
                    break;
                }
            }
            if (!c || dynamic_cast<GroupChat*>(c)==nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }
            User* uPtr = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i) {
                if (app.getUsers()[i]->getUsername()==MyString(user)) {
                    uPtr = app.getUsers()[i];
                    break;
                }
            }
            if (!uPtr) {
                std::cout << "User \"" << user << "\" does not exist.\n";
                continue;
            }
            if (c->hasParticipant(MyString(user))) {
                std::cout << "User \"" << user << "\" is already in group.\n";
                continue;
            }
            bool ok = app.addMemberToGroup(MyString(chatID), MyString(user));
            std::cout << (ok ? "Member added.\n" : "Failed. Must be group-admin or system-admin.\n");
        } else if (strcmp(cmd, "remove-member") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!chatID || !user) {
                std::cout << "Usage: remove-member <chat_id> <user>\n";
                continue;
            }
            Chat* c = nullptr;
            for (size_t i = 0; i < app.getChats().size(); ++i) {
                if (app.getChats()[i]->getID() == MyString(chatID)) {
                    c = app.getChats()[i];
                    break;
                }
            }
            if (!c || dynamic_cast<GroupChat*>(c)==nullptr) {
                std::cout << "Group not found.\n";
                continue;
            }
            User* uPtr = nullptr;
            for (size_t i = 0; i < app.getUsers().size(); ++i) {
                if (app.getUsers()[i]->getUsername()==MyString(user)) {
                    uPtr = app.getUsers()[i];
                    break;
                }
            }
            if (!uPtr) {
                std::cout << "User \"" << user << "\" does not exist.\n";
                continue;
            }
            if (!c->hasParticipant(MyString(user))) {
                std::cout << "User \"" << user << "\" is not in group.\n";
                continue;
            }
            bool ok = app.removeMemberFromGroup(MyString(chatID), MyString(user));
            std::cout << (ok ? "Member removed.\n" : "Failed. Must be group-admin or system-admin.\n");
        } else if (strcmp(cmd, "add-to-group") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!chatID || !user) {
                std::cout << "Usage: add-to-group <chat_id> <user>\n";
                continue;
            }
            bool ok = app.addToGroup(MyString(chatID), MyString(user));
            if (ok) std::cout << user << " added to group!\n";
            else std::cout << "Failed to add to group.\n";
        } else if (strcmp(cmd, "kick-from-group") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!chatID || !user) {
                std::cout << "Usage: kick-from-group <chat_id> <user>\n";
                continue;
            }
            bool ok = app.kickFromGroup(MyString(chatID), MyString(user));
            if (ok) std::cout << user << " removed from group!\n";
            else std::cout << "Failed to remove from group.\n";
        } else if (strcmp(cmd, "set-group-admin") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            char* user = ChatSystem::getArg(pos);
            if (!chatID || !user) {
                std::cout << "Usage: set-group-admin <chat_id> <user>\n";
                continue;
            }
            bool ok = app.setGroupAdmin(MyString(chatID), MyString(user));
            if (ok) std::cout << user << " is now an admin!\n";
            else std::cout << "Failed to set admin.\n";
        } else if (strcmp(cmd, "group-stats") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            if (!chatID) {
                std::cout << "Usage: group-stats <chat_id>\n";
                continue;
            }
            app.groupStats(MyString(chatID));
        } else if (strcmp(cmd, "leave-group") == 0) {
            char* pos = args;
            char* chatID = ChatSystem::getArg(pos);
            if (!chatID) {
                std::cout << "Usage: leave-group <chat_id>\n";
                continue;
            }
            bool ok = app.leaveGroup(MyString(chatID));
            std::cout << (ok ? "You left the group.\n" : "Could not leave group.\n");
        } else if (strcmp(cmd, "delete-user") == 0) {
            char* pos = args;
            char* u2 = ChatSystem::getArg(pos);
            if (!u2) {
                std::cout << "Usage: delete-user <username>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteUser(MyString(u2));
            if (ok) {
                std::cout << "User " << u2 << " deleted!\n";
            } else {
                std::cout << "Failed.\n";
            }
        } else if (strcmp(cmd, "delete-chat") == 0) {
            char* pos = args;
            char* cid2 = ChatSystem::getArg(pos);
            if (!cid2) {
                std::cout << "Usage: delete-chat <chat_id>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteChat(MyString(cid2));
            std::cout << (ok ? "Chat deleted.\n" : "Failed.\n");
        } else if (strcmp(cmd, "delete-group") == 0) {
            char* pos = args;
            char* cid3 = ChatSystem::getArg(pos);
            if (!cid3) {
                std::cout << "Usage: delete-group <chat_id>\n";
                continue;
            }
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            bool ok = app.deleteChat(MyString(cid3));
            std::cout << (ok ? "Group deleted!\n" : "Failed.\n");
        } else if (strcmp(cmd, "view-all-users") == 0) {
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            app.viewAllUsers();
        } else if (strcmp(cmd, "view-all-chats") == 0) {
            if (!app.getLoggedInUser() || !app.getLoggedInUser()->isAdmin()) {
                std::cout << "Error: require system-admin permissions.\n";
                continue;
            }
            app.viewAllChats();
        } else if (strcmp(cmd, "help") == 0) {
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
"  send <chat_id> <message>\n"
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
        } else {
            std::cout << "Unknown command. Type help.\n";
        }
    }

    for (size_t i = 0; i < app.getChats().size(); ++i) {
        Chat* c = app.getChats()[i];
        MyString id = c->getID(); 

        MyVector<Chat*> single;
        single.push_back(c);

        FileManager::saveChats(
        single,
        ( MyString("data/chats_") + id + MyString(".txt") ).c_str()
        );
        FileManager::saveMessages(
        single,
        ( MyString("data/messages_") + id + MyString(".txt") ).c_str()
        );

        FileManager::saveChatsBinary(
        single,
        ( MyString("data/chats_") + id + MyString(".dat.bin") ).c_str()
        );
        FileManager::saveMessagesBinary(
        single,
        ( MyString("data/messages_") + id + MyString(".dat.bin") ).c_str()
        );

        FileManager::saveChatsBinary(
        single,
        ( MyString("data/chats_") + id + MyString(".bin") ).c_str()
        );
    }


    FileManager::saveUsers(app.getUsers(),       "data/users.txt");
    FileManager::saveUsersBinary(app.getUsers(), "data/users.dat.bin");

    std::cout << "Exiting...\n";
    return 0;
}
