/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_application_jump.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <twl/os.h>
#include <application_jump_private.h>
#include <twl/spi.h>
#include <twl/code32.h>
#include <twl/mcu.h>
#include <twl/hw/common/mmap_shared.h>
#include <twl/hw/common/mmap_parameter.h>

// define data-------------------------------------------------------
#define OSi_LAUNCHER_PARAM_MAGIC_CODE           "TLNC"
#define OSi_LAUNCHER_PARAM_MAGIC_CODE_LEN       4
#define OSi_TITLE_ID_MEDIA_FLAG_MASK            ( 0x0001ULL << 34 )
#define OSi_TITLE_ID_DATA_ONLY_FLAG_MASK        ( 0x0001ULL << 35 )
#define OSi_TMP_APP_SIZE_MAX                    ( 16 * 1024 * 1024 )

#define OSi_TMP_APP_PATH_RAW                    "nand:/tmp/jump.app"
#define OSi_TMP_APP_PATH_RAW_LENGTH             18

typedef struct NandFirmResetParameter {
    u8      isHotStart :1;
    u8      isResetSW :1;
    u8      rsv :5;
    u8      isValid :1;
}NandFirmResetParameter;

// NAND�t�@�[���p�����[�^�A�h���X
#define OSi_GetNandFirmResetParam()         ( (NandFirmResetParameter *)HW_NAND_FIRM_HOTSTART_FLAG )

// �����`���[�p�����[�^�A�h���X
#define OSi_GetLauncherParam()              ( (LauncherParam *)HW_PARAM_LAUNCH_PARAM )


// extern data-------------------------------------------------------

// function's prototype----------------------------------------------
static void OSi_DoHardwareReset( void );
static BOOL OSi_IsNandApp( OSTitleId id );
static BOOL OSi_IsSameMakerCode(OSTitleId id);
static OSTitleId OSi_SearchTitleIdFromList(OSTitleId id, u64 mask);

// global variables--------------------------------------------------

// static variables--------------------------------------------------
#include  <twl/ltdmain_begin.h>
static OSTitleId s_prevTitleId = 0;
#include  <twl/ltdmain_end.h>
// const data--------------------------------------------------------

#include <twl/ltdmain_begin.h>
static BOOL OSi_IsNandApp( OSTitleId id )
{
    if( id & OSi_TITLE_ID_MEDIA_FLAG_MASK )
    {
        return TRUE;
    }
    return FALSE;
}

static BOOL OSi_IsSameMakerCode(OSTitleId id)
{
    const OSTitleIDList *list = (const OSTitleIDList *)HW_OS_TITLE_ID_LIST;
    const int   total = MATH_MIN( list->num, OS_TITLEIDLIST_MAX );
    int i;
    for (i = 0; i < total; ++i)
    {
        if (list->TitleID[i] == id)
        {
            int pos = (i >> 3);
            int bit = (0x01 << (i & 7));
            if (( list->sameMakerFlag[pos] & bit) != 0)
            {
                /* ���[�J�[�R�[�h������ */
                return TRUE;
            }
            else
            {
                /* ���[�J�[�R�[�h���قȂ� */
                return FALSE;
            }
        }
    }
    /* �Ώۂ� TitleID �����X�g�ɑ��݂��Ȃ� */
    return FALSE;
}

static OSTitleId OSi_SearchTitleIdFromList( OSTitleId id, u64 mask )
{
    const OSTitleIDList *list = (const OSTitleIDList *)HW_OS_TITLE_ID_LIST;
    const int OS_TITLE_ID_LIST_MAX = sizeof(list->TitleID) / sizeof(*list->TitleID);
    const int num = MATH_MIN(list->num, OS_TITLE_ID_LIST_MAX);
    int i;

    /* �C�j�V�����R�[�h�����j�[�N�ł��邱�Ƃ�O��Ƀ��X�g����Y������ TitleID �𒊏o */
    for (i = 0; i < num; ++i)
    {
        if ( (id & mask) == (list->TitleID[i] & mask)                       /* �C�j�V�����R�[�h����v���邩? */
            && !(list->TitleID[i] & OSi_TITLE_ID_DATA_ONLY_FLAG_MASK) )     /* �f�[�^�I�����[�^�C�g���ł͂Ȃ���? */
        {
            return list->TitleID[i];
        }
    }
    return 0;
}

#include  <twl/ltdmain_end.h>

#ifdef SDK_ARM9

