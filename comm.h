#ifndef COMM_H
#define COMM_H

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef volatile u8     S3C24X0_REG8;
typedef volatile u16    S3C24X0_REG16;
typedef volatile u32    S3C24X0_REG32;

typedef unsigned long		ulong;

#define barrier() __asm__ __volatile__("": : :"memory")

#endif