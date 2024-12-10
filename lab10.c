#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "lab10.h"

extern char * dishes[];
int curr_customer = 0;
struct pair{
	
	int customer_waiting;
	int customer_seated;
	int global_var;
	int global_var2;
	int yes;
	pthread_mutex_t var_lock;
	pthread_cond_t test;
	pthread_cond_t test2;
	pthread_cond_t test3;
	pthread_cond_t test4;
	pthread_cond_t test5;

};
pthread_mutex_t var_lock;
pthread_cond_t t;
int idk = -1;
int c = 0;
static int served_customers;
struct pair p[MAX_CUSTOMERS];
void initialize(void)
{
    // add initializations here
	for(int i = 0 ; i < MAX_CUSTOMERS; i++){
		p[i].customer_waiting = -1;
		p[i].customer_seated = -1;
		p[i].global_var = -1;
		p[i].global_var2 = -1;
		p[i].yes = -1;
		pthread_mutex_init(&(p[i].var_lock), NULL);
		pthread_cond_init(&(p[i].test), NULL);
		pthread_cond_init(&(p[i].test2), NULL);
		pthread_cond_init(&(p[i].test3), NULL);
		pthread_cond_init(&(p[i].test4), NULL);
		pthread_cond_init(&(p[i].test5), NULL);
	
	
	}
	pthread_cond_init(&t, NULL);
	pthread_mutex_init(&var_lock, NULL);
}
int x = 0;

void customer(int id, int dish)
{
    // signal customer ready to be served
    
	pthread_mutex_lock(&(p[x].var_lock));
	ready_to_seat_customer(x);

    // TODO 1: try to get ownership of the lock
    p[x].customer_waiting = x;
    curr_customer = x;

    pthread_cond_signal(&t);
    idk = 1;

    // TODO 2: Signal to the waiter that customer is waiting

    pthread_cond_signal(&(p[x].test));
    //pthread_mutex_unlock(&(p[id].var_lock));
    // TODO: 4 Wait to be seated


    //pthread_mutex_lock(&(p[id].var_lock));
    while(p[id].customer_seated == -1){
	    pthread_cond_wait(&(p[x].test2), &(p[x].var_lock));
    }
    //pthread_mutex_unlock(&var_lock);

    // TODO: 6 place order and signal waiter
    //pthread_mutex_lock(&var_lock);
    order_placed_by_customer(x, dish);


    p[id].global_var = dish;
    pthread_cond_signal(&(p[x].test3));


    while(p[x].yes == -1){
	pthread_cond_wait(&(p[x].test5), &(p[x].var_lock));
    }
    service_completed_for_customer(x);

    p[c].global_var2 = 1;
    pthread_cond_signal(&(p[x].test4));
	
    x++;
    pthread_mutex_unlock(&(p[x-1].var_lock));
}


void waiter(void)
{
    if (served_customers >= MAX_CUSTOMERS)
        return;

    // seat waiting customer

    // TODO 3: replace busy loop with predicate/cond_wait construct
    pthread_mutex_lock(&(p[c].var_lock));

    while(idk == -1){
	    pthread_cond_wait(&t, &(p[c].var_lock));

    }
    while (p[c].customer_waiting == -1){
    	pthread_cond_wait(&(p[c].test), &(p[c].var_lock));
    }
    int customer = p[c].customer_waiting;

    // TODO 5: seat the customer and then signal the customer thread
    seat_customer(customer);

    p[c].customer_seated = 1;
    pthread_cond_signal(&(p[c].test2));
    // TODO 6: wait for customer to place order then determine the order number
    while(p[c].global_var == -1){
	pthread_cond_wait(&(p[c].test3), &(p[c].var_lock));
    }
    bring_food_to_customer(customer, p[c].global_var); // second arg should be zero in starter
   
    p[c].yes = 1;
    pthread_cond_signal(&(p[c].test5));
    
    
    while(p[c].global_var2 == -1){
	pthread_cond_wait(&(p[c].test4), &(p[c].var_lock));
    }

    served_customers++;

    c++;
    pthread_mutex_unlock(&(p[c-1].var_lock));
}
