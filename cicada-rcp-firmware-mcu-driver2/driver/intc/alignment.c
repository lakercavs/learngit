/*
 *  arch/arm/kernel/alignment.c  - handle alignment exceptions for CSKY CPU. 
 *  
 *  Copyright (C) 2011, C-SKY Microsystems Co., Ltd. (www.c-sky.com) 
 *  Copyright (C) 2011, Hu Junshan (junshan_hu@c-sky.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//extern void die_if_kernel(char *, struct Ckcore_SavedRegisters *, long);

#include "ck810.h"
#include "datatype.h"

#include "misc.h"

#undef HANDLER_SUCCESS
#undef HANDLER_FAILURE
#define HANDLER_SUCCESS 0
#define HANDLER_FAILURE	1

#undef SP_NUM
#define SP_NUM 14

#undef R4_NUM
#define R4_NUM 4
#undef R15_NUM
#define R15_NUM 4
#undef R16_NUM
#define R16_NUM 16
#undef R28_NUM
#define R28_NUM 28

#ifdef CONFIG_SOFT_HANDMISSALIGN

/* C-SKY CPU V2 32 bit instruction like 11'B in the highest two bit  */
#define IS_T32(hi16)  (((hi16) & 0xc000) == 0xc000 )

#define CODING_BITS(i)  (i & 0xFC000000)
#define LDST_TYPE(i)    (i & 0xf000)

#ifdef  __cskyBE__ 
#define BE		1
#define FIRST_BYTE_16	"rotri	%1, 8\n"
#define FIRST_BYTE_32	"rotri	%1, 24\n"
#define NEXT_BYTE	"rotri  %1, 24\n"
#else
#define BE		0
#define FIRST_BYTE_16
#define FIRST_BYTE_32
#define NEXT_BYTE	"lsri   %1, 8\n"
#endif

/*
 * Zero Userspace
 */
//struct exception_table_entry
//{
//	unsigned long insn;
//	unsigned long nextinsn;
//};
//
//extern struct exception_table_entry __start___ex_table[];
//extern struct exception_table_entry __stop___ex_table[];

#define __get8_unaligned_check(val,addr,err)		\
	__asm__(					\
 	"1:	ldb	%1, (%2)\n"			\
 	"	addi	%2, 1\n"			\
	"	br	3f\n"				\
	"2:	movi	%0, 1\n"			\
	"	br	3f\n"				\
	"	.section __ex_table,\"a\"\n"		\
	"	.align	2\n"				\
	"	.long	1b, 2b\n"			\
	"	.previous\n"				\
	"3:\n"						\
	: "=r" (err), "=r" (val), "=r" (addr)		\
	: "0" (err), "2" (addr))

#define get16_unaligned_check(val,addr)				\
	do {							\
		unsigned int err = 0, v, a = addr;		\
		__get8_unaligned_check(v,a,err);		\
		val =  v << ((BE) ? 8 : 0);			\
		__get8_unaligned_check(v,a,err);		\
		val |= v << ((BE) ? 0 : 8);			\
		if (err)					\
			goto fault;				\
	} while (0)

#define get32_unaligned_check(val,addr)				\
	do {							\
		unsigned int err = 0, v, a = addr;		\
		__get8_unaligned_check(v,a,err);		\
		val =  v << ((BE) ? 24 :  0);			\
		__get8_unaligned_check(v,a,err);		\
		val |= v << ((BE) ? 16 :  8);			\
		__get8_unaligned_check(v,a,err);		\
		val |= v << ((BE) ?  8 : 16);			\
		__get8_unaligned_check(v,a,err);		\
		val |= v << ((BE) ?  0 : 24);			\
		if (err)					\
			goto fault;				\
	} while (0)

#define put16_unaligned_check(val,addr)				\
	do {							\
		unsigned int err = 0, v = val, a = addr;	\
		__asm__( FIRST_BYTE_16				\
	 	"1:	stb	%1, (%2)\n"			\
	 	"	addi	%2, 1\n"			\
			NEXT_BYTE				\
		"2:	stb	%1, (%2)\n"			\
		"	br	4f\n"				\
		"3:	movi	%0, 1\n"			\
		"	br	4f\n"				\
		"	.section __ex_table,\"a\"\n"		\
		"	.align	2\n"				\
		"	.long	1b, 3b\n"			\
		"	.long	2b, 3b\n"			\
		"	.previous\n"				\
		"4:\n"						\
		: "=r" (err), "=r" (v), "=r" (a)		\
		: "0" (err), "1" (v), "2" (a));			\
		if (err)					\
			goto fault;				\
	} while (0)

