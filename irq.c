#include "comm.h"

extern int serial_getc (void);
extern void serial_puts (const char *s);
extern void ledOn();

#define ISR_EINT0_OFT     0
#define ISR_EINT1_OFT     1
#define ISR_EINT2_OFT     2
#define ISR_EINT3_OFT     3
#define ISR_EINT4_7_OFT   4
#define ISR_EINT8_23_OFT  5
#define ISR_NOTUSED6_OFT  6
#define ISR_BAT_FLT_OFT   7
#define ISR_TICK_OFT      8
#define ISR_WDT_OFT       9
#define ISR_TIMER0_OFT    10
#define ISR_TIMER1_OFT    11
#define ISR_TIMER2_OFT    12
#define ISR_TIMER3_OFT    13
#define ISR_TIMER4_OFT    14
#define ISR_UART2_OFT     15
#define ISR_LCD_OFT       16
#define ISR_DMA0_OFT      17
#define ISR_DMA1_OFT      18
#define ISR_DMA2_OFT      19
#define ISR_DMA3_OFT      20
#define ISR_SDI_OFT       21
#define ISR_SPI0_OFT      22
#define ISR_UART1_OFT     23
#define ISR_NOTUSED24_OFT 24
#define ISR_USBD_OFT      25
#define ISR_USBH_OFT      26
#define ISR_IIC_OFT       27
#define ISR_UART0_OFT     28
#define ISR_SPI1_OFT      29
#define ISR_RTC_OFT       30
#define ISR_ADC_OFT       31

// PENDING BIT
#define BIT_EINT0		(0x1)
#define BIT_EINT1		(0x1<<1)
#define BIT_EINT2		(0x1<<2)
#define BIT_EINT3		(0x1<<3)
#define BIT_EINT4_7		(0x1<<4)
#define BIT_EINT8_23	(0x1<<5)
#define BIT_CAM			(0x1<<6)		// Added for 2440.
#define BIT_BAT_FLT		(0x1<<7)
#define BIT_TICK			(0x1<<8)
#define BIT_WDT_AC97	(0x1<<9)
#define BIT_TIMER0		(0x1<<10)
#define BIT_TIMER1		(0x1<<11)
#define BIT_TIMER2		(0x1<<12)
#define BIT_TIMER3		(0x1<<13)
#define BIT_TIMER4		(0x1<<14)
#define BIT_UART2		(0x1<<15)
#define BIT_LCD			(0x1<<16)
#define BIT_DMA0		(0x1<<17)
#define BIT_DMA1		(0x1<<18)
#define BIT_DMA2		(0x1<<19)
#define BIT_DMA3		(0x1<<20)
#define BIT_SDI			(0x1<<21)
#define BIT_SPI0			(0x1<<22)
#define BIT_UART1		(0x1<<23)
#define BIT_NFCON		(0x1<<24)		// Added for 2440.
#define BIT_USBD		(0x1<<25)
#define BIT_USBH		(0x1<<26)
#define BIT_IIC			(0x1<<27)
#define BIT_UART0		(0x1<<28)
#define BIT_SPI1			(0x1<<29)
#define BIT_RTC			(0x1<<30)
#define BIT_ADC			(0x1<<31)
#define BIT_ALLMSK		(0xffffffff)

#define BIT_SUB_ALLMSK	(0x7fff)
#define BIT_SUB_AC97 	(0x1<<14)
#define BIT_SUB_WDT 	(0x1<<13)
#define BIT_SUB_CAM_S	(0x1<<12)		// Added for 2440.
#define BIT_SUB_CAM_C	(0x1<<11)		// Added for 2440.
#define BIT_SUB_ADC		(0x1<<10)
#define BIT_SUB_TC		(0x1<<9)
#define BIT_SUB_ERR2	(0x1<<8)
#define BIT_SUB_TXD2	(0x1<<7)
#define BIT_SUB_RXD2	(0x1<<6)
#define BIT_SUB_ERR1	(0x1<<5)
#define BIT_SUB_TXD1	(0x1<<4)
#define BIT_SUB_RXD1	(0x1<<3)
#define BIT_SUB_ERR0	(0x1<<2)
#define BIT_SUB_TXD0	(0x1<<1)
#define BIT_SUB_RXD0	(0x1<<0)

#define S3C24X0_INTERRUPT_BASE      0x4A000000

typedef struct {
    S3C24X0_REG32   SRCPND;
    S3C24X0_REG32   INTMOD;
    S3C24X0_REG32   INTMSK;
    S3C24X0_REG32   PRIORITY;
    S3C24X0_REG32   INTPND;
    S3C24X0_REG32   INTOFFSET;
    S3C24X0_REG32   SUBSRCPND;
    S3C24X0_REG32   INTSUBMSK;
}S3C24X0_INTERRUPT;

