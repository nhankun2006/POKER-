#ifndef __usefulFuncs_H__
#define __usefulFuncs_H__

#define TOTAL_CARDS 52

#include <fstream>

#include "UserData.h"

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern SDL_Surface* gScreen;
extern SDL_Texture* gTexture;
extern int PLAYERS_NUM;
extern int POKER_RUSH_NO_CARDS;
const int CARDS_NUM_ONE_HAND = 2;
const int COMUNITY_CARDS = 5;
extern int** CheckedCards;
extern int CommunityCards[COMUNITY_CARDS];
extern struct _TTF_Font* gFont;

// For rendering purpose
enum screenMode
{
    FULL_SCREEN,        // Render full screen
    ORIGINAL_SCREEN,    // Render with texture's original dimensions
    MINIMAL_SCREEN,     // Render with scale 1:10
    QUARTER_SCREEN      // Render with scale 1:4
};

// 
enum ButtonState
{
    BUTTON_OUT,
    BUTTON_OVER,
    BUTTON_PRESS,
    BUTTON_RELEASE
};

bool init();

class Graphic
{
	public:
		// Declaration variables
		Graphic();

		// Deallocates memory
		~Graphic();
		
		// Deallocates texture
		void free();

        // Set Width and Height for texture 
        void setWidthHeight(int w, int h);

        // Load image
        bool LoadMedia(std::string path, int r = 1, int g = 1, int b = 1);

        // Creates image from font string file
        bool loadFromText( std::string textureText, SDL_Color textColor );

        // Access to mTexture
        void setTexture(SDL_Texture* pTexture = gTexture);

		// Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

        // Get Texture
        SDL_Texture* getTexture();
		
		// Renders texture at given point
		void render( int x, int y, screenMode mode = ORIGINAL_SCREEN, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		// Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

// this point will require deck database of the game
#include "DeckData.h"

// Button wrapper
class button : public Graphic
{
    private:
        // Button position is stored here using BUTTON_SetPosition(int x, int y) funtion
        SDL_Point bPosition;

        // Store 3 kinds of button states (out, over, press) as texture
        SDL_Texture* bTexture[3];

        // This var store current state of button after BUTTON_HandleEvent() function called
        ButtonState bState;
    public:
        // construct
        button();

        // set current button position as it called
        void BUTTON_SetPosition(int x, int y);

        // Handle event for button
        void BUTTON_HandleEvent(SDL_Event* bEvent);

        // Load button image from file
        bool BUTTON_Load(std::string path_bOver, std::string path_bIn, std::string path_bPress);

        // Render button at any position (x, y)
        void BUTTON_Render(int x, int y, screenMode mode = MINIMAL_SCREEN);

        // Render button with any specific states
        void BOX_Render(int x, int y, ButtonState _specific_State);

        // Return dimensions of button
        int BUTTON_GetWidth();
        int BUTTON_GetHeight();

        // Return button state
        ButtonState BUTTON_GetState();

        // Reset button state
        void BUTTON_ResetState();
};

/// @brief Load image/texture for each card as well as rank value and suit value
/// @param CARDS 
/// @return true if initial process is success, otherwise false
bool InitDeck(card* CARDS);

/// @brief shuffle deck with an global double pointer variable to hold dealed card position
void ShuffleDeck();

/// @brief 
/// @returns 1 - entering the login scene @line 
/// @returns 2 - entering the sing up scene
/// @returns 3 - quit the game
int WelcomeScene();


bool LoginScene();


bool SignupScene();


int lobbyScene();

// Texas Hold'em mode
bool gameplay(card CARDS[]);

// 
bool PokerRush(card CARDS[]);

// close everything
void close();

#endif