#define put32_unaligned_check(val,addr)				\
	do {							\
		unsigned int err = 0, v = val, a = addr;	\
		__asm__( FIRST_BYTE_32				\
	 	"1:	stb	%1, (%2)\n"			\
	 	"	addi	%2, 1\n"			\
			NEXT_BYTE				\
	 	"2:	stb	%1, (%2)\n"			\
	 	"	addi	%2, 1\n"			\
			NEXT_BYTE				\
	 	"3:	stb	%1, (%2)\n"			\
	 	"	addi	%2, 1\n"			\
			NEXT_BYTE				\
		"4:	stb	%1, (%2)\n"			\
		"	br	6f\n"				\
		"5:	movi	%0, 1\n"			\
		"	br	6f\n"				\
		"	.section __ex_table,\"a\"\n"		\
		"	.align	2\n"				\
		"	.long	1b, 5b\n"			\
		"	.long	2b, 5b\n"			\
		"	.long	3b, 5b\n"			\
		"	.long	4b, 5b\n"			\
		"	.previous\n"				\
		"6:\n"						\
		: "=r" (err), "=r" (v), "=r" (a)		\
		: "0" (err), "1" (v), "2" (a));			\
		if (err)					\
			goto fault;				\
	} while (0)

#define instruction_pointer(regs) ((regs)->pc)
extern int __get_user_bad(void) __attribute__((noreturn));
extern int __put_user_bad(void) __attribute__((noreturn));

#define	EFAULT		14	/* Bad address */

/*
 * CKCORE_SAVED_CONTEXT -- Saved by a normal interrput or exception
 */
typedef struct
{       
    CK_UINT32 pc;
    CK_UINT32 psr;
    CK_UINT32 r1; 
    CK_UINT32 r2; 
    CK_UINT32 r3; 
    CK_UINT32 r4; 
    CK_UINT32 r5; 
    CK_UINT32 r6; 
    CK_UINT32 r7; 
    CK_UINT32 r8; 
    CK_UINT32 r9; 
    CK_UINT32 r10;
    CK_UINT32 r11;
    CK_UINT32 r12;
    CK_UINT32 r13;
    CK_UINT32 r14;
    CK_UINT32 r15;
} __attribute__ ((aligned, packed)) Ckcore_SavedRegisters;

/*
 * These are the main single-value transfer routines.  They automatically
 * use the right size if we just have the right pointer type.
 *
 * This gets kind of ugly. We want to return _two_ values in "get_user()"
 * and yet we don't want to do any pointers, because that is too much
 * of a performance impact. Thus we have a few rather ugly macros here,
 * and hide all the ugliness from the user.
 *
 * The "__xxx" versions of the user access functions are versions that
 * do not verify the address space, that must have been done previously
 * with a separate "access_ok()" call (this is used when we do multiple
 * accesses to the same area of user memory).
 *
 * As we use the same address space for kernel and user data on
 * Ckcore, we can just do these as direct assignments.  (Of course, the
 * exception handling means that it's no longer "just"...)
 */

#define put_user(x,ptr) \
  __put_user_check((x), (ptr), sizeof(*(ptr)))

#define __put_user(x,ptr) \
  __put_user_nocheck((x), (ptr), sizeof(*(ptr)))

#define __ptr(x) ((unsigned long *)(x))

#define get_user(x,ptr) \
  __get_user_check((x), (ptr), sizeof(*(ptr)))

#define __get_user(x,ptr) \
  __get_user_nocheck((x), (ptr), sizeof(*(ptr)))

