/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_reset.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-09#$
  $Rev: 10877 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#include        <nitro.h>
#include        <nitro/mb.h>
#include        <nitro/snd/common/global.h>
#include        <nitro/card.h>

#include        <nitro/code32.h>

//---- autoload start
#ifdef SDK_ARM9
#define OSi_HW_DTCM               SDK_AUTOLOAD_DTCM_START
#endif

//---------------- local functions
#ifdef SDK_ARM9
static void OSi_CpuClear32(register u32 data, register void *destp, register u32 size);
static void OSi_ReloadRomData(BOOL ontwl);
static void OSi_ReadCardRom32(u32 src, void *dst, int len);
#endif // ifdef SDK_ARM9

void OSi_DoResetSystem(void);
static void OSi_DoBoot(void);

#ifdef  SDK_TWL
static void OSi_ReloadTwlRomData(void);
#ifdef  SDK_ARM7
extern void SDK_LTDAUTOLOAD_LTDMAIN_START(void);
void* const OSi_LtdMainParams[] =
{
    (void*)SDK_LTDAUTOLOAD_LTDMAIN_START,
};
#endif
#endif

//---------------- local variables
static u16 OSi_IsInitReset = FALSE;

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitReset

  Description:  init reset system

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitReset(void)
{
    if (OSi_IsInitReset)
    {
        return;
    }
    OSi_IsInitReset = TRUE;

    //---------------- ARM9
#ifdef SDK_ARM9
    //---- wait ARM7 PXI library start
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_OS, PXI_PROC_ARM7))
    {
    }
#endif

    //---- set PXI receive callback
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_OS, OSi_CommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         OS_ResetSystem

  Description:  Start reset sequence.
                To decrease code in itcm or wram,
                separate this function from OSi_DoResetSystem()

  Arguments:    (ARM9) parameter : reset parameter
                (ARM7) None

  Returns:      None
 *---------------------------------------------------------------------------*/
//---------------- ARM9
#ifdef SDK_ARM9
void OS_ResetSystem(u32 parameter)
{
#ifdef  SDK_TWL
    //---- stop if reset from non-card application
    if ((MB_IsMultiBootChild() == TRUE) ||
        ((OS_IsRunOnTwl() == TRUE) && ((*((u32*)(HW_TWL_ROM_HEADER_BUF + 0x234)) & 0x00000004) != 0)))
    {
#ifndef SDK_FINALROM
        OS_TPanic("Only card booted application can execute software reset.\nSee OS_ResetSystem() reference manual.");
#else
        OS_TPanic("");
#endif
    }
#else
    //---- stop if reset from multiboot child
    if (MB_IsMultiBootChild())
    {
#ifndef SDK_FINALROM
        OS_TPanic("Cannot reset from MB child.\nSee OS_ResetSystem() reference manual.");
#else
        OS_TPanic("");
#endif
    }
#endif

#ifdef  SDK_TWL
    if ( OS_IsRunOnTwl() )
    {
        // �I������
        // ARM7�ɏ������˗����Ă���ꍇ�̓R�[���o�b�N�֐����Ŋ�����҂���
        PMi_ExecutePostExitCallbackList();
    }
#endif

    //---- wait while card is locked
    {
        u16     id = (u16)OS_GetLockID();
        CARD_LockRom(id);
    }

    //---- set only pxi interrupt available
    (void)OS_SetIrqMask(OS_IE_FIFO_RECV);
    (void)OS_ResetRequestIrqMask(0xffffffff /*all request */ );

    //---- stop all dma
	MI_StopAllDma();
#ifdef SDK_TWL
	if (OS_IsRunOnTwl())
	{
		MI_StopAllNDma();
	}
#endif

    //---- save parameter
    *(u32 *)HW_RESET_PARAMETER_BUF = parameter;

    //---- send reset command to ARM7
    OSi_SendToPxi(OS_PXI_COMMAND_RESET);

    //---- reset
    OSi_DoResetSystem();

    // don't reach
}

