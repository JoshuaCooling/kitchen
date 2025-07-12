# 🍞 Multithreaded Bakery Simulation

This project simulates multiple bakers working concurrently in a shared kitchen to prepare a variety of recipes. It demonstrates the use of **threads**, **semaphores**, and **mutexes** in C to coordinate access to shared resources and ensure thread-safe execution.

## 👨‍🍳 Description

Each baker is represented by a **thread**. Bakers must gather ingredients and use shared kitchen resources (mixers, bowls, ovens, etc.) that are protected with **semaphores** and **mutexes** to simulate a real-world scenario of limited resources and access contention.

A fun twist in the simulation is the randomly chosen "**Ramsied Baker**" — a baker who has a chance of getting "RAMSIED!" during recipe preparation, forcing them to restart the current recipe. This adds complexity to thread execution and highlights the value of robust synchronization mechanisms.

## 🧠 Key Concepts

- **Threads** (`pthread`) for simulating concurrent bakers.
- **Semaphores** (`sem_t`) for managing limited quantities of shared resources.
- **Mutexes** (`pthread_mutex_t`) for controlling access to shared arrays and avoiding race conditions.
- **Random behavior** with potential forced restarts (RAMSIED effect).
- **Console logging** with ANSI color codes for improved readability per baker thread.

## 🛠️ Technologies Used

- C Programming Language
- POSIX Threads (pthreads)
- POSIX Semaphores
- ANSI Terminal Coloring (for logs)
- Linux/GCC-compatible systems

## 🚀 How to Run

### 1. Compile the code:

```bash
gcc -o bakery bakery.c -lpthread
```

### 2. Run the program:

```bash
./bakery
```

You’ll be prompted to enter the number of bakers (max 6). One will be randomly selected to be “Ramsied.”

## 🧪 Example Output

Sample output when the program runs (colored per thread):

```
Baker 0: Starting recipe: Cookies
Baker 2: Starting recipe: Cookies
Baker 1: Starting recipe: Cookies
Baker 2: Accessed pantry for Sugar
Baker 1: Accessed pantry for Flour
Baker 0: Opened refrigerator #0 for Milk
...
Baker 2: RAMSIED! Restarting recipe.
...
Baker 1: Finished recipe: Cinnamon Rolls
Baker 1: All recipes completed!
```

## 🍽️ Recipes Simulated

1. Cookies  
2. Pancakes  
3. Pizza Dough  
4. Soft Pretzels  
5. Cinnamon Rolls  

Each recipe has its own list of ingredients. Some require access to the **pantry**, others the **refrigerator**, both of which are semaphored resources.

## 🎯 RAMSIED Mechanic

One randomly selected baker has a **chance (1 in 12)** of being “RAMSIED!” during ingredient collection. When this happens:
- The baker logs the event.
- The recipe restarts from the beginning (using `goto retry`).

## 🧩 Shared Resources

| Resource         | Type        | Quantity | Access Controlled By   |
|------------------|-------------|----------|--------------------------|
| Mixers           | Equipment   | 2        | `sem_t` + mutex          |
| Bowls            | Equipment   | 3        | `sem_t` + mutex          |
| Spoons           | Equipment   | 5        | `sem_t` + mutex          |
| Ovens            | Equipment   | 1        | `sem_t` only             |
| Pantry           | Ingredients | 1        | `sem_t`                  |
| Refrigerator     | Ingredients | 2        | `sem_t` + mutex          |
| Ingredient Access| Global      | 1        | `sem_t`                  |

## ✅ What This Project Demonstrates

- How to use semaphores and mutexes to **synchronize threads** and avoid race conditions.
- Managing access to **limited shared resources** in a concurrent system.
- Creating thread-safe abstractions for **resource acquisition/release**.
- Simulating real-world constraints (like only one person opening the fridge at a time).
- Fun, educational use of `goto` and randomness to simulate kitchen chaos.
