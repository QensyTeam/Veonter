#ifndef _CPUID_H_
#define _CPUID_H_

int do_intel(void);
int do_amd(void);

void printregs(int eax, int ebx, int ecx, int edx);

#define cpuid(in, a, b, c, d) __asm__("cpuid": "=a" (a), \
                                               "=b" (b), \
                                               "=c" (c), \
                                               "=d" (d) : \
                                               "a" (in));

int detect_cpu(void);

#endif // _CPUID_H_
