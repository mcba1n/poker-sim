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

int makeCompDecision();
int makeCompBet();

void createDeck();
void cardToBoard();
void printHand(struct card *header);
void printBoard();
void handStateOne();
void handStateTwo();
void changePlayer();
void makeCheck();
void makeFold();
void raisePotBy(int raise);
void dealBoard();
void continueHand();

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

int pot = 0, done = 0, gamed_not_finished = 0;

int main() {
    createDeck();
    Player_One.stack = 10000;
    Player_Two.stack = 10000;
    strcpy(Player_One.name, "Player One");
    strcpy(Player_Two.name, "Player Two");
    srand(time(NULL));

    while (gamed_not_finished) {
        // Pre-flop:
        Player_One.header = dealHand();
        Player_Two.header = dealHand();
        printf("Player One, your hand is\t");
        printHand(Player_One.header);
        handStateOne();
        printf("\nPre-flop is done");
        
        // Flop:
        continueHand();
        printf("\nFlop is done");

        // 2nd street:
        continueHand();
        printf("\n2nd street is done");

        // Turn:
        continueHand();
        printf("\nTurn is done");

        // River:
        continueHand();
        printf("\nRiver is done");

        // Showdown:
        printf("\nShowdown is done");

        printf("\nDo you want to play another hand (0=no, 1=yes)?\t");
        scanf("%i", &gamed_finished);
        if (!gamed_finished) {
            free(board_header);
            free(board_end);
            free(Player_One.header);
            free(Player_Two.header);
            for (int i = 0; i < CARD_NUM; i++) free(deck[i]);
            done = 0;
        }
    }
    printf("\n\n");
    return 0;
}

void continueHand() {
    if (!done) {
        dealBoard();
        printf("\nThe board is now\t");
        printBoard();
        handStateOne();
    }
}
void handStateOne() {
    int decision;
    if (Player == &Player_One) {
        printf("Make your turn (1=bet, 0=check, -1=fold)\t");
        scanf("%i", &decision);
        if (decision > 0) {
            int bet_size;
            printf("How much do you want to bet?\t");
            scanf("%i", &bet_size);
            printf("%s bets %i chips\n", Player->name, bet_size);
            raisePotBy(bet_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            printf("\n%s has checked", Player->name);
            makeCheck();
        }
    } else if (Player == &Player_Two) {
        decision = makeCompDecision();
        if (decision > 0) {
            int bet_size = makeCompBet();
            printf("%s bets %i chips\n", Player->name, bet_size);
            raisePotBy(bet_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            printf("\n%s has checked", Player->name);
            makeCheck();
        }
    }
}

void handStateTwo(int current_raise) {
    int decision;
    if (Player == &Player_One) {
        printf("Make your turn (1=raise, 0=call, -1=fold)\t");
        scanf("%i", &decision);
        if (decision > 0) {
            int raise_size;
            printf("What do you want to raise to?\t");
            scanf("%i", &raise_size);
            printf("%s raises by %i chips\n", Player->name, raise_size);
            raisePotBy(raise_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            Player->stack -= current_raise;
            pot += current_raise;
            printf("\n%s has called", Player->name);
            printf("\nCurrent pot is %i, %s has %i chips\n", pot, Player->name, Player->stack);
            makeCheck();
        }
    } else if (Player == &Player_Two) {
        decision = makeCompDecision();
        if (decision > 0) {
            int raise_size = makeCompBet();
            printf("%s raises by %i chips\n", Player->name, raise_size);
            raisePotBy(raise_size);
        } else if (decision < 0) {
            makeFold();
        } else {
            Player->stack -= current_raise;
            pot += current_raise;
            printf("\n%s has called", Player->name);
            printf("\nCurrent pot is %i, %s has %i chips\n", pot, Player->name, Player->stack);
            makeCheck();
        }
    }
}

void raisePotBy(int raise) {
    if (raise > Player->stack) {
        printf("\nYou do not have enough chips");
    } else {
        pot += raise;
        Player->stack -= raise;
        printf("\nCurrent pot is %i, %s has %i chips\n", pot, Player->name, Player->stack);
        changePlayer();
        handStateTwo(raise);
    }
}

void changePlayer() {
    if (Player == &Player_One) {
        Player = &Player_Two;
    } else if (Player == &Player_Two) {
        Player = &Player_One;
    } else {
        printf("\nPlayer could not be changed");
    }
}

void makeCheck() {
    if (Player == Last_Player) {
        changePlayer();
    } else {
        changePlayer();
        handStateOne();
    }
}

void makeFold() {
    printf("\n%s has folded... ", Player->name);
    changePlayer();
    Player->stack += pot;
    pot = 0;
    printf("%s wins!", Player->name);
    changePlayer();
}

int makeCompDecision() {
    // make calculations on current situation based on EV
    // 1. find distribution of hands player one may have
    // 2. narrow it using a specified range --- how?
    // 3. *randomly* choose one of these probabilities to calculate an EV
    // 4. make the decision ( 1=raise, -1=fold, 0=call )
    return 0;
}

int makeCompBet() {
    // analyse current pot, bet size, and then compare with Player Two's EV
    // 10% above = raise, 10% below = fold, between these = call
    return 1000;
}

void cardToBoard() {
    struct card *card = getRandCard();

    if (board_header == NULL) { 
        board_header = card;
        board_end = card;
        Player_One.header->next->next = board_header;
        Player_Two.header->next->next = board_header;
    } else {
        board_end->next = card; 
    }
}

void createDeck() {
    for (int i = 0; i < SUIT_NUM; i++) {
        for (int j = 0; j < SUIT_CARD_NUM; j++) {
            struct card *card = (struct card *)malloc(sizeof(struct card));
            if (card == NULL) {
                printf("\nThe card could not be created");
            } else {
                card->value = j + 1;
                card->suit = i + 1;
                deck[i*SUIT_CARD_NUM + j] = card;
            }
        }
    }
}

struct card* getRandCard() {
    struct card *new_card = NULL;
    int card_index;

    while (new_card == NULL) {
        card_index = rand() % CARD_NUM;
        new_card = deck[card_index];
    }
    deck[card_index] = NULL;
    return new_card;
}

struct card* dealHand() {
    struct card *player_header = NULL;
    struct card *card_one = getRandCard();
    struct card *card_two = getRandCard();

    player_header = card_one;
    card_one->next = card_two;
    card_two->next = NULL;

    return player_header;
}

void dealBoard() {
    if (board_header == NULL) {
        board_header = getRandCard();
        board_end = board_header;
    } else {
        board_end->next = getRandCard();
        board_end = board_end->next;
    }
    board_end->next = NULL;
}

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
}

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
}

void printHand(struct card *header) {
    struct card *temp = header;
    printf("<%s><%s>\t", getValue(temp->value), getSuit(temp->suit));
    printf("<%s><%s>\t", getValue(temp->next->value), getSuit(temp->next->suit));
    printf("\n");
}

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

// analyse current hand of both players, return: 1=player one best hand, 0=equal hand, -1=player two best hand
int bestHand() {
    // isRoyalFlush(); etc.
}