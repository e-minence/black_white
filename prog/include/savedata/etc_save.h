//==============================================================================
/**
 * @file    etc_save.h
 * @brief   ��2��MISC�w�b�_
 * @author  matsuda
 * @date    2010.04.06(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
///���̑��Z�[�u�\���̂̕s���S�^
typedef struct _ETC_SAVE_WORK ETC_SAVE_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 EtcSave_GetWorkSize( void );
extern void EtcSave_WorkInit(void *work);
extern ETC_SAVE_WORK * SaveData_GetEtc( SAVE_CONTROL_WORK * p_sv );

extern void EtcSave_SetAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id);
extern BOOL EtcSave_CheckAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id);
