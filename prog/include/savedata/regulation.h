//============================================================================================
/**
 * @file	regulation.h
 * @brief	�o�g�����M�����[�V�����f�[�^�A�N�Z�X�p�w�b�_
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================
#pragma once


#include <gflib.h>
#include "item/shooter_item.h"
#include "poke_tool/pokeparty.h" //RegulationView_SetEazy �̂��߂����ɌĂ�ł܂��d
#include "net/dreamworld_netdata.h"
//============================================================================================
//============================================================================================


#define WIFI_PLAYER_TIX_CUPNAME_MOJINUM (72) //�f�W�^���I��ؑ������񕶎���
#define REGULATION_CUP_MAX (7)  //regulation.rb��regulation_cup.gmm�ƘA��
#define REGULATION_RULE_MAX (6) //regulation.rb��regulation_rule.gmm�ƘA��

//----------------------------------------------------------
/**
 * @brief	�o�g�����M�����[�V�����f�[�^�^��`
 */
//----------------------------------------------------------


typedef enum {
  REGULATION_CUPNAME_SIZE = 12,      ///< �J�b�v���̒��� 12����24�o�C�g +EOM2byte
  REGULATION_RULENAME_SIZE = 12,      ///< ���[�����̒��� 12����24�o�C�g +EOM2byte
  REGULATION_VS_TIME = 99,        ///< �ΐ펞��MAX 99��
  REGULATION_COMMAND_TIME = 99,   ///< �R�}���h���͎��� 99�b
  REGULATION_SHOW_POKE_TIME_MAX = 99,   ///< ������������ 99�b
  REGULATION_SHOW_POKE_TIME_NONE = 0,   ///< ��������������
  REGULATION_AGE_NONE = 0,   ///< �N�����

} REGULATION_ETC_PARAM_TYPE;

typedef enum  {
  REGULATION_CUPNUM,   ///< �J�b�v�ԍ�
  REGULATION_RULENUM,  ///< ���[���ԍ�
  REGULATION_NUM_LO, ///<    #�Q��������
  REGULATION_NUM_HI, ///<    #�Q�������
  REGULATION_LEVEL,  ///<    #�Q�����x��
  REGULATION_LEVEL_RANGE, ///<    #���x���͈�
  REGULATION_LEVEL_TOTAL, ///<    #���x�����v
  REGULATION_BOTH_POKE, ///<    #�����|�P����
  REGULATION_BOTH_ITEM,  ///<    #�����ǂ���
  REGULATION_VETO_POKE_BIT,  ///<    #�Q���֎~�|�P����
  REGULATION_VETO_ITEM, ///<    #�������݋֎~����
  REGULATION_MUST_POKE,   ///<    #�K�{�|�P����
  REGULATION_MUST_POKE_FORM,  ///<  #�K�{�|�P�����t�H����
  REGULATION_SHOOTER,    ///<    #�V���[�^�[
  REGULATION_VETO_SHOOTER_ITEM, ///< �֎~����
  REGULATION_TIME_VS,     ///<    #�ΐ펞�� ���P�ʃo�g���ɓn���Ƃ��ɂ͕b�ɕϊ����Ă�������
  REGULATION_TIME_COMMAND, ///<    #���͎���  ���P�ʃo�g���ɓn���Ƃ��ɂ͕b�ɕϊ����Ă�������
  REGULATION_NICKNAME, ///<    #�j�b�N�l�[���\��
  REGULATION_AGE_LO,  ///<    #�N����ȏ�
  REGULATION_AGE_HI,  ///<    #�N����ȉ�
  REGULATION_SHOW_POKE,  ///<    #�|�P������������ONOFF
  REGULATION_SHOW_POKE_TIME,  ///<    #�|�P�������������������� 0�͖�����
  REGULATION_BATTLETYPE,  ///<  �o�g���^�C�v
  REGULATION_BTLCOUNT,  ///<  �퓬�K���
} REGULATION_PARAM_TYPE;

