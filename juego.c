#include "juego.h"
#include "config_conecta_K.h"

static uint32_t (*FIFO_estadisticas)(EVENTO_T);

static uint8_t salida[8][8];

TABLERO cuadricula;

uint8_t fila;
uint8_t columna;
uint8_t jugador = 1;
uint8_t numJugada = 0;

enum estado_partida {
	INICIO,
	ESPERAR_COMANDO,
	ESPERAR_CANCELAR,
	GUARDAR_TABLERO,
	VOLVER_A_JUGAR,	
	FIN
} ;

static volatile enum estado_partida estadoConectaK = INICIO;

#define USUARIO_CANCELAR 3000 /*tiempo máximo en ms antes de confirmar jugada*/

/*MENSAJES PARA MOSTRAR POR LÍNEA SERIE*/

#define MENSAJE_INICIO "Bienvenido a Conecta-K. Se trata de un tablero 7x7\n\
y en el inicio, se pondra una sola jugada pero a continuacion, seran dos jugadas seguidas.\n\
Inserta uno de los siguientes comandos para empezar a jugar:\n\
	-Acabar la partida: $END!\n\
	-Nueva partida: $NEW!\n\
	-Jugada: $#-#! donde #-# sera el numero de fila-columna codificado\n\
	con dos caracteres (1..7, 1..7). Ejemplo: el comando $2-3! Indica la fila 2\n\
	y la columna 3.\n\n"

#define MENSAJE_VOLVER_A_JUGAR "Insertar el comando $NEW para volver a jugar. \n\
Si quieres dejar de jugar, pulsa el boton o escribe el comando $END para rendirte.\n\n"

#define MENSAJE_FIN "Se ha acabado la partida\n\n"

#define MENSAJE_ERROR_COLUMNA "Columna no valida\n\n"

#define MENSAJE_ERROR_COMANDO "Comando no valido\n\n"

#define MENSAJE_ESPERA_CANCELAR "Pulsa el boton 1 para cancelar el movimiento\n\n"

#define MENSAJE_MOVIMIENTO_CANCELADO "Movimiento cancelado\n\n"

/*FIN DE MENSAJES PARA MOSTRAR POR LÍNEA SERIE*/


/* ESTADÍSTICAS */

struct estadisticas{
	int num;
	double tiempoTotal;
};

static struct estadisticas tHayLinea;

static struct estadisticas tHumano;

static uint32_t tiempoPartida = 0;

	// Escribimos el histograma de eventos
char* nombresEventos[NUM_EVENTOS] = {
		"TOTAL",
		"ID_ALARMA",
		"ALARMA_OVERFLOW",
		"ID_PULSACION1",
		"ID_PULSACION2",
		"ID_AUSENTE",
		"ev_VISUALIZAR_HELLO",
		"ev_LATIDO",
		"PULSACION",
		"ev_comprobar",
		"ev_confirmar_jugada",
		"ev_TX_SERIE",
		"ev_RX_SERIE",
		"ev_cancelar_jugada"
	};

	char mensajeEventos[200] = "Se han encolado un total de ";
	int eventos;
	char eventosMensaje[13];
	
/* FIN DE ESTADÍSTICAS */	
	

	
/* Juego realiza las tareas necesarias para su inicialización */
void juego_inicializar(uint32_t (*funcion_estadisticas_cola)(EVENTO_T)) {
	FIFO_estadisticas = funcion_estadisticas_cola;
	tablero_inicializar(&cuadricula);
	conecta_K_test_cargar_tablero(&cuadricula);
	linea_serie_drv_enviar_array(MENSAJE_INICIO, (int)strlen(MENSAJE_INICIO));
	
}

/* Le da el turno al jugador contrario, teniendo en cuenta el jugador inicial de la última partida */
void alternar_jugador(uint8_t* jugador) {
	if(*jugador == 1) {
		*jugador = 2;
		//return;
	} else if(*jugador == 2) {
		*jugador = 1;
	}
}

