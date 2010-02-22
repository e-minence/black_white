//======================================================================
/**
 * @file  trial_house_def.h
 * @brief  �g���C�A���n�E�X�֘A�\���̒�`
 * @author  Saito
 */
//======================================================================
#pragma once

#include "savedata/bsubway_savedata.h"  //for _BSUBWAY_PARTNER_DATA
#include "battle/bsubway_battle_data.h"  //for _BSUBWAY_PARTNER_DATA

#include "poke_tool/pokeparty.h"

typedef struct TH_POINT_WORK_tag
{
  u16 TurnNum;       //���������^�[����
  u16 PokeChgNum;    //����
  u16 VoidAtcNum;    //���ʂ��Ȃ��Z���o������
  u16 WeakAtcNum;    //�΂���̋Z���o������
  u16 ResistAtcNum;  //���܂ЂƂ̋Z���o������
  u16 VoidNum;       //���ʂ��Ȃ��Z���󂯂���
  u16 ResistNum;     //���܂ЂƂ̋Z���󂯂���
  u16 WinTrainerNum; //�|�����g���[�i�[��
  u16 WinPokeNum;    //�|�����|�P������
  u16 LosePokeNum;   //�|���ꂽ�|�P������
  u16 RestHpPer;     //�c��g�o����
  u16 UseWazaNum;    //�g�p�����Z�̐�
}TH_POINT_WORK;

typedef struct TRIAL_HOUSE_WORK_tag
{
  BSUBWAY_PARTNER_DATA  TrData;
  HEAPID HeapID;
  int MemberNum;        //�V���O��3�@�_�u��4
  POKEPARTY *Party;
  BSWAY_PLAYMODE PlayMode;
  BOOL DownLoad;
  TH_POINT_WORK PointWork;
}TRIAL_HOUSE_WORK;
