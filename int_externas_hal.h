#ifndef INT_EXTERNAS_H
#define INT_EXTERNAS_H

#include <LPC210X.H>                            // LPC21XX Peripheral Registers
#include <stdint.h>

void eint1_clear_nueva_pulsacion(void);

unsigned int eint1_read_nueva_pulsacion(void);

unsigned int eint1_read_cuenta(void);

void eint1_init (void) ;


void eint2_clear_nueva_pulsacion(void);

unsigned int eint2_read_nueva_pulsacion(void);

unsigned int eint2_read_cuenta(void);

void eint2_init (void) ;


//funcion para ver si el boton de eint1 sigue pulsado o no
uint32_t int_externas_hal_leer_EXTINT_eint1(void);

//funcion para ver si el boton de eint2 sigue pulsado o no
uint32_t int_externas_hal_leer_EXTINT_eint2(void);

void int_externas_hal_config(void (*funcion_callback)());

//Habilita interrupciones de EINT1
void activar_EINT1(void);

//Habilita interrupciones de EINT2
void activar_EINT2(void);

#endif
