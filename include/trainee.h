#ifndef TRAINEE_H
#define TRAINEE_H

#include <semaphore.h>
#include "course.h"

/**
 * @struct TraineeStats
 * @brief Holds the performance statistics for a single trainee.
 */
typedef struct {
    long long total_elapsed_time;
    long long total_wait_time;
    long long total_active_time;
    // Per-obstacle stats
    long long* obstacle_start_times;   // [num_obstacle_types]
    long long* obstacle_finish_times;  // [num_obstacle_types]
    long long* obstacle_durations;     // [num_obstacle_types]
    int* obstacle_type_indices;        // [num_obstacle_types] - which type (0=wall, 1=rope, ...)
    int* obstacle_instance_indices;    // [num_obstacle_types] - which instance (e.g., wall 1, rope 2)
    long long enter_time;
    long long exit_time;
} TraineeStats;

/**
 * @struct Trainee
 * @brief Represents a single trainee participating in the course.
 */
typedef struct {
    char* name;
    int skill_modifier;
    sem_t personal_sem; // A personal semaphore for waiting
    TraineeStats stats;
} Trainee;

/**
 * @struct TraineeThreadArgs
 * @brief A helper struct to pass multiple arguments to a trainee's thread.
 */
typedef struct {
    Trainee* trainee;
    Course* course;
} TraineeThreadArgs; // <-- The semicolon was missing here.

// --- Function Prototypes ---

/**
 * @brief Loads all trainees from the team description file.
 */
Trainee* load_trainees_from_file(const char* filename, int* num_trainees);

/**
 * @brief Frees all memory associated with the trainees.
 */
void cleanup_trainees(Trainee* trainees, int num_trainees);

/**
 * @brief The main execution function for each trainee thread.
 */
void* trainee_run(void* arg);

#endif // TRAINEE_H