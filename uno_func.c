// Clifford Rice
// November 15, 2012
// The UNO Project

// Import necessary libraries.
#include "uno.h"

int main(){
    //Create main loop escape variable
    int exit;

    //Print a welcome message
    printWelcome();

    // Begin first game; loop until the player is done playing
    while(1){
        // Create game, print a welcome message, seed the random number generator, initialize game values
        Game g;
        srand(time(0));
        startGame(&g);

        // Begin looping through user/computer turns
        while(1){
            // Create variable to break second loop
            char win;

            // If the next player to go is a computer, simulate their turn, otherwise allow a user-controlled turn
            if(strcmp(g.players[g.whoseTurn].type, "computer")==0){
                win = compTurn(&g);
            } else {
                win = userTurn(&g);
            }

            // Check if the last player won, end the game loop
            if(win == '!')
                break;

            // Move index to the next player in the game
            nextPlayer(&g);

    }

        // Ask if the user would like to play again
        printf("\n\nWould you like to:\n");
        printf("[1] Play Again\n");
        printf("[2] Quit\n");
        scanf("%d", &exit);

        // Convert user input into a boolean variable
        exit--;

        // Exit if chosen to do so
        if(exit)
            break;

        printf("\n\n");
    }
    //Say goodbye and thank user
    printBye();
    system("PAUSE");

    return 0;
}

void startGame(Game* g){
    // Create file pointer and open file
    FILE *ifp;
    ifp = fopen("stddeck.in", "r");

    //Load the deck from the file and player info from the user
    loadDeck(ifp, (g->pile).deck);
    loadPlayers(g->players);

    // Set standard beginning game variables
    g->pile.deckTop = DECK_SIZE - 1;
    g->pile.discTop = 0;
    shuffleDeck(g->pile.deck, DECK_SIZE);
    g->turnBuffer = SAFE_START + (rand() % NUM_PLAYERS);
    g->whoseTurn = g->turnBuffer % NUM_PLAYERS;
    g->turnInc = 1;

    //Pick one of two ways to deal for more game variability
    int deal = (rand() % 2) + 1;
    if(deal==1){
        deal1(g);
    } else{
        deal2(g);
    }


}

// Take in an empty array of cards and input file. Initialize the "deck"
void loadDeck(FILE *ifp, Card deck[]){
    int count, num;
    char color[MAX_ATT_SIZE];
    char qual;
    for(count = 0; count < DECK_SIZE; count++){
        fscanf(ifp, "%d", &num);
        fscanf(ifp, "%s ", color);
        fscanf(ifp, "%c", &qual);
        deck[count] = makeCard(num, color, qual);
    }
}

// Take in values for attributes and return a Card struct with those values
Card makeCard(int number, char col[MAX_ATT_SIZE], char qual){
    Card c;
    c.num = number;
    strcpy(c.color, col);
    c.qualifier = qual;
    return c;
}

// Get player info from user(s) including number of PEOPLE playing and names for those users
void loadPlayers(Player players[]){
    int numUsers, i;

    // Find out number of users
    printf("How many people will be playing? (Maximum of %d):\n", NUM_PLAYERS);
    scanf("%d", &numUsers);

    // Recursive call if invalid number of players is given.
    if ((numUsers > NUM_PLAYERS) || (numUsers < 0)){
        printf("Sorry, you entered an invalid number of players.\n");
        loadPlayers(players);
    } else {
        //Make appropriate "User" players
        for(i=0; i<numUsers; i++){
            players[i] = makeUser(i+1);
        }

        //Fill the rest of the "players" array with numbered computer players
        for(i=numUsers; i < NUM_PLAYERS; i++){
            players[i] = makeComp(i+1);
        }
    }
}

// Makes a "user" player with a user-prompted name
Player makeUser(int num){
    Player p;
    char name[MAX_ATT_SIZE];
    printf("Player %d, what is your first name?\n", num);
    scanf("%s", p.name);
    strcpy(p.type, "user");
    p.handSize = 0;
    return p;
}

// Makes a computer player with standard variables
Player makeComp(int num){
    Player p;
    num += INTEGER_START;
    char name[MAX_ATT_SIZE] = "COM ";
    name[strlen(name)] = num;
    strcpy(p.name, name);
    strcpy(p.type, "computer");
    p.handSize = 0;
    return p;
}

