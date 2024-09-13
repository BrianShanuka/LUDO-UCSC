#include <stdio.h>
#include <stdlib.h>
#include "types.h"

extern struct mysteryCell MysteryCell;
extern struct player Players[4];

void printPlayer(int Player) {
    if(Player == 0) {printf("Green");}
    else if(Player == 1) {printf("Yellow");}
    else if(Player == 2) {printf("Blue");}
    else if(Player == 3) {printf("Red");}
}

void printPlayerID(int Player, int pieceNumber) {
    if(Player == 0) {printf("G%d", pieceNumber);}
    else if(Player == 1) {printf("Y%d", pieceNumber);}
    else if(Player == 2) {printf("B%d", pieceNumber);}
    else if(Player == 3) {printf("R%d", pieceNumber);}
}

int rollDie() {
    return (rand() % 6) + 1;
}

int coinToss() {
    return rand() % 2;
}

// Set defaults to start the game
void initializePlayers() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Players[i].pieces[j].loc = BASE;
            Players[i].pieces[j].isAtBase = true;
            Players[i].pieces[j].hasFinished = false;
            Players[i].pieces[j].completedRound = false;
            Players[i].pieces[j].direction = 1;
            Players[i].pieces[j].originalDirection = 1;
            Players[i].pieces[j].hasCaptured = false;
            Players[i].pieces[j].captureCount = 0;
            Players[i].pieces[j].teleportEffect = -1;
            Players[i].pieces[j].effectRoundsLeft = 0;
        }
    }
}

// Teleport logic for mystery cell
void teleportPiece(int player, int pieceNumber, int teleportLocation) {
    struct piece* currentPiece = &Players[player].pieces[pieceNumber];

    switch (teleportLocation) {
        case Bhawana:
            printf("Teleported to Bhawana. Effect: ");
            if (rand() % 2 == 0) {
                printf("Energized (Movement doubled for 4 rounds).\n");
                currentPiece->teleportEffect = Bhawana;
            } else {
                printf("Sick (Movement halved for 4 rounds).\n");
                currentPiece->teleportEffect = Bhawana + 1;
            }
            break;
        case Kotuwa:
            printf("Teleported to Kotuwa. Cannot move for 4 rounds. Rolls three consecutively = teleported to base.\n");
            currentPiece->teleportEffect = Kotuwa;
            break;
        case Pita_Kotuwa:
            printf("Teleported to Pita-Kotuwa. Effect: Changes direction if moving clockwise, teleports to Kotuwa if counterclockwise.\n");
            currentPiece->teleportEffect = Pita_Kotuwa;
            break;
        case Teleport_Base:
            printf("Teleported to Base.\n");
            currentPiece->loc = BASE;
            currentPiece->isAtBase = true;
            currentPiece->teleportEffect = -1; // No effect
            break;
        case Teleport_X:
            printf("Teleported to X of the piece color.\n");
            currentPiece->loc = START;
            break;
        case Teleport_Approach:
            printf("Teleported to Approach of the piece color.\n");
            currentPiece->loc = FULL_CIRCLE - 6; // Approximate Approach location
            break;
    }

    currentPiece->effectRoundsLeft = MYSTERY_DURATION; // Set effect duration
}

void updateMysteryCell() {
    if (MysteryCell.isActive) {
        MysteryCell.roundsActive--;

        if (MysteryCell.roundsActive == 0) {
            // Deactivate the current mystery cell
            MysteryCell.isActive = false;
            MysteryCell.location = -1;

            printf("\nThe Mystery Cell has disappeared!\n");
        }
    } else {
        // Generate a new Mystery Cell location if not currently active
        MysteryCell.location = rand() % FULL_CIRCLE + 1; // Random location on the board
        MysteryCell.isActive = true;
        MysteryCell.roundsActive = MYSTERY_DURATION;

        printf("\nA Mystery Cell has appeared at L%d!\n", MysteryCell.location);
    }
}

bool isPieceOnMysteryCell(int player, int pieceNumber) {
    return Players[player].pieces[pieceNumber].loc == MysteryCell.location;
}

