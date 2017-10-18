#include "events.h"

static uint8_t head = 0, tail = 0;
static event_t ievents[MAXEVENTS];

void event_push(event_t *ev)
{
   ievents[tail] = *ev; 
   tail = (tail + 1) % MAXEVENTS;
}

event_t *event_pop()
{
    if(head == tail){
        return nullptr;
    }

    event_t *result = &ievents[head];
    head = (head + 1) % MAXEVENTS;

    return result;
}
