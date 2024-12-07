#include "include/SDL.h"
#include "include/SDL_image.h"
#include "usefulFuncs.h"
#include "include/SDL_ttf.h"
#include <string>
#include <fstream>

/******** These variables for graphic */
// Global variables for working across-files
SDL_Window* gWindow = nullptr;         
SDL_Renderer* gRenderer = nullptr;
SDL_Surface* gScreen = nullptr;
SDL_Texture* gTexture = nullptr;
struct _TTF_Font* gFont = nullptr;


int PLAYERS_NUM;
int POKER_RUSH_NO_CARDS;
int** CheckedCards;
int CommunityCards[COMUNITY_CARDS];

int main(int argc, char *argv[])
{
    card CARDS[TOTAL_CARDS];

    // call initial functions
    bool first_flag = init();
    if (!first_flag)
    {
        return 1;
    }
    else
    {
        bool second_flag = InitDeck(CARDS);
        if ( !second_flag )
        {
            return 1;
        }
        else
        {
            gFont = TTF_OpenFont("Font/lazy.ttf", 28);
            if (gFont == nullptr)
            {
                return 1;
            }   
        }
    }

    // These are welcome scenes
    // 3 major functions: welcome, log in, sign up
    // Welcome scene handles 
    // Login scene handles data of user and loads them before entering the main game
    // Sign up scene handles data of user (checks and creates new one) before entering the main game
    bool login_flag = true;
    bool signup_flag = true;
    bool quit_flag = false;
    do
    {
        switch (WelcomeScene())
        {
            case 1:
                login_flag = LoginScene();
                break;
            case 2:
                signup_flag =  SignupScene();
                break;
            case 3:
                login_flag = false;
                signup_flag = false;
                quit_flag = true;
                break;
        }       
    } while (login_flag && signup_flag);
    // End the welcome scene process
    
    // Enter the main game
    if (!quit_flag)
    {
        bool Texas_Holdem_flag = true;
        bool pokerRush = true;
        do
        {
            switch (lobbyScene())       // Lobby scene handle the fisrt scene at main game stage as soon as entering the main game
                                        // this scene includes winrate, money, username of that user
            {
            case 0:
                Texas_Holdem_flag = false;
                pokerRush = false;
                break;
            case 1:
                Texas_Holdem_flag = gameplay(CARDS);
                break;
            case 2:
                pokerRush = PokerRush(CARDS);
                break;
            default:
                break;
            }
        } while (Texas_Holdem_flag && pokerRush);
        
    }
    
    // Close everything
    close();
    return 0;
}