void applyMysteryCellEffect(int player, int pieceNumber) {
    if (isPieceOnMysteryCell(player, pieceNumber)) {
        int teleportLocation = rand() % NUM_TELEPORT_LOCATIONS;
        printf("Piece ");
        printPlayerID(player, pieceNumber + 1);
        printf(" has landed on the Mystery Cell and will be teleported!\n");

        teleportPiece(player, pieceNumber, teleportLocation);
    }
}

void printPiecesOnBoardAndBase(int player) {
    int onBoard = 0, onBase = 0;
    for(int i = 0; i < 4; i++) {
        if(Players[player].pieces[i].isAtBase) {
            onBase++;
        } else {
            onBoard++;
        }
    }
    printPlayer(player);
    printf(" player now has %d/4 pieces on the board and %d/4 pieces on the base.\n", onBoard, onBase);
}

void printPlayerStatus(int player) {
    printPiecesOnBoardAndBase(player);

    printf("======================================================================\n");
    printf("Location of pieces ");
    printPlayer(player);
    printf("\n======================================================================\n");

    for (int i = 0; i < 4; i++) {
        if (!Players[player].pieces[i].isAtBase) {
            printPlayerID(player, i + 1);
            if (Players[player].pieces[i].loc == 57) {
                printf(" is at HOME\n");
            } else if (Players[player].pieces[i].loc == 0) {
                printf(" is at BASE\n");
            } else {
                printf(" is at L%d\n", Players[player].pieces[i].loc);
            }
        }
    }
    printf("\n");
}

bool isBlock(int loc, int player){
    int count = 0;
    for(int i = 0; i < 4; i++){
        if(Players[player].pieces[i].loc == loc){
            count++;
        }
    }
    return count >= 2;
}

void captureBlockade(int capturingPlayer, int capturedPlayer) {
    for (int i = 0; i < 4; i++) { // Loop through each piece of the capturing player
        for (int j = 0; j < 4; j++) { // Loop through each piece of the captured player
            if (Players[capturedPlayer].pieces[j].loc == Players[capturingPlayer].pieces[i].loc && !Players[capturedPlayer].pieces[j].isAtBase) {
                // Reset the captured piece's information
                Players[capturedPlayer].pieces[j].loc = BASE; // Send back to base
                Players[capturedPlayer].pieces[j].isAtBase = true;
                Players[capturedPlayer].pieces[j].hasFinished = false;
                Players[capturedPlayer].pieces[j].direction = 1; // Reset to default direction
                Players[capturedPlayer].pieces[j].originalDirection = 1; // Reset to default original direction
                Players[capturedPlayer].pieces[j].hasCaptured = false; // Reset capture flag
                Players[capturedPlayer].pieces[j].captureCount = 0; // Reset capture count
                
                // Increment capture count for each participating piece
                Players[capturingPlayer].pieces[i].captureCount++;
                
                printPlayer(capturedPlayer);
                printf(" player’s piece ");
                printPlayerID(capturedPlayer, j + 1);
                printf(" was captured and sent back to the base.\n");
            }
        }
    }
    printPlayer(capturingPlayer);
    printf(" player’s blockade captured the blockade of player ");
    printPlayer(capturedPlayer);
    printf(".\n");
}

