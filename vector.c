//Fuente: http://happybearsoftware.com/implementing-a-dynamic-array.html
// vector.c

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void vector_init(Vector *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->data
  vector->data = malloc(sizeof(int) * vector->capacity);
  vector->paquete = malloc(sizeof(char) * vector->capacity);
}

void vector_append(Vector *vector, int value, char paquete) {
  // make sure there's room to expand into
  vector_double_capacity_if_full(vector);

  // append the value and increment vector->size
   int size=vector->size++;
    vector->data[size] = value;
    vector->paquete[size] = paquete;
}

int vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(1);
  }
  return vector->data[index];
}

char vector_get_PAQUETE(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(1);
  }
  return vector->paquete[index];
}

void vector_set(Vector *vector, int index, int value, char paquete) {  
  // set the value at the desired index
  vector->data[index] = value;
  vector->paquete[index] = paquete;
}

void vector_double_capacity_if_full(Vector *vector) {
  if (vector->size >= vector->capacity) {
    // double vector->capacity and resize the allocated memory accordingly
    vector->capacity *= 2;
    vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
  }
}

void desencolar(Vector * vector){
  int i;
  for(i=0; i<vector->size -1;i++){
    vector_set(vector,i,vector_get(vector,i+1),vector_get_PAQUETE(vector,i+1));
  }
  vector->size--;
}

int vector_size(Vector *vector){
  return vector->size ;
}
void vector_free(Vector *vector) {
  free(vector->data);
}