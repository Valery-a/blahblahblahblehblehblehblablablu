#include "FileManager.h"
#include "MyString.h"
#include "MyVector.hpp"
#include <fstream>
#include <iostream>
#include <ctime>

bool FileManager::loadUsers(MyVector<User*>& users, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No user data found (" << filename.c_str() << ")\n";
        return false;
    }
    MyString type, uname, pwd, code;
    while (in >> type >> uname >> pwd) {
        if (type == MyString("admin")) {
            in >> code;
            users.push_back(new Admin(uname, pwd, code));
        } else {
            users.push_back(new User(uname, pwd));
        }
    }
    return true;
}

void FileManager::saveUsers(const MyVector<User*>& users, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error saving users.\n";
        return;
    }
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->isAdmin()) {
            auto a = dynamic_cast<Admin*>(users[i]);
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
    MyString label, idBuf;
    while (in >> label >> idBuf) {
        if (label != MyString("chat")) continue;

        MyString pLabel;
        size_t count = 0;
        in >> pLabel >> count;
        if (pLabel != MyString("participants")) {
            std::cerr << "Malformed chat file.\n";
            return false;
        }

        MyVector<MyString> parts;
        for (size_t i = 0; i < count; ++i) {
            MyString tmp;
            in >> tmp;
            parts.push_back(tmp);
        }

        size_t len = idBuf.size();
        bool isGroup = false;
        if (len > 6) {
            const char* tail = idBuf.c_str() + (len - 6);
            if (MyString(tail) == MyString("_group")) {
                isGroup = true;
            }
        }

        Chat* c = nullptr;
        if (isGroup) {
            size_t gnLen = len - 6;
            char* buffer = new char[gnLen + 1];
            const char* src = idBuf.c_str();
            for (size_t i = 0; i < gnLen; ++i) {
                buffer[i] = src[i];
            }
            buffer[gnLen] = '\0';
            MyString gname(buffer);
            delete[] buffer;

            c = new GroupChat(idBuf, gname, parts[0]);
            for (size_t i = 1; i < parts.size(); ++i)
                c->addParticipant(parts[i]);
        }
        else if (parts.size() == 2) {
            c = new IndividualChat(parts[0], parts[1]);
        }

        if (c) chats.push_back(c);
    }
    return true;
}

void FileManager::saveChats(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error saving chats.\n";
        return;
    }
    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";
        const auto& parts = chats[i]->getParticipants();
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
    MyString label, idBuf;
    while (in >> label >> idBuf) {
        if (label != MyString("chat")) continue;
        MyString chatID = idBuf;

        size_t msgCount;
        in >> msgCount;
        in.ignore();

        Chat* chat = nullptr;
        for (size_t i = 0; i < chats.size(); ++i) {
            if (chats[i]->getID() == chatID) {
                chat = chats[i];
                break;
            }
        }

        for (size_t i = 0; i < msgCount; ++i) {
            MyString sender, content;
            {
                char buffer[4096];
                in.getline(buffer, 4096);
                sender = MyString(buffer);
            }
            {
                char buffer[4096];
                in.getline(buffer, 4096);
                content = MyString(buffer);
            }

            time_t ts;
            in >> ts;
            in.ignore();

            if (chat) {
                Message msg(sender, content);
                msg.setTimestamp(ts);
                chat->addMessage(msg);
            }
        }
    }
    return true;
}

