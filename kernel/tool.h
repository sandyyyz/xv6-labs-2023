#ifndef TOOL_H
#define TOOL_H
#include"page.h"
#include "riscv.h"
unsigned long ROUNDDOWN2(unsigned long number);
unsigned long ROUNDUP2(unsigned long n);
unsigned long get_order_2(unsigned long n);
unsigned long power(unsigned long base, unsigned long exp);
#define power2(exp) (power(2,exp))
#define SPLIT_CHILD_PFN(original_pfn,original_order) \
				((unsigned long)((char*)PFN2PA((original_pfn))) + ((1 << original_order - 1) * PGSIZE))
typedef __SIZE_TYPE__ size_t;
#endif