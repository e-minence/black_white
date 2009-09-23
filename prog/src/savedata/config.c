//============================================================================================
/**
 * @file	config.c
 * @brief	�ݒ�֘A�A�N�Z�X
 * @author	tamada	GAME FREAK Inc.
 * @date	2006.01.27
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/config.h"


//============================================================================================
//============================================================================================
#define	MSG_SPEED_SLOW		( 8 )		///< ���b�Z�[�W���x�F�x��
#define	MSG_SPEED_NORMAL	( 4 )		///< ���b�Z�[�W���x�F����
#define	MSG_SPEED_FAST		( 1 )		///< ���b�Z�[�W���x�F����


//============================================================================================
//============================================================================================


//==============================================================================
//	�f�[�^
//==============================================================================
///�L�[�J�X�^�}�C�Y�f�[�^�e�[�u��
///	�����̃e�[�u���͏풓�ɂ���K�v������܂�
static const GFL_UI_KEY_CUSTOM_TBL KeyCustmizeData[] ={
	//�m�[�}��
	{GFL_UI_KEY_END,0,0},		// �I���f�[�^
	
	//START=X
	{PAD_BUTTON_START,PAD_BUTTON_X,GFL_UI_KEY_COPY},		//START��X������
	{GFL_UI_KEY_END,0,0},		// �I���f�[�^

	//L=A
	{PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//L��A������
	{GFL_UI_KEY_END,0,0},		// �I���f�[�^
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �L�[�J�X�^�}�C�Y�e�[�u���f�[�^�o�^
 *
 * �Q�[�����A��x�����Ăяo���΂悢�ł�
 */
//--------------------------------------------------------------
void CONFIG_SYSTEM_KyeControlTblSetting(void)
{
	GFL_UI_KEY_SetControlModeTbl(KeyCustmizeData);
}


//==============================================================================
//	
//==============================================================================
//----------------------------------------------------------
//---------------------------------------------------------
int CONFIG_GetWorkSize(void)
{
	return sizeof(CONFIG);
}

//----------------------------------------------------------
//---------------------------------------------------------
CONFIG * CONFIG_AllocWork(u32 heapID)
{
	CONFIG * cfg;
	cfg = GFL_HEAP_AllocMemory(heapID, sizeof(CONFIG));
	CONFIG_Init(cfg);
	return cfg;
}

//----------------------------------------------------------
//---------------------------------------------------------
void CONFIG_Copy(const CONFIG * from, CONFIG * to)
{
	GFL_STD_MemCopy(from, to, sizeof(CONFIG));
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_Init(CONFIG * cfg)
{
	GFL_STD_MemFill(cfg, 0, sizeof(CONFIG));
	cfg->msg_speed = MSGSPEED_NORMAL;
	cfg->sound_mode = SOUNDMODE_STEREO;
	cfg->battle_rule = BATTLERULE_IREKAE;
	cfg->wazaeff_mode = WAZAEFF_MODE_ON;
#ifdef OLDCONFIG_ON
	cfg->input_mode = INPUTMODE_NORMAL;
	cfg->window_type = WINTYPE_01;
#endif
  cfg->moji_mode = MOJIMODE_HIRAGANA;
  cfg->wirelesssave_mode = WIRELESSSAVE_OFF;
	cfg->network_search	= NETWORK_SEARCH_ON;
}

//============================================================================================
//============================================================================================
/**
 *	@brief	�L�[�R���t�B�O���Q�[���ɔ��f����
 */
#ifdef OLDCONFIG_ON
void config_SetKeyConfig(SAVE_CONTROL_WORK * sv,INPUTMODE mode)
{
	CONFIG* config;
	
	if(sv != NULL){
		mode = CONFIG_GetInputMode(SaveData_GetConfig(sv));
	}
	GFL_UI_KEY_SetControlMode(mode);
}
#endif

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
MSGSPEED CONFIG_GetMsgSpeed(const CONFIG * cfg)
{
	return cfg->msg_speed;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_SetMsgSpeed(CONFIG * cfg, MSGSPEED speed)
{
	cfg->msg_speed = speed;
}

//----------------------------------------------------------
/**
 * @brief	�ݒ�f�[�^���當���`�摬�x���擾
 * @param	cfg		�R���t�B�O�f�[�^
 * @return	�����`�摬�x
 */
//---------------------------------------------------------
u8 CONFIG_GetMsgPrintSpeed( const CONFIG * cfg )
{
	int speed = CONFIG_GetMsgSpeed(cfg);
	if (speed == 0){
		return MSG_SPEED_SLOW;
	} else if (speed == 1) {
		return MSG_SPEED_NORMAL;
	} else {
		return MSG_SPEED_FAST;
	}
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
SOUNDMODE CONFIG_GetSoundMode(const CONFIG * cfg)
{
	return cfg->sound_mode;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_SetSoundMode(CONFIG * cfg, SOUNDMODE snd_mode)
{
	cfg->sound_mode = snd_mode;
}

//----------------------------------------------------------
/**
 */
//----------------------------------------------------------
WAZAEFF_MODE CONFIG_GetWazaEffectMode(const CONFIG * cfg)
{
	return cfg->wazaeff_mode;
}

//----------------------------------------------------------
/**
 */
//----------------------------------------------------------
void CONFIG_SetWazaEffectMode(CONFIG * cfg, WAZAEFF_MODE mode)
{
	cfg->wazaeff_mode = mode;
}
//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
BATTLERULE CONFIG_GetBattleRule(const CONFIG * cfg)
{
	return cfg->battle_rule;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_SetBattleRule(CONFIG * cfg, BATTLERULE rule)
{
	cfg->battle_rule = rule;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
#ifdef OLDCONFIG_ON
INPUTMODE CONFIG_GetInputMode(const CONFIG * cfg)
{
	return cfg->input_mode;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_SetInputMode(CONFIG * cfg, INPUTMODE mode)
{
	cfg->input_mode = mode;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
WINTYPE CONFIG_GetWindowType(const CONFIG * cfg)
{
	return cfg->window_type;
}

//----------------------------------------------------------
/**
 */
//---------------------------------------------------------
void CONFIG_SetWindowType(CONFIG * cfg, WINTYPE type)
{
	cfg->window_type = type;
}
#endif

//	�������[�h
MOJIMODE CONFIG_GetMojiMode(const CONFIG * cfg)
{
  return cfg->moji_mode;
}

void CONFIG_SetMojiMode(CONFIG * cfg, MOJIMODE type)
{
  cfg->moji_mode = type;
	GFL_MSGSYS_SetLangID( cfg->moji_mode );
}

//	���C�����X�Z�[�u���[�h
WIRELESSSAVE_MODE CONFIG_GetWirelessSaveMode(const CONFIG * cfg)
{
  return cfg->wirelesssave_mode;
}

void CONFIG_SetWirelessSaveMode(CONFIG * cfg, WIRELESSSAVE_MODE type)
{
  cfg->wirelesssave_mode = type;
}

//�Q�[�����̃l�b�g���[�N�T�[�`���[�h
NETWORK_SEARCH_MODE CONFIG_GetNetworkSearchMode( const CONFIG * cfg )
{	
	return cfg->network_search;
}
void CONFIG_SetNetworkSearchMode( CONFIG * cfg, NETWORK_SEARCH_MODE mode )
{	
	cfg->network_search	= mode;
}
