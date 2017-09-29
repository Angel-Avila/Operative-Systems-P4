#define NEW 0
#define END 1
#define READY 2
#define RUNNING 3
#define BLOCKED 4


#define TIMER 0x000
#define ENDTHREAD 0x100
#define NEWTHREAD 0x200
#define BLOCKTHREAD 0x300
#define UNBLOCKTHREAD 0x400

#define MAXTHREAD 10

typedef struct _QUEUE {
	int elements[MAXTHREAD];
	int head;
	int tail;
} QUEUE;

typedef struct THREAD {
	int tid;			// Identificador del hilo
	int status;			// Status END(1),RUNNING(3),BLOCKED(4)
	int sleeptime;
	int retstat;		// Valor de retorno
	ucontext_t thread_context;	// Contexto 
	char iterator_stack[SIGSTKSZ];
	struct THREAD *next;
} THANDLER; 