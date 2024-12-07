#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_ttf.h"
#include "include/SDL_mixer.h"
#include "usefulFuncs.h"

/// @brief Initialize SDL, video, audio, image jpg, Window, Renderer
/// @param gWindow SDL_Window*
/// @param gRenderer SDL_Renderer*
/// @return true (1) if everything are loaded, otherwise false (0) 
bool init()
{
    std::fstream getErr;        // Keep trank on bug
    getErr.open("Errors.txt");

    // Initial video of SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        getErr << "Init " << SDL_GetError() << '\n';
        return false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow("Poker Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (gWindow == nullptr)
        {
            getErr << "Create Window " << SDL_GetError() << '\n';
            return false;
        }
        else
        {
            // Create renderer
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == nullptr)
            {
                getErr << "Renderer " << SDL_GetError();
                return false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                int JPG_flag = IMG_INIT_JPG;    // Initial sub video library for JPG
                int PNG_flag = IMG_INIT_PNG;    // Initial sub video library for PNG
				if( !( IMG_Init( JPG_flag ) & JPG_flag ) || !( IMG_Init( PNG_flag ) & PNG_flag ))
				{
					getErr << "Image Error " << IMG_GetError() << '\n';
					return false;
				}
                else
                {
                    // Initial font for text 
                    if (TTF_Init() == -1)
                    {
                        getErr << "Load font error" << TTF_GetError() << '\n';
                        return false;
                    }
                    else
                    {
                        getErr.close();
                        return true;
                        
                    }
                    
                }
                
            }
            
        }
            
    }
    
}

/// @brief close everything
void close()
{
    SDL_DestroyTexture(gTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_FreeSurface(gScreen);
    SDL_DestroyWindow(gWindow);
    TTF_CloseFont( gFont );


    gWindow = nullptr;
    gTexture = nullptr;
    gRenderer = nullptr;
    gScreen = nullptr;
	gFont = NULL;

    IMG_Quit();
    TTF_Quit();
    // Mix_Quit();
    SDL_Quit();
}

// construction of Graphic class
Graphic::Graphic()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

void Graphic::setWidthHeight(int w, int h)
{
    mWidth = w;
    mHeight = h;
}

bool Graphic::LoadMedia( std::string path, int r, int g, int b)
{
    // Keep bugs track
    std::fstream getErr("Errors.txt");

	//The final temporary texture
	SDL_Texture* newTexture = NULL;

	// Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		getErr << "Error: image surface in LoadMedia function " << SDL_GetError() << " " << IMG_GetError();
        getErr.close();
        return false;
	}
	else
	{
		// Transparent specific color on image surface
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, r, g, b ) );

		// Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			getErr << "Error: image texture in LoadMedia function " << SDL_GetError() << " " << IMG_GetError();
            getErr.close();
            return false;
		}
		else
		{
			// Get image dimensions
			setWidthHeight(loadedSurface->w, loadedSurface->h);

	    	// Get rid of no longer use of loaded surface
		    SDL_FreeSurface( loadedSurface );
            loadedSurface = nullptr;

            // Set specific texture for Graphic object 
            setTexture(newTexture);
            getErr.close();
            return true;
		}
	}
}


void Graphic::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

bool Graphic::loadFromText( std::string textureText, SDL_Color textColor )
{

    // Keep bugs track
    std::fstream getErr("Errors.txt");

	// Get surface of desired rendering text
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		getErr << "Load text surface phase " << TTF_GetError() << '\n';
        getErr.close();
        return false;
	}
	else
	{
		// Create texture from surface by renderer
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			getErr << "Load text render phase " << TTF_GetError() << '\n';
            getErr.close();
            return false;
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	    return true;
	}
}

Graphic::~Graphic()
{
	//Deallocate
	free();
}

void Graphic::render( int x, int y, screenMode mode, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	// Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
    
	// Render to screen
    if (mode == ORIGINAL_SCREEN)
    {
	    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
    }
    else if (mode == FULL_SCREEN)
    {
        SDL_RenderCopyEx( gRenderer, mTexture, clip, nullptr, angle, center, flip );
    }
    else if (mode == MINIMAL_SCREEN)
    {
        renderQuad.w = renderQuad.w / 10;
        renderQuad.h = renderQuad.h / 10;
        SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
    }
    else if (mode == QUARTER_SCREEN)
    {
        renderQuad.w = renderQuad.w / 4;
        renderQuad.h = renderQuad.h / 4;
        SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
    }

}

// Handle Button event
void Graphic::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	// Modulate texture RGB
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

int Graphic::getWidth()
{
	return mWidth;
}

int Graphic::getHeight()
{
	return mHeight;
}

void Graphic::setTexture(SDL_Texture* pTexture)
{
    mTexture = pTexture;
}

// return current texture of object
SDL_Texture* Graphic::getTexture()
{
    std::fstream getErr("Errors.txt");
    if (mTexture == nullptr)
    {
        getErr << "get texture from class Graphic";
        getErr.close();
    }
    
    return mTexture;
}

/// @brief construction of button object
button::button()
{
    bPosition.x = 0;
    bPosition.y = 0;
    setWidthHeight(0, 0);
    bState = BUTTON_OUT;
}

void button::BUTTON_SetPosition(int x, int y)
{
    bPosition.x = x;
    bPosition.y = y;
}

void button::BUTTON_HandleEvent(SDL_Event* bEvent)
{
    if (bEvent->type == SDL_MOUSEMOTION || bEvent->type == SDL_MOUSEBUTTONDOWN || bEvent->type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        bool inside = true;
        if (x < bPosition.x + 8)
        {
            inside = false;
        }
        else if (x > bPosition.x + (BUTTON_GetWidth() / 10) - 12 )
        {
            inside = false;
        }
        else if (y < bPosition.y + 15)
        {
            inside = false;
        }
        else if (y > bPosition.y + (BUTTON_GetHeight() / 10) - 15)
        {
            inside = false;
        }

        if (inside == false)
        {
            bState = BUTTON_OUT;
        }
        else
        {
            switch (bEvent->type)
            {
                case SDL_MOUSEMOTION:
                    bState = BUTTON_OVER;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    bState = BUTTON_PRESS;
                    break;
                case SDL_MOUSEBUTTONUP:
                    bState = BUTTON_OVER;
                    break;
            }
        }
        
    }
}

void button::BUTTON_Render(int x, int y, screenMode mode)
{
    setTexture(bTexture[bState]);
    BUTTON_SetPosition(x, y);
    render(x, y, mode);
}

bool button::BUTTON_Load(std::string path_bOut, std::string path_bOver, std::string path_bPress)
{
    std::fstream getErr("Errors.txt");
    if (!(LoadMedia(path_bOut, 0xFF, 0xFF, 0xFF)))
    {
        getErr << "load button over" << SDL_GetError();
        return false;
    }
    else
    {   
        bTexture[BUTTON_OUT] = getTexture();
        if (!(LoadMedia(path_bOver, 0xFF, 0xFF, 0xFF)))
        {
            getErr << "load button in" << SDL_GetError();
            return false;
        }
        else
        {   
            bTexture[BUTTON_OVER] = getTexture();
            if (!(LoadMedia(path_bPress, 0xFF, 0xFF, 0xFF)))
            {
                getErr << "load button in" << SDL_GetError();
                return false;
            }
            else
            {   
                bTexture[BUTTON_PRESS] = getTexture();
                getErr.close();
                return true;
            }
            
        }
        
    }
    
}

/// @brief render text box with specific state
/// @param x position to render
/// @param y position to render
/// @param _specific_State <ButtonState> defined-data type
void button::BOX_Render(int x, int y, ButtonState _specific_State)
{
    setTexture(bTexture[_specific_State]);
    BUTTON_SetPosition(x, y);
    SDL_Rect clip {0, 0, 1900, 700};
    render(x, y, MINIMAL_SCREEN, &clip);
}

int button::BUTTON_GetWidth()
{
    return getWidth();
}

int button::BUTTON_GetHeight()
{
    return getHeight();
}

ButtonState button::BUTTON_GetState()
{
    return bState;
}

void button::BUTTON_ResetState()
{
    bState = BUTTON_OUT;
}

int WelcomeScene()
{
    std::fstream getErr("Errors.txt");  // Keep tranking on bug
    button quitButton;
    button loginButton;
    button signupButton;
    Graphic WelcomeBoard;

    // Load board image
    if ( !(WelcomeBoard.LoadMedia("image/green-board-blur1.jpg")) )
    {
        getErr << "Welcome scene broken stage 1 " << SDL_GetError();
    }
    WelcomeBoard.render(0, 0, FULL_SCREEN);

    // Load 3 kinds of state of button
    if (   !(loginButton.BUTTON_Load("image/login-out.png", "image/login-over.png", "image/login-down.png"))
        || !(signupButton.BUTTON_Load("image/signup-out.png", "image/signup-over.png", "image/signup-down.png"))
        || !(quitButton.BUTTON_Load("image/quit-out.png", "image/quit-over.png", "image/quit-down.png"))    )
    {
        getErr << "Welcome scene broken stage 2 " << SDL_GetError();
        getErr.close();
        return 3;
    }

    // event loop
    bool isRunningWelcomeScene = true;
    SDL_RenderPresent(gRenderer);
    while (isRunningWelcomeScene)
    {
        SDL_Event EV;
        while (SDL_PollEvent(&EV))
        {
            if (EV.type == SDL_QUIT)                    // stop the program if clicking on close
            {
                isRunningWelcomeScene = false;
            }
            else if (EV.type == SDL_KEYDOWN)
            {
                if (EV.key.keysym.sym == SDLK_ESCAPE)   // stop the program if pressing on escape key
                {
                    isRunningWelcomeScene = false;
                }
            }
            
            // Handle button event
            loginButton.BUTTON_HandleEvent(&EV);
            signupButton.BUTTON_HandleEvent(&EV);
            quitButton.BUTTON_HandleEvent(&EV);
        }

        // Update scene per frame
        WelcomeBoard.render(0, 0, FULL_SCREEN);
        loginButton.BUTTON_Render(1600 / 2 - 100, 300);
        signupButton.BUTTON_Render(1600 / 2 - 100, 400);
        quitButton.BUTTON_Render(1600 / 2 - 100, 500);

        SDL_RenderPresent(gRenderer);

        // jump out if encounting any events relating to button
        if (signupButton.BUTTON_GetState() == BUTTON_PRESS || loginButton.BUTTON_GetState() == BUTTON_PRESS || quitButton.BUTTON_GetState() == BUTTON_PRESS)
        {
            isRunningWelcomeScene = false;
        }    
    }

    if (quitButton.BUTTON_GetState() == BUTTON_PRESS)
    {
        getErr.close();
        return 3;
    } 
    else if (loginButton.BUTTON_GetState() == BUTTON_PRESS)
    {
        getErr.close();
        return 1;
    }
    else if (signupButton.BUTTON_GetState() == BUTTON_PRESS)
    {
        getErr.close();
        return 2;
    }
}

