/***************************************************************************
* File:  main.c
* Author:  SkibbleBip
* Procedures:
* main          -Main driver function
* initialDeal   -Function to deal the first 2 cards to each hand
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "main.h"

#define         MAX_COUNTS      1000000
/*define the number of tests the program performs*/

void initialDeal(card* dCards);
//prototype for the Initial card dealing function

Player dealer;
//Player struct for the dealer


/***************************************************************************
* int main()
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Main driver function
*
* Parameters: N/A
**************************************************************************/
int main()
{
        fprintf(stdout,
        "This software is designed to simulate 2 different strategies for the\n"
        "cardgame Twenty-One. The first strategy is to only stand on values\n"
        "higher that 17, and the second strategy attempts to count cards.\n"
        "Dealer hits until 17 or higher. Program simulates %d rounds.\n\n"
        "\tPress Enter to continue...\n", MAX_COUNTS);
        /* Inform the user what the purpose of the program is */

        getchar();
        /* Get the enter press */

        initCards();
        /* Initialize the cards*/

        pid_t userOne, userTwo, dealer;
        /*
         * Create the PIDs for the 2 players and dealer
         * dealer will be the parent, users will be the children
        */

         if( pipe(playerOne.parentIn) == -1){
        /* Open the IPC pipes for player one */
                perror("Failed to create player one pipes");
                exit(-2);
        }
        if( pipe(playerOne.parentOut) == -1 ){
        /* Open the IPC pipes for player one. sibling pipe is the pipe it sends
         * the counted cards
         */
                perror("Failed to create player one pipes");
                exit(-2);
        }

        if(pipe(playerOne.sibling) == -1){
        /* Open the IPC pipes for player one. sibling pipe is the pipe it sends
         * the counted cards
         */
                perror("Failed to create player one pipes");
                exit(-2);
        }

        if( pipe(playerTwo.parentIn) == -1){
        /* Open the IPC pipes for player two */
                perror("Failed to create player two pipes");
                exit(-2);
        }

        if( pipe(playerTwo.parentOut) == -1){
        /* Open the IPC pipes for player two */
                perror("Failed to create player two pipes");
                exit(-2);
        }

        if( pipe(dealerPlayer.parentOut) == -1 ){
        /* This pipe is a hack that will allow player 2 to
         *count the dealer's cards
         */
                perror("Failed to create player two pipes");
                exit(-2);
        }

        /* Initialize all the player's and dealer's default properties */
        playerOne.wins = 0;
        playerOne.loss = 0;
        playerOne.count = 0;
        playerOne.hadAce = false;

        playerTwo.wins = 0;
        playerTwo.loss = 0;
        playerTwo.count = 0;
        playerTwo.hadAce = false;

        dealerPlayer.wins = 0;
        dealerPlayer.loss = 0;
        dealerPlayer.count = 0;
        dealerPlayer.hadAce = false;
        /* all the players and dealer have been set up */


        dealer = getpid();
        /* Get the PID of the dealer */
        fprintf(stdout, "Dealer has been created, PID is %d.\n", dealer);

        pid_t tmp = fork();
        //get tmp pid of newest fork
        if(tmp < 0){
                fprintf(stderr, "Failed to fork! %s\n", strerror(errno));
                exit(-1);

        }
        else{
               /* Return PID to parent, this is the PID of the child (user 1) */
               userOne = tmp;
               //userTwo = -1;
               if(userOne!=0){
                        fprintf(stdout, "Created first user. PID is %d.\n",
                        userOne
                        );
                }

        }

        if(userOne > 0)
                tmp = fork();
        /* Fork second time to create userTwo */

        if(tmp < 0){
                fprintf(stderr, "Failed to fork! %s\n", strerror(errno));
                exit(-1);
        }
        else if(userOne > 0){
                /* Return PID to parent, this is the PID of the child
                (user 2) */
                userTwo = tmp;
                if(userTwo!=0){
                        fprintf(stdout,
                        "Created Second user. PID is %d.\n",
                        userTwo
                        );
                }

        }
        //getchar();

        /* From this point on, the PIDs of all the needed processes have been
        procured. */



        int i;
        for(i = 0; i < MAX_COUNTS; i++){
        /* Begin the simulation process. This repeats MAX_COUNTS times*/

        /* Passing cards initially works like this: p1, p2, d, p1, p2, d */
                if(userOne > 0 && userTwo > 0){
                /* Dealer. Dealer hits until 17 or higher. */
                        shuffle();
                        /*Shuffle the cards*/

                        int p1Count = 2;
                        int p2Count = 2;
                        /*The initial hand count (starts with 2)*/
                        bool p1BlackJackOrBust = false;
                        bool p2BlackJackOrBust = false;
                        /*Flag for when the players bust or get a blackjack*/



                        card dealtCards[6];
                        int j;
                        int z = 0;
                        /*z is the index of the dealer's shoe as it is being
                         *dealt
                         */

                        for(j = 0; j < 6; j++){
                                dealtCards[j] = cardDeck[z];
                                z++;
                        }
                        /* Draw the first 2 cards per each player + dealer */

                        initialDeal(dealtCards);
                        /* Cards have been dealt */

                        action a;
                        if(read(playerOne.parentOut[0], &a, sizeof(a)) == -1){
                        /* read the playerOne's original choice */
                                failedReadP1();
                        }

                        while(a == HIT){
                        /* while the player is hitting, send a new card to
                         * the player
                         */

                                p1Count++;
                                /*Increase the amount of cards dealt to p1*/
                                if(write(playerOne.parentIn[1], &cardDeck[z],
                                        sizeof(card)) == -1){
                                /*Send card to p1*/
                                                failedWriteP1();
                                }

                                z++;
                                /*Increase the deal deck index*/
                                if(read(playerOne.parentOut[0], &a,
                                        sizeof(a)) == -1){
                                /*Receive response from player 1*/
                                                failedReadP1();
                                }

                        }
                        if(read(playerOne.parentOut[0], &playerOne.count,
                                sizeof(short)) == -1){
                        /* Obtain the count of the first player */
                                        failedReadP1();
                        }


                        if(playerOne.count > 21){
                                printf("Player 1 busts with %d\n",
                                        playerOne.count
                                );
                                /*If p1 count is bigger than 21, it busted*/
                                p1BlackJackOrBust = true;
                                playerOne.loss++;
                                dealerPlayer.wins++;

                        }
                        else if(playerOne.count == 21 && p1Count ==2){
                        /*If hand is 21 and only 2 cards were dealt, then its
                         *a blackjack
                         */
                                printf("PLAYER 1 GOT A BLACKJACK!\n");
                                playerOne.wins++;
                                dealerPlayer.loss++;
                                p1BlackJackOrBust = true;

                        }
                        /* Ended player one turn. Player 2 begins.*/


                        if(read(playerTwo.parentOut[0], &a, sizeof(a)) == -1){
                        /* read the playerOne's original choice */
                                failedReadP2();
                        }


                        while(a == HIT){
                        /* while the player is hitting, send a new card to
                         * the player
                         */
                                p2Count++;
                                /*Increase the hand card count*/
                                if(write(playerTwo.parentIn[1],
                                &cardDeck[z], sizeof(card)) == -1){
                                /*send the dealt card to p2*/
                                        failedWriteP2();
                                }
                                z++;
                                /*Increase the shoe index*/
                                if(read(playerTwo.parentOut[0],
                                &a, sizeof(a)) == -1){
                                /*receive p2's action*/
                                        failedReadP2();
                                }

                        }
                        if(read(playerTwo.parentOut[0], &playerTwo.count,
                                sizeof(short)) == -1){
                        /* Obtain the count of the second player */
                                        failedReadP2();
                        }


                        if(playerTwo.count > 21){
                        /*If player 2 busts*/
                                printf("Player 2 busts with %d\n",
                                        playerTwo.count
                                );
                                p2BlackJackOrBust = true;
                                playerTwo.loss++;
                                dealerPlayer.wins++;

                        }
                        else if(playerTwo.count == 21 && p2Count == 2){
                        /*If player 2 got a blackjack*/
                                printf("PLAYER 2 GOT A BLACKJACK!\n");
                                playerTwo.wins++;
                                dealerPlayer.loss++;
                                p2BlackJackOrBust = true;
                        }
                        /* Ended player Two turn. Dealer begins.*/

                        while(dealerPlayer.count < 17){
                        /*Drawing until 17 or more is reached */
                                addCard(&dealerPlayer, cardDeck[z]);
                                z++;
                        }
                        printf("Player 1: %d Player 2: %d Dealer: %d\n",
                        playerOne.count, playerTwo.count, dealerPlayer.count
                                );
                        /*Display the current hand values of the players */

                        if(p1BlackJackOrBust != true){
                        /*if Player 1 didnt bust or natural, then draw to 17*/
                                if(dealerPlayer.count >21){
                                        printf("Dealer busted, Player one wins\n");
                                        playerOne.wins++;
                                        dealerPlayer.loss++;
                                }
                                else{
                                        if(dealerPlayer.count > playerOne.count){
                                        /*if the dealer's hand is bigger, p1 lost*/
                                                printf("Dealer beat Player one\n");
                                                playerOne.loss++;
                                                dealerPlayer.wins++;
                                        }
                                        else if(dealerPlayer.count < playerOne.count){
                                        /*if p1 hand is bigger than dealer*/
                                                printf("Player One beat Dealer\n");
                                                playerOne.wins++;
                                                dealerPlayer.loss++;
                                        }
                                        else{
                                        /*otherwise, its a draw*/
                                                printf("Player One and Dealer"
                                                " tied: %d-%d\n",
                                                playerOne.count,
                                                dealerPlayer.count
                                                        );
                                        }

                                }

                        }

                        if(p2BlackJackOrBust != true){
                        /*if Player 2 didnt bust or natural, then draw to 17*/
                                if(dealerPlayer.count >21){
                                        printf("Dealer busted, Player Two wins\n");
                                        playerTwo.wins++;
                                        dealerPlayer.loss++;
                                }
                                else{
                                        if(dealerPlayer.count > playerTwo.count){
                                        /*if dealer hand is bigger than the p2 hand*/
                                                printf("Dealer beat Player Two\n");
                                                playerTwo.loss++;
                                                dealerPlayer.wins++;
                                        }
                                        else if(dealerPlayer.count < playerTwo.count){
                                        /*if the p2 hand is bigger than the dealer's*/
                                                printf("Player 2 beat Dealer\n");
                                                playerTwo.wins++;
                                                dealerPlayer.loss++;
                                        }
                                        else{
                                        /*Otherwise its a draw*/
                                               printf("Player Two and Dealer"
                                                " tied: %d-%d\n",
                                                playerTwo.count,
                                                dealerPlayer.count
                                                        );
                                        }

                                }

                        }

                        clearCards(&dealerPlayer);
                        /*Clear the dealer's hands*/


                }
                else if(userOne == 0){
                /* First user's strategy */

                        card d;
                        if( read(playerOne.parentIn[0], &d, sizeof(d)) < 0 ){
                        /*read the first initial dealt card*/
                                failedReadP1();
                        }

                        addCard(&playerOne, d);
                        /* First dealt card */

                        if(write(playerOne.sibling[1], &d, sizeof(d)) == -1){
                        /* Send first card to player two to count */
                                failedWriteP1();
                        }

                        card q;
                        if(read(playerOne.parentIn[0], &q, sizeof(q)) < 0 ){
                        /*read the second initial dealt card*/
                                failedReadP1();
                        }
                        addCard(&playerOne, q);
                        /* Second dealt card */

                        if(write(playerOne.sibling[1], &q, sizeof(q)) == -1){
                        /* Send send card to player two to count */
                                failedWriteP1();
                        }

                        processOne();
                        /* Begin thinking for player 1. Will return when player
                         * stands.
                         */

                }
                else if(userTwo == 0){
                /* Second user's strategy */
                        card d;
                        short tmpCount = 0;
                        if( read(playerTwo.parentIn[0], &d, sizeof(d)) < 0 ){
                        /*Receive first initial dealt card*/
                                failedReadP2();
                        }

                        addCard(&playerTwo, d);
                        /* First dealt card */
                        tmpCount += getCountedCardValue(d);
                        /*Get card count value*/

                        card q;
                        if( read(playerTwo.parentIn[0], &q, sizeof(q)) < 0 ){
                        /*Receive second initial dealt card*/
                                failedReadP2();
                        }

                        addCard(&playerTwo, q);
                        /*Second dealt card*/

                        tmpCount += getCountedCardValue(q);
                        /*Get card count value*/

                        /* Take player two's cards and create the "card count"
                         * value for them. Pass them into the processing
                         * function
                         */

                        processTwo(tmpCount);
                        /* Begin processing player two's strategy */



                }


        }


        close(playerOne.parentIn[0]);
        close(playerOne.parentIn[1]);
        close(playerTwo.parentIn[0]);
        close(playerTwo.parentIn[1]);
        close(playerOne.parentOut[0]);
        close(playerOne.parentOut[1]);
        close(playerTwo.parentOut[0]);
        close(playerTwo.parentOut[1]);
        close(playerTwo.sibling[0]);
        close(playerTwo.sibling[1]);
        close(dealerPlayer.parentOut[0]);
        close(dealerPlayer.parentOut[1]);
        /*Close all the opened pipes*/

        if(getpid() == dealer){
        /*Dealer will then discuss the results*/
                printf("\n\n\tCOMPLETED:\n"
                "Results: Player 1 Wins: %d Player 1 Losses: %d\n"
                "         Player 2 Wins: %d Player 2 Losses: %d\n",
                playerOne.wins, playerOne.loss, playerTwo.wins, playerTwo.loss
                        );

                printf("Dealer's Total Wins: %d Dealer's Total Losses: %d\n",
                        dealerPlayer.wins, dealerPlayer.loss);
        }

        exit(0);
}
/***************************************************************************
* void initialDeal(card* dCards)
* Author: SkibbleBip
* Date: 03/07/2021
* Description: Function to deal the first 2 cards to each hand
*
* Parameters:
*        dCards    I/P    card*    Pointer to the array that contains the
                                   6 initial cards that are dealt to the table
**************************************************************************/
void initialDeal(card* dCards)
/* Total of 6 cards will be dealt initially */
{
        if(write(playerOne.parentIn[1], &dCards[0], sizeof(card) )<0){
        /* Deal player1 first card */
                failedWriteP2();
        }
        if(write(playerTwo.parentIn[1], &dCards[1], sizeof(card) )<0){
        /* Deal player2 first card */
                failedWriteP2();
        }
        addCard(&dealer, dCards[2]);
        /* Deal first card to dealer */
        if(write(dealerPlayer.parentOut[1], &dCards[2], sizeof(dCards[2]))==-1){
        /* Send first dealer card to player two to count */
                failedDealerPipe();
        }

        if(write(playerOne.parentIn[1], &dCards[3], sizeof(card) )<0){
        /* Deal player1 second card */
                failedWriteP1();
        }

        if(write(playerTwo.parentIn[1], &dCards[4], sizeof(card) )<0){
        /* Deal player2 second card */
                failedWriteP2();
        }

        addCard(&dealer, dCards[5]);
        /* Deal second card to dealer */
        //if(write(dealerPlayer.parentOut[1], &dCards[5], sizeof(dCards[5]))==-1){
        /* Send second dealer card to player two to count */
         //       failedDealerPipe();
        //}


        /* All cards have been dealt. Begin processing */

}





