#ifndef PLANIFICADOR_HAL_H
#define PLANIFICADOR_HAL_H


/*El planificador inicializará la cola indicando que el GPIO para marcar el
overflow es el GPIO31 definido convenientemente en la cabecera
io_reserva.h como GPIO_OVERFLOW = 31 y de longitud GPIO_OVERFLOW_BITS =1. 
El planificador básicamente estará a la espera pendiente de la cola de eventos.
Cuando aparezca un evento nuevo no tratado, lo procesará.
*/
void planificador(void);



#endif
