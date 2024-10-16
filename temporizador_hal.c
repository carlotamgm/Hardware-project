#include <LPC210X.H>                            // LPC21XX Peripheral Registers
#include <stdint.h>
#include "temporizador_hal.h"


#define T0MR0_val 0xFFFFFFFE				//Interrumpe cada MUCHO


// variable para contabilizar el n�mero de interrupciones
static volatile unsigned int timer0_irq_count = 0;

void timer0_ISR (void) __irq {
    timer0_irq_count++;
    T0IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                       // Acknowledge Interrupt
}

/*
temporizador_hal_iniciar(): función que programa un contador para
que pueda ser utilizado.
*/
void temporizador_hal_iniciar(void) {
    // Set the value of the match register 0 for timer 0 to 150.
    // llegará una IRQ cada 150 ticks
    T0MR0 = T0MR0_val;
    // Set Timer 0 Match Control Register to enable interrupt and reset on match
    T0MCR = 0x03;

     // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
		VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
		VICVectCntl0 = 0x20 | 4;                   
    VICIntEnable = VICIntEnable | 0x00000010;                  // Enable Timer0 Interrupt

}

/*
temporizador_hal_empezar(): función que inicia la cuenta de un
contador de forma indefinida.
*/
void temporizador_hal_empezar(void) {
    timer0_irq_count = 0;
    T0TCR = 0x01;                             // Timer0 Enable
}

// uint64_t temporizador_hal_leer(): función que lee el tiempo que lleva
// contando el contador desde la última vez que se ejecutó
// temporizador_hal_empezar y lo devuelve en ticks.
uint64_t temporizador_hal_leer(void) {
    return T0MR0_val * timer0_irq_count + T0TC; //valor de TC real
}

// uint64_t temporizador_hal_parar(): detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el último temporizador_hal_empezar.
uint64_t temporizador_hal_parar(void) {
    T0TCR = 3;                             // Timer0 Reset 
    T0TCR = 0;                             // Timer0 disable
    return temporizador_hal_leer();
}


/*Timer1*/

static EVENTO_T evento;
static uint32_t auxData;
static void (*funcion_callback1)(EVENTO_T evento, uint32_t auxData);

void timer1_ISR (void) __irq {
   
	  (*funcion_callback1)(evento, auxData); //Llamar a la función
	  T1IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                       // Desactivar interrupción
}


/*función dependiente del hardware (timer1)
que programa el reloj para que llame a la función de callback cada
periodo. El periodo se indica en ms. Si el periodo es cero se para el
temporizador.*/
void temporizador_hal_reloj(uint32_t periodo, void (*funcion_callback)(EVENTO_T, uint32_t), EVENTO_T id_evento, uint32_t auxDat) {
    if(periodo == 0) {
        T1TCR = 3;                             // Timer1 Reset 
        T1TCR = 0;                             // Timer1 disable
        return;
    }
		
    funcion_callback1 = funcion_callback;
		evento = id_evento;
		auxData = auxDat;
    T1MR0 = periodo*1000*temporizador_hal_ticks2us;
    // Set Timer 1 Match Control Register to enable interrupt and reset on match
    T1MCR = 3;
		T1TCR = 1;                             // Timer1 Enable
     // configuration of the IRQ slot number 0 of the VIC for Timer 1 Interrupt
		VICVectAddr1 = (unsigned long)timer1_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 5 is the number of the interrupt assigned. Number 5 is the Timer 1 (see table 40 of the LPC2105 user manual  
		VICVectCntl1 = 0x20 | 5;                   
    VICIntEnable = VICIntEnable | 0x00000020;                  // Enable Timer1 Interrupt

}
    


