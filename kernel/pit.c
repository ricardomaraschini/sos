#include <system.h>

#define PIT_HZ		1193180
#define PIT_DATA0	0x40
#define PIT_DATA1	0x41
#define PIT_DATA2	0x42
#define PIT_CMD		0x43

#define MAX_EVENTS	256

unsigned long ticks = 0;
struct scheduled_event global_events[MAX_EVENTS];

void
dispatch_events()
{
	int i;
	void (*handler)();

	for(i=0; i<MAX_EVENTS; i++) {
		if (global_events[i].every > 0) {
			handler = global_events[i].handler;
			if (ticks % global_events[i].every == 0) {
				if (handler)
					handler();
			}
		}
	}
}

void
timer_callback(struct regs *r) {
	ticks++;
	dispatch_events();
}

void
event_handler() {
	putint(ticks);
	putch('\n');
}

int
register_event(unsigned long every, void (*handler)()) {
	int i;

	for (i=0; i<MAX_EVENTS; i++) {
		if (global_events[i].every == 0) {
			global_events[i].every = every;
			global_events[i].handler = handler;
			return 0;
		}
	}

	return -1;
}

void
empty_events_queue() {
	int i;

	for (i=0; i<MAX_EVENTS; i++) {
		global_events[i].every = 0;
		global_events[i].handler = NULL;
	}
}

void
init_timer(int frequency) {
	
	int divisor;
	unsigned char low;
	unsigned char high;

	irq_install_handler(IRQ_PIT,timer_callback);

	divisor = PIT_HZ / frequency;

	// command to set frequency
	outportb(PIT_CMD, 0x36);

	low = divisor & 0xFF;
	high = (divisor >> 8) & 0xFF;

	outportb(PIT_DATA0,low);
	outportb(PIT_DATA0,high);

	empty_events_queue();

	register_event(100, event_handler);

}
