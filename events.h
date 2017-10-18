#ifndef EVENTS_H
#define EVENTS_H

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

static uint8_t head, tail;

void push_event(event_t *ev);
event_t *pop_event();
#endif
