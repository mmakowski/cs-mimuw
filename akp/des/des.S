/* (c) Maciek Makowski 2001 */

.data
	/*ztablicowane permutacje i sboksy*/
	#include "pc1.inc"
	#include "pc2.inc"
	#include "ip.inc"
	#include "e.inc"
	#include "sbox.inc"
	#include "p.inc"
	#include "fp.inc"
	
	key:	.ascii	"1234567" 
	k:	.fill	96, 1, 3
	idb:	.ascii	"00000000"	
//idb:	 .byte 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	odb:	.fill	8, 1, 0		
	expand:	.fill	6, 1, 0
	per:	.fill	4, 1, 0
	nl:	.ascii	"\n."	
	dp:	.ascii  "1:2:3:4:5:6:7:8\n"
.text
.globl main

/* main ***************************************************/	
main:
	push %ebp
	push %ebx
	mov %esp, %ebp

	/* robienie kluczy */
	call makekeys
	
	/* kodowanie */
	call encode
	call decode	

	mov $odb, %eax
	mov $idb, %ebx
	movq (%eax), %mm0
	movq %mm0, (%ebx)
	call encode
	call decode
		
	mov %ebp, %esp
	pop %ebx
	pop %ebp
	ret
	
/* makekeys ***********************************************/
makekeys:
	push %ebp
	push %ebx
	mov %esp, %ebp
	mov %ebp, %esp

	mov %ebp, %esp
	pop %ebx
	pop %ebp
	ret

/* encode *************************************************/		
encode:
	push %ebp
	push %ebx
	mov %esp, %ebp

	/* tworzenie zmiennych lokalnych */
	mov $0, %ebx
	push %ebx	//-4
	push %ebx	//-8
	push %ebx	//-12
	push %ebx	//-16
		
	mov $idb, %eax
	call diagprint
	
	/* zerowanie odb */
	mov $odb, %edx
	movl $0, (%edx)	
	movl $0, 4(%edx)

	/*kopia*/
	mov $idb, %ebx
	movq (%ebx), %mm0
	movq %mm0, (%edx)

	/* IP /
	mov $0, %ecx
	movl $ip, %eax
	enc_ip_next_idb_byte:	
		mov $idb, %edx
		movb $0, %ch
		mov $0, %ebx
		movb (%edx, %ecx), %bl
		movq odb, %mm0
		por (%eax, %ebx, 8), %mm0
		movq %mm0, odb
		add $2048, %eax
		incb %cl
		cmpb $8, %cl
		jne enc_ip_next_idb_byte 	*/


	//////


	/* kodowanie */
	movl $odb, -12(%ebp)	//L[0]
	movl $odb, -8(%ebp)
	addl $4, -8(%ebp)	//R[0]
	mov $0, %ecx
	
	enc_apply_keys:
		// r <-> l
		push %ecx
		pusha
		mov -8(%ebp), %eax
		mov -12(%ebp), %ebx
		movl (%eax), %ecx
		movl (%ebx), %edx
		movl %ecx, (%ebx)
		movl %edx, (%eax)
		popa
	
		/* zerowanie expand */
		mov $expand, %edx
		movl $0, (%edx)
		movw $0, 4(%edx)
	
		/* E */	
		movl $e, -4(%ebp)
		movb $0, %cl
		enc_e_next_r_byte:	
			mov -8(%ebp), %edx
			movb $0, %ch
			mov $0, %ebx
			movb (%edx, %ecx), %bl	//w bx jest wart. aktualnego bajtu R[]
			enc_e_next_e_byte:
				mov $expand, %edx
				push %ecx
				push %eax
				mov %ch, %al
				mov $0, %ecx
				mov %al, %cl
				pop %eax
				add %ecx, %edx	//w dx jest adres akt. bajtu expand
				movb (%edx), %al	//w al jest wart akt. bajtu expand
				push %edx
				mov -4(%ebp), %edx
				add %ecx, %edx
				push %ebx
				shl %ebx
				movl %ebx, -16(%ebp)
				shl %ebx
				addl -16(%ebp), %ebx
				orb (%edx, %ebx), %al
				pop %ebx
				pop %edx
				movb %al, (%edx)
				pop %ecx 
				incb %ch
				cmpb $6, %ch
				jne enc_e_next_e_byte
			/* przesuwanie wskaznika poczatku tablicy E */
			mov -4(%ebp), %eax
			add $1536, %eax
			movl %eax, -4(%ebp)
			incb %cl
			cmpb $4, %cl
			jne enc_e_next_r_byte 	

		/* xor z K[cx] */
		mov $k, %ebx
		pop %ecx
		push %ecx
		shl %ecx
		movl %ecx, -16(%ebp)
		shl %ecx
		addl -16(%ebp), %ecx
		add %ecx, %ebx
		mov $expand, %eax
		mov $0, %ecx
		mov $0, %edx
		movl (%eax), %ecx
		movl (%ebx), %ecx
		movl %ecx, (%eax)
		movw 4(%eax), %cx
		movw 4(%ebx), %cx
		movw %cx, 4(%eax)
	