//---------------- ARM7
#else  // ifdef SDK_ARM9
#pragma dont_inline on
void OS_ResetSystem(void)
{
    //---- set only pxi interrupt available
    (void)OS_SetIrqMask(OS_IE_FIFO_RECV);
    (void)OS_ResetRequestIrqMask(0xffffffff /*all request */ );

    //---- stop all dma
	{
		u32 n;
		for( n=0; n<4; n++ )
		{
			MI_StopDma(n);
#ifdef SDK_TWL
			if (OS_IsRunOnTwl())
			{
				MI_StopNDma(n);
			}
#endif
		}
	}

    //---- sound stop
    SND_Shutdown();

    //---- send reset command to ARM9
    OSi_SendToPxi(OS_PXI_COMMAND_RESET);

    //---- reset
    OSi_DoResetSystem();
}
#pragma dont_inline reset
#endif // ifdef SDK_ARM9

#define SDK_OS_RESET_COMPACT

#if defined(SDK_TWL) && defined(SDK_ARM9)
#include <twl/itcm_begin.h>

#if defined(SDK_OS_RESET_COMPACT)
// �\�t�g�E�F�A���Z�b�g�̍ŏI�i�K�Ŏg�p����SVC�������܂Ƃ߂�ITCM�֑ޔ��B
#include <nitro/code16.h>
#define SVC_ID_SHA1_INIT   36
#define SVC_ID_SHA1_UPDATE 37
#define SVC_ID_SHA1_FINAL  38
#define SVC_ID_CALC_SHA1   39
#define SVC_ID_CMP_SHA1    40
asm static BOOL OSi_VerifyStaticSegmentHash(const u8 *src, u32 len, const void *digest)
{
#define VAR_STACK_DIGEST  (0)
#define VAR_STACK_PADBUF  (VAR_STACK_DIGEST  + SVC_SHA1_DIGEST_SIZE)
#define VAR_STACK_CONTEXT (VAR_STACK_PADBUF  + SVC_SHA1_BLOCK_SIZE)
#define VAR_STACK_TOTAL   (VAR_STACK_CONTEXT + sizeof(SVCSHA1Context))
    PUSH    {R3, R4, R5, R6, R7, LR}
    SUB     SP, #VAR_STACK_TOTAL // 0xB8
    MOV     R4, R1 // len
    MOV     R5, R0 // src
    MOV     R7, R2 // digest
    // SHA-1�\���̂��������B
    ADD     R0, SP, #VAR_STACK_CONTEXT
    MOV     R3, #SVCSHA1Context.sha_block
    ADD     R3, R0
    MOV     R2, #0
    STR     R2, [R3]
    SWI     SVC_ID_SHA1_INIT
    // ipad����́B
    LDR     R0, =@key
    ADD     R1, SP, #VAR_STACK_PADBUF
    MOV     R2, #0x0
    MOV     R3, #0x36
@0:
    LDRB    R6, [R0, R2]
    EOR     R6, R3
    STRB    R6, [R1, R2]
    ADD     R2, R2, #0x1
    CMP     R2, #SVC_SHA1_BLOCK_SIZE
    BLT     @0
    ADD     R0, SP, #VAR_STACK_CONTEXT
    SWI     SVC_ID_SHA1_UPDATE
    // ���b�Z�[�W����́B
    ADD     R0, SP, #VAR_STACK_CONTEXT
    MOV     R1, R5
    MOV     R2, R4
    SWI     SVC_ID_SHA1_UPDATE
    // i�n�b�V�����Z�o�B
    ADD     R0, SP, #VAR_STACK_DIGEST
    ADD     R1, SP, #VAR_STACK_CONTEXT
    SWI     SVC_ID_SHA1_FINAL
    // SHA-1�\���̂��������B
    ADD     R0, SP, #VAR_STACK_CONTEXT
    MOV     R3, #SVCSHA1Context.sha_block
    ADD     R3, R0
    MOV     R2, #0
    STR     R2, [R3]
    SWI     SVC_ID_SHA1_INIT
    // opad����́B
    LDR     R0, =@key
    ADD     R1, SP, #VAR_STACK_PADBUF
    MOV     R2, #0x0
    MOV     R3, #0x5C
@1:
    LDRB    R6, [R0, R2]
    EOR     R6, R3
    STRB    R6, [R1, R2]
    ADD     R2, R2, #0x1
    CMP     R2, #SVC_SHA1_BLOCK_SIZE
    BLT     @1
    ADD     R0, SP, #VAR_STACK_CONTEXT
    SWI     SVC_ID_SHA1_UPDATE
    // i�n�b�V������́B
    ADD     R0, SP, #VAR_STACK_CONTEXT
    ADD     R1, SP, #VAR_STACK_DIGEST
    MOV     R2, #SVC_SHA1_DIGEST_SIZE
    SWI     SVC_ID_SHA1_UPDATE
    // o�n�b�V�����Z�o�B
    ADD     R0, SP, #VAR_STACK_DIGEST
    ADD     R1, SP, #VAR_STACK_CONTEXT
    SWI     SVC_ID_SHA1_FINAL
    // o�n�b�V�����r�B
    ADD     R0, SP, #VAR_STACK_DIGEST
    MOV     R1, R7
    SWI     SVC_ID_CMP_SHA1
    CMP     R0, #0x0
    BEQ     @2
    MOV     R0, #0x1
@2:
    ADD     SP, #0xB8
    POP     {R3, R4, R5, R6, R7, PC}
    NOP
#undef VAR_STACK_DIGEST
#undef VAR_STACK_PADBUF
#undef VAR_STACK_CONTEXT
#undef VAR_STACK_TOTAL
@key:
    dcd 0xDEC00621
    dcd 0x3FCE98BA
    dcd 0x9DE392A6
    dcd 0x01EDF246
    dcd 0x08CCE376
    dcd 0xFA632356
    dcd 0xDFECD4CA
    dcd 0x3478629A
    dcd 0x3C636D8F
    dcd 0x92CA22FE
    dcd 0x23978820
    dcd 0xC2AECFD2
    dcd 0xFE8D6732
    dcd 0x986483CA
    dcd 0x373EFDAC
    dcd 0x24584687
}
#include <nitro/codereset.h>
#include <nitro/code32.h>
#endif

