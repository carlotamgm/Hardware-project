#include "power.h"
#include <LPC210x.H>                       /* LPC210x definitions */

extern void Setup_PLL(void); /*importa la función de inicialización del PLL de Startup.s*/

/* Duerme el procesador. Detiene el reloj del procesador, mientras que los periféricos en el chip permanecen activos. 
Cualquier interrupción habilitada de un periférico o una fuente de interrupción externa provocará que el procesador reanude la ejecución*/
void power_hal_wait() {

	PCON |= 0x01; //activa modo idle
}

/* Pasa al estado power-down, los periféricos también entran en bajo
consumo y dejan de funcionar. El procesador despertará si recibe una interrupción externa*/
void power_deep_sleep () {
	EXTWAKE = 6; // EINT1 despierta al procesador de power-down
	PCON |= 0x02; 
	/*paraliza los relojes del oscilador y de todos los chips. Una interrupción externa
		reanuda el oscilador y la ejecución del procesador. Bit se pone a 0*/
	Setup_PLL();
	/*Volver a habilitar el PLL al volver de power-down*/
}
