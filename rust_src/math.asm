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
push eax
movzx edi, word [esp + 28]
test edi, edi
je .LBB1_22
mov ebp, [esp + 22]
mov dl, 43
xor eax, eax
xor esi, esi
add edi, ebp
jmp .LBB1_4
.LBB1_2:
mov ecx, [esp]
xor esi, esi
mov edx, ecx
.LBB1_3:
inc ebp
cmp ebp, edi
jae .LBB1_16
.LBB1_4:
movzx ecx, byte [ebp]
mov [esp], ecx
add ecx, -37
cmp ecx, 10
ja .LBB1_12
mov ebx, 1377
bt ebx, ecx
jae .LBB1_12
mov ebx, edx
xor edx, edx
mov ecx, 0
cmp bl, 43
cmove ecx, esi
add eax, ecx
cmp bl, 45
mov ecx, eax
cmove edx, esi
mov eax, 1
sub ecx, edx
cmp bl, 42
cmove eax, esi
imul eax, ecx
cmp bl, 37
je .LBB1_10
movzx edx, bl
cmp edx, 47
jne .LBB1_2
mov ebx, [esp]
test si, si
jne .LBB1_14
xor esi, esi
mov edx, ebx
xor eax, eax
jmp .LBB1_3
.LBB1_10:
test si, si
jne .LBB1_15
mov eax, [esp]
xor esi, esi
mov edx, eax
xor eax, eax
jmp .LBB1_3
.LBB1_12:
mov ebx, [esp]
add bl, -48
cmp bl, 9
ja .LBB1_3
add esi, esi
lea ecx, [esi + esi*4]
movzx esi, bl
add esi, ecx
jmp .LBB1_3
.LBB1_14:
mov eax, ecx
xor edx, edx
div si
xor esi, esi
mov edx, ebx
jmp .LBB1_3
.LBB1_15:
xor edx, edx
div si
mov eax, [esp]
xor esi, esi
mov ecx, eax
mov eax, edx
mov edx, ecx
jmp .LBB1_3
.LBB1_16:
cmp dl, 45
je .LBB1_24
movzx ecx, dl
cmp ecx, 43
je .LBB1_23
cmp dl, 42
mov ebx, edx
mov edx, 1
cmove edx, esi
imul edx, eax
cmp bl, 37
je .LBB1_26
cmp ecx, 47
jne .LBB1_25
test si, si
je .LBB1_29
xor edx, edx
div si
mov edx, eax
jmp .LBB1_25
.LBB1_22:
xor eax, eax
xor esi, esi
.LBB1_23:
add esi, eax
mov eax, esi
xor esi, esi
.LBB1_24:
sub eax, esi
mov edx, eax
.LBB1_25:
mov eax, edx
add esp, 4
pop esi
pop edi
pop ebx
pop ebp
ret
.LBB1_26:
test si, si
je .LBB1_29
mov eax, edx
xor edx, edx
div si
jmp .LBB1_25
.LBB1_29:
xor edx, edx
jmp .LBB1_25
.Lfunc_end1:
