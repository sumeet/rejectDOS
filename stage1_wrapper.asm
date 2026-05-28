.8086
        .model tiny

        .code
        ;; Stage 1 bootloader always loads at physical address 07C00h
        ORG 7C00h

        ;; reference to our main function (in Rust or C)
        extern main_: near ptr

        public _cstart_, __STK, _small_code_

_small_code_ label near

_cstart_:
        ;; Setup segment registers and stack
        CLI
        XOR AX, AX
        MOV DS, AX
        MOV SS, AX
        MOV SP, 0FFFEh
        STI

        ;; Print our beautiful gradient ASCII logo first (Stage 1 logic)
        CALL main_

        ;; NOW: Read Stage 2 from Sector 2 into memory at address 1000h:0000h
        ;; BIOS Disk Service (INT 13h, AH=02h)
        MOV AX, 1000h
        MOV ES, AX          ;; ES = 1000h
        XOR BX, BX          ;; ES:BX = 1000h:0000h (read buffer)

        XOR DL, DL          ;; DL = 00h (Floppy Drive A)
        MOV DH, 00h         ;; DH = 00h (Head 0)
        MOV CH, 00h         ;; CH = 00h (Cylinder 0)
        MOV CL, 02h         ;; CL = 02h (Start reading from Sector 2)
        MOV AL, 08h         ;; AL = 08h (Read 8 sectors = 4KB of space for Stage 2!)
        MOV AH, 02h         ;; AH = 02h (BIOS Read Sectors Function)
        INT 13h             ;; Call BIOS disk interrupt

        JC disk_error       ;; If carry flag is set, there was an error reading floppy

        ;; Perform a far jump to Stage 2 load segment (hand over control to Stage 2 shell!)
        MOV AX, 1000h
        PUSH AX
        XOR AX, AX
        PUSH AX
        RETF

disk_error:
        ;; Loop forever on error
        HLT
        JMP disk_error

__STK:
        RET

        end _cstart_
