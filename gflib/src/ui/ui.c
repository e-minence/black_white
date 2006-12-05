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
    UI_TPSYS* pTP;         ///< �^�b�`�p�l���Ǘ��\����
    UI_KEYSYS* pKey;       ///< �L�[�Ǘ��\����
	u8 DontSleep;          ///< �X���[�v��Ԃɂ��Ă͂����Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
	u8 DontSoftReset;      ///< �\�t�g���Z�b�g�������Ȃ��ꍇBIT�������Ă��� ���v�ȏꍇ0
	u8 DS_Boot_Flag;       ///< 
} ;

static UISYS* _pUI = NULL;   ///< �������������Ȃ��̂ł����Ń|�C���^�Ǘ�

//==============================================================================
//
//			�֐�
//
//==============================================================================

//==============================================================================
/**
 * @brief UI������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UISYS  work�n���h��
 */
//==============================================================================

UISYS* GFL_UI_sysInit(const int heapID)
{
    UISYS* pUI = GFL_HEAP_AllocMemory(heapID, sizeof(UISYS));

    MI_CpuClear8(pUI, sizeof(UISYS));
    pUI->pKey = GFL_UI_Key_sysInit(heapID);
    pUI->pTP = GFL_UI_TP_sysInit(heapID);
    _pUI = pUI;
    return pUI;
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
    GFL_UI_TP_sysMain();
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
    GFL_UI_TP_sysEnd();
    GFL_HEAP_FreeMemory(pUI);
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   none
 * @return  none
 */
//==============================================================================

void GFL_UI_sysEnd(void)
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
 * @brief      �^�b�`�p�l���Ǘ��\���̂������o�� �O���[�v���֐�
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @return  �^�b�`�p�l���Ǘ��\����
 */
//------------------------------------------------------------------

UI_TPSYS* _UI_GetTPSYS(const UISYS* pUI)
{
    return pUI->pTP;
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
    return _pUI;
}
