//======================================================================
/**
 * @file  bsubway_scrwork.h
 * @bfief  �o�g���T�u�E�F�C�X�N���v�g���[�N
 * @author Satoshi Nohara
 * @date  07.05.28
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef  __SUBWAY_SCRWORK_H__
#define  __SUBWAY_SCRWORK_H__

#include "field/location.h"

#include "net_app/comm_entry_menu.h"

#include "savedata/bsubway_savedata.h"
#include "savedata/bsubway_savedata_def.h"
#include "battle/bsubway_battle_data.h"
#include "bsubway_scr_def.h"

#include "event_field_proclink.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
#if 0 //wb
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
#endif

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�X�N���v�g���[�N
//--------------------------------------------------------------
struct _TAG_BSUBWAY_SCRWORK
{
  int magicNo;  ///<�}�W�b�N�i���o�[
  int heapID;  ///<�q�[�v
  
  u8 member_num;  ///<�����o�[��
  u8 play_mode;  ///<�v���C�f�[�^
  u8 my_sex;      ///<�����̐���
  u8 pare_sex;    ///<�����ƃp�[�g�i�[�̐���
  
  u8 clear_f:1;  ///<�N���A�t���O
  u8 boss_f:2;  ///<�{�X��|�����t���O
  u8 retire_f:1;  ///<���^�C�A�t���O
  u8 prize_f:1;  ///<���J���t���O
  u8 partner:3;  ///<�p�[�g�i�[No
  u8 comm_sio_f:1; ///<�ʐM���t���O
  u8 padding0:7; ///<�]��
  u8 padding1; ///<�]��
  
  u16 pare_poke[2];  ///<�ʐM�}���`�p�[�g�i�[�����|�P����No
  
  u16 pare_stage_no; ///<�ʐM�}���`�p�[�g�i�[�̎���
  u16 rec_turn; ///<���������^�[����
  u16 rec_damage; ///<�H������_���[�W
  u16 rec_down; ///<�|�P�����̃_�E����
  
  u8  member[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̎莝��No
  u16  mem_poke[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P������monsno
  u16  mem_item[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̃A�C�e��no
  u16  trainer[BSUBWAY_STOCK_TRAINER_MAX];  ///<�ΐ�g���[�i�[No
  
  LOCATION now_location; //���݂̃��P�[�V����
  
  GAMEDATA *gdata; ///<GAMEDATA
  BSUBWAY_PLAYDATA *playData;  ///<�Z�[�u�f�[�^�ւ̃|�C���^
  BSUBWAY_SCOREDATA *scoreData;  ///<�Z�[�u�f�[�^�ւ̃|�C���^
  
  u8 pokelist_select_num[6]; ///<�|�P�����I���Ŏ擾�����莝��No
  u16 pokelist_result_select; ///<�|�P�������X�g�߂�l
  u16 pokelist_return_mode;
  
  //�ΐ푊��f�[�^�i�[�ꏊ
  BSUBWAY_PARTNER_DATA  tr_data[2];
 	
  //5�l�O���I�f�[�^�i�[�ꏊ
	BSUBWAY_PARTNER_DATA five_data[BSWAY_FIVE_NUM];
  
	//5�l�O���I�|�P�����p�����[�^�i�[�ꏊ
	BSUBWAY_PAREPOKE_PARAM five_poke[BSWAY_FIVE_NUM];

	//5�l�O���I�|�P�����A�C�e��fix�t���O
	u8 five_item[BSWAY_FIVE_NUM]; 
  
  u16  send_buf[35];  //�ʐM�p�f�[�^�o�b�t�@
  u16  recv_buf[35];  //�ʐM�p�f�[�^�o�b�t�@
  
  COMM_ENTRY_BEACON commBeaconData; //�r�[�R���f�[�^
  COMM_ENTRY_MENU_PTR pCommEntryMenu; //�ʐM�}���`�Q�����j���[
  u16 *pCommEntryResult; //�ʐM�}���`�Q�����j���[���ʊi�[��
  u8  comm_recieve_count; //��M�J�E���g
  u8  comm_mode; //�ʐM���[�h
  u8  first_btl_flag;    //����������Ȃ������t���O
  u8  comm_timing_no; //�ʐM�^�C�~���O�ԍ�
  u16 comm_check_work;
  u16 ret_work_id;
  
  //�v���`�i�Œǉ�
//  int winlose_flag; //(LOCATION_WORK�������āA�����ɓ���邱�Ƃ��\�H)
// void *p_work; //(LOCATION_WORK�������āA�����ɓ���邱�Ƃ��\�H)
};

#endif  //__SUBWAY_SCRWORK_H__
