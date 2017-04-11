/*
 * Filename: texcept.c
 * Description: To test the except driver program
 * Author: Liu Changyu
 * date: Oct 31 2008
 */

#include <math.h>
#include "ck810.h"
#include "cache.h"
#include "intc.h"
#include "misc.h"

static double sqrtm1(double x)
{
  return sqrt(x) - 1.0;
}

static void FPU_Except_Test()
{
  CK_UINT32 status;
  double x, y;

  printf("\nExcept Test: FPU. . . \n");

  x = -4.2;
  CK_CPU_ClearFpeStatus();
  y = sqrtm1(x);
  status = CK_CPU_GetFpeStatus();
  printf("%g %g %d\n", x, y, status);
}

static void Alignment_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  CK_UINT32 result = *((CK_UINT8 *)&exceptStatus + 1);
  printf("\nExcept Test: Alignment. . . \n");

  asm  ("lrw r7, 0x18000001");
  asm  ("ldw r8, (r7, 0x0)");

  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_ALIGN);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void Access_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  printf("\nExcept Test: Access. . . \n");

  CK_UINT8 result = *((CK_UINT8 *)0xFFFFFFFF);

  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_ACCESS);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void ZeroDiv_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  CK_UINT32 denomi;

  printf("\nExcept Test: ZeroDiv. . . \n");
  denomi /= 0;
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_ZERODIV);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void Illegal_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;

  printf("\nExcept Test: Illegal. . . \n");
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_ILLEGAL);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void Privil_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;

  printf("\nExcept Test: Privil. . . \n");
  asm  ("mtcr   a0, ss0\n\t"
        "mfcr   a0, psr\n\t"
        "bclri  a0, 31\n\t"
        "mtcr   a0, psr\n\t"
        "mfcr   a0, ss0\n\t"); /* User Mode mfcr command execute */
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_PRIV);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void Unrecover_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  CK_UINT32 denomi;

  printf("\nExcept Test: Unrecover. . . \n");
  denomi /= 0;
  asm  ("psrclr ee");
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_UNRECOVER);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void Idly_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;

  printf("\nExcept Test: Idly. . . \n");
  asm  ("idly\n\t"
        "mov    sp, sp\n\t"
        "mov    sp, sp\n\t"
/* @todo Need to Add Bus Access Exception */
        "mov    sp, sp\n\t"
        "mov    sp, sp\n\t"
        "mov    sp, sp\n\t");

  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_IDLY);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void HWaccel_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  CK_UINT32 getValue;

  printf("\nExcept Test: IHWaccel. . . \n");
  /* clear CCR2.PLD */
  asm  ("mtcr   a0, ss0\n\t"
        "mfcr   a0, cr<31, 0>\n\t"
        "bclri  a0, 2\n\t"
        "mtcr   a0, cr<31, 0>\n\t"
        "mfcr   a0, ss0\n\t");
  /* flush DCache */
  CK_Cache_FlushD();
  /* serious liner Memory Access */
  getValue = *((CK_UINT32 *)0x100000);
  getValue = *((CK_UINT32 *)0x200000);
  getValue = *((CK_UINT32 *)0x300000);
  getValue = *((CK_UINT32 *)0x400000);
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_HWACCEL);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

static void TLB_Except_Test()
{
  PCKStruct_ExceptSts exceptStatus;
  CK_UINT32 getValue;

  printf("\nExcept Test: TLB series. . . \n");
  /* Invalid All TLB */
  asm  ("bgeni	a0,	26\n\t"
        "mtcr	a0,	cr<8,15>\n\t");
  /* Memory Access */
  getValue = *((CK_UINT32 *)0x100000);
  getValue = *((CK_UINT32 *)0x200000);
  getValue = *((CK_UINT32 *)0x300000);
  getValue = *((CK_UINT32 *)0x400000);
  /* @todo other TLB Exception */
  exceptStatus = CK_CPU_GetExceptStatus(CKCORE_VECTOR_TLBMISS);
  printf("status=%d::count=%d\n", exceptStatus -> status, exceptStatus -> count);
}

/*
 * main function of expect test program.
 */
void CK_Except_Test()
{
  printf("\nStart Except Test. . . \n");

  /* FPU Exception Test */
//  FPU_Except_Test();
  /* Alignment Exception Test */
  Alignment_Except_Test();
  /* Access Exception Test */
  Access_Except_Test();
  /* ZeroDiv Exception Test */
  ZeroDiv_Except_Test();
  /* @todo Illegal Exception Test */
  Illegal_Except_Test();
  /* Privil Exception Test */
  Privil_Except_Test();
  /* Unrecover Exception Test */
  Unrecover_Except_Test();
  /* Idly Exception Test */
  Idly_Except_Test();
  /* HW Accel Exception Test */
  HWaccel_Except_Test();
  /* TLB Exception Test */
  TLB_Except_Test();

  /* trap */
  CK_CPU_TrapTrigger();

  printf("\nEnd Except Test. . . \n");
}

