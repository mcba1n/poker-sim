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
int handCompare();
int valueCompare();
int handStrength(struct card *header);
void arraySort(int Array[], int n);
void linkedListSort(struct card *header, struct card *end);

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
        Player_One.header = dealHand();
        Player_Two.header = dealHand();
        printf("Player One, your hand is\t");
        printHand(Player_One.header);
        handStateOne();
        printf("\nPre-flop is done");
        
        continueHand();
        printf("\nFlop is done");
        continueHand();
        printf("\n2nd street is done");
        continueHand();
        printf("\nTurn is done");
        continueHand();
        printf("\nRiver is done");

        if (handCompare() > 0) {
            Player_One.stack += pot;
            printf("\nPlayer One wins!");
        } else if (handCompare() < 0) {
            Player_Two.stack += pot;
            printf("\nPlayer Two wins!");
        } else {
            Player_One.stack += pot/2;
            Player_Two.stack += pot/2;
            printf("\nChop the pot!");
        }

        printf("\nDo you want to play another hand (0=no, 1=yes)?\t");
        scanf("%i", &gamed_not_finished);
        
        free(board_header);                                             // Reset hand
        free(board_end);
        free(Player_One.header);
        free(Player_Two.header);
        for (int i = 0; i < CARD_NUM; i++) free(deck[i]);
        done = 0;
        pot = 0;
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

int handCompare() {
    int Player_One_hand = handStrength(Player_One.header);
    int Player_Two_hand = handStrength(Player_Two.header);
    if (Player_One_hand > Player_Two_hand) {
        return -1;
    } else if (Player_Two_hand > Player_One_hand) {
        return 1;
    } else {
        int value = valueCompare();
        if (value > 0) {
            return 1;
        } else if (value < 0) {
            return -1;
        } else {
            return 0;
        }
    }
}

int valueCompare() {
    struct card *temp_one = Player_One.header;
    struct card *temp_two = Player_Two.header;
    while (temp_one != NULL && temp_two != NULL) {
        if (temp_one->value > temp_two->value) {
            return 1;
        } else if (temp_two->value > temp_one->value) {
            return -1;
        } else {
            return 0;
        }
        temp_one = temp_one->next;
        temp_two = temp_two->next;
    }
}

int handStrength(struct card *header) {
    int freq[13];
    int isRoyalFlush, isStraightFlush, isFourKind, 
        isFullHouse, isFlush, isStraight, isThreeKind,
        isTwoPair, isPair, isHighCard;
    struct card *temp = header;

    linkedListSort(header, board_end);
    isFlush = 1;
    while(temp != NULL) {
        if (temp->next != NULL) {
            if (temp->suit != temp->next->suit) isFlush = 0;
        }
        freq[temp->value]++;
        temp = temp->next;
    }

    arraySort(freq, SUIT_CARD_NUM);
    if (header->value - board_end->value == 4) isStraight = 1;
    else isStraight = 0;
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

    if (isRoyalFlush) return 1;
    else if (isStraight) return 2;
    else if (isFourKind) return 3;
    else if (isFullHouse) return 4;
    else if (isFlush) return 5;
    else if (isStraight) return 6;
    else if (isThreeKind) return 7;
    else if (isTwoPair) return 8;
    else if(isPair) return 9;
    else return 10;
}

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

void linkedListSort(struct card *header, struct card *end) {
    struct card *curr, *next, *prev, *tmp;
    end = NULL;

    while(header->next != end) {                            // Sort complete when the second node is marked as the end
        prev = curr = header;                               // Initialisation for a new iteration
        next = header->next;

        while(curr != end) {                                // Loop the swap or continue process until the end
            if(curr->value < next->value) {                 // Decide to swap
                if(curr == header) {
                                                            // Swap curr and next nodes
                    tmp = next -> next;
                    next->next = curr;
                    curr->next = tmp;

                    header = next;                          // Set the new header node
                    prev = next;
                }
                else {
                    tmp = next->next;                       // Swap curr and next nodes
                    next->next = curr;
                    curr->next = tmp;

                    prev->next = next;                      // Set the new previous node
                    prev = next;
               }
           }
           else {
                prev = curr;                                // Set the new prev node
                curr = curr->next;                          // Set the new curr node
           }
           next = next->next;                               // Traverse to the new next node
           if(next == end) end = curr;                      // Assume the last node checked is the greatest
        }
    }
}



