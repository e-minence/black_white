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

#ifdef TEST_IN_DOS
#include "ds_types.h"
#endif

#include <net.h>
#include <procsys.h>

#include "poke_tool/pokeparty.h"
#include "tr_tool/trno_def.h"
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"

//--------------------------------------------------------------
/**
 *  �쓮�^�C�v
 */
//--------------------------------------------------------------
typedef enum {

  BTL_ENGINE_ALONE,         ///< �X�^���h�A����
  BTL_ENGINE_COMM_PARENT,   ///< �ʐM�i�e�j
  BTL_ENGINE_COMM_CHILD,    ///< �ʐM�i�q�j

}BtlEngineType;

//--------------------------------------------------------------
/**
 *  �ΐ탋�[��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_RULE_SINGLE,    ///< �V���O��
  BTL_RULE_DOUBLE,    ///< �_�u��
  BTL_RULE_TRIPLE,    ///< �g���v��

}BtlRule;

//--------------------------------------------------------------
/**
 *  �ΐ푊��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMPETITOR_WILD,    ///< �쐶
  BTL_COMPETITOR_TRAINER,   ///< �Q�[�����g���[�i�[
  BTL_COMPETITOR_COMM,    ///< �ʐM�ΐ�

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
 *  �n�`�i@@@ �܂��b��I�Ȃ��̂ł��B�s���ȏꍇ�AGRASS���w�肵�Ă��������j
 */
//--------------------------------------------------------------
typedef enum {
  BTL_LANDFORM_GRASS,   ///< ���ނ�
  BTL_LANDFORM_SAND,    ///< ���n
  BTL_LANDFORM_SEA,     ///< �C
  BTL_LANDFORM_RIVER,   ///< ��
  BTL_LANDFORM_SNOW,    ///< �ጴ
  BTL_LANDFORM_CAVE,    ///< ���A
  BTL_LANDFORM_ROCK,    ///< ���
  BTL_LANDFORM_ROOM,    ///< ����

  BTL_LANDFORM_MAX,
}BtlLandForm;


//-----------------------------------------------------------------------------------
/**
 *  �n�`
 */
//-----------------------------------------------------------------------------------
typedef struct {

  BtlEngineType   engine;
  BtlCompetitor   competitor;
  BtlRule         rule;
  BtlLandForm     landForm;

  GFL_NETHANDLE*  netHandle;
  BtlCommMode     commMode;
  u8              commPos;    ///< �ʐM�ΐ�Ȃ玩���̗����ʒu�i��ʐM���͖����j
  u8              netID;      ///< NetID
  u8              multiMode;  ///< �_�u���̎��A�P���ƃ}���`�o�g���B

  POKEPARTY*      partyPlayer;  ///< �v���C���[�̃p�[�e�B
  POKEPARTY*      partyPartner; ///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
  POKEPARTY*      partyEnemy1;  ///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
  POKEPARTY*      partyEnemy2;  ///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

  const MYSTATUS*   statusPlayer; ///< �v���C���[�̃X�e�[�^�X
  MYITEM*           itemData;     ///< �A�C�e���f�[�^
  TrainerID         trID;         ///<�ΐ푊��g���[�i�[ID�i7/31ROM�Ńg���[�i�[�G���J�E���g���������邽�߂̎b��j

  u16       musicDefault;   ///< �f�t�H���g����BGM�i���o�[
  u16       musicPinch;     ///< �s���`����BGM�i���o�[

}BATTLE_SETUP_PARAM;


//--------------------------------------------------------------
/**
 *  Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA BtlProcData;


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
