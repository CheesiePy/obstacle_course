#ifndef SIMULATION_H
#define SIMULATION_H

#include <pthread.h>
#include "trainee.h"

/**
 * @struct WaitingNode
 * @brief A node in a linked list for trainees who are waiting.
 *
 * [cite_start]This structure creates a linked list to manage trainees who are waiting for an obstacle to become free[cite: 31].
 */
typedef struct WaitingNode {
    Trainee* trainee;
    struct WaitingNode* next;
} WaitingNode;

// --- Global Shared Variables ---

/**
 * @var waiting_list_mutex
 * @brief A mutex to ensure atomic access to the waiting list.
 *
 * This lock is essential to prevent race conditions when multiple trainees
 * [cite_start]try to add or remove themselves from the waiting list simultaneously[cite: 38, 55].
 */
extern pthread_mutex_t waiting_list_mutex;

/**
 * @var waiting_list_head
 * @brief A pointer to the head of the linked list of waiting trainees.
 */
extern WaitingNode* waiting_list_head;


// --- Function Prototypes ---

/**
 * @brief Initializes the simulation's shared resources.
 *
 * This function must be called once at the start of the program to
 * initialize the waiting list mutex.
 */
void init_simulation();

/**
 * @brief Adds a trainee to the shared waiting list in a thread-safe manner.
 * @param trainee A pointer to the trainee who needs to wait.
 */
void add_to_waiting_list(Trainee* trainee);

/**
 * @brief Wakes up all trainees currently on the waiting list.
 *
 * [cite_start]A trainee who frees an obstacle calls this function[cite: 32]. It iterates through
 * the list, signals each trainee's personal semaphore to wake them up,
 * [cite_start]and then clears the list[cite: 32, 34].
 */
void wake_up_all_waiters();

/**
 * @brief Cleans up and destroys the simulation's shared resources.
 *
 * This function should be called at the end of the program to destroy
 * the mutex and free any remaining memory associated with the waiting list.
 */
void destroy_simulation();

#endif // SIMULATION_H