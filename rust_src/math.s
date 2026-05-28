	.file	"math.c595931b92a39f73-cgu.0"
	.section	.text._RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind,"ax",@progbits
	.hidden	_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind
	.globl	_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind
	.p2align	4
	.type	_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind,@function
_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind:
	.cfi_startproc
	.p2align	4
.LBB0_1:
	jmp	.LBB0_1
.Lfunc_end0:
	.size	_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind, .Lfunc_end0-_RNvCsfLfy6EI15iL_7___rustc17rust_begin_unwind
	.cfi_endproc

	.section	.text.eval,"ax",@progbits
	.globl	eval
	.p2align	4
	.type	eval,@function
eval:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	pushl	%ebx
	.cfi_def_cfa_offset 12
	pushl	%edi
	.cfi_def_cfa_offset 16
	pushl	%esi
	.cfi_def_cfa_offset 20
	.cfi_offset %esi, -20
	.cfi_offset %edi, -16
	.cfi_offset %ebx, -12
	.cfi_offset %ebp, -8
	movzwl	24(%esp), %eax
	testw	%ax, %ax
	je	.LBB1_1
	movl	20(%esp), %edx
	movzwl	%ax, %esi
	movb	$43, %al
	xorl	%edi, %edi
	xorl	%ecx, %ecx
	jmp	.LBB1_8
	.p2align	4
.LBB1_17:
	addl	%ecx, %ecx
	leal	(%ecx,%ecx,4), %ebp
	movzbl	%bh, %ecx
	addl	%ebp, %ecx
.LBB1_16:
	incl	%edx
	decl	%esi
	je	.LBB1_3
.LBB1_8:
	movzbl	(%edx), %ebx
	movb	%bl, %bh
	addb	$-48, %bh
	cmpb	$10, %bh
	jb	.LBB1_17
	movzbl	%bl, %ebp
	cmpl	$43, %ebp
	je	.LBB1_11
	cmpl	$45, %ebp
	jne	.LBB1_16
.LBB1_11:
	cmpb	$45, %al
	je	.LBB1_14
	movzbl	%al, %eax
	cmpl	$43, %eax
	jne	.LBB1_15
	addl	%ecx, %edi
	jmp	.LBB1_15
.LBB1_14:
	subl	%ecx, %edi
.LBB1_15:
	xorl	%ecx, %ecx
	movl	%ebx, %eax
	jmp	.LBB1_16
.LBB1_3:
	cmpb	$43, %al
	je	.LBB1_2
	movzbl	%al, %eax
	cmpl	$45, %eax
	jne	.LBB1_6
	subl	%ecx, %edi
	jmp	.LBB1_6
.LBB1_1:
	xorl	%edi, %edi
	xorl	%ecx, %ecx
.LBB1_2:
	addl	%ecx, %edi
.LBB1_6:
	movl	%edi, %eax
	popl	%esi
	.cfi_def_cfa_offset 16
	popl	%edi
	.cfi_def_cfa_offset 12
	popl	%ebx
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	eval, .Lfunc_end1-eval
	.cfi_endproc

	.ident	"rustc version 1.95.0 (59807616e 2026-04-14)"
	.section	".note.GNU-stack","",@progbits
