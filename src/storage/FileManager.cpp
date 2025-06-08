#include "FileManager.h"
#include <fstream>
#include <iostream>

// USERS

void FileManager::saveUsers(const MyVector<User*>& users, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error opening file for saving users.\n";
        return;
    }

    for (size_t i = 0; i < users.size(); ++i) {
        out << (users[i]->isAdmin() ? "admin" : "user") << " ";
        out << users[i]->getUsername().c_str() << " ";
        out << users[i]->getPassword().c_str();
        out << "\n";
    }

    out.close();
}

void FileManager::loadUsers(MyVector<User*>& users, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No user data found.\n";
        return;
    }

    MyString type, username, password;
    while (in >> type >> username >> password) {
        if (type == "admin") {
            users.push_back(new Admin(username, password));
        } else {
            users.push_back(new User(username, password));
        }
    }


    in.close();
}

// CHATS

void FileManager::saveChats(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error opening file for saving chats.\n";
        return;
    }

    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";

        const MyVector<MyString>& parts = chats[i]->getParticipants();
        out << "participants " << parts.size() << " ";
        for (size_t j = 0; j < parts.size(); ++j) {
            out << parts[j].c_str() << " ";
        }
        out << "\n";
    }

    out.close();
}

void FileManager::loadChats(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No chat data found.\n";
        return;
    }

    MyString label, chatID;
    while (in >> label >> chatID) {
        if (label != "chat") continue;

        Chat* c = new IndividualChat("u1", "u2");
        *const_cast<MyString*>(&c->getID()) = chatID; // force-set ID
        chats.push_back(c);

        in >> label;
        if (label == "participants") {
            size_t count;
            in >> count;
            for (size_t i = 0; i < count; ++i) {
                MyString user;
                in >> user;
                c->addParticipant(user);
            }
        }
    }

    in.close();
}

void FileManager::saveMessages(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error saving messages.\n";
        return;
    }

    for (size_t i = 0; i < chats.size(); ++i) {
        out << "chat " << chats[i]->getID().c_str() << "\n";
        const MyVector<Message>& msgs = chats[i]->getMessages();
        out << msgs.size() << "\n";
        for (size_t j = 0; j < msgs.size(); ++j) {
            out << msgs[j].getSender().c_str() << "\n";
            out << msgs[j].getContent().c_str() << "\n";
            out << msgs[j].getTimestamp() << "\n";
        }
    }

    out.close();
}

void FileManager::loadMessages(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "No messages to load.\n";
        return;
    }

    MyString label, chatID;
    while (in >> label >> chatID) {
        if (label != "chat") continue;

        Chat* chat = nullptr;
        for (size_t i = 0; i < chats.size(); ++i) {
            if (chats[i]->getID() == chatID) {
                chat = chats[i];
                break;
            }
        }

        if (!chat) continue;

        size_t messageCount;
        in >> messageCount;
        in.ignore(); // skip newline

        for (size_t i = 0; i < messageCount; ++i) {
            std::string senderStr, contentStr;
            std::getline(in, senderStr);
            std::getline(in, contentStr);
            time_t timestamp;
            in >> timestamp;
            in.ignore();

            Message msg(MyString(senderStr.c_str()), MyString(contentStr.c_str()));
            msg.setTimestamp(timestamp);
            chat->addMessage(msg);
        }
    }

    in.close();
}

void FileManager::saveChatsBinary(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out) return;

    size_t chatCount = chats.size();
    out.write(reinterpret_cast<const char*>(&chatCount), sizeof(chatCount));

    for (size_t i = 0; i < chatCount; ++i) {
        MyString id = chats[i]->getID();
        size_t len = id.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(id.c_str(), len);

        const MyVector<MyString>& participants = chats[i]->getParticipants();
        size_t pCount = participants.size();
        out.write(reinterpret_cast<const char*>(&pCount), sizeof(pCount));
        for (size_t j = 0; j < pCount; ++j) {
            len = participants[j].size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(participants[j].c_str(), len);
        }
    }

    out.close();
}

void FileManager::loadChatsBinary(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in) return;

    size_t chatCount;
    in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));

    for (size_t i = 0; i < chatCount; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buf = new char[len + 1];
        in.read(buf, len);
        buf[len] = '\0';
        MyString chatID(buf);
        delete[] buf;

        Chat* chat = new IndividualChat("x", "y");
        *const_cast<MyString*>(&chat->getID()) = chatID;

        size_t pCount;
        in.read(reinterpret_cast<char*>(&pCount), sizeof(pCount));
        for (size_t j = 0; j < pCount; ++j) {
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            buf = new char[len + 1];
            in.read(buf, len);
            buf[len] = '\0';
            chat->addParticipant(MyString(buf));
            delete[] buf;
        }

        chats.push_back(chat);
    }

    in.close();
}

void FileManager::saveMessagesBinary(const MyVector<Chat*>& chats, const MyString& filename) {
    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out) return;

    size_t chatCount = chats.size();
    out.write(reinterpret_cast<const char*>(&chatCount), sizeof(chatCount));

    for (size_t i = 0; i < chatCount; ++i) {
        MyString id = chats[i]->getID();
        size_t len = id.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(id.c_str(), len);

        const MyVector<Message>& messages = chats[i]->getMessages();
        size_t mCount = messages.size();
        out.write(reinterpret_cast<const char*>(&mCount), sizeof(mCount));
        for (size_t j = 0; j < mCount; ++j) {
            MyString sender = messages[j].getSender();
            MyString text = messages[j].getContent();
            std::time_t time = messages[j].getTimestamp();

            len = sender.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(sender.c_str(), len);

            len = text.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(text.c_str(), len);

            out.write(reinterpret_cast<const char*>(&time), sizeof(time));
        }
    }

    out.close();
}

void FileManager::loadMessagesBinary(MyVector<Chat*>& chats, const MyString& filename) {
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in) return;

    size_t chatCount;
    in.read(reinterpret_cast<char*>(&chatCount), sizeof(chatCount));

    for (size_t i = 0; i < chatCount; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buf = new char[len + 1];
        in.read(buf, len);
        buf[len] = '\0';
        MyString chatID(buf);
        delete[] buf;

        Chat* chat = nullptr;
        for (size_t j = 0; j < chats.size(); ++j) {
            if (chats[j]->getID() == chatID) {
                chat = chats[j];
                break;
            }
        }

        if (!chat) continue;

        size_t mCount;
        in.read(reinterpret_cast<char*>(&mCount), sizeof(mCount));
        for (size_t j = 0; j < mCount; ++j) {
            // sender
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            buf = new char[len + 1];
            in.read(buf, len);
            buf[len] = '\0';
            MyString sender(buf);
            delete[] buf;

            // message
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            buf = new char[len + 1];
            in.read(buf, len);
            buf[len] = '\0';
            MyString text(buf);
            delete[] buf;

            std::time_t time;
            in.read(reinterpret_cast<char*>(&time), sizeof(time));

            Message msg(sender, text);
            msg.setTimestamp(time);
            chat->addMessage(msg);
        }
    }

    in.close();
}

