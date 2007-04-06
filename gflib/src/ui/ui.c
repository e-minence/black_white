//==============================================================================
/**
 * @file	ui.c
 * @brief	���[�U�[�C���^�[�t�F�C�X�Ǘ�
 * @author	GAME FREAK Inc.
 * @date	2005.07.29
 */
//==============================================================================

#include "gflib.h"
#include "ui.h"
#include "ui_def.h"
#include "fade.h"

//==============================================================================
//
//			��`�A�֐��錾
//
//==============================================================================

#define _SOFTRESET_TYPE_NORMAL    (0)  // �d���������Ɠ���
#define _SOFTRESET_TYPE_NETERROR  (1)  // �ʐM�G���[�̎�



//------------------------------------------------------------------
/**
 * @brief	_UI_SYS �^�錾
 * ���[�U�[�C���^�[�t�F�C�X���
 */
//------------------------------------------------------------------
struct _UI_SYS {
    UI_KEYSYS* pKey;       ///< �L�[�Ǘ��\����
    GFL_UI_SLEEPRELEASE_FUNC* pRelease;  ///< �X���[�v�������ɌĂ΂��֐�
    void* pWork;           ///< �X���[�v�������Ɉ����Ƃ��ēn�����[�N
    u8 DontSleep;          ///< �X���[�v��Ԃɂ��Ă͂����Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
    u8 DontSoftReset;      ///< �\�t�g���Z�b�g�������Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
    u8 AgbCasetteVersion;          ///< AGB�J�Z�b�g�Ή����̃J�Z�b�g�o�[�W���� �ʏ��0
    PMBackLightSwitch lightState;  ///< �o�b�N���C�g��IPL���
} ;

static UISYS* _pUI = NULL;   ///< �������������Ȃ��̂ł����Ń|�C���^�Ǘ�
static void _UI_SleepFunc(void);
static void _UI_ResetLoop(int resetNo);

//==============================================================================
//
//			�֐�
//
//==============================================================================

//==============================================================================
/**
 * @brief UI������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  none
 */
//==============================================================================

void GFL_UI_Boot(const HEAPID heapID)
{
    UISYS* pUI = GFL_HEAP_AllocMemory(heapID, sizeof(UISYS));

    MI_CpuClear8(pUI, sizeof(UISYS));
    pUI->pKey = GFL_UI_KEY_Init(heapID);
    _pUI = pUI;

    OS_EnableIrq();  // ���̊֐��͉�c�ő��k������ňړ�����  @@OO

    PM_GetBackLight(NULL,&pUI->lightState);  // �o�b�N���C�g��ԓǂݍ���
    //return pUI;
}

//==============================================================================
/**
 * @brief UIMain����
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

void GFI_UI_Main(UISYS* pUI)
{
    GFL_UI_KEY_Main();
    _UI_SleepFunc();  // �X���[�v��ԊǗ�

    if ((GFL_UI_KEY_GetCont() & PAD_BUTTON_SOFTRESET) == PAD_BUTTON_SOFTRESET) {
        if(pUI->DontSoftReset == 0){  // �}������BIT�������������Reset
            _UI_ResetLoop(_SOFTRESET_TYPE_NORMAL);  // ���̊֐�����softreset�����
        }
    }

}

//==============================================================================
/**
 * @brief UIMain����
 * @param   none
 * @return  none
 */
