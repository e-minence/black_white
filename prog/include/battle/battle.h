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

#include "..\pm_define.h"
#include "system/timezone.h"
#include "system/pms_data.h"
#include "field/field_light_status.h"
#include "poke_tool/pokeparty.h"
#include "tr_tool/trno_def.h"
#include "savedata/zukan_savedata.h"
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "savedata/record.h"
#include "gamesystem/comm_player_support.h"
#include "gamesystem/playerwork.h"
#include "item/shooter_item.h"

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

  BTL_COMPETITOR_DEMO_CAPTURE,  ///< �ߊl�f��

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

}BtlCommMode_tag;

typedef u8 BtlCommMode;

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

  BTL_WEATHER_MAX,

}BtlWeather_tag;

typedef u8 BtlWeather;

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
  BTL_RESULT_COMM_ERROR,  ///< �ʐM�G���[�ɂ��

  BTL_RESULT_MAX,

}BtlResult;

typedef enum{
  BTL_CLIENT_PLAYER,
  BTL_CLIENT_ENEMY1,
  BTL_CLIENT_PARTNER,
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
  u8          season;

  // ���C�g�ݒ�p�p�����[�^
  ZONEID      zoneID;
  u8          hour;
  u8          minute;
}BTL_FIELD_SITUATION;

//-----------------------------------------------------------------------------------
/**
 * �t�B�[���h����o�g���Ɉ����n���ׂ��t���O�Q
 */
//-----------------------------------------------------------------------------------
typedef enum {
  //�t���O����16��������Ƃ���BATTLE_SETUP_PARAM->btl_status_flag�̌^��u16����g�����Ă�������
  BTL_STATUS_FLAG_FISHING = 0x0001,       ///<�ނ�퓬�Ȃ�TRUE
  BTL_STATUS_FLAG_PCNAME_OPEN = 0x0002,   ///<�p�\�R���̖��O���I�[�v������Ă��邩�H
  BTL_STATUS_FLAG_LEGEND = 0x0004,        ///<�`�����|�P�����Ƃ̃G���J�E���g�Ȃ�TRUE(Btl�G���J�E���g���b�Z�[�W�ω�)
  BTL_STATUS_FLAG_MOVE_POKE = 0x0008,     ///<�ړ��|�P�����Ƃ̃G���J�E���g�Ȃ�TRUE
  BTL_STATUS_FLAG_SYMBOL = 0x0010,        ///<PDW�A���̃V���{���G���J�E���g�Ȃ�TRUE
  BTL_STATUS_FLAG_HIGH_LV_ENC = 0x0020,   ///<�쐶�n�C���x���G���J�E���g�Ȃ�TRUE
  BTL_STATUS_FLAG_BOXFULL = 0x0040,       ///<�莝���E�{�b�N�X�����t�ł����ߊl�ł��Ȃ�����TRUE
  BTL_STATUS_FLAG_WILD_TALK = 0x0080,     ///<�b�������ł̖쐶��̎���TRUE(Btl�G���J�E���g���b�Z�[�W�ω�)
  BTL_STATUS_FLAG_NO_LOSE = 0x0100,       ///<�Q�[���I�[�o�[�ɂȂ�Ȃ��o�g���Ȃ�TRUE
  BTL_STATUS_FLAG_LEGEND_EX = 0x0200,     ///<�V�i���I���X�g�̃V���E�����p�i�o���l�Ȃ��E�|�������b�Z�[�W�\���Ȃ��j
  BTL_STATUS_FLAG_CAMERA_WCS = 0x0400,    ///< WCS�p�J�������[�h
  BTL_STATUS_FLAG_CAMERA_OFF = 0x0800,    ///< �J��������I�t
  BTL_STATUS_FLAG_CONFIG_SHARE = 0x1000,  ///< �R���t�B�O�ݒ��e�@�̂��̂ōs��
}BTL_STATUS_FLAG;

//-----------------------------------------------------------------------------------
/**
 * �g���[�i�[�f�[�^�\����
 */
//-----------------------------------------------------------------------------------
enum {
  BSP_TRAINERDATA_ITEM_MAX = 4,
  BSP_TRAINERID_SUBWAY = 0xFFFF,
};

