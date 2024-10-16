#ifndef LINEA_SERIE_DRV_H
#define LINEA_SERIE_DRV_H

#include "eventos.h"
#include <stdint.h>

void linea_serie_drv_inicializar(void(*funcion_encolar_evento)(EVENTO_T, uint32_t));


/* inicializará las estructuras necesarias
y enviará el primer carácter*/
void linea_serie_drv_enviar_array(char* envio, int length);

/*enviará el siguiente
carácter hasta llegar al final del buffer*/
void linea_serie_drv_continuar_envio(void);

void linea_serie_drv_maquina_estados(void);

#endif