bool LoginScene()
{
    std::fstream getErr("Errors.txt");
    Graphic WelcomeBoard;
    Graphic boxLogin;
    button cancelButton;
    button box_username;
    Graphic username;
    button box_password;
    Graphic password;
    Graphic message;
    button EnterButton;
    user gUser;
    SDL_Color textColor = { 0, 0, 0 };

    // Load welcome
    if ( !(WelcomeBoard.LoadMedia("image/green-board-blur.jpg")) )
    {
        getErr << "Welcome scene broken stage 1 " << SDL_GetError();
    }

    // Load login box
    if ( !(boxLogin.LoadMedia("image/boxLogin.png", 84, 130, 53)))
    {
        getErr << "Error: Load box Login" << SDL_GetError();
    }
    
    if ( !(cancelButton.BUTTON_Load("image/cancel-out.png", "image/cancel-over.png", "image/cancel-down.png")) || 
         !(box_username.BUTTON_Load("image/box-out.png", "image/box-over.png", "image/box-typing.png"))        ||
         !(box_password.BUTTON_Load("image/box-out.png", "image/box-over.png", "image/box-typing.png"))        ||
         !(EnterButton.BUTTON_Load("image/enter-out.png", "image/enter-over.png", "image/enter-down.png"))       )
    {
        getErr << "Error: Load button log in scene " << SDL_GetError();
    }
    
    WelcomeBoard.render(0, 0, FULL_SCREEN);
    boxLogin.render(-150, -100);
    std::string __message_ = " ";
    std::string textInput1 = " ";
    std::string textInput2 = " ";
    std::string t_textInput1_;
    std::string t_textInput2_;
    SDL_StartTextInput();
    bool isRunningLogin = true;
    int bSwitch = 0;
    while (isRunningLogin)
    {
        SDL_Event EV;
        while (SDL_PollEvent(&EV))
        {
            if (EV.type == SDL_QUIT)
            {
                isRunningLogin = false;
            }
            box_username.BUTTON_HandleEvent(&EV);
            box_password.BUTTON_HandleEvent(&EV);
            EnterButton.BUTTON_HandleEvent(&EV);
            cancelButton.BUTTON_HandleEvent(&EV);
            if (box_username.BUTTON_GetState() == BUTTON_PRESS)
            {
                bSwitch = 1;
            }
            else if (box_password.BUTTON_GetState() == BUTTON_PRESS)
            {
                bSwitch = 2;
            }   
            if (EV.type == SDL_MOUSEBUTTONDOWN && box_username.BUTTON_GetState() == BUTTON_OUT && box_password.BUTTON_GetState() == BUTTON_OUT )
            {
                bSwitch = 0;
            }
            switch (bSwitch)
            {
            case 1:

                if (EV.type == SDL_KEYDOWN || EV.type == SDL_TEXTINPUT)
                {
                    __message_ = " ";
                    if (EV.type == SDL_KEYDOWN)
                    {
                        switch (EV.key.keysym.sym)
                        {
                        case SDLK_BACKSPACE:
                            if (textInput1.length() > 1)
                            {
                                textInput1 = textInput1.substr(0, textInput1.length() - 1);
                            }
                            break;
                        case SDLK_ESCAPE:
                            isRunningLogin = false;
                            break;
                        case SDLK_RETURN:
                            t_textInput1_ = textInput1.substr(1, textInput1.length() - 1);
                            t_textInput2_ = textInput2.substr(1, textInput2.length() - 1);
                            if (gUser.USER_CheckAccount(t_textInput1_, t_textInput2_) )
                            {   
                                SDL_StopTextInput();
                                getErr.close();
                                return false;
                            }
                            else
                            {
                                __message_ = "Incorrect !";
                                // return false;
                            }
                            break;
                        }
                    }
                    else if (EV.type == SDL_TEXTINPUT)
                    {
                        textInput1 += EV.text.text;
                    }   
                }
                break;

            case 2:

                if (EV.type == SDL_KEYDOWN || EV.type == SDL_TEXTINPUT)
                {
                    __message_ = " ";
                    switch (EV.key.keysym.sym)
                    {
                    case SDLK_BACKSPACE:
                        if (textInput2.length() > 1)
                        {
                            textInput2 = textInput2.substr(0, textInput2.length() - 1);
                        }
                        break;
                    case SDLK_ESCAPE:
                        isRunningLogin = false;
                        break;
                    case SDLK_RETURN:
                        t_textInput1_ = textInput1.substr(1, textInput1.length() - 1);
                        t_textInput2_ = textInput2.substr(1, textInput2.length() - 1);
                        if (gUser.USER_CheckAccount(t_textInput1_, t_textInput2_) )
                        {   
                            SDL_StopTextInput();
                            getErr.close();
                            return false;
                        }
                        else
                        {
                            __message_ = "Incorrect !";
                            // return false;
                        }
                        
                        break;
                    }
                    if (EV.type == SDL_TEXTINPUT)
                    {
                        textInput2 += EV.text.text;
                    }
                }
                break;
            }
        }
        username.loadFromText(textInput1, textColor);
        password.loadFromText(textInput2, textColor);
        message.loadFromText(__message_, textColor);
        WelcomeBoard.render(0, 0, FULL_SCREEN);
        boxLogin.render(-150, -100);
        box_username.BUTTON_Render(715, 325);
        box_password.BUTTON_Render(715, 425);
        EnterButton.BUTTON_Render(500, 520);
        cancelButton.BUTTON_Render(900, 520);


        switch (bSwitch)
        {
        case 1:
            box_username.BOX_Render(715, 325, BUTTON_PRESS);
            break;
        case 2:
            box_password.BOX_Render(715, 425, BUTTON_PRESS);
            break;
        }
        message.render(750, 500);
        username.render(725, 350);
        password.render(725, 450);
        SDL_RenderPresent(gRenderer);

        if (cancelButton.BUTTON_GetState() == BUTTON_PRESS)
        {
            return true;
        }
        else if (EnterButton.BUTTON_GetState() == BUTTON_PRESS)
        {
            t_textInput1_ = textInput1.substr(1, textInput1.length() - 1);
            t_textInput2_ = textInput2.substr(1, textInput2.length() - 1);
            if (gUser.USER_CheckAccount(t_textInput1_, t_textInput2_) )
            {   
                SDL_StopTextInput();
                getErr.close();
                return false;
            }
            else
            {
                __message_ = "Incorrect !";
                // return false;
            }
        }
        
    }
    getErr.close();
    SDL_StopTextInput();
    close(); 
}
    
