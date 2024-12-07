// including this library is to help team keep tracking on bugs occuring while developing phase
#include <fstream>
#include <ctime>
#include "include/SDL.h"
#include "include/SDL_image.h"

// this library contains all helpful functions which may be use mostly by team
#include "usefulFuncs.h"
// create struct card with 3 valuable rank, suit and textture (address of card's image )
void card::CARD_SetValue(int rank, suit Suit, SDL_Texture* pTexture)
{
    rankCard = rank;
    SuitCard = Suit;
    gTextureCard = pTexture;
}

// Initial deck
bool InitDeck(card* CARDS)
{
    SDL_Surface* tSurface = nullptr;

    // make sure, here we initial IMG PNG flag one more time
    int flag = IMG_INIT_PNG;

    // Keep bugs track
    std::fstream getErr("Errors.txt");

    // Get path direction by a text file
    std::ofstream W_Path;
    std::ifstream R_Path;
    //write to file path.txt the address of the card image
    for (int i = 0; i < TOTAL_CARDS; i++)//find address of rank
    {
        W_Path.open("path.txt");
        W_Path << "image/PNG-cards/";
        switch (i % 13)
        {
        case 9:
            W_Path << "jack_of_";
            break;
        case 10:
            W_Path << "queen_of_";
            break;
        case 11:
            W_Path << "king_of_";
            break;
        case 12:
            W_Path << "ace_of_";
            break;
        default:
            W_Path << (i % 13) + 2 << "_of_";
            break;
        }

        suit tempSuit;
        if (i < 13)//find address of suit
        {
            tempSuit = SPADES;
            W_Path << "spades.png";
        }
        else if (i < 26)
        {
            tempSuit = CLUBS;
            W_Path << "clubs.png";
        }
        else if (i < 39)
        {
            tempSuit = DIAMONDS;
            W_Path << "diamonds.png";
        }
        else
        {
            tempSuit = HEARTS;
            W_Path << "hearts.png";
        }

        // Stop writing file
        W_Path.close();

        // Start reading that file
        R_Path.open("path.txt");

        // copy card image address from that path
        std::string TempPath;
        getline(R_Path, TempPath);
        R_Path.close();

        if ( !(CARDS[i].LoadMedia(TempPath)) )
        {
            getErr << "Error: LoadMedia function at the end " << SDL_GetError();
            getErr.close();
            return false;
        }
        
        SDL_Texture* tempTexture = nullptr;
        tempTexture = CARDS[i].getTexture();
        if (tempTexture == nullptr)
        {
            getErr << "tempTexture at the end phase of InitDeck function" << SDL_GetError();
            getErr.close();
            return false;
        }
        CARDS[i].CARD_SetValue( (i % 13) + 2, tempSuit, tempTexture);
    }
    getErr.close();
    return true;
}

player::player()
{
    D = false;
    Fold = false;
    raised = false;
}

void ShuffleDeck()
{
    srand(time(NULL));

    bool dealedCard[TOTAL_CARDS] = {false};

    // Make sure all dealedCard elements are assigned by false value
    for (int i = 0; i < TOTAL_CARDS; i++)
    {
        dealedCard[i] = false;
    }

    int i = 0;
    int countPlayers = 0;
    int cardNum1Hand = 0;
    while (i < PLAYERS_NUM*CARDS_NUM_ONE_HAND)
    {
        for ( countPlayers = 0; countPlayers < PLAYERS_NUM; countPlayers++)
        {
            cardNum1Hand = 0;
            while (cardNum1Hand < CARDS_NUM_ONE_HAND)
            {
                int temp_num = rand() % TOTAL_CARDS;
                if (!dealedCard[temp_num])
                {
                    dealedCard[temp_num] = true;
                    CheckedCards[countPlayers][cardNum1Hand] = temp_num;
                    i++;
                    cardNum1Hand++;

                }
                
            }
            
        }
        
    }
    i = 0;
    while (i < COMUNITY_CARDS)
    {
        int temp_num = rand() % TOTAL_CARDS;
        if ( !dealedCard[temp_num] )
        {
            dealedCard[temp_num] = true;
            CommunityCards[i] = temp_num;
            i++;
        }
        
    }
        
    
    
    
}