void FileManager::saveMessages(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error saving messages.\n";
        return;
    }
    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";
        const auto& msgs = chats[i]->getMessages();
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

    size_t chatCount;
    in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buf = new char[len+1];
        in.read(buf, len);
        buf[len] = '\0';
        MyString idBuf(buf);
        delete[] buf;

        size_t pCount;
        in.read(reinterpret_cast<char*>(&pCount), sizeof(pCount));
        MyVector<MyString> parts;
        for (size_t j = 0; j < pCount; ++j) {
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            char* buf2 = new char[len+1];
            in.read(buf2, len);
            buf2[len] = '\0';
            parts.push_back(MyString(buf2));
            delete[] buf2;
        }

        bool isGroup = false;
        if (idBuf.size() > 6) {
            const char* tail = idBuf.c_str() + (idBuf.size() - 6);
            if (MyString(tail) == MyString("_group"))
                isGroup = true;
        }

        Chat* c = nullptr;
        if (isGroup) {
            size_t gnLen = idBuf.size() - 6;
            char* gbuf = new char[gnLen+1];
            const char* src2 = idBuf.c_str();
            for (size_t k = 0; k < gnLen; ++k) {
                gbuf[k] = src2[k];
            }
            gbuf[gnLen] = '\0';
            MyString gname(gbuf);
            delete[] gbuf;

            c = new GroupChat(idBuf, gname, parts[0]);
            for (size_t k = 1; k < parts.size(); ++k)
                c->addParticipant(parts[k]);
        }
        else if (parts.size() == 2) {
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

        const auto& parts = chats[i]->getParticipants();
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

    size_t chatCount;
    in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));
    for (size_t i = 0; i < chatCount; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buf = new char[len+1];
        in.read(buf, len);
        buf[len] = '\0';
        MyString chatID(buf);
        delete[] buf;

        size_t mCount;
        in.read(reinterpret_cast<char*>(&mCount), sizeof(mCount));

        Chat* chat = nullptr;
        for (size_t j = 0; j < chats.size(); ++j) {
            if (chats[j]->getID() == chatID) {
                chat = chats[j];
                break;
            }
        }

        for (size_t j = 0; j < mCount; ++j) {
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            char* sbuf = new char[len+1];
            in.read(sbuf, len);
            sbuf[len] = '\0';
            MyString sender(sbuf);
            delete[] sbuf;

            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            char* cbuf = new char[len+1];
            in.read(cbuf, len);
            cbuf[len] = '\0';
            MyString content(cbuf);
            delete[] cbuf;

            time_t ts;
            in.read(reinterpret_cast<char*>(&ts), sizeof(ts));

            if (chat) {
                Message msg(sender, content);
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

        const auto& msgs = chats[i]->getMessages();
        size_t mCount = msgs.size();
        out.write(reinterpret_cast<const char*>(&mCount), sizeof(mCount));
        for (size_t j = 0; j < mCount; ++j) {
            const MyString& sender = msgs[j].getSender();
            const MyString& text   = msgs[j].getContent();
            time_t tstamp          = msgs[j].getTimestamp();

            size_t l2 = sender.size();
            out.write(reinterpret_cast<const char*>(&l2), sizeof(l2));
            out.write(sender.c_str(), l2);

            l2 = text.size();
            out.write(reinterpret_cast<const char*>(&l2), sizeof(l2));
            out.write(text.c_str(), l2);

            out.write(reinterpret_cast<const char*>(&tstamp), sizeof(tstamp));
        }
    }
}

bool FileManager::loadUsersBinary(MyVector<User*>& users, const MyString& filename) {
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in) return false;
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (size_t i = 0; i < count; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* name = new char[len+1];
        in.read(name, len);
        name[len] = '\0';
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* pass = new char[len+1];
        in.read(pass, len);
        pass[len] = '\0';
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* code = new char[len+1];
        in.read(code, len);
        code[len] = '\0';
        if (len)
            users.push_back(new Admin(name, pass, code));
        else
            users.push_back(new User(name, pass));
        delete[] name; delete[] pass; delete[] code;
    }
    return true;
}

void FileManager::saveUsersBinary(const MyVector<User*>& users, const MyString& filename) {
    std::ofstream out(filename.c_str(), std::ios::binary);
    size_t count = users.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (size_t i = 0; i < count; ++i) {
        const User* u = users[i];
        std::string name = u->getUsername().c_str();
        std::string pass = u->getPassword().c_str();
        std::string code = u->isAdmin() ? dynamic_cast<const Admin*>(u)->getAdminCode().c_str() : "";
        size_t len;
        len = name.size(); out.write(reinterpret_cast<const char*>(&len), sizeof(len)); out.write(name.data(), len);
        len = pass.size(); out.write(reinterpret_cast<const char*>(&len), sizeof(len)); out.write(pass.data(), len);
        len = code.size(); out.write(reinterpret_cast<const char*>(&len), sizeof(len)); out.write(code.data(), len);
    }
}