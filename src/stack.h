#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdint.h>

typedef struct{
	uint8_t* stack;
	size_t size;
	size_t memb_size;
} Stack;

#define Stack_Create(type) Stack_CreateWithSize(sizeof(type))

Stack* Stack_CreateWithSize(size_t memb_size);

void Stack_Push(Stack* stack, void* member);

void Stack_Pop(Stack* stack);

void* Stack_Get(Stack* stack, size_t index);

void Stack_Clean(Stack* stack);

void Stack_Remove(Stack* stack, size_t index);

void* Stack_Top(Stack* stack);

void Stack_Destroy(Stack* stack);

#endif