bool SignupScene()
{
    // std::fstream getErr("Errors.txt");
    Graphic WelcomeBoard;
    Graphic boxSignup;
    button cancelButton;
    button box_username;
    Graphic username;
    button box_password;
    Graphic password;
    Graphic message;
    button EnterButton;
    user gUser;
    SDL_Color textColor = { 0, 0, 0 };

    // Load welcome
    if ( !(WelcomeBoard.LoadMedia("image/green-board-blur.jpg")) )
    {
        // getErr << "Welcome scene broken stage 1 " << SDL_GetError();
    }

    // Load login box
    if ( !(boxSignup.LoadMedia("image/boxSignup.png", 112, 173, 71)))
    {
        // getErr << "Error: Load box Login" << SDL_GetError();
    }
    
    if ( !(cancelButton.BUTTON_Load("image/cancel-out.png", "image/cancel-over.png", "image/cancel-down.png")) || 
         !(box_username.BUTTON_Load("image/box-out.png", "image/box-over.png", "image/box-typing.png")) ||
         !(box_password.BUTTON_Load("image/box-out.png", "image/box-over.png", "image/box-typing.png")) )
    {
        // getErr << "Error: Load button log in scene " << SDL_GetError();
    }
    
    WelcomeBoard.render(0, 0, FULL_SCREEN);
    boxSignup.render(-150, -100);
    std::string __message_ = " ";
    std::string textInput1 = " ";
    std::string textInput2 = " ";
    std::string t_textInput1_;
    std::string t_textInput2_;
    SDL_StartTextInput();
    bool isRunningSignUp = true;
    int bSwitch = 0;
    while (isRunningSignUp)
    {
        SDL_Event EV;
        while (SDL_PollEvent(&EV))
        {
            if (EV.type == SDL_QUIT)
            {
                isRunningSignUp = false;
            }
            box_username.BUTTON_HandleEvent(&EV);
            box_password.BUTTON_HandleEvent(&EV);
            EnterButton.BUTTON_HandleEvent(&EV);
            cancelButton.BUTTON_HandleEvent(&EV);
            if (box_username.BUTTON_GetState() == BUTTON_PRESS)
            {
                bSwitch = 1;
            }
            else if (box_password.BUTTON_GetState() == BUTTON_PRESS)
            {
                bSwitch = 2;
            }   
            if (EV.type == SDL_MOUSEBUTTONDOWN && box_username.BUTTON_GetState() == BUTTON_OUT && box_password.BUTTON_GetState() == BUTTON_OUT )
            {
                bSwitch = 0;
            }
            switch (bSwitch)
            {
            case 1:

                if (EV.type == SDL_KEYDOWN || EV.type == SDL_TEXTINPUT)
                {
                    __message_ = " ";
                    if (EV.type == SDL_KEYDOWN)
                    {
                        switch (EV.key.keysym.sym)
                        {
                        case SDLK_BACKSPACE:
                            if (textInput1.length() > 1)
                            {
                                textInput1 = textInput1.substr(0, textInput1.length() - 1);
                            }
                            break;
                        case SDLK_ESCAPE:
                            isRunningSignUp = false;
                            break;
                        case SDLK_RETURN:
                            t_textInput1_ = textInput1.substr(1, textInput1.length() - 1);
                            t_textInput2_ = textInput2.substr(1, textInput2.length() - 1);
                            if (gUser.USER_CheckAccount(t_textInput1_, t_textInput2_) )
                            {   
                                __message_ = "this account had been existed !";
                            }
                            else
                            {
                                if (gUser.USER_CreateAccount(t_textInput1_, t_textInput2_))
                                {
                                    SDL_StopTextInput();
                                    // getErr.close();
                                    return false;
                                }
                                else
                                {
                                    // getErr << "Can not create account \n"; 
                                }
                                
                            }
                            break;
                        }
                    }
                    else if (EV.type == SDL_TEXTINPUT)
                    {
                        textInput1 += EV.text.text;
                    }   
                }
                break;

            case 2:

                if (EV.type == SDL_KEYDOWN || EV.type == SDL_TEXTINPUT)
                {
                    __message_ = " ";
                    switch (EV.key.keysym.sym)
                    {
                    case SDLK_BACKSPACE:
                        if (textInput2.length() > 1)
                        {
                            textInput2 = textInput2.substr(0, textInput2.length() - 1);
                        }
                        break;
                    case SDLK_ESCAPE:
                        isRunningSignUp = false;
                        break;
                    case SDLK_RETURN:
                        t_textInput1_ = textInput1.substr(1, textInput1.length() - 1);
                        t_textInput2_ = textInput2.substr(1, textInput2.length() - 1);
                        if (gUser.USER_CheckAccount(t_textInput1_, t_textInput2_) )
                        {   
                            __message_ = "this account had been existed !";
                        }
                        else
                        {
                            if (gUser.USER_CreateAccount(t_textInput1_, t_textInput2_))
                            {
                                SDL_StopTextInput();
                                // getErr.close();
                                return false;
                            }
                            else
                            {
                                // getErr << "Can not create account \n"; 
                            }
                            
                        }
                        
                        break;
                    }
                    if (EV.type == SDL_TEXTINPUT)
                    {
                        textInput2 += EV.text.text;
                    }
                }
                break;
            }
        }
        username.loadFromText(textInput1, textColor);
        password.loadFromText(textInput2, textColor);
        message.loadFromText(__message_, textColor);
        WelcomeBoard.render(0, 0, FULL_SCREEN);
        boxSignup.render(-150, -100);
        box_username.BUTTON_Render(715, 325);
        box_password.BUTTON_Render(715, 425);
        EnterButton.BUTTON_Render(550, 520);
        cancelButton.BUTTON_Render(900, 520);

        

        switch (bSwitch)
        {
        case 1:
            box_username.BOX_Render(715, 325, BUTTON_PRESS);
            break;
        case 2:
            box_password.BOX_Render(715, 425, BUTTON_PRESS);
            break;
        }
        message.render(640, 490);
        username.render(725, 350);
        password.render(725, 450);
        SDL_RenderPresent(gRenderer);

        if (cancelButton.BUTTON_GetState() == BUTTON_PRESS)
        {
            return true;
        }
        // else if (EnterButton.BUTTON_GetState() == BUTTON_PRESS)
        // {
            
        // }
        
    }
    // getErr.close();
    SDL_StopTextInput();
    close(); 
}

void getNoCard(button txBox, Graphic box, SDL_Color color)
{   
    std::fstream getErr("Errors.txt");
    bool isTyping = false;
    Graphic amoCard;
    std::string numInput = " ";
    bool isRunning = true;
    while (isRunning)
    {
        SDL_Event subEV;
        while (SDL_PollEvent(&subEV))
        {
            if (subEV.type == SDL_QUIT)
            {
                close();
            }
            

            txBox.BUTTON_HandleEvent(&subEV);
            if (subEV.type == SDL_MOUSEBUTTONDOWN &&  txBox.BUTTON_GetState() == BUTTON_OUT)
            {
                isTyping = false;
            }
            
            if (isTyping)
            {
                // TextBox.BUTTON_HandleEvent(&EV);
                if (subEV.type == SDL_TEXTINPUT || subEV.type == SDL_KEYDOWN)
                {
                    if (subEV.type == SDL_KEYDOWN)
                    {
                        switch (subEV.key.keysym.sym)
                        {
                        case SDLK_BACKSPACE:
                            if (numInput.length() > 1)
                            {
                                numInput = numInput.substr(0, numInput.length() - 1);
                            }
                            break;
                        case SDLK_ESCAPE:
                            isRunning = false;
                            break;
                        case SDLK_RETURN:
                            POKER_RUSH_NO_CARDS = std::stoi(numInput);
                            if (POKER_RUSH_NO_CARDS < 8 && POKER_RUSH_NO_CARDS > 4)
                            {
                                getErr.close();
                                SDL_StopTextInput();
                                return;
                            }
                            break;
                        }
                    }
                    else if (subEV.type == SDL_TEXTINPUT)
                    {
                        char* temp = subEV.text.text;
                        if (*temp >= '0' && *temp <= '9')
                        {
                            numInput += *temp;
                        }
                    }
                
                }
            }
        }

        box.render(-150, -100);
        txBox.BUTTON_Render(700, 400);
        if (txBox.BUTTON_GetState() == BUTTON_PRESS)
        {
            isTyping = true;
        }
        
        if (isTyping)
        {
            txBox.BOX_Render(700, 400,BUTTON_PRESS);
            amoCard.free();
            amoCard.loadFromText(numInput, color);
            amoCard.render(725, 430);
        }
        SDL_RenderPresent(gRenderer);
    }
    
}

