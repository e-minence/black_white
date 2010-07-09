//==============================================================================
/**
 * @file  battle_rec_local.h
 * @brief �퓬�^�惍�[�J���w�b�_
 * @author  matsuda
 * @date  2009.11.18(��)
 */
//==============================================================================
#ifndef __BATTLE_REC_LOCAL_H__
#define __BATTLE_REC_LOCAL_H__

#pragma once


//----------------------------------------------------------------------
//  includes
//----------------------------------------------------------------------
#include "gds_local_common.h"

///�f�[�^�i���o�[�̃o�C�g�T�C�Y
#define DATANUMBER_SIZE   (sizeof(DATA_NUMBER))


///�^��f�[�^�̑��ݗL��
#define REC_OCC_MAGIC_KEY   (0xe281)


//--------------------------------------------------------------
/**
 *  �퓬�^��p�ɃJ�X�^�}�C�Y���ꂽPOKEPARTY
 */
//--------------------------------------------------------------
typedef struct{
  /// �ێ��ł���|�P�������̍ő�
  u16 PokeCountMax;
  /// ���ݕێ����Ă���|�P������
  u16 PokeCount;
  /// �|�P�����f�[�^
  REC_POKEPARA member[TEMOTI_POKEMAX];
}REC_POKEPARTY;

/**
  *  �o�g����ʃZ�b�g�A�b�v�p�����[�^�̘^��f�[�^�p�T�u�Z�b�g
 */
struct _BTLREC_SETUP_SUBSET {
  GFL_STD_RandContext   randomContext;    ///< �����̃^�l
  BTL_FIELD_SITUATION   fieldSituation;   ///< �w�i�E���~�Ȃǂ̍\�z�p���
  CONFIG                config;           ///< ���[�U�[�R���t�B�O
  u16  musicDefault;            ///< �f�t�H���gBGM
  u16  musicWin;                ///< ������BGM
  u16  debugFlagBit;            ///< �f�o�b�O�@�\Bit�t���O -> enum BtlDebugFlag @ battle/battle.h
  u8   competitor : 5;          ///< �ΐ�҃^�C�v�i�Q�[�����g���[�i�[�A�ʐM�ΐ�j-> enum BtlCompetitor @ battle/battle.h
  u8   myCommPos  : 3;          ///< �ʐM�ΐ펞�̎����̗����ʒu�i�}���`�̎��A0,2 vs 1,3 �ɂȂ�A0,1�������^2,3���E���ɂȂ�j
  u8   rule       : 4;          ///< ���[���i�V���O���E�_�u���E�g���v���E���[�e�j-> enum BtlRule @ battle/battle.h
  u8   MultiMode  : 3;          ///< �}���`�o�g�����[�h�i���[���͂��Ȃ炸�_�u���j-> enum BtlMultiMode_tag @ battle/battle.h
  u8   shooterBit : 1;          ///< �V���[�^�[�L���r�b�g�i��l���̊G�����ς��j

};

/**
  *  �^��Z�[�u�p�g���[�i�[�f�[�^
 */
typedef struct {
  u32     ai_bit;
  u16     tr_id;
  u16     tr_type;
  u16     use_item[ BSP_TRAINERDATA_ITEM_MAX ]; // �g�p����A�C�e��u16x4 = 8byte
  /// 16 byte

  STRCODE name[BUFLEN_PERSON_NAME];  // �g���[�i�[��<-32byte

  PMS_DATA  win_word;   //�퓬�I�����������b�Z�[�W <-8byte
  PMS_DATA  lose_word;  //�퓬�I�����������b�Z�[�W <-8byte

}BTLREC_TRAINER_STATUS;


typedef enum {
  BTLREC_CLIENTSTATUS_NONE,
  BTLREC_CLIENTSTATUS_PLAYER,
  BTLREC_CLIENTSTATUS_TRAINER,
}BtlRecClientStatusType;

typedef struct {
  u16                       type;         ///< BtlRecClientStatusType
  u16                       voiceLevel;   ///< �y���b�v�{�C�X���x��
  union {
    MYSTATUS                player;
    BTLREC_TRAINER_STATUS   trainer;
  };
}BTLREC_CLIENT_STATUS;

/**
  *  �N���C�A���g������e�ۑ��o�b�t�@
 */
struct _BTLREC_OPERATION_BUFFER {
  u32  size;
  u8   buffer[ BTLREC_OPERATION_BUFFER_SIZE ];
};

//----------------------------------------------------------
/**
 *  �^��Z�[�u�f�[�^�{�́i6116 bytes�j
 */
//----------------------------------------------------------
typedef struct _BATTLE_REC_WORK {
  BTLREC_SETUP_SUBSET       setupSubset;   ///< �o�g����ʃZ�b�g�A�b�v�p�����[�^�̃T�u�Z�b�g
  BTLREC_OPERATION_BUFFER   opBuffer;      ///< �N���C�A���g������e�̕ۑ��o�b�t�@

  REC_POKEPARTY         rec_party[ BTL_CLIENT_MAX ];
  BTLREC_CLIENT_STATUS  clientStatus[ BTL_CLIENT_MAX ];

  u16 magic_key;
  u16 padding;

  //CRC(�K���Ō���ɂ��Ă�������)
  GDS_CRC       crc;
}BATTLE_REC_WORK;

//--------------------------------------------------------------
/**
 *  �퓬�^��̃w�b�_  (68 byte)
 */
//--------------------------------------------------------------
typedef struct _BATTLE_REC_HEADER{
  u16 monsno[HEADER_MONSNO_MAX];  ///< �|�P�����ԍ�(�\������K�v���Ȃ��̂Ń^�}�S�̏ꍇ��0)
  u8  form_no_and_sex[HEADER_MONSNO_MAX]; ///< 6..0bit�ڃ|�P�����̃t�H�����ԍ��@7bit�ڐ��ʁi0�F��or�Ȃ� 1�F���j

  u16 battle_counter;   ///<�A����                        2
  u16 mode;        ///<�퓬���[�h(�t�@�N�g���[50�A�t�@�N�g���[100�A�ʐM�ΐ�...)

  u16 magic_key;      ///<�}�W�b�N�L�[
  u8 secure;        ///<TRUE:���S���ۏႳ��Ă���B�@FALSE�F�Đ����������Ȃ�

  u8 server_vesion;   ///<�o�g���̃T�[�o�[�o�[�W����
  u8 work[12];      ///< �\��

  //CRC�ƃf�[�^�i���o�[(�K���Ō���ɂ��Ă�������)
  DATA_NUMBER   data_number;///<�f�[�^�i���o�[(�T�[�o�[���ŃZ�b�g�����)�B�`�F�b�N�T���Ώۂ���O��
  GDS_CRC       crc;
}BATTLE_REC_HEADER;

//--------------------------------------------------------------
/**
 *  �^��Z�[�u�f�[�^�FGDS�v���t�B�[���{�w�b�_�{�{��
 */
//--------------------------------------------------------------
struct _BATTLE_REC_SAVEDATA{
  GDS_PROFILE profile;      ///<GDS�v���t�B�[��
  BATTLE_REC_HEADER head;     ///<�퓬�^��w�b�_
  BATTLE_REC_WORK rec;      ///<�퓬�^��{��
};


#endif  //__BATTLE_REC_LOCAL_H__
