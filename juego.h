#ifndef JUEGO_H
#define JUEGO_H

#include "eventos.h"
#include "temporizador_drv.h"
#include "celda.h"
#include "tablero.h"
#include "System_calls_hal.h"
#include "linea_serie_drv.h"
#include "tablero.h"
#include "conversor.h"
#include "alarmas.h"
#include "conversor.h"
#include "conecta_K_2023.h"
#include "gpio_hal.h"
#include "io_reserva.h"

#include <stdio.h>
#include <string.h>

/* Juego realiza las tareas necesarias para su inicialización */
void juego_inicializar(uint32_t (*funcion_estadisticas_cola)(EVENTO_T));

/* Le da el turno al jugador contrario, teniendo en cuenta el jugador inicial de la última partida */
void alternar_jugador(uint8_t* jugador);

/*Con cada nueva pulsación, juego crea el evento
ev_VISUALIZAR_CUENTA colocando la cuenta en el campo auxiliar*/
void juego_tratar_evento(EVENTO_T ID_evento, char* comando);

void visualizar_jugada_provisional(TABLERO *t,  uint8_t memoria[8][8], uint8_t fila, uint8_t columna, uint8_t color);

// funcion que visualiza en "pantalla" el contenido de las 7 primeras filas y columnas 
// de las m*n del tablero en juego (suponemos que m y n son >= 7 siempre)
void conecta_K_visualizar_tablero2(TABLERO *t, uint8_t pantalla[8][8]);

// visualiza en "pantalla" el contenido de las 7 primeras filas y columnas 
// de las m*n del tablero con la jugada provisional (suponemos que m y n son >= 7 siempre)
void conecta_K_visualizar_tablero_provisional(TABLERO *t, uint8_t memoria[8][8]);

/* 
Muestra por pantalla: 
o Tiempo total de uso de procesador en esta partida (sin power-down).
o Total y media de tiempo de computo de conecta_K_hay_linea.
o Total y media de tiempo que al humano le cuesta pensar jugada.
o Total de eventos encolados en esta partida e histograma por tipo de evento.
*/
void mostrarEstadisticas(void);

/*Inicialización del gestor de errores en comandos. Se le pasa como parámetro el pin 
del GPIO utilizado para marcar errores y marca el GPIO029 para indicar error.*/
void error_inicializar(GPIO_HAL_PIN_T pin_error);

#endif
