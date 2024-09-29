# Untitled

# CalTrain (Synchronization and Mutual Exclusion)

## Overview

This project simulates a synchronized boarding process at a train station using multi-threading in C. The project ensures that passengers and trains interact without any race conditions or deadlocks using **Pthreads**, specifically **mutexes** and **condition variables**. The key components include the train arriving, passengers waiting to board, and passengers notifying the system when they have taken their seats.

The synchronization between passengers and trains is carefully managed to avoid busy-waiting and maximize concurrency, allowing multiple passengers to board simultaneously when possible.

## System Requirements

- **Operating System**: Linux (Ubuntu recommended)
- **Programming Language**: C
- **Libraries**: Pthreads (POSIX threads)
- **Synchronization Primitives**: Mutexes, condition variables

## Compilation and Execution

### Compilation:

The project can be compiled using the following command:

```bash
gcc -pthread -o caltrain caltrain.c caltrain-runner.c
```

### Execution:

To run the program:

```bash
./caltrain
```

## Code Structure and Explanation

### 1. **station_init(struct station station)*

In the `station_init()` function, I initialize the necessary synchronization primitives that will be used throughout the program. Specifically, this function initializes the **mutex** and **condition variables** that will control access to shared resources like the number of available seats and passengers waiting to board.

```c
void station_init(struct station *station) {
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->train_arrived, NULL);
    pthread_cond_init(&station->passenger_boarded, NULL);
    station->waiting_passengers = 0;
    station->available_seats = 0;
    station->boarded_passengers = 0;
}
```

This ensures that the station is properly initialized, with counters set to zero and synchronization primitives ready to be used by other functions.

### 2. **station_load_train(struct station station, int count)*

The `station_load_train()` function is invoked when a train arrives at the station. The train provides the number of available seats, and this function is responsible for ensuring that all passengers who are waiting and can board are allowed to do so. The function will not return until either all passengers have boarded or the train is full.

```c
void station_load_train(struct station *station, int count) {
    pthread_mutex_lock(&station->lock);
    station->available_seats = count;

    while (station->available_seats > 0 && station->waiting_passengers > 0) {
        pthread_cond_broadcast(&station->train_arrived); // Notify all passengers that train has arrived
        pthread_cond_wait(&station->passenger_boarded, &station->lock);
    }

    station->available_seats = 0; // Reset available seats after train departs
    pthread_mutex_unlock(&station->lock);
}

```

In this implementation, I use **pthread_cond_broadcast()** to signal all waiting passengers that a train has arrived and they may board. The train will wait for passengers to board using **pthread_cond_wait()**, and it will continue waiting until all available seats are filled or no more passengers are left.

### 3. **station_wait_for_train(struct station station)*

When a passenger arrives at the station and is waiting to board a train, they call `station_wait_for_train()`. This function ensures that the passenger does not proceed until a train has arrived and there are available seats.

```c
void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers++;

    while (station->available_seats == 0) {
        pthread_cond_wait(&station->train_arrived, &station->lock);
    }

    station->waiting_passengers--;
    station->available_seats--; // Decrease available seat count as passenger boards
    pthread_mutex_unlock(&station->lock);

```

Here, I use **pthread_cond_wait()** to block the passenger until they are signaled that a train has arrived. Once a seat is available, the passenger claims the seat, and the count of available seats and waiting passengers is updated accordingly.

### 4. **station_on_board(struct station station)*

After the passenger successfully boards the train and takes a seat, they call `station_on_board()` to signal that they are on board. This function is crucial for notifying the train that the boarding process can continue.

```c
void station_on_board(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->boarded_passengers++;
    pthread_cond_signal(&station->passenger_boarded); // Notify the train that a passenger has boarded
    pthread_mutex_unlock(&station->lock);
}
```

In this function, I use **pthread_cond_signal()** to notify the train that a passenger has boarded. This allows the train to track the number of boarded passengers and decide when it can depart.

### Synchronization Overview

The project uses **mutex locks** and **condition variables** to ensure that passengers and trains interact in a synchronized manner:

- **Mutex Locking**: The mutex is used to protect shared resources such as the number of available seats and the number of waiting passengers. This prevents race conditions when multiple passengers or trains try to update these values concurrently.
- **Condition Variables**: Condition variables allow the program to block threads (either trains or passengers) until a certain condition is met, such as a train arriving or a passenger boarding. This ensures that threads only proceed when it is safe to do so, avoiding busy waiting.

### Avoiding Busy Waiting

The program avoids busy waiting by using **pthread_cond_wait()** to block threads that are waiting for a specific condition. Instead of constantly checking for the condition, the thread will sleep until it is signaled that the condition has changed. This results in more efficient CPU usage and smoother synchronization.

### Passenger and Train Interaction

The interaction between trains and passengers is carefully managed to ensure that:

1. **Trains only depart when they are full or all passengers have boarded**. The train waits using **pthread_cond_wait()** until all passengers are seated.
2. **Passengers only board when a train is present and seats are available**. This is achieved by signaling passengers using **pthread_cond_broadcast()** when a train arrives.
3. **Multiple passengers can board simultaneously**, reducing idle time for both the passengers and the train.

### Example Scenario

- A train arrives at the station with 5 seats available. The `station_load_train()` function signals all waiting passengers that they can start boarding.
- As passengers board, the number of available seats is reduced, and the train waits for all boarded passengers to signal they are seated using `station_on_board()`.
- Once all passengers have boarded or the train is full, the train departs, and the process repeats with the next arriving train.