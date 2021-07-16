#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include<math.h>

int item_to_produce, curr_masters_size, item_to_consume, head=0, tail=0, items_consumed, count=0, threshold;
int total_items, max_buf_size, num_workers, num_masters;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER, not_empty =  PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock =  PTHREAD_MUTEX_INITIALIZER;

int *buffer;
int *thread_count;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}

void produce(int thread_id){
  buffer[head] = item_to_produce;
  print_produced(item_to_produce,thread_id);
  item_to_produce++;
  head = (head + 1) % max_buf_size;
  count++;
}

void consume(int thread_id){
  item_to_consume = buffer[tail];
  items_consumed++;
  print_consumed(item_to_consume,thread_id);
  thread_count[thread_id]++;
  tail = (tail + 1) % max_buf_size;
  count--;
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  pthread_mutex_lock(&qlock);
  while(1){
  int thread_id = *((int *)data);

  //printf("producer %d\n",thread_id);

  while(count  ==  max_buf_size && item_to_produce<total_items) {
    pthread_cond_wait(&not_full, &qlock);
  }
  if(item_to_produce>=total_items){

    //printf("items_produced%d  %d\n",thread_id, item_to_produce);

    for(int h=0;h<num_workers;h++){
        pthread_cond_signal(&not_empty);
      }
    pthread_mutex_unlock(&qlock);
    pthread_exit(NULL);
  }
  produce(thread_id);
  pthread_cond_signal(&not_empty);
  if(item_to_produce>=total_items){

    //printf("items_produced%d  %d\n",thread_id, item_to_produce);

    for(int h=0;h<num_workers;h++){
        pthread_cond_signal(&not_empty);
      }
    pthread_mutex_unlock(&qlock);
    pthread_exit(NULL);
  }
}
pthread_mutex_unlock(&qlock);
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *consume_requests_loop(void *data)
{
pthread_mutex_lock(&qlock);
  while(1){
  int thread_id = *((int *)data);

  //printf("consumer %d\n",thread_id);
  
  while (count == 0 && items_consumed<total_items && thread_count[thread_id]<threshold) {
    pthread_cond_wait(&not_empty, &qlock);
  }

  if(thread_count[thread_id]>=threshold || items_consumed>=total_items){
    if(items_consumed>=total_items){

            //printf("items_consumed%d    %d\n",thread_id, items_consumed);

      for(int h=0;h<num_masters;h++){
        pthread_cond_signal(&not_full);
      }
    }

    pthread_mutex_unlock(&qlock);
    pthread_exit(NULL);

}
  consume(thread_id);
  pthread_cond_signal(&not_full);


  if(thread_count[thread_id]>=threshold || items_consumed>=total_items){
    if(items_consumed>=total_items){

            //printf("items_consumed%d    %d\n",thread_id, items_consumed);

      for(int h=0;h<num_masters;h++){
        pthread_cond_signal(&not_full);
      }
    }

    pthread_mutex_unlock(&qlock);
    pthread_exit(NULL);

}

}

pthread_mutex_unlock(&qlock);
}

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  int *worker_thread_id;
  pthread_t *worker_thread;
  item_to_produce = 0;
  items_consumed = 0;
  // pthread_mutex_init(&lock, NULL);
  // pthread_cond_init(&not_empty, NULL);
  // pthread_cond_init(&not_full, NULL);
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
  curr_masters_size = num_masters;

   buffer = (int *)malloc (sizeof(int) * max_buf_size);
   thread_count = (int *)malloc (sizeof(int) * num_workers);
   threshold = total_items/num_workers;

  if(total_items%num_workers!=0){
    threshold++;
  }

   for(int c=0;c<num_workers;c++){
    thread_count[c] = 0;
   }


   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;
  
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
      curr_masters_size--;
    }

  for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }

  
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(thread_count);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);
  //pthread_mutex_destroy(&qlock);
  
  return 0;
}
