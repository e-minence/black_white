//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wldtimer_snd.h
 *	@brief		���E���v	��
 *	@author		tomoya takahashi
 *	@data		2007.09.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//#include "system/snd_tool.h"
#include "sound/pm_sndsys.h"
#include "net_app/net_bugfix.h"


#if WB_TEMP_FIX //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//#define WLDTIMER_SND_ZOMEIN			( SEQ_SE_PL_TIMER01 )		// �Y�[���C��
#define WLDTIMER_SND_ZOMEIN				( SEQ_SE_PL_TIMER03 )		// �Y�[���C��
//#define WLDTIMER_SND_ZOMEOUT			( SEQ_SE_PL_TIMER01 )		// �Y�[���A�E�g
#define WLDTIMER_SND_ZOMEOUT			( SEQ_SE_PL_TIMER03 )		// �Y�[���A�E�g

#define WLDTIMER_SND_XSELECT			( SEQ_SE_PL_TIMER04 )		// x�Z���N�g

#define WLDTIMER_SND_ENTER				( SEQ_SE_DP_DECIDE )		// �G���^�[

#define WLDTIMER_SND_BALLOON			( SEQ_SE_PL_TOKEI3 )		// ���D�����ł��鉹(���[�v��)

#define WLDTIMER_SND_YAMERU				( SEQ_SE_DP_DECIDE )		// ��߂���^�b�`�AB�L�����Z��


#else //WB_TEMP_FIX //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#define WLDTIMER_SND_ZOMEIN				( SEQ_SE_SELECT1 )		// �Y�[���C��
#define WLDTIMER_SND_ZOMEOUT			( SEQ_SE_SELECT1 )		// �Y�[���A�E�g

#define WLDTIMER_SND_XSELECT			( SEQ_SE_SELECT1 )		// x�Z���N�g

#define WLDTIMER_SND_ENTER				( SEQ_SE_SELECT1 )		// �G���^�[

#define WLDTIMER_SND_BALLOON			( SEQ_SE_SELECT1 )		// ���D�����ł��鉹(���[�v��)

#define WLDTIMER_SND_YAMERU				( SEQ_SE_SELECT1 )		// ��߂���^�b�`�AB�L�����Z��


#endif //WB_TEMP_FIX //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

