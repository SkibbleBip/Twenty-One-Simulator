/***************************************************************************
* File:  user.c
* Author: SkibbleBip
* Procedures:
* clearCards            -Clears the inputted player's card inventory.
* addCard               -Adds cards to the inputted player
* processOne            -The main logic function for the first process.
* processTwo            -The main logic function for the second process.
* getCountedCardValue   -Translates the card into a card-counting value
* failedReadP1          -Print error message when reading Player One Pipe
* failedReadP2          -Print error message when reading Player Two Pipe
* failedWriteP1         -Print error message when writing to Player One Pipe
* failedWriteP2         -Print error message when writing to Player Two Pipe
* failedDealerPipe      -Print error message when reading or writing to Dealer's
*                               Pipe
***************************************************************************/
#include "user.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>



/***************************************************************************
* void clearCards(Player* player)
* Author: SkibbleBip
* Date: 03/03/2021
* Description: Clears the inputted player's card inventory.
*
* Parameters:
*        player    I/O    Player*    Player struct pointerthat contains all the
*                                    statistics such as wins, losses, pipes,
*                                    and current card values.
**************************************************************************/
void clearCards(Player* player)
{
        player->count = 0;
        player->hadAce = false;
        /* Set the card value count to 0, set the ace card pickup to false*/
}


/***************************************************************************
* void addCard(Player* player, card c)
* Author: SkibbleBip
* Date: 03/03/2021
* Description: Adds cards to the inputted player
*
* Parameters:
*        player    I/O    Player*    The struct pointer to the player
*        c         I/P    card       The card that is to be added to the player
**************************************************************************/
void addCard(Player* player, card c)
{
        if(c.value == 1){
        /* If the added card is Ace, inform the ace flag and add 11 to total */
                if(player->hadAce == true){
                /* If the deck already has an ace, adding another will bypass
                 * the 21 limit, so instead we add 1 to the total.
                 */
                        player->count += 1;
                }
                else{
                /* Otherwise just add the Ace */
                        player->hadAce = true;
                        player->count += 11;

                }

                return;

        }
        if(c.value > 7 && player->hadAce == true && player->count == 14){
        /* Hack algorythm designed to simply determine if the
         * player has an ace and the total hand value excedes 21, so to
         * replace the 11 with a 1
         */
                player->count -= 10;
                player->count += c.value;
                player->hadAce = false;
                /*Reset the ace flags*/

        }
        else{
        /* If the previous check for an ace causing a >21 value returns false,
         * the just add the card to the total count
         */
                player->count += c.value;

        }

}


/***************************************************************************
* void processOne(void)
* Author: SkibbleBip
* Date: 03/03/2021
* Description: The main logic function for the first process.
*
* Parameters:   N/A
**************************************************************************/
void processOne(void)
{

        bool stop = false;
        do{

                action a;
                if(playerOne.count >=17){
                /*if the hand count is 17 or greater, stand*/
                        a = STAND;
                        if(write(playerOne.parentOut[1], &a, sizeof(a)) == -1){
                                failedWriteP1();
                        }
                        stop = true;
                }
                else{
                /*Otherwise, hit*/
                        a = HIT;
                        if(write(playerOne.parentOut[1], &a, sizeof(a)) == -1){
                        /*Send our choice to the dealer*/
                                failedWriteP1();
                        }
                        card c;
                        if(read(playerOne.parentIn[0], &c, sizeof(c)) == -1){
                        /*receive new card from dealer*/
                                failedReadP1();
                        }
                        addCard(&playerOne, c);
                        /*add card to hand*/
                }

        }while(stop == false);
        /*loop until told to stop*/

        if(write(playerOne.parentOut[1], &playerOne.count, sizeof(short)) == -1)
        {/* tell the dealer what the player one card deck totals to */
                failedWriteP1();
        }
        clearCards(&playerOne);
        /*clear player one's hands*/


}

