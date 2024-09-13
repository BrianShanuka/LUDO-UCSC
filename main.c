#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

// Global variables
struct mysteryCell MysteryCell;
struct player Players[4];


int main(){
    srand(48);

    initializePlayers();

    // Game Intro
    printf("\nThe green player has four (04) pieces named G1, G2, G3 and G4.\n");
    printf("The yellow player has four (04) pieces named Y1, Y2, Y3 and Y4.\n");
    printf("The blue player has four (04) pieces named B1, B2, B3 and B4.\n");
    printf("The red player has four (04) pieces named R1, R2, R3 and R4.\n");

    // Rolling the dice to determine the first player
    int max1 = 0;
    int max2 = 0;
    int maxplayer;

    while (1) {
        printf("\n");
        for (int i = 0; i < 4; i++) {
            int num = rollDie();
            if (max1 < num) {
                max1 = num;
                maxplayer = i;
            } else if (max1 == num) {
                max2 = num;
            }
            printPlayer(i);
            printf(" rolls %d\n", num);
        }
        if (max2 < max1) {
            printf("\n");
            break;
        }
        max1 = 0;
        max2 = 0;
        printf("\nTied! Rolling again.\n");
    }
    printPlayer(maxplayer);
    printf(" player has the highest roll and will begin the game.\n");
    printf("The order of a single round is ");
    printPlayer(maxplayer);

    int playerOrder[4];
    playerOrder[0] = maxplayer;

    for (int i = 0; i < 3; i++) {
        maxplayer++;
        if (maxplayer > 3) {
            maxplayer = 0;
        }
        if (i == 2) {
            printf(" and ");
        } else {
            printf(", ");
        }
        printPlayer(maxplayer);
        playerOrder[i + 1] = maxplayer;
    }
    printf("\n\n");

    int roundCount = 0;
    int sixRollCounter[4] = {0}; // Track consecutive six rolls for each player
    bool allFinished = false; // Track if all players have finished

    MysteryCell.isActive = false;
    MysteryCell.location = -1;
    MysteryCell.roundsActive = 0;

    while (!allFinished) {
        allFinished = true; // Assume all finished until proven otherwise

        for (int i = 0; i < 4; i++) {
                bool bonusRoll = true;
                while (bonusRoll) { // Loop to allow for bonus rolls
                    int roll = rollDie();
                    printPlayer(playerOrder[i]);
                    printf(" player rolled %d\n", roll);
                    
                    for (int j = 0; j < 4; j++){
                    // Apply movement effects
                        if (Players[i].pieces[j].teleportEffect == Bhawana && Players[i].pieces[j].effectRoundsLeft > 0) {
                            roll *= 2;
                            printf("Bhawana effect: Movement doubled! Moving %d steps.\n", roll);
                        } else if (Players[i].pieces[j].teleportEffect == Bhawana + 1 && Players[i].pieces[j].effectRoundsLeft > 0) {
                            roll /= 2;
                            printf("Bhawana effect: Movement halved! Moving %d steps.\n", roll);
                        }

                        applyMysteryCellEffect(i, j); // Apply mystery cell effect if applicable

                        // Reduce effect duration if applicable
                        if (Players[i].pieces[j].effectRoundsLeft > 0) {
                            Players[i].pieces[j].effectRoundsLeft--;
                            if (Players[i].pieces[j].effectRoundsLeft == 0) {
                                printf("Effect of Mystery Cell has worn off.\n");
                                Players[i].pieces[j].teleportEffect = -1; // Reset effect
                            }
                        }
                    }

                    // Choose which piece to move
                    bool captureOccurred = false;
                    for (int j = 0; j < 4; j++) {
                        if (!Players[playerOrder[i]].pieces[j].hasFinished) {
                            captureOccurred = movePiece(playerOrder[i], j, roll);
                            break;
                        }
                    }

                    // Check if there are consecutive sixes
                    if (roll == 6) {
                        sixRollCounter[playerOrder[i]]++; // Increment the counter for the current player
                    } else {
                        sixRollCounter[playerOrder[i]] = 0; // Reset the counter if not six
                    }

                    if (sixRollCounter[playerOrder[i]] >= 3) {
                        // Logic to break the blockade
                        breakBlockade(playerOrder[i]); // Call a function to handle blockade breaking
                        sixRollCounter[playerOrder[i]] = 0; // Reset counter after breaking the blockade
                    }

                    // Allowing bonus roll if a capture occured
                    bonusRoll = captureOccurred;
                }
        }

        // Check if all players have finished all their pieces
        for (int i = 0; i < 4; i++) {
            bool hasUnfinishedPiece = false;
            for (int j = 0; j < 4; j++) {
                if (!Players[i].pieces[j].hasFinished) {
                    hasUnfinishedPiece = true;
                    break;
                }
            }
            if (hasUnfinishedPiece) {
                allFinished = false;
                break;
            }
        }

        roundCount++;
        printf("\nRound %d completed.\n", roundCount);

        // Print status of each player after the round
        for (int i = 0; i < 4; i++) {
            printPlayerStatus(i);
        }
    }

    // Output the final standings
    int standings[4] = {0}; // To track finishing order
    for (int i = 0; i < 4; i++) {
        int finishedCount = 0;
        for (int j = 0; j < 4; j++) {
            if (Players[i].pieces[j].hasFinished) {
                finishedCount++;
            }
        }
        standings[i] = finishedCount;
    }

    // Sort standings in descending order (most finished pieces first)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i; j++) {
            if (standings[j] < standings[j + 1]) {
                int temp = standings[j];
                standings[j] = standings[j + 1];
                standings[j + 1] = temp;

                // Also swap playerOrder to maintain the connection between standings and players
                temp = playerOrder[j];
                playerOrder[j] = playerOrder[j + 1];
                playerOrder[j + 1] = temp;
            }
        }
    }

    // Print the final positions with 1st, 2nd, 3rd, 4th
    printf("\nFinal standings:\n");
    for (int i = 0; i < 4; i++) {
        // Handling ties
        if (i > 0 && standings[i] == standings[i - 1]) {
            printf("Tied for "); 
        } else {
            switch(i) {
                case 0: printf("1st: "); break;
                case 1: printf("2nd: "); break;
                case 2: printf("3rd: "); break;
                case 3: printf("4th: "); break;
            }
        }

        printPlayer(playerOrder[i]);
        printf(" player finished %d pieces\n", standings[i]);
    }

    return 0;
}