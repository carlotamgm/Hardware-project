#include "alarmas.h"
#include "botones.h"

static void(*FIFO_encolar)(EVENTO_T, uint32_t);
typedef struct Alarma {
    uint32_t activa, periodica;
    uint32_t retardo, auxData;
    EVENTO_T ID_evento;
    uint32_t tiempo_disparar;
} Alarma;

static Alarma alarmas[MAX_ALARMAS];
static uint32_t cont = 0;

/*Inicializa el array de alarmas*/
void alarma_inicializar(void(*_FIFO_encolar)(EVENTO_T, uint32_t)) {
    int i;
    FIFO_encolar = _FIFO_encolar;
    for(i=0; i<MAX_ALARMAS; i++) {
        alarmas[i].activa = 0;
        alarmas[i].periodica = 0;
        alarmas[i].retardo = 0;
        alarmas[i].auxData = 0;
        alarmas[i].ID_evento = ID_VOID;
				
    }
    temporizador_drv_iniciar();
}

/*Programa una alarma para el evento ID_evento y la información auxData, en un periodo retardo
que encolará el evento de forma única. Si se modifica el retardo a uno menor, se sobreescribe en la misma alarma. */
void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t auxData) {

		int id_encontrado,i,libre;
	
    /*Si el retardo es cero,
    se cancelará la alarma*/
    if(retardo == 0) {
        alarma_desactivar(ID_evento);
    }

	id_encontrado = 0;
    i = 0;
    libre = 0;
    
    while (libre == 0 && id_encontrado == 0 && i< MAX_ALARMAS) {
        if(alarmas[i].activa == 0) { /*alarma libre*/
            libre = 1;
        } else if(alarmas[i].ID_evento == ID_evento) { /*hay una alarma ya programada*/
            id_encontrado = 1;
        }
        else {
            i++;
        }
        
    } 
    if (libre == 1) { /*Un evento nuevo se asignará a la primera alarma libre*/
        alarmas[i].activa = 1;
        alarmas[i].periodica = (retardo & 0x80000000); /*retardo codifica en el bit de más peso si la alarma es periódica*/
        alarmas[i].retardo = retardo & 0x7FFFFFFF; /*los restantes bits indican el retardo en milisegundos*/
        alarmas[i].auxData = auxData;
        alarmas[i].ID_evento = ID_evento;
		alarmas[i].tiempo_disparar = cont + alarmas[i].retardo;

    } else if (id_encontrado == 1) {  /*Si se reprograma un determinado evento antes de que salte, se reprogramará al nuevo valor*/
        alarmas[i].activa = 1;
        alarmas[i].periodica = (retardo & 0x80000000); /*retardo codifica en el bit de más peso si la alarma es periódica*/
        alarmas[i].retardo = retardo & 0x7FFFFFFF; /*los restantes bits indican el retardo en milisegundos*/
        alarmas[i].auxData = auxData;
				alarmas[i].tiempo_disparar = cont + alarmas[i].retardo;
        
        
    } else { /*Si no hay alarmas disponibles se generará un evento de ALARMA_OVERFLOW que será
                gestionado por el planificador*/
        FIFO_encolar(ALARMA_OVERFLOW, 0);
    }
}

/*Cancela la alarma programada para ID_evento.*/
void alarma_desactivar(EVENTO_T ID_evento) {
		int i;
    for(i=0; i<MAX_ALARMAS; i++) {
        if(alarmas[i].ID_evento == ID_evento ) {
            alarmas[i].activa = 0;
        }
    }
}

/*Comprueba si hay que disparar el evento asociado a alguna de las alarmas programadas pendientes.*/
void alarma_tratar_evento() {

    int i = 0;
    cont++;

    while (i < MAX_ALARMAS) {
        if(alarmas[i].activa == 1) {
            if(alarmas[i].tiempo_disparar == cont) { /*vence el tiempo de la alarma*/
                FIFO_encolar(alarmas[i].ID_evento, alarmas[i].auxData); /*se dispara la alarma*/

                if(alarmas[i].periodica == 0) {
                    alarma_desactivar(alarmas[i].ID_evento); /*Las alarmas no periódicas se cancelan tras dispararse.*/
                } else { 
                    alarmas[i].tiempo_disparar += alarmas[i].retardo; /*se reprograma siguiente alarma porque es periódica*/
            
                }
            }
        } 
        i++;
    } 	

}

