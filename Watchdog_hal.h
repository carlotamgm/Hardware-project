#ifndef WATCHDOG_H
#define WATCHDOG_H

/*inicializa el watchdog timer para que
resetee el procesador dentro de sec segundos si no se le “alimenta”.*/
void WD_hal_inicializar(int sec);

/*alimenta al watchdog timer*/
void WD_hal_feed(void);

/*test con un bucle infinito y comprobar que el watchdog
realmente funciona.*/
void WD_hal_test(void) ;

#endif
