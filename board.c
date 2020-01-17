#include "comm.h"


extern ulong IRQ_STACK_START;	/* top of IRQ stack */
extern ulong _armboot_start;	/* code start */
extern void enable_interrupts (void);
extern void Isr_Init(void);


#define S3C24X0_UART_BASE       0x50000000
#define S3C24X0_GPIO_BASE       0x56000000
#define S3C2440_MPLL_400MHZ     ((0x5c<<12)|(0x01<<4)|(0x01))
#define S3C2440_MPLL_200MHZ     ((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_100MHZ     ((0x5c<<12)|(0x01<<4)|(0x03))
#define S3C2440_UPLL_96MHZ      ((0x38<<12)|(0x02<<4)|(0x01))
#define S3C2440_UPLL_48MHZ      ((0x38<<12)|(0x02<<4)|(0x02))
#define S3C2440_CLKDIV          (0x05) // | (1<<3))    /* FCLK:HCLK:PCLK = 1:4:8, UCLK = UPLL/2 */
#define S3C2440_CLKDIV188       0x04    /* FCLK:HCLK:PCLK = 1:8:8 */
#define S3C2440_CAMDIVN188      ((0<<8)|(1<<9)) /* FCLK:HCLK:PCLK = 1:8:8 */

typedef struct {
    S3C24X0_REG32   ULCON;
    S3C24X0_REG32   UCON;
    S3C24X0_REG32   UFCON;
    S3C24X0_REG32   UMCON;
    S3C24X0_REG32   UTRSTAT;
    S3C24X0_REG32   UERSTAT;
    S3C24X0_REG32   UFSTAT;
    S3C24X0_REG32   UMSTAT;

    S3C24X0_REG8    UTXH;
    S3C24X0_REG8    res1[3];
    S3C24X0_REG8    URXH;
    S3C24X0_REG8    res2[3];
	
    S3C24X0_REG32   UBRDIV;
}S3C24X0_UART;

typedef struct {
    S3C24X0_REG32   GPACON;
    S3C24X0_REG32   GPADAT;
    S3C24X0_REG32   res1[2];
    S3C24X0_REG32   GPBCON;
    S3C24X0_REG32   GPBDAT;
    S3C24X0_REG32   GPBUP;
    S3C24X0_REG32   res2;
    S3C24X0_REG32   GPCCON;
    S3C24X0_REG32   GPCDAT;
    S3C24X0_REG32   GPCUP;
    S3C24X0_REG32   res3;
    S3C24X0_REG32   GPDCON;
    S3C24X0_REG32   GPDDAT;
    S3C24X0_REG32   GPDUP;
    S3C24X0_REG32   res4;
    S3C24X0_REG32   GPECON;
    S3C24X0_REG32   GPEDAT;
    S3C24X0_REG32   GPEUP;
    S3C24X0_REG32   res5;
    S3C24X0_REG32   GPFCON;
    S3C24X0_REG32   GPFDAT;
    S3C24X0_REG32   GPFUP;
    S3C24X0_REG32   res6;
    S3C24X0_REG32   GPGCON;
    S3C24X0_REG32   GPGDAT;
    S3C24X0_REG32   GPGUP;
    S3C24X0_REG32   res7;
    S3C24X0_REG32   GPHCON;
    S3C24X0_REG32   GPHDAT;
    S3C24X0_REG32   GPHUP;
    S3C24X0_REG32   res8;

    S3C24X0_REG32   MISCCR;
    S3C24X0_REG32   DCLKCON;
    S3C24X0_REG32   EXTINT0;
    S3C24X0_REG32   EXTINT1;
    S3C24X0_REG32   EXTINT2;
    S3C24X0_REG32   EINTFLT0;
    S3C24X0_REG32   EINTFLT1;
    S3C24X0_REG32   EINTFLT2;
    S3C24X0_REG32   EINTFLT3;
    S3C24X0_REG32   EINTMASK;
    S3C24X0_REG32   EINTPEND;
    S3C24X0_REG32   GSTATUS0;
    S3C24X0_REG32   GSTATUS1;
    S3C24X0_REG32   GSTATUS2;
    S3C24X0_REG32   GSTATUS3;
    S3C24X0_REG32   GSTATUS4;

    /* s3c2440 */
    S3C24X0_REG32   res9[4];
    S3C24X0_REG32   GPJCON;
    S3C24X0_REG32   GPJDAT;
    S3C24X0_REG32   GPJUP;
} /*__attribute__((__packed__))*/ S3C24X0_GPIO;

typedef struct {
    S3C24X0_REG32   LOCKTIME;
    S3C24X0_REG32   MPLLCON;
    S3C24X0_REG32   UPLLCON;
    S3C24X0_REG32   CLKCON;
    S3C24X0_REG32   CLKSLOW;
    S3C24X0_REG32   CLKDIVN;
    S3C24X0_REG32   CAMDIVN;    /* for s3c2440, by www.arm9.net */
} /*__attribute__((__packed__))*/ S3C24X0_CLOCK_POWER;

