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
#include "demo/comm_btl_demo.h"
#include "net_app/irc_match.h"
#include "net_app/btl_rec_sel.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// �퓬�^��f�[�^���݃t���O
//--------------------------------------------------------------
typedef enum
{
  BSW_BTLREC_EXIST_NG = 0, ///<���݃`�F�b�N�֐��Ă�ł��Ȃ�
  BSW_BTLREC_EXIST_NON, ///<�Ȃ�
  BSW_BTLREC_EXIST_EXIST, ///<����
}BSW_BTLREC_EXIST;

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
  
  u16        clear_f:1;  ///<�N���A�t���O
  u16         boss_f:2;  ///<�{�X��|�����t���O
  u16       retire_f:1;  ///<���^�C�A�t���O
  u16        prize_f:1;  ///<���J���t���O
  u16        partner:3;  ///<�p�[�g�i�[No
  u16     comm_sio_f:1; ///<�ʐM���t���O
  u16     comm_irc_f:1; ///<�ԊO���ʐM�t���O
  u16 btlrec_exist_f:2; ///<�퓬�^�摶�݃t���O
  u16    padding_bit:4; ///<�]��
  u8 padding[2]; //�]��
  
  BtlResult comm_btl_result; ///< ���s����
  u16 pare_stage_no; ///<�ʐM�}���`�p�[�g�i�[�̎���
  
  u16 pare_poke[2];  ///<�ʐM�}���`�p�[�g�i�[�����|�P����No
  
  u8  member[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̎莝��No
  u16  mem_poke[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P������monsno
  u16  mem_item[BSUBWAY_STOCK_MEMBER_MAX];  ///<�I�񂾃|�P�����̃A�C�e��no
  u16  trainer[BSUBWAY_STOCK_TRAINER_MAX];  ///<�ΐ�g���[�i�[No
  
  LOCATION now_location; //���݂̃��P�[�V����
  
  GAMEDATA *gdata; ///<GAMEDATA
  BSUBWAY_PLAYDATA *playData;  ///<�Z�[�u�f�[�^�ւ̃|�C���^
  BSUBWAY_SCOREDATA *scoreData;  ///<�Z�[�u�f�[�^�ւ̃|�C���^
  BSUBWAY_WIFI_DATA *wifiData; ///<�Z�[�u�f�[�^�ւ̃|�C���^

  u8 pokelist_select_num[6]; ///<�|�P�����I���Ŏ擾�����莝��No
  u16 pokelist_result_select; ///<�|�P�������X�g�߂�l
  u16 pokelist_return_mode;
  
  //�ΐ푊��f�[�^�i�[�ꏊ
  BSUBWAY_PARTNER_DATA  tr_data[2];
 	
  //�p�[�g�i�[���I�f�[�^�i�[�ꏊ
	BSUBWAY_PARTNER_DATA five_data[BSWAY_PARTNER_NUM];
	//�p�[�g�i�[���I�|�P�����p�����[�^�i�[�ꏊ
	BSUBWAY_PAREPOKE_PARAM five_poke[BSWAY_PARTNER_NUM];
	//5�l�O���I�|�P�����A�C�e��fix�t���O
	u8 five_item[BSWAY_PARTNER_NUM]; 
  
  u16  send_buf[35];  //�ʐM�p�f�[�^�o�b�t�@
  u16  recv_buf[35];  //�ʐM�p�f�[�^�o�b�t�@
  
  COMM_ENTRY_BEACON commBeaconData; //�r�[�R���f�[�^
  COMM_ENTRY_MENU_PTR pCommEntryMenu; //�ʐM�}���`�Q�����j���[
  u16 *pCommEntryResult; //�ʐM�}���`�Q�����j���[���ʊi�[��
  u8  comm_receive_count; //��M�J�E���g
  u8  comm_mode; //�ʐM���[�h
  u8  first_btl_flag;    //����������Ȃ������t���O
  u8  comm_timing_no; //�ʐM�^�C�~���O�ԍ�
  u16 comm_check_work;
  u16 ret_work_id;
  
  MYSTATUS mystatus_fr; //�ʐM�p�[�g�i�[��MYSTATUS
  COMM_BTL_DEMO_PARAM comm_btl_demo_param;
  IRC_MATCH_WORK irc_match;
  
  POKEPARTY *btl_box_party; //�o�g���{�b�N�X�pPOKEPARTY
  
  BATTLE_SETUP_PARAM *btl_setup_param;
  BTL_REC_SEL_PARAM btl_rec_sel_param;
  u16 btlrec_save_work[2];
};

#endif  //__SUBWAY_SCRWORK_H__
