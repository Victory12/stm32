```assembly
	PRESERVE8 				; 8-битное выравнивание стека

	THUMB

	GET config.s				
	GET stm32f10x.s

	AREA RESET, CODE, READONLY

	DCD STACK_TOP				; constants
	DCD Reset_Handler

	ENTRY						; enter into the program

Reset_Handler PROC
	EXPORT Reset_Handler    	; Делаем Reset_Handler видимым вне этого файла
	
main 				    		; Основная подпрограмма
diod							; Инициализация диода
	MOV32 R0, PERIPH_BB_BASE + \
	RCC_APB2ENR * 32 + \
	4 * 4 						; вычисляем адрес для BitBanding 4-го бита регистра RCC_APB2ENR
								; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV R1, #1 					; включаем тактирование : 
	STR R1, [R0] 				; в 4-й бит RCC_APB2ENR пишем '1'

	MOV32 R0, GPIOC_CRH 		; помещаем адрес порта в R0
	MOV R1, #0x03 				; 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011")
	LDR R2, [R0] 				; считать порт
	BFI R2, R1, #0, #4 			; скопировать биты маски в позицию PIN8
	STR R2, [R0] 				; загрузить результат в регистр настройки порта

button							; Инициализация кнопки
	MOV32 R0, PERIPH_BB_BASE + \
	RCC_APB2ENR * 32 + \
	2 * 4 						; вычисляем адрес для BitBanding 4-го бита регистра RCC_APB2ENR
								; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV R1, #1 					; включаем тактирование порта 
	STR R1, [R0] 				; в 4-й бит RCC_APB2ENR пишем '1'

	MOV32 R0, GPIOA_CRL			; помещаем адрес порта в R0 (CRL - configuration register)
	MOV R1, #0x08				; 4-битная маска настроек для Input mode pull-down ("1000")
	LDR R2, [R0]				; считать порт
	BFI R2, R1, #0, #4 			; скопировать биты маски 
	STR R2, [R0]				; загрузить результат в регистр настройки порта

	MOV32 R0, GPIOA_BRR 		; помещаем адрес порта в R0 (BRR -  bit reset register)
	MOV R1, #0x01 				
	STR R2, [R0] 				; считать порт
const
	LDR R7, =DELAY_VAL 			; псевдоинструкция Thumb (загрузить константу в регистр)
	LDR R8, =FIN_DELAY
loop 							; Бесконечный цикл
	MOV32 R0, GPIOC_BSRR 		; адрес порта выходных сигналов
	MOV R1, #(PIN8)				
	STR R1, [R0] 				; загружаем в порт

	MOV32 R3, GPIOA_IDR			; помещаем адрес порта в R3 (IDR - input data register)
	LDR R4, [R3]				; считываем порт в регистр R4	
	ANDS R4, #0x01				 
	IT NE					
	BLNE delay_descrement		; уменьшить величину задержки

	BL delay 					; задержка

	MOV R1, #(PIN8 « 16) 		; сбрасываем в '0'
	STR R1, [R0] 				; загружаем в порт

	BL delay 					; задержка сохраняет точку возврата

	B loop 						; возвращаемся к началу цикла

	ENDP
delay_descrement PROC			; Подпрограмма изменения величины задержки
	SUB R7, #0x20000
	ENDP
delay PROC 						; Подпрограмма задержки
	CMP R7, R8					; Сравнить R7 R8
	IT LT
	LDRLT R7, =DELAY_VAL 		; псевдоинструкция Thumb (загрузить константу в регистр)
	PUSH {R7}
delay_loop
	SUBS R7, #1				; SUB с установкой флагов результата
	IT NE
	BNE delay_loop 				; переход, если Z==0 (результат вычитания не равен нулю)
	POP {R7}
	BX LR 					; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	ENDP
END
```
