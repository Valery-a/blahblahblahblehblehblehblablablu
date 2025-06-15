#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <ctime>

bool FileManager::loadUsers(MyVector<User*>& users, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No user data found (" << filename.c_str() << ")\n";
        return false;
    }
    std::string type, uname, pwd, code;
    while (in >> type >> uname >> pwd) {
        if (type == "admin") {
            in >> code;
            users.push_back(new Admin(
                MyString(uname.c_str()),
                MyString(pwd.c_str()),
                MyString(code.c_str())
            ));
        } else {
            users.push_back(new User(
                MyString(uname.c_str()),
                MyString(pwd.c_str())
            ));
        }
    }
    return true;
}

void FileManager::saveUsers(const MyVector<User*>& users, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) { std::cerr << "Error saving users.\n"; return; }
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->isAdmin()) {
            Admin* a = dynamic_cast<Admin*>(users[i]);
            out << "admin "
                << a->getUsername().c_str() << " "
                << a->getPassword().c_str() << " "
                << a->getAdminCode().c_str() << "\n";
        } else {
            out << "user "
                << users[i]->getUsername().c_str() << " "
                << users[i]->getPassword().c_str() << "\n";
        }
    }
}

bool FileManager::loadChats(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No chat data found (" << filename.c_str() << ")\n";
        return false;
    }
    std::string label, idBuf;
    while (in >> label >> idBuf) {
        if (label != "chat") continue;
        size_t count;
        in >> label >> count;
        if (label != "participants") {
            std::cerr << "Malformed chat file.\n";
            return false;
        }
        MyVector<MyString> parts;
        for (size_t i = 0; i < count; ++i) {
            std::string tmp;
            in >> tmp;
            parts.push_back(MyString(tmp.c_str()));
        }

        bool isGroup = idBuf.size() > 6 && idBuf.substr(idBuf.size()-6) == "_group";
        Chat* c = nullptr;
        if (isGroup) {
            std::string gname = idBuf.substr(0, idBuf.size()-6);
            c = new GroupChat(
                MyString(idBuf.c_str()),
                MyString(gname.c_str()),
                parts[0]
            );
            for (size_t i = 1; i < parts.size(); ++i)
                c->addParticipant(parts[i]);
        } else if (parts.size() == 2) {
            c = new IndividualChat(parts[0], parts[1]);
        }
        if (c) chats.push_back(c);
    }
    return true;
}

void FileManager::saveChats(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) { std::cerr << "Error saving chats.\n"; return; }
    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";
        const MyVector<MyString>& parts = chats[i]->getParticipants();
        out << "participants " << parts.size();
        for (size_t j = 0; j < parts.size(); ++j)
            out << " " << parts[j].c_str();
        out << "\n";
    }
}

bool FileManager::loadMessages(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No messages to load (" << filename.c_str() << ")\n";
        return false;
    }
    std::string label, idBuf;
    while (in >> label >> idBuf) {
        if (label != "chat") continue;
        MyString chatID(idBuf.c_str());
        size_t msgCount;
        in >> msgCount;
        in.ignore();
        Chat* chat = nullptr;
        for (size_t i = 0; i < chats.size(); ++i)
            if (chats[i]->getID() == chatID) { chat = chats[i]; break; }
        for (size_t i = 0; i < msgCount; ++i) {
            std::string sender, content;
            std::getline(in, sender);
            std::getline(in, content);
            time_t ts; in >> ts; in.ignore();
            if (chat) {
                Message msg(MyString(sender.c_str()), MyString(content.c_str()));
                msg.setTimestamp(ts);
                chat->addMessage(msg);
            }
        }
    }
    return true;
}

void FileManager::saveMessages(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) { std::cerr << "Error saving messages.\n"; return; }
    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";
        const MyVector<Message>& msgs = chats[i]->getMessages();
        out << msgs.size() << "\n";
        for (size_t j = 0; j < msgs.size(); ++j) {
            out << msgs[j].getSender().c_str()  << "\n";
            out << msgs[j].getContent().c_str() << "\n";
            out << msgs[j].getTimestamp()       << "\n";
        }
    }
}