// Function to sort card array in inscending order.
void sort_card (int player_card[], suit element_card[], int n){
    // sort the array elements based on the rank of the cards and the suit of the cards.
    int temp;
    suit stemp;
    // sort the elements in descending order
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if ((player_card[i] > player_card[j]) || ((player_card[i] == player_card[j]) && (element_card[i] > element_card[j])))
            {
                //swap player_card
                temp=player_card[i];
                player_card[i]=player_card[j];
                player_card[j]=temp;
                //swap element_card
                stemp=element_card[i];
                element_card[i]=element_card[j];
                element_card[j]=stemp;
            }
        }
    }
}
// Function to check if there is a Straight flush in the player's hand.
bool check_Straight_flush(int player_card[], suit element_card[], int &max_card, suit &max_element, int n){
    // copy player_card and element_card to dif_card and dif_ele
    const int num=n;
    int dif_card[num];
    suit dif_ele[num];
    for (int i = 0; i < n; i++)
    {
        dif_card[i] = player_card[i];
        dif_ele[i] = element_card[i];
    }
    // sort the elements in inscending order
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (dif_ele[i] > dif_ele[j])
            {
                //swap player_card
                std::swap(dif_card[i],dif_card[j]);
                //swap element_card
                std::swap(dif_ele[i],dif_ele[j]);
            }
        }
    }
    //check Straight flush
    bool Straight_flush = false;
    for (int i = n - 1; i > n - 4; i--)
    {
        Straight_flush = true;
        for (int j = 1; j < 5; j++)
        {
            if ((dif_ele[i - j] != dif_ele[i])||(dif_card[i] - dif_card[i - j] != j))
            {
                Straight_flush = false;
            }
        }
        if (Straight_flush){
            max_card = dif_card[i + 4];
            max_element = dif_ele[i + 4];
            return true;
        }
    }
    return false;
}
// Function to check if there is a Flush in the player's hand.
bool check_flush(int player_card[], suit element_card[], int &max_card, suit &max_element, int n){
    // copy player_card and element_card to dif_card and dif_ele
    const int num = n;
    int dif_card[num];
    suit dif_ele[num];
    for (int i = 0; i < n; i++)
    {
        dif_card[i] = player_card[i];
        dif_ele[i] = element_card[i];
    }
    //sort the array elements based on the suit of the cards.
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (dif_ele[i] > dif_ele[j])
            {
                //swap player_card
                std::swap(dif_card[i],dif_card[j]);
                //swap element_card
                std::swap(dif_ele[i],dif_ele[j]);
            }
        }
    }
    //check flush
    bool flush=false;
    for (int i = n - 1; i > n - 4; i--)
    {
        flush=true;
        for (int j = 1; j < 5; j++)
        {
            if (dif_ele[i-j] != dif_ele[i])
            {
                flush=false;
            }
        }
        if (flush){
            max_card = dif_card[i+4];
            max_element = dif_ele[i+4];
            return true;
        }
    }
    return false;
}
// Function to check if there is a Straight in the player's hand.
bool check_Straight(int player_card[], suit element_card[], int &max_card, suit &max_element, int n){
    //find different cards(no repeat)
    const int num=n;
    int dif_card[num];
    suit dif_ele[num];
    int dem = 1;
    dif_card[0] = player_card[0];
    dif_ele[0] = element_card[0];
    for  (int i = 1; i < n; i++){
        if (player_card[i] != player_card[i - 1]){
            dif_card[dem] = player_card[i];
            dif_ele[dem] = element_card[i];
            dem++;
        }
    }
    //if dem<5 make sure is'not Straight because number of cards that cannot form a straight
    if (dem < 5){return false;}
    //check Straight
    bool straight = false;
    for (int i = dem - 1; i > dem - 4; i--){
        straight = true;
        for (int j = 1; j < 5; j++)
        {
            if (dif_card[i - j] != dif_card[i] - j)
            {
                straight = false;
            }
        }
        if (straight){
            max_card = dif_card[i];
            max_element = dif_ele[i];
            return true;
            }
    }
    //The other case {1,2,3,4,5}
    if (dif_card[0] == 2 && dif_card[1] == 3 && dif_card[2] == 4 && dif_card[3] == 5 && dif_card[dem-1] == 14 )
    {
       max_card = dif_card[dem - 1];
       max_element = dif_ele[dem-1];
       return true;
    }
    return false;
}
// Function to check if there is a Four of a kind in the player's hand.
bool check_4kind(int player_card[], suit element_card[],int &max_card, suit &max_element,int n){
    bool fkind=false;
    for (int i = 0; i < n - 3; i++) 
    {
        fkind=true;
        for (int j = i + 1; j < i + 4; j++)
        {
            if (player_card[j] != player_card[i])
            {
                fkind = false;
            }
        }
        if (fkind){
            max_card = player_card[i + 3];
            max_element = element_card[i + 3];
            return true;
        }
    }
    return false;

}