void OSi_InitPrevTitleId( void )
{
    OSDeliverArgInfo deliverArgInfo;
    OS_InitDeliverArgInfo( &deliverArgInfo, 0 );
    if ( OS_DELIVER_ARG_SUCCESS == OS_DecodeDeliverArg() )
    {
        s_prevTitleId = OS_GetTitleIdFromDeliverArg();
    }
    if ( (s_prevTitleId != 0 && !OSi_SearchTitleIdFromList(s_prevTitleId, 0xffffffffffffffff)) ||
          (char)(s_prevTitleId >>24) == '0' ||
          (char)(s_prevTitleId >>24) == '1' ||
          (char)(s_prevTitleId >>24) == '2' ||
          (char)(s_prevTitleId >>24) == '3' ||
          (char)(s_prevTitleId >>24) == '4'  )
    {
        /* �����`���[����N�������悤�Ɍ��������ADeliverArg ������ */
        s_prevTitleId = 0;
        OS_InitDeliverArgInfo( &deliverArgInfo, 0 );
        (void)OS_EncodeDeliverArg();
    }
    else
    {
        OS_SetDeliverArgStateInvalid();
    }
}

// ���S�̂��߁A���̊֐���itcm�ɔz�u
#include <twl/itcm_begin.h>
static void OSi_DoHardwareReset( void )
{
    // [TODO:]�ŏI�I�ɂ̓}�C�R���o�R��HW���Z�b�g�ɕύX����B
    // ���Z�b�g���ߔ��s
    PM_ForceToResetHardware();
    OS_Terminate();
}

#include <twl/itcm_end.h>

