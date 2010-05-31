//==============================================================================
/**
 * @file    union_app.h
 * @brief   ���j�I�����[���ł̃~�j�Q�[������n
 * @author  matsuda
 * @date    2010.01.06(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/gamedata_def.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_types.h"
#include "savedata/record.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�r���ޏo���N�G�X�g�̐e�@����̕Ԏ�
typedef enum{
  UNION_APP_LEAVE_NULL,   ///<�Ԏ�����M
  UNION_APP_LEAVE_OK,     ///<�ޏoOK
  UNION_APP_LEAVE_NG,     ///<�ޏoNG
}UNION_APP_LEAVE;


//==============================================================================
//  �^��`
//==============================================================================
///�����R�[���o�b�N�֐��̌^
typedef void (*UNION_APP_CALLBACK_ENTRY_FUNC)(NetID net_id, const MYSTATUS *mystatus, void *userwork);
///�ޏo�R�[���o�b�N�֐��̌^
typedef void (*UNION_APP_CALLBACK_LEAVE_FUNC)(NetID net_id, const MYSTATUS *mystatus, void *userwork);


//==============================================================================
//  �\���̒�`
//==============================================================================

//--------------------------------------------------------------
//  �e�~�j�Q�[���p��ParentWork
//--------------------------------------------------------------
///���G�����pParentWork
typedef struct{
  UNION_APP_PTR uniapp;
  GAMEDATA *gamedata;
}PICTURE_PARENT_WORK;

///���邮������pParentWork
typedef struct{
  UNION_APP_PTR uniapp;
  GAMEDATA *gamedata;
  POKEPARTY *party;

//  int max;
  SAVE_CONTROL_WORK     *sv;
  RECORD                *record;
  
  //���邮��������ŃZ�b�g���Ă��炤���[�N
  BOOL                  bgm_change;   ///<TRUE:���邮���������BGM�̕ύX���s����
}GURUGURU_PARENT_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �V�X�e���Ŏg�p
//--------------------------------------------------------------
extern UNION_APP_PTR UnionAppSystem_AllocAppWork(
  UNION_SYSTEM_PTR unisys, HEAPID heap_id, u8 member_max, const MYSTATUS *myst);
extern void UnionAppSystem_FreeAppWork(UNION_APP_PTR uniapp);
extern BOOL UnionAppSystem_SetEntryUser(UNION_APP_PTR uniapp, NetID net_id, const MYSTATUS *myst);
extern void UnionAppSystem_Update(UNION_APP_PTR uniapp, UNION_SYSTEM_PTR unisys);
extern void UnionAppSystem_ReqBasicStatus(UNION_APP_PTR uniapp, NetID net_id);
extern void UnionAppSystem_SetBasicStatus(
  UNION_APP_PTR uniapp, const UNION_APP_BASIC *basic_status);
extern void UnionAppSystem_ReqMystatus(UNION_APP_PTR uniapp, NetID net_id);
extern void UnionAppSystem_SetMystatus(UNION_APP_PTR uniapp, UNION_SYSTEM_PTR unisys, NetID net_id, const UNION_APP_MY_PARAM *pAppmy);
extern BOOL UnionAppSystem_CheckBasicStatus(UNION_APP_PTR uniapp);
extern BOOL UnionAppSystem_CheckMystatus(UNION_APP_PTR uniapp);
extern void UnionAppSystem_SetIntrudeReady(UNION_APP_PTR uniapp, NetID net_id);
extern void UnionAppSystem_SetLeaveChild(UNION_APP_PTR uniapp, NetID net_id);
extern u32 UnionAppSystem_GetBasicSize(void);
extern void UnionAppSystem_IntrudeReadySetup(UNION_APP_PTR uniapp);
extern BOOL UnionAppSystem_GetEntryNGFlag(UNION_APP_PTR uniapp);


//--------------------------------------------------------------
//  �ȉ��A�A�v���P�[�V�����p
//--------------------------------------------------------------
//==================================================================
/**
 * �����A�ޏo���̃R�[���o�b�N�֐���o�^
 *
 * @param   uniapp		        ���j�I���A�v�����䃏�[�N�ւ̃|�C���^
 * @param   entry_callback		�����R�[���o�b�N
 * @param   eave_callback		  �ޏo�R�[���o�b�N
 * @param   userwork		      �R�[���o�b�N�֐��ň����Ƃ��ēn����郏�[�N�̃|�C���^
 */
