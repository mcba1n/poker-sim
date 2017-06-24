#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const int CARD_NUM = 52;
const int SUIT_NUM = 4;
const int SUIT_CARD_NUM = 13;

struct card* getRandCard();
struct card* dealHand();
char* getValue(int value);
char* getSuit(int suit);
char* getHand(int hand_strength);
void createDeck();
void printHand(struct card *header);
void printBoard();
void handStateOne();
void handStateTwo();
void changePlayer();
void makeCheck();
void makeFold();
void raisePotBy(int raise);
void dealBoard();
void arraySort(int Array[], int n);
void updateHandData(int n);
void writeHandData();
int makeCompDecision();
int makeCompBet();
int handCompare();
int valueCompare();
int handStrength(struct card *header);
int multiplyPrimes(struct card *header);
int suitCount(struct card *header);
int getPrime(int value);


struct card {
    int value;
    int suit;                                     
    struct card *next;
};

struct player {
    struct card *header;
    int stack;
    char name[20];  
};

struct card *deck[CARD_NUM];
struct card *board_header = NULL;
struct card *board_end = NULL;

struct player Player_One;
struct player Player_Two;
struct player *Player = &Player_One;
struct player *Last_Player = &Player_Two;

FILE *fptr;

int hand_data[9];
int pot = 0, done = 0, gamed_not_finished = 1;

int main() {
    createDeck();
    Player_One.stack = 10000;
    Player_Two.stack = 10000;
    strcpy(Player_One.name, "Player One");
    strcpy(Player_Two.name, "Player Two");
    srand(time(NULL));      // Prevents pseudo-random numbers of rand()

    fptr = fopen("/Users/brendonmcbain/Documents/GitHub/poker-sim/simulation_data.txt", "a+");      // File open for reading and appending
    if (fptr == NULL) 
		printf("Error, can't open the input file");

    while (gamed_not_finished) {
        // Pre-flop
        Player_One.header = dealHand();
        Player_Two.header = dealHand();
        printf("Player One, your hand is\t");
        printHand(Player_One.header);
        handStateOne();
        updateHandData(0);
        printf("\nPre-flop is done\n\n");
        
        // Flop
        if (!done) {
            dealBoard();
            dealBoard();
            dealBoard();
            printf("\nThe board is now\t");
            printBoard();
            handStateOne();
            updateHandData(1);
            printf("\nFlop is done\n\n");
        }

        // Turn
        if (!done) {
            dealBoard();
            printf("\nThe board is now\t");
            printBoard();
            handStateOne();
            updateHandData(2);
            printf("\nTurn is done\n\n");
        }

        // River
        if (!done) {
            dealBoard();
            printf("\nThe board is now\t");
            printBoard();
            handStateOne();
            updateHandData(3);
            printf("\nRiver is done\n\n");
        }

        // Showdown
        int handCmp = handCompare();
        if (handCmp > 0) {
            Player_One.stack += pot;
            printf("\nPlayer One wins with\t");
            printHand(Player_One.header);
            hand_data[8] = 0;
        } else if (handCmp < 0) {
            Player_Two.stack += pot;
            printf("\nPlayer Two wins with\t");
            printHand(Player_Two.header);
            hand_data[8] = 1;
        } else {
            Player_One.stack += pot/2;
            Player_Two.stack += pot/2;
            printf("\nChop the pot!");
            hand_data[8] = 1;
        }
        writeHandData();

        printf("\n\nDo you want to play another hand (0=no, 1=yes)?\t");
        scanf("%i", &gamed_not_finished);
        printf("\n\n");

        // Clear memory and reset variables
        for (int i = 0; i < CARD_NUM; i++) free(deck[i]);
        done = 0;
        pot = 0;
    }
    printf("\n\n");
    fclose(fptr);
    return 0;
}

int multiplyPrimes(struct card *header) {
    struct card *temp = header;
    int product = 1;
    while (temp != NULL) {
        product *= getPrime(temp->value);
        temp = temp->next;
    }
    return product;
}

int suitCount(struct card *header) {
    struct card *temp = header;
    int suit_freq[4] = {0, 0, 0, 0};

    while (temp != NULL) {
        suit_freq[temp->suit - 1]++;
        temp = temp->next;
    }

    int max_suit = 0;
    for (int i = 0; i < SUIT_NUM; i++)
        if (suit_freq[i] > max_suit) max_suit = suit_freq[i];

    return max_suit;
}

