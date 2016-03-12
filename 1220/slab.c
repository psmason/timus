#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define CHUNK 10
#define SLAB_COUNT 11000

// size of this slab:
// next = 4 bytes
// pos  = 2 bytes
// data = 10*4 = 40 bytes
// TOTAL:
//   46

typedef struct Slab Slab;
struct Slab 
{
  Slab*   next;
  int16_t pos;
  int32_t data[CHUNK];
};

Slab  slabs[SLAB_COUNT];
Slab* freeSlabs;

void initFreeSlabs()
{
  freeSlabs = &slabs[0];
  int i;
  for (i=0; i<SLAB_COUNT-1; ++i) {
    slabs[i].next = &slabs[i+1];
  }
}

Slab* getFreeSlab()
{
  Slab* freeSlab = freeSlabs;
  freeSlabs      = freeSlabs->next;
  return freeSlab;
}

void addToFreeSlabs(Slab* slab)
{
  slab->next = freeSlabs;
  freeSlabs  = slab;
}

typedef struct Stack Stack;
struct Stack
{
  Slab* root;
  Slab* slab;
};

#define STACK_COUNT 1000
Stack stacks[1000];

void pushToStack(int stack, int value)
{
  Stack* stackPtr = &stacks[stack-1];
  if (NULL == stackPtr->root) {
    stackPtr->root = getFreeSlab();
    stackPtr->slab = stackPtr->root;
  }

  int pos = stackPtr->slab->pos;
  if (CHUNK == pos) {
    Slab* newSlab = getFreeSlab();
    stackPtr->slab->next = newSlab;
    stackPtr->slab       = newSlab;
    stackPtr->slab->next = NULL;
    pos                  = stackPtr->slab->pos;
  }

  stackPtr->slab->data[pos] = value;
  ++(stackPtr->slab->pos);
}

Slab* getPrevious(Stack* stack, Slab* slab)
{
  Slab* currentSlab = stack->root;
  while (currentSlab->next != slab) {
    currentSlab = currentSlab->next;
  }
  return currentSlab;
}

void popStack(int stack)
{
  Stack* stackPtr = &stacks[stack-1];
  
  if (0 == stackPtr->slab->pos) {
    Slab* previousSlab = getPrevious(stackPtr, stackPtr->slab);
    
    addToFreeSlabs(stackPtr->slab);
    stackPtr->slab       = previousSlab;
    stackPtr->slab->next = NULL;
  }

  int pos = stackPtr->slab->pos;
  printf("%d\n", stackPtr->slab->data[pos-1]);
  --(stackPtr->slab->pos);
}

void parsePush()
{
  int stack, value;
  scanf("%d %d", &stack, &value);
  pushToStack(stack, value);
}

void parsePop()
{
  int stack;
  scanf("%d", &stack);
  popStack(stack);
}

int main(int argc, char* argv[])
{
  initFreeSlabs();

  int commands;
  scanf("%d", &commands);

  for (;commands>0;--commands) {
    char command[5];
    scanf("%s", command);
    if (0 == strcmp("PUSH", command)) {
      parsePush();
    }
    else {
      parsePop();
    }
  }
}


