#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  pthread_mutex_init(&(rw->main_lock), NULL);
  rw->num_readers = 0;
  rw->num_writers_waiting = 0;
  rw->writer_present = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&(rw->main_lock));
  while(rw->writer_present || rw->num_writers_waiting) pthread_cond_wait(&(rw->cond_reader), &(rw->main_lock));
  rw->num_readers++;
  pthread_mutex_unlock(&rw->main_lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&(rw->main_lock));
  rw->num_readers--;
  if(rw->num_readers == 0) pthread_cond_signal(&(rw->cond_writer));
  pthread_mutex_unlock(&rw->main_lock);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&(rw->main_lock));
  rw->num_writers_waiting++;
  while(rw->num_readers || rw->writer_present) pthread_cond_wait(&(rw->cond_writer), &(rw->main_lock));
  rw->num_writers_waiting--;
  rw->writer_present = 1;
  pthread_mutex_unlock(&(rw->main_lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&(rw->main_lock));
  rw->writer_present = 0;
  if(rw->num_writers_waiting == 0) pthread_cond_broadcast(&(rw->cond_reader));
  else pthread_cond_signal(&(rw->cond_writer));
  pthread_mutex_unlock(&(rw->main_lock));
}
