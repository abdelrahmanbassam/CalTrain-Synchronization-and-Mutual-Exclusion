#include <pthread.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->emptySeats = 0;
	station->waitingPassengers = 0;
	station->passengersWalkingOnTrain = 0;
	
	pthread_mutex_init(&station->mutex_, NULL);
	pthread_cond_init(&station->trainArrived, NULL);
	pthread_cond_init(&station->trainReadyToLeave, NULL);
}

void
station_load_train(struct station *station, int trainCapacity)
{
	pthread_mutex_lock(&station->mutex_);
	
	 station->emptySeats = trainCapacity;
	
	 pthread_cond_broadcast(&station->trainArrived);
    
    

	while(station->emptySeats > 0 && station->waitingPassengers > 0){
		pthread_cond_wait(&station->trainReadyToLeave, &station->mutex_);
	}
     station->emptySeats = 0;  
    pthread_mutex_unlock(&station->mutex_);

    
}

void
station_wait_for_train(struct station *station){

   //to protect the number of waiting passengers
	pthread_mutex_lock(&station->mutex_);
	 
	 station->waitingPassengers++;
	
	 //wait until train arrives and there are at least one empty seat
	 while(station->emptySeats == 0){
	 	pthread_cond_wait(&station->trainArrived, &station->mutex_);
	 }
    
	 station->emptySeats--;
	 station->waitingPassengers--;
	 station->passengersWalkingOnTrain++;

    pthread_mutex_unlock(&station->mutex_);
}


void
station_on_board(struct station *station){
	
	pthread_mutex_lock(&station->mutex_);
	 station->passengersWalkingOnTrain--;
	
	 //notify the train that the passenger has boarded and if the train is full, the train will leave
	 if((station->emptySeats == 0 || station->waitingPassengers == 0 )&& station->passengersWalkingOnTrain == 0){
		// station->emptySeats = 0;
		pthread_cond_signal(&station->trainReadyToLeave);
	 }
	pthread_mutex_unlock(&station->mutex_);

}
