        .8086
        .model tiny

        .code
        ;; this ORG line is necessary for .COM files so that the data
        ;; section will not be referred by wrong locations.
        ORG 7C00h

        ;; reference to the `main` function in C.
        extern main_: near ptr

        ;; export these symbols
        public _cstart_, __STK, _small_code_

        ;; Required for linker to recognize
_small_code_ label near

        ;; program starting point.
        ;; there shouldn't be any data or instructions before this point
_cstart_:
        ;; enable interrupt and setup stack pointer. normally
        ;; these two would be done by DOS when loading .COM file.
        ;; these instructions are here just to make sure.
        ;; because we're using the tiny model so we don't need
        ;; to setup SS.
        STI
        MOV SP, 0FFFEh

        ;; jump to the main function in C.
        JMP main_

__STK:
        ;; stack overflow checking.
        ;; this does nothing but you can add your own checking here.
        RET

        end _cstart_
