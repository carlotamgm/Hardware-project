#include <LPC210X.H>
#include "linea_serie_hal.h"

typedef void (*Funcion)();
static Funcion funcion_callback_RDA;
static Funcion funcion_callback_THRE;

/*Interrupción de línea serie*/
void linea_serie_ISR(void) __irq{
    VICVectAddr = 0;                       // Desactivar interrupción
		char irq = U0IIR;
    if((irq & 0x04) == 4){ //Interrupt Identification Register
        //se ha recibido un dato
        funcion_callback_RDA(); // Receive Data Available

    }
    else if((irq & 0x02) == 2){
        //se ha enviado un dato, es decir, el buffer de envío esta vacío
        funcion_callback_THRE(); 
    }
}

/*Inicializa las estructuras del línea serie*/
void linea_serie_hal_inicializar(void(*funcion_callback1)(), void(*funcion_callback2)()){
		PCONP |= 8;
    PINSEL0 = PINSEL0 & 0xFFFFFFF0;
    PINSEL0 |= 0x05;
		U0LCR = 0x83; //Line Control Register (formato datos): 8bits de longitud y Enable access to Divisor Latches (DLAB)
    
    U0DLL = 97;  //Divisor Latch registers (divisor de reloj PCLK a UART)
    U0LCR = 0x03;
		U0IER = 0x03; // Interrupt Enable Register: enable THRE and RDA interrupts
    VICVectCntl5 = 0x20 | 6;
    VICVectAddr5 = (unsigned long)linea_serie_ISR;
    VICIntEnable = 0x00000040;


    funcion_callback_RDA = funcion_callback1;
    funcion_callback_THRE = funcion_callback2;
}

/*Escribe en el línea serie el caracter c*/
void linea_serie_hal_escribir(char c){
    U0THR = c; //Transmit Holding Register
}

/*Lee el caracter del línea serie*/
char linea_serie_hal_leer(void){
    return U0RBR; //Receiver Buffer Register 
}
