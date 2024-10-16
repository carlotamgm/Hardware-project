
#ifndef HELLO_WORLD_HAL_T
#define HELLO_WORLD_HAL_T
#include "gpio_hal.h"
#include "eventos.h"

/*Inicializa los pins de GPIO.*/
void hello_world_inicializar(GPIO_HAL_PIN_T pin, uint8_t num_bits, void(*FIFO_encolar)(EVENTO_T, uint32_t)) ;

/*Actualiza el contador y el estado de los leds. */
void hello_world_tick_tack(void);

void hello_world_tratar_evento(void);

#endif