//Máquina de estados 
void juego_tratar_evento(EVENTO_T ID_evento, char* comando) {
	int boton = comando[0] - '0';
	int tiempoLinea = 0, tiempoHumano = 0;
	
    switch(estadoConectaK){
		
		case INICIO: 
			
			tHayLinea.num = 0;
			tHayLinea.tiempoTotal = 0;
			tHumano.num = 0;
			tHumano.tiempoTotal = 0;			
			numJugada = 0; //primera jugada

			if(comando[0] == 'N' && comando[1] == 'E' && comando[2] == 'W') {		
				jugador = 1; //turno de jugador 1
				conecta_K_visualizar_tablero2(&cuadricula,salida); //muestra tablero test
				estadoConectaK = ESPERAR_COMANDO;
				tiempoPartida = clock_get_us();
				tiempoHumano = clock_get_us();
			}

		break;
       
		case ESPERAR_COMANDO:
			tHumano.num++;
			tHumano.tiempoTotal += clock_get_us() - tiempoHumano;
			tiempoHumano = clock_get_us();
			if( (comando[0] == 'E' && comando[1] == 'N' && comando[2] == 'D' ) || (ID_evento == PULSACION && boton == 2)) { //comando $END! o botón 2
				estadoConectaK = VOLVER_A_JUGAR; 
		
				if(ID_evento == PULSACION){ // se ha pulsado boton 2
					linea_serie_drv_enviar_array("FIN DE LA PARTIDA: Se ha pulsado el boton 2\n\n", 45);
				}
				else{ // comando $END!
					linea_serie_drv_enviar_array("FIN DE LA PARTIDA: Se ha escrito el comando $END!\n\n", 51);
				}
				tiempoPartida = clock_get_us() - tiempoPartida;
				
				mostrarEstadisticas();


			}
			else if(comando[1] == '-' ) { //comando $#-#! de jugada 
			
				if(comando[0] >= '1' && comando[0] <= '7' && comando[2] >= '1' && comando[2] <= '7' ) { //jugada válida
					fila = comando[0] - '0'; //convertir a entero
					columna = comando[2] - '0';
					
					if (celda_vacia(tablero_leer_celda(&cuadricula, fila-1, columna-1))) { //celda sin ocupar
						alarma_activar(ev_confirmar_jugada, USUARIO_CANCELAR, 0); /*inicia alarma de 3 segundos para pulsar botón 1*/
						estadoConectaK = ESPERAR_CANCELAR; 
						visualizar_jugada_provisional(&cuadricula, salida, fila-1, columna-1, jugador);
						linea_serie_drv_enviar_array(MENSAJE_ESPERA_CANCELAR, (int)strlen(MENSAJE_ESPERA_CANCELAR));
			

					} else { //celda ocupada, error
						linea_serie_drv_enviar_array(MENSAJE_ERROR_COLUMNA, (int)strlen(MENSAJE_ERROR_COLUMNA));
						error_inicializar(GPIO_SERIE_ERROR);
						estadoConectaK = ESPERAR_COMANDO;
					}

				} else if((comando[0] < '1' || comando[0] > '7') || (comando[1] != '-') || (comando[2] < '1' || comando[2] > '7')) { //fila o columna inexistentes
					error_inicializar(GPIO_SERIE_ERROR);
					linea_serie_drv_enviar_array(MENSAJE_ERROR_COLUMNA, (int)strlen(MENSAJE_ERROR_COLUMNA));
					estadoConectaK = ESPERAR_COMANDO;

				}

			} else { //comando no válido
				error_inicializar(GPIO_SERIE_ERROR);
				
				if ((comando[0]!= 'N' || comando[1] != 'E' || comando[2] != 'W' ) && (comando[0] != 'E' || comando[1] != 'N' || comando[2] != 'D' ) ) {

					linea_serie_drv_enviar_array(MENSAJE_ERROR_COMANDO, (int)strlen(MENSAJE_ERROR_COMANDO));

				} 
				estadoConectaK = ESPERAR_COMANDO;
			}
	
     	break;

		case ESPERAR_CANCELAR: //comando $#-#! de jugada válida
			if (ID_evento == PULSACION && boton == 1) { //se cancela la jugada
				alarma_activar(ev_confirmar_jugada, 0, 0); //cancelar alarma activa de confirmar jugada
				conecta_K_visualizar_tablero2(&cuadricula,salida); //muestra tablero anterior
				linea_serie_drv_enviar_array(MENSAJE_MOVIMIENTO_CANCELADO, (int)strlen(MENSAJE_MOVIMIENTO_CANCELADO));
				alarma_activar(ev_cancelar_jugada,1, 0); //encolar evento para llamar de nuevo a juego_tratar_evento
				estadoConectaK = ESPERAR_COMANDO;
	
			} else if (ID_evento == ev_confirmar_jugada) { //se confirma la jugada por timeout de 3 segundos sin pulsar botón 1
				alarma_activar(ev_confirmar_jugada,1, 0); //encolar evento para llamar de nuevo a juego_tratar_evento
				tablero_insertar_color(&cuadricula, fila-1, columna-1, jugador); //guardar tablero
				estadoConectaK = GUARDAR_TABLERO; 
				numJugada++;
				if(numJugada % 2 == 1) { /* en el primer turno, cada jugador ponen 2 fichas y luego en cada turno, se alterna el jugador */
					alternar_jugador(&jugador);
				}

			} 

		break;

		case GUARDAR_TABLERO: //timeout de 3 segundos sin pulsar botón 1
			conecta_K_visualizar_tablero2(&cuadricula,salida); //muestra tablero tras jugada
			tHayLinea.num++;
			tiempoLinea = clock_get_us();
			if(conecta_K_hay_linea_c_c(&cuadricula, fila-1, columna-1, jugador)) { //hay ganador 
				tHayLinea.tiempoTotal += clock_get_us() - tiempoLinea;
				if(jugador == 1) {
					linea_serie_drv_enviar_array("FIN DE LA PARTIDA: El ganador es el jugador 1\n\n", 49);
				}
				else{
					linea_serie_drv_enviar_array("FIN DE LA PARTIDA: El ganador es el jugador 2\n\n", 49);
				}
				tiempoPartida = clock_get_us() - tiempoPartida;
				
				mostrarEstadisticas();
				linea_serie_drv_enviar_array(MENSAJE_VOLVER_A_JUGAR, (int)strlen(MENSAJE_VOLVER_A_JUGAR));
				estadoConectaK = VOLVER_A_JUGAR;
			} else  { //no hay ganador
				tHayLinea.tiempoTotal += clock_get_us() - tiempoLinea;
				estadoConectaK = ESPERAR_COMANDO;			
				
			}

		break;

		case VOLVER_A_JUGAR: /*se ha terminado la partida*/

			// Inicializar los datos para almacenar las estadisticas
			tHayLinea.num = 0; 
			tHayLinea.tiempoTotal = 0;
			tHumano.num = 0;
			tHumano.tiempoTotal = 0;
			

			tiempoPartida = clock_get_us();
					
			if(comando[0] == 'N' && comando[1] == 'E' && comando[2] == 'W' ) { /*nueva partida*/
				tablero_inicializar(&cuadricula); 
				conecta_K_visualizar_tablero2(&cuadricula,salida); //muestra tablero vacío
				numJugada = 0;
				estadoConectaK = ESPERAR_COMANDO;

			} else if((comando[0] == 'E' && comando[1] == 'N' && comando[2] == 'D' ) || (ID_evento == PULSACION && boton == 2)) { /*terminar juego*/
					if(ID_evento == PULSACION){ // se ha pulsado boton 2
						linea_serie_drv_enviar_array("FIN DE LA PARTIDA: Se ha pulsado el boton 2\n\n", 45);
					} else{ // comando $END!
						linea_serie_drv_enviar_array("FIN DE LA PARTIDA: Se ha escrito el comando $END!\n\n", 51);
					}
				linea_serie_drv_enviar_array(MENSAJE_FIN, (int)strlen(MENSAJE_FIN));
				estadoConectaK = FIN;
			} 

		break;

		case FIN: // se ha escrito comando $END! o se ha pulsado botón 2
			while(1)
		break;
	

    }
}