/* �ȉ��̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_begin.h>
#endif
// LauncherParam���w�肵�ăn�[�h�E�F�A���Z�b�g
void OS_SetLauncherParamAndResetHardware( OSTitleId id, LauncherBootFlags *flag )
{
    // ���[�J�[�R�[�h�ƃQ�[���R�[�h���R�s�[
    u32 *maker_code_dest_addr = (u32 *)((u32)(OSi_GetLauncherParam()) + HW_PARAM_DELIVER_ARG_MAKERCODE_OFS);
    u32 *game_code_dest_addr = (u32 *)((u32)(OSi_GetLauncherParam()) + HW_PARAM_DELIVER_ARG_GAMECODE_OFS);
    u16 *maker_code_src_addr = (u16 *)(HW_TWL_ROM_HEADER_BUF + 0x10);
    u32 *game_code_src_addr = (u32 *)(HW_TWL_ROM_HEADER_BUF + 0xc);
    *maker_code_dest_addr = (u32)*maker_code_src_addr;
    *game_code_dest_addr = *game_code_src_addr;

    // �����`���[�p�����[�^�̐ݒ�
    OSi_GetLauncherParam()->body.v1.prevTitleID = *(OSTitleId *)(HW_TWL_ROM_HEADER_BUF + 0x230);
    OSi_GetLauncherParam()->body.v1.bootTitleID = id;
    OSi_GetLauncherParam()->body.v1.flags = *flag;
    MI_CpuCopyFast( OSi_LAUNCHER_PARAM_MAGIC_CODE, (char *)&OSi_GetLauncherParam()->header.magicCode, OSi_LAUNCHER_PARAM_MAGIC_CODE_LEN);
    OSi_GetLauncherParam()->header.bodyLength = sizeof(LauncherParamBody);
    OSi_GetLauncherParam()->header.crc16 = SVC_GetCRC16( 65535, &OSi_GetLauncherParam()->body, OSi_GetLauncherParam()->header.bodyLength );
    OSi_GetLauncherParam()->header.version = 1;

    if( id == 0 || OS_IsDeliverArgEncoded() != TRUE || OS_GetTitleIdLastEncoded() != OSi_GetLauncherParam()->body.v1.prevTitleID )
    {
        // DeliverArg�����݂̃A�v���ŃG���R�[�h����Ă��Ȃ��ꍇ�ADeliverArg�̋����������ƃG���R�[�h
        OSDeliverArgInfo deliverArgInfo;
        OS_InitDeliverArgInfo( &deliverArgInfo, 0 );
        (void)OS_EncodeDeliverArg();
    }

    DC_FlushRange((const void *)HW_PARAM_DELIVER_ARG, HW_PARAM_DELIVER_ARG_SIZE + HW_PARAM_LAUNCH_PARAM_SIZE );
    DC_WaitWriteBufferEmpty();

    OSi_DoHardwareReset();
}

// �A�v���W�����v��̃A�v�����猳�̃A�v���ɖ߂�֐�
// ���̃A�v���������ꍇ�́AFALSE
BOOL OS_ReturnToPrevApplication( void )
{
    if( s_prevTitleId != 0)
    {
        return OS_DoApplicationJump( s_prevTitleId, OS_APP_JUMP_NORMAL ); //never return
    }
    return FALSE;
}

static inline BOOL IsSameTitleID( OSTitleId titleID, OSTitleId listedTitleID )
{
    if ((char)titleID == 'A')
    {
        // ����R�[�hA�I�v�V�����Ή�
        return (listedTitleID >> 8) == (titleID >> 8);
    }
    else
    {
        return listedTitleID == titleID;
    }
}

// �^����titleID�ɑ΂��Ď��ۂɋN���\��ID���擾����
// �N���\��ID�����݂���΁A*bootableTitleID�ɂ���ID��ݒ肵�ATRUE��Ԃ�
// �N���\��ID�����݂��Ȃ�������AFALSE��Ԃ�
static BOOL OS_GetBootableTitleID( OSTitleId titleID, OSTitleId *bootableTitleID )
{
    const OSTitleIDList *list = (const OSTitleIDList *)HW_OS_TITLE_ID_LIST;
    const int OS_TITLE_ID_LIST_MAX = sizeof(list->TitleID) / sizeof(*list->TitleID);
    const int num = MATH_MIN(list->num, OS_TITLE_ID_LIST_MAX);
    int i;
    for (i = 0; i < num; ++i)
    {
        if (IsSameTitleID(titleID, list->TitleID[i]) && (list->appJumpFlag[i / 8] & (u8)(0x1 << (i % 8))))
        {
            *bootableTitleID = list->TitleID[i];
            return TRUE;
        }
    }
    return FALSE;
}

// TitleID�ɑΉ�����A�v���P�[�V�������C���X�g�[������Ă��邩�ǂ���
BOOL OSi_CanApplicationJumpTo( OSTitleId titleID )
{
    return OS_GetBootableTitleID(titleID, &titleID);
}

// OS_SetLauncherParamAndResetHardware�̃��b�p�[
BOOL OS_DoApplicationJump( OSTitleId id, OSAppJumpType jumpType )
{
    FSFile  file[1];
    LauncherBootFlags flag;
    u8 platform_code;
    u8 bit_field;
    
    switch( jumpType )
    {
        case OS_APP_JUMP_NORMAL:
            if( id & OSi_TITLE_ID_DATA_ONLY_FLAG_MASK )
            {
                // DataOnly�Ȃ��̂ɂ̓W�����v�ł��Ȃ��悤�ɂ��Ă���
                return FALSE;
            }
            if( id != 0 )
            {
                if (!OS_GetBootableTitleID(id, &id))
                {
                    return FALSE;
                }
            }
            // bootType��ݒ肵�A�W�����v
            if( id & OSi_TITLE_ID_MEDIA_FLAG_MASK )
            {
                flag.bootType = LAUNCHER_BOOTTYPE_NAND;
            }else
            {
                flag.bootType = LAUNCHER_BOOTTYPE_ROM;
            }
            break;
        case OS_APP_JUMP_TMP:
            // �������g��TMP�A�v���ł���ꍇ��TMP�W�����v�ł��Ȃ�
            if( OS_IsTemporaryApplication() )
            {
                OS_TPrintf("OS_DoApplicationJump error : tmp app can't jump to tmp app!\n");
                return FALSE;
            }
            // OS_TMP_APP_PATH�̃t�@�C������TitleID�̎擾��TMP�W�����v���r�b�g���`�F�b�N
            FS_InitFile( file );
            if( !FS_OpenFileEx(file, OS_TMP_APP_PATH, FS_FILEMODE_R) )
            {
                OS_TPrintf("OS_DoApplicationJump error : tmp app open error!\n");
                return FALSE;
            }
            // �t�@�C���T�C�Y�`�F�b�N
            if( FS_GetFileLength( file ) > OSi_TMP_APP_SIZE_MAX )
            {
                OS_TPrintf("OS_DoApplicationJump error : too large tmp app size!\n");
                return FALSE;
            }
            
            if( !FS_SeekFile( file, 0x12, FS_SEEK_SET ) ||
                ( sizeof(platform_code) != FS_ReadFile( file, &platform_code, sizeof(platform_code) ) ) ||
                !FS_SeekFile( file, 0x1d, FS_SEEK_SET ) ||
                ( sizeof(bit_field) != FS_ReadFile( file, &bit_field, sizeof(bit_field) ) )
            )
            {
                OS_TPrintf("OS_DoApplicationJump error : tmp app read error!\n");
                (void)FS_CloseFile(file);
                return FALSE;
            }
            
            if( !( bit_field & 0x2 ) )
            {
                OS_TPrintf("OS_DoApplicationJump error : tmp jump bit is not enabled!\n");
                (void)FS_CloseFile(file);
                return FALSE;
            }
            flag.bootType = LAUNCHER_BOOTTYPE_TEMP;
            // TWL�A�v���̓^�C�g��ID���w�b�_����擾���ē���Ă����BNTR�A�v���̏ꍇ�ATitleID�͂Ƃ肠����0�ȊO�����Ă���
            if( platform_code & 0x2 )
            {
                if( !FS_SeekFile( file, 0x0230, FS_SEEK_SET ) ||
                    ( sizeof(id) != FS_ReadFile( file, &id, sizeof(id) ) )
                )
                {
                    OS_TPrintf("OS_DoApplicationJump error : tmp app read error!\n");
                    (void)FS_CloseFile(file);
                    return FALSE;
                }
            }else
            {
                id = 0x1;
            }
            (void)FS_CloseFile(file);
            break;
        default:
            return FALSE;
    }

    if ( PMi_TryLockForReset() == FALSE )
    {
        return FALSE;
    }
    
    // ���ʂ̐ݒ�
    flag.isValid = TRUE;
    flag.isLogoSkip = TRUE;
    flag.isInitialShortcutSkip = FALSE;
    flag.isAppLoadCompleted = FALSE;
    flag.isAppRelocate = FALSE;
    flag.rsv = 0;
    
    OS_SetLauncherParamAndResetHardware( id, &flag ); // never return.
    return TRUE;
}

/* �ȏ�̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToSystemMenu

  Description:  �n�[�h�E�F�A���Z�b�g���s���A�V�X�e�����j���[�ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToSystemMenu( void )
{
    BOOL result = FALSE;
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        // ---- Application Jump with id==0
        //      means to jump to system menu.
        result = OS_DoApplicationJump( 0, OS_APP_JUMP_NORMAL );
    }
    else
#endif
    {
        OS_Warning("This Hardware don't support this funciton");
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         OS_RebootSystem

  Description:  �n�[�h�E�F�A���Z�b�g���s���A�������g���ċN�����܂��B
  
  Arguments:    None
 
  Returns:      FALSE �c NITRO ��Ŏ��s�A�������͍ċN�������Ɏ��s����
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_RebootSystem( void )
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        if( OS_IsTemporaryApplication() )
        {
            OS_TPrintf("OS_RebootSystem error : tmp app can't execute this function\n");
            return FALSE;
        }
        // �������g�փA�v���P�[�V�����W�����v����
        return OS_DoApplicationJump( OS_GetTitleId(), OS_APP_JUMP_NORMAL );
    }
    else
#endif
    {
        OS_Warning("This Hardware don't support this funciton");
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsRebooted

  Description:  OS_RebootSystem �ɂ��ċN�����s��ꂽ�����`�F�b�N���܂�
  
  Arguments:    None
 
  Returns:      TRUE  �c 1��ȏ�ċN�����s��ꂽ
                FALSE �c ����N��
 *---------------------------------------------------------------------------*/