bool FileManager::loadChatsBinary(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in) return false;
    size_t chatCount; in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        size_t len; in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string idBuf(len, '\0'); in.read(&idBuf[0], len);
        size_t pCount; in.read(reinterpret_cast<char*>(&pCount), sizeof(pCount));
        MyVector<MyString> parts;
        for (size_t j = 0; j < pCount; ++j) {
            size_t l2; in.read(reinterpret_cast<char*>(&l2), sizeof(l2));
            std::string tmp(l2, '\0');
            in.read(&tmp[0], l2);
            parts.push_back(MyString(tmp.c_str()));
        }

        bool isGroup = idBuf.size() > 6 && idBuf.substr(idBuf.size()-6) == "_group";
        Chat* c = nullptr;
        if (isGroup) {
            std::string gname = idBuf.substr(0, idBuf.size()-6);
            c = new GroupChat(
                MyString(idBuf.c_str()),
                MyString(gname.c_str()),
                parts[0]
            );
            for (size_t k = 1; k < parts.size(); ++k)
                c->addParticipant(parts[k]);
        } else if (parts.size() == 2) {
            c = new IndividualChat(parts[0], parts[1]);
        }
        if (c) chats.push_back(c);
    }
    return true;
}

void FileManager::saveChatsBinary(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out) return;
    size_t chatCount = chats.size();
    out.write(reinterpret_cast<const char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        const MyString& id = chats[i]->getID();
        size_t len = id.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(id.c_str(), len);

        const MyVector<MyString>& parts = chats[i]->getParticipants();
        size_t pCount = parts.size();
        out.write(reinterpret_cast<const char*>(&pCount), sizeof(pCount));
        for (size_t j = 0; j < pCount; ++j) {
            const MyString& p = parts[j];
            size_t l2 = p.size();
            out.write(reinterpret_cast<const char*>(&l2), sizeof(l2));
            out.write(p.c_str(), l2);
        }
    }
}

bool FileManager::loadMessagesBinary(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in) return false;
    size_t chatCount; in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        size_t len; in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string idBuf(len, '\0'); in.read(&idBuf[0], len);
        MyString chatID(idBuf.c_str());
        size_t mCount; in.read(reinterpret_cast<char*>(&mCount), sizeof(mCount));
        Chat* chat = nullptr;
        for (size_t j = 0; j < chats.size(); ++j)
            if (chats[j]->getID() == chatID) { chat = chats[j]; break; }
        for (size_t j = 0; j < mCount; ++j) {
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string s(len, '\0'); in.read(&s[0], len);
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string c2(len, '\0'); in.read(&c2[0], len);
            time_t ts; in.read(reinterpret_cast<char*>(&ts), sizeof(ts));
            if (chat) {
                Message msg(MyString(s.c_str()), MyString(c2.c_str()));
                msg.setTimestamp(ts);
                chat->addMessage(msg);
            }
        }
    }
    return true;
}

void FileManager::saveMessagesBinary(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out) return;
    size_t chatCount = chats.size();
    out.write(reinterpret_cast<const char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        const MyString& id = chats[i]->getID();
        size_t len = id.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(id.c_str(), len);

        const MyVector<Message>& msgs = chats[i]->getMessages();
        size_t mCount = msgs.size();
        out.write(reinterpret_cast<const char*>(&mCount), sizeof(mCount));
        for (size_t j = 0; j < mCount; ++j) {
            const MyString& sender = msgs[j].getSender();
            const MyString& text   = msgs[j].getContent();
            time_t          tstamp = msgs[j].getTimestamp();
            size_t l2;

            l2 = sender.size();
            out.write(reinterpret_cast<const char*>(&l2), sizeof(l2));
            out.write(sender.c_str(), l2);

            l2 = text.size();
            out.write(reinterpret_cast<const char*>(&l2), sizeof(l2));
            out.write(text.c_str(), l2);

            out.write(reinterpret_cast<const char*>(&tstamp), sizeof(tstamp));
        }
    }
}
