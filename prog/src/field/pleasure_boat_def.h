//======================================================================
/**
 * @file  pleasure_boat_def.h
 * @brief  �V���D�֘A�\���̒�`
 * @author  Saito
 */
//======================================================================
#pragma once

#define ROOM_NUM  (15)
#define DBL_BTL_ROOM_NUM  (2)

//@note ��g���[�i�[�����邪�ǂ��Ƀg���[�i�[���z�u����邩������Ȃ��̂őS�������ێ����Ă��郁���o������
typedef struct PL_BOAT_ROOM_PRM_tag
{
  int EntryID;    //���I�G���g���h�c�i0�I���W���ŁA�g���[�i�[�Ɣ�g���[�i�[�ŐU��Ȃ����j
  int TrID;       //�g���[�i�[�h�c(�g���[�i�[�̂Ƃ��Ɏg�p)
  int ObjCode;    //������
  int NpcType;    //���b�Z�[�W�Ƃ��m�o�b�^�C�v�Ƃ��̎��ʗp
  int NpcMsg[2];
  BOOL BattleEnd;   //���̕����Ő퓬���I�������H�i�g���[�i�[�̂Ƃ��Ɏg�p�j
}PL_BOAT_ROOM_PRM;

typedef struct PL_BOAT_WORK_tag
{
  BOOL WhistleReq;
  int Time;
  int WhistleCount;   //�D�J�Ȃ�����
  PL_BOAT_ROOM_PRM RoomParam[ROOM_NUM];
  u8 TrNumLeft;
  u8 TrNumRight;
  u8 TrNumSingle;
  u8 TrNumDouble;
  BOOL DblBtl[DBL_BTL_ROOM_NUM];
}PL_BOAT_WORK;

