#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
private:
    MyString adminCode;

public:
    Admin();
    Admin(const MyString& username, const MyString& password, const MyString& code);

    const MyString& getAdminCode() const;

    bool isAdmin() const override;
};

#endif
