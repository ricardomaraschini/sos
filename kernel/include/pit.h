#ifndef __PIT_H
#define __PIT_H

#define PIT_HZ		1193180
#define PIT_DATA0	0x40
#define PIT_DATA1	0x41
#define PIT_DATA2	0x42
#define PIT_CMD		0x43
#define MAX_EVENTS	256

void init_timer(u32int frequency);

struct scheduled_event {
	unsigned long every;
	void (*handler)(void);
};

#endif
