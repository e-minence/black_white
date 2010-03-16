//============================================================================================
/**
	* @file	pmsi_sound_def.h
	* @bfief	�{�b�N�X��ʃT�E���h�i���o�[��`
	* @author	taya
	* @date	06.02.15
	*/
//============================================================================================

#ifndef __PMSI_SOUND_DEF_H__
#define __PMSI_SOUND_DEF_H__

#include "sound\pm_sndsys.h"

#define PMS_USE_SND (1)

#if PMS_USE_SND
#define  SOUND_MOVE_CURSOR		(SEQ_SE_SELECT1)		// �J�[�\���ړ�

#define  SOUND_DECIDE			(SEQ_SE_DECIDE1)		// ���艹

#define  SOUND_CANCEL			(SEQ_SE_CANCEL1)		// �L�����Z����

#define  SOUND_CHANGE_SENTENCE	(SEQ_SE_SELECT1)		// ���͕ύX

#define  SOUND_CHANGE_CATEGORY	(SEQ_SE_SELECT4)		// �J�e�S���ύX
//#define  SOUND_CHANGE_CATEGORY	(SEQ_SE_DP_BUTTON9)		// �J�e�S���ύX

#define  SOUND_DISABLE_CATEGORY	(SEQ_SE_BEEP)	// �����ȃJ�e�S���I��
//#define  SOUND_DISABLE_CATEGORY	(SEQ_SE_DP_CUSTOM06)	// �����ȃJ�e�S���I��

#define  SOUND_DISABLE_BUTTON	(SEQ_SE_BEEP)	// �����ȃ{�^���^�b�`
//#define  SOUND_DISABLE_BUTTON	(SEQ_SE_DP_CUSTOM06)	// �����ȃ{�^���^�b�`

#define SOUND_WORD_INPUT (SEQ_SE_DECIDE1)  // ��������
#define SOUND_WORD_DELETE (SEQ_SE_CANCEL3)  // �����폜

#define SOUND_SEARCH_DISABLE (SEQ_SE_BEEP) // �������s

#define SOUND_TOUCH_FLIPBUTTON (SEQ_SE_SELECT1) // �Z���e���X�ύX�{�^���^�b�`

#define SOUND_TOUCH_SLIDEBAR (SEQ_SE_SYS_06)    // �X���C�h�o�[���^�b�`

#endif //PMS_USE_SND

#endif	//__PMSI_SOUND_DEF_H__
