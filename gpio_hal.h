#include <stdint.h>
#include <LPC210X.H>                            // LPC21XX Peripheral Registers
#ifndef GPIO_HAL_H
#define GPIO_HAL_H

/*
el conjunto de direcciones E/S del GPIO gpio_hal_pin_dir_t:
GPIO_HAL_PIN_DIR_INPUT, GPIO_HAL_PIN_DIR_OUTPUT.
*/
typedef enum {
    GPIO_HAL_PIN_DIR_INPUT,
    GPIO_HAL_PIN_DIR_OUTPUT
} gpio_hal_pin_dir_t;

/*
 el tipo de datos de la representación del pin del GPIO: GPIO_HAL_PIN_T
*/
typedef uint32_t GPIO_HAL_PIN_T ;


/*
 GPIO_hal_iniciar(): Permite emplear el GPIO y debe ser invocada antes
de poder llamar al resto de funciones de la biblioteca.
*/
__inline void GPIO_hal_iniciar() {
	//todas las variables de GPIO se encuentran ya inicializadas en <LPC210X.H>     
}

/*
 GPIO_hal_sentido(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,
gpio_hal_pin_dir_t direccion): los bits indicados se utilizarán como
entrada o salida según la dirección.
*/
__inline void GPIO_hal_sentido(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,
gpio_hal_pin_dir_t direccion) {
	
	uint32_t mask = (((1 << num_bits) - 1) << gpio_inicial ); //poner a 1 num_bits a partir de gpio_inicial
		//10000000-1=1111111
	if(direccion == GPIO_HAL_PIN_DIR_INPUT) {
		IODIR = IODIR & ~mask; //poner a 0 los bits donde se quiere escribir
		} 
	else if(direccion == GPIO_HAL_PIN_DIR_OUTPUT) {
			IODIR = IODIR | mask; //poner a 1 los bits donde se quiere escribir
    }

}


/*
 uint32_t GPIO_hal_leer(GPIO_HAL_PIN_T gpio_inicial, uint8_t
num_bits): gpio_inicial indica el primer bit a leer, num_bits indica cuántos
bits queremos leer. La función devuelve un entero con el valor de los bits
indicados. Ejemplo:
o valor de los pines: 0x0F0FAFF0
o bit_inicial: 12 num_bits: 4
o valor que retorna la función: 10 (lee los 4 bits 12-15)
*/
__inline uint32_t GPIO_hal_leer(GPIO_HAL_PIN_T gpio_inicial, uint8_t
num_bits) {
   uint32_t mask = (((1 << num_bits) - 1) << gpio_inicial);
   uint32_t result= ((IOPIN & mask) >> gpio_inicial);
	return result;
} 

/*
 GPIO_hal_escribir(GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits,
uint32_t valor): similar al anterior, pero en lugar de leer escribe en los
bits indicados el valor (si valor no puede representarse en los bits
indicados se escribirá los num_bits menos significativos a partir del inicial)
*/
void GPIO_hal_escribir(GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits,
uint32_t valor);

__inline void GPIO_hal_escribir(GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits,
uint32_t valor) {
	
    uint32_t mask = (((1 << num_bits) - 1) << bit_inicial); //seleccionar los bits donde escribir
	
   /* (si valor no puede representarse en los bit indicados se escribirá los num_bits
	menos significativos a partir del inicial)*/

	uint32_t valor_desplazado = (valor << bit_inicial);
	
	
    //OR((IOPIN AND bits donde escribir a 0),(valor colocado en los bits AND bits donde escribir a 1)):
    IOPIN = (IOPIN & ~mask ) | (valor_desplazado & mask); //escribir valor en los bits correspondientes
    
}
#endif

