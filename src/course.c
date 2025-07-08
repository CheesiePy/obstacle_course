#define _GNU_SOURCE // For strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // <--- Missing header added here
#include "course.h"

/**
 * @brief Loads course data from the specified file.
 */
Course* load_course_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening course description file");
        return NULL;
    }

    // Allocate the main course struct
    Course* course = (Course*)malloc(sizeof(Course));
    if (!course) {
        perror("Failed to allocate memory for course");
        fclose(file);
        return NULL;
    }

    // Read the number of obstacle types
    fscanf(file, "%d\n", &course->num_obstacle_types);

    // Allocate memory for the array of obstacle types
    course->obstacle_types = (ObstacleType*)malloc(course->num_obstacle_types * sizeof(ObstacleType));
    if (!course->obstacle_types) {
        perror("Failed to allocate memory for obstacle types");
        free(course);
        fclose(file);
        return NULL;
    }

    // Read details for each obstacle type
    for (int i = 0; i < course->num_obstacle_types; i++) {
        char name_buffer[100];
        fscanf(file, "%s\t%d\t%d\n", name_buffer, &course->obstacle_types[i].count, &course->obstacle_types[i].average_time_ms);
        course->obstacle_types[i].name = strdup(name_buffer);

        // Allocate memory for the individual instances of this obstacle type
        course->obstacle_types[i].instances = (ObstacleInstance*)malloc(course->obstacle_types[i].count * sizeof(ObstacleInstance));
        if (!course->obstacle_types[i].instances) {
            // Complex cleanup needed here in a real scenario
            perror("Failed to allocate memory for obstacle instances");
            exit(EXIT_FAILURE);
        }

        // Initialize a mutex for each obstacle instance
        for (int j = 0; j < course->obstacle_types[i].count; j++) {
            if (pthread_mutex_init(&course->obstacle_types[i].instances[j].lock, NULL) != 0) {
                perror("Failed to initialize obstacle mutex");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
    return course;
}

/**
 * @brief Frees all memory associated with the course.
 */
void cleanup_course(Course* course) {
    if (!course) return;

    for (int i = 0; i < course->num_obstacle_types; i++) {
        // Destroy the mutex for each instance
        for (int j = 0; j < course->obstacle_types[i].count; j++) {
            pthread_mutex_destroy(&course->obstacle_types[i].instances[j].lock);
        }
        free(course->obstacle_types[i].instances);
        free(course->obstacle_types[i].name);
    }
    free(course->obstacle_types);
    free(course);
}