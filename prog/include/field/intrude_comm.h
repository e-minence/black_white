//==============================================================================
/**
 * @file    
 * @brief   �ȒP�Ȑ���������
 * @author  matsuda
 * @date    2009.09.03(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================
#pragma once

#include "field/fieldmap_proc.h"

//==============================================================================
//  �\���̒�`
//==============================================================================
///GameCommSys_Boot���鎞�Ɉ����n��parent_work
typedef struct{
  GAMESYS_WORK *gsys;
  GAME_COMM_SYS_PTR game_comm;
  u8 parent_macAddress[6];
  u8 my_invasion;             ///<TRUE:�N���J�n���郆�[�U�[�ł���
  u8 padding;
}FIELD_INVALID_PARENT_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void * IntrudeComm_InitCommSystem( int *seq, void *pwk );
extern BOOL  IntrudeComm_InitCommSystemWait( int *seq, void *pwk, void *pWork );
extern void  IntrudeComm_UpdateSystem( int *seq, void *pwk, void *pWork );
extern BOOL  IntrudeComm_TermCommSystem( int *seq, void *pwk, void *pWork );
extern BOOL  IntrudeComm_TermCommSystemWait( int *seq, void *pwk, void *pWork );
extern void IntrudeComm_FieldCreate(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork);
extern void IntrudeComm_FieldDelete(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork);

