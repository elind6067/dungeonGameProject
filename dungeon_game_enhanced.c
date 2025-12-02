/* student name: Aisha Muftau
   student name: Ella Lind
   date: 12/03/2025
   Introduction To Software Engineering
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Enumerations
typedef enum { 
    EMPTY, 
    MONSTER, 
    TREASURE, 
    TRAP,       // New: Traps that damage player
    HEALING,    // New: Healing fountains
    MERCHANT,   // New: Merchant to buy items
    BOSS        // New: Boss room at the end
} RoomType;

typedef enum {
    WEAPON,
    ARMOR,
    POTION
} ItemType;

// Structures
typedef struct Item {
    char name[30];
    ItemType type;
    int value;
    int cost;
    struct Item* next;
} Item;

typedef struct Room {
    RoomType type;
    int value;
    int difficulty;  // For progressive scaling
    struct Room *next;
    struct Room *previous;  // For backtracking
    Item* item;     // Item in the room (if any)
} Room;

typedef struct {
    char name[50];
    int strength;
    int max_strength;
    int health;
    int max_health;
    int coins;
    int level;
    int experience;
    Item* inventory;  // Linked list of items
    int weapon_bonus;
    int armor_bonus;
} Player;

// Function Prototypes
Room* createRoom(int index, int numRooms);
Room* createDungeon(int numRooms);
void deleteDungeon(Room *dungeon);
void displayRoom(const Room* room, int roomIndex);
void playerAction(Player* player, Room* room, int roomIndex);
void fightMonster(Player* player, int monsterStrength, int roomIndex, RoomType roomType);
void gameLoop(Player* player, Room *dungeonHead, int numRooms);
void playGame(int numRooms);
void initializePlayer(Player* player);
void showPlayerStatus(Player* player);
void levelUp(Player* player);
void showInventory(Player* player);
Item* createRandomItem(int roomIndex);
void addItemToInventory(Player* player, Item* item);
void usePotion(Player* player);
void visitMerchant(Player* player, int roomIndex);
void handleTrap(Player* player, int roomIndex);
void handleHealing(Player* player, int roomIndex);
void handleBossBattle(Player* player, int roomIndex);
void saveGame(Player* player, int roomIndex, int totalRooms);
int loadGame(Player* player, int* roomIndex, int* totalRooms);
void displayTitle();

// Global game states
int GAME_SAVED = 0;

// Main Function
int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    displayTitle();
    
    printf("=== DUNGEON AISHELLA - THE FORGOTTEN REALMS ===\n\n");
    
    int choice;
    printf("1. New Adventure\n");
    printf("2. Continue Journey\n");
    printf("3. Exit Realm\n");
    printf("Choose: ");
    scanf("%d", &choice);
    getchar();
    
    if (choice == 3) {
        printf("May your path be safe, traveler!\n");
        return 0;
    }
    
    if (choice == 2) {
        Player player;
        int loadedRoom = 0;
        int totalRooms = 0;
        if (loadGame(&player, &loadedRoom, &totalRooms)) {
            printf("Journey resumed successfully!\n");
            // Create dungeon and navigate to saved position
            Room* dungeonHead = createDungeon(totalRooms);
            Room* currentRoom = dungeonHead;
            for (int i = 0; i < loadedRoom && currentRoom; i++) {
                currentRoom = currentRoom->next;
            }
            if (currentRoom) {
                gameLoop(&player, currentRoom, totalRooms - loadedRoom);
            }
            deleteDungeon(dungeonHead);
            return 0;
        } else {
            printf("No saved journey found. Starting new adventure.\n");
        }
    }
    
    int numRooms;
    if (argc == 2) {
        numRooms = atoi(argv[1]);
    } else {
        printf("Enter dungeon depth (5-20 rooms): ");
        scanf("%d", &numRooms);
        getchar();
    }
    
    if (numRooms < 5) numRooms = 5;
    if (numRooms > 20) numRooms = 20;
    
    playGame(numRooms);
    return 0;
}

void displayTitle() {
    printf("\n");
    printf("############################################\n");
    printf("#  WELCOME TO DUNGEON AISHELLA ADVENTURE   #\n");
    printf("#     The Legend of Aishella's Keep        #\n");
    printf("############################################\n");
}

void initializePlayer(Player* player) {
    printf("Enter your adventurer's name: ");
    fgets(player->name, 50, stdin);
    player->name[strcspn(player->name, "\n")] = 0;
    
    printf("\nChoose your heritage:\n");
    printf("1. Warrior of Aisha (High strength, resilient)\n");
    printf("2. Rogue of Ella (Balanced and agile)\n");
    printf("3. Guardian of Aishella (High health, protective)\n");
    printf("Choose: ");
    
    int classChoice;
    scanf("%d", &classChoice);
    
    switch(classChoice) {
        case 1: // Warrior of Aisha
            printf("\nYou are a Warrior of Aisha - strong and unyielding!\n");
            player->strength = 30;
            player->health = 70;
            break;
        case 2: // Rogue of Ella
            printf("\nYou are a Rogue of Ella - agile and resourceful!\n");
            player->strength = 25;
            player->health = 80;
            break;
        case 3: // Guardian of Aishella
            printf("\nYou are a Guardian of Aishella - protective and enduring!\n");
            player->strength = 20;
            player->health = 100;
            break;
        default:
            player->strength = 25;
            player->health = 85;
    }
    
    player->max_strength = player->strength;
    player->max_health = player->health;
    player->coins = 100;
    player->level = 1;
    player->experience = 0;
    player->inventory = NULL;
    player->weapon_bonus = 0;
    player->armor_bonus = 0;
    
    // Give starting items
    Item* sword = malloc(sizeof(Item));
    strcpy(sword->name, "Rusty Sword of Aishella");
    sword->type = WEAPON;
    sword->value = 5;
    sword->cost = 0;
    sword->next = NULL;
    player->inventory = sword;
    player->weapon_bonus = 5;
    
    Item* potion = malloc(sizeof(Item));
    strcpy(potion->name, "Healing Elixir");
    potion->type = POTION;
    potion->value = 20;
    potion->cost = 0;
    potion->next = NULL;
    sword->next = potion;
    
    printf("\nYou begin your journey with 100 gold coins.\n");
    printf("Your equipment: Rusty Sword of Aishella and Healing Elixir\n");
    printf("Press Enter to enter the dungeon...");
    getchar();
}

void showPlayerStatus(Player* player) {
    printf("\n============================================\n");
    printf("          ADVENTURER'S STATUS              \n");
    printf("============================================\n");
    printf("Name:       %s\n", player->name);
    printf("Heritage:   Level %d\n", player->level);
    printf("Experience: %d/100 to next level\n", player->experience);
    printf("Health:     %d/%d\n", player->health, player->max_health);
    printf("Strength:   %d (+%d from weapon)\n", player->strength, player->weapon_bonus);
    printf("Defense:    +%d from armor\n", player->armor_bonus);
    printf("Gold:       %d coins\n", player->coins);
    printf("============================================\n");
}

Item* createRandomItem(int roomIndex) {
    Item* item = malloc(sizeof(Item));
    int rarity = rand() % 100;
    
    char* prefixes[] = {"Aishella's", "Ella's", "Ancient", "Enchanted", "Forgotten"};
    char* weaponNames[] = {"Blade", "Dagger", "Staff", "Mace", "Bow"};
    char* armorNames[] = {"Armor", "Shield", "Helmet", "Gauntlets", "Boots"};
    
    if (rarity < 50) { // Common
        strcpy(item->name, "Healing Potion");
        item->type = POTION;
        item->value = 20 + roomIndex * 2;
        item->cost = 30 + roomIndex * 3;
    } else if (rarity < 80) { // Uncommon
        char weaponName[30];
        sprintf(weaponName, "%s %s", prefixes[rand() % 5], weaponNames[rand() % 5]);
        strcpy(item->name, weaponName);
        item->type = WEAPON;
        item->value = 10 + roomIndex * 3;
        item->cost = 50 + roomIndex * 5;
    } else { // Rare
        char armorName[30];
        sprintf(armorName, "%s %s", prefixes[rand() % 5], armorNames[rand() % 5]);
        strcpy(item->name, armorName);
        item->type = ARMOR;
        item->value = 15 + roomIndex * 2;
        item->cost = 80 + roomIndex * 6;
    }
    
    item->next = NULL;
    return item;
}

Room* createRoom(int index, int numRooms) {
    Room *room = malloc(sizeof(Room));
    
    // Last room is always a BOSS
    if (index == numRooms - 1) {
        room->type = BOSS;
        room->value = 100 + index * 20; // Boss strength
    } else {
        // Progressive difficulty distribution
        int randVal = rand() % 100;
        int difficultyFactor = index * 100 / numRooms;
        
        if (randVal < 25) {
            room->type = MONSTER;
            room->value = 15 + difficultyFactor; // Monster strength
        } else if (randVal < 45) {
            room->type = TREASURE;
            room->value = 30 + difficultyFactor * 2; // Treasure value
        } else if (randVal < 55) {
            room->type = TRAP;
            room->value = 10 + difficultyFactor; // Trap damage
        } else if (randVal < 65) {
            room->type = HEALING;
            room->value = 20 + difficultyFactor; // Healing amount
        } else if (randVal < 75) {
            room->type = MERCHANT;
            room->value = 0;
        } else {
            room->type = EMPTY;
            room->value = 0;
        }
    }
    
    room->difficulty = index;
    room->next = NULL;
    room->previous = NULL;
    
    // Some rooms have random items
    if (room->type == TREASURE || room->type == EMPTY) {
        if (rand() % 100 < 30) { // 30% chance for item
            room->item = createRandomItem(index);
        } else {
            room->item = NULL;
        }
    } else {
        room->item = NULL;
    }
    
    return room;
}

Room* createDungeon(int numRooms) {
    Room *head = NULL, *current = NULL, *prev = NULL;
    for(int i = 0; i < numRooms; ++i) {
        if (i == 0) {
            head = current = createRoom(i, numRooms);
        } else {
            prev = current;
            current->next = createRoom(i, numRooms);
            current = current->next;
            current->previous = prev;
        }
        prev = current;
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

void displayRoom(const Room* room, int roomIndex) {
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|               CHAMBER %-3d                |\n", roomIndex + 1);
    printf("+------------------------------------------+\n");
    
    switch(room->type) {
        case MONSTER:
            printf("| A DREADFUL CREATURE BLOCKS YOUR PATH!   |\n");
            printf("| Creature Strength: %-3d                  |\n", room->value);
            break;
        case TREASURE:
            printf("| TREASURE VAULT!                         |\n");
            printf("| Gold Coins: %-4d                        |\n", room->value);
            if (room->item) {
                printf("| Also contains: %-22s |\n", room->item->name);
            }
            break;
        case TRAP:
            printf("| TRAP-INFESTED CHAMBER!                  |\n");
            printf("| Danger Level: %-3d                      |\n", room->value);
            break;
        case HEALING:
            printf("| ANCIENT HEALING SPRING                  |\n");
            printf("| Restoration: %-3d health points        |\n", room->value);
            break;
        case MERCHANT:
            printf("| WANDERING MERCHANT'S CAMP               |\n");
            printf("| Goods and wares for trade!              |\n");
            break;
        case BOSS:
            printf("| THRONE ROOM OF THE DUNGEON LORD         |\n");
            printf("| FINAL CONFRONTATION AWAITS              |\n");
            printf("| Boss Power: %-4d                        |\n", room->value);
            break;
        default:
            printf("| EMPTY CHAMBER                           |\n");
            if (room->item) {
                printf("| You spot: %-28s |\n", room->item->name);
            }
    }
    printf("+------------------------------------------+\n");
}

void showInventory(Player* player) {
    printf("\n+------------------------------------------+\n");
    printf("|               INVENTORY                  |\n");
    printf("+------------------------------------------+\n");
    
    if (player->inventory == NULL) {
        printf("| (Your pack is empty)                   |\n");
    } else {
        Item* current = player->inventory;
        int count = 1;
        while (current != NULL) {
            char type[10];
            switch(current->type) {
                case WEAPON: strcpy(type, "[Weapon]"); break;
                case ARMOR: strcpy(type, "[Armor]"); break;
                case POTION: strcpy(type, "[Potion]"); break;
            }
            printf("| %2d. %-20s %-10s |\n", 
                   count, current->name, type);
            current = current->next;
            count++;
        }
    }
    printf("+------------------------------------------+\n");
}

void addItemToInventory(Player* player, Item* item) {
    if (player->inventory == NULL) {
        player->inventory = item;
    } else {
        Item* current = player->inventory;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = item;
    }
    item->next = NULL;
}

void usePotion(Player* player) {
    Item* potion = NULL;
    Item* prev = NULL;
    Item* current = player->inventory;
    
    // Find first potion
    while (current != NULL) {
        if (current->type == POTION) {
            potion = current;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    if (potion == NULL) {
        printf("You have no healing potions!\n");
        return;
    }
    
    int healAmount = potion->value;
    player->health += healAmount;
    if (player->health > player->max_health) {
        player->health = player->max_health;
    }
    
    printf("You drink the %s and restore %d health!\n", potion->name, healAmount);
    
    // Remove potion from inventory
    if (prev == NULL) {
        player->inventory = potion->next;
    } else {
        prev->next = potion->next;
    }
    free(potion);
}

void visitMerchant(Player* player, int roomIndex) {
    printf("\nMerchant: \"Greetings, brave soul of Aishella!\"\n");
    printf("You have %d gold coins.\n", player->coins);
    
    // Generate 3 random items for sale
    Item* items[3];
    for (int i = 0; i < 3; i++) {
        items[i] = createRandomItem(roomIndex);
    }
    
    int choice;
    do {
        printf("\nMerchant's Wares:\n");
        for (int i = 0; i < 3; i++) {
            printf("%d. %s - %d gold", i + 1, items[i]->name, items[i]->cost);
            switch(items[i]->type) {
                case WEAPON: 
                    printf(" (Weapon +%d)\n", items[i]->value);
                    break;
                case ARMOR: 
                    printf(" (Armor +%d)\n", items[i]->value);
                    break;
                case POTION: 
                    printf(" (Heals %d HP)\n", items[i]->value);
                    break;
            }
        }
        printf("4. Trade your items\n");
        printf("5. Bid farewell to merchant\n");
        printf("Choose: ");
        scanf("%d", &choice);
        
        if (choice >= 1 && choice <= 3) {
            Item* selected = items[choice - 1];
            if (player->coins >= selected->cost) {
                player->coins -= selected->cost;
                
                // Add to inventory
                Item* boughtItem = malloc(sizeof(Item));
                memcpy(boughtItem, selected, sizeof(Item));
                boughtItem->next = NULL;
                addItemToInventory(player, boughtItem);
                
                printf("You acquire the %s!\n", selected->name);
                
                // Apply bonuses immediately if weapon/armor
                if (selected->type == WEAPON) {
                    player->weapon_bonus = selected->value;
                    printf("Your weapon bonus is now +%d!\n", selected->value);
                } else if (selected->type == ARMOR) {
                    player->armor_bonus = selected->value;
                    printf("Your armor bonus is now +%d!\n", selected->value);
                }
            } else {
                printf("The merchant shakes his head. \"Not enough gold, traveler.\"\n");
            }
        } else if (choice == 4) {
            printf("You trade some old gear for 50 gold.\n");
            player->coins += 50;
        }
    } while (choice != 5);
    
    printf("Merchant: \"Safe travels through Aishella's depths!\"\n");
    
    // Cleanup merchant items
    for (int i = 0; i < 3; i++) {
        free(items[i]);
    }
}

void handleTrap(Player* player, int roomIndex) {
    printf("\nCLICK! You trigger an ancient trap!\n");
    
    int trapDamage = rand() % (roomIndex + 10) + 5;
    printf("You suffer %d damage!\n", trapDamage);
    
    player->health -= trapDamage;
    
    // Check for armor reduction
    if (player->armor_bonus > 0) {
        int reduction = trapDamage / 4;
        if (reduction > 0) {
            printf("Your armor absorbs %d damage!\n", reduction);
            player->health += reduction;
        }
    }
    
    if (player->health <= 0) {
        printf("The trap proves fatal!\n");
    } else {
        printf("You manage to avoid further danger.\n");
    }
}

void handleHealing(Player* player, int roomIndex) {
    int healAmount = rand() % (roomIndex * 2 + 10) + 15;
    printf("\nYou drink from the ancient healing spring.\n");
    printf("The waters restore %d health!\n", healAmount);
    
    player->health += healAmount;
    if (player->health > player->max_health) {
        player->health = player->max_health;
    }
}

void levelUp(Player* player) {
    player->level++;
    player->experience = 0;
    
    printf("\n*** YOU HAVE ASCENDED TO LEVEL %d! ***\n", player->level);
    printf("The spirit of Aishella grants you a blessing:\n");
    printf("1. +10 Maximum Health (Resilience of Aisha)\n");
    printf("2. +5 Strength (Might of the Dungeon)\n");
    printf("3. +50 Gold (Wealth of Ella)\n");
    printf("Choose your blessing: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            player->max_health += 10;
            player->health += 10;
            printf("Your maximum health increases to %d!\n", player->max_health);
            break;
        case 2:
            player->strength += 5;
            player->max_strength += 5;
            printf("Your strength grows to %d!\n", player->strength);
            break;
        case 3:
            player->coins += 50;
            printf("You discover 50 additional gold coins!\n");
            break;
        default:
            player->max_health += 10;
            printf("Your maximum health increases to %d!\n", player->max_health);
    }
}

void fightMonster(Player* player, int monsterStrength, int roomIndex, RoomType roomType) {
    int totalStrength = player->strength + player->weapon_bonus;
    
    printf("\n========== BATTLE COMMENCES ==========\n");
    printf("Your might: %d (+%d weapon) = %d\n", 
           player->strength, player->weapon_bonus, totalStrength);
    printf("Foe's power: %d\n", monsterStrength);
    
    // Critical hit chance
    int critChance = rand() % 100;
    int isCritical = (critChance < 15); // 15% crit chance
    
    if (isCritical) {
        printf("*** MIGHTY BLOW! Double damage! ***\n");
        totalStrength *= 2;
    }
    
    if (totalStrength >= monsterStrength) {
        int expGain = monsterStrength * 2;
        int coinGain = monsterStrength * 3 + roomIndex * 2;
        
        printf("*** VICTORY! ***\n");
        printf("You gain %d experience and %d gold!\n", expGain, coinGain);
        
        player->experience += expGain;
        player->coins += coinGain;
        
        if (roomType == BOSS) {
            player->coins += 500; // Bonus for beating boss
            printf("*** DUNGEON LORD'S BOUNTY: 500 gold! ***\n");
        }
        
        if (player->experience >= 100) {
            levelUp(player);
        }
    } else {
        int damage = (monsterStrength - totalStrength) / 2;
        if (damage < 5) damage = 5;
        
        printf("*** DEFEAT! ***\n");
        printf("You suffer %d damage!\n", damage);
        
        player->health -= damage;
        
        if (player->health <= 0) {
            printf("You fall in battle...\n");
        } else {
            printf("You retreat to recover your strength...\n");
        }
    }
    printf("====================================\n");
}

void handleBossBattle(Player* player, int roomIndex) {
    printf("\n============================================\n");
    printf("    FINAL CONFRONTATION: DUNGEON LORD      \n");
    printf("============================================\n");
    printf("The air grows cold. Before you stands the\n");
    printf("ancient guardian of Aishella's deepest vault!\n");
    
    // Boss has multiple phases
    int bossHealth = 150 + roomIndex * 20;
    int phase = 1;
    
    while (bossHealth > 0 && player->health > 0) {
        printf("\n--- BATTLE PHASE %d ---\n", phase);
        printf("Dungeon Lord Health: %d\n", bossHealth);
        printf("Your Health: %d\n", player->health);
        
        printf("\nChoose your action:\n");
        printf("1. Standard Attack\n");
        printf("2. Heroic Strike (Costs 10 HP)\n");
        printf("3. Use Healing Item\n");
        printf("4. Attempt Retreat (50%% chance)\n");
        printf("Choose: ");
        
        int choice;
        scanf("%d", &choice);
        
        int playerDamage = player->strength + player->weapon_bonus;
        int bossDamage = 20 + phase * 5;
        
        switch(choice) {
            case 1: // Normal attack
                printf("You strike for %d damage!\n", playerDamage);
                bossHealth -= playerDamage;
                break;
            case 2: // Special attack
                if (player->health > 10) {
                    playerDamage *= 2;
                    player->health -= 10;
                    printf("You unleash a heroic strike for %d damage!\n", playerDamage);
                    bossHealth -= playerDamage;
                } else {
                    printf("You lack the vitality for such a strike!\n");
                }
                break;
            case 3: // Use potion
                usePotion(player);
                break;
            case 4: // Flee
                if (rand() % 100 < 50) {
                    printf("You break away from the battle!\n");
                    return;
                } else {
                    printf("The Dungeon Lord blocks your escape!\n");
                }
                break;
        }
        
        // Boss attacks back if still alive
        if (bossHealth > 0) {
            int actualDamage = bossDamage;
            if (player->armor_bonus > 0) {
                actualDamage -= player->armor_bonus;
                if (actualDamage < 5) actualDamage = 5;
            }
            
            printf("The Dungeon Lord retaliates for %d damage!\n", actualDamage);
            player->health -= actualDamage;
        }
        
        phase++;
    }
    
    if (bossHealth <= 0) {
        printf("\n============================================\n");
        printf("   *** LEGENDARY VICTORY! ***             \n");
        printf("============================================\n");
        printf("The Dungeon Lord falls! Aishella is cleansed!\n");
        printf("Your name shall be remembered, %s!\n", player->name);
    }
}

void saveGame(Player* player, int roomIndex, int totalRooms) {
    FILE* file = fopen("aishella_save.txt", "w");
    if (file) {
        fprintf(file, "%s\n", player->name);
        fprintf(file, "%d %d %d %d %d %d %d %d %d %d\n",
                player->strength, player->max_strength,
                player->health, player->max_health,
                player->coins, player->level,
                player->experience, player->weapon_bonus,
                player->armor_bonus, roomIndex);
        fprintf(file, "%d\n", totalRooms);
        fclose(file);
        printf("Journey saved in the Annals of Aishella!\n");
        GAME_SAVED = 1;
    } else {
        printf("Failed to preserve your journey.\n");
    }
}

int loadGame(Player* player, int* roomIndex, int* totalRooms) {
    FILE* file = fopen("aishella_save.txt", "r");
    if (file) {
        fgets(player->name, 50, file);
        player->name[strcspn(player->name, "\n")] = 0;
        
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d\n",
               &player->strength, &player->max_strength,
               &player->health, &player->max_health,
               &player->coins, &player->level,
               &player->experience, &player->weapon_bonus,
               &player->armor_bonus, roomIndex);
        fscanf(file, "%d\n", totalRooms);
        
        player->inventory = NULL; // Simplified - you could save inventory too
        
        fclose(file);
        return 1;
    }
    return 0;
}

void playerAction(Player* player, Room* room, int roomIndex) {
    int action;
    
    printf("\nYour course of action:\n");
    printf("1. Advance further\n");
    printf("2. Check your condition\n");
    printf("3. Use an item\n");
    printf("4. Chronicle your journey\n");
    
    if (room->type == MONSTER || room->type == BOSS) {
        printf("5. Engage in combat\n");
        if (room->type == MONSTER) {
            printf("6. Withdraw (%d gold toll)\n", roomIndex * 15);
        }
    }
    
    printf("Choose: ");
    scanf("%d", &action);
    
    switch(action) {
        case 1: // Proceed
            switch(room->type) {
                case MONSTER:
                    fightMonster(player, room->value, roomIndex, room->type);
                    break;
                case TREASURE:
                    player->coins += room->value;
                    printf("You discover %d gold coins! Total: %d\n", 
                           room->value, player->coins);
                    if (room->item) {
                        printf("You also find %s!\n", room->item->name);
                        addItemToInventory(player, room->item);
                        room->item = NULL;
                    }
                    break;
                case TRAP:
                    handleTrap(player, roomIndex);
                    break;
                case HEALING:
                    handleHealing(player, roomIndex);
                    break;
                case MERCHANT:
                    visitMerchant(player, roomIndex);
                    break;
                case BOSS:
                    handleBossBattle(player, roomIndex);
                    break;
                case EMPTY:
                    if (room->item) {
                        printf("Your search reveals %s!\n", 
                               room->item->name);
                        addItemToInventory(player, room->item);
                        room->item = NULL;
                    } else {
                        printf("The chamber yields nothing of value.\n");
                    }
                    break;
            }
            break;
            
        case 2: // Check Status
            showPlayerStatus(player);
            break;
            
        case 3: // Use Item
            usePotion(player);
            break;
            
        case 4: // Save Game
            saveGame(player, roomIndex, 0);
            break;
            
        case 5: // Fight (monster/boss)
            if (room->type == MONSTER || room->type == BOSS) {
                fightMonster(player, room->value, roomIndex, room->type);
            }
            break;
            
        case 6: // Flee (monster only)
            if (room->type == MONSTER) {
                int fleeCost = (roomIndex + 1) * 10;   
                printf("Flee cost: %d gold.\n", fleeCost);
                if (player->coins >= fleeCost) {
                    player->coins -= fleeCost;
                    printf("You pay %d gold to retreat safely!\n", fleeCost);
                } else {
                    printf("Insufficient gold! You must stand and fight!\n");
                    fightMonster(player, room->value, roomIndex, room->type);
                }
            }
            break;
    }
}

void gameLoop(Player* player, Room *dungeonHead, int numRooms) {
    Room *currentRoom = dungeonHead;
    int roomIndex = 0;
    
    while (currentRoom != NULL && player->health > 0) {
        showPlayerStatus(player);
        displayRoom(currentRoom, roomIndex);
        playerAction(player, currentRoom, roomIndex);
        
        if (player->health <= 0) {
            printf("\n*** YOUR JOURNEY ENDS HERE ***\n");
            printf("%s has fallen within Aishella's depths.\n", player->name);
            printf("You reached chamber %d and gathered %d gold.\n", 
                   roomIndex + 1, player->coins);
            printf("May others learn from your bravery.\n");
            break;
        }
        
        // Move to next room
        currentRoom = currentRoom->next;
        roomIndex++;
        
        if (currentRoom == NULL && player->health > 0) {
            printf("\n============================================\n");
            printf("   *** LEGEND OF AISHELLA COMPLETED! ***   \n");
            printf("============================================\n");
            printf("You have conquered all chambers of the dungeon!\n");
            printf("Final Tally: %d gold | Level: %d\n", 
                   player->coins, player->level);
            
            if (player->coins > 1000) {
                printf("*** MASTER OF AISHELLA'S VAULTS! ***\n");
            }
            printf("Your name shall be sung in taverns forever!\n");
        }
    }
}

void playGame(int numRooms) {
    Player player;
    initializePlayer(&player);
    
    printf("\nThe gates of Aishella's dungeon creak open...\n");
    printf("A %d-chamber descent into legend awaits.\n", numRooms);
    printf("Press Enter to begin your descent...");
    getchar();
    
    Room* dungeonHead = createDungeon(numRooms);
    
    gameLoop(&player, dungeonHead, numRooms);
    
    deleteDungeon(dungeonHead);
    
    printf("\n============================================\n");
    printf("Thank you for playing DUNGEON AISHELLA!\n");
    printf("============================================\n");
}