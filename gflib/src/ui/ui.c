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
};


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
    return pUI;
}

//==============================================================================
/**
 * @brief UIMain����
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

//������
void GFL_UI_sysMain(UISYS* pUI)
{
    GFL_UI_Key_sysMain(pUI);
    GFL_UI_TP_sysMain(pUI);
    
}

//==============================================================================
/**
 * @brief   UI�I������
 * @param   pUI    UISYS
 * @return  none
 */
//==============================================================================

void GFL_UI_sysEnd(UISYS* pUI)
{
    GFL_HEAP_FreeMemory(pUI->pKey);
    GFL_HEAP_FreeMemory(pUI->pTP);
    GFL_HEAP_FreeMemory(pUI);
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SleepDisable(UISYS* pUI,const u8 sleepTypeBit)
{
	pUI->DontSleep |= sleepTypeBit;
}

//------------------------------------------------------------------
/**
 * @brief   �X���[�v��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   sleepTypeBit �X���[�v�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SleepEnable(UISYS* pUI, const u8 sleepTypeBit)
{
	pUI->DontSleep &= ~(sleepTypeBit);
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ��֎~����
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SoftResetDisable(UISYS* pUI,const u8 softResetBit)
{
	pUI->DontSoftReset |= softResetBit;
}

//------------------------------------------------------------------
/**
 * @brief   �\�t�g�E�G�A���Z�b�g��Ԃ�������
 * @param   pUI		���[�U�[�C���^�[�t�F�C�X�Ǘ��\����
 * @param   softResetBit ���Z�b�g�Ǘ�BIT
 * @return  none
 */
//------------------------------------------------------------------
void GFL_UI_SoftResetEnable(UISYS* pUI, const u8 softResetBit)
{
	pUI->DontSoftReset &= ~(softResetBit);
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


