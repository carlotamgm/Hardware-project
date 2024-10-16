#include "int_externas_hal.h"


// variable para comprobar que se hacen las interrupciones que deber�an hacerse
static volatile int eint1_cuenta = 0;
// variable que se activa al detectar una nueva pulsaci�n
static volatile int eint1_nueva_pulsacion = 0;

// variable para comprobar que se hacen las interrupciones que deber�an hacerse
static volatile int eint2_cuenta = 0;
// variable que se activa al detectar una nueva pulsaci�n
static volatile int eint2_nueva_pulsacion = 0;

typedef void (*Funcion)();
Funcion func_botones;


void eint1_ISR (void) __irq {
	eint1_cuenta++;
  VICIntEnClr = VICIntEnClr | 0x00008000;    //deshabilitamos eint1
	//EXTINT =  EXTINT | 2;        // clear interrupt flag        
	VICVectAddr = 0;             // Acknowledge Interrupt
  eint1_nueva_pulsacion = 1;
	func_botones(1);
}

void eint1_clear_nueva_pulsacion(){
	eint1_nueva_pulsacion = 0;
};

unsigned int eint1_read_nueva_pulsacion(){
	return eint1_nueva_pulsacion;
};

unsigned int eint1_read_cuenta(){
	return eint1_nueva_pulsacion;
};

void eint1_init (void) {
    
// NOTA: seg�n el manual se puede configurar c�mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar�an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci�n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
		eint1_nueva_pulsacion = 0;
		eint1_cuenta = 0;
    EXTINT =  EXTINT | 2;        // Limpiar la bandera de interrupción de EINT1

    // Configuración del slot de IRQ número 3 del VIC para EINT1
    VICVectAddr3 = (unsigned long)eint1_ISR;  // Establecer el vector de interrupción en 0
    // 0x20 bit 5 habilita las interrupciones vectorizadas. 
    // 15 es el número de la interrupción asignada. El número 15 es para EINT1 (ver la tabla 40 del manual de usuario LPC2105)
    
    PINSEL0 &= 0xefffffff;  // Establece los bits 28 y 29 en 10 para configurar EINT1
    PINSEL0 |= 0x20000000;  // Habilitar la interrupción de EXTINT1

    VICVectCntl3 = 0x20 | 15;  // Configurar el controlador de interrupciones de EINT1

    VICIntEnable |= 0x00008000;  // Habilitar la interrupción de EINT1
}

void eint2_ISR (void) __irq {
    eint2_cuenta++;
    VICIntEnClr = VICIntEnClr | 0x00010000;    // Deshabilitar EINT2
    //EXTINT =  EXTINT | 4;        // Limpiar la bandera de interrupción de EINT2
    VICVectAddr = 0;             // Reconocer la interrupción
    eint2_nueva_pulsacion = 1;
		func_botones(2);
}

void eint2_clear_nueva_pulsacion(){
	eint2_nueva_pulsacion = 0;
};

unsigned int eint2_read_nueva_pulsacion(){
	return eint2_nueva_pulsacion;
};

unsigned int eint2_read_cuenta(){
	return eint2_nueva_pulsacion;
};

void eint2_init (void) {
// NOTA: seg�n el manual se puede configurar c�mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar�an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci�n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
		eint2_nueva_pulsacion = 0;
		eint2_cuenta = 0;    
    
    EXTINT =  EXTINT | 4;        // Limpiar la bandera de interrupción de EINT2

    // Configuración del slot de IRQ número 4 del VIC para EINT2
    VICVectAddr4 = (unsigned long)eint2_ISR;  // Establecer el vector de interrupción en 0
    // 0x20 bit 5 habilita las interrupciones vectorizadas. 
    // 16 es el número de la interrupción asignada. El número 16 es para EINT2 
    PINSEL0 &= 0xBFFFFFFF;  // Establecer los bits 30 y 31 en 10 para configurar EINT2(tabla manual)
    PINSEL0 |= 0x80000000;  // Habilitar la interrupción de EXTINT2

    VICVectCntl4 = 0x20 | 16;  // Configurar el controlador de interrupciones de EINT2

    VICIntEnable |= 0x00010000;  // Habilitar la interrupción de EINT2
}

//funcion para ver si el boton de eint1 sigue pulsado o no
uint32_t int_externas_hal_leer_EXTINT_eint1(){
	
	uint32_t val;
	
	EXTINT = EXTINT | 0x2;
	val = EXTINT & 0x2;
	if(val == 0x2){
		return 0;
	}
	else {
		return 1;
	}
}

//funcion para ver si el boton de eint2 sigue pulsado o no
uint32_t int_externas_hal_leer_EXTINT_eint2(){
	
	uint32_t val;
	
	EXTINT = EXTINT | 0x4;
	val = EXTINT & 0x4;
	if(val == 0x4){
		return 0;
	}
	else {
		return 1;
	}
}


void int_externas_hal_config(void (*funcion_callback)()){
	func_botones = funcion_callback;
}

//Habilita interrupciones de EINT1
void activar_EINT1(){
	 EXTINT =  EXTINT | 2;        // Limpiar la bandera de interrupción de EINT2
	 VICIntEnable |= 0x00008000;  // Habilitar la interrupción de EINT2
}

//Habilita interrupciones de EINT2
void activar_EINT2(){
	 EXTINT =  EXTINT | 4;        // Limpiar la bandera de interrupción de EINT2
	 VICIntEnable |= 0x00010000;  // Habilitar la interrupción de EINT2
}

