
//============================================================================================
/**
 * @file	client.h
 * @bfief	戦闘クライアントプログラム
 * @author	HisashiSogabe
 * @date	05.05.24
 */
//============================================================================================

#ifndef __CLIENT_H_
#define __CLIENT_H_

#include	"battle/battle_common.h"
#include	"battle/fight.h"

///<クライアント関数テーブルジャンプ用定数定義
enum{
	CL_ENCOUNT_EFFECT=1,
	CL_POKEMON_ENCOUNT,
	CL_POKEMON_ENCOUNT_APPEAR,
	CL_POKEMON_APPEAR,
	CL_POKEMON_RETURN,
	CL_POKEMON_INHALE,
	CL_POKEMON_DELETE,
	CL_TRAINER_ENCOUNT,
	CL_TRAINER_THROW,
	CL_TRAINER_OUT,					//10
	CL_TRAINER_IN,
	CL_HP_GAUGE_IN,
	CL_HP_GAUGE_OUT,
	CL_COMMAND_SELECT,
	CL_WAZA_SELECT,
	CL_DIR_SELECT,
	CL_ITEM_SELECT,
	CL_POKEMON_SELECT,
	CL_YES_NO_SELECT,				//0x10
	CL_ATTACK_MESSAGE,				//20
	CL_MESSAGE,	
	CL_WAZA_EFFECT,		
	CL_POKEMON_BLINK,
	CL_HP_GAUGE_CALC,
	CL_EXP_GAUGE_CALC,
	CL_KIZETSU_EFFECT,
	CL_SE_PLAY,
	CL_FADE_OUT,
	CL_POKEMON_VANISH_ON_OFF,
	CL_HP_GAUGE_STATUS_SET,			//30
	CL_TRAINER_MESSAGE,
	CL_MODOSU_MESSAGE,
	CL_KURIDASU_MESSAGE,
	CL_ENCOUNT_MESSAGE,
	CL_ENCOUNT_KURIDASU_MESSAGE,
	CL_LEVEL_UP_EFFECT,
	CL_ALERT_MESSAGE,
	CL_HP_GAUGE_REFRESH,
	CL_PSP_TO_PSP_COPY,
	CL_TRAINER_BG_SLIDEIN,			//40
	CL_GAUGE_ANIME_STOP,
	CL_POKEMON_REFRESH,
	CL_WAZA_WASURE,
	CL_MOSAIC_SET,
	CL_FORM_CHG,
	CL_BG_CHG,
	CL_SUBBG_WALL_PUT,
	CL_BALL_GAUGE_ENCOUNT_SET,
	CL_BALL_GAUGE_ENCOUNT_OUT,
	CL_BALL_GAUGE_SET,				//50
	CL_BALL_GAUGE_OUT,
	CL_BALL_GAUGE_RESOURCE_LOAD,
	CL_BALL_GAUGE_RESOURCE_DELETE,
	CL_INC_RECORD,
	CL_SIO_WAIT_MESSAGE,
	CL_CHR_POP,
	CL_SS_TO_OAM_CALL,
	CL_OAM_TO_SS_CALL,
	CL_WIN_LOSE_MESSAGE,
	CL_ESCAPE_MESSAGE,				//60
	CL_GIVEUP_MESSAGE,
	CL_MIGAWARI_CHR_SET,
	CL_WAZAKOUKA_SE,
	CL_BGM_PLAY,
	CL_WIN_LOSE_SET,
	CL_BLANK_MESSAGE,	

	CL_END
};

extern	CLIENT_PARAM	*ClientInit(BATTLE_WORK *bw,FIGHT_SYSTEM_BOOT_PARAM *fsbp);
extern	void			ClientMain(BATTLE_WORK *battle_work,CLIENT_PARAM *client_param);
extern	void			ClientEnd(BATTLE_WORK *bw,CLIENT_PARAM *cp,int mode);

extern	void			ClientBMInit(BATTLE_WORK *bw,CLIENT_PARAM *cp,int ballID,int sel_mons_no);

#endif __CLIENT_H_
