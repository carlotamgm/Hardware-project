	AREA datos, DATA
NUM_FILAS EQU 7
NUM_COLUMNAS EQU 7
MAX_NO_CERO EQU 6

	AREA codigo, CODE
	EXPORT conecta_K_buscar_alineamiento_arm
	PRESERVE8 {true}
conecta_K_buscar_alineamiento_arm
		;r0 = t
		;r1 = fila
		;r2 = columna
		;r3 = color
		mov ip,sp
		stmdb r13!,{r4-r10,fp,lr}
		sub fp,ip,#4				; prólogo
		
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
		and r8,r8,#3				;transformar en formato de color
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
		ldmia r13!,{r4-r10,fp,lr}	;epílogo
		bx r14
final	mov r0,r6					;si sale por aqui, se mueve el resultado a r0 para cumplir con ATPCS

		ldmia r13!,{r4-r10,fp,lr}	;epílogo
		bx r14
		END