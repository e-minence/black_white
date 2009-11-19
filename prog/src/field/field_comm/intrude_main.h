//==============================================================================
/**
 * @file    intrude_main.h
 * @brief   �N�����C���̃w�b�_
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#pragma once

#include "field/field_subscreen.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�p���X�^�E���ł͂Ȃ��]�[�����Ƃ�����������ID
#define PALACE_TOWN_TBLNO_NULL    (0xff)

///Intrude_CheckSameZoneID�֐��̖߂�l
typedef enum{
  CHECKSAME_SAME,           ///<�����]�[���ɂ���
  CHECKSAME_SAME_REVERSE,   ///<�\���̃]�[���ɂ���
  CHECKSAME_NOT,            ///<�Ⴄ�]�[���ɂ���
}CHECKSAME;

enum{
  PALACE_TOWN_SIDE_FRONT,       ///<�\�t�B�[���h
  PALACE_TOWN_SIDE_REVERSE,     ///<���t�B�[���h
};

///PalaceTownData�̃f�[�^��
#define PALACE_TOWN_DATA_MAX      (8)
///PalaceTownData�̃p���X�����w��
#define PALACE_TOWN_DATA_PALACE   (PALACE_TOWN_DATA_MAX)
///PalaceTownData�̖����l
#define PALACE_TOWN_DATA_NULL     (0xff)

///�p���X�̗��t�B�[���h�փ��[�v���鎞�̃��[�v����W�̃p�^�[����
#define PALACE_WARP_POS_PATERN    (4)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�p���X�̊X�f�[�^�\����
typedef struct{
  u16 front_zone_id;        ///<�\�t�B�[���h�̃]�[��ID
  u16 reverse_zone_id;      ///<���t�B�[���h�̃]�[��ID
  u8 subscreen_x;           ///<�T�u�X�N���[���̊X�̈ʒuX
  u8 subscreen_y;           ///<�T�u�X�N���[���̊X�̈ʒuY
//  u8 padding[2];
  struct{
    u16 gx;
    u16 gz;
    u16 y;
  }warp_pos[PALACE_WARP_POS_PATERN];
}PALACE_TOWN_DATA;

///�p���X�̊X�f�[�^�A��������
typedef struct{
  u16 zone_id;
  u8 front_reverse;         ///<�\�A���A�ǂ���ɂ��邩(PALACE_TOWN_SIDE_???)
  u8 tblno;
}PALACE_TOWN_RESULT;

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void Intrude_Main(INTRUDE_COMM_SYS_PTR intcomm);
extern FIELD_SUBSCREEN_MODE Intrude_SUBSCREEN_Watch(GAME_COMM_SYS_PTR game_comm, FIELD_SUBSCREEN_WORK *subscreen);
extern BOOL Intrude_SearchPalaceTown(ZONEID zone_id, PALACE_TOWN_RESULT *result);
extern CHECKSAME Intrude_CheckSameZoneID(const PALACE_TOWN_RESULT *result_a, const PALACE_TOWN_RESULT *result_b);
extern void Intrude_PlayerPosRecalc(INTRUDE_COMM_SYS_PTR intcomm, fx32 mine_x);

//--------------------------------------------------------------
//  ���[�N�A�N�Z�X�֐�
//--------------------------------------------------------------
extern void Intrude_SetActionStatus(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_ACTION action);
extern void Intrude_SetSendProfileBuffer(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL Intrude_SetSendStatus(INTRUDE_COMM_SYS_PTR intcomm);
extern void Intrude_SetProfile(
  INTRUDE_COMM_SYS_PTR intcomm, int net_id, const INTRUDE_PROFILE *profile);
extern void Intrude_SetPlayerStatus(
  INTRUDE_COMM_SYS_PTR intcomm, int net_id, const INTRUDE_STATUS *sta);
extern void Intrude_InitTalkWork(INTRUDE_COMM_SYS_PTR intcomm, int talk_netid);
extern void Intrude_SetTalkReq(INTRUDE_COMM_SYS_PTR intcomm, int net_id);
extern void Intrude_SetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int net_id, INTRUDE_TALK_STATUS talk_status);
extern void Intrude_SetTalkCancel(INTRUDE_COMM_SYS_PTR intcomm, int net_id);
extern INTRUDE_TALK_STATUS Intrude_GetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm);
extern int Intrude_GetPalaceTownZoneID(int town_tblno);
extern void Intrude_GetPalaceTownRandPos(int town_tblno, VecFx32 *vec);
extern INTRUDE_COMM_SYS_PTR Intrude_Check_CommConnect(GAME_COMM_SYS_PTR game_comm);
extern u16 Intrude_GetObjCode(const INTRUDE_STATUS *sta, const MYSTATUS *myst);
extern BOOL Intrude_ObjPauseCheck(GAMESYS_WORK *gsys);

//==============================================================================
//  �f�[�^
//==============================================================================
extern const PALACE_TOWN_DATA PalaceTownData[];
extern u32 DisguiseObjCodeTblMax;

