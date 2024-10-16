#ifndef SYSTEM_CALLS_HAL_H
#define SYSTEM_CALLS_HAL_H

int __swi(0x01) read_irq_bit(void);

void __swi(0xFF) enable_irq(void);

void __swi(0xFE) disable_irq(void);

void __swi(0xFD) disable_fiq(void);

#endif