/*
    updateHandData(): Updates hand data of the computer at the nth stage of hand
*/
void updateHandData(int n) {
    if (n > 3) return;

    int handProduct = multiplyPrimes(Player_Two.header);
    int suitNum = suitCount(Player_Two.header);

    // Array index using rule i_n = 2n + 1
    hand_data[2*n + 1] = handProduct;
    hand_data[2*n + 2] = suitNum;
}

void writeHandData() {
    for (int i = 0; i < 8; i++) {
        fprintf(fptr, "%i,", hand_data[i]);
    }
    fprintf(fptr, "%i\n", hand_data[8]);
}

int getPrime(int value) {
    switch (value) {
        case 1:
            return 41;
        case 2:
            return 2;
        case 3:
            return 3;
        case 4:
            return 5;
        case 5:
            return 7;
        case 6:
            return 11;
        case 7:
            return 13;
        case 8:
            return 17;
        case 9:
            return 19;
        case 10:
            return 23;
        case 11:
            return 29;
        case 12:
            return 31;
        case 13:
            return 37;
    }
    return 0;
}

/*
    handStateOne(): This is the 1st possible state of a hand where a player
    may bet, check, or fold.
*/
void handStateOne() {
    int decision;
    if (Player == &Player_One) {
        printf("Make your turn (1=bet, 0=check, -1=fold)\t");
        scanf("%i", &decision);
        if (decision > 0) {
            int bet_size;
            printf("How much do you want to bet?\t");
            scanf("%i", &bet_size);
            printf("%s bets %i chips", Player->name, bet_size);
            raisePotBy(bet_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            printf("%s has checked", Player->name);
            makeCheck();
        }
    } else if (Player == &Player_Two) {
        decision = makeCompDecision();
        if (decision > 0) {
            int bet_size = makeCompBet();
            printf("%s bets %i chips", Player->name, bet_size);
            raisePotBy(bet_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            printf("%s has checked", Player->name);
            makeCheck();
        }
    }
}

/*
    handStateTwo(): This is the 2nd possible state of a hand where a player
    may raise, call, or fold.
*/
void handStateTwo(int current_raise) {
    int decision;
    if (Player == &Player_One) {
        printf("Make your turn (1=raise, 0=call, -1=fold)\t");
        scanf("%i", &decision);
        if (decision > 0) {
            int raise_size;
            printf("What do you want to raise to?\t");
            scanf("%i", &raise_size);
            printf("%s raises by %i chips", Player->name, raise_size);
            raisePotBy(raise_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            Player->stack -= current_raise;
            pot += current_raise;
            printf("%s has called", Player->name);
            printf("Current pot is %i, %s has %i chips", pot, Player->name, Player->stack);
            makeCheck();
        }
    } else if (Player == &Player_Two) {
        decision = makeCompDecision();
        if (decision > 0) {
            int raise_size = makeCompBet();
            printf("%s raises by %i chips", Player->name, raise_size);
            raisePotBy(raise_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            Player->stack -= current_raise;
            pot += current_raise;
            printf("%s has called", Player->name);
            printf("\nCurrent pot is %i, %s has %i chips", pot, Player->name, Player->stack);
            makeCheck();
        }
    }
}

/*
    raisePotBy(): Adds chips in the pot. This is used for betting and raising.
*/
void raisePotBy(int raise) {
    if (raise > Player->stack) {
        printf("\nYou do not have enough chips");
        // Now what?
    } else {
        pot += raise;
        Player->stack -= raise;
        printf("\nCurrent pot is %i, %s has %i chips\n", pot, Player->name, Player->stack);
        changePlayer();
        handStateTwo(raise);
    }
}

/*
    changePlayer(): Changes which player player takes the next action.
*/
void changePlayer() {
    if (Player == &Player_One) {
        Player = &Player_Two;
    } else if (Player == &Player_Two) {
        Player = &Player_One;
    } else {
        printf("\nPlayer could not be changed");
    }
}

/*
    makeCheck(): Check action by player. This is similar to a call action.
*/
void makeCheck() {
    if (Player == Last_Player) {
        changePlayer();
    } else {
        changePlayer();
        handStateOne();
    }
}

/*
    makeFold(): Fold action by player.
*/
void makeFold() {
    printf("\n%s has folded... ", Player->name);
    changePlayer();
    Player->stack += pot;
    pot = 0;
    done = 0;
    printf("%s wins!", Player->name);
    changePlayer();
}

int makeCompDecision() {
    return 0;
}

int makeCompBet() {
    return 1000;
}

/*
    createDeck(): Creates the node for each card in memory, then adds
    them to an array which represents the deck.
*/
void createDeck() {
    for (int i = 0; i < SUIT_NUM; i++) {
        for (int j = 0; j < SUIT_CARD_NUM; j++) {
            struct card *card = (struct card *)malloc(sizeof(struct card));
            if (card == NULL) {
                printf("\nThe card could not be created");
            } else {
                card->value = j + 1;
                card->suit = i + 1;
                deck[i*SUIT_CARD_NUM + j] = card;       // Add card to deck in order
            }
        }
    }
}

/*
    getRandCard(): Retrieves a card at *random* index of the instantiated deck.
*/
struct card* getRandCard() {
    struct card *new_card = NULL;
    int card_index;

    while (new_card == NULL) {
        card_index = rand() % CARD_NUM;
        new_card = deck[card_index];
    }
    deck[card_index] = NULL;        // Remove card from deck
    return new_card;
}

/*
    dealHand(): Deals a hand of two cards to each player in the form of
    a linked list.
*/
struct card* dealHand() {
    struct card *player_header = NULL;
    struct card *card_one = getRandCard();
    struct card *card_two = getRandCard();

    player_header = card_one;
    card_one->next = card_two;
    card_two->next = NULL;

    return player_header;
}

/*
    dealBoard(): Deals a card to the linked list of the board.
*/
void dealBoard() {
    if (board_header == NULL) {
        board_header = getRandCard();
        board_end = board_header;

        // Append player hands to the beginning of the linked list
        Player_One.header->next->next = board_header;
        Player_Two.header->next->next = board_header;
    } else {
        board_end->next = getRandCard();
        board_end = board_end->next;
    }
    board_end->next = NULL;
}

/*
    getValue(): Converts numerical value for card rank to its representing string.
*/
char* getValue(int value) {
    switch (value) {
        case 1:
            return "Ace";
        case 2:
            return "2";
        case 3:
            return "3";
        case 4:
            return "4";
        case 5:
            return "5";
        case 6:
            return "6";
        case 7:
            return "7";
        case 8:
            return "8";
        case 9:
            return "9";
        case 10:
            return "10";
        case 11:
            return "Jack";
        case 12:
            return "Queen";
        case 13:
            return "King";
    }
    return NULL;
}


/*
    getSuit(): Converts numerical value for suit to its representing string.
*/
char* getSuit(int suit) {
    switch (suit) {
        case 1:
            return "Diamond";
        case 2:
            return "Club";
        case 3:
            return "Spade";
        case 4:
            return "Heart";
    }
    return NULL;
}

/*
    printHand(): Prints the hand for a given player. It takes the header of a
    player's linked list as an argument.
*/
void printHand(struct card *header) {
    struct card *temp = header;
    printf("<%s><%s>\t", getValue(temp->value), getSuit(temp->suit));
    printf("<%s><%s>\t", getValue(temp->next->value), getSuit(temp->next->suit));
    printf("\n");
}

/*
    printBoard(): Prints the cards that have currently been dealt.
*/
void printBoard() {
    struct card *temp = board_header;
    if (temp == NULL) {
        printf("\nThe header is null");
    } else {
        while (temp != NULL) {
            printf("<%s><%s>\t", getValue(temp->value), getSuit(temp->suit));
            temp = temp->next;
        }
    }
    printf("\n");
}

/*
    handCompare(): Deligates the processes for evaluating the best hand at showdown.
*/
int handCompare() {
    int Player_One_hand = handStrength(Player_One.header);
    int Player_Two_hand = handStrength(Player_Two.header);
    printf("\nPlayer One has a %s, and Player Two has a %s", getHand(Player_One_hand), getHand(Player_Two_hand));
    
    if (Player_One_hand > Player_Two_hand) {
        return -1;
    } else if (Player_Two_hand > Player_One_hand) {
        return 1;
    } else {
        // Equal hand strengths
        int value = valueCompare();
        if (value > 0) return 1;
        else if (value < 0) return -1;
        else return 0;
    }
}

/*
    valueCompare(): When handStrength() returns the same hand strength for the players,
    we must then compare high cards of the hands.
*/
int valueCompare() {
    struct card *temp_one = Player_One.header;
    struct card *temp_two = Player_Two.header;

    while (temp_one != NULL && temp_two != NULL) {
        if (temp_one->value > temp_two->value) return 1;
        else if (temp_two->value > temp_one->value) return -1;
        else return 0;
        temp_one = temp_one->next;
        temp_two = temp_two->next;
    }
    printf("\nThe value could not be compared");
    return 0;
}

/*
    handStrength(): Returns an integer representing hand strength. Lower numbers signify 
    a stronger hand.
*/
int handStrength(struct card *header) {
    int freq[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};     // Histogram of card frequencies
    int isRoyalFlush, isStraightFlush, isFourKind, 
        isFullHouse, isFlush, isStraight, isThreeKind,
        isTwoPair, isPair, isHighCard;
    struct card *temp = header;

    isFlush = 1;
    while(temp != NULL) {
        // If a suit does not equal its neighbour, it is not a Flush
        if (temp->next != NULL)
            if (temp->suit != temp->next->suit) isFlush = 0;

        freq[temp->value - 1]++;
        temp = temp->next;
    }

    if (isFlush && freq[0] && freq[9] && freq[10] && freq[11] && freq[12]) isRoyalFlush = 1;
    else isRoyalFlush = 0;

    // Find isStraight by summing five histogram frequencies at a time and then shift right
    isStraight = 0;
    for (int i = 0; i < SUIT_CARD_NUM - 5; i++) {
        int sum = 0;

        for (int j = 0; j < 5; j++) sum += freq[i + j];

        if (sum == 5) {
            isStraight = 1;
            break;
        }
    }

    // Check the upper values of the histogram to determine hand types
    arraySort(freq, SUIT_CARD_NUM);
    if (isStraight && isFlush) isStraightFlush = 1;
    else isStraightFlush = 0;
    if (freq[0] == 4 && freq[1] == 1) isFourKind = 1;
    else isFourKind = 0;
    if (freq[0] == 3 && freq[1] == 2) isFullHouse = 1;
    else isFullHouse = 0;
    if (freq[0] == 3 && freq[1] == 1 && freq[2] == 1) isThreeKind = 1;
    else isThreeKind = 0;
    if (freq[0] == 2 && freq[1] == 2 && freq[2] == 1) isTwoPair = 1;
    else isTwoPair = 0;
    if (freq[0] == 2 && freq[1] == 1 && freq[2] == 1) isPair = 1;
    else isPair = 0;

    // Return the numerical value of hand strength
    if (isRoyalFlush) return 1;
    else if (isStraightFlush) return 2;
    else if (isFourKind) return 3;
    else if (isFullHouse) return 4;
    else if (isFlush) return 5;
    else if (isStraight) return 6;
    else if (isThreeKind) return 7;
    else if (isTwoPair) return 8;
    else if(isPair) return 9;
    else return 10;
}

/*
    getHand(): Converts numerical hand strength integer to its representing string.
*/
char* getHand(int hand_strength) {
    switch (hand_strength) {
        case 1:
            return "Royal Flush";
        case 2:
            return "Straight Flush";
        case 3:
            return "Four of a Kind";
        case 4:
            return "Full House";
        case 5:
            return "Flush";
        case 6:
            return "Straight";
        case 7:
            return "Three of a Kind";
        case 8:
            return "Two Pair";
        case 9:
            return "Pair";
        case 10:
            return "High Card";
    }
    return NULL;
}

/*
    arraySort(): Uses selection sort to re-order the values of an array
    in descending order from left.
*/
void arraySort(int Array[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int jMin = i;
        for (int j = i + 1; j < n; j++) {
            if (Array[jMin] < Array[j]) {
                jMin = j;
            }
        }
        if (jMin != i) {
            int temp = Array[jMin];
            Array[jMin] = Array[i];
            Array[i] = temp;
        }
    }
}