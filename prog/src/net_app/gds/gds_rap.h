//==============================================================================
/**
 * @file	gds_rap.h
 * @brief	GDS���C�u���������b�p�[��������
 * @author	matsuda
 * @date	2008.01.09(��)
 */
//==============================================================================
#ifndef __GDS_RAP_H__
#define __GDS_RAP_H__

#pragma once

#include <dwc.h>
#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "net_app/gds/gds_battle_mode.h"
#include "print/wordset.h"
#include "net/nhttp_rap_evilcheck.h"
#include "net\nhttp_rap.h"


#include "battle\btl_common.h"
#include "savedata\battle_rec.h"
#include "savedata/battle_rec_local.h"
#include "musical\musical_define.h"
#include "net_app/gds/gds_profile_local.h"
#define __GT_GDS_DEFINE_H__
#define __GT_GDS_PROFILE_LOCAL_H__
#define __GT_GDS_BATTLE_REC_SUB_H__
//#define __GT_GDS_BATTLE_REC_H__
#define __GT_GDS_MUSICAL_SUB_H__
//#define __GT_GDS_MUSICAL_H__
#define __GDS_MIN_MAX_H__
#define ___POKE_NET_BUILD_DS___
#include <poke_net_gds.h>


// �T�[�o��URL�܂���IP�A�h���X�ƃ|�[�g�ԍ����`
#define SYACHI_SERVER_URL	("pkgdstest.nintendo.co.jp")
#define SYACHI_SERVER_PORT	(12401)


//==============================================================================
//	�萔��`
//==============================================================================
///GDS�G���[�^�C�v
enum{
	GDS_ERROR_TYPE_LIB,			///<���C�u�����G���[(POKE_NET_GDS_LASTERROR)
	GDS_ERROR_TYPE_STATUS,		///<�X�e�[�^�X�G���[(POKE_NET_GDS_STATUS)
	GDS_ERROR_TYPE_APP,			///<�e�A�v�����̃G���[
};

///�o�g�����[�hNo   ��BattleModeBitTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
typedef enum{
  BATTLEMODE_SEARCH_NO_COLOSSEUM_SINGLE_NOSHOOTER,    ///<�R���V�A�� �V���O�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_SINGLE_SHOOTER,      ///<�R���V�A�� �V���O�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_DOUBLE_NOSHOOTER,    ///<�R���V�A�� �_�u�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_DOUBLE_SHOOTER,      ///<�R���V�A�� �_�u�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_TRIPLE_NOSHOOTER,    ///<�R���V�A�� �g���v�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_TRIPLE_SHOOTER,      ///<�R���V�A�� �g���v�� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_ROTATION_NOSHOOTER,  ///<�R���V�A�� ���[�e�[�V���� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_ROTATION_SHOOTER,    ///<�R���V�A�� ���[�e�[�V���� �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_MULTI_NOSHOOTER,     ///<�R���V�A�� �}���` �V���[�^�[����
  BATTLEMODE_SEARCH_NO_COLOSSEUM_MULTI_SHOOTER,       ///<�R���V�A�� �}���` �V���[�^�[����
  BATTLEMODE_SEARCH_NO_SUBWAY_SINGLE,                 ///<�n���S �V���O��
  BATTLEMODE_SEARCH_NO_SUBWAY_DOUBLE,                 ///<�n���S �_�u��
  BATTLEMODE_SEARCH_NO_SUBWAY_MULTI,                  ///<�n���S �}���`
  BATTLEMODE_SEARCH_NO_RANDOM_SINGLE,                 ///<�����_���}�b�` �V���O��
  BATTLEMODE_SEARCH_NO_RANDOM_DOUBLE,                 ///<�����_���}�b�` �_�u��
  BATTLEMODE_SEARCH_NO_RANDOM_TRIPLE,                 ///<�����_���}�b�` �g���v��
  BATTLEMODE_SEARCH_NO_RANDOM_ROTATION,               ///<�����_���}�b�` ���[�e�[�V����
  BATTLEMODE_SEARCH_NO_RANDOM_SHOOTER,                ///<�����_���}�b�` �V���[�^�[�o�g��
  BATTLEMODE_SEARCH_NO_CONTEST,                       ///<�o�g�����
  BATTLEMODE_SEARCH_NO_ALL,                           ///<�S��(�{�ݎw�薳��)
}BATTLEMODE_SEARCH_NO;


//==============================================================================
//	�\���̒�`
//==============================================================================

