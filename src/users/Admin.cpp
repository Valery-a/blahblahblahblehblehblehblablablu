#include "Admin.h"

Admin::Admin() : User(), adminCode("0000") {}

Admin::Admin(const MyString& username, const MyString& password, const MyString& code)
    : User(username, password), adminCode(code) {}

bool Admin::isAdmin() const {
    return true;
}

const MyString& Admin::getAdminCode() const {
    return adminCode;
}
