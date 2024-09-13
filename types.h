#ifndef TYPES_H 
#define TYPES_H

#include <stdbool.h>

#define BASE 0
#define START 1
#define FULL_CIRCLE 51
#define HOME_START 52
#define HOME_END 56
#define HOME 57

#define MYSTERY_DURATION 4 
#define NUM_TELEPORT_LOCATIONS 6

enum TeleportLocations {Bhawana, Kotuwa, Pita_Kotuwa, Teleport_Base, Teleport_X, Teleport_Approach};

struct mysteryCell {
    int location;
    int roundsActive;
    bool isActive;
};

struct piece {
    int loc;
    bool isAtBase;
    bool hasFinished;
    bool completedRound;
    int direction;
    int originalDirection;
    bool hasCaptured;
    int captureCount;
    int teleportEffect; 
    int effectRoundsLeft;
};

struct player {
    struct piece pieces[4];
};

struct player Players[4];

// Function prototypes
void printPlayer(int Player);
void printPlayerID(int Player, int pieceNumber);
int rollDie();
int coinToss();
void initializePlayers();
void teleportPiece(int player, int pieceNumber, int teleportLocation);
void updateMysteryCell();
bool isPieceOnMysteryCell(int player, int pieceNumber);
void applyMysteryCellEffect(int player, int pieceNumber);
void printPiecesOnBoardAndBase(int player);
void printPlayerStatus(int player);
bool isBlock(int loc, int player);
void captureBlockade(int capturingPlayer, int capturedPlayer);
void moveBlock(int player, int roll);
void breakBlockade(int player);
bool checkCapture(int currentPlayer, int targetLoc);
void capturePiece(int targetLoc);
bool movePiece(int player, int pieceNumber, int steps);

#endif