///���M�p�����[�^�\����
typedef struct{
	//���M�f�[�^
	union{
    MUSICAL_SHOT_SEND mus_send; ///<�~���[�W�J���V���b�g���M�f�[�^
		BATTLE_REC_SEND *battle_rec_send_ptr;	///<���M�f�[�^�ւ̃|�C���^
		BATTLE_REC_SEARCH_SEND battle_rec_search;	///<�r�f�I�������M�f�[�^
	};

	//���M�f�[�^�ɕt������I�v�V�����p�����[�^
	union{
		//�~���[�W�J���V���b�g�̃T�u�p�����[�^
		struct{
			u16 recv_monsno;	///<�擾����J�e�S���[�ԍ�(�|�P�����ԍ�)
			u16 mus_padding;
		}musical;
		//�o�g���r�f�I�̃T�u�p�����[�^
		struct{
			u64 data_number;	///<�f�[�^�i���o�[
		}rec;
	}sub_para;
}GDS_RAP_SEND_WORK;

///GDS���b�p�[���[�N�̃T�u���[�N
typedef struct _GDS_RAP_SUB_PROCCESS_WORK{
	int local_seq;
	int wait;
	int work;
}GDS_RAP_SUB_PROCCESS_WORK;

///GDS���C�u�����̃G���[���Ǘ��\����
typedef struct _GDS_RAP_ERROR_INFO{
	BOOL occ;				///<TRUE:�G���[��񂠂�B�@FALSE:�G���[��񖳂�
	int type;				///<�G���[�^�C�v(GDS_ERROR_TYPE_???)
	int req_code;			///<���N�G�X�g�R�[�h(POKE_NET_RESPONSE.ReqCode)
	int result;				///<����(POKE_NET_RESPONSE.Result)
}GDS_RAP_ERROR_INFO;

///�f�[�^��M���̃R�[���o�b�N�֐��̌^
typedef void (*GDSRAP_RESPONSE_FUNC)(void *, const GDS_RAP_ERROR_INFO *);
///�ʐM�G���[���b�Z�[�W�\���̃R�[���o�b�N�֐��̌^
typedef void (*GDSRAP_ERROR_WIDE_MSG_FUNC)(void *, STRBUF *);

