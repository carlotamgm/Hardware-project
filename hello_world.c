#include "hello_world.h"
#include "alarmas.h"
#include "temporizador_drv.h"

static void(*FIFO_encolar)(EVENTO_T, uint32_t);
static GPIO_HAL_PIN_T pin;
static uint8_t num_bits;

/*Inicializa los pins de GPIO y programará una alarma para que cada 10ms se genere el evento
ev_LATIDO.*/
void hello_world_inicializar(GPIO_HAL_PIN_T _pin, uint8_t _num_bits, void (*_FIFO_encolar)(EVENTO_T, uint32_t)) {
	FIFO_encolar = _FIFO_encolar;
	pin = _pin;
	num_bits = _num_bits;
	GPIO_hal_sentido(pin, num_bits, GPIO_HAL_PIN_DIR_OUTPUT);
	GPIO_hal_escribir(pin, num_bits, 0);
	alarma_activar(ev_LATIDO, O_PERIODICA|10, 0);
}

/*Actualiza el contador y el estado de los leds. */
void hello_world_tick_tack() {

	static uint32_t valor=0;
	valor = GPIO_hal_leer(pin, num_bits);
	valor++; //actualizar contador
	//escribir en los primeros 8 pines del GPIO
	GPIO_hal_escribir(pin, num_bits, valor);
  
}	

void hello_world_tratar_evento(void) {
	FIFO_encolar(ev_VISUALIZAR_HELLO, 0);
}


