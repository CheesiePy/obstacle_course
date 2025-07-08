#define _GNU_SOURCE // For strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For usleep
#include <sys/time.h> // For gettimeofday
#include "trainee.h"
#include "course.h"
#include "simulation.h"

/**
 * @brief Gets the current time in milliseconds.
 * @return The current time in milliseconds.
 */
static inline long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief Loads trainee data from the specified file.
 */
Trainee* load_trainees_from_file(const char* filename, int* num_trainees) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening team description file");
        return NULL;
    }

    // Read the number of trainees
    fscanf(file, "%d\n", num_trainees);

    Trainee* trainees = (Trainee*)malloc(*num_trainees * sizeof(Trainee));
    if (!trainees) {
        perror("Failed to allocate memory for trainees");
        fclose(file);
        return NULL;
    }

    // Read details for each trainee
    for (int i = 0; i < *num_trainees; i++) {
        char name_buffer[100];
        fscanf(file, "%s\t%d\n", name_buffer, &trainees[i].skill_modifier);
        trainees[i].name = strdup(name_buffer);

        // Initialize personal semaphore to 0 (locked)
        // This makes the trainee wait if they call sem_wait on it.
        if (sem_init(&trainees[i].personal_sem, 0, 0) != 0) {
            perror("Failed to initialize trainee semaphore");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
    return trainees;
}

/**
 * @brief Frees all memory associated with trainees.
 */
void cleanup_trainees(Trainee* trainees, int num_trainees) {
    if (!trainees) return;

    for (int i = 0; i < num_trainees; i++) {
        sem_destroy(&trainees[i].personal_sem);
        free(trainees[i].name);
    }
    free(trainees);
}

/**
 * @brief The main execution function for each trainee thread.
 */
void* trainee_run(void* arg) {
    TraineeThreadArgs* args = (TraineeThreadArgs*)arg;
    Trainee* trainee = args->trainee;
    Course* course = args->course;
    int num_obstacle_types = course->num_obstacle_types;

    // Allocate per-obstacle stats arrays
    trainee->stats.obstacle_start_times = calloc(num_obstacle_types, sizeof(long long));
    trainee->stats.obstacle_finish_times = calloc(num_obstacle_types, sizeof(long long));
    trainee->stats.obstacle_durations = calloc(num_obstacle_types, sizeof(long long));
    trainee->stats.obstacle_type_indices = calloc(num_obstacle_types, sizeof(int));
    trainee->stats.obstacle_instance_indices = calloc(num_obstacle_types, sizeof(int));

    // Record enter time
    trainee->stats.enter_time = get_time_ms();

    // Array to track which obstacle types have been completed
    int* completed = calloc(num_obstacle_types, sizeof(int));
    int types_completed_count = 0;

    long long total_active = 0;
    long long total_wait = 0;

    // --- MAIN SIMULATION LOOP ---
    while (types_completed_count < num_obstacle_types) {
        int found_obstacle = 0;

        // Iterate through all obstacle types to find one that is needed and available
        for (int i = 0; i < num_obstacle_types; i++) {
            if (completed[i]) continue; // Skip if already done

            for (int j = 0; j < course->obstacle_types[i].count; j++) {
                if (pthread_mutex_trylock(&course->obstacle_types[i].instances[j].lock) == 0) {
                    // --- SUCCESS: OBSTACLE ACQUIRED ---
                    found_obstacle = 1;
                    completed[i] = 1;
                    int obstacle_idx = types_completed_count;
                    trainee->stats.obstacle_type_indices[obstacle_idx] = i;
                    trainee->stats.obstacle_instance_indices[obstacle_idx] = j;
                    types_completed_count++;

                    // Record start time
                    long long start_time = get_time_ms();
                    trainee->stats.obstacle_start_times[obstacle_idx] = start_time;

                    // Calculate time to cross
                    float base_time = course->obstacle_types[i].average_time_ms;
                    float skill_factor = 1.0 - (trainee->skill_modifier / 100.0);
                    float random_factor = 1.0 + ((rand() % 21 - 10) / 100.0);
                    long sleep_duration_ms = base_time * skill_factor * random_factor;

                    // Simulate the time it takes to cross the obstacle
                    usleep(sleep_duration_ms * 1000); // usleep takes microseconds

                    // Record finish time and duration
                    long long finish_time = get_time_ms();
                    trainee->stats.obstacle_finish_times[obstacle_idx] = finish_time;
                    trainee->stats.obstacle_durations[obstacle_idx] = finish_time - start_time;
                    total_active += finish_time - start_time;

                    pthread_mutex_unlock(&course->obstacle_types[i].instances[j].lock);

                    // An obstacle is free, so wake up all waiting trainees.
                    wake_up_all_waiters();
                    goto next_round;
                }
            }
        }

    next_round:
        // --- FAILURE: NO OBSTACLE FOUND ---
        if (!found_obstacle) {
            // Record the time we start waiting.
            long long wait_start = get_time_ms();
            
            // Add self to the waiting queue and wait to be woken up.
            enqueue_waiter(trainee);
            sem_wait(&trainee->personal_sem);
            
            // Record how long we waited.
            long long wait_end = get_time_ms();
            total_wait += wait_end - wait_start;
        }
    }

    // --- CLEANUP ---
    trainee->stats.exit_time = get_time_ms();
    trainee->stats.total_elapsed_time = trainee->stats.exit_time - trainee->stats.enter_time;
    trainee->stats.total_active_time = total_active;
    trainee->stats.total_wait_time = total_wait;
    free(completed);
    return NULL;
}