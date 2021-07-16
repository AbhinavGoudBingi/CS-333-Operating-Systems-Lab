#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
	rw->general_lock = PTHREAD_MUTEX_INITIALIZER;
	rw->writers_waiting = 0;
	rw->readers = 0;
	rw->writer_present = false;
	rw->enter_writer = PTHREAD_COND_INITIALIZER;
	rw->enter_reader = PTHREAD_COND_INITIALIZER;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    pthread_mutex_lock(&(rw->general_lock));
    if(rw->writers_waiting > 0 || rw->writer_present){
		pthread_cond_wait(&(rw->enter_reader),&(rw->general_lock));
	}
	rw->readers++;
	pthread_mutex_unlock(&(rw->general_lock));
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&(rw->general_lock));
	rw->readers--;
	if(rw->readers == 0){
		pthread_cond_broadcast(&(rw->enter_writer));
	}
	pthread_mutex_unlock(&(rw->general_lock));
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&(rw->general_lock));
	rw->writers_waiting++;
	while(rw->readers > 0 || rw->writer_present){
		pthread_cond_wait(&(rw->enter_writer),&(rw->general_lock));
	}
	rw->writers_waiting--;
	rw->writer_present = true;
	pthread_mutex_unlock(&(rw->general_lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&(rw->general_lock));
	rw->writer_present = false;
	if(rw->writers_waiting == 0){
		pthread_cond_broadcast(&(rw->enter_reader));
	}
	else{
		pthread_cond_broadcast(&(rw->enter_writer));
	}
	pthread_mutex_unlock(&(rw->general_lock));
}
