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
	pushl	%eax
	.cfi_def_cfa_offset 24
	.cfi_offset %esi, -20
	.cfi_offset %edi, -16
	.cfi_offset %ebx, -12
	.cfi_offset %ebp, -8
	movzwl	28(%esp), %edi
	testl	%edi, %edi
	je	.LBB1_22
	movl	24(%esp), %ebp
	movb	$43, %dl
	xorl	%eax, %eax
	xorl	%esi, %esi
	addl	%ebp, %edi
	jmp	.LBB1_4
	.p2align	4
.LBB1_2:
	movl	(%esp), %ecx
	xorl	%esi, %esi
	movl	%ecx, %edx
.LBB1_3:
	incl	%ebp
	cmpl	%edi, %ebp
	jae	.LBB1_16
.LBB1_4:
	movzbl	(%ebp), %ecx
	movl	%ecx, (%esp)
	addl	$-37, %ecx
	cmpl	$10, %ecx
	ja	.LBB1_12
	movl	$1377, %ebx
	btl	%ecx, %ebx
	jae	.LBB1_12
	movl	%edx, %ebx
	xorl	%edx, %edx
	movl	$0, %ecx
	cmpb	$43, %bl
	cmovel	%esi, %ecx
	addl	%ecx, %eax
	cmpb	$45, %bl
	movl	%eax, %ecx
	cmovel	%esi, %edx
	movl	$1, %eax
	subl	%edx, %ecx
	cmpb	$42, %bl
	cmovel	%esi, %eax
	imull	%ecx, %eax
	cmpb	$37, %bl
	je	.LBB1_10
	movzbl	%bl, %edx
	cmpl	$47, %edx
	jne	.LBB1_2
	movl	(%esp), %ebx
	testw	%si, %si
	jne	.LBB1_14
	xorl	%esi, %esi
	movl	%ebx, %edx
	xorl	%eax, %eax
	jmp	.LBB1_3
	.p2align	4
.LBB1_10:
	testw	%si, %si
	jne	.LBB1_15
	movl	(%esp), %eax
	xorl	%esi, %esi
	movl	%eax, %edx
	xorl	%eax, %eax
	jmp	.LBB1_3
.LBB1_12:
	movl	(%esp), %ebx
	addb	$-48, %bl
	cmpb	$9, %bl
	ja	.LBB1_3
	addl	%esi, %esi
	leal	(%esi,%esi,4), %ecx
	movzbl	%bl, %esi
	addl	%ecx, %esi
	jmp	.LBB1_3
.LBB1_14:
	movl	%ecx, %eax
	xorl	%edx, %edx
	divw	%si
	xorl	%esi, %esi
	movl	%ebx, %edx
	jmp	.LBB1_3
.LBB1_15:
	xorl	%edx, %edx
	divw	%si
	movl	(%esp), %eax
	xorl	%esi, %esi
	movl	%eax, %ecx
	movl	%edx, %eax
	movl	%ecx, %edx
	jmp	.LBB1_3
.LBB1_16:
	cmpb	$45, %dl
	je	.LBB1_24
	movzbl	%dl, %ecx
	cmpl	$43, %ecx
	je	.LBB1_23
	cmpb	$42, %dl
	movl	%edx, %ebx
	movl	$1, %edx
	cmovel	%esi, %edx
	imull	%eax, %edx
	cmpb	$37, %bl
	je	.LBB1_26
	cmpl	$47, %ecx
	jne	.LBB1_25
	testw	%si, %si
	je	.LBB1_29
	xorl	%edx, %edx
	divw	%si
	movl	%eax, %edx
	jmp	.LBB1_25
.LBB1_22:
	xorl	%eax, %eax
	xorl	%esi, %esi
.LBB1_23:
	addl	%eax, %esi
	movl	%esi, %eax
	xorl	%esi, %esi
.LBB1_24:
	subl	%esi, %eax
	movl	%eax, %edx
.LBB1_25:
	movl	%edx, %eax
	addl	$4, %esp
	.cfi_def_cfa_offset 20
	popl	%esi
	.cfi_def_cfa_offset 16
	popl	%edi
	.cfi_def_cfa_offset 12
	popl	%ebx
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_def_cfa_offset 4
	retl
.LBB1_26:
	.cfi_def_cfa_offset 24
	testw	%si, %si
	je	.LBB1_29
	movl	%edx, %eax
	xorl	%edx, %edx
	divw	%si
	jmp	.LBB1_25
.LBB1_29:
	xorl	%edx, %edx
	jmp	.LBB1_25
.Lfunc_end1:
	.size	eval, .Lfunc_end1-eval
	.cfi_endproc

	.ident	"rustc version 1.95.0 (59807616e 2026-04-14)"
	.section	".note.GNU-stack","",@progbits