#define __put_user_nocheck(x, ptr, size)                                \
({                                                                      \
	long __pu_err=0;                                                \
	typeof(*(ptr)) *__pu_addr = (ptr);                              \
	typeof(*(ptr)) __pu_val = (typeof(*(ptr)))(x);                  \
	if(__pu_addr){                                                  \
		__put_user_size(__pu_val, (__pu_addr), (size), __pu_err);   \
	}                                                               \
	__pu_err;                                                       \
})

#define __put_user_check(x,ptr,size)                                    \
({                                                                      \
	long __pu_err = -EFAULT;                                        \
	typeof(*(ptr)) *__pu_addr = (ptr);                              \
	typeof(*(ptr)) __pu_val = (typeof(*(ptr)))(x);                  \
	if (access_ok(VERIFY_WRITE, __pu_addr, size) && __pu_addr)      \
		__put_user_size(__pu_val, __pu_addr, (size), __pu_err); \
	__pu_err;                                                       \
})

#define __put_user_size(x,ptr,size,retval)                              \
do {                                                                    \
	retval = 0;                                                     \
	switch (size) {                                                 \
		case 1: __put_user_asm_b(x, ptr, retval); break;        \
		case 2: __put_user_asm_h(x, ptr, retval); break;        \
		case 4: __put_user_asm_w(x, ptr, retval); break;        \
		case 8: __put_user_asm_64(x, ptr, retval); break;       \
		default: __put_user_bad();                              \
	}	                                                        \
} while (0)

/*
 * We don't tell gcc that we are accessing memory, but this is OK
 * because we do not write to any memory gcc knows about, so there
 * are no aliasing issues.
 *
 * Note that PC at a fault is the address *after* the faulting
 * instruction.
 */
#define __put_user_asm_b(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     stb   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)

#define __put_user_asm_h(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     sth   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)

#define __put_user_asm_w(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     stw   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)


#define __put_user_asm_64(x, ptr, err)                          \
do{                                                             \
	int tmp;                                                \
	int errcode;                                            \
	typeof(*(ptr)) src = ( typeof(*(ptr)))x;                \
	typeof(*(ptr)) *psrc = &src;                            \
	                                                        \
	__asm__ __volatile__(                                   \
	        "     ldw     %3, (%1, 0)     \n"               \
	        "1:   stw     %3, (%2, 0)     \n"               \
	        "     ldw     %3, (%1, 4)     \n"               \
	        "2:   stw     %3, (%2, 4)     \n"               \
	        "     br      4f              \n"               \
	        "3:   mov     %0, %4          \n"               \
	        "     br      4f              \n"               \
	        ".section __ex_table, \"a\"   \n"               \
	        ".align   2                   \n"               \
	        ".long    1b, 3b              \n"               \
	        ".long    2b, 3b              \n"               \
	        ".previous                    \n"               \
	        "4:                           \n"               \
	        :"=r"(err), "=r"(psrc), "=r"(ptr), "=r"(tmp), "=r"(errcode) \
	        : "0"(err), "1"(psrc), "2"(ptr), "3"(0), "4"(-EFAULT)       \
	        : "memory" );                                   \
}while (0)

#define __get_user_nocheck(x, ptr, size)                        \
({                                                              \
	long  __gu_err;	                                        \
	__get_user_size(x, (ptr), (size), __gu_err);            \
	__gu_err;                                               \
})

#define __get_user_check(x, ptr, size)                          \
({                                                              \
	int __gu_err = -EFAULT;	                                \
	const __typeof__(*(ptr)) __user * __gu_ptr = (ptr);     \
	if (access_ok(VERIFY_READ, __gu_ptr, size) && __gu_ptr) \
		__get_user_size(x, __gu_ptr, (size), __gu_err); \
	__gu_err;                                               \
})

#define __get_user_size(x, ptr, size, retval)                   \
do {                                                            \
	switch (size) {                                         \
		case 1: __get_user_asm_common((x),ptr,"ldb",retval); break; \
		case 2: __get_user_asm_common((x),ptr,"ldh",retval); break; \
		case 4: __get_user_asm_common((x),ptr,"ldw",retval); break; \
		default:                                        \
			x=0;                                    \
			(retval) = __get_user_bad();            \
	}                                                       \
} while (0)

