//============================================================================================
/**
 * @file	config.c
 * @brief	設定関連アクセス
 * @author	tamada	GAME FREAK Inc.
 * @date	2006.01.27
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/config.h"


//============================================================================================
//============================================================================================
#define	MSG_SPEED_SLOW		( 8 )		///< メッセージ速度：遅い
#define	MSG_SPEED_NORMAL	( 4 )		///< メッセージ速度：普通
#define	MSG_SPEED_FAST		( 1 )		///< メッセージ速度：速い


//============================================================================================
//============================================================================================


//==============================================================================
//	データ
//==============================================================================
///キーカスタマイズデータテーブル
///	※このテーブルは常駐にある必要があります
static const GFL_UI_KEY_CUSTOM_TBL KeyCustmizeData[] ={
	//ノーマル
	{GFL_UI_KEY_END,0,0},		// 終了データ
	
	//START=X
	{PAD_BUTTON_START,PAD_BUTTON_X,GFL_UI_KEY_COPY},		//STARTをX扱いに
	{GFL_UI_KEY_END,0,0},		// 終了データ

	//L=A
	{PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//LをA扱いに
	{GFL_UI_KEY_END,0,0},		// 終了データ
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   キーカスタマイズテーブルデータ登録
 *
 * ゲーム中、一度だけ呼び出せばよいです
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
	cfg->input_mode = INPUTMODE_NORMAL;
	cfg->window_type = WINTYPE_01;
  cfg->moji_mode = MOJIMODE_HIRAGANA;
}

//============================================================================================
//============================================================================================
/**
 *	@brief	キーコンフィグをゲームに反映する
 */
void config_SetKeyConfig(SAVE_CONTROL_WORK * sv,INPUTMODE mode)
{
	CONFIG* config;
	
	if(sv != NULL){
		mode = CONFIG_GetInputMode(SaveData_GetConfig(sv));
	}
	GFL_UI_KEY_SetControlMode(mode);
}

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
 * @brief	設定データから文字描画速度を取得
 * @param	cfg		コンフィグデータ
 * @return	文字描画速度
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

//	文字モード
MOJIMODE CONFIG_GetMojiMode(const CONFIG * cfg)
{
  return cfg->moji_mode;
}

void CONFIG_SetMojiMode(CONFIG * cfg, MOJIMODE type)
{
  cfg->moji_mode = type;
}

