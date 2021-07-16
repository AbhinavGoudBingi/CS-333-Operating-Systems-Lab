#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
	rw->general_lock = PTHREAD_MUTEX_INITIALIZER;
	rw->write_lock = PTHREAD_MUTEX_INITIALIZER;
	rw->readers = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
	pthread_mutex_lock(&(rw->general_lock));
	rw->readers++;
	if(rw->readers == 1){
		pthread_mutex_lock(&(rw->write_lock));
	}
	pthread_mutex_unlock(&(rw->general_lock));
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&(rw->general_lock));
	rw->readers--;
	if(rw->readers == 0){
		pthread_mutex_unlock(&(rw->write_lock));
	}
	pthread_mutex_unlock(&(rw->general_lock));
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&(rw->write_lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
	pthread_mutex_unlock(&(rw->write_lock));
}