//Takes in an array and the top value of that array, shuffles it a random number of time
void shuffleDeck(Card deck[], int top){
    // Create Loop Index
    int i;

    // Randomly determine number of deck shuffles
    int numShuffles = rand() % (MAX_SHUFFLES - MIN_SHUFFLES) + MIN_SHUFFLES;

    // Shuffle the designated number of times
    for(i=0; i < numShuffles; i++){
        shuffleOnce(deck, top);
    }
}

// Shuffles an array one time
void shuffleOnce(Card deck[], int top){
    // Create two arrays of cards equal in size to half of the deck
    Card stack1[top / 2];
    Card stack2[top / 2];

    // Create a loop index and temporary storage variables
    int i, j, num1, num2;
    char col1[MAX_ATT_SIZE], col2[MAX_ATT_SIZE];
    char qual1, qual2;

    // First loop makes stack1 equivalent to the first half of the deck
    for(i=0; i < top/2; i++){
        //Set temporary variables to those of the deck
        num1 = deck[i].num;
        strcpy(col1, deck[i].color);
        qual1 = deck[i].qualifier;

        //Use the variables to set each card equal to the corresponding card in "deck"
        stack1[i] = makeCard(num1, col1, qual1);
    }

    // Second loop sets stack2 equal to the second half of the deck
    for(i=top/2; i < top; i++){
        // Set attributes equal to those stored in the card of "deck"
        num2 = deck[i].num;
        strcpy(col2, deck[i].color);
        qual2 = deck[i].qualifier;

        // Use values to set each card equal to one in "deck
        stack2[i-(top/2)] = makeCard(num2, col2, qual2);
    }

    // Third loop places alternating cards from stack1 and stack2 into deck
    for(i=0,j=0; (i < top/2) && (j < top); i++, j+=2){
            // Set attributes for card from stack1
            num1 = stack1[i].num;
            strcpy(col1, stack1[i].color);
            qual1 = stack1[i].qualifier;

            //Set attributes for card from stack2
            num2 = stack2[i].num;
            strcpy(col2, stack2[i].color);
            qual2 = stack2[i].qualifier;

            // Add cards to deck
            deck[j] = makeCard(num1, col1, qual1);
            deck[j+1] = makeCard(num2, col2, qual2);
    }
}

// Deals in the standard "circular" gameflow method
void deal1(Game* g){
    int i;
    // Deals cards
    for(i=0; i < NUM_PLAYERS * CARDS_IN_HAND; i++){
            draw(&(g->players[g->whoseTurn]), &(g->pile));
            nextPlayer(g);
    }

    // Flips top card of deck to discard pile. Makes adjustments in index values
    g->pile.discard[0] = g->pile.deck[g->pile.deckTop];
    g->pile.deckTop -= 1;
    g->pile.discTop += 1;
}

// Deals a full hand to each user
void deal2(Game* g){
    int i,j;
    //Deals cards
    for(i=0; i < NUM_PLAYERS; i++){
        for(j=0; j<CARDS_IN_HAND; j++)
            draw(&(g->players[i]), &(g->pile));
    }

    //Flips card from deck to discard. Makes index adjustments.
    g->pile.discard[0] = g->pile.deck[g->pile.deckTop];
    g->pile.deckTop -= 1;
    g->pile.discTop += 1;
}

//Moves ne card from the top of the deck to indicated player's hand
Card draw(Player* plp,  Piles* pileptr){
    //Checks that cards are in deck, otherse calls the resetDeck function
    if(pileptr->deckTop < 0){
        resetDeck(pileptr);
    }

    // Copies deck card values
    int number = pileptr->deck[pileptr->deckTop].num;
    char col[MAX_ATT_SIZE];
    strcpy(col, pileptr->deck[pileptr->deckTop].color);
    char qual = pileptr->deck[pileptr->deckTop].qualifier;

    // Copies card values to top of hand
    plp->hand[plp->handSize] = makeCard(number, col, qual);

    // Makes adjustments in index variables
    pileptr->deckTop -= 1;
    plp->handSize += 1;

    //returns the card just drawn for testing
    return plp->hand[plp->handSize];
}

