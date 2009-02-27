//==============================================================================
/**
 * @file	balloon_tool.h
 * @brief	���D����F�c�[����
 * @author	matsuda
 * @date	2007.11.25(��)
 */
//==============================================================================
#ifndef __BALLOON_TOOL_H__
#define __BALLOON_TOOL_H__


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void BalloonTool_BalloonBGSet(int player_max, 
	int level, BALLOON_STATUS *bst);
extern void BalloonTool_BalloonUpdate(BALLOON_GAME_PTR game, BALLOON_STATUS *bst);
extern BOOL BalloonTool_PlayerAirParamAdd(BALLOON_GAME_PTR game, const BALLOON_AIR_DATA *air_data);
extern void Air_Update(BALLOON_GAME_PTR game);
extern void Air_ActorAllDelete(BALLOON_GAME_PTR game);
extern BOOL Exploded_Update(BALLOON_GAME_PTR game);
extern void Exploded_AllDelete(BALLOON_GAME_PTR game);
extern BOOL BalloonTool_ExplodedParamAdd(BALLOON_GAME_PTR game);
extern void IconBalloon_AllCreate(BALLOON_GAME_PTR game);
extern void IconBalloon_AllDelete(BALLOON_GAME_PTR game);
extern BOOL IconBalloon_Update(BALLOON_GAME_PTR game);
extern void BalloonTool_PaletteSwap_Pipe(BALLOON_GAME_PTR game);
extern void BalloonTool_PaletteSwap_PlayerOBJ(BALLOON_GAME_PTR game);
extern void BalloonTool_PaletteSwap_Storm(BALLOON_GAME_PTR game);
extern int BalloonTool_BalloonLevelGet(int balloon_no);
extern void BalloonTool_AirDataCreate(BALLOON_GAME_PTR game, 
	int balloon_no, s32 air, BALLOON_AIR_DATA *air_data);
extern void Joint_ActorCreateAll(BALLOON_GAME_PTR game, JOINT_WORK *joint);
extern void Joint_ActorDeleteAll(BALLOON_GAME_PTR game, JOINT_WORK *joint);
extern void Booster_ActorCreateAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster);
extern void Booster_ActorDeleteAll(BALLOON_GAME_PTR game, BOOSTER_WORK *booster);
extern void Booster_Init(BALLOON_GAME_PTR game, BOOSTER_WORK *booster);
extern void Booster_Update(BALLOON_GAME_PTR game, BOOSTER_WORK *booster);
extern BOOL BalloonTool_SendAirPush(BALLOON_GAME_PTR game, BALLOON_AIR_DATA *air_data);
extern BALLOON_AIR_DATA * BalloonTool_SendAirPop(BALLOON_GAME_PTR game);
extern BOOL BalloonTool_SendAirBoosterSet(BALLOON_GAME_PTR game, int booster_type);
extern void SioBooster_ActorCreateAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster);
extern void SioBooster_ActorDeleteAll(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster);
extern void SioBooster_Appear(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster, 
	int booster_type, int net_id, int arrival_frame);
extern void SioBooster_Update(BALLOON_GAME_PTR game, SIO_BOOSTER_WORK *sio_booster);
#if WB_TEMP_FIX
extern void BalloonTool_FontOamCreate(CATS_RES_PTR crp, 
	FONTOAM_SYS_PTR fontoam_sys, BALLOON_FONTACT *fontact, const STRBUF *str, 
	FONT_TYPE font_type, PRINTSYS_LSB color, int pal_offset, int pal_id, 
	int x, int y, int pos_center, int bg_pri, int soft_pri, int y_char_len);
#endif
extern void Balloon_FontOamDelete(BALLOON_FONTACT *fontact);
extern void Balloon_CounterUpdate(BALLOON_GAME_PTR game, BALLOON_COUNTER *counter);
extern void Balloon_CounterNextNumberSet(BALLOON_COUNTER *counter, int next_number);
extern void Balloon_CounterPosUpdate(BALLOON_COUNTER *counter);
extern GFL_CLWK* CounterWindow_ActorCreate(BALLOON_GAME_PTR game);
extern void CounterWindow_ActorDelete(BALLOON_GAME_PTR game, GFL_CLWK* cap);
extern GFL_CLWK* TouchPen_ActorCreate(BALLOON_GAME_PTR game);
extern void TouchPen_ActorDelete(BALLOON_GAME_PTR game, GFL_CLWK* cap);
extern BOOL BalloonTool_TouchPenDemoMove(BALLOON_GAME_PTR game, BALLOON_PEN *pen);
extern void CounterDummyNumber_ActorCreate(BALLOON_GAME_PTR game);
extern void CounterDummyNumber_ActorDelete(BALLOON_GAME_PTR game);
extern void BalloonTool_NameWindowPalNoSwap(BALLOON_GAME_PTR game);


#endif	//__BALLOON_TOOL_H__
