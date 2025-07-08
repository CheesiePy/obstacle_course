# Obstacle Course Simulation

This project is a **multi-threaded C simulation** of trainees navigating an obstacle course. It demonstrates the use of POSIX threads (`pthreads`), mutexes, and semaphores to coordinate concurrent access to shared resources. The simulation is highly configurable, allowing a variety of course layouts and trainee skill levels.

---

## 🚀 Features

* **Multi-Threaded Simulation** – each trainee runs in its own thread.  
* **Synchronization** – uses `pthread_mutex_t` to guard obstacle instances and the shared waiting queue, and `sem_t` to manage waiting trainees.  
* **Dynamic Configuration** – reads `course-desc.txt` and `team-desc.txt`, so you can tweak the course or team without recompiling.  
* **Detailed Statistics** – reports total time, active time, wait time, and per-obstacle completion times for every trainee.  
* **Correct “Thundering Herd” Handling** – when an obstacle frees up, all waiting trainees are awakened to contend for it, as required.

---

## 🏁 Getting Started

### Prerequisites

* A C compiler (e.g. `gcc`)  
* `make`  
* A POSIX-compliant OS (Linux or macOS recommended)

### Compilation

A `Makefile` is provided:

```bash
make
````

### Running the Simulation

```bash
./obstacle_course
```

The program will run the simulation and print each trainee’s results when finished.

---

## 📂 Project Structure

```
.
├── Makefile
├── course-desc.txt       # Obstacle course layout
├── team-desc.txt         # Trainee roster & skill modifiers
├── include/
│   ├── course.h          # Course & obstacle structures
│   ├── trainee.h         # Trainee data & stats
│   └── simulation.h      # Shared resources & queue
└── src/
    ├── main.c            # Entry point: loads data, spawns threads
    ├── course.c          # Parses & frees course data
    ├── trainee.c         # Trainee logic (trainee_run)
    └── simulation.c      # Waiting queue & wake-up logic
```

---

## ⚙️ Configuration

### `course-desc.txt`

```
# format: name count average_time_ms
wall      2   1500
crawl     1   1200
rope      3   2000
```

*First line:* number of obstacle types (omitted in example).
Each subsequent line defines an obstacle type:

* `name` – obstacle name
* `count` – number of instances
* `average_time_ms` – mean completion time in ms

### `team-desc.txt`

```
# format: name skill_modifier
Alice   -10
Bob       0
Clara   +15
```

*First line:* number of trainees (omitted in example).
Each subsequent line defines a trainee:

* `name` – trainee’s name
* `skill_modifier` – percentage: negative = faster, positive = slower

---

## 🛠️ Built With

* **C** – core language
* **pthreads** – multi-threading
* **make** – build automation

---

## 📚 Further Reading

* [POSIX Threads (Wikipedia)](https://en.wikipedia.org/wiki/POSIX_Threads)
* [Semaphores (Wikipedia)](https://en.wikipedia.org/wiki/Semaphore_%28programming%29)
* [“Pthreads Tutorial” – YouTube (Jacob Sorber)](https://www.youtube.com/watch?v=LLxaQz5AA4w)