void moveBlock(int player, int roll) {
    int maxDistance = -1;
    int direction = 1; // Default to clockwise
    int pieceDirection = 1; // Direction of the first piece in the block
    
    // Find the block with the maximum distance
    for (int i = 0; i < 4; i++) {
        if (Players[player].pieces[i].loc != BASE && !Players[player].pieces[i].hasFinished) {
            int distance = Players[player].pieces[i].loc;
            if (distance > maxDistance) {
                maxDistance = distance;
                pieceDirection = Players[player].pieces[i].originalDirection; // Track the direction of the first piece in the block
            }
        }
    }
    
    // Determine the direction of movement
    if (maxDistance < (HOME - maxDistance) / 2) {
        direction = -1; // Move counterclockwise if it's closer to HOME
    }
    
    // Move all pieces in the block
    for (int i = 0; i < 4; i++) {
        if (Players[player].pieces[i].loc == maxDistance) {
            // Handle capturing another blockade
            for (int j = 0; j < 4; j++) {
                if (isBlock(Players[player].pieces[i].loc + (roll / 2) * direction, (player + 1) % 4)) {
                    captureBlockade(player, (player + 1) % 4); // Capture the blockade
                }
            }

            // Move the piece
            Players[player].pieces[i].loc += (roll / 2) * direction;
            if (Players[player].pieces[i].loc >= HOME) {
                Players[player].pieces[i].loc = HOME;
                Players[player].pieces[i].hasFinished = true;
                printPlayer(player);
                printf(" player’s piece ");
                printPlayerID(player, i + 1);
                printf(" has finished the game!\n");
            } else if (Players[player].pieces[i].loc <= BASE) {
                Players[player].pieces[i].loc += HOME; // Wrap around the board
            }
            Players[player].pieces[i].direction = pieceDirection; // Set the direction of the block pieces to the original direction
        }
    }
    
    printPlayer(player);
    printf(" player’s block moves by %d units in %s direction.\n",
           roll / 2, direction == 1 ? "clockwise" : "counterclockwise");
}

void breakBlockade(int player) {
    int piecesMoved = 0;
    for (int i = 0; i < 4; i++) {
        if (!Players[player].pieces[i].isAtBase && !Players[player].pieces[i].hasFinished) {
            if (piecesMoved < 3) {
                // Move the piece in its original direction
                int moveDistance = 6 * Players[player].pieces[i].direction;
                Players[player].pieces[i].loc += moveDistance;
                if (Players[player].pieces[i].loc >= HOME) {
                    Players[player].pieces[i].loc = HOME;
                    Players[player].pieces[i].hasFinished = true;
                    printPlayer(player);
                    printf(" player’s piece ");
                    printPlayerID(player, i + 1);
                    printf(" has finished the game!\n");
                }
                if (Players[player].pieces[i].loc <= BASE) {
                    Players[player].pieces[i].loc += HOME; // Wrap around the board
                }
                piecesMoved++;
            } else {
                break;
            }
        }
    }
    // Reset direction for remaining pieces
    for (int i = 0; i < 4; i++) {
        if (!Players[player].pieces[i].isAtBase && !Players[player].pieces[i].hasFinished) {
            Players[player].pieces[i].direction = Players[player].pieces[i].originalDirection;
        }
    }
    printPlayer(player);
    printf(" player’s blockade was broken. Pieces were moved by 6 units cumulatively.\n");
}

bool checkCapture(int currentPlayer, int targetLoc){
    // Check if any opponent's piece is at the target location
    for(int i = 0; i < 4; i++){ // Loop through all players
        if(i != currentPlayer){ // Only check opponents
            for(int j = 0; j < 4; j++){ // Loop through each piece of the opponent
                if(Players[i].pieces[j].loc == targetLoc && !Players[i].pieces[j].isAtBase){
                    return true; // Capture possible
                }
            }
        }
    }
    return false; // No capture possible
}

void capturePiece(int targetLoc) {
    for (int i = 0; i < 4; i++) { // Loop through all players
        for (int j = 0; j < 4; j++) { // Loop through each piece
            if (Players[i].pieces[j].loc == targetLoc && !Players[i].pieces[j].isAtBase) {
                // Reset the captured piece's information
                Players[i].pieces[j].loc = BASE; // Send back to base
                Players[i].pieces[j].isAtBase = true;
                Players[i].pieces[j].hasFinished = false;
                Players[i].pieces[j].direction = 1; // Reset to default direction
                Players[i].pieces[j].originalDirection = 1; // Reset to default original direction
                Players[i].pieces[j].hasCaptured = false; // Reset capture flag
                Players[i].pieces[j].captureCount = 0; // Reset capture count
                
                printPlayer(i);
                printf(" player’s piece ");
                printPlayerID(i, j + 1);
                printf(" was captured and sent back to the base.\n");
                return; // Exit once the piece is captured
            }
        }
    }
}

