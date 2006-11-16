//*****************************************************************************
/**
 *@file		ui_def.h
 *@brief	���[�U�[�C���^�[�t�F�C�X�p�O���[�v����`
 *@author	k.ohno
 *@data		2006.11.16
 */
//*****************************************************************************

#ifndef __UI_DEF_H__
#define	__UI_DEF_H__

//------------------------------------------------------------------
/**
 * @brief	�e�@�\�̊Ǘ��\���̒�`
 */
//------------------------------------------------------------------
typedef struct _UI_TPSYS  UI_TPSYS;
typedef struct _UI_KEYSYS  UI_KEYSYS;

extern UI_TPSYS* _UI_GetTPSYS(const UISYS* pUI);

extern UI_KEYSYS* _UI_GetKEYSYS(const UISYS* pUI);


#endif  //__UI_DEF_H__
