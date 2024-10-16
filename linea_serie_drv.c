#include "linea_serie_drv.h"
#include "linea_serie_hal.h"
#include "eventos.h"
#include "gpio_hal.h"
#include "io_reserva.h"
#include "conversor.h"
#include "System_calls_hal.h"
#include <stdint.h>

#define MAX_BUFFER_SALIDA 1000

static int indiceCarga;
static char carga[3];
static int tamCarga;
static void(*FIFO_encolar)(EVENTO_T, uint32_t);


typedef enum {
    VACIO,
    LLENO
}estadoBuffer;

static estadoBuffer estado;

struct envio{
    char dato[MAX_BUFFER_SALIDA];
    int primero;
    int ultimo;
    int tamano;
};

static struct envio buffer;

// FUNCIONES COLA

void buffer_inicializar() {
    int i;
    for(i=0; i<MAX_BUFFER_SALIDA; i++) {
        buffer.dato[i] = 0;
    }
    buffer.ultimo = buffer.primero = buffer.tamano = 0;
}

int buffer_tamano() {
    return buffer.tamano;
}

char buffer_extraer() {
    char result = 0;
    if(buffer.tamano > 0) {

        result = buffer.dato[buffer.primero];
        buffer.primero = (buffer.primero + 1) % MAX_BUFFER_SALIDA;
        buffer.tamano--;
    }
    return result;
}

void buffer_encolar(char* palabra, int length) {
		/*deshabilitar interrupciones*/
		int I = read_irq_bit();
		disable_irq();
	
    int i;
    if((buffer.tamano + length) < MAX_BUFFER_SALIDA) { //cabe en cola
        for(i=0; i<length; i++) {
            buffer.dato[buffer.ultimo] = palabra[i];
            buffer.ultimo = (buffer.ultimo + 1) % MAX_BUFFER_SALIDA;
        }
        buffer.tamano += length;
    } else {
        GPIO_hal_escribir(31, 1, 1);
    }
		if(I == 0) {
			enable_irq();
		}
}

// FIN FUNCIONES COLA


void linea_serie_drv_inicializar(void(*funcion_encolar_evento)(EVENTO_T, uint32_t)){
    FIFO_encolar = funcion_encolar_evento;
    linea_serie_hal_inicializar(&linea_serie_drv_maquina_estados, &linea_serie_drv_continuar_envio);
    indiceCarga = 0;
    tamCarga = 0;
    estado = VACIO;
		buffer_inicializar();
}

/* inicializará las estructuras necesarias
y enviará el primer carácter*/
void linea_serie_drv_enviar_array(char envio[], int length){
	if(length == 0 ) {
		return;
	}
	/*si está enviando un string, basta con encolar el nuevo envío ya que se enviará después del envío actual.
	Si no se está enviando nada, se crea un nuevo envío.*/
	int tamano = buffer_tamano();
	buffer_encolar(envio,length);
	if(tamano == 0) { //buffer está vacío
		linea_serie_hal_escribir(buffer_extraer());
	} 
}

/*enviará el siguiente
carácter hasta llegar al final del buffer*/
void linea_serie_drv_continuar_envio(void) {
    if(buffer_tamano() > 0){
        linea_serie_hal_escribir(buffer_extraer());
    }
    else { //buffer vacío
        FIFO_encolar(ev_TX_SERIE, 0);
    }
}

void linea_serie_drv_maquina_estados(){
    char c = linea_serie_hal_leer();
		linea_serie_drv_enviar_array(&c, 1);
    switch(estado){
        case(VACIO):    
            switch (c)
            {
            case ('$'):
                indiceCarga = 0;
                tamCarga = 0;
                break;
            case ('!'):
								linea_serie_drv_enviar_array("\n\n", 2);
                if (tamCarga == 3){
                    FIFO_encolar(ev_comprobar, atoi(carga)); //planificador comprobará si los caracteres recibidos son correctos
                }
                break;    
            default:
                carga[indiceCarga] = c;
                tamCarga++;
                indiceCarga++;
                if(tamCarga > 3){
                    GPIO_hal_escribir(GPIO_SERIE_ERROR, 1, 1);
                    estado = LLENO;
                }
                break;
            }
        break;
        
        case(LLENO):
            switch(c){
                case('$'):
                    indiceCarga = 0;
                    tamCarga = 0;
                    estado = VACIO;
                    if(GPIO_hal_leer(GPIO_SERIE_ERROR, 1) == 1){
                        GPIO_hal_escribir(GPIO_SERIE_ERROR, 1 ,0);
                    }
                    break;
            }
        break;
    }
}


