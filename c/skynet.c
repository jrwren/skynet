#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chan.h"

pthread_attr_t attr;
void init_pthread_attr() {
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, sysconf(_SC_PAGESIZE));
}

typedef struct skynet_args_t {
    chan_t *c;
    int num;
    int size;
    int div;
} skynet_args_t;

void* skynet(void *v);
void skynet_thread(chan_t *c, int num, int size, int div){
    pthread_t th;
    skynet_args_t *skynet_args = malloc(sizeof(skynet_args_t));
    skynet_args->c = c;
    skynet_args->num = num;
    skynet_args->size = size;
    skynet_args->div = div;
    printf("  creating thread with %d %d %d\n", num, size, div);
    pthread_create(&th, &attr, skynet, (void*)&skynet_args);
}

void* skynet(void *v) {
    skynet_args_t *arg = (skynet_args_t *)v;
    if (arg->size == 1) {
        // c <- num
        printf("       !!! size 1, sending %d\n ", arg->num);
        chan_send(arg->c, (void *) (uintptr_t) arg->num);
    } else {
        //rc := make(chan int)
        chan_t *rc;
        chan_init(0);
        int sum = 0;
        // for i := 0; i < div; i++ {
        for(int i=0; i < arg->div; i++) {
            int sub_num = arg->num + i * (arg->size / arg->div);
            // go skynet(rc, sub_num, size / div, div)
            printf("calling w/%d, %d, %d\n",sub_num, arg->size / arg->div, arg->div);
            skynet_thread(rc, sub_num, arg->size / arg->div, arg->div);
        }
        // for i := 0; i < div; i++ {
        for(int i=0; i < arg->div; i++) {
            // sum += <-rc
            void *j;
            chan_recv(rc, &j);
            sum += (int) (uintptr_t) j;
            printf("     got %d\n", (int) (uintptr_t) j);
        }
        // c <- sum
        printf("---sending %d\n", sum);
        chan_send(arg->c, (void *) (uintptr_t) sum);
    }
    //free(v);
    return NULL;
}

void main() {
    chan_t *c = chan_init(0);
    init_pthread_attr();
    clock_t start = clock();
    // go skynet(c, 0, 1000000, 10)
    skynet_thread(c, 0, 1000000, 10);
    int *result;
    // result := <-c
    chan_recv(c, (void **)&result);
    int end = clock();
    printf("Result: %d in %.4f s.\n", *result, (double)(end - start)/CLOCKS_PER_SEC);
    chan_dispose(c);
}
