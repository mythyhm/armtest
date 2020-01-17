#include "comm.h"

#define S3C24X0_DMA_BASE        0x4B000000

#define   DMD_HS    31
#define   SYNC      30
#define   INT       29
#define   TSZ       28
#define   SERVMODE  27
#define   HWSRCSEL  24
#define   SWHW_SEL  23
#define   RELOAD    22
#define   DSZ       20
#define   TC        0
  

typedef struct {
    S3C24X0_REG32   DISRC;
    S3C24X0_REG32   DISRCC;
    S3C24X0_REG32   DIDST;
    S3C24X0_REG32   DIDSTC;
    S3C24X0_REG32   DCON;
    S3C24X0_REG32   DSTAT;
    S3C24X0_REG32   DCSRC;
    S3C24X0_REG32   DCDST;
    S3C24X0_REG32   DMASKTRIG;
    S3C24X0_REG32   res[7];
} S3C24X0_DMA;

typedef struct {
    S3C24X0_DMA dma[4];
} S3C24X0_DMAS;

S3C24X0_DMAS *pgDmasEntity = {0};

void Dma_init()
{
	serial_puts("\n Dma_init\n");
	pgDmasEntity = (S3C24X0_DMAS * const)S3C24X0_DMA_BASE;
	
	pgDmasEntity->dma[0].DISRC = 0x50000024;
	pgDmasEntity->dma[0].DISRCC = 0x3;
	pgDmasEntity->dma[0].DIDST = 0x34000000;
	pgDmasEntity->dma[0].DIDSTC = 0x0;
	pgDmasEntity->dma[0].DMASKTRIG = 0x2;
    pgDmasEntity->dma[0].DCON = 0x1<<TC  |
	                            0x0<<DSZ   |
								0x0<<RELOAD |
								0x1<<SWHW_SEL |
								0x001<<HWSRCSEL |
								0x1<<SERVMODE |
								0x0<<TSZ |
								0x1<<INT |
								0x0<<SYNC |
								0x1<<DMD_HS;
		            
}

void Dma_start()
{
	//if ((pgDmasEntity->dma[0].DSTAT>>20)&0x3 == 0)
	{		
	    //serial_puts("Dma_start\n");
	    //pgDmasEntity->dma[0].DMASKTRIG = 0x2;
		//if (pgDmasEntity->dma[0].DMASKTRIG == 0x2)
		//	pgDmasEntity->dma[0].DMASKTRIG = 0x3;
		//else
		//	serial_puts("Dma_start failed\n");
    }		
}

