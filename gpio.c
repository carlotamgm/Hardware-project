                  
#include <LPC210x.H>                       /* LPC210x definitions */
#include "planificador.h"
#include "gpio_hal.h"
#include "temporizador_drv.h"

// Nota: wait2 es una espera activa. Se puede eliminar poniendo el procesador en modo iddle. Probad a hacerlo
void wait2 (void)  {                         /* wait2 function */
  unsigned int i;

  i = clock_get_us(); // reads the number of previous timer IRQs
  while ((i + 10) != clock_get_us());              /* wait2s for 10 interrupts, i.e. 50ms */
}

int main2 (void) {
  unsigned int j;                           /* LED var */
	 
	//eint0_init(); // activates EINT0 interrupts
	// Nota la gestión del GPIO vosotros la debeís hacer en GPIO.c no en el main2 o en el reversi
  	GPIO_hal_iniciar();
	GPIO_hal_sentido(16, 7, GPIO_HAL_PIN_DIR_OUTPUT);					//Set LED pins as outputs
	GPIO_hal_escribir(16, 7, 0);					//Initialices the outputs to 0
	GPIO_hal_leer(16,2);
	planificador();

// bucle que realiza un blink de leds cada 50ms	   
	temporizador_drv_iniciar(); // generates an interrupt every 0,05ms and increments timeval0
	temporizador_drv_empezar();
	while (1)  {    
		/* Loop forever */
    for (j = 0x010000; j < 0x800000; j <<= 1) { /* Blink LED 0,1,2,3,4,5,6 */
      // Nota la gestión del GPIO vosotros la debeís hacer en GPIO.c no en el main2 o en el reversi
			
			GPIO_HAL_PIN_T pin = j;
			uint8_t num_bits = 1; // Configurando un solo bit como salida
			gpio_hal_pin_dir_t direccion = GPIO_HAL_PIN_DIR_OUTPUT;
	
			GPIO_hal_sentido(pin, num_bits, direccion);
			GPIO_hal_escribir(pin, num_bits, 1);
			wait2();
			direccion = GPIO_HAL_PIN_DIR_INPUT;
	
			GPIO_hal_sentido(pin, num_bits, direccion);
			GPIO_hal_escribir(pin, num_bits, 0);

    }
    for (j = 0x800000; j > 0x010000; j >>=1 ) { /* Blink LED 7,6,5,4,3,2,1 */
      // Nota la gestión del GPIO vosotros la debeís hacer en GPIO.c no en el main2 o en el reversi
		IOSET = j;                               /* Turn on LED */
		wait2 ();                                  /* call wait2 function */
		IOCLR = j;                               /* Turn off LED */
    }
  }
}


//int main2 (void) {
//  	
//  reversi8();
//	
//}
                       // LPC21XX Peripheral Registers