bool movePiece(int player, int pieceNumber, int steps) {
    struct piece* currentPiece = &Players[player].pieces[pieceNumber];
    bool captureOccurred = false; // Track if a capture happened

    currentPiece->hasCaptured = false;

    if (currentPiece->isAtBase) {
        if (steps == 6) {
            currentPiece->loc = START; // Move the piece to the starting position
            currentPiece->isAtBase = false;
            printPlayer(player);
            printf(" player moves piece ");
            printPlayerID(player, pieceNumber + 1);
            printf(" to the starting point.\n");
            
            int toss = coinToss();
            if (toss == 0) {
                currentPiece->direction = 1; // Clockwise
                currentPiece->originalDirection = 1;
                printf("Coin toss result: Heads - Clockwise direction\n");
            } else {
                currentPiece->direction = -1; // Counterclockwise
                currentPiece->originalDirection = -1;
                printf("Coin toss result: Tails - Counterclockwise direction\n");
            }

            printPiecesOnBoardAndBase(player);
        }
    } else {
        int prevLoc = currentPiece->loc; // Store previous location
        int targetLoc = currentPiece->loc + (steps * currentPiece->direction); // Calculate the target location
        
        // Wrap the target location for circular movement
        if (currentPiece->direction == 1) { // Clockwise
            if (targetLoc > FULL_CIRCLE) {
                targetLoc -= FULL_CIRCLE;
                currentPiece->completedRound = true; // Mark that the piece completed a round
            }
            if (targetLoc >= HOME) {
                targetLoc = HOME; // The piece has finished the game
                currentPiece->hasFinished = true;
                printPlayer(player);
                printf(" player’s piece ");
                printPlayerID(player, pieceNumber + 1);
                printf(" has finished the game!\n");
            }
        } else if (currentPiece->direction == -1) { // Counterclockwise
            if (targetLoc < BASE) {
                if (targetLoc == BASE - 1) {
                    targetLoc = HOME_START; // Move to L53
                } else if (targetLoc == HOME_START - 1) {
                    targetLoc = HOME_END; // Move to L52
                } else if (targetLoc < BASE - 2) {
                    targetLoc += FULL_CIRCLE; // Wrap around the board
                }
                currentPiece->completedRound = true; // Mark that the piece completed a round
            }
            if (targetLoc <= BASE) {
                targetLoc = HOME; // Move to HOME directly
                currentPiece->hasFinished = true;
                printPlayer(player);
                printf(" player’s piece ");
                printPlayerID(player, pieceNumber + 1);
                printf(" has finished the game!\n");
            }
        }

        // Check if the piece has completed a round
        if (currentPiece->completedRound) {
            currentPiece->direction = 1;
            currentPiece->originalDirection = 1;
            targetLoc = currentPiece->loc + steps;
            if (targetLoc >= HOME_START) {
                if (targetLoc >= HOME) {
                    targetLoc = HOME; // The piece has finished the game
                    currentPiece->hasFinished = true;
                    printPlayer(player);
                    printf(" player’s piece ");
                    printPlayerID(player, pieceNumber + 1);
                    printf(" has finished the game!\n");
                } else {
                    currentPiece->loc = targetLoc;
                    printPlayer(player);
                    printf(" player’s piece ");
                    printPlayerID(player, pieceNumber + 1);
                    printf(" moves towards HOME: L%d\n", targetLoc);
                }
                return captureOccurred; // No further movement needed after reaching Home
            }
        }
        
        // Check for capture
        if (checkCapture(player, targetLoc)) {
            capturePiece(targetLoc);
            currentPiece->hasCaptured = true; // Set the capture flag
            captureOccurred = true; // Mark that a capture occurred
        }

        // Update piece's location
        currentPiece->loc = targetLoc;
        
        // Movement details
        printPlayer(player);
        printf(" moves piece ");
        printPlayerID(player, pieceNumber + 1);
        printf(" from location L%d to L%d by %d units in %s direction.\n", 
               prevLoc, currentPiece->loc, steps, 
               currentPiece->direction == 1 ? "clockwise" : "counterclockwise");
    }
    
    return captureOccurred;
}