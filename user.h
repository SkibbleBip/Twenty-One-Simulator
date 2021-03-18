/***************************************************************************
* File:  user.h
* Author:  SkibbleBip
* Procedures: N/A
***************************************************************************/

#ifndef USER_H
#define USER_H


typedef int Anonymous_Pipe[2];
/* Define the stdin and stdout pipe array */
typedef enum {false, true} bool;
/* Quick hack to support bool types*/

typedef enum Action{HIT, STAND} action;
/* Define the enums for the actions hit and stand */





typedef struct User{
/* Defines the Player struct and it's properties */
        int wins;                /*The number of wins a player has received*/
        int loss;                /*The number of losses a player has received*/
        Anonymous_Pipe parentIn; /*The communication pipe for receiving data*/
        Anonymous_Pipe parentOut;/*The communication pipe for sending data*/
        Anonymous_Pipe sibling;  /*Ditto, but for the sibling player*/
        short count;             /*The current card hand value count*/
        bool hadAce;     /*flag for whether or not the player received an ace*/

} Player;


typedef struct Card{
/* struct of a single card. The card is comprised of it's ID
 * (position in the deck, in order) and value, which is its face
 * value (7, 10, 11)
 */
        int id;
        int value;
} card;

Player playerOne;
Player playerTwo;
Player dealerPlayer;
/*The Player structs for the players and dealer, they hold the information
 * about the game information such as wins/losses and hands
 */


void clearCards(Player* player);
void addCard(Player* player, card c);
void processOne(void);
void processTwo(short in);
short getCountedCardValue(card q);
/*Function prototype Definitions*/

void failedReadP1(void);
void failedReadP2(void);
void failedWriteP1(void);
void failedWriteP2(void);
void failedDealerPipe(void);
/*Function prototypes for handling pipe read/write errors*/



#endif // USER_H
