#include "botones.h" 

// Definir la estructura para la máquina de estados FSM
typedef enum {
    PULSADO,
    NO_PULSADO
} estado_boton;

static estado_boton estado_boton_eint1;
static estado_boton estado_boton_eint2;
static void(*FIFO_encolar)(EVENTO_T, uint32_t);

/*Inicializa los botones*/
void botones_init(void(*_FIFO_encolar)(EVENTO_T, uint32_t)){
	FIFO_encolar = _FIFO_encolar;
	eint1_init();
	eint2_init();
	int_externas_hal_config(gestionar_pulsacion);
}

//funcion al volver de interrupcion
void gestionar_pulsacion(uint32_t boton){
	FIFO_encolar(PULSACION, boton);
	switch (boton)
	{
	case 1:
		alarma_activar(ID_PULSACION1, O_PERIODICA|100, boton);
		estado_boton_eint1 = PULSADO;
		break;
	case 2:
		alarma_activar(ID_PULSACION2, O_PERIODICA|100, boton);
		estado_boton_eint2 = PULSADO;
		break;
	}
}

//maquina estados 1
void maquina_estados_EINT1(){

	uint32_t valor = int_externas_hal_leer_EXTINT_eint1();
	
	switch (estado_boton_eint1)
	{
		case PULSADO:
			switch(valor){
				case 1:
					activar_EINT1();
					alarma_activar(ID_PULSACION1, 0, 1); //reprogramar alarma de pulsación
					estado_boton_eint1 = NO_PULSADO;
				break;
			}
		case NO_PULSADO:
			break;
	}
}


//maquina estados 2
void maquina_estados_EINT2(){

	uint32_t valor = int_externas_hal_leer_EXTINT_eint2();
	
	switch (estado_boton_eint2)
	{
		case PULSADO:
			switch(valor){
				case 1:
					activar_EINT2();
					alarma_activar(ID_PULSACION2, 0, 2); //reprogramar alarma de pulsación
					estado_boton_eint2 = NO_PULSADO;
				break;
			}
		case NO_PULSADO:
			break;
	}
}

