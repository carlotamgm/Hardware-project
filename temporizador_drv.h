#include <stdint.h>
#include <stdio.h>
#include "temporizador_hal.h"
#include "eventos.h"

#ifndef TEMPORIZADOR_DRV_HAL_H
#define TEMPORIZADOR_DRV_HAL_H

/*
temporizador_drv_iniciar(): función que programa un contador para
que pueda ser utilizado.
*/
void temporizador_drv_iniciar(void) ;

/*
temporizador_drv_empezar(): función que inicia la cuenta de un
contador de forma indefinida.
*/
void temporizador_drv_empezar(void);

// uint64_t temporizador_drv_leer(): función que lee el tiempo que lleva
// contando el contador desde la última vez que se ejecutó
// temporizador_hal_empezar y lo devuelve en ticks.
//uint64_t temporizador_drv_leer(void) ;

// uint64_t temporizador_drv_parar(): detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el último temporizador_drv_empezar.
uint64_t temporizador_drv_parar(void);


/*función que programa el reloj para que encole un evento periódicamente en la
cola del planificador. El periodo se indica en ms.*/
void temporizador_drv_reloj (uint32_t periodo, void(*funcion_encolar_evento)(EVENTO_T, uint32_t), EVENTO_T ID_evento, uint32_t auxData);

/*para leer el tiempo transcurrido en microsegundos utilizando el
temporizador. Envuelve a la llamada temporizador_drv_leer que ya no podrá
ser invocada directamente desde espacio de usuario.*/
uint32_t __swi(0) clock_get_us(void);

#endif
