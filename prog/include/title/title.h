//==============================================================================
/**
 * @file	title.h
 * @brief	�^�C�g����ʂ̃w�b�_
 * @author	matsuda
 * @date	2008.12.03(��)
 */
//==============================================================================
#ifndef __TITLE_H__
#define __TITLE_H__

//==============================================================================
//	�萔��`
//==============================================================================
FS_EXTERN_OVERLAY(title);

typedef struct {
	// [ in ]
#ifdef	PM_DEBUG
	u32	skipMode;			// �^�C�g���X�L�b�v�i�f�o�b�O�p�j
#endif	// PM_DEBUG
	// [ out ]
	u32	endMode;			// �I�����[�h
}TITLE_PARAM;

// �I�����[�h
enum {
	TITLE_END_SELECT = 0,		// �Q�[���J�n
	TITLE_END_TIMEOUT,			// �^�C�g�����[�v
#ifdef	PM_DEBUG
	TITLE_END_DEBUG_CALL,		// �f�o�b�O���j���[�Ăяo��
#endif	// PM_DEBUG
};


//==============================================================================
//	�O���f�[�^
//==============================================================================
extern const GFL_PROC_DATA TitleProcData;
extern const GFL_PROC_DATA TitleControlProcData;


#endif	//__TITLE_H__
