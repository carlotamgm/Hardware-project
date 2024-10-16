#include "Watchdog_hal.h"
#include <LPC210X.H>                       /* LPC210x definitions */
#include "System_calls_hal.h"
#include <stdio.h>

/*inicializa el watchdog timer para que
resetee el procesador dentro de sec segundos si no se le “alimenta”.*/
void WD_hal_inicializar(int sec) {
    WDTC  = sec*15000000/4;			// Set watchdog time out value (multiplica por hercios del procesador y divide para 4)
    WDMOD = 0x03;                /* Enable watchdog timer (bit 0) and reset (bit 1).  */   
}

/*alimenta al watchdog timer*/
void WD_hal_feed(void) {
    int I = read_irq_bit();
    /*deshabilitar interrupciones*/
    disable_irq();
    WDFEED = 0xAA;						   
    WDFEED = 0x55;  
    /*habilitar interrupciones si es necesario*/
    if(I == 1){
        enable_irq();
    }
} 

/*test con un bucle infinito y comprobar que el watchdog
realmente funciona.*/
void WD_hal_test(void) {

    while(1) {
		
    }
	
}
