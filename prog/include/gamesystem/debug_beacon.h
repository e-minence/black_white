//==============================================================================
/**
 * @file    debug_beacon.h
 * @brief   �f�o�b�O�p�̃r�[�R�����쐬
 * @author  matsuda
 * @date    2010.02.06(�y)
 */
//==============================================================================
#pragma once
#ifdef PM_DEBUG

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"


///�f�o�b�O�p�s���f�[�^
typedef struct{
  u16 action_no;            ///<�s��No
  u16 param[12];     ///<�p�����[�^(�s��No���ɈقȂ�)
}DEBUG_ACTION_DATA;

//==================================================================
/**
 * @brief   �f�o�b�O�p�̉��z�l���f�[�^�\����
 */
//==================================================================
typedef struct{
  u8 nation;                               ///<���R�[�h
  u8 area;                                 ///<�n��R�[�h
  
  u32 suretigai_count;                  ///<����Ⴂ�l��
  u8 g_power_id;                        ///<�������Ă���G�p���[ID(GPOWER_ID_xxx)
  u8 pm_version;                        ///<PM_VERSION

  u16 zone_id;                           ///<���ݒn
  u8 sex;                               ///<����
  u8 language;                          ///<PM_LANG

  u8 townmap_tbl_index;                 ///<�^�E���}�b�v�ł̍��W�e�[�u��Index
  u8 trainer_view;                      ///<���j�I�����[���ł̌�����(�O�`�P�T�j
  u8 research_team_rank;                ///<�����������N(RESEARCH_TEAM_RANK)
  u8 favorite_color_index;              ///<�{�̏��̐F(Index) �O�`�P�T
  u32 thanks_recv_count;                 ///<������󂯂���
  
  u16 trainer_id;                          ///<�g���[�i�[ID
  
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<�g���[�i�[��

  STRCODE self_introduction[GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN + EOM_SIZE]; ///<���ȏЉ�b�Z�[�W
  
  PMS_DATA pmsdata;                      ///<���ȏЉ�ȈՉ�b

  DEBUG_ACTION_DATA action_data;     ///<�f�t�H���g�̍s���f�[�^
}DEBUG_GAMEBEACON_INFO;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void DebugSendBuf_BeaconSet(const DEBUG_GAMEBEACON_INFO *debuginfo);
extern void DebugRecvBuf_BeaconSet(const DEBUG_GAMEBEACON_INFO *debuginfo, const DEBUG_ACTION_DATA *debugaction);

#endif  //PM_DEBUG