void visualizar_jugada_provisional(TABLERO *t,  uint8_t memoria[8][8], uint8_t fila, uint8_t columna, uint8_t color) {
	char pantalla[NUM_FILAS+1][NUM_COLUMNAS*2+3];
	int i, j;
	
	memoria[0][0] = 00;

	for (i = 1; i <= NUM_FILAS; i++ ) {
       memoria[i][0] = 0xF0 + i ;
	}
  	for (j = 1; j <= NUM_COLUMNAS; j++ ) {
			memoria[0][j] = 0xC0 + j;
	} 

	for(i = 0; i < NUM_FILAS; i++) {
		for(j = 0; j< NUM_COLUMNAS; j++) {
			if (celda_color(tablero_leer_celda(t, i, j)) == 0) {
				memoria[i+1][j+1] = 0x00;                                                                                
			} else if (celda_color(tablero_leer_celda(t, i, j)) == 1) {
				memoria[i+1][j+1] = 0x11;  
			} else if (celda_color(tablero_leer_celda(t, i, j)) == 2) {
				memoria[i+1][j+1] = 0x22;  
			}
			
		} 
	}

	pantalla[0][0] = '-';
	pantalla[0][1] = '|';
	
	for (i = 1; i < NUM_FILAS+1; i++ ) {
    	pantalla[i][0] = i + '0';
		pantalla[i][1] = '|';
	}
	
    for (j = 2; j < NUM_COLUMNAS*2+3; j += 2 ) {
		pantalla[0][j] =  j/2 + '0';
		pantalla[0][j+1] =	'|' ;
    } 
	pantalla[0][NUM_COLUMNAS*2+2] = '\n';

	for(i = 1; i < NUM_FILAS+1; i++) {
		for(j = 2; j< NUM_COLUMNAS*2+2; j+=2) {
			if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 0) { //CELDA VACÍA
				if ((i - 1) == fila && (j / 2 - 1) == columna) { /* mostrar jugada provisional */
					if (color == 1) {
						pantalla[i][j] = 'b';
					} else if (color == 2) {
						pantalla[i][j] = 'n';
					}
            	} else {
                	pantalla[i][j] = ' ';
				}                                                                           
			} else if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 1) {
				pantalla[i][j] = 'B';  //ficha jugador 1
			} else if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 2) {
				pantalla[i][j] = 'N';  //ficha jugador 2
			}
			pantalla[i][j+1] = '|';
		}
    	pantalla[i][NUM_COLUMNAS * 2 + 2] = '\n';
	}
	
	char pantallaVector [(NUM_FILAS+1)*(NUM_COLUMNAS*2+3)+1];
	int k = 0;
	for (i = 0; i < NUM_FILAS+1; i++ ) {
    	for (j = 0; j < NUM_COLUMNAS*2+3; j++ ) {
				pantallaVector[k++] = pantalla[i][j];
		}
	}	
	pantallaVector[(NUM_FILAS+1)*(NUM_COLUMNAS*2+3)] = '\n';	
	linea_serie_drv_enviar_array(pantallaVector, (NUM_FILAS+1)*(NUM_COLUMNAS*2+3)+1);
}