//Turn with user input
char userTurn(Game* g){
        //Sets up and displays state of game before player's turn
        printf("\n%s's Turn.\n",g->players[g->whoseTurn].name);
        Sleep(TURN_DELAY);
        printState(g);
        int choice, match=0;

        //checks if te user can discard anything
        int check = checkMatches(&(g->players[g->whoseTurn]), &(g->pile));
        Sleep(TURN_DELAY);

        //If the user CAN discard something, asks them which. Otherwise draws until the player can discard
        if(check){
            printf("Which card would you like to discard?\n");
            scanf("%d", &choice);
            discard(&(g->players[g->whoseTurn]), &(g->pile), choice);

            // Displays the card the user discarded
            printf("\n%s has played a ", (g->players[g->whoseTurn]).name);
            printCard(&(g->pile.discard[g->pile.discTop - 1]));
            printf(".\n");
        } else {
            match = drawTilMatch(&(g->players[g->whoseTurn]), &(g->pile));
            discard(&(g->players[g->whoseTurn]), &(g->pile), match);

            // Prints out card the user plays
            printf("%s has played a ", (g->players[g->whoseTurn]).name);
            printCard(&(g->pile.discard[g->pile.discTop - 1]));
            printf(".\n\n");

            // Prints the user's new hand
            printHand(&(g->players[g->whoseTurn]));
            Sleep(TURN_DELAY);
        }


        //Checks if player won and prints/returns that it does
        if (checkWin(&(g->players[g->whoseTurn]))){
            Sleep(TURN_DELAY);
            printf("\n%s has won!\n", (g->players[g->whoseTurn]).name);
            return '!';
        }

        // Checks if user has only one card. Prints that they have uno
        if (g->players[g->whoseTurn].handSize == 1){
            Sleep(TURN_DELAY);
            printf("\n%s says \"UNO!\"\n", g->players[g->whoseTurn].name);
        }

        // Performs a special effect of a card if necessary (i.e. reverse, skip, draw, wild)
        if ((g->pile.discard[g->pile.discTop-1].num == -1)&&(match!=-1)){
            specialEffect(g);
        }

        Sleep(TURN_DELAY);
        printf("\n\n\n\n");
        return g->pile.discard[g->pile.discTop-1].qualifier;
}

//Turn without user input
char compTurn(Game* g){
    // Lets user know who is going
    printf("\n%s's Turn.\n",g->players[g->whoseTurn].name);
    int match = 0;
    Sleep(TURN_DELAY);

    // Figures out if computer has matches
    int check = checkMatches(&(g->players[g->whoseTurn]), &(g->pile));

    //Discards first match in computer hand, or draws until a card can be played
    if(check){
        discard(&(g->players[g->whoseTurn]), &(g->pile), check);

    } else {
        match = drawTilMatch(&(g->players[g->whoseTurn]), &(g->pile));
        discard(&(g->players[g->whoseTurn]), &(g->pile), match);
    }
    //Prints the card the computer plays for user to see
    Sleep(TURN_DELAY);
    printf("%s has played a ", (g->players[g->whoseTurn]).name);
    printCard(&(g->pile.discard[g->pile.discTop - 1]));
    printf(".\n\n");

    // Checks if computer won. Returns/prints if it does
    if (checkWin(&(g->players[g->whoseTurn]))){
        Sleep(TURN_DELAY);
        printf("%s has won!\n", (g->players[g->whoseTurn]).name);
        return '!';
    }

    // Performs a special effect if necessary
    if ((g->pile.discard[g->pile.discTop-1].num == -1)&&(match!=-1)){
        specialEffect(g);
    }

    // Displays if the computer has uno
    if (g->players[g->whoseTurn].handSize == 1){
        Sleep(TURN_DELAY);
        printf("%s says \"UNO!\"", g->players[g->whoseTurn].name);
    }

    Sleep(TURN_DELAY);
    printf("\n\n\n\n");
    return g->pile.discard[g->pile.discTop-1].qualifier;
}

// Checks a players hand for matches to the discard pile's top card
int checkMatches(Player* plp, Piles* pileptr){
    int i;
    Card dc = pileptr->discard[pileptr->discTop - 1];
    for(i = 0; i < plp->handSize; i++){
        if(compare(&(plp->hand[i]),&dc)){
            return i + 1;
        }
    }
    return 0;
}