#define __get_user_asm_common(x, ptr, ins, err)                 \
do{                                                             \
	int errcode;                                            \
	__asm__ __volatile__(                                   \
	        "1:   " ins "   %1, (%4,0)      \n"             \
	        "       br    3f                \n"             \
	        /* Fix up codes */                              \
	        "2:     mov   %0, %2            \n"             \
	        "       movi  %1, 0             \n"             \
	        "       br    3f                \n"             \
	        ".section __ex_table,\"a\"      \n"             \
	        ".align   2                     \n"             \
	        ".long    1b,2b                 \n"             \
	        ".previous                      \n"             \
	        "3:                            \n"              \
	        :"=r"(err), "=r"(x), "=r"(errcode)              \
	        :"0"(0), "r"(ptr), "2"(-EFAULT)                 \
	        : "memory");                                    \
}while(0)
inline static unsigned int 
get_regs_value(unsigned int rx, Ckcore_SavedRegisters *regs){
	unsigned int value;

	if(rx < 14){
		value  = *((int *)regs + rx + 3);
	}else if(rx == 14){
        value = sizeof(Ckcore_SavedRegisters) + ((unsigned int)regs);
	}else{
		value = *((int *)regs + rx + 2);
	}

	return value;
}

inline static int
put_regs_value(unsigned int value, unsigned int rx, Ckcore_SavedRegisters *regs){
	if(rx < 14){
		*((int *)regs + rx + 3) = value;
	}else if(rx == 14){
        printf("alignment handler trying to write sp.\n");
        goto fault;
	}else{
		*((int *)regs + rx + 2) = value;
	}

	return 0;
fault:
	return 1;
}

