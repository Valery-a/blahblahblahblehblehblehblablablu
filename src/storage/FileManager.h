#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "../util/MyString.h"
#include "../util/MyVector.hpp"
#include "../users/User.h"
#include "../users/Admin.h"
#include "../chats/Chat.h"
#include "../chats/IndividualChat.h"
#include "../chats/GroupChat.h"

class FileManager {
public:
    static bool  loadUsers           (MyVector<User*>&    users,   const MyString& filename);
    static void  saveUsers           (const MyVector<User*>& users, const MyString& filename);
    static bool  loadUsersBinary     (MyVector<User*>&    users,   const MyString& filename);
    static void  saveUsersBinary     (const MyVector<User*>& users, const MyString& filename);

    static bool  loadChats           (MyVector<Chat*>&    chats,   const MyString& filename);
    static void  saveChats           (const MyVector<Chat*>& chats, const MyString& filename);
    static bool  loadMessages        (MyVector<Chat*>&    chats,   const MyString& filename);
    static void  saveMessages        (const MyVector<Chat*>& chats, const MyString& filename);

    static bool  loadChatsBinary     (MyVector<Chat*>&    chats,   const MyString& filename);
    static void  saveChatsBinary     (const MyVector<Chat*>& chats, const MyString& filename);
    static bool  loadMessagesBinary  (MyVector<Chat*>&    chats,   const MyString& filename);
    static void  saveMessagesBinary  (const MyVector<Chat*>& chats, const MyString& filename);

    static bool  loadReadState (User& user,
                                const MyVector<Chat*>& chats,
                                const MyString& filename);
    static void  saveReadState (const User& user,
                                const MyString& filename);
};

#endif
