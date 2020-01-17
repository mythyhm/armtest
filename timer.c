#include "comm.h"

typedef struct {
    S3C24X0_REG32   TCNTB;
    S3C24X0_REG32   TCMPB;
    S3C24X0_REG32   TCNTO;
} S3C24X0_TIMER;

typedef struct {
    S3C24X0_REG32   TCFG0;
    S3C24X0_REG32   TCFG1;
    S3C24X0_REG32   TCON;
    S3C24X0_TIMER   ch[4];
    S3C24X0_REG32   TCNTB4;
    S3C24X0_REG32   TCNTO4;
} S3C24X0_TIMERS;

#define S3C24X0_TIMER_BASE      0x51000000
#define PCLK 50000000        //50MHZ

int timer_init (void)
{
	S3C24X0_TIMERS * const timers = (S3C24X0_TIMERS * const)S3C24X0_TIMER_BASE;

	/* use PWM Timer 4 because it has no output */
	/* prescaler for Timer 4 is 16 */
	timers->TCFG0 = 0x0f00;

	/* load value for 10 ms timeout */
	timers->TCNTB4 = PCLK/(2 * 16 * 100);
	/* auto load, manual update of Timer 4 */
	timers->TCON = (timers->TCON & ~0x0700000) | 0x600000;
	/* auto load, start Timer 4 */
	timers->TCON = (timers->TCON & ~0x0700000) | 0x500000;

	return (0);
}