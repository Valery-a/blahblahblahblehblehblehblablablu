#include "Admin.h"

const MyString Admin::MASTER_CODE = "7281";

Admin::Admin()
    : User()
    , adminCode(MASTER_CODE)
{}

Admin::Admin(const MyString& username,
             const MyString& password,
             const MyString& code)
    : User(username, password)
    , adminCode(code)
{}

bool Admin::isAdmin() const {
    return true;
}

const MyString& Admin::getAdminCode() const {
    return adminCode;
}