typedef struct {
  u32     tr_id;
  u32     tr_type;          //�g���[�i�[����
  u32     ai_bit;
  u16     use_item[BSP_TRAINERDATA_ITEM_MAX];      //�g�p����

  STRBUF*   name; //�g���[�i�[��

  //�o�g���T�u�E�F�C�� Wifi-DL�g���[�i�[�Ƃ̑ΐ펞�ɂ̂ݕK�v
  PMS_DATA  win_word;   //�퓬�I�����������b�Z�[�W
  PMS_DATA  lose_word;  //�퓬�I�����������b�Z�[�W

}BSP_TRAINER_DATA;

//--------------------------------------------------------------
/**
 *  �}���`���[�h
 */
//--------------------------------------------------------------

typedef enum {

  BTL_MULTIMODE_NONE = 0, ///< ��}���`
  BTL_MULTIMODE_PP_PP,    ///< �v���C���[�Q�l vs �v���C���[�Q�l�i�ʐM�}���`�j
  BTL_MULTIMODE_PP_AA,    ///< �v���C���[�Q�l vs AI�Q�l�iAI�ʐM�}���`�j
  BTL_MULTIMODE_PA_AA,    ///< �v���C���[�P�l��AI�P�l vs AI�Q�l�iAI�}���`�j
  BTL_MULTIMODE_P_AA,     ///< �v���C���[�P�l VS AI�Q�l�iAI�^�b�O�j

}BtlMultiMode_tag;

typedef u8 BtlMultiMode;

//--------------------------------------------------------------
/**
 *  �o�g����|�P������ԃR�[�h
 */
//--------------------------------------------------------------

typedef enum {

  BTL_POKESTATE_NORMAL = 0, ///< �ُ�Ȃ�
  BTL_POKESTATE_SICK,       ///< ��Ԉُ�
  BTL_POKESTATE_DEAD,       ///< �Ђ�

}BtlPokeStatCode_tag;

typedef u8 BtlPokeStatCode;


//--------------------------------------------------------------
/**
 *  �f�o�b�O�t���O
 */