static int 
handle_ldh_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 8) & 0x7;
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int destaddr;
	unsigned int dataregx;
	unsigned short tmpval16;

	dataregx = get_regs_value(regx, regs);
	destaddr = dataregx + (imm5 << 1);
	get16_unaligned_check(tmpval16, destaddr);
	if(put_regs_value((unsigned int)tmpval16, regz, regs) != 0){
			goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_ldw_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 8) & 0x7;
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int destaddr,tmpval32;
	unsigned int dataregx;

	dataregx = get_regs_value(regx, regs);
	destaddr = dataregx + (imm5 << 2);
	get32_unaligned_check(tmpval32, destaddr);
	if(put_regs_value(tmpval32, regz, regs) != 0){
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_ldw_sp_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int imm3 = (instr >> 8) & 0x7;
	unsigned int destaddr,tmpval32;
	unsigned int datasp;

	datasp = get_regs_value(SP_NUM, regs);
	destaddr = datasp + (((imm3 << 5) | imm5) << 2);
	get32_unaligned_check(tmpval32, destaddr);
	if(put_regs_value(tmpval32, regz, regs) != 0){
		goto fault;
	}
	
	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_sth_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 8) & 0x7;
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int destaddr;
	unsigned int dataregx,dataregz;

	dataregx = get_regs_value(regx, regs);
	destaddr = dataregx + (imm5 << 1);
	dataregz = get_regs_value(regz, regs);
	put16_unaligned_check(dataregz, destaddr);
	
	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_stw_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 8) & 0x7;
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int destaddr;
	unsigned int dataregx,dataregz;

	dataregx = get_regs_value(regx, regs);
	destaddr = dataregx + (imm5 << 2);
	dataregz = get_regs_value(regz, regs);
	put32_unaligned_check(dataregz, destaddr);
	
	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_stw_sp_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regz = (instr >> 5) & 0x7;
	unsigned int imm5 = instr & 0x1f;
	unsigned int imm3 = (instr >> 8) & 0x7;
	unsigned int destaddr;
	unsigned int datasp,dataregz;

	datasp = get_regs_value(SP_NUM, regs);
	destaddr = datasp + (((imm3 << 5) | imm5) << 2);
	dataregz = get_regs_value(regz, regs);
	put32_unaligned_check(dataregz, destaddr);
	
	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_push_pop_16(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int push_pop = instr & 0xffe0;
	unsigned int list1 = instr & 0xf;
	unsigned int has_r15 = (instr & 0x0010) >> 4;
	unsigned int datasp, tmpval32, i;

	datasp = get_regs_value(SP_NUM, regs);

	if(push_pop == 0x1480){ // pop
		for(i = 0; i < list1; i++){
			get32_unaligned_check(tmpval32, datasp + i * 4);
			if(put_regs_value(tmpval32, R4_NUM + i, regs) != 0){
				goto fault;
			}
		}
		if(has_r15){
			get32_unaligned_check(tmpval32, datasp + list1 * 4);
			if(put_regs_value(tmpval32, R15_NUM, regs) != 0){
				goto fault;
			}
		}
		datasp += (list1 + has_r15) * 4;
		if(put_regs_value(datasp, SP_NUM, regs) != 0){
			goto fault;
		}
	}else if(push_pop == 0x14c0){ // push
		datasp -= (list1 + has_r15) * 4;
		if(put_regs_value(datasp, SP_NUM, regs) != 0){
			goto fault;
		}
		for(i = 0; i < list1; i++){
			tmpval32 = get_regs_value(R4_NUM + i, regs);
			put32_unaligned_check(tmpval32, datasp + i * 4);
		}
		if(has_r15){
			tmpval32 = get_regs_value(R15_NUM, regs);
			put32_unaligned_check(tmpval32, datasp + list1 * 4);
		}
	}else{
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_push_pop(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int push_pop = instr & 0xfffffe00;
	unsigned int list1 = instr & 0xf;
	unsigned int list2 = (instr >> 5) & 0x7;
	unsigned int has_r15 = (instr >> 4) & 0x1;
	unsigned int has_r28 = (instr >> 8) & 0x1;
	unsigned int datasp, tmpval32, i;

	datasp = get_regs_value(SP_NUM, regs);

	if(push_pop == 0x1480){ // pop
		for(i = 0; i < list1; i++){
			get32_unaligned_check(tmpval32, datasp + i * 4);
			if(put_regs_value(tmpval32, R4_NUM + i, regs) != 0){
				goto fault;
			}
		}
		if(has_r15 == 0x1){
			get32_unaligned_check(tmpval32, datasp + list1 * 4);
			if(put_regs_value(tmpval32, R15_NUM, regs) != 0){
				goto fault;
			}
		}
		for(i = 0; i < list2; i++){
			get32_unaligned_check(tmpval32, datasp + (i + list1 + has_r15) * 4);
			if(put_regs_value(tmpval32, R16_NUM + i, regs) != 0){
				goto fault;
			}
		}
		if(has_r28 == 0x1){
			get32_unaligned_check(tmpval32, datasp + (list1 + list2 + has_r15) * 4);
			if(put_regs_value(tmpval32, R28_NUM, regs) != 0){
				goto fault;
			}
		}
		datasp += (list1 + list2 + has_r15 + has_r28) * 4;
		if(put_regs_value(datasp, SP_NUM, regs) != 0){
			goto fault;
		}
	}else if(push_pop == 0x14c0){ // push
		datasp -= (list1 + list2 + has_r15 + has_r28) * 4;
		if(put_regs_value(datasp, SP_NUM, regs) != 0){
			goto fault;
		}
		for(i = 0; i < list1; i++){
			tmpval32 = get_regs_value(R4_NUM + i, regs);
			put32_unaligned_check(tmpval32, datasp + i * 4);
		}
		if(has_r15 == 0x1){
			tmpval32 = get_regs_value(R15_NUM, regs);
			put32_unaligned_check(tmpval32, datasp + list1 * 4);
		}
		for(i = 0; i < list2; i++){
			tmpval32 = get_regs_value(R16_NUM + i, regs);
			put32_unaligned_check(tmpval32, datasp + (i + list1 + has_r15) * 4);
		}
		if(has_r28 == 0x1){
			tmpval32 = get_regs_value(R28_NUM, regs);
			put32_unaligned_check(tmpval32, datasp + (list1 + list2 + has_r15) * 4);
		}
	}else{
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_ldh_ldhs_ldw_ldd(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 16) & 0x1f;
	unsigned int regz = (instr >> 21) & 0x1f;
	unsigned int offset = instr & 0xfff;
	unsigned int destaddr, tmpval32;
	unsigned int dataregx;
	unsigned short tmpval16;

	dataregx = get_regs_value(regx, regs);

	switch((instr >> 12) & 0xf){
	case 1: // ldh
		destaddr = dataregx + (offset << 1);
		get16_unaligned_check(tmpval16, destaddr);
		if(put_regs_value((unsigned int)tmpval16, regz, regs) != 0){
			goto fault;
		}
		break;
	case 3: // ldd
		destaddr = dataregx + (offset << 2);
		get32_unaligned_check(tmpval32, destaddr);
		if(put_regs_value(tmpval32, regz, regs) != 0){
			goto fault;
		}
		get32_unaligned_check(tmpval32, destaddr + 4);
		if(put_regs_value(tmpval32, regz + 1, regs) != 0){
			goto fault;
		}
		break;
	case 5: // ldhs
		destaddr = dataregx + (offset << 1);
		get16_unaligned_check(tmpval16, destaddr);
		if(put_regs_value((unsigned int)((short)tmpval16), regz, regs) != 0){
			goto fault;
		}
		break; 
	case 2: // ldw
		destaddr = dataregx + (offset << 2);
		get32_unaligned_check(tmpval32, destaddr);
		if(put_regs_value(tmpval32, regz, regs) != 0){
			goto fault;
		}
		break;
	default:
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_sth_stw_std(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 16) & 0x1f;
	unsigned int regz = (instr >> 21) & 0x1f;
	unsigned int offset = instr & 0xfff;
	unsigned int destaddr;
	unsigned int dataregx, dataregz;

	dataregx = get_regs_value(regx, regs);

	switch((instr >> 12) & 0xf){
	case 1: // sth
		destaddr = dataregx + (offset << 1);
		dataregz = get_regs_value(regz, regs);
		put16_unaligned_check(dataregz, destaddr);
		break;
	case 3: // std
		destaddr = dataregx + (offset << 2);
		dataregz = get_regs_value(regz, regs);
		put32_unaligned_check(dataregz, destaddr);
		dataregz = get_regs_value(regz + 1, regs);
		put32_unaligned_check(dataregz, destaddr + 4);
		break;
	case 2: // stw
		destaddr = dataregx + (offset << 2);
		dataregz = get_regs_value(regz, regs);
		put32_unaligned_check(dataregz, destaddr);
		break;
	default:
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_ldrh_ldrhs_ldrw_ldm(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 16) & 0x1f;
	unsigned int regy = (instr >> 21) & 0x1f;
	unsigned int regz = instr & 0x1f;
	unsigned int dataregx, dataregy;
	unsigned int destaddr, tmpval32, i;
	unsigned short tmpval16;

	dataregx = get_regs_value(regx, regs);
	dataregy = get_regs_value(regy, regs);

	destaddr = dataregx + dataregy * ((instr >> 5) & 0x1f);

	switch((instr >> 10) & 0x3f){
	case 1: // ldrh
		get16_unaligned_check(tmpval16, destaddr);
		if(put_regs_value((unsigned int)tmpval16, regz, regs) != 0){
			goto fault;
		}
		break;
	case 5: // ldrhs
		get16_unaligned_check(tmpval16, destaddr);
		if(put_regs_value((unsigned int)((short)tmpval16), regz, regs) != 0){
			goto fault;
		}
		break;
	case 2: // ldrw
		get32_unaligned_check(tmpval32, destaddr);
		if(put_regs_value(tmpval32, regz, regs) != 0){
			goto fault;
		}
		break;
	case 7: // ldm	
		for(i = 0; i <= (instr & 0x1f); i++){
			get32_unaligned_check(tmpval32, dataregx + i * 4);
			if(put_regs_value(tmpval32, regy + i, regs) != 0){
				goto fault;
			}
		}
		return HANDLER_SUCCESS;
	default:
		goto fault;
	}
	
	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

static int 
handle_strh_strw_stm(unsigned long instr, Ckcore_SavedRegisters *regs){
	unsigned int regx = (instr >> 16) & 0x1f;
	unsigned int regy = (instr >> 21) & 0x1f;
	unsigned int regz = instr & 0x1f;
	unsigned int dataregx, dataregy, dataregz;
	unsigned int destaddr, tmpval32, i;

	dataregx = get_regs_value(regx, regs);
	dataregy = get_regs_value(regy, regs);

	destaddr = dataregx + dataregy * ((instr >> 5) & 0x1f);

	switch((instr >> 10) & 0x3f){
	case 1: // strh
		dataregz = get_regs_value(regz, regs);
		put16_unaligned_check(dataregz, destaddr);
		break;
	case 2: // strw
		dataregz = get_regs_value(regz, regs);
		put32_unaligned_check(dataregz, destaddr);
		break;
	case 7: //  stm
		for(i = 0; i <= (instr & 0x1f); i++){
			tmpval32 = get_regs_value(regy + i, regs);
			put32_unaligned_check(tmpval32, dataregx + 4 * i);
		}
	default:
		goto fault;
	}

	return HANDLER_SUCCESS;
fault:
	return HANDLER_FAILURE;	
}

void CK_INTC_AlignmentService(int vector, Ckcore_SavedRegisters *regs)
{
	int err;
	unsigned long instr = 0, instrptr;
	unsigned int fault;
	unsigned short tinstr = 0;
	int (*handler)(unsigned long inst, Ckcore_SavedRegisters *regs) = NULL;
	int isize = 2;
printf("....alignment!\n");
    CK_INTC_UpdateExceptStatus(vector);

	instrptr = instruction_pointer(regs);

	fault = __get_user(tinstr, (unsigned short *)(instrptr & ~1));
	instr = (unsigned long)tinstr;

	if (!fault) {
		if (IS_T32(tinstr)) {
			unsigned short tinst2 = 0;
			fault = __get_user(tinst2, (unsigned short *)(instrptr+2));
			instr = (tinstr << 16) | tinst2;	
			isize = 4;
		}
	}

	if (fault) {
		goto bad_or_fault;
	}

	regs->pc += isize;

	if(2 == isize ) {
		switch(instr & 0xf800){
		case 0x8800: // ldh
			handler = handle_ldh_16;
			break;
		case 0x9800: // ldw sp
			handler = handle_ldw_sp_16;
			break;
		case 0x9000: // ldw 
			handler = handle_ldw_16;
			break;
		case 0xa800: // ld
			handler = handle_sth_16;
			break;
		case 0xb000:
			handler = handle_stw_16;
			break;
		case 0xb800:
			handler = handle_stw_sp_16;
			break;
		case 0x0100:
			handler = handle_push_pop_16;
			printf("warnning: push/pop alignment.\n");
			break;
		default:
			goto bad_or_fault;
		}
	} 
	else {
		switch (CODING_BITS(instr)) {
		case 0xD8000000: // ld.h/ld.hs/ld.w/ld.d
			handler = handle_ldh_ldhs_ldw_ldd;
			break;
		case 0xDC000000: // st.h/st.w/st.d
			handler = handle_sth_stw_std;
			break;
		case 0xD0000000: // ldr.w/ldr.h/ldr.hs/ldm
			handler = handle_ldrh_ldrhs_ldrw_ldm;
			break;
		case 0xD4000000: // stm/str.h/str.w
			handler = handle_strh_strw_stm;
			break;
		case 0xE8000000:  // push/pop instruction.
			printf("warnning: push/pop alignment.\n");
			handler = handle_push_pop;
			break;
		default:
			// FIXME: stq/stq is pseudo instruction of stm/stm and now ignore.
			goto bad_or_fault;
		}
	}
	
	if (!handler)
		goto bad_or_fault;

	err = handler(instr, regs);
	if (err != HANDLER_SUCCESS)
	{
		regs->pc -=2;
		goto bad_or_fault;
	}

	return;

bad_or_fault:
    /* @todo Register exception instruction to some table */
//	if(fixup_exception(regs)) {
//	    return;
//	}
//	die_if_kernel("Alignment trap: not handle this instruction", regs, 0);

	return;
}

#else /* !CONFIG_SOFT_HANDMISSALIGN */

void CK_INTC_AlignmentService(Ckcore_SavedRegisters *regs)
{
//	if(fixup_exception(regs)) {
//	    return;
//	}
//	die_if_kernel("Kernel mode Alignment exception", regs, 0);
	return;
}

#endif /* CONFIG_SOFT_HANDMISSALIGN */
