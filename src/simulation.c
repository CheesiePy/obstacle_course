#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"

// --- Global Variable Definitions ---

// Mutex to protect the waiting list
pthread_mutex_t waiting_list_mutex;

// Head of the linked list for waiting trainees
WaitingNode* waiting_list_head = NULL;


// --- Function Implementations ---

/**
 * @brief Initializes the simulation's shared resources.
 */
void init_simulation() {
    if (pthread_mutex_init(&waiting_list_mutex, NULL) != 0) {
        perror("Failed to initialize waiting list mutex");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Adds a trainee to the shared waiting list.
 */
void add_to_waiting_list(Trainee* trainee) {
    // Lock the mutex to ensure exclusive access to the list
    pthread_mutex_lock(&waiting_list_mutex);

    // Create a new node
    WaitingNode* newNode = (WaitingNode*)malloc(sizeof(WaitingNode));
    if (!newNode) {
        perror("Failed to allocate memory for waiting list node");
        pthread_mutex_unlock(&waiting_list_mutex); // Unlock before exiting
        return;
    }
    newNode->trainee = trainee;
    newNode->next = waiting_list_head; // Add to the front of the list
    waiting_list_head = newNode;

    // Unlock the mutex
    pthread_mutex_unlock(&waiting_list_mutex);
}

/**
 * @brief Wakes up all trainees on the waiting list.
 */
void wake_up_all_waiters() {
    // Lock the mutex for exclusive access
    pthread_mutex_lock(&waiting_list_mutex);

    WaitingNode* current = waiting_list_head;
    while (current != NULL) {
        // Signal the trainee's personal semaphore to wake them up
        sem_post(&current->trainee->personal_sem);
        current = current->next;
    }

    // Free the entire list after waking everyone up
    while (waiting_list_head != NULL) {
        WaitingNode* temp = waiting_list_head;
        waiting_list_head = waiting_list_head->next;
        free(temp);
    }

    // Unlock the mutex
    pthread_mutex_unlock(&waiting_list_mutex);
}

/**
 * @brief Cleans up and destroys the simulation's shared resources.
 */
void destroy_simulation() {
    // There should be no waiters at the end, but clean up just in case
    while (waiting_list_head != NULL) {
        WaitingNode* temp = waiting_list_head;
        waiting_list_head = waiting_list_head->next;
        free(temp);
    }
    pthread_mutex_destroy(&waiting_list_mutex);
}