void (*isr_handle_array[50])(void);
S3C24X0_INTERRUPT * intregs;

void Dummy_isr(void)
{
    return;
}

void do_fiq()
{
	return;
}

void do_not_used()
{
	return;
}

void do_data_abort()
{
	return;
}

void do_prefetch_abort()
{
	return;
}

void do_software_interrupt()
{
	return;
}

void do_undefined_instruction()
{
	return;
}

void enable_interrupts (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}

int disable_interrupts (void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return (old & 0x80) == 0;
}

void ClearPending(int bit) 
{
    intregs->SRCPND = bit;
    intregs->INTPND = bit;
}

void ClearSubPending(int bit) 
{
    intregs->SUBSRCPND = bit;
}

void IsrUart0()
{
    char c;	
	//S3C24X0_UART * const uart = (S3C24X0_UART * const)(S3C24X0_UART_BASE + (0 * 0x4000));
	
	volatile u32 * pUFCON = ((volatile u32 *)(0x50000008));
	
	//serial_puts("IsrUart0 \n"); 
	//barrier();
	//c = serial_getc();
	//barrier();
	//serial_putc('\n');
	//barrier();
	//serial_putc(c);
	
	if (intregs->SUBSRCPND&BIT_SUB_RXD0 == BIT_SUB_RXD0)
		serial_puts("BIT_SUB_RXD0 \n"); 
	else
		serial_puts("no BIT_SUB_RXD0 \n"); 
	
	//intregs->INTMSK &=~(BIT_UART0);
	//intregs->INTSUBMSK &=~(BIT_SUB_RXD0);
	
	*pUFCON |= 0x2;
	
	ClearSubPending(BIT_SUB_RXD0);
	ClearPending(BIT_UART0);
	
	//Dma_start();
}

typedef struct {
	long    quot;
	long    rem;
} ldiv_t;

//extern ldiv_t ldiv (long int numer, long int denom);

void isrTmr4()
{
	static u32 u32count = 0;
	
	u32count++;
	
	//if (ldiv(u32count, 100).rem == 0)
	//if (u32count%100 == 0)		
	//	serial_puts("isrTmr4!\n"); 
	
	ClearPending(BIT_TIMER4);
}

void isrDma0()
{
#if 0
    volatile u32 *psrc = (volatile u32 *)0x33F80000;
	volatile u32 *pdst = (volatile u32 *)0x34000000;
	u16 u16len = 0;
	
	while ((psrc[u16len] == pdst[u16len]) && ++u16len<0x400);
	
	if (u16len < 0x400)
		serial_puts("dma transfer failed!!\n");
	else
	    serial_puts("dma transfer ok!!\n");
	
#endif

    volatile u8 *pdst = (volatile u8 *)0x34000000;  

    if ((*pdst) == 'g')
	    serial_puts("dma input g!!\n");
	else
		serial_puts("dma input other!!\n");
	
	ClearPending(BIT_DMA0);
	
}

void Isr_Init(void)
{
    int i = 0;
    intregs = (S3C24X0_INTERRUPT * const)S3C24X0_INTERRUPT_BASE;
    
    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++ )
    {
        isr_handle_array[i] = Dummy_isr;
    }

    intregs->INTMOD=0x0;	      // All=IRQ mode
    intregs->INTMSK=BIT_ALLMSK;	  // All interrupt is masked.

    isr_handle_array[ISR_UART0_OFT] = IsrUart0;
	isr_handle_array[ISR_TIMER4_OFT] = isrTmr4;
	isr_handle_array[ISR_DMA0_OFT] = isrDma0;
	intregs->INTMSK &=~(BIT_UART0); 	
	intregs->INTSUBMSK &=~(BIT_SUB_RXD0); 
	intregs->INTMSK &=~(BIT_DMA0);
	//intregs->INTMSK &=~(BIT_TIMER4);
	
	ClearSubPending(BIT_SUB_RXD0);
    ClearPending(BIT_UART0);
	ClearPending(BIT_DMA0);
	
	//ClearPending(BIT_TIMER4);

	enable_interrupts();
}


void IRQ_Handle()
{
	unsigned long oft = intregs->INTOFFSET;
	
	disable_interrupts();
	//ledOn();
      
	//intregs->SRCPND = 1<<oft;	
	//intregs->INTPND	= intregs->INTPND;	 

    /* run the isr */
    isr_handle_array[oft]();

	//enable_interrupts();
}