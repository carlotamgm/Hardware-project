#ifndef EVENTOS_H
#define EVENTOS_H

/*Definición del tipo de datos EVENTO_T: Conjunto de eventos posibles. En
el fichero de cabecera se incluirá el tipo de datos y el conjunto de
posibles eventos identificados con nombre humano. Reservemos el ID
VOID con valor cero para inicializar la cola.*/
typedef enum{
    ID_VOID = 0,
	ID_ALARMA,
	ALARMA_OVERFLOW,
	ID_PULSACION1,
	ID_PULSACION2,
	ID_AUSENTE,
	ev_VISUALIZAR_HELLO,
	ev_LATIDO,
	PULSACION,
	ev_comprobar,
	ev_confirmar_jugada,
	ev_TX_SERIE,
	ev_RX_SERIE, 
	ev_cancelar_jugada,
	// añadir aquí eventos
	ev_NUM_EVENTOS
  
}  EVENTO_T;


#define NUM_EVENTOS ev_NUM_EVENTOS /*número de tipos de eventos*/

#endif