//==================================================================
extern void Union_App_SetCallback(UNION_APP_PTR uniapp, 
  UNION_APP_CALLBACK_ENTRY_FUNC entry_callback, UNION_APP_CALLBACK_LEAVE_FUNC leave_callback, 
  void *userwork);

//==================================================================
/**
 * �������֎~����@���e�@��p����
 *
 * @param   uniapp		
 *
 * @retval  BOOL		  TRUE:�֎~�ɂ����B�@FALSE:�֎~�ɏo���Ȃ�(���ɗ�����]�҂�����)
 */
//==================================================================
extern BOOL Union_App_Parent_EntryBlock(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �����֎~�����Z�b�g����@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
extern void Union_App_Parent_ResetEntryBlock(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �w��l�������������󂯓����@���e�@��p����
 *
 * @param   uniapp		
 * @param   num       �󂯓����l��
 */
//==================================================================
extern void Union_App_Parent_EntryBlockNum(UNION_APP_PTR uniapp, int num);

//==================================================================
/**
 * �ޏo���֎~����(Union_App_ReqLeave�̌��ʂ����NG�ɂȂ�܂�)�@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
extern void Union_App_Parent_LeaveBlock(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �ޏo�֎~�����Z�b�g���܂��@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
extern void Union_App_Parent_ResetLeaveBlock(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �e�@�ɓr���ޏo�̋������N�G�X�g���܂�
 *
 * @param   uniapp		
 *
 * ���̊֐����s��AUnion_App_GetLeaveResult�Ō��ʂ̎擾���s���Ă�������
 */
//==================================================================
extern void Union_App_ReqLeave(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �r���ޏo���N�G�X�g�̐e�@����̕Ԏ����擾���܂�
 *
 * @param   uniapp		
 *
 * @retval  UNION_APP_LEAVE		�e�@����̕Ԏ�
 */
//==================================================================
extern UNION_APP_LEAVE Union_App_GetLeaveResult(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �ʐM�ؒf���܂�
 *
 * @param   uniapp		
 */
//==================================================================
extern void Union_App_Shutdown(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �ʐM�ؒf�̊����҂�
 *
 * @param   uniapp		
 *
 * @retval  BOOL		TRUE:�ʐM�ؒf�����@FALSE:������
 */
//==================================================================
extern BOOL Union_App_WaitShutdown(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �w��NetID��MYSTATUS���擾���܂�
 *
 * @param   net_id		�擾�����������NetID
 *
 * @retval  const MYSTATUS *		MYSTATUS�ւ̃|�C���^(NetID�̃��[�U�[���ޏo���Ă���ꍇ��NULL)
 */
//==================================================================
extern const MYSTATUS * Union_App_GetMystatus(UNION_APP_PTR uniapp, NetID net_id);

//==================================================================
/**
 * �ڑ����Ă���v���C���[��NetID��bit�P�ʂŎ擾���܂�
 *    ��GFL_NET_SendDataExBit�ł��̂܂܎g����`�ł�
 *
 * @param   uniapp		
 *
 * @retval  u32		�ڑ��v���C���[NetID(bit�P��)
 *
 * NetID:0�� NetID:2�� �̃v���C���[�������q�����Ă���ꍇ��
 *    return (1 << 0) | (1 << 2);
 * �Ƃ������l���Ԃ�܂�
 */
//==================================================================
extern u32 Union_App_GetMemberNetBit(UNION_APP_PTR uniapp);

//==================================================================
/**
 * �ڑ��l�����擾���܂�
 *
 * @param   uniapp		
 *
 * @retval  u8		�ڑ��l��
 */
//==================================================================
extern u8 Union_App_GetMemberNum(UNION_APP_PTR uniapp);
