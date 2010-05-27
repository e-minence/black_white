//==============================================================================
/**
 * @file    comm_player.h
 * @brief   �ʐM�v���C���[����̃w�b�_
 * @author  matsuda
 * @date    2009.07.17(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/gamesystem.h"
#include "field/fldmmdl.h"

//==============================================================================
//  �^��`
//==============================================================================
///COMM_PLAYER_SYS�\���̂̕s��`�|�C���^�^
typedef struct _COMM_PLAYER_SYS * COMM_PLAYER_SYS_PTR;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�ʐM�v���C���[�̍��W�f�[�^�p�b�P�[�W
typedef struct{
  BOOL vanish;
  VecFx32 pos;
  u16 dir;
  u8 player_form;       ///<PLAYER_MOVE_FORM
  u8 map_attr_hitch:1;  ///<TRUE:�N���s�̃A�g���r���[�g�̏�ɂ���
  u8 padding:7;
}COMM_PLAYER_PACKAGE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern COMM_PLAYER_SYS_PTR CommPlayer_Init(int max, GAMESYS_WORK *gsys, HEAPID heap_id);
extern void CommPlayer_Exit(GAMESYS_WORK *gsys, COMM_PLAYER_SYS_PTR cps);
extern void CommPlayer_Update(COMM_PLAYER_SYS_PTR cps);
extern void CommPlayer_Add(COMM_PLAYER_SYS_PTR cps, int index, u16 obj_code, const COMM_PLAYER_PACKAGE *pack);
extern void CommPlayer_Del(COMM_PLAYER_SYS_PTR cps, int index);
extern void CommPlayer_Push(COMM_PLAYER_SYS_PTR cps);
extern void CommPlayer_Pop(COMM_PLAYER_SYS_PTR cps);
extern void CommPlayer_FlagSet_UpdateStop(COMM_PLAYER_SYS_PTR cps, BOOL stop_flag);
extern BOOL CommPlayer_CheckOcc(COMM_PLAYER_SYS_PTR cps, int index);
extern u16 CommPlayer_GetObjCode(COMM_PLAYER_SYS_PTR cps, int index);
extern void CommPlayer_SetParam(COMM_PLAYER_SYS_PTR cps, int index, const COMM_PLAYER_PACKAGE *pack);
extern BOOL CommPlayer_Mine_DataUpdate(COMM_PLAYER_SYS_PTR cps, COMM_PLAYER_PACKAGE *pack);
extern BOOL CommPlayer_SearchGridPos(COMM_PLAYER_SYS_PTR cps, s16 gx, s16 gz, u32 *out_index);
extern BOOL CommPlayer_SetGyoeTask(COMM_PLAYER_SYS_PTR cps, int index);
extern MMDL * CommPlayer_GetMmdl(COMM_PLAYER_SYS_PTR cps, int index);
