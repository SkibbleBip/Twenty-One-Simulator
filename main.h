/***************************************************************************
* File:  main.h
* Author:  SkibbleBip
* Procedures:
* shuffle       -Shuffles the deck of cards
* initCards     -Initializes the deck of cards and their values in order
***************************************************************************/

#include "user.h"
#include <time.h>

#ifndef MAIN_H
#define MAIN_H

card cardDeck[52];
/* Deck of cards*/


/***************************************************************************
* void shuffle(void)
* Author: SkibbleBip
* Date: 02/28/2021
* Description: Simple algorythm found from
*       https://www.geeksforgeeks.org/shuffle-a-deck-of-cards-3/ that takes
*       the current deck and shuffles it.
*
* Parameters: N/A
**************************************************************************/
void shuffle(void)
/* Inspired by https://www.geeksforgeeks.org/shuffle-a-deck-of-cards-3/ */
{
        srand(time(0));
        /* Set the seed time for random generation */
        int i;
        for(i = 0; i < 52; i++){
                int v = i + (rand() % (52 -i));
                /* Simple algorythm I found on geeksforgeeks for
                shuffling cards */

                card tmp = cardDeck[i];
                cardDeck[i] = cardDeck[v];
                cardDeck[v] = tmp;
        }


}


/***************************************************************************
* void initCards(void)
* Author: SkibbleBip
* Date: 02/28/2021
* Description: Function called on startup of the main function that will
*       initialize the default order of the card deck.
*
* Parameters: N/A
**************************************************************************/
void initCards(void)
/* Initialize the card deck in order*/
{
       int i;
       int j = 1;
       /* i is the current ID of the card,
          j is the face value of the card
        */

       for(i = 0; i< 10; i++){
        /* Loop through first 10 cards of clubs */
                cardDeck[i] = (card){i, j};
                j++;
       }
        cardDeck[10] = (card){10, 10}; /* Jack of Clubs */
        cardDeck[11] = (card){11, 10}; /* Queen of Clubs */
        cardDeck[12] = (card){12, 10}; /* King of Clubs */
        j = 1;

        for(i = 13; i < 23; i++){
        /* Loop through first 10 cards of diamonds */
                cardDeck[i] = (card){i, j};
                j++;
        }
        cardDeck[23] = (card){23, 10}; /* Jack of Diamonds */
        cardDeck[24] = (card){24, 10}; /* Queen of Diamonds */
        cardDeck[25] = (card){25, 10}; /* King of Diamonds */
        j = 1;

        for(i = 26; i < 36; i++){
        /* Loop through first 10 cards of hearts */
                cardDeck[i] = (card){i, j};
                j++;
        }
        cardDeck[36] = (card){36, 10}; /* Jack of Hearts */
        cardDeck[37] = (card){37, 10}; /* Queen of Hearts */
        cardDeck[38] = (card){38, 10}; /* King of Hearts */
        j = 1;

        for(i = 39; i < 49; i++){
        /* Loop through first 10 cards of spades */
                cardDeck[i] = (card){i, j};
                j++;
        }
        cardDeck[49] = (card){49, 10}; /* Jack of Spades */
        cardDeck[50] = (card){50, 10}; /* Queen of Spades */
        cardDeck[51] = (card){51, 10}; /* King of Spades */


}



#endif // MAIN_H

