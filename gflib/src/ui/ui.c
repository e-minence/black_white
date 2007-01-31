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

//==============================================================================
//
//			��`�A�֐��錾
//
//==============================================================================


//------------------------------------------------------------------
/**
 * @brief	_UI_SYS �^�錾
 * ���[�U�[�C���^�[�t�F�C�X���
 */
//------------------------------------------------------------------
struct _UI_SYS {
    UI_KEYSYS* pKey;       ///< �L�[�Ǘ��\����
	u8 DontSleep;          ///< �X���[�v��Ԃɂ��Ă͂����Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
	u8 DontSoftReset;      ///< �\�t�g���Z�b�g�������Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
	u8 DS_Boot_Flag;       ///<
    u8 AgbCasetteVersion;          ///< AGB�J�Z�b�g�Ή����̃J�Z�b�g�o�[�W���� �ʏ��0
    PMBackLightSwitch lightState;  ///< �o�b�N���C�g��IPL���
} ;

static UISYS* _pUI = NULL;   ///< �������������Ȃ��̂ł����Ń|�C���^�Ǘ�
static void _UI_SleepFunc(void);

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

void GFL_UI_sysInit(const int heapID)
{
    UISYS* pUI = GFL_HEAP_AllocMemory(heapID, sizeof(UISYS));

    MI_CpuClear8(pUI, sizeof(UISYS));
    pUI->pKey = GFL_UI_Key_sysInit(heapID);
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

void GFI_UI_sysMain(UISYS* pUI)
{
    GFL_UI_Key_sysMain();
    _UI_SleepFunc();  // �X���[�v��ԊǗ�
}

//==============================================================================
/**
 * @brief UIMain����
 * @param   none
 * @return  none
 */
//==============================================================================
void GFL_UI_sysMain(void)
{
    GFI_UI_sysMain(_pUI);
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

void GFI_UI_sysEnd(UISYS* pUI)
{
    GFL_UI_Key_sysEnd();
    GFL_HEAP_FreeMemory(pUI);
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   none
 * @return  none
 */
//==============================================================================

void GFL_UI_sysExit(void)
{
    GFI_UI_sysEnd(_pUI);
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
            GFL_UI_TPAutoSamplingStop();
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
            GFL_UI_TPAutoSamplingReStart();
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