/* 
Muestra por pantalla: 
o Tiempo total de uso de procesador en esta partida (sin power-down).
o Total y media de tiempo de computo de conecta_K_hay_linea.
o Total y media de tiempo que al humano le cuesta pensar jugada.
o Total de eventos encolados en esta partida e histograma por tipo de evento.
*/
void mostrarEstadisticas(void) {
	
	// Escribimos el tiempo de la partida
	char mensajeTiempoPartida[100] = "La partida ha durado ";
	char tPartida[13];
	itoa(tiempoPartida/1000000, tPartida);
	strcat(mensajeTiempoPartida, tPartida);
	strcat(mensajeTiempoPartida, " s \n\n");
	linea_serie_drv_enviar_array(mensajeTiempoPartida, (int)strlen(mensajeTiempoPartida));
	
	// Escribimos el tiempo y la media que se ha empleado en conecta_k_hay_linea
	char mensajeHayLinea[100] = "Tiempo de conecta_k_hay_linea: "; 
	char tiempoHayLinea[13], mediaHayLinea[13];
	itoa(tHayLinea.tiempoTotal, tiempoHayLinea);
	itoa((tHayLinea.tiempoTotal)/tHayLinea.num, mediaHayLinea);
	strcat(mensajeHayLinea, tiempoHayLinea);
	strcat(mensajeHayLinea, " us \nMedia de tiempo en conecta_k_hay_linea: ");
	strcat(mensajeHayLinea, mediaHayLinea);
	strcat(mensajeHayLinea, " us \n\n");
	linea_serie_drv_enviar_array(mensajeHayLinea, (int)strlen(mensajeHayLinea));
	
		// Escribimos el tiempo y la media de la respuesta humana
	char mensajeTiempoHumano[120] = "Tiempo de respuesta humana total: "; 
	char tiempoHumano[13], mediaHumano[13];
	itoa(tHumano.tiempoTotal/1000000, tiempoHumano);
	itoa((tHumano.tiempoTotal/1000000)/tHumano.num, mediaHumano);
	strcat(mensajeTiempoHumano, tiempoHumano);
	strcat(mensajeTiempoHumano, " s \nMedia de tiempo de respuesta de humano: ");
	strcat(mensajeTiempoHumano, mediaHumano);
	strcat(mensajeTiempoHumano, " s \n\n");
	linea_serie_drv_enviar_array(mensajeTiempoHumano, (int)strlen(mensajeTiempoHumano));
	

	linea_serie_drv_enviar_array(mensajeEventos, (int) strlen(mensajeEventos));
	for (EVENTO_T evento = ID_VOID; evento < NUM_EVENTOS; evento++) {

		eventos = FIFO_estadisticas(evento);
		int len_num_eventos = itoa(eventos, eventosMensaje);
		linea_serie_drv_enviar_array(eventosMensaje, len_num_eventos);
		if(evento == ID_VOID){ /*muestra el total de eventos*/
			linea_serie_drv_enviar_array( " eventos, a continuacion desglosado en tipos:\n", 46);
		}
		else{
			linea_serie_drv_enviar_array( " eventos de tipo ", 17);
			linea_serie_drv_enviar_array(nombresEventos[evento], (int) strlen(nombresEventos[evento]));
			linea_serie_drv_enviar_array(" .\n", 3);
		}
	}
					linea_serie_drv_enviar_array(MENSAJE_VOLVER_A_JUGAR, (int)strlen(MENSAJE_VOLVER_A_JUGAR));

}

