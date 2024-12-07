#ifndef __UserData_H__
#define __UserData_H__
#include <fstream>

class user
{
    private:
        int amountOfAccount;
        int coin;
        std::fstream RW_DataUserName;
        std::fstream RW_DataPassword;
    public:
        user();
        ~user(); 
        bool USER_CheckAccount(std::string user, std::string pass);
        bool USER_CreateAccount(std::string user, std::string pass);
        
};


#endif