///�f�[�^��M���̃R�[���o�b�N�֐��Ȃǂ��܂Ƃ߂��\����
typedef struct{
	void *callback_work;	///<�f�[�^��M���̃R�[���o�b�N�֐��Ɉ����Ƃ��ēn���|�C���^

	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�~���[�W�J���V���b�g�o�^(POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST)
	GDSRAP_RESPONSE_FUNC func_musicalshot_regist;
	
	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�h���X�A�b�v�V���b�g�擾(POKE_NET_GDS_REQCODE_MUSICALSHOT_GET)
	GDSRAP_RESPONSE_FUNC func_musicalshot_get;
	
	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�o�g���r�f�I�o�^(POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST)
	GDSRAP_RESPONSE_FUNC func_battle_video_regist;

	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�o�g���r�f�I����(�ڍ׌����A�ŐV�A���C�ɓ��胉���L���O)(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH)
	GDSRAP_RESPONSE_FUNC func_battle_video_search_get;

	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�o�g���r�f�I�f�[�^�擾(POKE_NET_GDS_REQCODE_BATTLEDATA_GET)
	GDSRAP_RESPONSE_FUNC func_battle_video_data_get;

	///�f�[�^��M���̃R�[���o�b�N�֐��ւ̃|�C���^
	///�o�g���r�f�I���C�ɓ���o�^(POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE)
	GDSRAP_RESPONSE_FUNC func_battle_video_favorite_regist;
}GDS_RAP_RESPONSE_CALLBACK;

///�f�[�^��M��ɍs���T�u�v���Z�X�p�̃R�[���o�b�N�֐��̌^
typedef BOOL (*GDSRAP_RECV_SUB_PROCCESS_FUNC)(void *, void *);

///�f�[�^��M��ɍs���T�u�v���Z�X�p���[�N
typedef struct _GDS_RECV_SUB_PROCCESS_WORK{
	GDSRAP_RESPONSE_FUNC user_callback_func;	///<�f�[�^��M��ɌĂяo���̃R�[���o�b�N�֐�
	u16 recv_save_seq0;
	u16 recv_save_seq1;
	u8 recv_sub_seq;
	GDSRAP_RECV_SUB_PROCCESS_FUNC recv_sub_proccess;
}GDS_RECV_SUB_PROCCESS_WORK;

///GDS���C�u�����ANitroDWC�ɋ߂��֌W�̃��[�N�̍\����
typedef struct _GDS_RAP_WORK{
	GAMEDATA *gamedata;
	SAVE_CONTROL_WORK *savedata;
	
	//�A�v������n�����d�v�f�[�^
	POKE_NET_REQUESTCOMMON_AUTH pokenet_auth;
	int heap_id;
	
	POKE_NET_GDS_STATUS stat;
	POKE_NET_GDS_STATUS laststat;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///�f�[�^��M���̃R�[���o�b�N�֐�
	GDS_RAP_ERROR_INFO error_info;			///<�G���[���i�[�p���[�N
	
	//��check�@�b��
//	void *areanaLo;
	
	//���M�f�[�^
	GDS_RAP_SEND_WORK send_buf;				///<���M�f�[�^
	//��M�f�[�^
	void *response;		///<POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_xxxx����M����܂�
	
	//�����g�p���[�N
	int comm_initialize_ok;		///<TRUE:�ʐM���C�u�����������ς�
	BOOL gdslib_initialize;			///<TRUE:GDS���C�u�����������ς�
	BOOL connect_success;					///<TRUE:�l�b�g�ڑ���
	GDS_RAP_SUB_PROCCESS_WORK sub_work;		///<�T�u�v���Z�X����p���[�N

	int ConnectErrorNo;						///< DWC�E�܂��̓T�[�o�[����̃G���[
	int ErrorRet;
	int ErrorCode;
	int ErrorType;
	
	int send_req;	///<POKE_NET_GDS_REQCODE_???(���N�G�X�g�����̏ꍇ��POKE_NET_GDS_REQCODE_LAST)
	int recv_wait_req;///<send_req�̌��ʎ�M�f�[�^�҂�(POKE_NET_GDS_REQCODE_???)
	int send_req_option;	///<POKE_NET_GDS_REQCODE_???�ł���Ɍ����𕪂���ꍇ�̃I�v�V����

	GFL_MSGDATA *msgman_wifisys;		///<���b�Z�[�W�f�[�^�}�l�[�W���̃|�C���^
	WORDSET *wordset;						///<Alloc�������b�Z�[�W�p�P��o�b�t�@�ւ̃|�C���^
	STRBUF *ErrorString;					///<������W�J�o�b�t�@

	GDS_RECV_SUB_PROCCESS_WORK recv_sub_work;	///<�f�[�^��M��̃T�u�v���Z�X�p���[�N
	
	u8 div_save_seq;			///<�����Z�[�u���s�V�[�P���X
	u8 send_before_wait;		///<TRUE:���M�O�Ƀ��b�Z�[�W��������ׂ̈��E�F�C�g
	u8 local_seq;

  // �|�P�����F�؁@����
  NHTTP_RAP_WORK* p_nhttp;  // �|�P�����F�؃��[�N
  u16 poke_num;
  u8 sign[NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN];
  NHTTP_RAP_EVILCHECK_RESULT nhttp_last_error;    ///<�F�؃G���[���
  u8 error_poke_pos;        // �F�؃G���[���������Ă����|�P�����ʒu
  u8 error_nhttp;           // �F�؎擾���̂��̂��G���[
  DWCSvlResult svl_result;
}GDS_RAP_WORK;


///GDSRAP���������Ɉ����n������
typedef struct{
	int gs_profile_id;		///<GS�v���t�@�C��ID
	
	int heap_id;
	GAMEDATA *gamedata;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///�f�[�^��M���̃R�[���o�b�N�֐�
}GDSRAP_INIT_DATA;


//==============================================================================
//	�O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//	���C��
//--------------------------------------------------------------
extern int GDSRAP_Init(GDS_RAP_WORK *gdsrap, const GDSRAP_INIT_DATA *init_data);
extern int GDSRAP_Main(GDS_RAP_WORK *gdsrap);
extern void GDSRAP_Exit(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	���M�n
//--------------------------------------------------------------
extern int GDSRAP_Tool_Send_MusicalShotUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, const MUSICAL_SHOT_DATA *musshot);
extern int GDSRAP_Tool_Send_MusicalShotDownload(GDS_RAP_WORK *gdsrap, int monsno);
extern int GDSRAP_Tool_Send_BattleVideoUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp);
extern int GDSRAP_Tool_Send_BattleVideoSearchDownload(GDS_RAP_WORK *gdsrap, u16 monsno, BATTLEMODE_SEARCH_NO battle_mode_no, u8 country_code, u8 local_code);
extern int GDSRAP_Tool_Send_BattleVideoNewDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoSubwayDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoCommDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideo_DataDownload(GDS_RAP_WORK *gdsrap, u64 data_number);
extern int GDSRAP_Tool_Send_BattleVideo_FavoriteUpload(GDS_RAP_WORK *gdsrap, u64 data_number);

//--------------------------------------------------------------
//	�����n
//--------------------------------------------------------------
extern BOOL GDSRAP_MoveStatusAllCheck(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	�G���[
//--------------------------------------------------------------
extern BOOL GDSRAP_ErrorInfoGet(GDS_RAP_WORK *gdsrap, GDS_RAP_ERROR_INFO **error_info);
extern void GDSRAP_ErrorInfoClear(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	�f�o�b�O�p
//--------------------------------------------------------------
extern void DEBUG_GDSRAP_SaveFlagReset(GDS_RAP_WORK *gdsrap);
extern void DEBUG_GDSRAP_SendVideoProfileFreeWordSet(GDS_RAP_WORK *gdsrap, u16 *set_code);

#endif	//__GDS_RAP_H__