typedef enum  {
  REGULATION_LEVEL_RANGE_NORMAL,   ///<�Ȃ�
  REGULATION_LEVEL_RANGE_OVER,     ///<�ȏ�
  REGULATION_LEVEL_RANGE_LESS,     ///<�ȉ�
  REGULATION_LEVEL_RANGE_DRAG_DOWN,  ///<�ȏ�␳   �␳���x���ȏ�Ȃ�Ε␳���x���ɂ���i50�␳��51�Ȃ�50�j
  REGULATION_LEVEL_RANGE_SAME,         ///<�S�␳
  REGULATION_LEVEL_RANGE_PULL_UP,     ///<�ȉ��␳  �␳���x���ȉ��Ȃ�Ε␳���x���ɂ���(50�␳��49�Ȃ�50)
  REGULATION_LEVEL_RANGE_MAX   //
} REGULATION_LEVEL_RANGE_TYPE;

typedef enum  {
  REGULATION_BATTLE_SINGLE,   ///<�V���O��
  REGULATION_BATTLE_DOUBLE,   ///<�_�u��
  REGULATION_BATTLE_TRIPLE,   ///<�g���v��
  REGULATION_BATTLE_ROTATION,   ///<���[�e�[�V����
  REGULATION_BATTLE_MULTI,   ///<�}���`
  REGULATION_BATTLE_MAX   //
} REGULATION_BATTLE_TYPE;

typedef enum {
  REGULATION_SHOOTER_INVALID,   ///<����(�Ȃ�)
  REGULATION_SHOOTER_VALID,     ///<�L��(����)
  REGULATION_SHOOTER_MANUAL,    ///<�}�j���A��
}REGULATION_SHOOTER_TYPE;



typedef enum  {
  REGULATION_CARD_CUPNAME,     ///< �J�b�v��
  REGULATION_CARD_ROMVER,      ///< �J�Z�b�g�o�[�W�����F
  REGULATION_CARD_CUPNO,       ///< ���No.
  REGULATION_CARD_LANGCODE,    ///< ���R�[�h�F
  REGULATION_CARD_START_YEAR,  ///< �J�n�N�F00-99
  REGULATION_CARD_START_MONTH, ///< �J�n���F01-12
  REGULATION_CARD_START_DAY,   ///< �J�n���F01-31
  REGULATION_CARD_END_YEAR,    ///< �I���N�F00-99
  REGULATION_CARD_END_MONTH,   ///< �I�����F01-12
  REGULATION_CARD_END_DAY,     ///< �I�����F01-31
  REGULATION_CARD_STATUS,      ///< ����ԁFnet/dreamworld_netdata.h��DREAM_WORLD_MATCHUP_TYPE�̒l
  REGULATION_CARD_BGM,         ///< ���Ŏg�p����ȁi�V�[�P���X�ԍ������̂܂ܓ����Ă���킯�ł͂Ȃ��j
  REGULATION_CARD_SAMEMATCH,         ///< �����l�Ƃ̑ΐ�������邩�ǂ��� TRUE�ŋ���
} REGULATION_CARD_PARAM_TYPE;

typedef enum {  
  REGULATION_VIEW_MONS_NO,
  REGULATION_VIEW_FROM_NO,
  REGULATION_VIEW_ITEM_FLAG,
  REGULATION_VIEW_SEX,

} REGULATION_VIEW_PARAM_TYPE;

typedef enum
{ 
  REGULATION_CARD_BGM_TRAINER, //SEQ_BGM_VS_TRAINER_WIFI
  REGULATION_CARD_BGM_WCS,  //SEQ_BGM_WCS
} REGULATION_CARD_BGM_TYPE;  //wifi����live���ł����g���܂���

typedef enum {
  REGULATION_CARD_TYPE_WIFI,   //WIFI���Ŏg�p
  REGULATION_CARD_TYPE_LIVE,   //���C�u���Ŏg�p

  REGULATION_CARD_TYPE_MAX,    //.c�Ŏg�p
} REGULATION_CARD_TYPE;


//----------------------------------------------------------
/**
 * @brief	�o�g�����M�����[�V�����f�[�^�^��`  fushigi_data.h�Q�Ƃ̈׊O�����J��
 */
