/* guarda para evitar inclusiones múltiples ("include guard") */
#ifndef CONECTA_K_2023_H
#define CONECTA_K_2023_H

#include <inttypes.h>
#include "celda.h"
#include "tablero.h"

/* *****************************************************************************
 * declaracion de funciones visibles en el exterior conecta_K
 */

// función principal del juego
void conecta_K_jugar(void);

// devuelve la longitud de la línea más larga
uint8_t conecta_K_buscar_alineamiento_c(TABLERO *t, uint8_t fila,
	uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna);

// devuelve true si encuentra una línea de longitud mayor o igual a 4
uint8_t conecta_K_hay_linea_c_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

// carga el estado a mitad de partida de las primeras 7 filas y columnas 
// a la estructura de datos tablero para facilitar el test de posibles jugadas
//
// 0: casilla vacia, 1:ficha jugador uno, 2: ficha jugador dos
void conecta_K_test_cargar_tablero(TABLERO *t);


// funciones a implementar en ARM

// devuelve la longitud de la línea más larga en un determinado sentido
uint8_t conecta_K_buscar_alineamiento_arm(TABLERO *t, uint8_t
	fila, uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna);

// devuelve true si encuentra una línea de longitud mayor o igual a K
uint8_t conecta_K_hay_linea_arm_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

// devuelve true si encuentra una línea de longitud mayor o igual a K
uint8_t conecta_K_hay_linea_arm_arm(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);



/* *****************************************************************************
 * declaración funciones internas conecta_K
 */

int conecta_K_verificar_K_en_linea(TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color);

#endif /* CONECTA_K_2023_H */
