//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_note_snd.h
 *	@brief		�F�B�蒠�@�T�E���h
 *	@author		tomoya takahshi
 *	@data		2007.07.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_NOTE_SND_H__
#define __WIFI_NOTE_SND_H__

#include "sound/pm_sndsys.h"



// SE�p��`
//#define WIFINOTE_MOVE_SE		(SEQ_SE_SELECT)
#define WIFINOTE_MOVE_SE		(SEQ_SE_SELECT1)  //��

#define WIFINOTE_MENU_MOVE_SE	(SEQ_SE_SELECT1)	// ���j���[�̈ړ���

//#define WIFINOTE_DECIDE_SE	(SEQ_SE_SELECT)
//#define WIFINOTE_DECIDE_SE	(SEQ_SE_BUTTON3)
#define WIFINOTE_DECIDE_SE		(SEQ_SE_DECIDE1)

#define WIFINOTE_MENU_DECIDE_SE	(SEQ_SE_SELECT1)	// ���j���[�̌���

//#define WIFINOTE_BS_SE		(SEQ_SE_SELECT)
//#define WIFINOTE_BS_SE		(SEQ_SE_BUTTON3)
#define WIFINOTE_BS_SE			(SEQ_SE_CANCEL1)
#define WIFINOTE_EXIT				(SEQ_SE_CLOSE1)

#define WIFINOTE_MENU_BS_SE		(SEQ_SE_CANCEL1)	// ���j���[�̃L�����Z��

#define WIFINOTE_CLEAN_SE		(SEQ_SE_SYS_41)	// �����S���ŏ�����

//#define WIFINOTE_SCRLL_SE		(SEQ_SE_BOX02)	// �X�N���[����
#define WIFINOTE_SCRLL_SE		(SEQ_SE_SELECT1)	// �X�N���[����

#endif		// __WIFI_NOTE_SND_H__

