    AREA datos, DATA 
N_DELTAS        EQU 4
deltas_fila     DCB 0, -1, -1, 1
deltas_columna  DCB -1, 0, -1, -1
FALSO           EQU 0
VERDADERO       EQU 1
K_SIZE          EQU 4


    AREA codigo, CODE
    EXPORT conecta_K_hay_linea_arm_c
    IMPORT conecta_K_buscar_alineamiento_c
	PRESERVE8 {TRUE}

conecta_K_hay_linea_arm_c

                mov ip, sp
                stmdb sp!, {r4-r10, fp, ip, lr, pc}
                sub fp, ip, #4

                mov r4, #0      ; r4 = i = 0
                mov r6, #0      ; r6 = long_linea = 0

                mov r7, r0      ; r7 = &tablero
                mov r8, r1      ; r8 = fila
                mov r9, r2      ; r9 = columna
                mov r10, r3     ; r10 = color

bucle           ldr r1, =deltas_fila
                ldrb r1, [r1, r4]  ; r1=deltas_filas[i]

                ldr r2, =deltas_columna
                ldrb r2, [r2, r4]  ; r2=deltas_columnas[i]              

                PUSH{r1, r2}              ; apilar 5º y 6º parametro
                
                mov r0, r7      ; r0 = tablero
                mov r1, r8      ; r1 = fila
                mov r2, r9      ; r2 = columna
                mov r3, r10     ; r3 = color

                bl conecta_K_buscar_alineamiento_c

                mov r6, r0
                
                cmp r0, #K_SIZE
                movge r0, #VERDADERO
                bge fin

            
                ldr r0, =deltas_fila
                ldrb r0, [r0, r4]  ; r0=deltas_filas[i]

                ldr r1, =deltas_columna
                ldrb r1, [r1, r4]  ; r1=deltas_columnas[i] 
                mov r3, r10     ; recuperar valores para llamar a funcion

                mov r2, #0      ; r2 = 0

                sub r0, r2, r0  ; r0 = -deltas_filas[i]
				and r0, r0, #0xFF
                sub r1, r2, r1  ; r1 = -deltas_columnas[i]
                and r1, r1, #0xFF
				
                PUSH{r0,r1}

                add r2, r9, r1  ; r2 = columna-deltas_columna[i]
				and r2, r2, #0xFF
                add r1, r8, r0  ; r1 = fila-deltas_fila[i] 
				and r1, r1, #0xFF
                
                mov r0, r7      ; r0 = tablero
               
                bl conecta_K_buscar_alineamiento_c

                add r6, r6, r0

                cmp r6, #K_SIZE
                movge r0, #VERDADERO
                movlo r0, #FALSO
                bge fin

fin_bucle       add r4, r4, #1   ;i++
                cmp r4, #N_DELTAS
				blo bucle
                
fin           ldmdb FP, {r4-r10,FP,SP,PC}
				
				END