//--------------------------------------------------------------
typedef enum {

  BTL_DEBUGFLAG_MUST_TUIKA = 0, ///< �ǉ����ʂ̔�������100���ɂ���
  BTL_DEBUGFLAG_MUST_TOKUSEI,   ///< �ꕔ�Ƃ������̌��ʔ�������100���ɂ���
  BTL_DEBUGFLAG_MUST_ITEM,      ///< �ꕔ�A�C�e���̌��ʔ�������100���ɂ���
  BTL_DEBUGFLAG_MUST_CRITICAL,  ///< �Ō����U�̃N���e�B�J����������100���ɂ���
  BTL_DEBUGFLAG_HP_CONST,       ///< HP����Ȃ�
  BTL_DEBUGFLAG_PP_CONST,       ///< PP����Ȃ�
  BTL_DEBUGFLAG_HIT100PER,      ///< �����������100��
  BTL_DEBUGFLAG_DMG_RAND_OFF,   ///< �_���[�W�̃����_���␳���I�t
  BTL_DEBUGFLAG_SKIP_BTLIN,     ///< ����G�t�F�N�g�X�L�b�v�i��ʐM���̂݁j
  BTL_DEBUGFLAG_AI_CTRL,        ///< AI���N�������Ƀv���C���[�����삷��
  BTL_DEBUGFLAG_SHOOTER_MODE,   ///< ��ʐM�����V���[�^�[���[�h�Ńo�b�O��ʂ��N��

  BTL_DEBUGFLAG_MAX,

}BtlDebugFlag;


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
  u16             musicWin;       ///< ��������BGM�i���o�[

  //�ʐM�f�[�^
  GFL_NETHANDLE*  netHandle;
  BtlCommMode     commMode;
  u8              commPos;     ///< �ʐM�ΐ�Ȃ玩���̗����ʒu�i��ʐM���͏��0���w��j
  BtlMultiMode    multiMode;   ///< �}���`�ΐ탂�[�h�i�_�u���o�g�����̂ݗL���j
  u8              fRecordPlay; ///< TRUE���Ƙ^��f�[�^�Đ����[�h

  // [io] �p�[�e�B�f�[�^�E�ΐ푊��X�e�[�^�X
  //�i�ʐM�o�g����A�^��f�[�^�����̂��߂ɑ���v���C���[�ȊO�̃f�[�^���i�[�����j
  POKEPARTY*        party[ BTL_CLIENT_NUM ];
  MYSTATUS*         playerStatus[ BTL_CLIENT_NUM ];

  // �g���[�i�[�f�[�^
  BSP_TRAINER_DATA*  tr_data[BTL_CLIENT_NUM];

  // �Z�[�u�f�[�^�n
  GAMEDATA*         gameData;     ///< GameData
  const CONFIG*     configData;   ///< �R���t�B�O�f�[�^
  MYITEM*           itemData;     ///< �A�C�e���f�[�^
  BAG_CURSOR*       bagCursor;    ///< �o�b�O�J�[�\���f�[�^
  ZUKAN_SAVEDATA*   zukanData;    ///< �}�Ӄf�[�^
  RECORD*           recordData;   ///< ���R�[�h�f�[�^

  // �Q�[���f�[�^�n
  COMM_PLAYER_SUPPORT*  commSupport;  ///< �ʐM�v���C���[�T�|�[�g

  // �������Ԑݒ�
  u16         LimitTimeGame;        ///< �����������ԁi�b�j         [ 0 = ������ ]
  u16         LimitTimeCommand;     ///< �R�}���h�I�𐧌����ԁi�b�j [ 0 = ������ ]

  // �e��X�e�[�^�X�t���O
  SHOOTER_ITEM_BIT_WORK  shooterBitWork;  ///< �V���[�^�[�g�p�����p�����[�^
  u8          badgeCount;       ///< �v���C���[�擾�ς݃o�b�W��
  u16         btl_status_flag;  ///< �e��X�e�[�^�X�t���O(��`�l BTL_STATUS_FLAG�^)
  u16         commNetIDBit;     ///< �ʐM�ΐ펞��NetIDBit�t���O

  // �f�o�b�O�n
  u16         DebugFlagBit;          ///< �e��f�o�b�O�t���O


  //----- �ȉ��A�o�g���̌��ʊi�[�p�����[�^ ----
  int         getMoney;         ///< ��ɓ��ꂽ��������
  BtlResult   result;           ///< ���s����
  u8          capturedPokeIdx;  ///< �ߊl�����|�P�����̃����o�[���C���f�b�N�X�ipartyEnemy1 ���j
  u8          commServerVer;    ///< �ʐM�ΐ펞�̃T�[�o�o�[�W�����i�ڑ������}�V�����A�Ō�o�[�W�����j

  u8*         recBuffer;        ///< �^��f�[�^������
  u32         recDataSize;      ///< �^��f�[�^�T�C�Y
  GFL_STD_RandContext  recRandContext;    ///< �^��f�[�^�p�����R���e�L�X�g

  // -- �o�g�����茋�� --
  u16  HPSum;       //�����c��|�P������HP���a
  u8 TurnNum;       //���������^�[����
  u8 PokeChgNum;    //����
  u8 VoidAtcNum;    //���ʂ��Ȃ��Z���o������
  u8 WeakAtcNum;    //�΂���̋Z���o������
  u8 ResistAtcNum;  //���܂ЂƂ̋Z���o������
  u8 VoidNum;       //���ʂ��Ȃ��Z���󂯂���
  u8 ResistNum;     //���܂ЂƂ̋Z���󂯂���
  u8 WinTrainerNum; //�|�����g���[�i�[��
  u8 WinPokeNum;    //�|�����|�P������
  u8 LosePokeNum;   //�|���ꂽ�|�P������
  u8 UseWazaNum;    //�g�p�����Z�̐�

  u8 cmdIllegalFlag      : 1;  // �R�}���h�s���`�F�b�N�t���O�i�q�@�̂ݑ���B�e�@�͏��FALSE�j
  u8 recPlayCompleteFlag : 1;  // �^��Đ����[�h�ōŌ�܂ōĐ��ł�����TRUE�B����ȊO��FALSE
  u8 WifiBadNameFlag     : 1;  // Wifi�őΐ푊�肪�s������������TRUE�B����ȊO��FALSE

  u8 fightPokeIndex[ TEMOTI_POKEMAX ];  // �퓬�ɏo���|�P�����̃p�[�e�B��Index�Ɠ����ʒu��TRUE�ɂȂ�i����ȊO��FALSE�j

  // �Ђ񎀁E��Ԉُ�Ȃǌ��ʊi�[
  BtlPokeStatCode  party_state[ BTL_CLIENT_NUM ][ TEMOTI_POKEMAX ];
  u32              restHPRatio[ BTL_CLIENT_NUM ];

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
  BTL_NETFUNCTBL_ELEMS = 9,
};


#endif