/*		enc_next_key_byte:
			movb (%eax, %edx), %cl 
			movb (%ebx, %edx), %cl
			movb %cl, (%eax, %edx)
			inc %edx
			cmp $6, %edx
			jne enc_next_key_byte*/
//?OK		
		/* SBox */
		mov $expand, %eax
		//S[1]
		mov $0, %ebx
		mov $0, %ecx
		mov $s1, %edx
		movb (%eax), %bl
		shrb %bl
		shrb %bl	//w bl jest B[1]
		movb (%edx, %ebx), %cl	//w cl jest S[1]
		push %eax
		movl -8(%ebp), %eax
		mov $0, %ebx
		movb (%eax), %bl 
		andb $0x0F, %bl
		shlb %cl
		shlb %cl
		shlb %cl
		shlb %cl
		orb %cl, %bl
		movb %bl, (%eax)
		pop %eax		
		//S[2]
		mov $0, %ebx
		mov $0, %ecx
		mov $s2, %edx
		movb (%eax), %bl
		shlb %bl
		shlb %bl
		shlb %bl
		shlb %bl
		andb $0x30, %bl
		movb 1(%eax), %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		andb $0x0F, %cl
		orb %cl, %bl	//w bl jest B[2]
		movb (%edx, %ebx), %cl	//w cl jest S[2]
		push %eax
		movl -8(%ebp), %eax
		mov $0, %ebx
		movb (%eax), %bl 
		andb $0xF0, %bl
		orb %cl, %bl
		movb %bl, (%eax)
		pop %eax		
		//S[3]
		mov $0, %ebx
		mov $0, %ecx
		mov $s3, %edx
		movb 1(%eax), %bl
		shlb %bl
		shlb %bl
		andb $0x3c, %bl
		movb 2(%eax), %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		andb $0x03, %cl
		orb %cl, %bl	//w bl jest B[3]
		movb (%edx, %ebx), %cl	//w cl jest S[3]
		push %eax
		movl -8(%ebp), %eax
		mov $0, %ebx
		movb 1(%eax), %bl 
		andb $0x0F, %bl
		shlb %cl
		shlb %cl
		shlb %cl
		shlb %cl
		orb %cl, %bl
		movb %bl, 1(%eax)
		pop %eax		
		//S[4]
		mov $0, %ebx
		mov $0, %ecx
		mov $s4, %edx
		movb 2(%eax), %bl
		andb $0x3F, %bl	//w bl jest B[4]
		movb (%edx, %ebx), %cl	//w cl jest S[4]
		push %eax
		movl -8(%ebp), %eax
		mov $0, %ebx
		movb 1(%eax), %bl 
		andb $0xF0, %bl
		orb %cl, %bl
		movb %bl, 1(%eax)
		pop %eax		

		//to samo co wyzej, tylko przesuniete o 3 bajty:	
		add $3, %eax
		//S[5]
		mov $0, %ebx
		mov $0, %ecx
		mov $s5, %edx
		movb (%eax), %bl
		shrb %bl
		shrb %bl	//w bl jest B[5]
		movb (%edx, %ebx), %cl	//w cl jest S[5]
		push %eax
		movl -8(%ebp), %eax
		addl $2, %eax
		mov $0, %ebx
		movb (%eax), %bl 
		andb $0x0F, %bl
		shlb %cl
		shlb %cl
		shlb %cl
		shlb %cl
		orb %cl, %bl
		movb %bl, (%eax)
		pop %eax		
		//S[6]
		mov $0, %ebx
		mov $0, %ecx
		mov $s6, %edx
		movb (%eax), %bl
		shlb %bl
		shlb %bl
		shlb %bl
		shlb %bl
		andb $0x30, %bl
		movb 1(%eax), %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		andb $0x0F, %cl
		orb %cl, %bl	//w bl jest B[6]
		movb (%edx, %ebx), %cl	//w cl jest S[6]
		push %eax
		movl -8(%ebp), %eax
		addl $2, %eax
		mov $0, %ebx
		movb (%eax), %bl 
		andb $0xF0, %bl
		orb %cl, %bl
		movb %bl, (%eax)
		pop %eax		
		//S[7]
		mov $0, %ebx
		mov $0, %ecx
		mov $s7, %edx
		movb 1(%eax), %bl
		shlb %bl
		shlb %bl
		andb $0x3c, %bl
		movb 2(%eax), %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		shrb %cl
		andb $0x03, %cl
		orb %cl, %bl	//w bl jest B[7]
		movb (%edx, %ebx), %cl	//w cl jest S[7]
		push %eax
		movl -8(%ebp), %eax
		addl $2, %eax
		mov $0, %ebx
		movb 1(%eax), %bl 
		andb $0x0F, %bl
		shlb %cl
		shlb %cl
		shlb %cl
		shlb %cl
		orb %cl, %bl
		movb %bl, 1(%eax)
		pop %eax		
		//S[8]
		mov $0, %ebx
		mov $0, %ecx
		mov $s8, %edx
		movb 2(%eax), %bl
		andb $0x3F, %bl	//w bl jest B[8]
		movb (%edx, %ebx), %cl	//w cl jest S[8]
		push %eax
		movl -8(%ebp), %eax
		addl $2, %eax
		mov $0, %ebx
		movb 1(%eax), %bl 
		andb $0xF0, %bl
		orb %cl, %bl
		movb %bl, 1(%eax)
		pop %eax		

		/* zerowanie per */
		mov $per, %edx
		movl $0, (%edx)
	
		/* P */
		movl $p, -4(%ebp)
		movb $0, %cl
		enc_p_next_r_byte:	
			mov -8(%ebp), %edx
			movb $0, %ch
			mov $0, %ebx
			movb (%edx, %ecx), %bl	//w bx jest wart. aktualnego bajtu R[]
			enc_p_next_p_byte:
				mov $per, %edx
				push %ecx
				push %eax
				mov %ch, %al
				mov $0, %ecx
				mov %al, %cl
				pop %eax
				add %ecx, %edx	//w dx jest adres akt. bajtu per
				movb (%edx), %al	//w al jest wart akt. bajtu per
				push %edx
				mov -4(%ebp), %edx
				add %ecx, %edx
				push %ebx
				shl %ebx
				movl %ebx, -16(%ebp)
				shl %ebx
				addl -16(%ebp), %ebx
				orb (%edx, %ebx), %al
				pop %ebx
				pop %edx
				movb %al, (%edx)
				pop %ecx 
				incb %ch
				cmpb $4, %ch
				jne enc_p_next_p_byte
			mov -4(%ebp), %eax
			add $1024, %eax
			movl %eax, -4(%ebp)
			incb %cl
			cmpb $4, %cl
			jne enc_p_next_r_byte

		/* kopiowanie per do r[] */
		mov $per, %eax
		mov -8(%ebp), %ebx
		movl (%eax, %ecx), %edx
		movl %edx, (%ebx, %ecx)

		/* xor r[] z l[] */
		mov -8(%ebp), %eax
		mov -12(%ebp), %ebx
		movl (%eax), %edx
		xorl %ebx, %edx
		movl %edx, (%eax)

	
		pusha
		mov $odb, %eax
		call diagprint
		popa
	
		//HERE
		pop %ecx
		inc %ecx
		cmp $16, %ecx
		jne enc_apply_keys

	/* r <-> l */
	mov -8(%ebp), %eax
	mov -12(%ebp), %ecx
	movl (%eax), %ebx
	movl (%ecx), %edx
	movl %ebx, (%ecx)
	movl %edx, (%eax)

	////////////
				
	/* kopiowanie odb do idb */
	mov $odb, %eax
	mov $idb, %ebx
	movq (%eax), %mm0
	movq %mm0, (%ebx)

	/* zerowanie odb /
	mov $odb, %edx
	movl $0, (%edx)	
	movl $0, 4(%edx) */
	
	/* wypiywanie wyniku */
	mov $idb, %eax
	call diagprint
	
	/* FP /
	mov $0, %ecx
	movl $fp, %eax
	enc_fp_next_idb_byte:	
		mov $idb, %edx
		movb $0, %ch
		mov $0, %ebx
		movb (%edx, %ecx), %bl
		movq odb, %mm0
		por (%eax, %ebx, 8), %mm0
		movq %mm0, odb
		add $2048, %eax
		incb %cl
		cmpb $8, %cl
		jne enc_fp_next_idb_byte 	*/


	/* wypiywanie wyniku /
	mov $odb, %eax
	call diagprint*/
	
	mov %ebp, %esp
	pop %ebx
	pop %ebp
	ret

/* decode *************************************************/		
decode:
	push %ebp
	push %ebx
	mov %esp, %ebp
	mov %ebp, %esp

	push $1
	push $nl
	push $1
	call write

	mov %ebp, %esp
	pop %ebx
	pop %ebp
	ret

diagprint:
	push %ebp
	push %ebx
	mov %esp, %ebp

	
	mov $0, %ecx
	mov $0, %ebx
	mov $dp, %edx
	copy_next_char:
		movb (%eax, %ecx), %bl
		movb %bl, (%edx, %ecx, 2)
		inc %ecx
		cmp $8, %ecx
		jne copy_next_char

	push $16
	push $dp
	push $1
	call write
			
	mov %ebp, %esp
	pop %ebx
	pop %ebp
	ret








