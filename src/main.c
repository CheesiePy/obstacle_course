#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "course.h"
#include "trainee.h"
#include "simulation.h"

void print_trainee_results(Trainee* trainee, Course* course, int num_obstacle_types) {
    printf("%s results:\n", trainee->name);
    printf("enter: %lld\n", trainee->stats.enter_time);

    for (int i = 0; i < num_obstacle_types; i++) {
        int type_idx = trainee->stats.obstacle_type_indices[i];
        int inst_idx = trainee->stats.obstacle_instance_indices[i];
        printf("  %d. %s obs: %d start: %lld finish: %lld duration: %lld\n",
               i,
               course->obstacle_types[type_idx].name,
               inst_idx,
               trainee->stats.obstacle_start_times[i],
               trainee->stats.obstacle_finish_times[i],
               trainee->stats.obstacle_durations[i]);
    }

    printf("exit: %lld elapsed: %lld active: %lld wait: %lld\n\n",
           trainee->stats.exit_time,
           trainee->stats.total_elapsed_time,
           trainee->stats.total_active_time,
           trainee->stats.total_wait_time);

    // Free the dynamically allocated stat arrays
    free(trainee->stats.obstacle_start_times);
    free(trainee->stats.obstacle_finish_times);
    free(trainee->stats.obstacle_durations);
    free(trainee->stats.obstacle_type_indices);
    free(trainee->stats.obstacle_instance_indices);
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
        print_trainee_results(&trainees[i], course, course->num_obstacle_types);
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