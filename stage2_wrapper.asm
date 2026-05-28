.8086
        .model tiny

        .code
        ;; Stage 2 loads at offset 0 of target segment 1000h
        ORG 0000h

        ;; Reference to Stage 2 C/Rust shell main function
        extern main_: near ptr

        public _cstart_, __STK, _small_code_

_small_code_ label near

_cstart_:
        ;; Align segment registers to match CS (which is 1000h)
        PUSH CS
        POP DS
        PUSH CS
        POP SS
        MOV SP, 0FFFEh

        ;; Jump directly to main() of the Stage 2 shell
        JMP main_

__STK:
        RET

        end _cstart_
