#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "course.h"
#include "trainee.h"
#include "simulation.h"

/**
 * @brief Prints the results for a single trainee.
 * @param trainee The trainee whose results are to be printed.
 */
void print_trainee_results(Trainee* trainee) {
    // This is a placeholder for the detailed printing logic.
    // In the full implementation, you would iterate through the trainee's
    // stats struct and print times for each obstacle as per the example output.
    printf("%s results:\n", trainee->name);
    printf("  exit: %lld elapsed: %lld active: %lld wait: %lld\n\n",
           trainee->stats.total_elapsed_time,
           trainee->stats.total_elapsed_time,
           trainee->stats.total_active_time,
           trainee->stats.total_wait_time);
}


int main(int argc, char* argv[]) {
    // Seed the random number generator for randomized obstacle times
    srand(time(NULL));

    // Initialize shared simulation resources, like the waiting list mutex
    init_simulation();

    // --- Load Data ---
    const char* course_file = "course-desc.txt";
    const char* team_file = "team-desc.txt";

    Course* course = load_course_from_file(course_file);
    if (!course) {
        return EXIT_FAILURE;
    }

    int num_trainees = 0;
    Trainee* trainees = load_trainees_from_file(team_file, &num_trainees);
    if (!trainees) {
        cleanup_course(course);
        return EXIT_FAILURE;
    }

    // --- Create Threads ---
    pthread_t* threads = malloc(num_trainees * sizeof(pthread_t));
    TraineeThreadArgs* thread_args = malloc(num_trainees * sizeof(TraineeThreadArgs));
    if (!threads || !thread_args) {
        perror("Failed to allocate memory for thread management");
        exit(EXIT_FAILURE);
    }
    
    printf("Starting simulation...\n\n");

    for (int i = 0; i < num_trainees; i++) {
        // Package arguments for the thread function
        thread_args[i].trainee = &trainees[i];
        thread_args[i].course = course;

        // Create one thread per trainee
        if (pthread_create(&threads[i], NULL, trainee_run, &thread_args[i]) != 0) {
            perror("Failed to create trainee thread");
            exit(EXIT_FAILURE);
        }
    }

    // --- Wait for Threads and Report Results ---
    for (int i = 0; i < num_trainees; i++) {
        // Wait for each thread to finish its execution
        pthread_join(threads[i], NULL);
        // After a thread joins, its results are ready for reporting
        print_trainee_results(&trainees[i]);
    }

    // --- Cleanup ---
    printf("Simulation finished. Cleaning up resources.\n");
    cleanup_trainees(trainees, num_trainees);
    cleanup_course(course);
    destroy_simulation();

    free(threads);
    free(thread_args);

    return EXIT_SUCCESS;
}