// Function to check if there is a Full house in the player's hand.
bool check_fullhouse(int player_card[], suit element_card[],int &max_card, suit &max_element, int n){
    //count the number of repetitions of the card rank
    const int num = n;
    int dif_card[num];
    int count_repeat[num] = {0};
    suit dif_ele[num];
    int dem = 1;
    dif_card[0] = player_card[0];
    dif_ele[0] = element_card[0];
    count_repeat[0] = 1;
    for  (int i = 1; i < n; i++)
    {
        if (player_card[i] != player_card[i-1])
        {
            dem++;
            dif_card[dem-1] = player_card[i];
            dif_ele[dem-1] = element_card[i];
            count_repeat[dem-1] += 1;;
        }
        else {
            dif_ele[dem-1]=element_card[i];
            count_repeat[dem-1]+=1;
        }
    }
    //sort the array based on the count of repetitions of the card rank.
    for (int i = dem-1; i >=0 ; i--)
    {
        for (int j = i-1; j >=0; j--)
        {
            if (count_repeat[i]<count_repeat[j])
            {
                std::swap(dif_card[i],dif_card[j]);
                std::swap(dif_ele[i],dif_ele[j]);
                std::swap( count_repeat[i],count_repeat[j]);
            }

        }
    }
    //check Full house
    if (count_repeat[dem-1]!=3){return false;}
    else {
        if (count_repeat[dem-2]<2){
            return false;
        }
        else
        {
            max_card=dif_card[dem-1];
            max_element = dif_ele[dem-1];
            return true;
        }
    }

}

// Function to check if there is a Three of a kind in the player's hand.
bool check_3kind(int player_card[], suit element_card[],int &max_card, suit &max_element, int n){
    bool tkind=false;
    for (int i = 0; i < n-2; i++)
    {
        tkind=true;
        for (int j = i+1; j < i+3; j++)
        {
            if (player_card[j]!=player_card[i])
            {
                tkind=false;
            }                
        }
        if (tkind){
            max_card = player_card[i+2];
            max_element = element_card[i+2];
            return true;
        }
    }
    return false;

}

// Function to check if there is a Two Pairs in the player's hand.
bool check_2Pairs(int player_card[], suit element_card[],int &max_card, suit &max_element, int n){
    //count the number of repetitions of the card rank
    const int num = n;
    int dif_card[num];
    int count_repeat[num] = {0};
    suit dif_ele[num];
    int dem = 1;
    dif_card[0] = player_card[0];
    dif_ele[0] = element_card[0];
    count_repeat[0] = 1;
    for  (int i = 1; i < n; i++)
    {
        if (player_card[i] != player_card[i - 1])
        {
            dem++;
            dif_card[dem-1] = player_card[i];
            dif_ele[dem-1] = element_card[i];
            count_repeat[dem - 1]++;
        }
        else {
            dif_ele[dem-1] = element_card[i];
            count_repeat[dem-1]++;
        }
    }
    //sort the array based on the count of repetitions of the card rank.
    for (int i = dem - 1; i >= 0 ; i--){
        for (int j = i - 1; j >= 0; j--)
        {
            if (count_repeat[i] < count_repeat[j])
            {
                std::swap(dif_card[i], dif_card[j]);
                std::swap(dif_ele[i], dif_ele[j]);
                std::swap( count_repeat[i], count_repeat[j]);
            }

        }

    }
    //check Two Pairs
    if (count_repeat[dem - 1] != 2){return false;}
    else {
        if (count_repeat[dem - 2] != 2){
            return false;
        }
        else
        {
            max_card = dif_card[dem - 1];
            max_element= dif_ele[dem - 1];
            return true;
        }
    }
}

