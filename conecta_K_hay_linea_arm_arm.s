    AREA datos, DATA 
N_DELTAS        EQU 4
deltas_fila     DCB 0, -1, -1, 1
deltas_columna  DCB -1, 0, -1, -1
FALSO           EQU 0
VERDADERO       EQU 1
K_SIZE          EQU 4
NUM_COLUMNAS    EQU 7
NUM_FILAS       EQU 7
EXITO           EQU 0
ERROR           EQU -1
MAX_NO_CERO     EQU 6
; izq, abajo, diagonal izq abajo, diagonal izq arriba

    AREA codigo, CODE
	EXPORT conecta_K_hay_linea_arm_arm
	PRESERVE8 {TRUE}

conecta_K_hay_linea_arm_arm

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

                bl conecta_K_buscar_alineamiento_arm

                mov r6, r0
                
                cmp r0, #K_SIZE
                movge r0, #VERDADERO	; si es igual o mayor que K_SIZE, carga #VERDADERO en r0 para cumplir con ATPCS y va a fin
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
               
                bl conecta_K_buscar_alineamiento_arm

                add r6, r6, r0

                cmp r6, #K_SIZE
                movge r0, #VERDADERO	; si >= K_SIZE -> verdadero
                movlo r0, #FALSO		; si < K_SIZE -> falso
                bge fin

fin_bucle       add r4, r4, #1   ;i++
                cmp r4, #N_DELTAS
				blo bucle
                
fin           ldmdb FP, {r4-r10,FP,SP,PC}
				
				

conecta_K_buscar_alineamiento_arm
		;r0 = t
		;r1 = fila
		;r2 = columna
		;r3 = color
		mov ip,sp
		stmdb r13!,{r4-r10,fp,lr}
		sub fp,ip,#4
		
		cmp r1,#NUM_FILAS			;fila inválida
		bgt error

		cmp r1,#0
		blt error

		cmp r2,#NUM_COLUMNAS		;columna inválida
		bgt error
		
		cmp r2,#0
		blt error

		mov r5,#0					;r5=col
		b col
iterar	add r5,r5,#1				;col++

col		cmp r5,#MAX_NO_CERO			;col < MAX_NO_CERO
		beq error
		add r10,r1,r1,LSL #1
		add r8,r0,r10,LSL #1
		ldrb r8,[r8,r5]				;r8=t->columnas[fila][col] 
		cmp r8,r2					;comparar con columna
		bne iterar
		
fin_buc	add r10,r1,r1,LSL #1
		add r8,r0,#0x0000002A
		add r8,r8,r10,LSL #1
		ldrb r8,[r8,r5]				;r8 = no_ceros[fila][col]
		and r8,r8,#3				;transformar en formato color
		cmp r8,r3					
		beq exito

error 	mov r6,#0					;devolver 0
		b final
exito 	ldrsb r4,[fp, #4]			;r4=delta_fila	
		add r1,r1,r4				;r1=nueva_fila	

		ldrsb r5,[fp, #8]			;r5=delta_columna
		add r2,r2,r5				;r2=nueva_columna

		stmdb r13!,{r4-r5}
		bl conecta_K_buscar_alineamiento_arm
		add sp,sp,#8				;liberar parámetros
		add r0,r0,#1				;resultado++
		ldmia r13!,{r4-r10,fp,lr}
		bx r14
final	mov r0,r6

		ldmia r13!,{r4-r10,fp,lr}
		bx r14
		END