// Takes a chosen card and removes it from the players hand
int discard(Player* plp, Piles* pileptr, int choice){
    // Converts choice to valid array index
    choice--;
    // Checks if a valid card, else asks for another choice
    if((choice < plp->handSize)&&(compare(&(plp->hand[choice]),&(pileptr->discard[pileptr->discTop - 1])))){
        int number = plp->hand[choice].num;
        char col[MAX_ATT_SIZE];
        strcpy(col, plp->hand[choice].color);
        char qual = plp->hand[choice].qualifier;

        // Copies values to top of discard pile adjusts index variable
        pileptr->discard[pileptr->discTop] = makeCard(number, col, qual);
        pileptr->discTop += 1;

        // Moves last card in player hand to the removed card's spot. adjusts index.
        plp->hand[choice] = plp->hand[plp->handSize-1];
        plp->handSize -= 1;

        return 1;

    } else {
        printf("That is not a valid choice. Please select another card.\n");
        scanf("%d", &choice);
        return discard(plp,pileptr,choice);
    }
}

// Checks to see if cards can be played on one another
int compare(Card* c1, Card* c2){
    // Checks for valid number
    if((c1->num == c2->num) && (c1->num != -1)){
        return 1;
    }
    // Checks for valid color
    else if(strcmp(c1->color,c2->color)==0){
        return 1;
    }
    // Checks for special card matches
    else if((c1->qualifier == c2->qualifier)&&(c1->qualifier != 'n')){
        return 1;
    }
    // Allows for wild cards to be played always
    else if((c1->qualifier == 'w') || (c1->qualifier == '4')){
        return 1;
    }
    // returns false for nonmatches
    else{
        return 0;
    }
}

//Displays a player's hand
void printHand(Player* p){
    int j;
    printf("%s's Hand: \n", p->name);
        for(j=0;j<p->handSize;j++){
            printf("%d- ", j+1);
            printCard(&(p->hand[j]));
            printf("\n");
        }
        printf("\n");
}

// Prints cards
void printCard(Card* c){
    // Prints normal card or defers it to a function to deal with special cards
    if(c->num != -1){
        printf("%s %d", c->color, c->num);
    }
    else{
        printSpecial(c);
    }
}

// Prints nontraditional cards
void printSpecial(Card* c){
    // Prints based on qualifier
    switch (c->qualifier){
        case 'r':
            printf("%s Reverse", c->color);
            break;
        case 's':
            printf("%s Skip", c->color);
            break;
        case 'd':
            printf("%s Draw 2", c->color);
            break;
        case 'w':
            printf("%s Card", c->color);
            break;
        case '4':
            printf("%s Draw 4", c->color);
            break;
    }
}

//Prints the discard pile's top card and the current player's hand
void printState(Game* g){
    Card c = g->pile.discard[g->pile.discTop - 1];
    printf("\nThe card on top of the discard pile is a ");
    printCard(&c);
    printf(".\n\n");
    Sleep(TURN_DELAY);
    printHand(&(g->players[g->whoseTurn]));
}

// Performs special card actions
void specialEffect(Game* g){
    Card c = g->pile.discard[g->pile.discTop-1];

    // Calls appropriate functions and adjusts variables. Print effects.
    switch (c.qualifier){
        case 'r':
            g->turnInc *= -1;
            printf("The direction of gameplay is changed!\n");
            break;
        case 'd':
            nextDraw2(g);
            nextPlayer(g);
            break;
        case 's':
            nextPlayer(g);
            printf("%s loses a turn!\n", g->players[g->whoseTurn].name);
            break;
        case 'w':
            wild(g);
            break;
        case '4':
            wild(g);
            nextDraw4(g);
            nextPlayer(g);
            break;

    }
}

// Increments the turnBuffer and figures out the next player
void nextPlayer(Game* g){
    g->turnBuffer += g->turnInc;
    g->whoseTurn = g->turnBuffer % NUM_PLAYERS;
}

// Changes the color in the case of a wild card
void wild(Game* g){
    char col[MAX_ATT_SIZE];
    int choice;
    // Gets user color choice or randomly generates a computer choice
    if(strcmp(g->players[g->whoseTurn].type, "user") == 0){
        printf("\nPlease enter your color choice?\n1-Red\n2-Yellow\n3-Green\n4-Blue\n");
        scanf("%d", &choice);
    } else {
        choice = rand() % NUM_COLORS + 1;
    }
    // Changes color of wild card to the selected card
    strcpy(col, toColor(choice));
    strcpy(g->pile.discard[g->pile.discTop - 1].color, col);
    printf("%s has changed the color to %s.\n",g->players[g->whoseTurn].name,col);
}

