                  
#include <LPC210x.H>                       /* LPC210x definitions */
#include "temporizador_drv.h"
#include "System_calls_hal.h"
#include "conversor.h"
#include "linea_serie_drv.h"
#include "planificador.h"
#include "fifo.h"

// Nota: wait es una espera activa. Se puede eliminar poniendo el procesador en modo iddle. Probad a hacerlo
void wait (void)  {                         /* wait function */
  unsigned int i;

  i = clock_get_us(); // reads the number of previous timer IRQs
  while ((i + 10) != clock_get_us());              /* waits for 10 interrupts, i.e. 50ms */
}


int main (void) {

  disable_fiq();
	enable_irq();

  temporizador_drv_iniciar();

  linea_serie_drv_inicializar(FIFO_encolar);

  planificador();


}

