#ifndef BOTONES_H
#define BOTONES_H
#include "eventos.h"
#include "int_externas_hal.h"
#include "alarmas.h"

/*Inicializa los botones*/
void botones_init(void(*funcion_encolar_evento)(EVENTO_T, uint32_t));

//funcion al volver de interrupcion
void gestionar_pulsacion(uint32_t boton);

//maquina estados 1
void maquina_estados_EINT1(void);

//maquina estados 2
void maquina_estados_EINT2(void);

#endif