//==============================================================================
void GFL_UI_Main(void)
{
    GFI_UI_Main(_pUI);
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

void GFI_UI_Exit(UISYS* pUI)
{
    GFL_UI_KEY_Exit();
    GFL_HEAP_FreeMemory(pUI);
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   none
 * @return  none
 */
//==============================================================================

void GFL_UI_Exit(void)
{
    GFI_UI_Exit(_pUI);
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFI_UI_SleepDisable(UISYS* pUI,const u8 sleepTypeBit)
{
    pUI->DontSleep |= sleepTypeBit;
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ��֎~����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SleepDisable(const u8 sleepTypeBit)
{
    GFI_UI_SleepDisable(_pUI, sleepTypeBit);
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFI_UI_SleepEnable(UISYS* pUI, const u8 sleepTypeBit)
{
    pUI->DontSleep &= ~(sleepTypeBit);
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ�������
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SleepEnable(const u8 sleepTypeBit)
{
    GFI_UI_SleepEnable(_pUI, sleepTypeBit);
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�������ɌĂ΂��֐����Z�b�g����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
void GFI_UI_SleepReleaseSetFunc(UISYS* pUI, GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork)
{
    pUI->pRelease = pFunc;
    pUI->pWork = pWork;
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v�������ɌĂ΂��֐����Z�b�g����
 * @param   pFunc   �X���[�v�������ɌĂ΂��֐�
 * @param   pWork   �Ă΂��ۂɓn�����[�N
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork)
{
    GFI_UI_SleepReleaseSetFunc(_pUI, pFunc, pWork);
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFI_UI_SoftResetDisable(UISYS* pUI,const u8 softResetBit)
{
    pUI->DontSoftReset |= softResetBit;
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SoftResetDisable(const u8 softResetBit)
{
    GFI_UI_SoftResetDisable(_pUI, softResetBit);
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFI_UI_SoftResetEnable(UISYS* pUI, const u8 softResetBit)
{
    pUI->DontSoftReset &= ~(softResetBit);
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ�������
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SoftResetEnable(const u8 softResetBit)
{
    GFI_UI_SoftResetEnable(_pUI, softResetBit);
}

//------------------------------------------------------------------
/**
 * @brief   �L�[�Ǘ��\���̂������o�� �O���[�v���֐�
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @return  �L�[�Ǘ��\����
 */
//------------------------------------------------------------------

UI_KEYSYS* _UI_GetKEYSYS(const UISYS* pUI)
{
    return pUI->pKey;       ///< �L�[�Ǘ��\����
}


//------------------------------------------------------------------
/**
 * @brief   UI�Ǘ��\���̂������o�� �O���[�v���֐�
 * @param   none
 * @return  ���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 */
//------------------------------------------------------------------
UISYS* _UI_GetUISYS(void)
{
    if(_pUI==NULL){
        OS_TPanic("no init");
    }
    return _pUI;
}

//---------------------------------------------------------------------------
/**
 * @brief	�X���[�v��Ԃ̊Ǘ�
 * @param	none
 */
//---------------------------------------------------------------------------
static void _UI_SleepFunc(void)
{
    PMBackLightSwitch up,down;
    PMWakeUpTrigger trigger;
    UISYS* pUI = _UI_GetUISYS();

    if(PAD_DetectFold()){ // �ӂ����܂��Ă���
        if(pUI->DontSleep == 0){  // �X���[�v���Ă����ꍇ
            GFL_UI_TP_AutoSamplingStop();
            trigger = PM_TRIGGER_COVER_OPEN|PM_TRIGGER_CARD;
            // �����AGB�J�[�g���b�W���h�����Ă���ꍇ�̂ݕ��A�����ɃJ�[�g���b�W�ݒ�
            if(pUI->AgbCasetteVersion)
                trigger |= PM_TRIGGER_CARTRIDGE;
            //SLEEP
            PM_GoSleepMode( trigger, 0, 0 );
            // ���A��A�J�[�h��������Ă�����d��OFF
            if(CARD_IsPulledOut()){
                PM_ForceToPowerOff();
            } else if((OS_GetIrqMask() & OS_IE_CARTRIDGE) && CTRDG_IsPulledOut()){
                // ���A��A�J�[�g���b�W��������Ă�����c
                if(PAD_DetectFold()){
                    // �܂��ӂ����܂��Ă����ԂȂ�΍ēx�X���[�v�ɓ�������ɓd��OFF
                    PM_GoSleepMode( PM_TRIGGER_COVER_OPEN|PM_TRIGGER_CARD, 0, 0 );
                    PM_ForceToPowerOff();
                } else {
                    // �ӂ����J���Ă�����d��OFF
                    PM_ForceToPowerOff();
                }
            }
            {
                GFL_UI_SLEEPRELEASE_FUNC* pRelease = pUI->pRelease;
                pRelease(pUI->pWork);
            }
            GFL_UI_TP_AutoSamplingReStart();
        } else{
            // �������J�[�g���b�W�������ꂽ��SLEEP���d��OFF
            if((OS_GetIrqMask() & OS_IE_CARTRIDGE) && CTRDG_IsPulledOut()){
                PM_GoSleepMode( PM_TRIGGER_COVER_OPEN|PM_TRIGGER_CARD, 0, 0 );
                PM_ForceToPowerOff();
            }
            //BK OFF
            PM_GetBackLight(&up,&down);
            if(PM_BACKLIGHT_ON == up){
                PM_SetBackLight(PM_LCD_ALL,PM_BACKLIGHT_OFF);
            }
        }
    } else{  // �J���Ă���
        PM_GetBackLight(&up,&down);
        if(PM_BACKLIGHT_OFF == up){
            PM_SetBackLight(PM_LCD_ALL, pUI->lightState);
        }
    }
}

//---------------------------------------------------------------------------
/**
 * @brief	�\�t�g�E�G�A���Z�b�g���N�����ꍇ�̏���
 * @param	resetNo   OS_ResetSystem�ɓn�����Z�b�g���
 */
//---------------------------------------------------------------------------
static void _UI_ResetLoop(int resetNo)
{
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0,16, _GFI_FADE_BASESPEED );
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB, 0,16, _GFI_FADE_BASESPEED );
    GFL_NET_Disconnect();  // �ʐM���Z�b�g�ֈڍs
#if 0  //����ǉ� @@OO
    // �����Z�[�u���Ă���L�����Z�����Ă����Ȃ��ƃ��Z�b�g�ł��Ȃ�
    SaveData_DivSave_Cancel(SaveData_GetPointer());
#endif
    while (1) {
        // �ʐM�Ƃ��ă��Z�b�g���Ă�������� + �������[�J�[�h�I��
        if(GFL_NET_IsResetEnable() && CARD_TryWaitBackupAsync()){

            GFL_BG_Exit();  // ���O����������G���A�J��������e�X�g���s��  k.ohno

            OS_ResetSystem(resetNo);  // �ؒf�m�F��I��
        }
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);

        GFL_UI_Main();   // �K�v�ŏ���GFL�֐�
        GFL_NET_Main();
        GFL_FADE_Main();
        
    }
}