//----------------------------------------------------------

#define REG_POKENUM_MAX_BYTE (656/8)  //##���̂��炢�ɑ����邩�� �W�Q�o�C�g
#define REG_ITEMNUM_MAX_BYTE (608/8)  //##���̂��炢�ɂӂ��邩��


typedef struct{
  u8 cupNo;  ///< ���J�b�v�ԍ�
  u8 ruleNo; ///< ���[���ԍ�
  u8 NUM_LO; ///<    #�Q��������
  u8 NUM_HI; ///<    #�Q�������
  u8 LEVEL;  ///<    #�Q�����x��
  u8 LEVEL_RANGE; ///<    #���x���͈�
  u16 LEVEL_TOTAL; ///<    #���x�����v
  u8 BOTH_POKE; ///<    #�����|�P����
  u8 BOTH_ITEM;  ///<    #�����ǂ���             60
  u8 VETO_POKE_BIT[REG_POKENUM_MAX_BYTE];  ///<    #�Q���֎~�|�P���� 142
  u8 VETO_ITEM[REG_ITEMNUM_MAX_BYTE]; ///<    #�������݋֎~���� 218
  u16 MUST_POKE;   ///<    #�K�{�|�P����
  u8 MUST_POKE_FORM;   ///<    #�K�{�|�P�����t�H����
  u8 SHOOTER;    ///<    #�V���[�^�[
  u8 VETO_SHOOTER_ITEM[SHOOTER_ITEM_BIT_TBL_MAX]; ///<    #�V���[�^�[�֎~���� 7byte
  u8 TIME_VS;     ///<    #�ΐ펞�� ���P�ʃo�g���ɓn���Ƃ��ɂ͕b�ɕϊ����Ă�������
  u8 TIME_COMMAND; ///<    #���͎���  ���P�ʃo�g���ɓn���Ƃ��͕b�ɕϊ����Ă�������
  u8 NICKNAME; ///<    #�j�b�N�l�[���\��
  u8 AGE_LO;  ///<    #�N����ȏ�
  u8 AGE_HI;  ///<    #�N����ȉ�
  u8 SHOW_POKE;  ///<    #�|�P������������
  u8 SHOW_POKE_TIME;  ///<�|�P����������������
  u8 BATTLE_TYPE;    ///< �o�g���^�C�v
  u8 BTL_COUNT;     ///< �퓬�K��� 238 
} REGULATION;


//�I���
typedef struct {
  REGULATION regulation_buff;
  u32 ver;//�J�Z�b�g�o�[�W�����F
  STRCODE cupname[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE];  //���i72�����{EOM�j
  short no;//���No.
  char code;//���R�[�h�F
  char start_year;//�J�n�N�F00-99
  char start_month;//�J�n���F01-12
  char start_day;//�J�n���F01-31
  char end_year;//�I���N�F00-99
  char end_month;//�I�����F01-12
  char end_day;//�I�����F01-31
  char status;  //����ԁF net/dreamworld_netdata.h��DREAM_WORLD_MATCHUP_TYPE�̒l
  u8   bgm_no;   //���Ŏg�p����BGM REGULATION_CARD_BGM_TYPE�񋓎Q��
  u8   same_match;//�����l�Ƃ̑ΐ�������邩�ǂ���TRUE�ŋ�����
  u16  crc;  //����������
} REGULATION_CARDDATA;

//�I��؂̌����ڏ��
typedef struct _REGULATION_VIEWDATA REGULATION_VIEWDATA;

//�Z�[�u�f�[�^
typedef struct _REGULATION_SAVEDATA REGULATION_SAVEDATA;

//============================================================================================
//============================================================================================
//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e����ʐM�Ŏg�p����֐�
//----------------------------------------------------------
extern int RegulationSaveData_GetWorkSize(void);
extern int Regulation_GetWorkSize(void);
extern int RegulationData_GetWorkSize(void);
extern REGULATION * Regulation_AllocWork(HEAPID heapID);
extern void Regulation_Copy(const REGULATION * from, REGULATION * to);
extern int Regulation_Cmp(const REGULATION* pCmp1,const REGULATION* pCmp2);

