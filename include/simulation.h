#ifndef SIMULATION_H
#define SIMULATION_H

#include <pthread.h>
#include "trainee.h"

/**
 * @struct WaitingNode
 * @brief A node in a linked list for trainees who are waiting.
 */
typedef struct WaitingNode {
    Trainee* trainee;
    struct WaitingNode* next;
} WaitingNode;

/**
 * @struct WaitingQueue
 * @brief A FIFO queue to manage waiting trainees efficiently.
 *
 * This structure holds the head and tail of the waiting list and
 * is protected by a single mutex to prevent race conditions.
 */
typedef struct {
    WaitingNode* head;
    WaitingNode* tail;
    pthread_mutex_t lock;
} WaitingQueue;

// --- Global Shared Variable ---
extern WaitingQueue waiting_queue;


// --- Function Prototypes ---

/**
 * @brief Initializes the simulation's shared resources.
 */
void init_simulation();

/**
 * @brief Adds a trainee to the end of the shared waiting queue.
 * @param trainee A pointer to the trainee who needs to wait.
 */
void enqueue_waiter(Trainee* trainee);

/**
 * @brief Wakes up all trainees currently in the waiting queue.
 *
 * This function is called by a trainee who has just freed an obstacle.
 * It wakes up all trainees to let them re-evaluate obstacle availability,
 * as required by the simulation specification.
 */
void wake_up_all_waiters();

/**
 * @brief Cleans up and destroys the simulation's shared resources.
 */
void destroy_simulation();

#endif // SIMULATION_H