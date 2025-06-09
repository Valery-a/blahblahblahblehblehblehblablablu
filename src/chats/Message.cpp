#include "Message.h"
#include <iostream>
#include <iomanip>

Message::Message() : sender(""), content(""), timestamp(std::time(nullptr)) {}

Message::Message(const MyString& sender, const MyString& content)
    : sender(sender), content(content), timestamp(std::time(nullptr)) {}

MyString Message::getSender() const {
    return sender;
}

MyString Message::getContent() const {
    return content;
}

void Message::setTimestamp(std::time_t time) {
    timestamp = time;
}

std::time_t Message::getTimestamp() const {
    return timestamp;
}

void Message::print() const {
    std::tm* timeInfo = std::localtime(&timestamp);
    std::cout << "[" << std::put_time(timeInfo, "%d/%m/%Y %H:%M:%S") << "] "
            << sender << ": " << content << std::endl;
}
