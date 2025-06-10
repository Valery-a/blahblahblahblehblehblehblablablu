#ifndef MESSAGE_H
#define MESSAGE_H

#include "../util/MyString.h"
#include <ctime>

class Message {
private:
    MyString sender;
    MyString content;
    std::time_t timestamp;

public:
    Message();
    Message(const MyString& sender, const MyString& content);
    
    MyString getSender() const;
    MyString getContent() const;
    std::time_t getTimestamp() const;

    void setTimestamp(std::time_t time);
    void print() const;
};

#endif
