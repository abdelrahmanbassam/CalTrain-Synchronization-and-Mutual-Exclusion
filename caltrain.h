#include <pthread.h>

struct station {
	// FILL ME IN
	int emptySeats;
	int waitingPassengers;
	int passengersWalkingOnTrain;
	pthread_mutex_t mutex_;
	pthread_cond_t trainArrived, trainReadyToLeave;
	
	
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);