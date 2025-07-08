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
        return; // In a real-world scenario, you might want more robust error handling
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
 * @brief Wakes up the next trainee in the queue.
 */
void wake_up_next_waiter() {
    // Lock the queue for exclusive access
    pthread_mutex_lock(&waiting_queue.lock);

    // Check if there is anyone to wake up
    if (waiting_queue.head == NULL) {
        pthread_mutex_unlock(&waiting_queue.lock);
        return;
    }

    // Dequeue the trainee at the head of the list
    WaitingNode* node_to_wake = waiting_queue.head;
    waiting_queue.head = waiting_queue.head->next;
    if (waiting_queue.head == NULL) { // If the queue is now empty
        waiting_queue.tail = NULL;
    }

    // Unlock the queue
    pthread_mutex_unlock(&waiting_queue.lock);

    // Signal the trainee's personal semaphore to wake them up
    sem_post(&node_to_wake->trainee->personal_sem);
    free(node_to_wake); // Free the memory for the dequeued node
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