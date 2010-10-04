//==============================================================================
/**
 * @file    d_haisin_power.h
 * @brief   
 * @author  matsuda
 * @date    2010.08.18(��)
 */
//==============================================================================
#pragma once

#include "field/gpower_id.h"


//==============================================================================
//  �萔��`
//==============================================================================
///��x�ɃZ�b�g�ł���z�M�p���[��
#define HAISIN_POWER_MAX    (8)

enum{
  HAISIN_RETURN_MODE_NAMEIN,
  HAISIN_RETURN_MODE_SELFMSG,
  HAISIN_RETURN_MODE_START,
};


//==============================================================================
//  �\���̒�`
//==============================================================================
///�z�M�p���[�ݒ�f�[�^
typedef struct{
  GPOWER_ID g_power_id; //G�p���[ID
  s32 time;       //�b
  s32 odds;       //�m��
}HAISIN_POWER;

///�z�M�p���[�ݒ�f�[�^�O���[�v
typedef struct{
  HAISIN_POWER hp[HAISIN_POWER_MAX];
  u32 data_num;   //hp�ɓ����Ă���f�[�^��

  s8 start_time_hour;
  s8 start_time_min;
  s8 end_time_hour;
  s8 end_time_min;
  s32 beacon_space_time;  ///<�r�[�R���z�M�Ԋu
  int movemode;    ///< ���샂�[�h 0�����_�� 1�����΂�
}HAISIN_POWER_GROUP;

typedef struct{
  HAISIN_POWER_GROUP powergroup;
  STRBUF *name;           ///<�z�M�}�V����
  STRBUF *selfmsg;        ///<�ꌾ���b�Z�[�W
  s32 union_index;        ///<������
  int return_mode;
  BOOL bsavedata;   ///< �Z�[�u����Ă������ǂ���
}HAISIN_CONTROL_WORK;


//--------------------------------------------------------------
//  �Z�[�u
//--------------------------------------------------------------
///�Z�[�u�f�[�^
typedef struct{
  u32 magic_key;    //HAISIN_SAVE_MAGICKEY

  HAISIN_POWER_GROUP powergroup;
  STRCODE name[SAVELEN_PLAYER_NAME + EOM_SIZE];
  STRCODE selfmsg[8 + EOM_SIZE];
  s32 union_index;
  s8 start_time_hour;
  s8 start_time_min;
  s8 end_time_hour;
  s8 end_time_min;
  s32 beacon_space_time;
}HAISIN_SAVEDATA;

///�Z�[�u�f�[�^�L���������}�W�b�N�L�[
#define HAISIN_SAVE_MAGICKEY    (0x3235aef)