//----------------------------------------------------------
//	REGULATION����̂��߂̊֐�
//----------------------------------------------------------
extern void Regulation_Init(REGULATION * my);

//�擾
extern REGULATION* RegulationData_GetRegulation(REGULATION_CARDDATA *pRegData);
extern void RegulationData_SetRegulation(REGULATION_CARDDATA *pRegData, const REGULATION* pReg);

//�J�b�v��
extern void Regulation_GetCupName(const REGULATION* pReg,STRBUF* pReturnCupName);
extern STRBUF* Regulation_CreateCupName(const REGULATION* pReg, HEAPID heapID);

//���[����
extern void Regulation_GetRuleName(const REGULATION* pReg,STRBUF* pReturnRuleName);
extern STRBUF* Regulation_CreateRuleName(const REGULATION* pReg, HEAPID heapID);

extern int Regulation_GetParam(const REGULATION* pReg, REGULATION_PARAM_TYPE type);
extern void Regulation_SetParam(REGULATION * pReg, REGULATION_PARAM_TYPE type, int param);

extern BOOL Regulation_CheckParamBit(const REGULATION* pReg, REGULATION_PARAM_TYPE type, int no);

extern void Regulation_GetShooterItem( const REGULATION* cp_reg, SHOOTER_ITEM_BIT_WORK *p_wk );
//----------------------------------------------------------
//	REGULATION_CARDDATA����̂��߂̊֐� �f�W�^���I��؂̎�
//----------------------------------------------------------
extern void RegulationData_Init(REGULATION_CARDDATA * my);

//�擾
extern REGULATION_CARDDATA* RegulationSaveData_GetRegulationCard( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type );
extern void RegulationSaveData_SetRegulation(REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type, const REGULATION_CARDDATA* pReg);

//�J�b�v��
extern void Regulation_GetCardCupName(const REGULATION_CARDDATA* pReg,STRBUF* pReturnCupName);
extern STRBUF* Regulation_CreateCardCupName(const REGULATION_CARDDATA* pReg, HEAPID heapID);
extern const STRCODE *Regulation_GetCardCupNamePointer( const REGULATION_CARDDATA* pReg );

extern int Regulation_GetCardParam(const REGULATION_CARDDATA* pReg, REGULATION_CARD_PARAM_TYPE type);
extern void Regulation_SetCardParam(REGULATION_CARDDATA * pReg, REGULATION_CARD_PARAM_TYPE type, int param);

extern BOOL Regulation_CheckCrc( const REGULATION_CARDDATA* pReg );

//----------------------------------------------------------
//	REGULATION_VIEWDATA����̂��߂̊֐� �f�W�^���I��؂̌����ځi�o�^�|�P�����Ƃ��j
//----------------------------------------------------------
extern void RegulationView_Init(REGULATION_VIEWDATA * my);

//���[�N�擾
extern REGULATION_VIEWDATA* RegulationSaveData_GetRegulationView( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type );

//�f�[�^�擾�E�ݒ�
extern int RegulationView_GetParam( const REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx );
extern void RegulationView_SetParam( REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx, int param );
extern void RegulationView_SetEazy( REGULATION_VIEWDATA *pView, const POKEPARTY *cp_party );


//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern void RegulationSaveData_Init(REGULATION_SAVEDATA * my);
extern REGULATION_SAVEDATA* SaveData_GetRegulationSaveData(SAVE_CONTROL_WORK* pSave);

//----------------------------------------------------------
//	�f�o�b�O�p
//----------------------------------------------------------
#ifdef PM_DEBUG
extern void Regulation_SetDebugData( REGULATION_CARDDATA* pReg );
extern void Regulation_PrintDebug( const REGULATION_CARDDATA* cpReg );
#else
#define Regulation_SetDebugData(x) /*  */
#define Regulation_PrintDebug(x)  /*  */
#endif //PM_DEBUG