int lobbyScene()
{
    std::fstream getErr("Errors.txt");
    std::ifstream getWinRate;
    std::ifstream getUsername;
    std::ifstream getCoin;
    button TextBox;
    button quitButton;
    button Texas;
    button PoRush;
    button Cancel;
    Graphic Board;
    Graphic winRate;
    Graphic username_GUI;
    Graphic coin_GUI;
    Graphic money;
    Graphic amoPlayers;
    Graphic boxNoP;
    Graphic boxNoCa;
    std::string numInput = " ";
    // std::string t_numInput_;
    SDL_Color black = {0, 0, 0};
    if ( !(Board.LoadMedia("image/green-board-blur.jpg"))  ||
         !(boxNoP.LoadMedia("image/box-no-player.png", 112, 173, 71))    ||
         !(boxNoCa.LoadMedia("image/box-no-cards.png", 112, 173, 71)) )
    {
        getErr << "Load image" << '\n';
    }
    else
    {
        if ( !(Texas.BUTTON_Load("image/texas-out.png", "image/texas-over.png", "image/texas-down.png"))  ||
             !(PoRush.BUTTON_Load("image/rush-out.png", "image/rush-over.png", "image/rush-down.png"))    ||   
             !(TextBox.BUTTON_Load("image/box-out.png", "image/box-over.png", "image/box-typing.png"))    ||
             !(Cancel.BUTTON_Load("image/cancel-out.png", "image/cancel-over.png", "image/cancel-down.png"))   )
        {
            getErr << "Load Button" << '\n';
        }
    }
    
    getWinRate.open("UserData/Pos.txt");
    getUsername.open("UserData/account.txt");
    getCoin.open("UserData/coin.txt");
    if ( !(getWinRate.is_open()) || !(getUsername.is_open()) || !(getCoin.is_open()) ) 
    {
        getErr << "Can not open Pos.txt/ account.txt/ coin.txt file" << '\n';
        getErr.close();
        return 0;
    }
    
    int pos;
    getWinRate >> pos;
    getWinRate.close();
    getWinRate.open("UserData/winrate.txt");
    if ( !(getWinRate.is_open()) )
    {
        getErr << "Can not open winrate.txt file" << '\n';
        getErr.close();
        return 0;
    }

    int No_Win;
    int No_lose;
    int coin;
    std::string usernameStr;
    for (int i = 0; i < pos; i++)
    {
        getline(getUsername, usernameStr);
        getWinRate >> No_Win;
        getWinRate >> No_lose;
        getCoin >> coin;
    }

    getWinRate.close();
    getCoin.close();
    getUsername.close();
    
    SDL_Color white = {0xFF, 0xFF, 0xFF};
    username_GUI.loadFromText( "Hello! " + usernameStr, white);
    coin_GUI.loadFromText("money :  $ " + std::to_string(coin), white);
    float rate = (No_Win * 100.0) / (No_Win * 1.0 + No_lose);
    winRate.loadFromText( "Win rate : " + std::to_string( std::ceil(rate * 100.0) / 100.0 ) + " %", white);

    bool isTyping = false;
    SDL_StartTextInput();
    bool isRunning = true;
    int mode = 0;
    while (isRunning)
    {
        SDL_Event EV;
        while (SDL_PollEvent(&EV))
        {
            if (EV.type == SDL_QUIT)
            {
                isRunning = false;
            }
            if (mode == 0)
            {
                PoRush.BUTTON_HandleEvent(&EV);
                Texas.BUTTON_HandleEvent(&EV);
            }
            switch (mode)
            {
            case 1:
                TextBox.BUTTON_HandleEvent(&EV);
                if (EV.type == SDL_MOUSEBUTTONDOWN &&  TextBox.BUTTON_GetState() == BUTTON_OUT)
                {
                    isTyping = false;
                }
                
                if (isTyping)
                {
                    // TextBox.BUTTON_HandleEvent(&EV);
                    if (EV.type == SDL_TEXTINPUT || EV.type == SDL_KEYDOWN)
                    {
                        if (EV.type == SDL_KEYDOWN)
                        {
                            switch (EV.key.keysym.sym)
                            {
                            case SDLK_BACKSPACE:
                                if (numInput.length() > 1)
                                {
                                    numInput = numInput.substr(0, numInput.length() - 1);
                                }
                                break;
                            case SDLK_ESCAPE:
                                isRunning = false;
                                break;
                            case SDLK_RETURN:
                                PLAYERS_NUM = std::stoi(numInput);
                                if (PLAYERS_NUM < 8 && PLAYERS_NUM > 2)
                                {
                                    CheckedCards = new int*[PLAYERS_NUM];
                                    for (int i = 0; i < PLAYERS_NUM; i++)
                                    {
                                        CheckedCards[i] = new int[CARDS_NUM_ONE_HAND];
                                    }
                                    getErr.close();
                                    SDL_StopTextInput();
                                    return 1;
                                }
                                break;
                            }
                        }
                        else if (EV.type == SDL_TEXTINPUT)
                        {
                            char* temp = EV.text.text;
                            if (*temp >= '0' && *temp <= '9')
                            {
                                numInput += *temp;
                            }
                        }
                    
                    }
                }
                break;
            case 2:
                TextBox.BUTTON_HandleEvent(&EV);
                if (EV.type == SDL_MOUSEBUTTONDOWN &&  TextBox.BUTTON_GetState() == BUTTON_OUT)
                {
                    isTyping = false;
                }
                
                if (isTyping)
                {
                    if (EV.type == SDL_TEXTINPUT || EV.type == SDL_KEYDOWN)
                    {
                        if (EV.type == SDL_KEYDOWN)
                        {
                            switch (EV.key.keysym.sym)
                            {
                            case SDLK_BACKSPACE:
                                if (numInput.length() > 1)
                                {
                                    numInput = numInput.substr(0, numInput.length() - 1);
                                }
                                break;
                            case SDLK_ESCAPE:
                                isRunning = false;
                                break;
                            case SDLK_RETURN:
                                PLAYERS_NUM = std::stoi(numInput);
                                if (PLAYERS_NUM < 8 && PLAYERS_NUM > 2)
                                {
                                    getErr.close();
                                    getNoCard(TextBox, boxNoCa, black);
                                    SDL_StopTextInput();
                                    return 2;
                                }
                                break;
                            }
                        }
                        else if (EV.type == SDL_TEXTINPUT)
                        {
                            char* temp = EV.text.text;
                            if (*temp >= '0' && *temp <= '9')
                            {
                                numInput += *temp;
                            }
                        }
                    
                    }
                }
                break;
            }
            
        }
        amoPlayers.loadFromText(numInput, black);
        Board.render(0, 0, FULL_SCREEN);
        Texas.BUTTON_Render(570, 500);
        PoRush.BUTTON_Render(800, 500);
        username_GUI.render(690, 350);
        coin_GUI.render(690, 400);
        winRate.render(690, 450);

        if (Texas.BUTTON_GetState() == BUTTON_PRESS && mode == 0)
        {
            mode = 1;
        }
        else if (PoRush.BUTTON_GetState() == BUTTON_PRESS && mode == 0)
        {
            mode = 2;
        }
        
        switch (mode)
        {
        case 1:
            boxNoP.render(-150, -100);
            TextBox.BUTTON_Render(700, 400);
            if (TextBox.BUTTON_GetState() == BUTTON_PRESS)
            {
                isTyping = true;
            }
            
            break;
        case 2:
            boxNoP.render(-150, -100);
            TextBox.BUTTON_Render(700, 400);
            if (TextBox.BUTTON_GetState() == BUTTON_PRESS)
            {
                isTyping = true;
            }
            break;
        }
        
        if (isTyping)
        {
            TextBox.BOX_Render(700, 400, BUTTON_PRESS);
            amoPlayers.render(725, 430);
        }
        else
        {
            
        }
        
        SDL_RenderPresent(gRenderer);
    }
    
    getErr.close();
    SDL_StopTextInput();
    return 0;
}

