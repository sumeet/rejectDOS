;; Translated from 32-bit i686 AT&T asm to 16-bit 386 Intel asm
BITS 16
SECTION _TEXT class=CODE

global _RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind
_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind:
.LBB0_1:
jmp .LBB0_1
.Lfunc_end0:
global _eval
_eval:
push ebp
push ebx
push edi
push esi
movzx eax, word [esp + 22]
test ax, ax
je .LBB1_1
mov edx, [esp + 18]
movzx esi, ax
mov al, 43
xor edi, edi
xor ecx, ecx
jmp .LBB1_8
.LBB1_17:
add ecx, ecx
lea ebp, [ecx + ecx*4]
movzx ecx, bh
add ecx, ebp
.LBB1_16:
inc edx
dec esi
je .LBB1_3
.LBB1_8:
movzx ebx, byte [edx]
mov bh, bl
add bh, -48
cmp bh, 10
jb .LBB1_17
movzx ebp, bl
cmp ebp, 43
je .LBB1_11
cmp ebp, 45
jne .LBB1_16
.LBB1_11:
cmp al, 45
je .LBB1_14
movzx eax, al
cmp eax, 43
jne .LBB1_15
add edi, ecx
jmp .LBB1_15
.LBB1_14:
sub edi, ecx
.LBB1_15:
xor ecx, ecx
mov eax, ebx
jmp .LBB1_16
.LBB1_3:
cmp al, 43
je .LBB1_2
movzx eax, al
cmp eax, 45
jne .LBB1_6
sub edi, ecx
jmp .LBB1_6
.LBB1_1:
xor edi, edi
xor ecx, ecx
.LBB1_2:
add edi, ecx
.LBB1_6:
mov eax, edi
pop esi
pop edi
pop ebx
pop ebp
ret
.Lfunc_end1:
