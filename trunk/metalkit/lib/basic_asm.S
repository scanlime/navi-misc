/*
 * ====================================================
 * Copyright (C) 1998, 2002 by Red Hat Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

        .global memcpy
memcpy:

        pushl %ebp
        movl %esp,%ebp
        pushl %esi
        pushl %edi
        pushl %ebx
        movl 8(%ebp),%edi
        movl 16(%ebp),%ecx
        movl 12(%ebp),%esi
        cld

        cmpl $8,%ecx
        jbe .L3

/* move any preceding bytes until destination address is long word aligned */

        movl %edi,%edx
        movl %ecx,%ebx
        andl $3,%edx
        jz .L11
        movl $4,%ecx
        subl %edx,%ecx
        andl $3,%ecx
        subl %ecx,%ebx
        rep
        movsb

        mov %ebx,%ecx

/* move bytes a long word at a time */

.L11:
        shrl $2,%ecx
        .p2align 2
        rep
        movsl

        movl %ebx,%ecx
        andl $3,%ecx

/* handle any remaining bytes */

.L3:
        rep
        movsb
.L5:
        movl 8(%ebp),%eax

        leal -12(%ebp),%esp
        popl %ebx
        popl %edi
        popl %esi
        leave
        ret


        .global memset
memset:

        pushl %ebp
        movl %esp,%ebp
        pushl %edi
        pushl %ebx
        movl 8(%ebp),%edi
        movl 12(%ebp),%eax
        movl 16(%ebp),%ecx
        cld

        andl $255,%eax
        movl %ecx,%ebx
        testl $3,%edi
        jne .L19
        cmpl $16,%ecx
        jbe .L19

        movl %eax,%edx
        sall $8,%eax
        orl %edx,%eax

        movl %eax,%edx
        sall $16,%edx
        orl %edx,%eax

        shrl $2,%ecx
        andl $3,%ebx
        rep
        stosl
        movl %ebx,%ecx

.L19:
        rep
        stosb

        movl 8(%ebp),%eax

        leal -8(%ebp),%esp
        popl %ebx
        popl %edi
        leave
        ret

