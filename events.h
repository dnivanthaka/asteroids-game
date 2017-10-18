#ifndef EVENTS_H
#define EVENTS_H

#include <cstdint>

#define MAXEVENTS 32


typedef enum sevent_t{
    //TODO Add joystick events
    KEYUP = 0,
    KEYDOWN,
    QUIT
} sevent_t;

typedef struct event_t{
    sevent_t event;
    uint8_t data1, data2;
} event_t;

void event_push(event_t *ev);
event_t *event_pop();
#endif
