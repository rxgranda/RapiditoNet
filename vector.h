//Fuente: http://happybearsoftware.com/implementing-a-dynamic-array.html
// vector.h


#define VECTOR_INITIAL_CAPACITY 100

// Define a vector type
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  int *data;     // array of integers we're storing
  char *paquete;
} Vector;

void vector_init(Vector *vector);

void vector_append(Vector *vector, int valuem, char paquete);

int vector_get(Vector *vector, int index);

char vector_get_PAQUETE(Vector *vector, int index) ;

void vector_set(Vector *vector, int index, int value, char paquete);

void vector_double_capacity_if_full(Vector *vector);

int vector_size(Vector *vector);

void desencolar();

void vector_free(Vector *vector);