// Function to check if there is a Pair in the player's hand.
bool check_Pair(int player_card[], suit element_card[],int &max_card, suit &max_element, int n){
    //count the number of repetitions of the card rank
    const int num = n;
    int dif_card[num];
    int count_repeat[num] = {0};
    suit dif_ele[num];
    int dem = 1;
    dif_card[0] = player_card[0];
    dif_ele[0] = element_card[0];
    count_repeat[0] = 1;
    for  (int i = 1; i < n; i++)
    {
        if (player_card[i] != player_card[i-1])
        {
            dem++;
            dif_card[dem - 1] = player_card[i];
            dif_ele[dem - 1] = element_card[i];
            count_repeat[dem - 1]++;
        }
        else {
            dif_ele[dem - 1] = element_card[i];
            count_repeat[dem - 1]++;
        }
    }
    //sort the array based on the count of repetitions of the card rank.
    for (int i = dem - 1; i >= 0 ; i--)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            if (count_repeat[i] < count_repeat[j])
            {
                std::swap(dif_card[i], dif_card[j]);
                std::swap(dif_ele[i], dif_ele[j]);
                std::swap( count_repeat[i], count_repeat[j]);
            }

        }

    }
    //check Pair
    if (count_repeat[dem - 1] != 2){return false;}
    else
        {
            max_card = dif_card[dem - 1];
            max_element = dif_ele[dem - 1];
            return true;
        }
}


kindHand player::handRanking(int n)
{
    bool flag_flush = false;
    bool flag_straight = false;
    int* rank = new int[n];
    suit* Suit = new suit[n];
    for (int i = 0; i < n; i++)
    {
        rank[i] = hand[i].CARD_GetRank();
        Suit[i] = hand[i].CARD_GetSuit();
    }
    
    if (n == 2)
    {
        if (rank[0]==rank[1])
        {
            delete[] rank;
            delete[] Suit;
            return ONE_PAIR;
        }
    }
    else if (n > 4 && n < 8)
    {
        sort_card (rank, Suit, n);

        if ( check_Straight_flush(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n))
        {
            delete[] rank;
            delete[] Suit;
            return STRAIGHT_FLUSH;
        }
        else if ( check_4kind(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n) )
        {
            delete[] rank;
            delete[] Suit;
            return FOUR_OF_A_KIND;
        }
        else if ( check_fullhouse(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n) )
        {
            delete[] rank;
            delete[] Suit;
            return FULL_HOUSE;
        }
        else if ( flag_flush )
        {
            delete[] rank;
            delete[] Suit;
            return FLUSH;
        }
        else if ( flag_straight )
        {
            delete[] rank;
            delete[] Suit;
            return STRAIGHT;
        }
        else if ( check_3kind(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n) )
        {
            delete[] rank;
            delete[] Suit;
            return THREE_OF_A_KIND;
        }
        else if ( check_2Pairs(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n) )
        {
            delete[] rank;
            delete[] Suit;
            return TWO_PAIR;
        }
        else if ( check_Pair(rank, Suit, PLAYER_HighestCard, PLAYER_HighestSuit, n) )
        {
            delete[] rank;
            delete[] Suit;
            return ONE_PAIR;
        }
        
    }
    return HIGH_CARD;
}

// these are three getting value function definitions
int card::CARD_GetRank()
{
    return rankCard;
}

suit card::CARD_GetSuit()
{
    return SuitCard;
}

SDL_Texture* card::CARD_GetTexture()
{
    return gTextureCard;
}