// Clifford Rice
// November 15, 2012
// The UNO Project

// Import necessary libraries.
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <windows.h>

// Define necessary constants
#define NUM_PLAYERS 4
#define CARDS_IN_HAND 7
#define DECK_SIZE 108
#define MAX_ATT_SIZE 10
#define SEC_TO_RESPOND 2
#define INTEGER_START 48
#define MAX_SHUFFLES 101
#define MIN_SHUFFLES 33
#define NUM_COLORS 4
#define SAFE_START 200
#define TURN_DELAY 1500

// Create three structs to be used in the program
typedef struct {
    int num;
    char color[MAX_ATT_SIZE];
    char qualifier;
} Card;

typedef struct {
    char name[MAX_ATT_SIZE];
    char type[MAX_ATT_SIZE];
    Card hand[DECK_SIZE];
    int handSize;
} Player;

typedef struct {
    Card deck[DECK_SIZE];
    Card discard[DECK_SIZE];
    int deckTop;
    int discTop;
} Piles;

typedef struct {
    Piles pile;
    Player players[NUM_PLAYERS];
    int turnBuffer; // Used to prevent errors due to "reversed" gameplay and negative numbers
    int whoseTurn;
    int turnInc;
} Game;

// Declare functions for compiler
void startGame(Game* g);
void loadDeck(FILE *ifp, Card deck[]);
void loadPlayers(Player players[]);
Player makeUser(int num);
Player makeComp(int num);
void shuffleDeck(Card deck[], int top);
void shuffleOnce(Card deck[], int top);
void deal1(Game* g);
void deal2(Game* g);
char userTurn(Game* g);
char compTurn(Game* g);
void printState(Game* g);
void printHand(Player* p);
void printSpecial(Card* c);
void printCard(Card* c);
int checkMatches(Player * plp, Piles* pileptr);
Card draw(Player* plp, Piles* pptr);
int discard(Player* plp, Piles* pileptr, int choice);
int compare(Card* c1, Card* c2);
Card makeCard(int number, char col[MAX_ATT_SIZE], char qual);
void specialEffect(Game* g);
void wild(Game* g);
char* toColor(int num);
void nextPlayer(Game* g);
void nextDraw2(Game* g);
void nextDraw4(Game* g);
int checkQual(Player* p, char q);
int drawTilMatch(Player* p, Piles* pile);
void resetDeck(Piles* pile);
void printWelcome();
void printBye();