#if !defined(SDK_OS_RESET_COMPACT)
#define OSi_TCM_REGION_BASE_MASK  HW_C9_TCMR_BASE_MASK  // 0xfffff000
static asm u32 OSi_GetDTCMAddress(void)
{
    mrc      p15, 0, r0, c9, c1, 0
    ldr      r1, =OSi_TCM_REGION_BASE_MASK
    and      r0, r0, r1
    bx       lr
}
#endif
/*---------------------------------------------------------------------------*
  Name:         OSi_VerifyStaticSegments

  Description:  Verify 4 static segments before rebooting.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_VerifyStaticSegments(void)
{
    BOOL                    result = TRUE;
    const CARDRomHeaderTWL *header = (const CARDRomHeaderTWL *)HW_TWL_ROM_HEADER_BUF;
    if ((((const u8 *)header)[0x1C] & 0x01) != 0)
    {
        int                     i;
#if !defined(SDK_OS_RESET_COMPACT)
        // use DTCM as workarea
        u8                     *work = (u8 *)OSi_GetDTCMAddress();
        SVCHMACSHA1Context     *context = (SVCHMACSHA1Context *)&work[0];
        u8                     *digest = &work[sizeof(SVCHMACSHA1Context)];
#else
        BOOL OSi_VerifyStaticSegmentHash(const u8 *src, u32 len, const void *digest);
#endif
        // calculate and verify HMAC-SHA1 of the static segments
        enum { SEGMENT_TOTAL = 4 };
        struct Segment { u8 *address; u32 size; } *(table[SEGMENT_TOTAL]);
        const void             *(compareDigests[SEGMENT_TOTAL]);
        table[0] = (struct Segment *)&header->ntr.main_ram_address;
        table[1] = (struct Segment *)&header->ntr.sub_ram_address;
        table[2] = (struct Segment *)&header->main_ltd_ram_address;
        table[3] = (struct Segment *)&header->sub_ltd_ram_address;
        compareDigests[0] = &((const u8 *)header)[0x3A0];
        compareDigests[1] = header->sub_static_digest;
        compareDigests[2] = header->main_ltd_static_digest;
        compareDigests[3] = header->sub_ltd_static_digest;
        for (i = 0; i < SEGMENT_TOTAL; ++i)
        {
#if !defined(SDK_OS_RESET_COMPACT)
            static const u8 hmacKey[] =
            {
                0x21, 0x06, 0xc0, 0xde, 0xba, 0x98, 0xce, 0x3f,
                0xa6, 0x92, 0xe3, 0x9d, 0x46, 0xf2, 0xed, 0x01,
                0x76, 0xe3, 0xcc, 0x08, 0x56, 0x23, 0x63, 0xfa,
                0xca, 0xd4, 0xec, 0xdf, 0x9a, 0x62, 0x78, 0x34,
                0x8f, 0x6d, 0x63, 0x3c, 0xfe, 0x22, 0xca, 0x92,
                0x20, 0x88, 0x97, 0x23, 0xd2, 0xcf, 0xae, 0xc2,
                0x32, 0x67, 0x8d, 0xfe, 0xca, 0x83, 0x64, 0x98,
                0xac, 0xfd, 0x3e, 0x37, 0x87, 0x46, 0x58, 0x24,
            };
#endif
            const u8   *address = table[i]->address;
            u32         size = table[i]->size;
            if (i == 0)
            {
                address += 0x4000;
                size -= 0x4000;
            }
#if !defined(SDK_OS_RESET_COMPACT)
            SVC_HMACSHA1Init(context, hmacKey, sizeof(hmacKey));
            SVC_HMACSHA1Update(context, address, size);
            SVC_HMACSHA1GetHash(context, digest);
            result &= (SVC_CompareSHA1(digest, compareDigests[i]) != FALSE);
#else
            result &= OSi_VerifyStaticSegmentHash(address, size, compareDigests[i]);
#endif
        }
    }
    // if any segment is invalid, enter the endless-loop
    while (result == FALSE)
    {
    }
}
#include <twl/itcm_end.h>
#endif // defined(SDK_TWL) && defined(SDK_ARM9)

/*---------------------------------------------------------------------------*
  Name:         OSi_DoResetSystem

  Description:  Start reset sequence.
                Subroutine of OS_ResetSystem().
                This part is put in itcm/wram because rom data will be
                loaded newly.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//---------------- ARM9
#ifdef SDK_ARM9
#ifdef  SDK_TWL
#include    <twl/itcm_begin.h>
#else
#include    <nitro/itcm_begin.h>
#endif
SDK_WEAK_SYMBOL
void OSi_DoResetSystem(void)
__attribute__((never_inline))
{
    //---- wait to receive reset command from ARM7
    while (!OS_IsResetOccurred())
    {
    }

    //---- set IME = 0
    reg_OS_IME = 0;

    //---- reload ROM data
#ifdef  SDK_TWL
    if (OS_IsRunOnTwl() == TRUE)
    {
        OSi_ReloadRomData(TRUE);
        OSi_VerifyStaticSegments();
    }
    else
#endif
    {
        OSi_ReloadRomData(FALSE);
    }

    //---- do boot
    OSi_DoBoot();
}

#ifdef  SDK_TWL
#include    <twl/itcm_end.h>
#else
#include    <nitro/itcm_end.h>
#endif

//---------------- ARM7
#else  // ifdef SDK_ARM9
#ifdef  SDK_TWL
#include    <twl/wram_begin.h>
#else
#include    <nitro/wram_begin.h>
#endif
SDK_WEAK_SYMBOL
void OSi_DoResetSystem(void)
__attribute__((never_inline))
{
    //---- set IME = 0
    reg_OS_IME = 0;

#ifdef  SDK_TWL
    if (OS_IsRunOnTwl() == TRUE)
    {
        OSi_ReloadTwlRomData();
    }
#endif

    //---- do boot
    OSi_DoBoot();
}

#ifdef  SDK_TWL
#include    <twl/wram_end.h>
#else
#include    <nitro/wram_end.h>
#endif
#endif // ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         OSi_DoBoot

  Description:  do boot

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
//---------------- ARM9
#ifdef SDK_ARM9
#ifdef  SDK_TWL
#include    <twl/itcm_begin.h>
#else
#include    <nitro/itcm_begin.h>
#endif
asm void OSi_DoBoot( void )
{
    //---- set IME = 0
    mov         r12, #HW_REG_BASE
    str         r12, [r12, #REG_IME_OFFSET]

    //---- clear interrupt vector just to make sure
    ldr         r1, =OSi_HW_DTCM
    add         r1, r1, #0x3fc0
    add         r1, r1, #HW_DTCM_SYSRV_OFS_INTR_VECTOR
    mov         r0, #0
    str         r0, [r1, #0]
    
    //---- synchronousness with ARM7
    ldr         r1, =REG_SUBPINTF_ADDR
@1:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001
    bne         @1                              // wait till subp status == 1
    mov         r0, #0x0100
    strh        r0, [r1]                        // mainp status == 1

    //---- clear some memory
    mov         r0, #0
    ldr         r3, =HW_EXCP_VECTOR_MAIN        // save exception vector
    ldr         r4, [r3]
    
    ldr         r1, =HW_BIOS_EXCP_STACK_MAIN    // clear system area. (exception, arena, DMA clear buf...)
    mov         r2, #0x80
    bl          OSi_CpuClear32
    
    str         r4, [r3]                        // restore exception vector
    
    ldr         r1, =HW_PXI_SIGNAL_PARAM_ARM9   // clear system area. (wm, thread, lock, intr...)
    mov         r2, #0x18
    bl          OSi_CpuClear32
    
    ldr         r1, =HW_WM_RSSI_POOL
    strh        r0, [r1]
    
    ldr         r1, =HW_COMPONENT_PARAM         // clear system area. (wm, thread, lock, intr...)
    mov         r2, #0x64
    bl          OSi_CpuClear32
    
    //---- syncronousness with ARM7
    ldr         r1, =REG_SUBPINTF_ADDR
@2:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001                     // wait till subp status == 0
    beq         @2
    mov         r0, #0
    strh        r0, [r1]                        // mainp status == 0
    
    //---- get boot address
    ldr         r3, =HW_ROM_HEADER_BUF          // r3 = ROM header
    ldr         r12, [r3, #0x24]                // lr = r12 = ARM9 entry address
    mov         lr, r12
    
    //---- clear registers r0-r11
    ldr         r11, =HW_PXI_SIGNAL_PARAM_ARM9  // clear r0-r11
    ldmia       r11, {r0-r10}
    mov         r11, #0

    //---- boot game
    bx          r12                             // jump to r12
}
#ifdef  SDK_TWL
#include <twl/itcm_end.h>
#else
#include <nitro/itcm_end.h>
#endif

//---------------- ARM7
#else  //ifdef SDK_ARM9
#ifdef  SDK_TWL
#include <twl/wram_begin.h>
#else
#include <nitro/wram_begin.h>
#endif
asm void OSi_DoBoot( void )
{
    //---- set IME = 0
    mov         r12, #HW_REG_BASE
    str         r12, [r12, #REG_IME_OFFSET]

    //---- clear interrupt vector just to make sure
    ldr         r1, =HW_INTR_VECTOR_BUF
    mov         r0, #0
    str         r0, [r1, #0]
    
    //---- syncronousness with ARM9
    ldr         r1, =REG_MAINPINTF_ADDR
    mov         r0, #0x0100
    strh        r0, [r1]                        // subp status = 1
@1:
    ldrh        r0, [r1]
    and         r0, r0, #0x000f
    cmp         r0, #0x0001                     // wait till mainp status == 1
    bne         @1

    //---- syncronousness with ARM9
    ldr         r1, =REG_MAINPINTF_ADDR
    mov         r0, #0                          // subp status = 0
    strh        r0, [r1]
@2:
    ldrh        r0, [r1]
    cmp         r0, #0x0001                     // wait till mainp status == 0
    beq         @2

    //---- get boot address
    ldr         r3, =HW_ROM_HEADER_BUF          // r3 = ROM header
    ldr         r12, [r3, #0x34]                // lr = r12 = ARM7 entry address
    mov         lr, r12
    
    //---- clear registers r0-r3
    mov         r0, #0
    mov         r1, #0
    mov         r2, #0
    mov         r3, #0
    
    //---- boot game
    bx          r12
}
#ifdef  SDK_TWL
#include <twl/wram_end.h>
#else
#include <nitro/wram_end.h>
#endif
#endif //ifdef SDK_ARM9


#ifdef SDK_ARM9
#ifdef  SDK_TWL
#include    <twl/itcm_begin.h>
#else
#include    <nitro/itcm_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         OSi_CpuClear32

  Description:  fill memory with specified data.
                32bit version

  Arguments:    data  : fill data
                destp : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
static asm void  OSi_CpuClear32( register u32 data, register void *destp, register u32 size )
{
    add     r12, r1, r2             // r12: destEndp = destp + size
@1:
    cmp     r1, r12                 // while (destp < destEndp)
    stmltia r1!, {r0}               // *((vu32 *)(destp++)) = data
    blt     @1
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ReloadRomData

  Description:  reload ROM data from card

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void OSi_ReloadRomData(BOOL ontwl)
{
    stmfd   sp!, {r4, lr}
    mov     r4, r0

#ifdef SDK_TWL
@shakeHand01:
    cmp     r4, #0
    beq     @shakeHand01_end
    /* ARM7 �Ɠ��� */
    ldr     r0, =HW_BOOT_SYNC_PHASE
    mov     r1, #1
    strh    r1, [r0]
    ldr     r0, =HW_BOOT_SHAKEHAND_9
    ldr     r1, =HW_BOOT_SHAKEHAND_7
    ldrh    r2, [r1]
    ldrh    r3, [r0]
