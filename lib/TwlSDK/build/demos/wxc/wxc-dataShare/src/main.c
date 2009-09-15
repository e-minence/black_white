/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - wxc-dataShare
  File:     main.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-15#$
  $Rev: 2414 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include <nitro/wxc.h>

#include "user.h"
#include "common.h"
#include "disp.h"
#include "gmain.h"
#include "wh.h"

void VBlankIntr(void);          // V�u�����N�����݃n���h��

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // �L�[���s�[�g�J�n�܂ł̃t���[����
#define     KEY_REPEAT_SPAN     10     // �L�[���s�[�g�̊Ԋu�t���[����

#define     PICTURE_FRAME_PER_GAME_FRAME    2


/* �e�X�g�p�� GGID */
#define SDK_MAKEGGID_SYSTEM(num)              (0x003FFF00 | (num))
#define GGID_ORG_1                            SDK_MAKEGGID_SYSTEM(0x52)

/* ����Sharing�Ŏg�p���� GGID */
#define WH_GGID                 SDK_MAKEGGID_SYSTEM(0x21)

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �L�[���͏��
typedef struct KeyInfo
{
    u16     cnt;                       // �����H���͒l
    u16     trg;                       // �����g���K����
    u16     up;                        // �����g���K����
    u16     rep;                       // �����ێ����s�[�g����

}
KeyInfo;


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void ModeSelect(void);          // �e�@/�q�@ �I�����
static void ModeError(void);           // �G���[�\�����


// �ėp�T�u���[�`��
static void KeyRead(KeyInfo * pKey);
static void InitializeAllocateSystem(void);

static void GetMacAddress(u8 * macAddr, u16 * gScreen);


static void ModeConnect();
static void ModeWorking(void);

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static KeyInfo gKey;                   // �L�[����
static vs32 gPictureFrame;             // �s�N�`���[�t���[�� �J�E���^


/*---------------------------------------------------------------------------*
    �O���ϐ���`
 *---------------------------------------------------------------------------*/
int passby_endflg;
WMBssDesc bssdesc;
WMParentParam parentparam;
u8 macAddress[6];


static void WaitPressButton(void);
static void GetChannelMain(void);
static BOOL ConnectMain(u16 tgid);
static void PrintChildState(void);
static BOOL JudgeConnectableChild(WMStartParentCallback *cb);


//============================================================================
//   �ϐ���`
//============================================================================

static s32 gFrame;                     // �t���[���J�E���^

//============================================================================
//   �֐���`
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �L�[�g���K�擾

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void VBlankIntr(void)
{
    // �s�N�`���[�t���[���J�E���^���C���N�������g
    gPictureFrame++;

    // �Q�[���t���[���ɍ��킹�čĕ`��
    if (!(gPictureFrame % PICTURE_FRAME_PER_GAME_FRAME))
    {
        DispVBlankFunc();
    }

    //---- ���荞�݃`�F�b�N�t���O
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  ���C�����[�`��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    u16 tgid = 0;

    // ��ʁAOS�̏�����
    CommonInit();
    // ��ʏ�����
    DispInit();
    // �q�[�v�̏�����
    InitAllocateSystem();

    // ���荞�ݗL��
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    OS_InitTick();
    FX_Init();

    DispOn();

    passby_endflg = 0;
    
    /* ���ꂿ�����ʐM�̏�����, �o�^, �N�� */
    User_Init();

    for (gFrame = 0; passby_endflg == 0; gFrame++)
    {
        /* �L�[���͏��擾 */
        KeyRead(&gKey);

        // �X�N���[���N���A
        BgClear();
        
        /* �ʐM��Ԃɂ�菈����U�蕪�� */
        switch (WXC_GetStateCode())
        {
        case WXC_STATE_END:
            ModeSelect();
            break;
        case WXC_STATE_ENDING:
            break;
        case WXC_STATE_ACTIVE:
            if(WXC_IsParentMode() == TRUE)
            {
                BgPutString(9, 2, 0x2, "Now parent...");
            }
            else
            {
                BgPutString(9, 2, 0x2, "Now child...");
            }
            break;
        }
        if (gKey.trg & PAD_BUTTON_START)
        {
            (void)WXC_End();
        }

        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
    }

    (void)WXC_End();
    
    // WXC_End����������܂ő҂�
    while( WXC_GetStateCode() != WXC_STATE_END )
    {
        ;
    }

    // �ϐ�������
    gPictureFrame = 0;

    // �f�[�^�V�F�A�����O�ʐM�p�Ƀo�b�t�@��ݒ�
    GInitDataShare();

    // ����������
    if( WH_Initialize() == FALSE )
    {
        OS_Printf("\n WH_Initialize() ERROR!!\n");
    }

    if(passby_endflg == 1)
    {
        WH_SetGgid(WH_GGID);
        
        // �ڑ��q�@�̔���p�֐���ݒ�
        WH_SetJudgeAcceptFunc(JudgeConnectableChild);
    
        /* ���C�����[�`�� */
        for (gFrame = 0; TRUE; gFrame++)
        {
            BgClear();

            switch (WH_GetSystemState())
            {
            case WH_SYSSTATE_IDLE:
                (void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, (u16)(parentparam.tgid+1), parentparam.channel);
                break;

            case WH_SYSSTATE_CONNECTED:
            case WH_SYSSTATE_KEYSHARING:
            case WH_SYSSTATE_DATASHARING:
                {
                    BgPutString(8, 1, 0x2, "Parent mode");
                    if(GStepDataShare(gPictureFrame) == FALSE)
                    {
                        gPictureFrame--;
                    }
                    GMain();
                }
                break;
            }

            // ������s�N�`���[�t���[���̊�����҂�
            while (TRUE)
            {
                // V�u�����N�҂�
                OS_WaitVBlankIntr();
                if (!(gPictureFrame % PICTURE_FRAME_PER_GAME_FRAME))
                {
                    break;
                }
            }
        }
    }
    else
    {
        OS_Printf("\nmacAddress = %02X:%02X:%02X:%02X:%02X:%02X\n", bssdesc.bssid[0],bssdesc.bssid[1],bssdesc.bssid[2],bssdesc.bssid[3],bssdesc.bssid[4],bssdesc.bssid[5]);
        
        bssdesc.gameInfo.tgid++;
    
        // ���C�����[�v
        for (gFrame = 0; TRUE; gFrame++)
        {
            // �X�N���[���N���A
            BgClear();

            // �ʐM��Ԃɂ�菈����U�蕪��
            switch (WH_GetSystemState())
            {
            case WH_SYSSTATE_CONNECT_FAIL:
                {
                    // WM_StartConnect()�Ɏ��s�����ꍇ�ɂ�WM�����̃X�e�[�g���s���ɂȂ��Ă����
                    // ��xWM_Reset��IDLE�X�e�[�g�Ƀ��Z�b�g����K�v������܂��B
                    WH_Reset();
                }
                break;
            case WH_SYSSTATE_IDLE:
                {
                    static retry = 0;
                    enum
                    {
                        MAX_RETRY = 30
                    };

                    if (retry < MAX_RETRY)
                    {
                        ModeConnect(bssdesc.bssid);
                        retry++;
                        break;
                    }
                    // MAX_RETRY�Őe�@�ɐڑ��ł��Ȃ����ERROR�\��
                }
            case WH_SYSSTATE_ERROR:
                ModeError();
                break;
            case WH_SYSSTATE_BUSY:
            case WH_SYSSTATE_SCANNING:
                ModeWorking();
                break;

            case WH_SYSSTATE_CONNECTED:
            case WH_SYSSTATE_KEYSHARING:
            case WH_SYSSTATE_DATASHARING:
                {
                    BgPutString(8, 1, 0x2, "Child mode");
                    if(GStepDataShare(gPictureFrame) == FALSE)
                    {
                        gPictureFrame--;
                    }
                    GMain();
                }
                break;
            }

            // �d�g��M���x�̕\��
            {
                int level;
                level = WH_GetLinkLevel();
                BgPrintStr(31, 23, 0xf, "%d", level);
            }

            // ������s�N�`���[�t���[���̊�����҂�
            while (TRUE)
            {
                // V�u�����N�҂�
                OS_WaitVBlankIntr();
                if (!(gPictureFrame % PICTURE_FRAME_PER_GAME_FRAME))
                {
                    break;
                }
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  �e�@/�q�@ �I����ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeSelect(void)
{
    BgPutString(3, 1, 0x2, "Push A to start");

    if (gKey.trg & PAD_BUTTON_A)
    {
        //********************************
        User_Init();
        //********************************
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  �G���[�\����ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeError(void)
{
    BgPutString(5, 10, 0x2, "======= ERROR! =======");
    BgPutString(5, 13, 0x2, " Fatal error occured.");
    BgPutString(5, 14, 0x2, "Please reboot program.");
}


/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  �L�[���͏���ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�����o�B

  Arguments:    pKey  - �ҏW����L�[���͏��\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInfo * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }
    pKey->cnt = r;                     // �����H�L�[����
}

/*---------------------------------------------------------------------------*
  Name:         JudgeConnectableChild

  Description:  �Đڑ����ɐڑ��\�Ȏq�@���ǂ����𔻒肷��֐�

  Arguments:    cb      �ڑ����Ă����q�@�̏��.

  Returns:      �ڑ����󂯕t����ꍇ�� TRUE.
                �󂯕t���Ȃ��ꍇ�� FALSE.
 *---------------------------------------------------------------------------*/
static BOOL JudgeConnectableChild(WMStartParentCallback *cb)
{
    if (cb->macAddress[0] != macAddress[0] || cb->macAddress[1] != macAddress[1] || 
        cb->macAddress[2] != macAddress[2] || cb->macAddress[3] != macAddress[3] || 
        cb->macAddress[4] != macAddress[4] || cb->macAddress[5] != macAddress[5] )
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         ModeConnect

  Description:  �ڑ��J�n

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ModeConnect()
{
    WH_SetGgid(WH_GGID);
    //********************************
    (void)WH_ChildConnectAuto(WH_CONNECTMODE_DS_CHILD, bssdesc.bssid,
                              bssdesc.channel);
    //********************************
}

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  ��������ʂ�\��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ModeWorking(void)
{
    BgPrintStr(9, 11, 0xf, "Now working...");
}
