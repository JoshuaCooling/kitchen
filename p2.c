#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define MAX_BAKERS 6
#define NUM_RECIPES 5

// Semaphores for kitchen resources
sem_t mixer, pantry, refrigerator, bowl, spoon, oven, ingredient_mutex;

// Resource slots
int mixer_slots[2] = {0};
int refrigerator_slots[2] = {0};
int bowl_slots[3] = {0};
int spoon_slots[5] = {0};

// Mutexes to protect slot arrays
pthread_mutex_t mixer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fridge_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bowl_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t spoon_mutex = PTHREAD_MUTEX_INITIALIZER;

// ANSI color codes for output
char *colors[] = {"\033[1;31m", "\033[1;32m", "\033[1;33m", "\033[1;34m", "\033[1;35m", "\033[1;36m"};
#define RESET_COLOR "\033[0m"

// Recipes and ingredients
const char *recipes[NUM_RECIPES] = {"Cookies", "Pancakes", "Pizza Dough", "Soft Pretzels", "Cinnamon Rolls"};

const char *ingredients[NUM_RECIPES][8] = {
    {"Flour", "Sugar", "Milk", "Butter", NULL},
    {"Flour", "Sugar", "Baking soda", "Salt", "Egg", "Milk", "Butter", NULL},
    {"Yeast", "Sugar", "Salt", NULL},
    {"Flour", "Sugar", "Salt", "Yeast", "Baking Soda", "Egg", NULL},
    {"Flour", "Sugar", "Salt", "Butter", "Egg", "Cinnamon", NULL}
};

// Ramsied baker
int ramsied_baker = -1;

void log_baker(int id, const char *msg) {
    printf("%sBaker %d: %s%s\n", colors[id % 6], id, msg, RESET_COLOR);
    fflush(stdout);
}

int acquire_slot(int *slots, int size, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    for (int i = 0; i < size; i++) {
        if (slots[i] == 0) {
            slots[i] = 1;
            pthread_mutex_unlock(mutex);
            return i;
        }
    }
    pthread_mutex_unlock(mutex);
    return -1;
}

void release_slot(int *slots, int index, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    slots[index] = 0;
    pthread_mutex_unlock(mutex);
}

void acquire_ingredient(int id, const char *item) {
    sem_wait(&ingredient_mutex); // ensure exclusive ingredient access
    if (strcmp(item, "Egg") == 0 || strcmp(item, "Milk") == 0 || strcmp(item, "Butter") == 0) {
        sem_wait(&refrigerator);
        int fridge_id = acquire_slot(refrigerator_slots, 2, &fridge_mutex);
        char msg[128];
        snprintf(msg, sizeof(msg), "Opened refrigerator #%d for %s", fridge_id, item);
        log_baker(id, msg);
        snprintf(msg, sizeof(msg), "Accessed %s from refrigerator #%d", item, fridge_id);
        log_baker(id, msg);
        snprintf(msg, sizeof(msg), "Closed refrigerator #%d", fridge_id);
        log_baker(id, msg);
        release_slot(refrigerator_slots, fridge_id, &fridge_mutex);
        sem_post(&refrigerator);
    } else {
        sem_wait(&pantry);
        char msg[128];
        snprintf(msg, sizeof(msg), "Accessed pantry for %s", item);
        log_baker(id, msg);
        snprintf(msg, sizeof(msg), "Closed pantry after accessing %s", item);
        log_baker(id, msg);
        sem_post(&pantry);
    }
    sem_post(&ingredient_mutex);
}

void* baker_thread(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_RECIPES; i++) {
        retry:
        char msg[128];
        snprintf(msg, sizeof(msg), "Starting recipe: %s", recipes[i]);
        log_baker(id, msg);

        // Access ingredients
        for (int j = 0; ingredients[i][j] != NULL; j++) {
            acquire_ingredient(id, ingredients[i][j]);
            if (id == ramsied_baker && rand() % 12 == 0) {
                log_baker(id, "RAMSIED! Restarting recipe.");
                goto retry;
            }
        }

        // Acquire bowl
        sem_wait(&bowl);
        int bowl_id = acquire_slot(bowl_slots, 3, &bowl_mutex);
        snprintf(msg, sizeof(msg), "Acquired bowl #%d", bowl_id);
        log_baker(id, msg);

        // Acquire spoon
        sem_wait(&spoon);
        int spoon_id = acquire_slot(spoon_slots, 5, &spoon_mutex);
        snprintf(msg, sizeof(msg), "Acquired spoon #%d", spoon_id);
        log_baker(id, msg);

        // Acquire mixer
        sem_wait(&mixer);
        int mixer_id = acquire_slot(mixer_slots, 2, &mixer_mutex);
        snprintf(msg, sizeof(msg), "Acquired mixer #%d", mixer_id);
        log_baker(id, msg);

        log_baker(id, "Mixing ingredients...");
        sleep(1);
        log_baker(id, "Ingredients mixed!");

        // Release mixing tools
        snprintf(msg, sizeof(msg), "Released mixer #%d", mixer_id);
        log_baker(id, msg);
        release_slot(mixer_slots, mixer_id, &mixer_mutex);
        sem_post(&mixer);

        snprintf(msg, sizeof(msg), "Released spoon #%d", spoon_id);
        log_baker(id, msg);
        release_slot(spoon_slots, spoon_id, &spoon_mutex);
        sem_post(&spoon);

        snprintf(msg, sizeof(msg), "Released bowl #%d", bowl_id);
        log_baker(id, msg);
        release_slot(bowl_slots, bowl_id, &bowl_mutex);
        sem_post(&bowl);

        // Use oven
        sem_wait(&oven);
        log_baker(id, "Baking in oven...");
        sleep(1);
        log_baker(id, "Removed from oven");
        sem_post(&oven);

        snprintf(msg, sizeof(msg), "Finished recipe: %s", recipes[i]);
        log_baker(id, msg);
    }
    log_baker(id, "All recipes completed!");
    return NULL;
}

int main() {
    srand(time(NULL));
    int num_bakers;
    printf("Enter number of bakers (max %d): ", MAX_BAKERS);
    scanf("%d", &num_bakers);
    if (num_bakers < 1 || num_bakers > MAX_BAKERS) {
        printf("Invalid number of bakers.\n");
        return 1;
    }

    ramsied_baker = rand() % num_bakers;
    printf("Baker %d has a chance to be Ramsied!\n", ramsied_baker);

    sem_init(&mixer, 0, 2);
    sem_init(&pantry, 0, 1);
    sem_init(&refrigerator, 0, 2);
    sem_init(&bowl, 0, 3);
    sem_init(&spoon, 0, 5);
    sem_init(&oven, 0, 1);
    sem_init(&ingredient_mutex, 0, 1);

    pthread_t bakers[MAX_BAKERS];
    int ids[MAX_BAKERS];

    for (int i = 0; i < num_bakers; i++) {
        ids[i] = i;
        pthread_create(&bakers[i], NULL, baker_thread, &ids[i]);
    }

    for (int i = 0; i < num_bakers; i++) {
        pthread_join(bakers[i], NULL);
    }

    return 0;
}