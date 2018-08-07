#define uint32_t unsigned int
typedef enum _bool{
        FALSE = 0,
        TRUE
} bool_t;

#define Q_DEFAULT_SIZE  50
typedef struct _Queue{
        void *elem[Q_DEFAULT_SIZE];
        uint32_t front;
        uint32_t rear;
        uint32_t count;
} Queue_t;

Queue_t* initQ();

bool_t
is_queue_empty(Queue_t *q);

bool_t
is_queue_full(Queue_t *q);

bool_t
enqueue(Queue_t *q, void *ptr);

void*
deque(Queue_t *q);

void
print_Queue(Queue_t *q);
