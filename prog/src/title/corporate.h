//============================================================================================
/**
 * @file		corporate.h
 * @brief		��Ж��\��
 * @author	Hiroyuki Nakamura
 * @date		2009.12.16
 */
//============================================================================================
#pragma	once


#ifdef	PM_DEBUG
// �I�����[�h
enum {
	CORPORATE_RET_NORMAL = 0,		// �ʏ�I��
	CORPORATE_RET_SKIP,					// �X�L�b�v
	CORPORATE_RET_DEBUG,				// �f�o�b�O��
};
#endif


// PROC�f�[�^
extern const GFL_PROC_DATA CorpProcData;