/*Inicialización del gestor de errores en comandos. Se le pasa como parámetro el pin 
del GPIO utilizado para marcar errores y marca el GPIO029 para indicar error.*/
void error_inicializar(GPIO_HAL_PIN_T pin_error) {

	GPIO_hal_sentido(pin_error, 1, GPIO_HAL_PIN_DIR_OUTPUT);
	GPIO_hal_escribir(pin_error, 1, 1);
}


// visualiza en "pantalla" el contenido de las 7 primeras filas y columnas 
// de las m*n del tablero en juego (suponemos que m y n son >= 7 siempre)
// en memoria se deberia ver algo tal que así:
// 00 C1 C2 C3 C4 C5 C6 C7
// F1 00 11 00 00 00 22 00
// F2 11 11 00 00 00 22 22
// F3 00 00 11 00 22 00 00
// F4 00 00 00 00 00 22 00
// F5 00 00 22 00 00 00 00
// F6 22 22 00 11 00 11 11
// F7 00 22 00 00 00 11 00 
//y en línea serie así:

//-|1|2|3|4|5|6|7|
//1| |B| | | |N| |
//2|B|B| | | |N|N|
//3| | |B| |N| | |
//4| | | | | |N| |
//5| | |N| | | | |
//6|N|N| |B| |B|B|
//7| |N| | | |B| |
void conecta_K_visualizar_tablero2(TABLERO *t, uint8_t memoria[8][8])
{
	
	char pantalla[NUM_FILAS+1][NUM_COLUMNAS*2+3];
	int i, j;
	
	memoria[0][0] = 00;

	for (i = 1; i <= NUM_FILAS; i++ ) {
       memoria[i][0] = 0xF0 + i ;
	}
  	for (j = 1; j <= NUM_COLUMNAS; j++ ) {
			memoria[0][j] = 0xC0 + j;
	} 

	for(i = 0; i < NUM_FILAS; i++) {
		for(j = 0; j< NUM_COLUMNAS; j++) {
			if (celda_color(tablero_leer_celda(t, i, j)) == 0) {
				memoria[i+1][j+1] = 0x00;                                                                                
			} else if (celda_color(tablero_leer_celda(t, i, j)) == 1) {
				memoria[i+1][j+1] = 0x11;  
			} else if (celda_color(tablero_leer_celda(t, i, j)) == 2) {
				memoria[i+1][j+1] = 0x22;  
			}
			
		} 
	}

	pantalla[0][0] = '-';
	pantalla[0][1] = '|';
	
	for (i = 1; i < NUM_FILAS+1; i++ ) {
    	pantalla[i][0] = i + '0';
		pantalla[i][1] = '|';
	}
	
    for (j = 2; j < NUM_COLUMNAS*2+3; j += 2 ) {
		pantalla[0][j] =  j/2 + '0';
		pantalla[0][j+1] =	'|' ;
    } 
	pantalla[0][NUM_COLUMNAS*2+2] = '\n';
	

	for(i = 1; i < NUM_FILAS+1; i++) {
		for(j = 2; j< NUM_COLUMNAS*2+2; j+=2) {
			if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 0) {
				pantalla[i][j] = ' ';  //celda vacía                                                                              
			} else if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 1) {
				pantalla[i][j] = 'B';  //ficha jugador 1
			} else if (celda_color(tablero_leer_celda(t, i-1, (j/2-1))) == 2) {
				pantalla[i][j] = 'N';  //ficha jugador 2
			}
			pantalla[i][j+1] = '|';
		}
    	pantalla[i][NUM_COLUMNAS * 2 + 2] = '\n';
	}
	
	char pantallaVector [(NUM_FILAS+1)*(NUM_COLUMNAS*2+3)+1];
	int k = 0;
	for (i = 0; i < NUM_FILAS+1; i++ ) {
    	for (j = 0; j < NUM_COLUMNAS*2+3; j++ ) {
			pantallaVector[k++] = pantalla[i][j];
		}
	}	
	pantallaVector[(NUM_FILAS+1)*(NUM_COLUMNAS*2+3)] = '\n';	
	linea_serie_drv_enviar_array(pantallaVector, (NUM_FILAS+1)*(NUM_COLUMNAS*2+3)+1);
}
