//==============================================================================
/**
 * @file  battle_input_type.h
 * @brief �퓬���͉�ʂ̊O���Ŏg�p�����\���̂̒�`�Ȃ�
 * @author  matsuda
 * @date  2006.05.11(��)
 */
//==============================================================================
#ifndef __BATTLE_INPUT_TYPE_H__
#define __BATTLE_INPUT_TYPE_H__

#include "../btl_common.h"

//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
//  SCENE���[�N
//--------------------------------------------------------------
///�R�}���h�I����ʍ\���p��Scene���[�N
typedef struct{
  u8 client_type;
  u8 client_no;
  u8 sel_mons_no;
  u8    icon_status;

  s16   icon_hp;        //���݂�HP
  u16   icon_hpmax;       //���݂�HPMAX

  u8 cancel_escape;       ///<�u���ǂ�vor�u�ɂ���v
  u8 padding[3];
}BINPUT_SCENE_COMMAND;

///�Z�I����ʍ\���p��Scene���[�N
typedef struct{
  u16     wazano[ PTL_WAZA_MAX ];   //wazano
  u8      pp[ PTL_WAZA_MAX ];     //pp
  u8      ppmax[ PTL_WAZA_MAX ];      //ppmax
  u8      client_type;          //
}BINPUT_SCENE_WAZA;

//DIR_SELECT�p�i�v���`�i���玝���Ă����j
typedef struct{
  u8                  sex   :2;       //����
  u8                  exist :1;       //�I����
  u8                        :5;       //
  u8                  status;
  u16                 dummy;          //4�o�C�g���E�_�~�[

  s16                 hp;             //���݂�HP
  u16                 hpmax;          //���݂�HPMAX
  const POKEMON_PARAM *pp;
}DIR_SELECT_POKE_PARAM;

///�|�P�����I����ʍ\���p��Scene���[�N
typedef struct{
  DIR_SELECT_POKE_PARAM dspp[ BTL_CLIENT_MAX ];
  u8      client_type;
  u8      pokesele_type;  ///<�ΏۑI���^�C�v
}BINPUT_SCENE_POKE;

///�u�͂��E�������v�I����ʍ\���p��Scene���[�N
typedef struct{
  u16 waza_no;

  u16 dummy;    ///<4�o�C�g���E�_�~�[
}BINPUT_SCENE_YESNO;



#endif  //__BATTLE_INPUT_TYPE_H__

