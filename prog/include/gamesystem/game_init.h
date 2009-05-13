//============================================================================================
/**
 * @file	game_init.h
 * @date	2008.10.31
 * @author	tamada GAMEFREAK inc.
 * @brief	�Q�[���J�n���̏������p��`�Ȃ�
 */
//============================================================================================
#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

extern const GFL_PROC_DATA GameMainProcData;
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
	GAMEINIT_MODE_FIRST,
	GAMEINIT_MODE_CONTINUE,
	GAMEINIT_MODE_DEBUG,
}GAMEINIT_MODE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMEINIT_MODE mode;
	VecFx32 pos;
	u16 mapid;
	s16 dir;
	u8 always_net;      ///<TRUE:�펞�ʐM
	u8 padding[3];
}GAME_INIT_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GAME_INIT_WORK * DEBUG_GetGameInitWork(
  GAMEINIT_MODE mode, u16 mapid, VecFx32 *pos, s16 dir, BOOL always_net);

#ifdef	__cplusplus
}	//extern "C" {
#endif
