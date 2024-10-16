#include "fifo.h"
#include "io_reserva.h"
#include "System_calls_hal.h"

#define MAX_SIZE 32 // Tamaño máximo de la cola

//Estructura que representa el contenido de la cola circular 
typedef struct {
	EVENTO_T evento;
	uint32_t datoAux;
} Evento;


// Estructura que representa una cola circular
typedef struct {

  	Evento datos[MAX_SIZE];
	uint8_t anterior;
	uint8_t numVeces[NUM_EVENTOS];
  	uint8_t ultimo;
  	int tamano;
	GPIO_HAL_PIN_T pin_overflow;
} CircularQueue ;


static CircularQueue queue; 

/*Inicialización de la cola. Se le pasa como parámetro el pin del GPIO utilizado para marcar
errores.*/
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow) {

	int i;
    queue.ultimo = 0;
	queue.anterior = 0;
	for(i=0; i<queue.tamano; i++) {
		queue.datos[i].evento = ID_VOID;
	}
	for(i=0; i<NUM_EVENTOS; i++) {
		queue.numVeces[i] = 0;
	}
    queue.tamano = 0;

	//overflow
	queue.pin_overflow = pin_overflow;
	GPIO_hal_sentido (queue.pin_overflow, 1/*GPIO_OVERFLOW_BITS*/, GPIO_HAL_PIN_DIR_OUTPUT);
	GPIO_hal_escribir(queue.pin_overflow, GPIO_OVERFLOW_BITS, 0);
}

 /*Esta función guardará en la cola el evento. El campo ID_evento, que permita
identificar el evento (p.e. qué interrupción ha saltado) y el campo
auxData en caso de que el evento necesite pasar información extra.*/
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData) {

	int I = read_irq_bit();
	/*deshabilitar interrupciones*/
	disable_irq();
	
	if (queue.tamano < MAX_SIZE) {
        queue.datos[queue.ultimo].evento = ID_evento;
        queue.datos[queue.ultimo].datoAux = auxData;
        queue.ultimo = (queue.ultimo + 1) % MAX_SIZE;
        queue.tamano++;
		queue.numVeces[ID_evento]++;

    } else { //overflow
		GPIO_hal_escribir(queue.pin_overflow, GPIO_OVERFLOW_BITS, 1) ;
	}

	/*habilitar interrupciones si es necesario*/
		if(I == 0){ /*se mantiene el bit irq como estaba al principio*/
			enable_irq();
		}
}

/*Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento
más antiguo sin procesar por referencia. Cero indicará que la cola está
vacía y no se ha devuelto ningún evento.*/

uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData) {
	
	int I = read_irq_bit();
	/*deshabilitar interrupciones*/
	disable_irq();
	
	if(queue.tamano > 0 ) {
		*ID_evento = queue.datos[queue.anterior].evento;
		*auxData = queue.datos[queue.anterior].datoAux;
		queue.tamano--;
		queue.anterior = (queue.anterior+1) % MAX_SIZE;
		/*habilitar interrupciones si es necesario*/
		if(I == 1){
			enable_irq();
		}
		return 1;
	}

	/*habilitar interrupciones si es necesario*/
	if(I == 1){
		enable_irq();
	}
		//cola vacía

	return 0;
}
 
/*Dado un identificador de evento nos devuelve el número total de veces que ese
evento se ha encolado. El evento VOID nos devolverá el total de
eventos encolados desde el inicio.*/
uint32_t FIFO_estadisticas(EVENTO_T ID_evento) {
	
    if(ID_evento == ID_VOID) {
		uint8_t cont = 0;

		for(int i=0; i<NUM_EVENTOS; i++) {
			cont += queue.numVeces[i] ;
		}
		return cont;
	} else {
        return queue.numVeces[ID_evento];
	}
}