/***************************************************************************
* void processTwo(short in)
* Author: SkibbleBip
* Date: 03/03/2021
* Description: The main logic function for the second process.
*
* Parameters:
*        in    I/P    short    The counted card value of player two's
*                              owned cards.
**************************************************************************/
void processTwo(short in)
/* Stand on cardcount <= -2, hit otherwise */
{
        short cardCount = 0;

        /*The following block of code receives copies of cards from the dealer
         * and player one to calculate the "card counting" value. This is done
         * buy passing the cards to getCountedCardValue() which calculates the
         * card count value.
         */

        card c;
        if(read(playerOne.sibling[0], &c, sizeof(c)) == -1){
        /* get first counted card from sibling*/
                failedReadP2();
        }
        cardCount += getCountedCardValue(c);
        if(read(playerOne.sibling[0], &c, sizeof(c)) == -1){
        /* get second counted card from sibling*/
                failedReadP2();
        }
        cardCount += getCountedCardValue(c);
        if(read(dealerPlayer.parentOut[0], &c, sizeof(c)) == -1){
        /* get first counted dealer card */
                failedReadP2();
        }
        cardCount +=getCountedCardValue(c);
        //if(read(dealerPlayer.parentOut[0], &c, sizeof(c)) == -1){
        /* get second counted dealer card */
         //       failedReadP2();
        //}
        //cardCount +=getCountedCardValue(c);
        cardCount += in;
        /*Finished calculated the current deck deal count*/

        action a;
        do{

                if(cardCount <=-2 || playerTwo.count >= 21){
                /*if the card count value is <=-2 (or if busted), stand and
                 *send the action to the dealer */
                        a = STAND;
                        if(write(playerTwo.parentOut[1], &a, sizeof(a)) == -1){
                                failedWriteP2();
                        }
                }
                else{
                /*otherwise, hit and send the hit action to the dealer,
                 * receive the new card fromt he dealer
                 */
                        a = HIT;
                        if(write(playerTwo.parentOut[1], &a, sizeof(a)) == -1){
                                failedWriteP2();
                        }

                        card z;
                        read(playerTwo.parentIn[0], &z, sizeof(z));
                        addCard(&playerTwo, z);
                        cardCount += getCountedCardValue(z);
                }



        }while(a == HIT);
        /* Continue receiving cards while the player is hitting */



        if(write(playerTwo.parentOut[1], &playerTwo.count, sizeof(short))==-1){
                failedWriteP2();
        }
        /* tell the dealer what the player one card deck totals to */
        clearCards(&playerTwo);

}

/***************************************************************************
* short getCountedCardValue(card q)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Translates the card into a card-counting value
*
* Parameters:
*        q                      I/P    card     The inputted card to be
*                                               calculated
*        getCountedCardValue    O/P    short    The calculated card-count value
**************************************************************************/

short getCountedCardValue(card q)
{
        if(q.value == 1)
                return -1;
        else if(q.value >= 2 && q.value <= 6)
                return 1;
        else if(q.value >= 7 && q.value <= 9)
                return 0;
        else
                return -1;
        /* When counting cards, all cards >=2 and <=6 produce a card count of
         * +1, >=7 and <=9 is ignored (0), and from 10 and ace is -1. The Lower
         * the total card count when adding all the cards together, the better
         * chance you have of pulling a bigger card
         * (obviously casinos dont like this)
         */

}
/***************************************************************************
* void failedReadP1(void)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Print error message when reading Player One Pipe
*
* Parameters: N/A
**************************************************************************/
void failedReadP1(void){
        perror("Failed to read from Player One Pipe");
        exit(-2);

}
/***************************************************************************
* void failedReadP2(void)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Print error message when reading Player Two Pipe
*
* Parameters: N/A
**************************************************************************/
void failedReadP2(void){
        perror("Failed to read from Player Two Pipe");
        exit(-2);

}
/***************************************************************************
* void failedWriteP1(void)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Print error message when writing to Player One Pipe
*
* Parameters: N/A
**************************************************************************/
void failedWriteP1(void){
        perror("Failed to write to Player One Pipe");
        exit(-2);

}
/***************************************************************************
* void failedWriteP2(void)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Print error message when writing to Player Two Pipe
*
* Parameters: N/A
**************************************************************************/
void failedWriteP2(void){
        perror("Failed to write to Player Two Pipe");
        exit(-2);

}
/***************************************************************************
* void failedDealerPipe(void)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Print error message when reading or writing to Dealer's Pipe
*
* Parameters: N/A
**************************************************************************/
void failedDealerPipe(void){
        perror("Failed to read/write to or from Dealer's Pipe");
        exit(-2);
}




