//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_snd.h
 *	@brief		���w�b�_
 *	@author		tomoya takahashi
 *	@data		2007.09.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net_app/net_bugfix.h"
#include "sound/pm_sndsys.h"

#if WB_TEMP_FIX

#define BCT_SND_MARUIN				( SEQ_SE_DP_ELEBETA )		// �}���m�[���o��
#define BCT_SND_MARUIN2				( SEQ_SE_DP_HAMARU )		// �}���m�[���o���������
#define BCT_SND_MARUIN3				( SEQ_SE_DP_KI_GASYAN )		// �}���m�[���o��͂܂�

#define BCT_SND_COUNT				( SEQ_SE_DP_DECIDE )		// �R�E�Q�E�P

#define BCT_SND_START				( SEQ_SE_DP_CON_016 )		// �X�^�[�g

#define BCT_SND_SLOW				( SEQ_SE_DP_FW104 )			// �����鉹

#define BCT_SND_EAT					( SEQ_SE_PL_NOMI02 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT100				( SEQ_SE_PL_POINT1 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT200				( SEQ_SE_PL_POINT2 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT300				( SEQ_SE_PL_POINT3 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_BOUND				( SEQ_SE_DP_023 )			// �͂�����

#define BCT_SND_TIMEUP				( SEQ_SE_DP_PINPON )		// �^�C���A�b�v

#define BCT_SND_FEVER_CHIME			( SEQ_SE_PL_PINPON )		// �t�B�[�o�[�O�̃`���C��

#define BCT_SND_FEVER_MOVE			( SEQ_SE_PL_BALLOON05_2 )	// �}���m�[���̃t�B�[�o�[����



#else     //WB_TEMP_FIX   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#define BCT_SND_MARUIN				( SEQ_SE_SELECT1 )		// �}���m�[���o��
#define BCT_SND_MARUIN2				( SEQ_SE_SELECT1 )		// �}���m�[���o���������
#define BCT_SND_MARUIN3				( SEQ_SE_SELECT1 )		// �}���m�[���o��͂܂�

#define BCT_SND_COUNT				( SEQ_SE_SELECT1 )		// �R�E�Q�E�P

#define BCT_SND_START				( SEQ_SE_SELECT1 )		// �X�^�[�g

#define BCT_SND_SLOW				( SEQ_SE_SELECT1 )			// �����鉹

#define BCT_SND_EAT					( SEQ_SE_SELECT1 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT100				( SEQ_SE_SELECT1 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT200				( SEQ_SE_SELECT1 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_EAT300				( SEQ_SE_SELECT1 )		// �H�ׂ鉹

//////////////////////////////////////////////////////////////////////////////
#define BCT_SND_BOUND				( SEQ_SE_SELECT1 )			// �͂�����

#define BCT_SND_TIMEUP				( SEQ_SE_SELECT1 )		// �^�C���A�b�v

#define BCT_SND_FEVER_CHIME			( SEQ_SE_SELECT1 )		// �t�B�[�o�[�O�̃`���C��

#define BCT_SND_FEVER_MOVE			( SEQ_SE_SELECT1 )	// �}���m�[���̃t�B�[�o�[����


#endif		//WB_TEMP_FIX

