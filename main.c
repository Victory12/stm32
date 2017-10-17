PRESERVE8 			    ; 8-битное выравнивание стека

THUMB

GET config.s
GET stm32f10x.s

AREA RESET, CODE, READONLY

DCD STACK_TOP
DCD Reset_Handler

ENTRY

Reset_Handler PROC
EXPORT Reset_Handler    ; Делаем Reset_Handler видимым вне этого файла

main 				    ; Основная подпрограмма
diod					; Инициализация диода
MOV32 R0, PERIPH_BB_BASE + \
RCC_APB2ENR * 32 + \
4 * 4 					; вычисляем адрес для BitBanding 4-го бита регистра RCC_APB2ENR
						; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
MOV R1, #1 				; включаем тактирование порта D (в 4-й бит RCC_APB2ENR пишем '1`)
STR R1, [R0] 			; загружаем это значение

MOV32 R0, GPIOC_CRH 	; адрес порта
MOV R1, #0x03 			; 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011")
LDR R2, [R0] 			; считать порт
BFI R2, R1, #0, #4 		; скопировать биты маски в позицию PIN7 ,r1 to r2 from r1 from 28 - 4
STR R2, [R0] 			; загрузить результат в регистр настройки порта

button					; Инициализация кнопки
MOV32 R0, PERIPH_BB_BASE + \
RCC_APB2ENR * 32 + \
2 * 4 					; вычисляем адрес для BitBanding 4-го бита регистра RCC_APB2ENR
						; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
MOV R1, #1 				; включаем тактирование порта D (в 4-й бит RCC_APB2ENR пишем '1`)
STR R1, [R0] 			; загружаем это значение

MOV32 R0, GPIOA_CRL
MOV R1, #0x08
LDR R2, [R0]
BFI R2, R1, #0, #4 		; скопировать биты маски в позицию PIN7 ,r1 to r2 from r1 from 28 - 4
STR R2, [R0]			; загрузить результат в регистр настройки порта

MOV32 R0, GPIOA_BRR 	; address of button port
MOV R1, #0x01 			; pull-down - 10
STR R2, [R0] 			; считать порт
const
LDR R7, =DELAY_VAL 		; псевдоинструкция Thumb (загрузить константу в регистр)
LDR R8, =FIN_DELAY
loop 					; Бесконечный цикл
MOV32 R0, GPIOC_BSRR 	; адрес порта выходных сигналов

MOV R1, #(PIN8)			; устанавливаем вывод в '1'
STR R1, [R0] 			; загружаем в порт

MOV32 R3, GPIOA_IDR
LDR R4, [R3]
ANDS R4, #0x01
IT NE
BLNE delay_descrement

BL delay 				; задержка

MOV R1, #(PIN8 « 16) 	; сбрасываем в '0'
STR R1, [R0] 			; загружаем в порт

BL delay 				; задержка сохраняет точку возврата

B loop 					; возвращаемся к началу цикла

ENDP
delay_descrement PROC
SUB R7, #0x20000
ENDP
delay PROC 				; Подпрограмма задержки
;SUB R7, #0x20000
CMP R7, R8
IT LT
LDRLT R7, =DELAY_VAL 	; псевдоинструкция Thumb (загрузить константу в регистр)
PUSH {R7}
delay_loop
SUBS R7, #1				; SUB с установкой флагов результата
IT NE
BNE delay_loop 			; переход, если Z==0 (результат вычитания не равен нулю)
POP {R7}
BX LR 					; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
ENDP

END