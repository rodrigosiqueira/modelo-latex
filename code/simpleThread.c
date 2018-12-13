/**
* Código acessado em 12/12/2018 e baseado no exemplo fornecido em:
* http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * thread_kernel( void * message_pointer ) {
  char * message = NULL;
  message = (char *) message_pointer;
  printf("%s \n", message);
  return NULL;
}

int main(int argc, char ** argv) {
  pthread_t thread1, thread2;
  const char * message1 = "Thread 1";
  const char * message2 = "Thread 2";
  int rc1 = 0, rc2 = 0;

  rc1 = pthread_create( &thread1, NULL, thread_kernel, (void*) message1);
  if (rc1) {
    fprintf(stderr, "pthread_create(): %d\n", rc1);
    exit(EXIT_FAILURE);
  }

  rc2 = pthread_create( &thread2, NULL, thread_kernel, (void*) message2);
  if (rc2) {
    fprintf(stderr, "pthread_create(): %d\n", rc2);
    exit(EXIT_FAILURE);
  }

  pthread_join( thread1, NULL );
  pthread_join( thread2, NULL ); 

  return EXIT_SUCCESS;
}
