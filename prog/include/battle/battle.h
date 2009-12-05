//=============================================================================================
/**
 * @file  battle.h
 * @brief �|�P����WB �o�g���V�X�e�� �O���V�X�e���Ƃ̃C���^�[�t�F�C�X
 * @author  taya
 *
 * @date  2008.09.22  �쐬
 */
//=============================================================================================
#ifndef __BATTLE_H__
#define __BATTLE_H__

#include <gflib.h>
#include <net.h>
#include <procsys.h>

#include "system/timezone.h"
#include "system/pms_data.h"
#include "poke_tool/pokeparty.h"
#include "tr_tool/trno_def.h"
#include "savedata/zukan_savedata.h"
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "buflen.h"

#include "battle_bg_def.h"  //zonetable�R���o�[�^����Q�Ƃ��������̂Œ�`�𕪗����܂��� by iwasawa

//--------------------------------------------------------------
/**
 *  �ΐ탋�[��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_RULE_SINGLE,    ///< �V���O��
  BTL_RULE_DOUBLE,    ///< �_�u��
  BTL_RULE_TRIPLE,    ///< �g���v��
  BTL_RULE_ROTATION,  ///< ���[�e�[�V����

}BtlRule;

//--------------------------------------------------------------
/**
 *  �ΐ푊��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMPETITOR_WILD,    ///< �쐶
  BTL_COMPETITOR_TRAINER, ///< �Q�[�����g���[�i�[
  BTL_COMPETITOR_SUBWAY,  ///< �o�g���T�u�E�F�C�g���[�i�[
  BTL_COMPETITOR_COMM,    ///< �ʐM�ΐ�

  BTL_COMPETITOR_MAX,

}BtlCompetitor;

//--------------------------------------------------------------
/**
 *  �ʐM�^�C�v
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMM_NONE,  ///< �ʐM���Ȃ�
  BTL_COMM_DS,    ///< DS�����ʐM
  BTL_COMM_WIFI,  ///< Wi-Fi�ʐM

}BtlCommMode;


//--------------------------------------------------------------
/**
 *  �V��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_WEATHER_NONE = 0,   ///< �V��Ȃ�

  BTL_WEATHER_SHINE,    ///< �͂�
  BTL_WEATHER_RAIN,     ///< ����
  BTL_WEATHER_SNOW,     ///< �����
  BTL_WEATHER_SAND,     ///< ���Ȃ��炵
  BTL_WEATHER_MIST,     ///< ����

  BTL_WEATHER_MAX,

}BtlWeather;

//--------------------------------------------------------------
/**
 *  ���s�R�[�h
 */
//--------------------------------------------------------------
typedef enum {

  BTL_RESULT_LOSE,        ///< ������
  BTL_RESULT_WIN,         ///< ������
  BTL_RESULT_DRAW,        ///< �Ђ��킯
  BTL_RESULT_RUN,         ///< ������
  BTL_RESULT_RUN_ENEMY,   ///< ���肪�������i�쐶�̂݁j
  BTL_RESULT_CAPTURE,     ///< �߂܂����i�쐶�̂݁j

  BTL_RESULT_MAX,

}BtlResult;

typedef enum{
  BTL_CLIENT_PLAYER,
  BTL_CLIENT_PARTNER,
  BTL_CLIENT_ENEMY1,
  BTL_CLIENT_ENEMY2,
  BTL_CLIENT_NUM,
}BTL_CLIENT_ID;

//-----------------------------------------------------------------------------------
/**
 * �t�B�[���h�̏�Ԃ��猈�肳���o�g���V�`���G�[�V�����f�[�^
 */
//-----------------------------------------------------------------------------------
typedef struct {

  BtlBgType   bgType;
  BtlBgAttr   bgAttr;
  BtlWeather  weather;

  TIMEZONE    timeZone;
  u8          season;
}BTL_FIELD_SITUATION;

//-----------------------------------------------------------------------------------
/**
 * �g���[�i�[�f�[�^�\����
 */
//-----------------------------------------------------------------------------------
typedef struct {
  u32     tr_id;
  u32     tr_type;          //�g���[�i�[����
  u32     ai_bit;
  u16     use_item[4];      //�g�p����

  STRBUF*   name; //�g���[�i�[��

  //�o�g���T�u�E�F�C�� Wifi-DL�g���[�i�[�Ƃ̑ΐ펞�ɂ̂ݕK�v
  PMS_DATA  win_word;   //�퓬�I�����������b�Z�[�W
  PMS_DATA  lose_word;  //�퓬�I�����������b�Z�[�W

}BSP_TRAINER_DATA;

//-----------------------------------------------------------------------------------
/**
 *  �o�g���Z�b�g�A�b�v�p�����[�^
 */
//-----------------------------------------------------------------------------------
typedef struct {

  //�o�g�����[��
  BtlCompetitor   competitor;
  BtlRule         rule;

  //�t�B�[���h�̏�Ԃ��猈�肳���o�g���V�`���G�[�V�����f�[�^
  BTL_FIELD_SITUATION   fieldSituation;
  u16             musicDefault;   ///< �f�t�H���g����BGM�i���o�[
  u16             musicPinch;     ///< �s���`����BGM�i���o�[

  //�ʐM�f�[�^
  GFL_NETHANDLE*  netHandle;
  BtlCommMode     commMode;
  u8              commPos;    ///< �ʐM�ΐ�Ȃ玩���̗����ʒu�i��ʐM���͖����j
  u8              netID;      ///< NetID
  u8              multiMode;  ///< �_�u���̎��A�P���ƃ}���`�o�g���B
  u8              fExpDisable;///< TRUE���ƌo���l�擾���s��Ȃ�

  //�ΐ�f�[�^
  POKEPARTY*      partyPlayer;  ///< �v���C���[�̃p�[�e�B
  POKEPARTY*      partyPartner; ///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
  POKEPARTY*      partyEnemy1;  ///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
  POKEPARTY*      partyEnemy2;  ///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

  BSP_TRAINER_DATA*  tr_data[BTL_CLIENT_NUM]; //�g���[�i�[�f�[�^

  //�Z�[�u�f�[�^�n
  const MYSTATUS*   statusPlayer; ///< �v���C���[�̃X�e�[�^�X
  const CONFIG*     configData;   ///< �R���t�B�O�f�[�^
  MYITEM*           itemData;     ///< �A�C�e���f�[�^
  BAG_CURSOR*       bagCursor;    ///< �o�b�O�J�[�\���f�[�^
  ZUKAN_SAVEDATA*   zukanData;    ///< �}�Ӄf�[�^


  //----- �ȉ��A�o�g���̌��ʊi�[�p�����[�^ ----

  BtlResult   result;           ///< ���s����
  u8          capturedPokeIdx;  ///< �ߊl�����|�P�����̃����o�[���C���f�b�N�X�ipartyEnemy1 ���j

  u8*         recBuffer;        ///< �^��f�[�^������

}BATTLE_SETUP_PARAM;


//--------------------------------------------------------------
/**
 *  Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA BtlProcData;

// �I�[�o�[���C���W���[���錾
FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------------
/**
 *  NetFunc Table
 */
//--------------------------------------------------------------
extern const NetRecvFuncTable BtlRecvFuncTable[];
enum {
  BTL_NETFUNCTBL_ELEMS = 6,
};


#endif
