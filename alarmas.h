#ifndef ALARMAS_H
#define ALARMAS_H
#include "eventos.h"
#include "temporizador_drv.h"
#include "power.h"

#define MAX_ALARMAS 4
#define O_PERIODICA 0x80000000 /*máscara para marcar el primer bit de retardo para alarma periódica*/


/*Inicializa el array de alarmas*/
void alarma_inicializar(void(*funcion_encolar_evento)(EVENTO_T, uint32_t));

/*Programa una alarma para el evento ID_evento y la información auxData, en un periodo retardo
que encolará el evento de forma única. Si se modifica el retardo a uno menor, se sobreescribe en la misma alarma. */
void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t
auxData);

/*Cancela la alarma programada para ID_evento.*/
void alarma_desactivar(EVENTO_T ID_evento);

/*Comprueba si hay que disparar el evento asociado a alguna de las alarmas programadas pendientes.*/
void alarma_tratar_evento(void);


#endif


