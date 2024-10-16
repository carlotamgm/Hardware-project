#include "temporizador_drv.h"

/*
temporizador_drv_iniciar(): función que programa un contador para
que pueda ser utilizado.
*/
void temporizador_drv_iniciar(void) {
    temporizador_hal_iniciar();
}

/*
temporizador_drv_empezar(): función que inicia la cuenta de un
contador de forma indefinida.
*/
void temporizador_drv_empezar(void) {
    temporizador_hal_empezar();
}

// uint64_t temporizador_drv_leer(): función que lee el tiempo que lleva
// contando el contador desde la última vez que se ejecutó
// temporizador_hal_empezar y lo devuelve en ticks.
uint64_t temporizador_drv_leer(void) {
    return temporizador_hal_leer() / temporizador_hal_ticks2us;
}

// uint64_t temporizador_drv_parar(): detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el último temporizador_drv_empezar.
uint64_t temporizador_drv_parar(void) {
    return temporizador_hal_parar() /	temporizador_hal_ticks2us;
}

/*función que programa el reloj para que encole un evento periódicamente en la
cola del planificador. El periodo se indica en ms.*/
void temporizador_drv_reloj (uint32_t periodo, void(*funcion_encolar_evento)(EVENTO_T, uint32_t), EVENTO_T ID_evento, uint32_t auxData) {

    temporizador_hal_reloj(periodo, funcion_encolar_evento, ID_evento, auxData);
}

/*para leer el tiempo transcurrido en microsegundos utilizando el
temporizador. Envuelve a la llamada temporizador_drv_leer que ya no podrá
ser invocada directamente desde espacio de usuario.*/

uint32_t __SWI_0 (void) {
    return (uint32_t) temporizador_drv_leer();
}