BOOL OS_IsRebooted( void )
{
    BOOL result = FALSE;
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        if( OS_GetTitleId() == s_prevTitleId )
        {
            result = TRUE;
        }
    }
#endif
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_CanArbitraryJumpTo

  Description:  �w�肵���C�ӂ̃A�v���P�[�V�����ɃW�����v�����`�F�b�N���܂��B
  
  Arguments:    �W�����v�������A�v���P�[�V�����̃Q�[���R�[�h(�C�j�V�����R�[�h)
  
  Returns:      TRUE  �c �Ώۂ̃A�v���P�[�V�����ɃW�����v�\�B
                FALSE �c �Ώۂ̃A�v���P�[�V�����ɃW�����v���o���Ȃ��B
 *---------------------------------------------------------------------------*/
 
BOOL OSi_CanArbitraryJumpTo(u32 initialCode)
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        OSTitleId id = OSi_SearchTitleIdFromList( (OSTitleId)initialCode, 0x00000000ffffffff );
        /* NAND �A�v���ȊO�͏��O */
        if( OSi_IsNandApp(id) )
        {
            return OSi_CanApplicationJumpTo( id ) && OSi_IsSameMakerCode( id );
        }
    }
#endif
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToArbitraryApplication

  Description:  �w�肵���C�ӂ̃A�v���P�[�V�����ɃW�����v����B
  
  Arguments:    �W�����v�������A�v���P�[�V�����̃Q�[���R�[�h(�C�j�V�����R�[�h)
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToArbitraryApplication(u32 initialCode)
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        OSTitleId id = OSi_SearchTitleIdFromList( (OSTitleId)initialCode, 0x00000000ffffffff );
        if( OSi_CanArbitraryJumpTo( initialCode ) )
        {
            return OS_DoApplicationJump( id, OS_APP_JUMP_NORMAL );
        }
    }
#endif
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetPrevTitleId

  Description:  OS_JumpToArbitraryApplication �֐����g���ăW�����v���Ă���
                �A�v���P�[�V������ Title ID ���擾����B
  
  Arguments:    �Ȃ�
 
  Returns:      OS_JumpToArbitraryApplication �֐����g���ăW�����v���Ă���
                �A�v���P�[�V������ Title ID
                �V�X�e�����j���[����N�������ꍇ ����уf�o�b�K�ŋN�������ꍇ�A
                NITRO ��Ŏ��s�����ꍇ�ɂ� 0 ��Ԃ�
 *---------------------------------------------------------------------------*/
OSTitleId OSi_GetPrevTitleId(void)
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        const OSTitleIDList *list = (const OSTitleIDList *)HW_OS_TITLE_ID_LIST;
        const int   total = MATH_MIN( list->num, OS_TITLEIDLIST_MAX );
        int i;
        for (i = 0; i < total; ++i)
        {
            if (list->TitleID[i] == s_prevTitleId )
            {
                return s_prevTitleId;
            }
        }
    }
#endif
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsBootFromSystemMenu

  Description:  �V�X�e�����j���[����N�����ꂽ���𔻒�B
  
  Arguments:    �Ȃ�
 
  Returns:      TRUE �V�X�e�����j���[����N��
                FALSE ����ȊO�̃A�v���P�[�V��������N��
 *---------------------------------------------------------------------------*/
BOOL OS_IsBootFromSystemMenu( void )
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        if( OSi_GetPrevTitleId() == 0 )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
#endif
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IsJumpFromSameMaker

  Description:  �������[�J�[�R�[�h�̃A�v���P�[�V��������N�����ꂽ���𔻒�B
  
  Arguments:    �Ȃ�
 
  Returns:      TRUE �������[�J�[�R�[�h�̃A�v���P�[�V��������N��
                FALSE ����ȊO�̃A�v���P�[�V�����A�������� �V�X�e�����j���[����N��
 *---------------------------------------------------------------------------*/
BOOL OSi_IsJumpFromSameMaker( void )
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        if( OS_IsBootFromSystemMenu() )
        {
            /* System Menu ����N�������ꍇ�͂��炩���ߏ��O */
            return FALSE;
        }
    return OSi_IsSameMakerCode( s_prevTitleId );
    }
#endif
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetInitialCode

  Description:  ���s���̃A�v���P�[�V�����̃Q�[���R�[�h(�C�j�V�����R�[�h)���擾����B
  
  Arguments:    None.
 
  Returns:      �Q�[���R�[�h(�C�j�V�����R�[�h)
                NITRO ��Ŏ��s�����ꍇ�ɂ͎擾�ł��Ȃ����߁A0 ��Ԃ�
 *---------------------------------------------------------------------------*/
u32 OSi_GetInitialCode(void)
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        return (u32)( OS_GetTitleId() );
    }
#endif
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetPrevInitialCode

  Description:  OS_JumpToArbitraryApplication �֐����g���ăW�����v���Ă���
                �A�v���P�[�V�����̃Q�[���R�[�h(�C�j�V�����R�[�h)���擾�B
  
  Arguments:    None.
 
  Returns:      �Q�[���R�[�h(�C�j�V�����R�[�h)
                �V�X�e�����j���[����N�������ꍇ ����уf�o�b�K�ŋN�������ꍇ�A
                NITRO ��Ŏ��s�����ꍇ�ɂ͎擾�ł��Ȃ����߁A0 ��Ԃ�
 *---------------------------------------------------------------------------*/
u32 OSi_GetPrevInitialCode(void)
{
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        return (u32)( OSi_GetPrevTitleId() );
    }
#endif
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsTemporaryApplication

  Description:  �������g�� TMP �A�v���ł��邩���`�F�b�N���܂��B
  
  Arguments:    None
 
  Returns:      TRUE  �c TMP �A�v��
                FALSE �c TMP �A�v���ȊO
 *---------------------------------------------------------------------------*/
BOOL OS_IsTemporaryApplication(void)
{
#ifdef SDK_TWL
    static BOOL isChecked = FALSE, isTmpApp = FALSE;
    if( OS_IsRunOnTwl() )
    {
        if( isChecked == FALSE ) /* ���Ƀ`�F�b�N�ς݂Ȃ�O��̌��ʂ𗬗p */
        {
            if( 0 == STD_CompareNString( OS_GetBootSRLPath(), OSi_TMP_APP_PATH_RAW, OSi_TMP_APP_PATH_RAW_LENGTH ) )
            {
                isTmpApp = TRUE;
            }
            else
            {
                isTmpApp = FALSE;
            }
            isChecked = TRUE;
        }
        return isTmpApp;
    }
#endif
    return FALSE;
}

#else // SDK_ARM9
    //----------------------------------------------------------------
    // for ARM7

static BOOL OSi_IsValidLauncherParam( void )
{
    return ( STD_StrNCmp( (const char *)&OSi_GetLauncherParam()->header.magicCode,
                             OSi_LAUNCHER_PARAM_MAGIC_CODE,
                             OSi_LAUNCHER_PARAM_MAGIC_CODE_LEN ) == 0 ) &&
    ( OSi_GetLauncherParam()->header.bodyLength > 0 ) &&
    ( OSi_GetLauncherParam()->header.crc16 == SVC_GetCRC16( 65535, &OSi_GetLauncherParam()->body, OSi_GetLauncherParam()->header.bodyLength ) );
}

static BOOL OSi_IsEnableJTAG( void )
{
    // SCFG���W�X�^�������ɂȂ��Ă�����ASCFG���W�X�^�̒l��"0"�ɂȂ�̂ŁAWRAM�ɑޔ����Ă���l���`�F�b�N����B
    u8 value = (u8)(( reg_SCFG_EXT & REG_SCFG_EXT_CFG_MASK ) ?
                    ( reg_SCFG_JTAG & REG_SCFG_JTAG_CPUJE_MASK ) :
                    ( *(u8 *)HWi_WSYS09_ADDR & HWi_WSYS09_JTAG_CPUJE_MASK ));
    return value ? TRUE : FALSE;
}

// �����`���[�p�����[�^�̃��[�h�����Hot/Cold�u�[�g����
// �����`���[�p�����[�^���L���Ȃ��TRUE�A�����ł����FALSE��Ԃ�
// LauncherParam�ɂ̓����`���[�p�����[�^���L���ł���΁A���̃����`���[�p�����[�^���i�[�����
// isHotstart�ɂ�Hot�u�[�g�Ȃ��TRUE���ACold�u�[�g�Ȃ��False���i�[�����
BOOL OS_ReadLauncherParameter( LauncherParam *buf, BOOL *isHotstart )
{
    if( !OSi_GetNandFirmResetParam()->isValid ) {
        *(u8 *)OSi_GetNandFirmResetParam() = (u8)MCU_GetFreeRegister( OS_MCU_RESET_VALUE_OFS );
        OSi_GetNandFirmResetParam()->isValid = 1;
    }
    
    // Hot/Cold�u�[�g����
    // "�}�C�R���t���[���W�X�^��HOTBT�t���O=0"�Ȃ�Cold�u�[�g
    if( !OSi_GetNandFirmResetParam()->isHotStart ) {
        *isHotstart = FALSE;
    }else {
        *isHotstart = TRUE;
        // �����`���[�p�����[�^�L������
        if(  OSi_IsValidLauncherParam() &&
            !OSi_GetNandFirmResetParam()->isResetSW
        ) {
            // �����`���[�p�����[�^���L���Ȃ�Abuf�ɃR�s�[
            MI_CpuCopy32 ( OSi_GetLauncherParam(), buf, sizeof(LauncherParam) );
            return TRUE;
        }else {
            // �����Ȃ��΁A�]����o�b�t�@���N���A
            MI_CpuClear32 ( buf, sizeof(LauncherParam) );
        }
    }
    return FALSE;
}

#endif // SDK_ARM9
