#  2 Player Multi-Process Cardgame Simulator
## Problem Statement

The assigned project was to create a program that will fork into 3 separate processes and simulate a game of Twenty-One, a cardgame where the goal is to be dealt a hand with a value as close as possible to 21 without going over and beating the dealer who is to draw until 17 is reached without going over 21. The program will use multiprocessing to simulate a table of 2 players each with their own strategy for winning, plus a dealer.
## ​Approach

To create 3 separate processes from the main program is to use the fork() method in the C language. Fork() is provided in the Linux header API and allows the operating system to create a new process using initialized data from the parent process that will then begin to run independent from the parent process. By taking and keeping note of the PID values of each process, we can make sure each process only runs specific instructions by reserving specific instructions to specific PID values. This is done by enclosing functions inside blocks of if statements.

The choice of communication method between the 3 processes was to use pipes. Pipes in the C language are represented as an array of ints with a length of 2. int pipe[0] is the input FIFO and int pipe[1] is the output pipe. Pipes are controlled by the operating system so any data written to a pipe is available to all processes. When a process reads a pipe, the data is removed off the stack in the FIFO and that information becomes unavailable to the other processes. One of the main features of pipes is if there is no data to be read, the process will wait until data becomes available. We can use this to synchronize the processes so they can both execute separately and wait when they need data to be sent or received from each other.

The choice of strategies for beating the dealer was to have one process attempt to draw until 17 (similar to that of the dealer) and the other process to attempt to try Casino Card Counting. To do this, each process sans the card counter will send copies of their dealt card to the card counter process. The act of card counting has an algorithm where cards of value 2 through 6 have a value of +1, 7 to 9 have a value of 0, and jack, queen, king, and ace have a value of -1. all the cards dealt will have these values added together to create a “score” which will deduce whether or not to stand or draw. The strategy chosen for the card counting process is to stand when the card count total is -2 or less. The simulation will run 1 million times to simulate each player’s strategy.
## Solution

To separate and store data for each process, a player struct is created for each virtual player. Each struct contains information such as the number of wins, losses, current hand value, and communication pipes. The struct calculates the hand value by using a boolean flag hadAce which when the player receives an ace, will add 11 instead of 1 and then set the flag to true. That way, if a value is added to the card which overflows the hand past 21 and the player has an ace in the hand, it will set the ace from a value of 11 to 1. The most important point of interest in these structs is the pipes the processes will use to communicate with each other. Each struct, and therefor player and process, contains 3 pipes: the parentIn, the parentOut, and the sibling pipe. The parentIn pipe is used to receive data from the dealer and the parentOut is used to send data from the player to the dealer such as choice of hitting or standing, or the current player card count. Only the player 1 process uses it’s sibling pipe, it is used to transmit it’s card types to the player 2 process so player 2 can attempt casino card counting. The dealer uses it’s parentOut pipe as a way to transmit it’s hand to the card counting process. The dealer does not use any of it’s pipes other than the parentOut pipe.

During each turn, the player processes send their choices and hand value through the parentOut pipes to the dealer and the dealer replies back the requested cards to the parentIn pipes. When both player processes stand, the dealer will take their hand values and award them either a win, a tie, or a loss depending on whether it busted or beat the dealer hand. The pipes keep the separate processes synchronized with each other as they will wait until there is data available in the pipes.
#### Compiling
    

Compiling the program is very simple, no external libraries need to be linked. The compiling statement is:

```
gcc -o main.o -c main.c

gcc -o user.o -c user.c

gcc -o Pgm2Base.out main.o user.o
```
## Results

Process player 1 had a rough win:loss ratio of 9.5:10, meaning there was a slightly less amount of wins than there were to losses. Player 2 had a much less win to loss ratio, almost 3:10. Player 1 had a much more successful strategy than player 2, even though the win amount was slightly lower than the loss amount. Each round of Twenty-One prints the current card hand values of each player and after the last round the dealer will print the wins to loss ratio. We are able to deduce the ratio of the wins to losses from each player through this prompt and can use this information that player 1’s strategy of hitting until a hand value of 17 or more is reached.


![figure 1](/Images/fig1.png) \
*Figure 1: Example of output while simulating* 

![figure 2](/Images/fig2.png) \
*Figure 2: Final output of program once simulation is complete*
