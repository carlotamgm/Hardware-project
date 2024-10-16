#include "planificador.h"
#include "io_reserva.h"
#include "fifo.h"
#include "temporizador_drv.h"
#include "hello_world.h"
#include "power.h"
#include "botones.h"
#include "alarmas.h"
#include "juego.h"
#include "conversor.h"
#include "Watchdog_hal.h"

#define USUARIO_AUSENTE 60000 /*tiempo máximo en ms antes de pasar a power-down
												debería ser de 12s pero el contador de segundos va muy rápido*/


/*El planificador inicializará la cola indicando que el GPIO para marcar el
overflow es el GPIO31 definido convenientemente en la cabecera
io_reserva.h como GPIO_OVERFLOW = 31 y de longitud GPIO_OVERFLOW_BITS =1. 
El planificador básicamente estará a la espera pendiente de la cola de eventos.
Cuando aparezca un evento nuevo no tratado, lo procesará.
*/
void planificador(void) {
		
	/*inicializa módulos*/
    hello_world_inicializar(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS, FIFO_encolar);
    juego_inicializar(FIFO_estadisticas);
		temporizador_drv_empezar();
    FIFO_inicializar(GPIO_OVERFLOW);
    botones_init(FIFO_encolar);
    alarma_inicializar(FIFO_encolar);

	/*encola eventos*/
	
    temporizador_drv_reloj(1,&FIFO_encolar,ID_ALARMA,0); //Cada 1 ms recibe un evento de activación periódica

	/*programa alarmas*/

    alarma_activar(ID_ALARMA, 10, 0); //Alarma no periódica (única) en 10ms
    alarma_activar(ID_ALARMA, 5, 0); //Reprogramar alarma no periódica (única) en 5ms
    
    alarma_activar(ID_AUSENTE, USUARIO_AUSENTE, 1); //Alarma no periódica de botón pulsado EINT1
    alarma_activar(ID_AUSENTE, USUARIO_AUSENTE, 2); //Alarma no periódica de botón pulsado EINT2
		
	/* probar watchdog */
	 // WD_hal_inicializar(1);
	//	WD_hal_feed();


    while (1) { //Espera a que haya un evento en la cola
        EVENTO_T id_evento;
        uint32_t auxData;
        int hay_evento = FIFO_extraer(&id_evento, &auxData);
				char carga[4];
				

        if(hay_evento) {
            
            switch (id_evento) {

                case ID_AUSENTE:
                    power_deep_sleep(); /*Pasa a power-down*/
                    break;

                case ID_VOID:
                    /* tratamos evento */
                    break;

                case ALARMA_OVERFLOW:     
                    GPIO_hal_escribir(GPIO_OVERFLOW, GPIO_OVERFLOW_BITS, 1);
                    power_hal_wait(); /*para ejecución*/
                    break;
                
                case ID_PULSACION1:
                    maquina_estados_EINT1();
                    break;
                            
                case ID_PULSACION2:
                    maquina_estados_EINT2();
                    break;
                            
                case PULSACION: 									
										alarma_activar(ID_AUSENTE, USUARIO_AUSENTE, 0); //Alarma no periódica de botón pulsado EINT1
                    carga[0] = '0' + auxData;
                    juego_tratar_evento(id_evento, carga);
                    break;
                            
                case ID_ALARMA:
                    alarma_tratar_evento();
                    break;

                case ev_LATIDO:
                    hello_world_tratar_evento();
                    break;

                case ev_VISUALIZAR_HELLO:
                    hello_world_tick_tack();
                    break;

                case ev_confirmar_jugada: /* ha saltado la alarma de confirmar jugada */
										juego_tratar_evento(id_evento, carga);
                    break;

                case ev_cancelar_jugada: /* ha saltado la alarma de confirmar jugada */
                    juego_tratar_evento(id_evento, carga);
										break;
                
                case ev_comprobar: /*llega un comando por línea serie*/
                    alarma_activar(ID_AUSENTE, USUARIO_AUSENTE, 0);
                    auxDataToString(auxData, carga);
                    juego_tratar_evento(id_evento, carga);
                    break;
								

                case ev_RX_SERIE:
                    //juego_tratar_evento(id_evento, carga);
                    break;

                case ev_TX_SERIE:
                    //juego_finalizar_evento();
                    break;
								
								case ev_NUM_EVENTOS:
									break;

            }
    
        } else {
            power_hal_wait(); /*modo idle*/
        }
    }
}

