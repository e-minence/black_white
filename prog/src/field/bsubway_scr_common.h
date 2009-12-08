//======================================================================
/**
 * @file  bsubway_scr_common.h
 * @bfief  �o�g���T�u�E�F�C�X�N���v�g���[�N
 * @author Satoshi Nohara
 * @date  07.05.28
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef  __SUBWAY_SCR_COMMON_H__
#define  __SUBWAY_SCR_COMMON_H__

//#include "battle/battle_common.h"
//#include "battle/b_tower_data.h"

#include "savedata/bsubway_savedata.h"
#include "savedata/bsubway_savedata_def.h"
#include "battle/bsubway_battle_data.h"
#include "bsubway_scr_def.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  LOCATION_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  int zone_id;
  int door_id;
  int grid_x;
  int grid_z;
  int dir;
}LOCATION_WORK;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�X�N���v�g���[�N
//--------------------------------------------------------------
typedef struct _BSUBWAY_SCRWORK
{
  int  magicNo;  ///<�}�W�b�N�i���o�[
  int  heapID;  ///<�q�[�v
  u32  play_rnd_seed;  ///<�v���C�p�����_���V�[�h
  u8  now_round;  ///<���݉��l�ځH
  u8  now_win;  ///<���݉��l�ɏ������H
  u8  member_num;  ///<�����o�[��
  u8  play_mode;  ///<�v���C�f�[�^

  u8  clear_f:1;  ///<�N���A�t���O
  u8  leader_f:2;  ///<�^�C�N�[��2
  u8  retire_f:1;  ///<���^�C�A�t���O
  u8  prize_f:1;  ///<���J���t���O
  u8  partner:3;  ///<�p�[�g�i�[No

  u8  my_sex;      ///<�����̐���
  u8  pare_sex;    ///<�����ƃp�[�g�i�[�̐���
  u16  pare_stage;    ///<�ʐM�}���`�p�[�g�i�[�̎���
  u16  pare_poke[2];  ///<�ʐM�}���`�p�[�g�i�[�����|�P����No
  
  u16  renshou;  ///<���݂̘A����
  u16  stage;    ///<���݂̎���
  u32  win_cnt;  ///<���݂܂ł̑�������

  u16  rec_turn;  ///<���������^�[����
  u16  rec_damage;  ///<�H������_���[�W
  u16  rec_down;  ///<�|�P�����̃_�E����

  u8  member[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̎莝��No
  u16  mem_poke[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P������monsno
  u16  mem_item[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̃A�C�e��no
  u16  trainer[BSUBWAY_STOCK_TRAINER_MAX];  ///<�ΐ�g���[�i�[No

  //���݂̃��P�[�V�������L�����Ă���
  LOCATION_WORK  now_location;

  BSUBWAY_PLAYWORK*  playSave;  ///<�Z�[�u�f�[�^�ւ̃|�C���^
  BSUBWAY_SCOREWORK*  scoreSave;  ///<�Z�[�u�f�[�^�ւ̃|�C���^

  //�ΐ푊��f�[�^�i�[�ꏊ
  BSUBWAY_PARTNER_DATA  tr_data[2];
  
  //5�l�O���I�f�[�^�i�[�ꏊ
  BSUBWAY_PARTNER_DATA  five_data[BSWAY_FIVE_NUM];
  
  //5�l�O���I�|�P�����p�����[�^�i�[�ꏊ
  BSUBWAY_PAREPOKE_PARAM  five_poke[BSWAY_FIVE_NUM];
  
  //5�l�O���I�|�P�����A�C�e��fix�t���O
  u8            five_item[BSWAY_FIVE_NUM];
  
  //�ʐM�p�f�[�^�o�b�t�@
  u16  send_buf[35];
  u16  recv_buf[35];
  
  //�v���`�i�Œǉ�
  int winlose_flag;    //(LOCATION_WORK�������āA�����ɓ���邱�Ƃ��\�H)
  void* p_work;      //(LOCATION_WORK�������āA�����ɓ���邱�Ƃ��\�H)
  u8  recieve_count;
  u8  mode;
  u8  first_btl_flag;    //����������Ȃ������t���O
  u8  dummy;
  u16  check_work;
  u16  ret_wkno;
}BSUBWAY_SCRWORK;

#endif  //__SUBWAY_SCR_COMMON_H__