/// @brief This function will run the Texas Hold'em game which is the most popular and well-known on the world generally and in the casino field specifically
/// taking only <card> data defined-type 
/// @param CARDS that is defined in deck database
/// @return true if everything works correctly or the user want to back to lobby scene, otherwise false
bool gameplay(card CARDS[])
{
    std::fstream getErr("Errors.txt");  // for tracking bugs
    player gPlayers[PLAYERS_NUM];       // player array holding all important stuffs to manipulate
    button raise;                       // For rendering raise button
    button call;                        // For rendering call button
    button check;                       // For rendering check button
    button fold;                        // For Rendering fold button
    button homeButton;                  // For Rendering home button
    Graphic Board;                      // Board image only
    Graphic downCard;                   // For rendering face-down card image
    Graphic coin;     
    Graphic D;

    // Set money $1000 to each bot by default
    for (int i = 1; i < PLAYERS_NUM; i++)
    {
        gPlayers[i].coin = 1000;
    }
    
    // this variable will take white color information
    SDL_Color white = {0xFF, 0xFF, 0xFF};

    // Get the position of loaded account
    std::ifstream getAccPos("UserData/Pos.txt");
    int pos;
    getAccPos >> pos;
    getAccPos.close();

    // Get money of that account base on its position has just been gotten
    std::fstream getCoin("UserData/coin.txt");
    for (int i = 0; i < pos; i++)
    {
        getCoin >> gPlayers[0].coin;
    }

    // Load board game image
    if ( !(Board.LoadMedia("image/green-board.jpg")))
    {
        getErr << "Load board" << '\n';
        getErr.close();
        return false;
    }
    else
    {
        // Load face-down card image
        // Load dealer button image
        if ( !(downCard.LoadMedia("image/face-down-card.png"))      || 
             !(D.LoadMedia("image/D-button.png", 0xFF, 0xFF, 0xFF)))
        {
            getErr << "Load down-card / Dealer image" << '\n';
            getErr.close();
            return false;
        }
        else
        {
            // Load user's money as texture
            if ( !(coin.loadFromText(std::to_string(gPlayers[0].coin), white)))
            {
                getErr << "Error: Load coin text" << '\n';
                getErr.close();
                return false;
            }
            else
            {
                // Load raise, call, check, fold and home butotn as three textures for three kinds of button states
                if ( !(raise.BUTTON_Load("image/raise-out.png", "image/raise-over.png", "image/raise-down.png")) || 
                    !(call.BUTTON_Load("image/call-out.png", "image/call-over.png", "image/call-down.png"))      || 
                    !(check.BUTTON_Load("image/check-out.png", "image/check-over.png", "image/check-down.png"))  || 
                    !(fold.BUTTON_Load("image/fold-out.png", "image/fold-over.png", "image/fold-down.png"))     ||
                    !(homeButton.BUTTON_Load("image/homeButton-out.png", "image/homeButton-over.png", "image/homeButton-down.png")) )
                {
                    getErr << "Error: Load button in game" << '\n';
                    getErr.close();
                    return false;
                }

            }
            
        }
        
    }

    // By the fault, 0 will be that user/player playing this game at first
    int k = 0;

    // main loop game
    bool BIG_RUNNING = true;
    while (BIG_RUNNING)
    {
        // Shuffle deck and deal for cards for each player
        ShuffleDeck();
        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            for (int j = 0; j < CARDS_NUM_ONE_HAND; j++)
            {
                gPlayers[i].hand[j] = CARDS[CheckedCards[i][j]];
            }
        }
        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            for (int j = 2; j < COMUNITY_CARDS + 2; j++)
            {
                gPlayers[i].hand[j] = CARDS[CommunityCards[j - 2]];
            }
        }
        Board.render(0, 0, FULL_SCREEN);
        SDL_RenderPresent(gRenderer);
        /***************************************************************/


        // Position (x, y) process field
        int centerX = 1600 / 2;
        int centerY = 900 / 2;
        int R = 250;
        gPlayers[0].x = 1600 / 2;
        gPlayers[0].y = 900 / 2 + R;
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            gPlayers[i].x = centerX - R * sin((2 * M_PI / PLAYERS_NUM) * i);
            gPlayers[i].y = centerY + R * cos((2 * M_PI / PLAYERS_NUM) * i) - 100;

            if (gPlayers[i].x < centerX - 10)
            {
                gPlayers[i].x -= 500;
            }
            else if (gPlayers[i].x > centerX + 10)
            {
                gPlayers[i].x += 255;
            }
            else
            {
                gPlayers[i].x -= 100;
            }
        }
        /***************************************************************/


        // Use the timer for handling time delay
        uint32_t timer = SDL_GetTicks();

        // Render dealing cards for bot stage
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            SDL_Event EV;
            downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
            SDL_RenderPresent(gRenderer);
            while (SDL_GetTicks() - timer < 200) {SDL_PollEvent(&EV);}
            timer = SDL_GetTicks();

            downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );
            SDL_RenderPresent(gRenderer);
            while (SDL_GetTicks() - timer < 200) {SDL_PollEvent(&EV);}
            timer = SDL_GetTicks();
            
            coin.free();
            coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin), white);
            coin.render(gPlayers[i].x + 50, gPlayers[i].y + 200);
            SDL_RenderPresent(gRenderer);
            while (SDL_GetTicks() - timer < 200) {SDL_PollEvent(&EV);}
            timer = SDL_GetTicks();
        }
        /***************************************************************/

        
        // Reset string state action
        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            gPlayers[i].TextOpt = " ";
        }
        /**************************************************************/

        // Render cards for user stage
        gPlayers[0].x = 700;    // Adjust user's position
        gPlayers[0].y = 570;
        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();
        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();
        coin.free();
        coin.loadFromText("me $" + std::to_string(gPlayers[0].coin), white);
        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();
        SDL_RenderPresent(gRenderer);
        /***************************************************************/

        // Render 5 community cards at center the board
        for (int i = 0; i < COMUNITY_CARDS; i++)
        {
            downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
            SDL_Event EV;
            while (SDL_GetTicks() - timer < 200) {SDL_PollEvent(&EV);}
            timer = SDL_GetTicks();
            SDL_RenderPresent(gRenderer);
        }
        
        int TotalCoin = 0;
        coin.free();
        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
        coin.render(200, 55);
        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();

        gPlayers[k + 1].coin--;
        TotalCoin++;
        gPlayers[k + 1].TextOpt = "Small Blind";

        Board.render(0, 0, FULL_SCREEN);
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
            downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

            coin.free();
            coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
            coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
        }

        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
        coin.free();
        coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
        for (int i = 0; i < COMUNITY_CARDS; i++)
        {
            downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
        }
        coin.free();
        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
        coin.render(200, 55);
        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);
        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();

        gPlayers[k + 2].coin -=2;
        TotalCoin += 2;
        gPlayers[k + 2].TextOpt = "Big Blind";

        Board.render(0, 0, FULL_SCREEN);
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
            downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

            coin.free();
            coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
            coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
        }

        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
        coin.free();
        coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
        for (int i = 0; i < COMUNITY_CARDS; i++)
        {
            downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
        }
        coin.free();
        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
        coin.render(200, 55);
        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);
        SDL_RenderPresent(gRenderer);
        // while (SDL_GetTicks() - timer < 400) {}
        // timer = SDL_GetTicks();

        gPlayers[k].D = true;
        
        int face_up_card = 2;
        int Start_Raise;
        // int Pre_Start_Raise = Start_Raise;
        int run = (k + 3) % PLAYERS_NUM;
        int val_raise = 2;
        bool isRaising = false;
        
        // Fisrt betting stage
        int mainRun = run;
        for (int z = 0; z < PLAYERS_NUM; z++)
        {
            if (mainRun == 0 && !gPlayers[0].Fold)
            {
                bool isPlaying = true;
                while (isPlaying)
                {
                    SDL_Event EV;
                    while (SDL_PollEvent(&EV))
                    {
                        if (EV.type == SDL_QUIT)
                        {
                            isPlaying = false;
                            return false;
                        }
                        
                        raise.BUTTON_HandleEvent(&EV);
                        fold.BUTTON_HandleEvent(&EV);
                        call.BUTTON_HandleEvent(&EV);
                    }
                    fold.BUTTON_Render(450, 750);
                    call.BUTTON_Render(660, 750);
                    raise.BUTTON_Render(900, 750);
                    SDL_RenderPresent(gRenderer);

                    if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || call.BUTTON_GetState() == BUTTON_PRESS )
                    {
                        isPlaying = false;
                    }
                }

                if (raise.BUTTON_GetState() == BUTTON_PRESS)
                {
                    isRaising = true;
                    gPlayers[0].raised = true;
                    gPlayers[0].TextOpt = "Raise +$6";
                    Start_Raise = 0;
                    run = 1;
                    val_raise += 6;
                    gPlayers[0].coin -= val_raise;
                    TotalCoin += val_raise;
                }
                else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                {
                    gPlayers[0].TextOpt = "Fold";
                    gPlayers[0].Fold = true;
                }
                else if (call.BUTTON_GetState() == BUTTON_PRESS)
                {
                    gPlayers[0].TextOpt = "Call";
                    gPlayers[0].coin -= val_raise;
                    TotalCoin += val_raise;
                }

                raise.BUTTON_ResetState();
                fold.BUTTON_ResetState();
                call.BUTTON_ResetState();
                check.BUTTON_ResetState();
            }
            else
            {
                if ( gPlayers[mainRun].handRanking(face_up_card) == ONE_PAIR )
                {
                    isRaising = true;
                    gPlayers[mainRun].raised = true;
                    gPlayers[mainRun].TextOpt = "Raise +$6";
                    val_raise += 6;
                    TotalCoin += val_raise;
                    gPlayers[mainRun].coin -= val_raise;
                    Start_Raise = mainRun;
                    run = (mainRun + 1) % PLAYERS_NUM;
                }
                else
                {
                    gPlayers[mainRun].TextOpt = "Call";
                    TotalCoin += val_raise;
                    gPlayers[mainRun].coin -= val_raise;
                }
            }
            
            if (isRaising == true)
            {
                do 
                {
                    if(run % PLAYERS_NUM == 0 && !(gPlayers[0].Fold) )
                    {
                        bool isPlaying = true;
                        while (isPlaying)
                        {
                            SDL_Event EV;
                            while (SDL_PollEvent(&EV))
                            {
                                if (EV.type == SDL_QUIT)
                                {
                                    isPlaying = false;
                                    return false;
                                }
                                
                                raise.BUTTON_HandleEvent(&EV);
                                fold.BUTTON_HandleEvent(&EV);
                                call.BUTTON_HandleEvent(&EV);
                            }
                            fold.BUTTON_Render(450, 750);
                            call.BUTTON_Render(660, 750);
                            raise.BUTTON_Render(900, 750);
                            SDL_RenderPresent(gRenderer);

                            if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || call.BUTTON_GetState() == BUTTON_PRESS )
                            {
                                isPlaying = false;
                            }
                        }

                        if (raise.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            isRaising = true;
                            gPlayers[0].raised = true;
                            gPlayers[0].TextOpt = "Raise +$6";
                            val_raise += 6;
                            gPlayers[0].coin -= val_raise;
                            TotalCoin += val_raise;
                            Start_Raise = 0;
                        }
                        else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            gPlayers[0].TextOpt = "Fold";
                            gPlayers[0].Fold = true;
                        }
                        else if (call.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            gPlayers[0].TextOpt = "Call";
                            gPlayers[0].coin -= val_raise;
                            TotalCoin += val_raise;
                        }

                        raise.BUTTON_ResetState();
                        fold.BUTTON_ResetState();
                        call.BUTTON_ResetState();
                        check.BUTTON_ResetState();
                    }
                    else
                    {          
                        if ( gPlayers[run].handRanking(face_up_card) == ONE_PAIR && !gPlayers[run].raised )
                        {
                            gPlayers[run].TextOpt = "Raise $6";
                            val_raise += 6;
                            Start_Raise = run % PLAYERS_NUM;
                            gPlayers[run].raised = true;
                        }
                        else
                        {
                            gPlayers[run].TextOpt = "Call";
                        }

                        TotalCoin += val_raise;
                        gPlayers[run].coin -= val_raise;
                    }

                    Board.render(0, 0, FULL_SCREEN);
                    for (int i = 1; i < PLAYERS_NUM; i++)
                    {
                        downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
                        downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

                        coin.free();
                        coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
                        coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
                    }

                    gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
                    gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
                    coin.free();
                    coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
                    coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
                    for (int i = 0; i < COMUNITY_CARDS; i++)
                    {
                        downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
                    }
                    coin.free();
                    coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
                    coin.render(200, 55);
                    D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

                    SDL_RenderPresent(gRenderer);
                    while (SDL_GetTicks() - timer < 200) {}
                    timer = SDL_GetTicks();
                    run = (run + 1) % PLAYERS_NUM;

                } while ((run % PLAYERS_NUM) != Start_Raise); 

                break;
            }

            Board.render(0, 0, FULL_SCREEN);
            for (int i = 1; i < PLAYERS_NUM; i++)
            {
                downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
                downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

                coin.free();
                coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
                coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
            }

            gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
            gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
            coin.free();
            coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
            coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
            for (int i = 0; i < COMUNITY_CARDS; i++)
            {
                downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
            }
            coin.free();
            coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
            coin.render(200, 55);
            D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

            SDL_RenderPresent(gRenderer);
            while (SDL_GetTicks() - timer < 200) {}
            timer = SDL_GetTicks();

            mainRun = (mainRun + 1) % PLAYERS_NUM;
        }

        // test 
        // while (SDL_GetTicks() - timer < 4000) {}
        // timer = SDL_GetTicks();

        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            gPlayers[i].raised = false;
        }
        
        face_up_card += 3;
        // for (int i = 0; i < face_up_card - 2; i++)
        // {
        //     gPlayers[0].hand[i + 2].render(390 + i * 150, 350, QUARTER_SCREEN);
        //     SDL_RenderPresent(gRenderer);
        //     while (SDL_GetTicks() - timer < 200) {}
        //     timer = SDL_GetTicks();
        // }
        
        // Start_Raise = (Start_Raise - 1) % PLAYERS_NUM;
        // if (gPlayers[Start_Raise].handRanking(face_up_card) > FOUR_OF_A_KIND)
        // {
        //     gPlayers[Start_Raise].coin -= 70;
        //     val_raise += 70;
        // }

        // Flop turn third and fourth
        for (int flop = 0; flop < 2; flop++)
        {
            mainRun = (k + 1) % PLAYERS_NUM;
            isRaising = false;

            for (int z = 0; z < PLAYERS_NUM; z++)
            {
                if (mainRun == 0 && !(gPlayers[0].Fold) )
                {
                    bool isPlaying = true;
                    while (isPlaying)
                    {
                        SDL_Event EV;
                        while (SDL_PollEvent(&EV))
                        {
                            if (EV.type == SDL_QUIT)
                            {
                                isPlaying = false;
                                return false;
                            }
                            
                            fold.BUTTON_HandleEvent(&EV);
                            check.BUTTON_HandleEvent(&EV);
                            raise.BUTTON_HandleEvent(&EV);
                        }
                        fold.BUTTON_Render(450, 750);
                        check.BUTTON_Render(660, 750);
                        raise.BUTTON_Render(900, 750);
                        SDL_RenderPresent(gRenderer);

                        if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || check.BUTTON_GetState() == BUTTON_PRESS )
                        {
                            isPlaying = false;
                        }
                    }

                    if (raise.BUTTON_GetState() == BUTTON_PRESS)
                    {
                        isRaising = true;
                        gPlayers[0].raised = true;
                        gPlayers[0].TextOpt = "Raise +$20";
                        val_raise += 20;        // NOTE !!! : Update late
                        TotalCoin += val_raise;
                        gPlayers[0].coin -= val_raise;
                        Start_Raise = 0;
                        run = 1;
                    }
                    else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                    {
                        gPlayers[0].TextOpt = "Fold";
                        gPlayers[0].Fold = true;
                    }
                    else if (check.BUTTON_GetState() == BUTTON_PRESS)
                    {
                        gPlayers[0].TextOpt = "Check";
                    }

                    raise.BUTTON_ResetState();
                    fold.BUTTON_ResetState();
                    call.BUTTON_ResetState();
                    check.BUTTON_ResetState();
                }
                else
                {
                    if (gPlayers[mainRun].handRanking(face_up_card) > ONE_PAIR && !gPlayers[mainRun].raised )
                    {
                        isRaising = true;
                        gPlayers[mainRun].raised = true;
                        
                        if (gPlayers[mainRun].handRanking(face_up_card) > FOUR_OF_A_KIND)
                        {
                            val_raise += 70;
                            gPlayers[mainRun].TextOpt = "Raise +$70";
                        }
                        else if (gPlayers[mainRun].handRanking(face_up_card) > STRAIGHT)
                        {
                            val_raise += 50;
                            gPlayers[mainRun].TextOpt = "Raise +$50";
                        }
                        else if (gPlayers[mainRun].handRanking(face_up_card) > ONE_PAIR)
                        {
                            val_raise += 30;
                            gPlayers[mainRun].TextOpt = "Raise +$30";
                        }
                        TotalCoin += val_raise;
                        gPlayers[mainRun].coin -= val_raise;
                        run = (mainRun + 1) % PLAYERS_NUM;
                        Start_Raise = mainRun % PLAYERS_NUM;
                    }
                    else
                    {
                        gPlayers[mainRun].TextOpt = "Check";
                    }
                }
                
                
                // Start raise phase
                if (isRaising == true)
                {
                    do 
                    {
                        // the player who plays this game
                        if(run % PLAYERS_NUM == 0 && !(gPlayers[0].Fold) )
                        {
                            bool isPlaying = true;
                            while (isPlaying)
                            {
                                SDL_Event EV;
                                while (SDL_PollEvent(&EV))
                                {
                                    if (EV.type == SDL_QUIT)
                                    {
                                        isPlaying = false;
                                        return false;
                                    }
                                    
                                    // Handles mouse event
                                    raise.BUTTON_HandleEvent(&EV);
                                    call.BUTTON_HandleEvent(&EV);
                                    fold.BUTTON_HandleEvent(&EV);
                                }

                                // button GUI
                                fold.BUTTON_Render(450, 750);
                                call.BUTTON_Render(660, 750);
                                raise.BUTTON_Render(900, 750);
                                SDL_RenderPresent(gRenderer);

                                if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || call.BUTTON_GetState() == BUTTON_PRESS )
                                {
                                    isPlaying = false;
                                }
                            }

                            if (raise.BUTTON_GetState() == BUTTON_PRESS)
                            {
                                isRaising = true;
                                gPlayers[0].raised = true;
                                gPlayers[0].TextOpt = "Raise +$20";
                                val_raise += 20;            // NOTE !!! : Update later
                                gPlayers[0].coin -= val_raise;
                                TotalCoin += val_raise;
                                Start_Raise = 0;
                            }
                            else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                            {
                                gPlayers[0].TextOpt = "Fold";
                                gPlayers[0].Fold = true;
                            }
                            else if (call.BUTTON_GetState() == BUTTON_PRESS)
                            {
                                gPlayers[0].TextOpt = "Call";
                                gPlayers[0].coin -= val_raise;
                                TotalCoin += val_raise;
                            }

                            raise.BUTTON_ResetState();
                            fold.BUTTON_ResetState();
                            call.BUTTON_ResetState();
                            check.BUTTON_ResetState();
                        }
                        // Bot processing heres
                        else
                        {
                            if (gPlayers[run].handRanking(face_up_card) > STRAIGHT && !gPlayers[run].raised)
                            {
                                gPlayers[run].raised = true;
                                isRaising = true;
                                
                                if (gPlayers[mainRun].handRanking(face_up_card) > FOUR_OF_A_KIND)
                                {
                                    val_raise += 70;
                                    gPlayers[run].TextOpt = "Raise +$70";
                                }
                                else if (gPlayers[mainRun].handRanking(face_up_card) > STRAIGHT)
                                {
                                    val_raise += 50;
                                    gPlayers[run].TextOpt = "Raise +$50";
                                }
                                else if (gPlayers[mainRun].handRanking(face_up_card) > ONE_PAIR)
                                {
                                    val_raise += 30;
                                    gPlayers[run].TextOpt = "Raise +$30";
                                }
                                Start_Raise = run % PLAYERS_NUM;
                            }
                            else
                            {
                                gPlayers[run].TextOpt = "Call";
                            }

                            gPlayers[run].coin -= val_raise;
                            TotalCoin += val_raise;
                        }

                        // Update Screen
                        Board.render(0, 0, FULL_SCREEN);
                        for (int i = 1; i < PLAYERS_NUM; i++)
                        {
                            downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
                            downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

                            coin.free();
                            coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
                            coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
                        }

                        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
                        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
                        coin.free();
                        coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
                        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
                        for (int i = 0; i < COMUNITY_CARDS; i++)
                        {
                            downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
                        }
                        coin.free();
                        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
                        coin.render(200, 55);
                        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

                        for (int i = 0; i < face_up_card - 2; i++)
                        {
                            gPlayers[0].hand[i + 2].render(390 + i * 150, 350, QUARTER_SCREEN);
                        }

                        SDL_RenderPresent(gRenderer);
                        while (SDL_GetTicks() - timer < 200) {}
                        timer = SDL_GetTicks();
                        /********************************************************************************************************************** */

                        run = (run + 1) % PLAYERS_NUM;

                    } while ((run % PLAYERS_NUM) != Start_Raise); 
                    isRaising = false;
                    for (int i = 0; i < PLAYERS_NUM; i++)
                    {
                        gPlayers[i].raised = false;
                    }
                    
                    break;
                }

                // Update Screen
                Board.render(0, 0, FULL_SCREEN); // Render backgound board
                for (int i = 1; i < PLAYERS_NUM; i++)
                {
                    downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);       // render face-down card 
                    downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );  // render face-down card

                    coin.free();
                    coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
                    coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
                }

                gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
                gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
                coin.free();
                coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
                coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
                for (int i = 0; i < COMUNITY_CARDS; i++)
                {
                    downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
                }
                coin.free();
                coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
                coin.render(200, 55);
                D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

                for (int i = 0; i < face_up_card - 2; i++)
                {
                    gPlayers[0].hand[i + 2].render(390 + i * 150, 350, QUARTER_SCREEN);
                }

                SDL_RenderPresent(gRenderer);
                while (SDL_GetTicks() - timer < 200) {}
                timer = SDL_GetTicks();
                /******************************************************************************************************************** */

                mainRun = (mainRun + 1) % PLAYERS_NUM;
            }

            for (int i = 0; i < PLAYERS_NUM; i++)
            {
                gPlayers[i].raised = false;
            }
            
            face_up_card++;
        }

        // Update screen
        Board.render(0, 0, FULL_SCREEN); // Render backgound board
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            downCard.render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);       // render face-down card 
            downCard.render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );  // render face-down card

            coin.free();
            coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " (" + gPlayers[i].TextOpt + ")", white);
            coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
        }

        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
        coin.free();
        coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " (" + gPlayers[0].TextOpt + ")", white);
        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
        for (int i = 0; i < COMUNITY_CARDS; i++)
        {
            downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
        }
        coin.free();
        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
        coin.render(200, 55);
        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

        for (int i = 0; i < face_up_card - 2; i++)
        {
            gPlayers[0].hand[i + 2].render(390 + i * 150, 350, QUARTER_SCREEN);
        }

        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 200) {}
        timer = SDL_GetTicks();
        /**************************************************************************** */

        // River Stage
        mainRun = (k + 1) % PLAYERS_NUM;
        isRaising = false;
        for (int z = 0; z < PLAYERS_NUM; z++)
        {
            if (mainRun == 0 && !(gPlayers[0].Fold) )
            {
                bool isPlaying = true;
                while (isPlaying)
                {
                    SDL_Event EV;
                    while (SDL_PollEvent(&EV))
                    {
                        if (EV.type == SDL_QUIT)
                        {
                            isPlaying = false;
                            return false;
                        }
                        
                        fold.BUTTON_HandleEvent(&EV);
                        check.BUTTON_HandleEvent(&EV);
                        raise.BUTTON_HandleEvent(&EV);
                    }
                    fold.BUTTON_Render(450, 750);
                    check.BUTTON_Render(660, 750);
                    raise.BUTTON_Render(900, 750);
                    SDL_RenderPresent(gRenderer);

                    if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || check.BUTTON_GetState() == BUTTON_PRESS )
                    {
                        isPlaying = false;
                    }
                }

                if (raise.BUTTON_GetState() == BUTTON_PRESS)
                {
                    isRaising = true;
                    gPlayers[0].raised = true;
                    gPlayers[0].TextOpt = "Raise +$20";
                    Start_Raise = 0;
                    run = 1;
                    val_raise += 20;        // NOTE !!! : Update later
                    TotalCoin += val_raise;
                    gPlayers[0].coin -= val_raise;
                }
                else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                {
                    gPlayers[0].TextOpt = "Fold";
                    gPlayers[0].Fold = true;
                }
                else if (check.BUTTON_GetState() == BUTTON_PRESS)
                {
                    gPlayers[0].TextOpt = "Check";
                }

                raise.BUTTON_ResetState();
                fold.BUTTON_ResetState();
                call.BUTTON_ResetState();
                check.BUTTON_ResetState();
            }
            else
            {
                if ( !(gPlayers[mainRun].Fold) )
                {
                    if (gPlayers[mainRun].handRanking(face_up_card) > ONE_PAIR && !gPlayers[mainRun].raised )
                    {
                        isRaising = true;
                        gPlayers[mainRun].raised = true;
            
                        if (gPlayers[mainRun].handRanking(face_up_card) > FOUR_OF_A_KIND)
                        {
                            val_raise += 70;
                            gPlayers[mainRun].TextOpt = "Raise +$70";
                        }
                        else if (gPlayers[mainRun].handRanking(face_up_card) > STRAIGHT)
                        {
                            val_raise += 50;
                            gPlayers[mainRun].TextOpt = "Raise +$50";
                        }
                        else if (gPlayers[mainRun].handRanking(face_up_card) > ONE_PAIR)
                        {
                            val_raise += 30;
                            gPlayers[mainRun].TextOpt = "Raise +$30";
                        }
                        TotalCoin += val_raise;
                        gPlayers[mainRun].coin -= val_raise;
                        run = (mainRun + 1) % PLAYERS_NUM;
                        Start_Raise = mainRun % PLAYERS_NUM;
                    }
                    else if (face_up_card == 6 && gPlayers[mainRun].handRanking(face_up_card) <= ONE_PAIR)
                    {
                        gPlayers[mainRun].Fold = true;
                        gPlayers[mainRun].TextOpt = "Fold";
                    }
                    else
                    {
                        gPlayers[mainRun].TextOpt = "Check";
                    }
                    
                }
                
            }
            
            if (isRaising == true)
            {
                do 
                {
                    if(run % PLAYERS_NUM == 0 && !(gPlayers[0].Fold) )
                    {
                        bool isPlaying = true;
                        while (isPlaying)
                        {
                            SDL_Event EV;
                            while (SDL_PollEvent(&EV))
                            {
                                if (EV.type == SDL_QUIT)
                                {
                                    isPlaying = false;
                                    return false;
                                }
                                
                                raise.BUTTON_HandleEvent(&EV);
                                call.BUTTON_HandleEvent(&EV);
                                fold.BUTTON_HandleEvent(&EV);
                            }
                            fold.BUTTON_Render(450, 750);
                            call.BUTTON_Render(660, 750);
                            raise.BUTTON_Render(900, 750);
                            SDL_RenderPresent(gRenderer);

                            if ( raise.BUTTON_GetState() == BUTTON_PRESS || fold.BUTTON_GetState() == BUTTON_PRESS || call.BUTTON_GetState() == BUTTON_PRESS )
                            {
                                isPlaying = false;
                            }
                        }

                        if (raise.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            isRaising = true;
                            gPlayers[0].raised = true;
                            gPlayers[0].TextOpt = "Raise +$20";
                            val_raise += 20;            // NOTE !!! : Update later
                            gPlayers[0].coin -= val_raise;
                            TotalCoin += val_raise;
                            Start_Raise = 0;
                        }
                        else if (fold.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            gPlayers[0].TextOpt = "Fold"; 
                            gPlayers[0].Fold = true;
                        }
                        else if (call.BUTTON_GetState() == BUTTON_PRESS)
                        {
                            gPlayers[0].TextOpt = "Call";
                            gPlayers[0].coin -= val_raise;
                            TotalCoin += val_raise;
                        }

                        raise.BUTTON_ResetState();
                        fold.BUTTON_ResetState();
                        call.BUTTON_ResetState();
                    }
                    else
                    {
                        if ( !(gPlayers[run].Fold) )
                        {
                            if (gPlayers[run].handRanking(face_up_card) > ONE_PAIR && !gPlayers[run].raised)
                            {
                                isRaising = true;
                                gPlayers[run].raised = true;
                                if (gPlayers[run].handRanking(face_up_card) > FOUR_OF_A_KIND)
                                {
                                    val_raise += 70;
                                    gPlayers[run].TextOpt = "Raise +$70";
                                }
                                else if (gPlayers[run].handRanking(face_up_card) > STRAIGHT)
                                {
                                    val_raise += 50;
                                    gPlayers[run].TextOpt = "Raise +$50";
                                }
                                else if (gPlayers[run].handRanking(face_up_card) > TWO_PAIR)
                                {
                                    val_raise += 30;
                                    gPlayers[run].TextOpt = "Raise +$30";
                                }
                                
                                gPlayers[run].coin -= val_raise;
                                TotalCoin += val_raise;
                                Start_Raise = run;
                            }
                            else if (face_up_card == 7 && gPlayers[run].handRanking(face_up_card) <= ONE_PAIR)
                            {
                                gPlayers[run].Fold = true;
                                gPlayers[run].TextOpt = "Fold";
                            }
                            else
                            {
                                gPlayers[run].TextOpt = "Call";
                                gPlayers[run].coin -= val_raise;
                                TotalCoin += val_raise;
                            }
                            
                        }
                        

                    }
                    run = (run + 1) % PLAYERS_NUM;
                } while ((run % PLAYERS_NUM) != Start_Raise);

                isRaising = false;
                for (int i = 0; i < PLAYERS_NUM; i++)
                {
                    gPlayers[i].raised = false;
                }
                
                break;
            }
            mainRun = (mainRun + 1) % PLAYERS_NUM;
        }

        /******************************************************************************** */   

        // find the highest hand 
        int winner = 0;
        while (gPlayers[winner].Fold)
        {
            winner++;
        }
        
        kindHand highestHand = gPlayers[winner].handRanking(face_up_card);
        int highestRank = gPlayers[winner].PLAYER_HighestCard;
        suit highestSuit = gPlayers[winner].PLAYER_HighestSuit;
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            if ( !gPlayers[i].Fold && i != winner )
            {
                kindHand tempHand = gPlayers[i].handRanking(face_up_card);
                int tempRank = gPlayers[i].PLAYER_HighestCard;
                suit tempSuit = gPlayers[i].PLAYER_HighestSuit;
                if (tempHand > highestHand)
                {
                    winner = i;
                    highestHand = tempHand;
                }
                else if (tempHand == highestHand)
                {
                    if (tempRank > highestRank)
                    {
                        winner = i;
                    }
                    else if (tempRank == highestRank)
                    {
                        if (tempSuit > highestSuit)
                        {
                            winner = i;
                        }
                        
                    }
                    
                }

            }
            
        }

        std::string strKind = "";
        switch (highestHand)
        {
        case STRAIGHT_FLUSH:
            strKind = "Straight Flush";
            break;
        case FOUR_OF_A_KIND:
            strKind = "Four of a kind";
            break;
        case FULL_HOUSE:
            strKind = "Full house";
            break;
        case FLUSH:
            strKind = "Flush";
            break;
        case STRAIGHT:
            strKind = "Straight";
            break;
        case THREE_OF_A_KIND:
            strKind = "Three of a kind";
            break;
        case TWO_PAIR:
            strKind = "Two Pair";
            break;;
        case ONE_PAIR:
            strKind = "One Pair";
            break;
        case HIGH_CARD:
            strKind = "High card";
            break;
        default:
            break;
        }
        SDL_Color black = {0, 0, 0};

        // Update user information
        // Open files to handle
        std::fstream RW_winrateFile("UserData/winrate.txt");
        std::fstream W_coinFile("UserData/coin.txt");
        if ( !(RW_winrateFile.is_open()) || !(W_coinFile.is_open()) )
        {
            getErr << "Can not open file to handle information at the end of the game" << '\n';
        }

        //Thái Hòa Test
        int win[100]={0}; //create a win array to store the number of wins for each player
        int lose[100]={0};//create a lose array to store the number of loses for each player
        int money[100];
        int i = 0;
        /*The winrate.txt file is written in rows for each player 
        and stores two parameters win and lose separated by a space.*/
        while ( !RW_winrateFile.eof() )//use while to check when the txt file is finished reading
        {
            RW_winrateFile >> win[i]; 
            RW_winrateFile >> lose[i];
            W_coinFile >> money[i];//Also read player's amounts
            i++;
        }
        i--;

        // End read file
        RW_winrateFile.close();
        W_coinFile.close();
        //update winrate.txt with new data of player's amounts
        RW_winrateFile.open("UserData/winrate.txt", std::fstream::out);
        W_coinFile.open("UserData/coin.txt", std::fstream::out);

        for (int j = 0; j < pos - 1; j++)
        {
            RW_winrateFile << win[j] << " " << lose[j] << '\n';
            W_coinFile << money[j] << '\n';

        }
        // Update the winner money
        gPlayers[winner].coin += TotalCoin;
        W_coinFile << gPlayers[winner].coin << '\n';
        if (winner == 0)
        {
            win[pos - 1]++;
        }
        else
        {
            lose[pos - 1]++;
        }
        
        for (int j = pos - 1; j < i; j++)
        {
            RW_winrateFile << win[j] << " " << lose[j] << '\n';
        }

        for (int j = pos; j < i; j++)
        {
            W_coinFile << money[j] << '\n';
        }
        

        // Close file
        RW_winrateFile.close();
        W_coinFile.close();
        
        // Update Screen
        /***************************************************************************************************** */
        Board.render(0, 0, FULL_SCREEN);
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            // this below area is a little bit different from others above
            // downcard --> gPlayers[i]
            gPlayers[i].hand[0].render(gPlayers[i].x, gPlayers[i].y, QUARTER_SCREEN, nullptr, -15.0);
            gPlayers[i].hand[1].render(gPlayers[i].x + 20, gPlayers[i].y, QUARTER_SCREEN, nullptr, 15.0 );

            // this is not changed
            coin.free();
            if (winner == i)
            {
                coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin) + " ! WINNER !", white);
            }
            else
            {
                coin.loadFromText("P" + std::to_string(i) + " $" + std::to_string(gPlayers[i].coin), white);
                
            }
            
            coin.render(gPlayers[i].x + 40, gPlayers[i].y + 200);
        }

        gPlayers[0].hand[0].render(gPlayers[0].x, gPlayers[0].y, QUARTER_SCREEN, nullptr, -15.0 );
        gPlayers[0].hand[1].render(gPlayers[0].x + 20, gPlayers[0].y, QUARTER_SCREEN, nullptr, 15.0 );
        coin.free();
        if (winner == 0)
        {
            coin.loadFromText("me $" + std::to_string(gPlayers[0].coin) + " !! WINNER !", white);
        }
        else
        {
            coin.loadFromText("me $" + std::to_string(gPlayers[0].coin), white);
            
        }
        coin.render(gPlayers[0].x + 180, gPlayers[0].y + 100);
        // for (int i = 0; i < COMUNITY_CARDS; i++)
        // {
        //     downCard.render(350 + 150 * i, 350, QUARTER_SCREEN);
        // }

        // Print Total bet of the pot 
        coin.free();
        coin.loadFromText("Total Bet: $" + std::to_string(TotalCoin), white);
        coin.render(200, 55);
        D.render(gPlayers[k].x - 765, gPlayers[k].y - 460);

        for (int i = 0; i < face_up_card - 2; i++)
        {
            gPlayers[0].hand[i + 2].render(390 + i * 150, 350, QUARTER_SCREEN);
        }

        coin.free();
        coin.loadFromText(strKind, black);
        coin.render(700, 450);

        SDL_RenderPresent(gRenderer);
        while (SDL_GetTicks() - timer < 300) {}
        timer = SDL_GetTicks();
        /******************************************************************************************************************* */

        coin.free();
        coin.loadFromText("Press any key to start", white);
        bool askingAgain = true;
        while (askingAgain)
        {
            SDL_Event getEvent;
            while (SDL_PollEvent(&getEvent))
            {
                if (getEvent.type == SDL_QUIT)
                {
                    BIG_RUNNING = false;
                    askingAgain = false;
                }
                homeButton.BUTTON_HandleEvent(&getEvent);
                if (getEvent.type == SDL_KEYDOWN)
                {
                    switch (getEvent.key.keysym.sym)
                    {
                    default:
                        askingAgain = false;
                        break;
                    }
                }
                if (homeButton.BUTTON_GetState() == BUTTON_PRESS)
                {
                    BIG_RUNNING = false;
                    askingAgain = false;
                }
            }
            homeButton.BUTTON_Render(1300, 30);
            coin.render(650, 800);
            SDL_RenderPresent(gRenderer);
        }

        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            gPlayers[i].Fold = false;
        }
        
        gPlayers[k].D = false;

        // Play again ?

        k = (k + 1) % PLAYERS_NUM;
    }
    if (homeButton.BUTTON_GetState() == BUTTON_PRESS)
    {
        getErr.close();
        return true;
    }
    getErr.close();
    return false;
}


