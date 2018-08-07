#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>

Queue_t* initQ(){
	Queue_t *q = calloc(1, sizeof(Queue_t));
	q->rear = Q_DEFAULT_SIZE -1;
	q->front = q->rear;
        return q;	
}


bool_t
is_queue_empty(Queue_t *q){
	if(q->count == 0)
		return TRUE;
	return FALSE;
}


bool_t
is_queue_full(Queue_t *q){
	if(q->count == Q_DEFAULT_SIZE)
		return TRUE;
	return FALSE;
}

bool_t
enqueue(Queue_t *q, void *ptr){
	if(!q || !ptr) return FALSE;
	if(q && is_queue_full(q)){ 
		//printf("Queue is full\n");
		return FALSE;
	}
	
	if(is_queue_empty(q)){
		q->elem[q->rear] = ptr;
		//printf("element inserted at index = %d\n", q->rear);
		q->count++;
		return TRUE;
	}
		
	if(q->rear == 0){
		if(q->front == Q_DEFAULT_SIZE -1 && is_queue_full(q)){
			//printf("Queue is full\n");
			return FALSE;
		}
		q->rear = Q_DEFAULT_SIZE -1;
		q->elem[q->rear] = ptr;
		q->count++;
		//printf("element inserted at index = %d\n", q->rear);
		return TRUE;
	}

	q->rear--;
	q->elem[q->rear] = ptr;
	q->count++;
	//printf("element inserted at index = %d\n", q->rear);
	return TRUE;
}

void*
deque(Queue_t *q){
	if(!q) return NULL;
	if(is_queue_empty(q))
		return NULL;

	void *elem = q->elem[q->front];
	q->elem[q->front] = NULL;
	// for last elem
	if(q->front == q->rear){
		q->count--;
		return elem;
	}

	if(q->front == 0)
		q->front = Q_DEFAULT_SIZE -1;
	else
		q->front--;
	q->count--;
	return elem;
}

void
print_Queue(Queue_t *q){
	uint32_t i = 0;
	printf("q->front = %d, q->rear = %d, q->count = %d\n", q->front, q->rear, q->count);
	for(i = 0; i < Q_DEFAULT_SIZE; i++){
		#if 1
		if(q->elem[i] == NULL)
			continue;
		#endif
		printf("index = %u, elem = 0x%x\n", i, (uint32_t)q->elem[i]);
	}
}

#if 0
int 
main(int argc, char **argv){
return 0;	
}
#endif