@shakeHand01_loop:
    add     r3, r3, #1
    strh    r3, [r0]
    ldrh    r12, [r1]
    cmp     r2, r12
    beq     @shakeHand01_loop
    add     r3, r3, #1
    strh    r3, [r0]
@shakeHand01_end:
#endif // SDK_TWL

    // �K�v�Ȃ�ROM�w�b�_���ă��[�h�B (NTR���[�h���쎞�̂�)
    cmp     r4, #0
    movne   r0, #0
    ldreq   r0, =HW_ROM_BASE_OFFSET_BUF
    ldreq   r0, [r0]
    stmfd   sp!, {r0}
    cmp     r0, #0x8000
    ldrcs   r1, =HW_ROM_HEADER_BUF
    movcs   r2, #0x160
    blcs    OSi_ReadCardRom32

@loadStatic:
    /* ARM9 �p�o�C�i�������[�h */
    ldr     r12, =HW_ROM_HEADER_BUF
    ldr     r0, [r12, #0x20]
    ldr     r1, [r12, #0x28]
    ldr     r2, [r12, #0x2c]
    ldr     r3, [sp]
    add     r0, r0, r3
    subs    r3, r0, #0x8000
    movlt   r0, #0x8000
    sublt   r1, r1, r3
    addlt   r2, r2, r3
    cmp     r2, #0
    blgt    OSi_ReadCardRom32
    /* ARM7 �p�o�C�i�������[�h */
    ldr     r12, =HW_ROM_HEADER_BUF
    ldr     r0, [r12, #0x30]
    ldr     r1, [r12, #0x38]
    ldr     r2, [r12, #0x3c]
    ldr     r3, [sp]
    add     r0, r0, r3
    cmp     r2, #0
    blgt    OSi_ReadCardRom32
    ldmfd   sp!, {r0}

#ifdef SDK_TWL
@loadLtdStatic:
    cmp     r4, #0
    beq     @loadLtdStatic_end
    /* ARM9 �p�g���o�C�i�������[�h */
    ldr     r12, =HW_TWL_ROM_HEADER_BUF
    ldr     r0, [r12, #0x1c0]
    ldr     r1, [r12, #0x1c8]
    ldr     r2, [r12, #0x1cc]
    add     r0, r0, #0x4000
    add     r1, r1, #0x4000
    sub     r2, r2, #0x4000
    subs    r3, r0, #0x8000
    movlt   r0, #0x8000
    sublt   r1, r1, r3
    addlt   r2, r2, r3
    cmp     r2, #0
    blgt    OSi_ReadCardRom32
    /* ARM7 �p�g���o�C�i�������[�h */
    ldr     r12, =HW_TWL_ROM_HEADER_BUF
    ldr     r0, [r12, #0x1d0]
    ldr     r1, [r12, #0x1d8]
    ldr     r2, [r12, #0x1dc]
    add     r0, r0, #0x4000
    add     r1, r1, #0x4000
    sub     r2, r2, #0x4000
    subs    r3, r0, #0x8000
    movlt   r0, #0x8000
    sublt   r1, r1, r3
    addlt   r2, r2, r3
    cmp     r2, #0
    blgt    OSi_ReadCardRom32
@loadLtdStatic_end:
#endif // SDK_TWL

    // �L���b�V�������ׂăt���b�V���B
@arrangeCache:
    /* DC_StoreAll() */
    mov     r1, #0
@arrangeCache_loop01:
    mov     r0, #0
@arrangeCache_loop02:
    orr     r2, r1, r0
    mcr     p15, 0, r2, c7, c10, 2
    add     r0, r0, #HW_CACHE_LINE_SIZE
    cmp     r0, #(HW_DCACHE_SIZE / 4)
    blt     @arrangeCache_loop02
    adds    r1, r1, #(1 << HW_C7_CACHE_SET_NO_SHIFT)
    bne     @arrangeCache_loop01
    /* DC_InvalidateAll() */
    mov     r0, #0
    mcr     p15, 0, r0, c7, c6, 0
    /* IC_InvalidateAll() */
//  mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0
    /* DC_WaitWriteBufferEmpty() */
//  mov     r0, #0
    mcr     p15, 0, r0, c7, c10, 4

#ifdef SDK_TWL
    cmp     r4, #0
    beq     @shakeHand02_end
    /* ARM7 �Ɠ��� */
    /* �ޔ������g���o�C�i���̐擪�����̕��A���K�v */
    mov     r3, #2
    ldr     r0, =HW_BOOT_SYNC_PHASE
    strh    r3, [r0]
    ldr     r0, =HW_BOOT_SHAKEHAND_9
    ldr     r1, =HW_BOOT_SHAKEHAND_7
    ldrh    r2, [r1]
    ldrh    r3, [r0]
@shakeHand02_loop:
    add     r3, r3, #1
    strh    r3, [r0]
    ldrh    r12, [r1]
    cmp     r2, r12
    beq     @shakeHand02_loop
    add     r3, r3, #1
    strh    r3, [r0]
@shakeHand02_end:
#endif // SDK_TWL

    ldmfd   sp!, {r4, pc}
//  bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ReadCardRom32

  Description:  read card data by CPU
                (subroutine of OSi_ReloadRomData)

  Arguments:    src  : source address, need to be 4 byte aligned
                dest : destination address, need to be 4 byte aligned
                len  : size of forwarding

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_ReadCardRom32(u32 src, void *dst, int len)
{
    //---- I/O registers for CARD
    vu8    *const reg_CARD_MASTERCNT = (vu8 *)(HW_REG_BASE + 0x1a1);
    vu8    *const reg_CARD_CMD = (vu8 *)(HW_REG_BASE + 0x1a8);
    vu32   *const reg_CARD_CNT = (vu32 *)(HW_REG_BASE + 0x1a4);
    vu32   *const reg_CARD_DATA = (vu32 *)(HW_REG_BASE + 0x100010);
    vu32   *const hdr_GAME_BUF = (vu32 *)(HW_ROM_HEADER_BUF + 0x60);


    enum
    {
        CARD_MASTER_SELECT_ROM = 0x00,
        CARD_MASTER_ENABLE = 0x80,
        CARD_CMD_READ_PAGE = 0xb7,
        CARD_CTRL_CMD_MASK = 0x07000000,
        CARD_CTRL_CMD_PAGE = 0x01000000,
        CARD_CTRL_READ = 0x00000000,
        CARD_CTRL_RESET_HI = 0x20000000,
        CARD_CTRL_START = 0x80000000,
        CARD_CTRL_READY = 0x00800000,
        CARD_ENUM_END
    };

    const u32 ctrl_start = (u32)((*hdr_GAME_BUF & ~CARD_CTRL_CMD_MASK)
                                 | (CARD_CTRL_CMD_PAGE | CARD_CTRL_READ
                                    | CARD_CTRL_START | CARD_CTRL_RESET_HI));

    //---- position of page. minus value mean before range.
    int     pos = -(int)(src & (512 - 1));

    //---- wait finishing just to make sure, and turn master enable on
    while ((*reg_CARD_CNT & CARD_CTRL_START) != 0)
    {
    }
    *reg_CARD_MASTERCNT = (u32)(CARD_MASTER_SELECT_ROM | CARD_MASTER_ENABLE);

    for (src = (u32)(src + pos); pos < len; src += 512)
    {
        //---- setting for command of one page reading
        reg_CARD_CMD[0] = (u8)(CARD_CMD_READ_PAGE);
        reg_CARD_CMD[1] = (u8)(src >> 24);
        reg_CARD_CMD[2] = (u8)(src >> 16);
        reg_CARD_CMD[3] = (u8)(src >> 8);
        reg_CARD_CMD[4] = (u8)(src >> 0);
        reg_CARD_CMD[5] = (u8)(0);
        reg_CARD_CMD[6] = (u8)(0);
        reg_CARD_CMD[7] = (u8)(0);

        //---- start forwarding by CPU
        *reg_CARD_CNT = ctrl_start;
        for (;;)
        {
            u32     ctrl = *reg_CARD_CNT;

            //---- check if ok to read word data
            if ((ctrl & CARD_CTRL_READY) != 0)
            {
                //---- read data
                const u32 data = *reg_CARD_DATA;

                //---- store data
                if ((pos >= 0) && (pos < len))
                {
                    *(u32 *)((u32)dst + pos) = data;
                }

                pos += sizeof(u32);
            }

            //---- notice: need to read whole one page data
            if (!(ctrl & CARD_CTRL_START))
            {
                break;
            }
        }
    }
}

#ifdef  SDK_TWL
#include <twl/itcm_end.h>
#else
#include <nitro/itcm_end.h>
#endif
#endif //ifdef SDK_ARM9

#ifdef  SDK_TWL
/*---------------------------------------------------------------------------*
  Name:         OSi_ReloadTwlRomData

  Description:  

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9
//---------------- ARM9
#else   // ifdef SDK_ARM9
//---------------- ARM7
#include <twl/ltdwram_begin.h>
static asm void
OSi_ReloadTwlRomData(void)
{
@shakeHand01:
    /* ARM9 �Ɠ��� */
    ldr     r0, =HW_BOOT_SHAKEHAND_9
    ldr     r1, =HW_BOOT_SHAKEHAND_7
    ldrh    r2, [r0]
    ldrh    r3, [r1]
@shakeHand01_loop:
    add     r3, r3, #1
    strh    r3, [r1]
    ldrh    r12, [r0]
    cmp     r2, r12
    beq     @shakeHand01_loop
    add     r3, r3, #1
    strh    r3, [r1]

    ldr     r0, =HW_BOOT_SYNC_PHASE
@shakeHand01_waitLoop:
    ldrh    r1, [r0]
    cmp     r1, #3
    beq     @shakeHand02
    cmp     r1, #2
    bne     @shakeHand01_waitLoop

@restoreShelteredBinary:
    ldr     r12, =HW_TWL_ROM_HEADER_BUF
    /* ARM7 �p�g���o�C�i���̑ޔ��f�[�^�𕜋A */
    ldr     r0, [r12, #0x1dc]
    cmp     r0, #0
    beq     @restoreNext
    ldr     r2, [r12, #0x1d8]   // r2 = ARM7 �p�g���풓���W���[�� RAM �A�h���X
    ldr     r0, =OSi_LtdMainParams
    ldr     r3, [r0]            // r3 = end of src address
    sub     r1, r3, #0x4000     // r1 = start of src address
@restoreLoop01:
    ldr     r0, [r1], #4
    str     r0, [r2], #4
    cmp     r1, r3
    blt     @restoreLoop01

@restoreNext:
    /* ARM9 �p�g���o�C�i���̑ޔ��f�[�^�𕜋A */
    ldr     r0, [r12, #0x1cc]
    cmp     r0, #0
    beq     @shakeHand02
    ldr     r2, [r12, #0x1c8]   // r2 = ARM9 �p�g���풓���W���[�� RAM �A�h���X
    ldr     r0, =OSi_LtdMainParams
    ldr     r0, [r0]
    sub     r3, r0, #0x4000     // r3 = end of src address
    sub     r1, r3, #0x4000     // r1 = start of src address
@restoreLoop02:
    ldr     r0, [r1], #4
    str     r0, [r2], #4
    cmp     r1, r3
    blt     @restoreLoop02

@shakeHand02:
    /* ARM9 �Ɠ��� */
    ldr     r0, =HW_BOOT_SHAKEHAND_9
    ldr     r1, =HW_BOOT_SHAKEHAND_7
    ldrh    r2, [r0]
    ldrh    r3, [r1]
@shakeHand02_loop:
    add     r3, r3, #1
    strh    r3, [r1]
    ldrh    r12, [r0]
    cmp     r2, r12
    beq     @shakeHand02_loop
    add     r3, r3, #1
    strh    r3, [r1]

    bx      lr
}
#include <twl/ltdwram_end.h>

#endif  // ifdef SDK_ARM9
#endif  // ifdef SDK_TWL