static inline void delay (unsigned long loops)
{
    __asm__ volatile ("1:\n"
      "subs %0, %1, #1\n"
      "bne 1b":"=r" (loops):"0" (loops));
}

void ledOn(u8 u8index)
{	
	S3C24X0_GPIO * const gpio = (S3C24X0_GPIO * const)S3C24X0_GPIO_BASE;
	u32 u32len = 5000000;
	u32 u32gpbV = 0;
	
	if (u8index > 3)
		return;
	
	u32gpbV = 0x1<<(10+u8index*2);
	gpio->GPBCON = u32gpbV;

	//while(1)
	while(u32len--)
		gpio->GPBDAT = 0x0;
	
}


void serial_init()
{
    S3C24X0_UART * const uart = (S3C24X0_UART * const)(S3C24X0_UART_BASE + (0 * 0x4000));
	int i;
	unsigned int reg = 0;

	/* value is calculated so : (int)(PCLK/16./baudrate) -1 */
	reg = 50000000 / (16 * 115200) - 1;

	/* FIFO enable, Tx/Rx FIFO clear */
	uart->UFCON = 0x7;
	//uart->UFCON = 0x07;
	uart->UMCON = 0x0;
	/* Normal,No parity,1 stop,8 bit */
	uart->ULCON = 0x3;
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	//uart->UCON = 0x245;
	uart->UCON = 0x246; //for dma0
	uart->UBRDIV = reg;

	for (i = 0; i < 100; i++);

	return;
}

void gpio_init()
{
    S3C24X0_GPIO * const gpio = (S3C24X0_GPIO * const)S3C24X0_GPIO_BASE;

    /* set up the I/O ports */
    gpio->GPACON = 0x007FFFFF;
    gpio->GPBCON = 0x00044555;
    gpio->GPBUP = 0x000007FF;
    gpio->GPCCON = 0xAAAAAAAA;
    gpio->GPCUP = 0x0000FFFF;
    gpio->GPDCON = 0xAAAAAAAA;
    gpio->GPDUP = 0x0000FFFF;
    gpio->GPECON = 0xAAAAAAAA;
    gpio->GPEUP = 0x0000FFFF;
    gpio->GPFCON = 0x000055AA;
    gpio->GPFUP = 0x000000FF;
    gpio->GPGCON = 0xFF95FFBA;
    gpio->GPGUP = 0x0000FFFF;
    gpio->GPHCON = 0x002AFAAA;
    gpio->GPHUP = 0x000007FF;

	return;
}

void clock_init()
{
	S3C24X0_CLOCK_POWER *clk_power = (S3C24X0_CLOCK_POWER *)0x4C000000;

        /* FCLK:HCLK:PCLK = 1:4:8 */
    clk_power->CLKDIVN = S3C2440_CLKDIV;

    /* change to asynchronous bus mod */
    __asm__(    "mrc    p15, 0, r1, c1, c0, 0\n"    /* read ctrl register   */  
                "orr    r1, r1, #0xc0000000\n"      /* Asynchronous         */  
                "mcr    p15, 0, r1, c1, c0, 0\n"    /* write ctrl register  */  
                :::"r1"
                );

    /* to reduce PLL lock time, adjust the LOCKTIME register */
    clk_power->LOCKTIME = 0xFFFFFFFF;

    /* configure UPLL */
    clk_power->UPLLCON = S3C2440_UPLL_48MHZ;

    /* some delay between MPLL and UPLL */
    delay (4000);

    /* configure MPLL */
    clk_power->MPLLCON = S3C2440_MPLL_400MHZ;

    /* some delay between MPLL and UPLL */
    delay (8000);

	//ledOn();
	
}

void serial_putc (const char c)
{
	S3C24X0_UART * const uart = (S3C24X0_UART * const)(S3C24X0_UART_BASE + (0 * 0x4000));

	/* wait for room in the tx FIFO */
	while (!(uart->UTRSTAT & 0x2));

	uart->UTXH = c;

	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');
}


void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}


int CopyCode2Ram(unsigned long start_addr, unsigned char *buf, int size)
{
    unsigned int *pdwDest;
    unsigned int *pdwSrc;
    int i;

    pdwDest = (unsigned int *)buf;
    pdwSrc  = (unsigned int *)start_addr;
    /* �?NOR Flash启动 */
    for (i = 0; i < size / 4; i++)
    {
        pdwDest[i] = pdwSrc[i];
    }
    return 0;
}

#if 0
void set_sysMode (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x1f\n"
				 "orr %0, %0, #0x13\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}
#endif

void board_init()
{
	u32 u32count = 10;
	IRQ_STACK_START = _armboot_start -64 - 4;
			
	gpio_init();
	serial_init();	
	Isr_Init();
    //timer_init();	
	Dma_init();
    //ledOn(0);
	//Dma_start();

	serial_puts("\n\n*****Init complete!!!*******\n");			

	while (1)
	{
		ledOn(1);
		serial_puts("board_init loop\n");
		//set_sysMode();
		//gph_init();
		//Isr_Init();s
		//ser_co_init();
	}

	return;
}

