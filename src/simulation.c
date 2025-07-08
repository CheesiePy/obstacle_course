#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"

// --- Global Variable Definition ---
WaitingQueue waiting_queue;

/**
 * @brief Initializes the simulation's shared resources.
 */
void init_simulation() {
    waiting_queue.head = NULL;
    waiting_queue.tail = NULL;
    if (pthread_mutex_init(&waiting_queue.lock, NULL) != 0) {
        perror("Failed to initialize waiting queue mutex");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Adds a trainee to the end of the waiting queue (FIFO).
 */
void enqueue_waiter(Trainee* trainee) {
    // Allocate a new node for the trainee
    WaitingNode* newNode = (WaitingNode*)malloc(sizeof(WaitingNode));
    if (!newNode) {
        perror("Failed to allocate memory for waiting list node");
        return;
    }
    newNode->trainee = trainee;
    newNode->next = NULL;

    // Lock the queue for exclusive access
    pthread_mutex_lock(&waiting_queue.lock);

    // Add the new node to the tail of the queue
    if (waiting_queue.tail == NULL) { // If the queue is empty
        waiting_queue.head = newNode;
        waiting_queue.tail = newNode;
    } else {
        waiting_queue.tail->next = newNode;
        waiting_queue.tail = newNode;
    }

    // Unlock the queue
    pthread_mutex_unlock(&waiting_queue.lock);
}

/**
 * @brief Wakes up all trainees in the queue using a safer pattern.
 */
void wake_up_all_waiters() {
    pthread_mutex_lock(&waiting_queue.lock);

    // Move the entire list of waiters to a temporary list.
    WaitingNode* head_to_wake = waiting_queue.head;

    // Reset the main queue, making it available for new waiters immediately.
    waiting_queue.head = NULL;
    waiting_queue.tail = NULL;
    
    pthread_mutex_unlock(&waiting_queue.lock);

    // Now, wake up the trainees from the temporary list.
    // This avoids holding the lock while signaling, which is safer.
    while (head_to_wake != NULL) {
        sem_post(&head_to_wake->trainee->personal_sem);
        WaitingNode* to_free = head_to_wake;
        head_to_wake = head_to_wake->next;
        free(to_free);
    }
}


/**
 * @brief Cleans up and destroys the simulation's shared resources.
 */
void destroy_simulation() {
    // Free any remaining nodes in the list
    WaitingNode* current = waiting_queue.head;
    while (current != NULL) {
        WaitingNode* temp = current;
        current = current->next;
        free(temp);
    }
    // Destroy the mutex
    pthread_mutex_destroy(&waiting_queue.lock);
}