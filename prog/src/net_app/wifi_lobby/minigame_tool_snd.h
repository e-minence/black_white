//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_tool_snd.h
 *	@brief		�~�j�Q�[���c�[����p�T�E���h��`
 *	@author		tomoya takahashi
 *	@data		2007.10.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "sound/pm_sndsys.h"
#include "net_app/net_bugfix.h"

#if WB_FIX   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#define MNGM_SND_COUNT				( SEQ_SE_DP_DECIDE )		// �R�E�Q�E�P
#define MNGM_SND_START				( SEQ_SE_DP_CON_016 )		// �X�^�[�g

#define MNGM_SND_TIMEUP				( SEQ_SE_DP_PINPON )		// �^�C���A�b�v

//////////////////////////////////////////////////////////////////////////////
//�ȉ��𓖂ĂĂ�������

#define MNGM_SND_TITLE				( SEQ_SE_PL_FCALL )			// 3�C�̊G���\������鎞
//#define MNGM_SND_BALLOON			( SEQ_SE_DP_BOX02 )			// ���͂��Ċ��������D��\������鉹
#define MNGM_SND_BALLOON			( SEQ_SE_PL_BALLOON07 )		// ���͂��Ċ��������D��\������鉹

//�ʐM�ΐ�ƍ��킹�Ă���
#define MNGM_SND_BAR_IN				( SEQ_SE_PL_MNGM_IN )		// �o�[(�^�b�`�y��)���C�����铮��
#define MNGM_SND_BAR_OUT			( SEQ_SE_PL_MNGM_OUT )		// �o�[(�^�b�`�y��)���A�E�g���铮��
#define MNGM_SND_BAR_FLASH			( SEQ_SE_PL_FVSDEMO06 )		// 1�ʂ̃o�[���_�� 
//SEQ_PL_WINMINI2(ME�𓖂Ă邱�ƂɕύX�\�� 08.03.18)





#else //WB_FIX @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@





#define MNGM_SND_COUNT				( SEQ_SE_SELECT1 )		// �R�E�Q�E�P
#define MNGM_SND_START				( SEQ_SE_SELECT1 )		// �X�^�[�g

#define MNGM_SND_TIMEUP				( SEQ_SE_SELECT1 )		// �^�C���A�b�v

#define MNGM_SND_TITLE				( SEQ_SE_SELECT1 )			// 3�C�̊G���\������鎞
//#define MNGM_SND_BALLOON			( SEQ_SE_SELECT1 )			// ���͂��Ċ��������D��\������鉹
#define MNGM_SND_BALLOON			( SEQ_SE_SELECT1 )		// ���͂��Ċ��������D��\������鉹

//�ʐM�ΐ�ƍ��킹�Ă���
#define MNGM_SND_BAR_IN				( SEQ_SE_SELECT1 )		// �o�[(�^�b�`�y��)���C�����铮��
#define MNGM_SND_BAR_OUT			( SEQ_SE_SELECT1 )		// �o�[(�^�b�`�y��)���A�E�g���铮��
#define MNGM_SND_BAR_FLASH			( SEQ_SE_SELECT1 )		// 1�ʂ̃o�[���_�� 
//SEQ_PL_WINMINI2(ME�𓖂Ă邱�ƂɕύX�\�� 08.03.18)

#endif  //WB_FIX

