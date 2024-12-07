#ifndef __DeckData_H__
#define __DeckData_H__ 
#include <iostream>
#include "usefulFuncs.h"

enum suit
{
    SPADES,
    CLUBS,
    DIAMONDS,
    HEARTS
};

enum kindHand
{
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
};

// group all propertises of all cards at one place
class card: public Graphic
{
    public:
        void CARD_SetValue(int rank, suit Suit, SDL_Texture* pTexture);
        int CARD_GetRank();
        suit CARD_GetSuit();
        SDL_Texture* CARD_GetTexture();
    private:
        int rankCard;
        suit SuitCard;
        SDL_Texture* gTextureCard;
};

struct player
{
    player();
    card hand[COMUNITY_CARDS + CARDS_NUM_ONE_HAND];
    bool D;
    bool Fold = false;
    bool raised = false;
    int coin;
    int x;
    int y;
    std::string TextOpt;
    kindHand handRanking(int n);
    int PLAYER_HighestCard;
    suit PLAYER_HighestSuit;
};

#endif