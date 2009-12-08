//======================================================================
/**
 * @file	bsubway_savedata_def.h
 * @bfief	�o�g���T�u�E�F�C�@�Z�[�u�f�[�^�֘A�萔��`
 * @author iwasawa
 * @date	07.05.28
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef	__SUBWAY_SAVEDATA_DEF_H__
#define __SUBWAY_SAVEDATA_DEF_H__

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// �v���C�f�[�^�擾ID
//--------------------------------------------------------------
typedef enum
{
 BSWAY_PSD_playmode,
 BSWAY_PSD_round,
 BSWAY_PSD_rec_down,
 BSWAY_PSD_rec_turn,
 BSWAY_PSD_rec_damage,
 BSWAY_PSD_pokeno,
 BSWAY_PSD_pare_poke,
 BSWAY_PSD_pare_itemfix,
 BSWAY_PSD_trainer,
 BSWAY_PSD_partner,
 BSWAY_PSD_rnd_seed,
}BSWAY_PSD_ID;

//--------------------------------------------------------------
/// �X�R�A�f�[�^���샂�[�h
//--------------------------------------------------------------
typedef enum
{
 BSWAY_DATA_get,		///<�f�[�^�擾
 BSWAY_DATA_set,		///<�Z�b�g
 BSWAY_DATA_reset,	///<���Z�b�g
 BSWAY_DATA_inc,		///<1++
 BSWAY_DATA_dec,		///<1--
 BSWAY_DATA_add,		///<add
 BSWAY_DATA_sub,		///<sub
}BSWAY_DATA_SETID;

//--------------------------------------------------------------
/// �X�R�A�f�[�^�t���O �A�N�Z�XID
//--------------------------------------------------------------
typedef enum
{
 BSWAY_SFLAG_SILVER_GET,
 BSWAY_SFLAG_GOLD_GET,
 BSWAY_SFLAG_SILVER_READY,
 BSWAY_SFLAG_GOLD_READY,
 BSWAY_SFLAG_WIFI_LOSE_F,
 BSWAY_SFLAG_WIFI_UPLOAD,
 BSWAY_SFLAG_WIFI_POKE_DATA,
 BSWAY_SFLAG_SINGLE_POKE_DATA,
 BSWAY_SFLAG_SINGLE_RECORD,
 BSWAY_SFLAG_DOUBLE_RECORD,
 BSWAY_SFLAG_MULTI_RECORD,
 BSWAY_SFLAG_CMULTI_RECORD,
 BSWAY_SFLAG_WIFI_RECORD,
 BSWAY_SFLAG_COPPER_GET,
 BSWAY_SFLAG_COPPER_READY,
 BSWAY_SFLAG_WIFI_MULTI_RECORD,		//�v���`�i�ǉ�
 BSWAY_SFLAG_MAX,					//�ő吔
}BSWAY_SFLAG_ID;

///�v���C���[���b�Z�[�W�f�[�^�@�A�N�Z�XID
typedef enum
{
 BSWAY_MSG_PLAYER_READY,
 BSWAY_MSG_PLAYER_WIN,
 BSWAY_MSG_PLAYER_LOSE,
 BSWAY_MSG_LEADER,
}BSWAY_PLAYER_MSG_ID;

///�X�R�A�f�[�^�@�|�P�����f�[�^�A�N�Z�X���[�h
typedef enum
{
 BSWAY_SCORE_POKE_SINGLE,
 BSWAY_SCORE_POKE_WIFI,
}BSWAY_SCORE_POKE_DATA;

///�I������莝���|�P��������MAX
#define BSUBWAY_STOCK_MEMBER_MAX	(4)
///�ۑ����Ă����ΐ�g���[�i�[�i���o�[�̐�
#define BSUBWAY_STOCK_TRAINER_MAX	(14)
///�ۑ����Ă���AI�}���`�y�A�̃|�P�����p�����[�^��
#define BSUBWAY_STOCK_PAREPOKE_MAX	(2)
///WiFi DL�v���C���[�f�[�^��
#define BSUBWAY_STOCK_WIFI_PLAYER_MAX	(7)
///WiFi DL���[�_�[�f�[�^��
#define BSUBWAY_STOCK_WIFI_LEADER_MAX	(30)
///���[���f�[�^��DL�t���O�G���A�o�b�t�@��
#define BSUBWAY_ROOM_DATA_FLAGS_LEN	(250)

//kari pt_save.h
#define	WAZA_TEMOTI_MAX		(4)		///<1�̂̃|�P���������Ă�Z�̍ő�l

#endif //__SUBWAY_SAVEDATA_DEF_H__
