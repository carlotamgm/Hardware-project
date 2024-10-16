#ifndef LINEA_SERIE_HAL_H
#define LINEA_SERIE_HAL_H

/*Inicializa las estructuras del línea serie*/
void linea_serie_hal_inicializar(void(*funcion_callback1)(), void(*funcion_callback2)());

/*Escribe en el línea serie el caracter c*/
void linea_serie_hal_escribir(char c);

/*Lee el caracter del línea serie*/
char linea_serie_hal_leer(void);

#endif
