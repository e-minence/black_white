//==============================================================================
/**
 *
 * �퓬�^��
 *
 */
//==============================================================================
#ifndef __GT_GDS_BATTLE_REC_H__
#define __GT_GDS_BATTLE_REC_H__


#include "gds_define.h"
#include "gds_profile_local.h"
#include "gds_battle_rec_sub.h"

#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/gds_profile.h"
#include "net_app/gds/gds_profile_local.h"
#include "battle/btl_common.h"
#include "savedata\battle_rec.h"
#include "savedata\battle_rec_local.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"


//----------------------------------------------------------
/**
 *	�^��Z�[�u�f�[�^�{�́i6116byte�j
 */
//----------------------------------------------------------
typedef struct {
  BTLREC_SETUP_SUBSET       setupSubset;   ///< �o�g����ʃZ�b�g�A�b�v�p�����[�^�̃T�u�Z�b�g
  BTLREC_OPERATION_BUFFER   opBuffer;      ///< �N���C�A���g������e�̕ۑ��o�b�t�@

  REC_POKEPARTY         rec_party[ BTL_CLIENT_MAX ];
  BTLREC_CLIENT_STATUS  clientStatus[ BTL_CLIENT_MAX ];

  u16 magic_key;
  u16 padding;
  
  //u32         checksum; //�`�F�b�N�T��  4byte
  GDS_CRC crc;              ///< CRC            4
}GT_BATTLE_REC_WORK;

//--------------------------------------------------------------
/**
 *	�퓬�^��̃w�b�_
 *		68byte
 */
//--------------------------------------------------------------
typedef struct {
	u16 monsno[GT_HEADER_MONSNO_MAX];	///<�|�P�����ԍ�(�\������K�v���Ȃ��̂Ń^�}�S�̏ꍇ��0)	24
	u8 form_no_and_sex[GT_HEADER_MONSNO_MAX];	///<6..0bit�ځF�|�P�����̃t�H�����ԍ��@7bit�ځF�|�P�����̐���		12

  u16 battle_counter;   ///<�A����                        2
  u16 mode;        ///<�퓬���[�h(�t�@�N�g���[50�A�t�@�N�g���[100�A�ʐM�ΐ�...)
  
  u16 magic_key;      ///<�}�W�b�N�L�[
  u8 secure;        ///<TRUE:���S���ۏႳ��Ă���B�@FALSE�F�Đ����������Ȃ�
  
  u8 server_vesion;   ///<�o�g���̃T�[�o�[�o�[�W����
  u8 work[12];      ///< �\��
	
	GT_DATA_NUMBER data_number;		///<�f�[�^�i���o�[(�T�[�o�[���ŃZ�b�g�����)	8
	GT_GDS_CRC crc;							///< CRC						4
}GT_BATTLE_REC_HEADER;

//--------------------------------------------------------------
/**
 *	���M�f�[�^�F�퓬�^��
 *	7500byte
 */
//--------------------------------------------------------------
typedef struct {
	GT_GDS_PROFILE profile;		///<GDS�v���t�B�[��		88
	GT_BATTLE_REC_HEADER head;		///<�퓬�^��̃w�b�_	100
	GT_BATTLE_REC_WORK rec;		///<�^��f�[�^�{��			7272
}GT_BATTLE_REC_SEND;

//--------------------------------------------------------------
/**
 *	���M�f�[�^�F�������N�G�X�g
 *	16byte
 */
//--------------------------------------------------------------
typedef struct{
	u16 monsno;					///< �|�P�����ԍ�
	u8 battle_mode;				///< �����{��
	u8 country_code;			///< �Z��ł��鍑�R�[�h
	u8 local_code;				///< �Z��ł���n���R�[�h
	
	u8 padding[3];				//�p�f�B���O
	
	u32 server_version;			///< ROM�̃T�[�o�[�o�[�W����
	
	u8 work[4];					///<�\��
}GT_BATTLE_REC_SEARCH_SEND;

//--------------------------------------------------------------
/**
 *	���M�f�[�^�F�����L���O�擾
 */
//--------------------------------------------------------------
typedef struct{
	u32 server_version;			///< ROM�̃T�[�o�[�o�[�W����
}GT_BATTLE_REC_RANKING_SEARCH_SEND;


//--------------------------------------------------------------
/**
 *	��M�f�[�^�F�T�v�ꗗ�擾��
 *	228byte
 */
//--------------------------------------------------------------
typedef struct {
	GT_GDS_PROFILE profile;		///<GDS�v���t�B�[��
	GT_BATTLE_REC_HEADER head;		///<�퓬�^��̃w�b�_
}GT_BATTLE_REC_OUTLINE_RECV;

//--------------------------------------------------------------
/**
 *	��M�f�[�^�F�ꌏ�̌������ʎ擾��
 *	7500byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;		///<GDS�v���t�B�[��
	GT_BATTLE_REC_HEADER head;		///<�퓬�^��̃w�b�_
	GT_BATTLE_REC_WORK rec;		///<�퓬�^��{��
}GT_BATTLE_REC_RECV;


#endif	//__GT_GDS_BATTLE_REC_H__

