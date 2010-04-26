//======================================================================
/**
 * @file  bsubway_savedata_local.h
 * @brief  �o�g���T�u�E�F�C�@�Z�[�u�f�[�^�\���̒�`
 * @authaor  iwasawa
 * @date  2008.12.11
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef __H_BSUBWAY_SAVEDATA_LOCAL_H__
#define __H_BSUBWAY_SAVEDATA_LOCAL_H__

//#include "multiboot/src/dl_child/pt_save.h"

#include "system/pms_data.h"
#include "system/gf_date.h"

#include "battle/bsubway_battle_data.h"
#include "savedata/bsubway_savedata_def.h"
#include "savedata/bsubway_savedata.h"


//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// �o�g���T�u�E�F�C �v���C���[���b�Z�[�W�f�[�^
/// 8*4=32byte
//--------------------------------------------------------------
struct _BSUBWAY_PLAYER_MSG
{
  PMS_DATA  msg[4];  ///<�����p���b�Z�[�W
};

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C WIFI �v���C���[�f�[�^(Dpw_Bt_Player)
/// 60*3+60=240byte
//--------------------------------------------------------------
struct _BSUBWAY_WIFI_PLAYER
{
  struct _BSUBWAY_POKEMON  poke[3];
  
  //�g���[�i�[��((PERSON_NAME_SIZE:7)+(EOM_SIZE:1))*(STRCODE:u16)=16
  STRCODE  name[PERSON_NAME_SIZE+EOM_SIZE];

  u8  casette_version;          //�J�Z�b�gversion
  u8  lang_version;            //����o�[�W����
  u8  country_code;            //���R�[�h
  u8  address;              //�Z��ł���Ƃ���

  u32  id_no;                //IDNo  

  u16  leader_word[4];            //���[�_�[���b�Z�[�W

  union{
    struct{
    u8  ngname_f:1;            //NG�l�[���t���O
    u8  gender:1;            //����
    u8      :6;            //���܂�6bit
    };
    u8  flags;
  };
  u8  tr_type;              //�g���[�i�[�^�C�v
  u16  appear_word[4];            //�o�ꃁ�b�Z�[�W  
  u16  win_word[4];            //�������b�Z�[�W  
  u16  lose_word[4];            //�s�ރ��b�Z�[�W  
  u16  result;                //����
};

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C WIFI DL�f�[�^
/// 280byte+1680(240*7)byte+1020(34*30)byte=2980byte
//--------------------------------------------------------------
struct _BSUBWAY_WIFI_DATA
{
  GFDATE  day;  ///<DL�������t4byte
  u8 flags[BSUBWAY_ROOM_DATA_FLAGS_LEN];  ///<DL�ς݃t���O�G���A(2000bit)
  u8 player_data_f;  ///<�v���C���[�f�[�^���L�����ǂ����H
  u8 leader_data_f;  ///<���[�_�[�f�[�^���L�����ǂ����H

  //DL�f�[�^�̃��[��ID
  u8  player_room;  ///<�v���C���[�f�[�^���[��No
  u8  player_rank;  ///<�v���C���[�f�[�^���[�������N
  u8  leader_room;  ///<���[�_�[�f�[�^���[��No
  u8  leader_rank;  ///<���[�_�[�f�[�^���[�������N

  ///�v���C���[�f�[�^
  struct _BSUBWAY_WIFI_PLAYER player[BSUBWAY_STOCK_WIFI_PLAYER_MAX];
  ///���[�_�[�f�[�^
  struct _BSUBWAY_LEADER_DATA leader[BSUBWAY_STOCK_WIFI_LEADER_MAX];
};

//======================================================================
//  extern
//======================================================================

#endif  //__H_BSUBWAY_SAVEDATA_LOCAL_H__
