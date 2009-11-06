//============================================================================================
/**
 * @file  config.h
 * @brief �ݒ�f�[�^�A�N�Z�X�p�w�b�_
 * @author  tamada GAME FREAK inc.
 * @date  2006.01.26
 */
//============================================================================================
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "savedata/save_control.h"

#define OLDCONFIG_ON

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief �ݒ�f�[�^�^��`
 */
//----------------------------------------------------------
typedef struct _CONFIG CONFIG;

//---------------------------------------------------------------------------
/**
 * @brief �ݒ�f�[�^�\���̒�`
 *
 * ���̍\���̂̃����o�[�ɓ����ȂǁA�����₷�����邽�߂Ɍ��J�����B
 * �������A�����ւ̃A�N�Z�X�͕K���A�N�Z�X�֐��o�R�݂̂ɂ��邱�ƁB
 */
//---------------------------------------------------------------------------
struct _CONFIG {
  u16 msg_speed:4;      ///<MSGSPEED    �l�r�f����̑��x
  u16 sound_mode:2;     ///<SOUNDMODE   �T�E���h�o��
  u16 battle_rule:1;      ///<BATTLERULE    �퓬���[��
  u16 wazaeff_mode:1;     ///<WAZAEFF_MODE  �킴�G�t�F�N�g
#ifdef OLDCONFIG_ON
  u16 input_mode:2;     ///<INPUTMODE   ���̓��[�h
  u16 window_type:5;      ///<WINTYPE     �E�B���h�E�^�C�v
#endif
  u16 moji_mode:1;      ///<MOJIMODE
  u16 wirelesssave_mode:1;      ///<WIRELESSSAVE_MODE   ���C�����X�ŃZ�[�u���͂��ނ��ǂ���
  u16 network_search:1;         ///<NETWORK_SEARCH_MODE �Q�[�����Ƀr�[�R���T�[�`���邩�ǂ���

#ifdef OLDCONFIG_ON
  u16 padding:14;
#else
  u16 padding1:5;

  u16 padding2; //WB�Ńp�f�B���O�����܂��� 2008.12.11(��) matsuda
#endif
};

//----------------------------------------------------------
/**
 */
//----------------------------------------------------------
typedef enum {
  MSGSPEED_SLOW = 0,    ///<���b�Z�[�W�\�����u�������v
  MSGSPEED_NORMAL,      ///<���b�Z�[�W�\�����u�ӂ��v
  MSGSPEED_FAST,        ///<���b�Z�[�W�\�����u�͂₢�v
  MSGSPEED_CONFIG_MAX,  ///<�v���C���[���R���t�B�O�ݒ�ł���ő�l

  MSGSPEED_FAST_EX=MSGSPEED_CONFIG_MAX,  ///<���b�Z�[�W�\���ő��i���[�U�����̒l�ɐݒ肷�邱�Ƃ͏o���Ȃ��B�C�x���g�p�j
}MSGSPEED;

//----------------------------------------------------------
//----------------------------------------------------------
typedef enum {
  WAZAEFF_MODE_ON = 0,    ///<�퓬�A�j�����u�݂�v
  WAZAEFF_MODE_OFF,     ///<�퓬�A�j�����u�݂Ȃ��v
  WAZAEFF_MODE_MAX,     ///<
}WAZAEFF_MODE;

//----------------------------------------------------------
//----------------------------------------------------------
typedef enum {
  BATTLERULE_IREKAE = 0,
  BATTLERULE_KACHINUKI,
  BATTLERULE_MAX,
}BATTLERULE;

//----------------------------------------------------------
//----------------------------------------------------------
typedef enum {
  SOUNDMODE_STEREO = 0, ///<�T�E���h�o��=�u�X�e���I�v
  SOUNDMODE_MONO,     ///<�T�E���h�o��=�u���m�����v
  SOUNDMODE_MAX,      ///<
}SOUNDMODE;

//----------------------------------------------------------
//----------------------------------------------------------
#ifdef OLDCONFIG_ON
typedef enum {  // ��KeyCustmizeData�ƕ��т������ł���K�v������܂�
  INPUTMODE_NORMAL = 0, ///<�{�^���ݒ聁�u�m�[�}���v
  INPUTMODE_START_X,    ///<�{�^���ݒ聁�uSTART=X�v
  INPUTMODE_L_A,      ///<�{�^���ݒ聁�uL=A�v
  INPUTMODE_MAX,      ///<
}INPUTMODE;

//----------------------------------------------------------
//----------------------------------------------------------
typedef enum{
  WINTYPE_01,
  WINTYPE_02,
  WINTYPE_03,
  WINTYPE_04,
  WINTYPE_05,
  WINTYPE_06,
  WINTYPE_07,
  WINTYPE_08,
  WINTYPE_09,
  WINTYPE_10,
  WINTYPE_11,
  WINTYPE_12,
  WINTYPE_13,
  WINTYPE_14,
  WINTYPE_15,
  WINTYPE_16,
  WINTYPE_17,
  WINTYPE_18,
  WINTYPE_19,
  WINTYPE_20,
  WINTYPE_MAX,
}WINTYPE;
#endif
//----------------------------------------------------------
//----------------------------------------------------------
typedef enum{
  MOJIMODE_HIRAGANA,
  MOJIMODE_KANJI,
  MOJIMODE_MAX,
}MOJIMODE;

//----------------------------------------------------------
//----------------------------------------------------------
typedef enum{
  WIRELESSSAVE_OFF,
  WIRELESSSAVE_ON,
  WIRELESSSAVE_MAX,
}WIRELESSSAVE_MODE;

typedef enum{
  NETWORK_SEARCH_OFF,
  NETWORK_SEARCH_ON,
  NETWORK_SEARCH_MAX,
}NETWORK_SEARCH_MODE;

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
//
//--------------------------------------------------------------
extern void CONFIG_SYSTEM_KyeControlTblSetting(void);

//----------------------------------------------------------
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int CONFIG_GetWorkSize(void);
extern CONFIG * CONFIG_AllocWork(u32 heapID);
extern void CONFIG_Copy(const CONFIG * from, CONFIG * to);

//----------------------------------------------------------
//  CONFIG����̂��߂̊֐�
//----------------------------------------------------------
extern void CONFIG_Init(CONFIG * cfg);

/**
 *  @brief  �L�[�R���t�B�O���Q�[���ɔ��f����
 */
#ifdef OLDCONFIG_ON
extern void config_SetKeyConfig(SAVE_CONTROL_WORK* sv,INPUTMODE mode);
static inline void CONFIG_SetKeyConfigFormSave(SAVE_CONTROL_WORK* sv)
{
  config_SetKeyConfig(sv,0);
}
static inline void CONFIG_SetKeyConfig(INPUTMODE mode)
{
  config_SetKeyConfig(NULL,mode);
}
#endif

//  �͂Ȃ��̂͂₳
extern MSGSPEED CONFIG_GetMsgSpeed(const CONFIG * cfg);
extern void CONFIG_SetMsgSpeed(CONFIG * cfg, MSGSPEED speed);

//  ����Ƃ��@�A�j��
extern WAZAEFF_MODE CONFIG_GetWazaEffectMode(const CONFIG * cfg);
extern void CONFIG_SetWazaEffectMode(CONFIG * cfg, WAZAEFF_MODE mode);

//  �������́@���[��
extern BATTLERULE CONFIG_GetBattleRule(const CONFIG * cfg);
extern void CONFIG_SetBattleRule(CONFIG * cfg, BATTLERULE rule);

//  �T�E���h
extern SOUNDMODE CONFIG_GetSoundMode(const CONFIG * cfg);
extern void CONFIG_SetSoundMode(CONFIG * cfg, SOUNDMODE snd_mode);

//  �{�^�����[�h
#ifdef OLDCONFIG_ON
extern INPUTMODE CONFIG_GetInputMode(const CONFIG * cfg);
extern void CONFIG_SetInputMode(CONFIG * cfg, INPUTMODE mode);

//  �E�B���h�E�^�C�v
extern WINTYPE CONFIG_GetWindowType(const CONFIG * cfg);
extern void CONFIG_SetWindowType(CONFIG * cfg, WINTYPE type);
#endif

//  �������[�h
extern MOJIMODE CONFIG_GetMojiMode(const CONFIG * cfg);
extern void CONFIG_SetMojiMode(CONFIG * cfg, MOJIMODE type);

//  ���C�����X�Z�[�u���[�h
extern WIRELESSSAVE_MODE CONFIG_GetWirelessSaveMode(const CONFIG * cfg);
extern void CONFIG_SetWirelessSaveMode(CONFIG * cfg, WIRELESSSAVE_MODE type);

//  �Q�[�����̃l�b�g���[�N�T�[�`���[�h
//  GAMESYS�ł�AlwaysNetFlag�ƂȂ��Ă��܂�
extern NETWORK_SEARCH_MODE CONFIG_GetNetworkSearchMode( const CONFIG * cfg );
extern void CONFIG_SetNetworkSearchMode( CONFIG * cfg, NETWORK_SEARCH_MODE mode );

//----------------------------------------------------------
//  �Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern CONFIG * SaveData_GetConfig(SAVE_CONTROL_WORK * sv);

//----------------------------------------------------------
//  �f�o�b�O�p�f�[�^�����̂��߂̊֐�
//----------------------------------------------------------
//extern void Debug_Config_Make(CONFIG * cfg, const STRCODE * name, int sex);

#endif //__CONFIG_H__