// takes in a value and changes it to a string of color
char* toColor(int num){
    switch(num){
        case 1:
            return "Red";
        case 2:
            return "Yellow";
        case 3:
            return "Green";
        case 4:
            return "Blue";
        default:
            return "Wild";
    }

}

// Checks if a player has no cards
int checkWin(Player* p){
    if(p->handSize == 0){
        return 1;
    } else{
        return 0;
    }
}

// Forces the next player to draw two cards
void nextDraw2(Game* g){
    Player* p = &(g->players[(g->turnBuffer + g->turnInc) % NUM_PLAYERS]);
    int i;
    /*int chk = checkQual(p, 'd');
    if(chk){
        printf("%s also plays a Draw 2.", p->name);
        discard(p, &(g->pile), chk);
    } else { */
        Sleep(TURN_DELAY);
        printf("%s has to draw two extra cards and loses a turn!\n", p->name);
        Sleep(TURN_DELAY);
        for (i=0; i<2; i++){
            draw(p, &(g->pile));
        }
   // }
}

// Forces next player to draw four cards
void nextDraw4(Game* g){
    Player* p = &(g->players[(g->turnBuffer + g->turnInc) % NUM_PLAYERS]);
    int i;
    /**int chk = checkQual(p, '4');
    if(chk){
        printf("%s also plays a Wild Draw 4.", p->name);
        discard(p, &(g->pile), chk);
    } else { **/
        Sleep(TURN_DELAY);
        printf("%s has to draw four extra cards and loses a turn!\n", p->name);
        Sleep(TURN_DELAY);
        for (i=0; i<4; i++){
            draw(p, &(g->pile));
        }
    //}
}

// Checks a player's hand for a type of card
int checkQual(Player* p, char q){
    int i;
    for(i=0; i<p->handSize; i++){
        if(p->hand[i].qualifier == q){
            return i;
        }
    }
    return 0;
}

// Draws from the deck until the card is able to be discarded
int drawTilMatch(Player* p, Piles* pile){
    printf("\n");
    while(1){
        printf("%s must draw another card...\n", p->name);
        Sleep(TURN_DELAY);
        draw(p, pile);

        if(checkMatches(p,pile))
            return p->handSize;

    }
    printf("\n");
}

// Moves discard pile to the deck and shuffles the deck
void resetDeck(Piles* pile){
    int i;
    //copies discard to deck
    for(i=0; i<pile->discTop-1; i++){
        char col[MAX_ATT_SIZE];
        int number = pile->discard[i].num;
        char qual = pile->discard[i].qualifier;
        strcpy(col, pile->discard[i].color);

        // Changes wild cards back to Wilds
        if((qual == 'w')||(qual=='4')){
            strcpy(col, "Wild");
        }

        pile->deck[i] = makeCard(number, col, qual);
    }

    // Adjusts discard pile and deckTop
    pile->discard[0] = pile->discard[pile->discTop-1];
    pile->deckTop = pile-> discTop;
    pile->deckTop -= 1;

    //Shuffles the "new" deck adjusts discard top.
    shuffleDeck(pile->deck, pile->deckTop);
    pile->discTop = 1;

    //Prints what has happened
    printf("\nThe deck has run out of cards.\n");
    Sleep(TURN_DELAY);
    printf("The top card of the discard pile is kept...\n");
    Sleep(TURN_DELAY);
    printf("The rest of the cards are re-shuffled and placed into the deck.\n");
    Sleep(TURN_DELAY);
}

//Prints a friendly welcome message
void printWelcome(){
    printf("Hello and welcome to UNO, the game for children of all ages!\n");
    Sleep(TURN_DELAY+500);
    printf("We put a lot of time and effort into this...so you better appreciate it.\n");
    Sleep(TURN_DELAY+500);
    printf("Or else >.<\n");
    Sleep(TURN_DELAY+500);
    printf("Anyway, enjoy! (For rules, please refer to the UNO website)\n\n");
    Sleep(TURN_DELAY+500);
}

//Prints a friendly farewell
void printBye(){
    Sleep(TURN_DELAY);
    printf("\n\nThank you for playing!\n");
    Sleep(TURN_DELAY+500);
    printf("We hope you will join us again for another game soon!\n");
    Sleep(TURN_DELAY+500);
    printf("See you next time!\n\n");
    Sleep(TURN_DELAY);
}
