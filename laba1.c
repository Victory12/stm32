```assembly
    PRESERVE8                            ; 8-битное выравнивание стека
    THUMB                                ; Режим Thumb (AUL) инструкций

    GET       config.s                   ; include-файлы
    GET       stm32f10x.s    

    AREA      RESET, CODE, READONLY

    ; Таблица векторов прерываний
    DCD       STACK_TOP                  ; Указатель на вершину стека
    DCD       Reset_Handler              ; Вектор сброса

    ENTRY                                ; Точка входа в программу

Reset_Handler    PROC                    ; Вектор сброса
    EXPORT    Reset_Handler              ; Делаем Reset_Handler видимым вне этого файла

main                                     ; Основная подпрограмма
    MOV32     R0, PERIPH_BB_BASE + \
              RCC_APB2ENR * 32 + \
              4 * 4                      ; адрес для BitBanding 4-го бита регистра RCC_APB2ENR
                                         ; BitAddress = BitBandBase + RegAddr*32 + BitNumber*4
    MOV       R1, #1                     ; включаем тактирование порта C
    STR       R1, [R0]                   ; загружаем это значение
    
    MOV32     R0, GPIOC_CRH              ; адрес порта
    MOV       R1, #0x3                   ; Output mode 50mHz, Push-Pull ("0011")
    LDR       R2, [R0]                   ; считать порт
    BFI       R2, R1, #0, #4             ; скопировать биты маски в позицию PIN8
    STR       R2, [R0]                   ; загрузить результат в регистр настройки порта

loop                                     ; Бесконечный цикл
    MOV32     R0, GPIOC_BSRR             ; адрес порта выходных сигналов

    MOV       R1, #(PIN8)                ; устанавливаем вывод в '1'
    STR       R1, [R0]                   ; загружаем в порт
    
    MOV       R1, #(PIN8 << 16)          ; сбрасываем в '0'
    STR       R1, [R0]                   ; загружаем в порт

    B         loop                       ; возвращаемся к началу цикла
    
    ENDP
    
    END
```
