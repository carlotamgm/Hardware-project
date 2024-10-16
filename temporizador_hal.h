#include <stdint.h>
#include "eventos.h"
#ifndef TEMPORIZADOR_HAL_H
#define TEMPORIZADOR_HAL_H
/*
la constante temporizador_hal_ticks2us: permite convertir de ticks a
microsegundos
*/
// PCLK = 15 ticks/ 1 µs
#define temporizador_hal_ticks2us 15

/*
temporizador_hal_iniciar(): función que programa un contador para
que pueda ser utilizado.
*/
void temporizador_hal_iniciar(void);

/*
temporizador_hal_empezar(): función que inicia la cuenta de un
contador de forma indefinida.
*/
void temporizador_hal_empezar(void);

// uint64_t temporizador_hal_leer(): función que lee el tiempo que lleva
// contando el contador desde la última vez que se ejecutó
// temporizador_hal_empezar y lo devuelve en ticks.
uint64_t temporizador_hal_leer(void);

// uint64_t temporizador_hal_parar(): detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el último temporizador_hal_empezar.
uint64_t temporizador_hal_parar(void) ;

/*función dependiente del hardware (timer1)
que programa el reloj para que llame a la función de callback cada
periodo. El periodo se indica en ms. Si el periodo es cero se para el
temporizador.*/
void temporizador_hal_reloj(uint32_t periodo, void (*funcion_callback)(EVENTO_T,uint32_t), EVENTO_T id_evento, uint32_t auxData) ;

#endif