bool PokerRush(card CARDS[])
{
    std::fstream getErr("Errors.txt");
    Graphic downcard;
    Graphic Board;
    Graphic text;
    SDL_Color white = {0xFF, 0xFF, 0xFF};
    button homeButton;
    player rPlayers[PLAYERS_NUM];

    if ( !(downcard.LoadMedia("image/face-down-card.png")) )
    {
        getErr << "Load down-card image" << '\n';
        getErr.close();
        return false;
    }
    else
    {
        if ( !(Board.LoadMedia("image/green-board.jpg")))
        {
            getErr << "Load board" << '\n';
            getErr.close();
            return false;
        }
        else
        {
            if (!(homeButton.BUTTON_Load("image/homeButton-out.png", "image/homeButton-over.png", "image/homeButton-down.png")))
            {
                getErr << "Load button" << '\n';
                getErr.close();
                return false;
            }
            
        }
        
    }

    // Position process
    int centerX = 1600 / 2;
    int centerY = 900 / 2;
    int R = 200;
    rPlayers[0].x = 1300 / 2;
    rPlayers[0].y = 900 / 2 + R;
    for (int i = 1; i < PLAYERS_NUM; i++)
    {
        rPlayers[i].x = centerX - R * sin((2 * M_PI / PLAYERS_NUM) * i);
        rPlayers[i].y = centerY + R * cos((2 * M_PI / PLAYERS_NUM) * i) - 120;

        if (rPlayers[i].x < centerX - 10)
        {
            rPlayers[i].x -= 500;
        }
        else if (rPlayers[i].x > centerX + 10)
        {
            rPlayers[i].x += 150;
        }
        else
        {
            rPlayers[i].x -= 80;
        }

        if (rPlayers[i].y > 330)
        {
            rPlayers[i].y += 60;
        }
        
    }
    /*********************************************************************** */

    bool BIG_RUNNING = true;
    while (BIG_RUNNING)
    {
        int checked_cards_pokRush[PLAYERS_NUM][POKER_RUSH_NO_CARDS];
        bool dealedCard[TOTAL_CARDS] = {false};

        // Make sure all dealedCard elements are assigned by false value
        for (int i = 0; i < TOTAL_CARDS; i++)
        {
            dealedCard[i] = false;
        }

        int i = 0;
        int countPlayers = 0;
        int cardNum1Hand = 0;
        while (i < PLAYERS_NUM * POKER_RUSH_NO_CARDS)
        {
            for ( countPlayers = 0; countPlayers < PLAYERS_NUM; countPlayers++)
            {
                cardNum1Hand = 0;
                while (cardNum1Hand < POKER_RUSH_NO_CARDS)
                {
                    int temp_num = rand() % TOTAL_CARDS;
                    if (!dealedCard[temp_num])
                    {
                        dealedCard[temp_num] = true;
                        checked_cards_pokRush[countPlayers][cardNum1Hand] = temp_num;
                        i++;
                        cardNum1Hand++;

                    }
                    
                }
                
            }
            
        }

        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            for (int j = 0; j < POKER_RUSH_NO_CARDS; j++)
            {
                rPlayers[i].hand[j] = CARDS[checked_cards_pokRush[i][j]];
            }
        }

        Board.render(0, 0, FULL_SCREEN);
        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            for (int j = 0; j < POKER_RUSH_NO_CARDS; j++)
            {
                downcard.render(rPlayers[i].x + j * 40, rPlayers[i].y, QUARTER_SCREEN);
                SDL_Event EV;
                uint32_t timer = SDL_GetTicks();
                while (SDL_GetTicks() - timer < 200 && SDL_PollEvent(&EV)) {}
                timer = SDL_GetTicks();
                SDL_RenderPresent(gRenderer);
                
            }
            
        }

        kindHand kind[PLAYERS_NUM];
        int highest_rank[PLAYERS_NUM];
        suit highest_suit[PLAYERS_NUM];

        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            kind[i] = rPlayers[i].handRanking(POKER_RUSH_NO_CARDS);
            highest_rank[i] = rPlayers[i].PLAYER_HighestCard;
            highest_suit[i] = rPlayers[i].PLAYER_HighestSuit;
        }

        int winner = 0;
        for (int i = 1; i < PLAYERS_NUM; i++)
        {
            if (kind[i] > kind[winner])
            {
                winner = i;
            }
            else if (kind[i] == kind[winner])
            {
                if (highest_rank[i] > highest_rank[winner])
                {
                    winner = i;
                }
                else if (highest_rank[i] == highest_rank[winner])
                {
                    if (highest_suit[i] > highest_suit[winner])
                    {
                        winner = i;
                    }
                    
                }
                
            }
            
        }
        
        text.free();
        text.loadFromText("Press any key to start", white);
        bool isAsking = true;
        bool start = false;
        while (!start)
        {
            SDL_Event EV;
            while (SDL_PollEvent(&EV))
            {
                if (EV.type == SDL_QUIT)
                {
                    BIG_RUNNING = false;
                    isAsking = false;
                    start = true;
                }
                else if (EV.type == SDL_MOUSEBUTTONDOWN || EV.type == SDL_KEYDOWN)
                {
                    start = true;
                }
                
            }
            text.render(620, 500);
            SDL_RenderPresent(gRenderer);
        }
        

        Board.render(0, 0, FULL_SCREEN);
        for (int i = 0; i < PLAYERS_NUM; i++)
        {
            for (int j = 0; j < POKER_RUSH_NO_CARDS; j++)
            {
                rPlayers[i].hand[j].render(rPlayers[i].x + j * 40, rPlayers[i].y, QUARTER_SCREEN);
                SDL_Event EV;
                uint32_t timer = SDL_GetTicks();
                while (SDL_GetTicks() - timer < 200 ) {SDL_PollEvent(&EV);}
                timer = SDL_GetTicks();
                SDL_RenderPresent(gRenderer);
                
            }
        }
        std::string strKind = "";
        text.free();
        switch (kind[winner])
        {
        case STRAIGHT_FLUSH:
            strKind = "Straight Flush";
            break;
        case FOUR_OF_A_KIND:
            strKind = "Four of a kind";
            break;
        case FULL_HOUSE:
            strKind = "Full house";
            break;
        case FLUSH:
            strKind = "Flush";
            break;
        case STRAIGHT:
            strKind = "Straight";
            break;
        case THREE_OF_A_KIND:
            strKind = "Three of a kind";
            break;
        case TWO_PAIR:
            strKind = "Two Pair";
            break;;
        case ONE_PAIR:
            strKind = "One Pair";
            break;
        case HIGH_CARD:
            strKind = "High card";
            break;
        default:
            break;
        }
        
        text.loadFromText(strKind + " Winner", white);
        text.render(rPlayers[winner].x, rPlayers[winner].y + 200);

        text.free();
        text.loadFromText("Press any key to start again", white);
        while (isAsking)
        {
            SDL_Event EV;
            while (SDL_PollEvent(&EV))
            {
                if (EV.type == SDL_QUIT)
                {
                    BIG_RUNNING = false;
                    isAsking = false;
                }
                homeButton.BUTTON_HandleEvent(&EV);
                if (EV.type == SDL_KEYDOWN)
                {
                    switch (EV.key.keysym.sym)
                    {
                    default:
                        isAsking = false;
                        break;
                    }
                }
                if (homeButton.BUTTON_GetState() == BUTTON_PRESS)
                {
                    BIG_RUNNING = false;
                    isAsking = false;
                }
            }
            homeButton.BUTTON_Render(690, 300);
            text.render(620, 500);
            SDL_RenderPresent(gRenderer);
        }
        
    }
    
    if (homeButton.BUTTON_GetState() == BUTTON_PRESS)
    {
        return true;
    }

    return false;
}