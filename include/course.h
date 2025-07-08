#ifndef COURSE_H
#define COURSE_H

#include <pthread.h>

// Represents one physical station, e.g., "rope 2" [cite: 17]
typedef struct {
    pthread_mutex_t lock;
} ObstacleInstance;

// Represents a category of obstacles, e.g., "rope" [cite: 13]
typedef struct {
    char* name;
    int count; // Number of instances [cite: 102]
    int average_time_ms; // Average time to cross [cite: 103]
    ObstacleInstance* instances; // Array of the actual obstacles
} ObstacleType;

// Main struct to hold all course data
typedef struct {
    int num_obstacle_types;
    ObstacleType* obstacle_types;
} Course;

// Add these declarations to match the implementations in src/course.c
Course* load_course_from_file(const char* filename);
void cleanup_course(Course* course);

#endif