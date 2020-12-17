; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones
;
%include "print.mac"

BITS 32

sched_task_offset:     dd 0     
sched_task_selector:   dw 0

;; PIC
extern pic_finish1
extern imprimir_excepcion
extern print_scan_code


;; Sched
extern sched_next_task
extern cmpScanCode
extern sched_idle
extern desactivar_tarea
extern modo_debug

extern ticks_counter

;game 
extern clock_task

;; Syscalls
extern sys_meeseek
extern sys_move
extern sys_look
extern sys_use_portal_gun


temp: dd 0         ; variable temporal

tempw1: db 0         ; variable temporal
tempw2: db 0         ; variable temporal

tempb1: db 0         ; variable temporal
tempb2: db 0         ; variable temporal


;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
%macro ISR 1
global _isr%1

_isr%1:
     mov eax, %1
     xchg bx, bx

     push eax
     call imprimir_excepcion
     add esp, 4


     call desactivar_tarea  
     call sched_next_task            ; obtener indice de la proxima tarea a ejecutar

     mov [sched_task_selector], ax   ; carga el selector del segmento de la tarea a saltar
     jmp far [sched_task_offset]     ; intercambio de tareas

    
     ;jmp $

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19


;; Rutina de atención del RELOJ
global _isr32

_isr32:
     pushad
     ;avisar al pic que se recibio la interrupcion
     call pic_finish1

     ;contador de ticks de para move
     call ticks_counter 
     ;imprimir el reloj de sistema
     call sched_next_task ; Crear esta funcion en C que basicamente cicle entre las tareas que hay y cuando llega a la ultima vuelva a la primera
     str cx
     cmp ax, cx           ; Me fijo si la proxima tarea no es la actual
     je .fin
     call clock_task
     call next_clock
     mov word [sched_task_selector], ax
     ;xchg bx, bx 
     jmp far [sched_task_offset]
     .fin:
     ;xchg bx, bx 
     popad
iret

;; -------------------------------------------------------------------------- ;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
%define k_debug    0x15   ;  Y

global _isr33

_isr33:
     pushad 
     in al, 0x60
     cmp al, k_debug
     jne .fin
     call modo_debug
     .fin:
     ;avisar al pic que se recibio la interrupcion
     call pic_finish1
     popad
     iret

global _isr88 

; in EAX=code       Código de la tarea Mr Meeseeks a ser ejecutada.
; in EBX=x          Columna en el mapa donde crear el Mr Meeseeks.
; in ECX=y          Fila en el mapa donde crear el Mr Meeseeks

_isr88:    
     ; xchg bx,bx
     pushad
     mov ebp, esp
     

     push ecx       ; Código de la tarea Mr Meeseeks a ser ejecutada.
     push ebx       ; Columna en el mapa donde crear el Mr Meeseeks.
     push eax       ; Fila en el mapa donde crear el Mr Meeseeks

     call sys_meeseek

     mov [temp], eax ; mov eax a variable temporal
     add esp,12     ; tenemos 3 parametros de entrada

     
     call sched_idle
xchg bx, bx
     mov word [sched_task_selector], ax  ; (cambiamos con nahu)
     jmp far [sched_task_offset]

     popad          ; recupero registros
     mov eax, [temp] ; returneo en ceax   
     ; pop ebp
iret



global _isr89

_isr89:
     pushad
     mov ebp, esp

     call sys_use_portal_gun

     call sched_idle
     mov word [sched_task_selector], ax
     jmp far [sched_task_offset]
     
     popad
iret

; _isr33:
;      pushad 
;      in al, 0x60
;      cmp al, k_debug
;      jne .fin
;      call modo_debug
;      .fin:
;      ;avisar al pic que se recibio la interrupcion
;      call pic_finish1
;      popad
;      iret



global _isr100

; _isr100:
;      pushad
;      mov ebp, esp

;      call sys_look
;      ; xchg bx, bx 
;      ; mov byte  [tempw1], al
;      ; ;shr ax, 8
;      ; mov byte [tempw2],ah

;      mov byte bl, [eax]
;      mov byte  [tempb1], bl
;      add eax, 8;
;      mov byte bl, [eax] 
;      mov byte [tempb2],bl


;      call sched_idle
;      mov word [sched_task_selector], ax  ; (cambiamos con nahu)
;      jmp far [sched_task_offset]

;      popad
;      mov byte eax, [tempb1]
;      mov byte ebx, [tempb2]

;      xchg bx, bx 
;      ;COMPLETAR BIEN DEL TODO VER COMO DEVOLVER LOS 2 VALORES

; iret

_isr100:
     pushad
     mov ebp, esp
     
     push 0
     call sys_look
     mov byte  [tempw1], al
     add esp, 4

     push 1
     call sys_look
     mov byte  [tempw2], al
     add esp, 4


     call sched_idle
     mov word [sched_task_selector], ax  ; (cambiamos con nahu)
     jmp far [sched_task_offset]

     popad
     mov byte eax, [tempw1]
     mov byte ebx, [tempw2]

iret



global _isr123

; in EAX=x Desplazamiento en x
; in EBX=y Desplazamiento en y
; out EAX=worked 0: No se desplazo, 1: Se desplazo



_isr123:
     pushad
     mov ebp,esp 
     
     push ebx
     push eax
     call sys_move
     mov [temp], eax ; mov eax a variable temporal
     add esp, 8

     call sched_idle
     mov word [sched_task_selector], ax  ; (cambiamos con nahu)
     jmp far [sched_task_offset]


     popad          ; recupero registros
     mov eax, [temp] ; returneo en ceax   
iret


; _isr123:
;      pushad
;      mov ebp, esp
;      push eax
;      call next_clock
;      mov ax,0x80 ;idle
;      mov word [sched_task_selector], ax  ; (cambiamos con nahu)
;      jmp far [sched_task_offset]
;      ;pop ebp
;      popad
; iret


;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]  
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret




