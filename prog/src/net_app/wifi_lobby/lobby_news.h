//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news.h
 *	@brief		���r�[�j���[�X	�`��
 *	@author		tomoya takahashi
 *	@data		2007.10.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __LOBBY_NEWS_H__
#define __LOBBY_NEWS_H__

#include "lobby_news_data.h"
#include "wflby_system_def.h"

#include "system/procsys.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�f�[�^�\���p	�v���b�N�f�[�^
//=====================================
typedef struct{
	NEWS_DATA* p_data;
	const WFLBY_TIME* cp_nowtime;	// ���ݎ���
	WFLBY_SYSTEM* p_system;

} NEWS_DRAW_DATA;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern GFL_PROC_RESULT NEWS_DRAW_Init(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);
extern GFL_PROC_RESULT NEWS_DRAW_Main(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);
extern GFL_PROC_RESULT NEWS_DRAW_Exit(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);


#endif		// __LOBBY_NEWS_H__

