#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include <string.h>
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "devices/timer.h"
#define N 10
#define BUFSIZE 3

static int buffer[BUFSIZE];
static int bufin = 0, bufout = 0;

static struct semaphore empty, full;
static struct lock buffer_lock;

static void producer(void *arg UNUSED) {
   int i, item;

   for (i = 0; i < N; i++) {
      sema_down(&empty);
      item = i * i;

      lock_acquire(&buffer_lock);
      buffer[bufin] = item;
      bufin = (bufin + 1) % BUFSIZE;
      lock_release(&buffer_lock);

      printf("p: put item %d\n", item);
      sema_up(&full);
   }
}

static void consumer(void *arg UNUSED) {
   int i, item;

   for (i = 0; i < N; i++) {
      sema_down(&full);

      lock_acquire(&buffer_lock);
      item = buffer[bufout];
      bufout = (bufout + 1) % BUFSIZE;
      lock_release(&buffer_lock);

      printf("c: get item %d\n", item);
      sema_up(&empty);
   }
}

void test_prod_cons(void) {
   lock_init(&buffer_lock);
   sema_init(&full, 0);
   sema_init(&empty, BUFSIZE);

   thread_create("producer", PRI_DEFAULT, producer, NULL);
   thread_create("consumer", PRI_DEFAULT, consumer, NULL);
}

