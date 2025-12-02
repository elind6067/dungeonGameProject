#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Enumerations
typedef enum { EMPTY, MONSTER, TREASURE } RoomType;

// Structures
typedef struct Room {
    RoomType type;
    int value;
    struct Room *next;
} Room;

typedef struct {
    char* name;
    int strength;
    int health;
} Player;

// Function Prototypes
Room* createRoom(int index);
Room* createDungeon(int numRooms);
void deleteDungeon(Room *dungeon);
void displayRoom(const Room* room);
void playerAction(Player* player, Room* room);
void fightMonster(Player* player, int monsterStrength);
void gameLoop(Player* player, Room *dungeonHead);
void playGame(int numRooms);

// Main Function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_rooms>\n", argv[0]);
        return 1;
    }

    int numRooms = atoi(argv[1]);
    if (numRooms <= 0) {
        printf("Number of rooms should be a positive integer.\n");
        return 1;
    }

    playGame(numRooms);

    return 0;
}

// Function Implementations
Room* createRoom(int index) {
    Room *room = malloc(sizeof(Room));
    room->type = rand() % 3; // Randomly assign room type
    // Randomly assign room value based on room type 
    // (10-60 for coins treasure, 10-50 strength for monster)
    room->value = (room->type == MONSTER) ? (rand() % 5 + 1) * 10 : (rand() % 50 + 10);
    room->next = NULL;
    return room;
}

Room* createDungeon(int numRooms) {
    Room *head = NULL, *current = NULL;
    for(int i = 0; i < numRooms; ++i) {
        if (i == 0) {
            head = current = createRoom(i);
        } else {
            current->next = createRoom(i);
            current = current->next;
        }
    }
    return head;
}

void deleteDungeon(Room *dungeon) {
    while (dungeon != NULL) {
        Room *temp = dungeon;
        dungeon = dungeon->next;
        free(temp);
    }
}

void displayRoom(const Room* room) {
    const char* roomType = room->type == EMPTY ? "an empty room"
                             : room->type == MONSTER ? "a room with a monster"
                             : "a room with treasure";
    printf("You enter %s.\n", roomType);
}

void playerAction(Player* player, Room* room) {
    switch (room->type) {
        case MONSTER:
            fightMonster(player, room->value);
            break;
        case TREASURE:
            printf("You found a treasure worth %d coins!\n", room->value);
            break;
        case EMPTY:
            printf("There's nothing here...\n");
            break;
    }
}

void fightMonster(Player* player, int monsterStrength) {
    // Simple fight mechanism: if player's strength is higher, they win.
    printf("A monster appears! It has a strength of %d!\n", monsterStrength);
    if (player->strength >= monsterStrength) {
        printf("You defeated the monster!\n");
    } else {
        printf("The monster overpowers you. You flee back to the previous room.\n");
        player->health -= monsterStrength;
        if (player->health <= 0) {
            printf("You have died...\n");
            return;
        } else {
            printf("You survived the encounter, but you are wounded.\n");
        }
    }
}

void gameLoop(Player* player, Room *dungeonHead) {
    Room *currentRoom = dungeonHead;
    int roomIndex = 0;
    while (currentRoom != NULL && player->health > 0) {
        printf("You are in room %d.\n", roomIndex + 1);
        displayRoom(currentRoom);
        playerAction(player, currentRoom);
        if (player->health <= 0) {
            printf("%s has fallen in battle...\n", player->name);
            break;
        }
        currentRoom = currentRoom->next;
        roomIndex++;
    }
    if (player->health > 0) {
        printf("Congratulations, %s! You've cleared the dungeon!\n", player->name);
    }
}

void playGame(int numRooms) {
    srand((unsigned int)time(NULL)); // Seed the random number generator

    Room *dungeonHead = createDungeon(numRooms); // Create the dungeon

    Player player = {"Adventurer", 20, 100}; // Create a player with 20 strength and 100 health

    gameLoop(&player, dungeonHead); // Start the game loop

    deleteDungeon(dungeonHead); // Cleanup
}
