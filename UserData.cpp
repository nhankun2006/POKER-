#include "UserData.h"
#include <string>
#include <cstring>

user::user()
{
    std::ifstream ReadAmount("UserData/number.txt");
    ReadAmount >> amountOfAccount;
    ReadAmount.close();
    RW_DataUserName.open("UserData/account.txt");
    RW_DataPassword.open("UserData/pass.txt");
}

user::~user()
{
    RW_DataUserName.close();
    RW_DataPassword.close();
}


bool user::USER_CheckAccount(std::string user, std::string pass)
{   
    std::fstream getErr("Errors.txt");      // Keep track on bugs

    // set the cursor READ and WRITE to the beginning of two files
    RW_DataUserName.seekg(0, std::fstream::beg);
    RW_DataPassword.seekg(0, std::fstream::beg);
    RW_DataUserName.seekp(0, std::fstream::beg);
    RW_DataPassword.seekp(0, std::fstream::beg);

    // check if two string parameters are empty
    if (user.empty() || pass.empty())
    {
        getErr << "username/password is empty" << '\n';
        getErr.close();
        return false;
    }
    else if (!RW_DataUserName.is_open() || !RW_DataPassword.is_open())      // Check if these files are opened
    {
        getErr << "Error: can not open account.txt and password.txt" << '\n';
        getErr.close();
        return false;
    }
    
    // checking pre-existed account process
    std::string tempUserName, tempPassword;
    for (int i = 0; i < amountOfAccount; i++)
    {
        RW_DataUserName >> tempUserName;
        RW_DataPassword >> tempPassword;
        if ( (tempUserName == user) && (tempPassword == pass) )
        { 
            getErr << "Check account correctly " << '\n';
            std::ofstream setAccPos("UserData/Pos.txt");
            setAccPos << (i + 1);
            setAccPos.close();

            getErr.close();
            return true;
        }
    }

    // This account has been existed
    // Return an error;
    getErr.close();
    return false;
}

// Create new account
bool user::USER_CreateAccount(std::string user, std::string pass)
{
    std::fstream getErr("Errors.txt");      // Keep track on bugs

    // set the cursor to the end of two files
    RW_DataUserName.seekp(0, std::fstream::end);
    RW_DataPassword.seekp(0, std::fstream::end);

    // Check if these files are opened
    if (!RW_DataUserName.is_open() || !RW_DataPassword.is_open())
    {
        getErr << "Error: can not open account.txt and password.txt" << '\n';
        getErr.close();
        return false;
    }

    RW_DataUserName << user << '\n';      // Add new username into account.txt file
    RW_DataPassword << pass << '\n';      // Add new password into pass.txt file

    // Add the amount of account by 1, then change its value by deleting entire data of the file
    amountOfAccount++;
    std::fstream WriteAmount("UserData/number.txt");
    WriteAmount << amountOfAccount;

    // Set account possition for new stages
    std::ofstream setAccPos("UserData/Pos.txt");
    setAccPos << amountOfAccount;

    // Create statistic for new user
    /*************************************************************************** */
    // winrate information
    std::ofstream CreateNewInfo("UserData/winrate.txt", std::ofstream::app);

    // Check if this file is opened
    if ( !CreateNewInfo.is_open())
    {
        getErr << "Error: can not open winrate.txt" << '\n';
        getErr.close();
        return false;
    }
    
    CreateNewInfo.seekp(0, std::ofstream::end);
    CreateNewInfo << 0 << " " << 0 << '\n';
    CreateNewInfo.close();

    // money information
    CreateNewInfo.open("UserData/coin.txt", std::ofstream::app);

    // Check if this file is opened
    if ( !CreateNewInfo.is_open())
    {
        getErr << "Error: can not open coin.txt" << '\n';
        getErr.close();
        return false;
    }
    
    CreateNewInfo.seekp(0, std::ofstream::end);
    CreateNewInfo << 3000 << '\n';
    /************************************************************************ */

    // Close files
    setAccPos.close();
    WriteAmount.close();
    CreateNewInfo.close();

    // Return success
    getErr.close();
    return true;
}