#include "conversor.h"
#include <string.h>

int atoi(char* a){
		int resultado = 0;
    for(int i=0; i<3; i++) {
            resultado |= ((unsigned char)a[i] << 8*i); 
    }
    return resultado;
}

int itoa(int num, char resultado[13]){
    int div = 1;
    while (num/div > 9) {
        div*=10;
    }

    for(int i=0; i<12; i++) {
        if(div == 0){
            resultado[i] = '\0';
            return i;
        }
        resultado[i] = ((num/div) % 10) + '0';  
        div/=10;  
    }
    resultado[12] = '\0';
    return 12;
}

void auxDataToString(int num, char resultado[4]){
    for(int i=0; i<4; i++) {
        if((num >> 8*i) == 0) 
            break;
        resultado[i] = (((num >> 8*i) & 0xFF));     
    }
}
