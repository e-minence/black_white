//============================================================================================
/**
 * @file	mysign.h
 * @bfief	�����T�C���쐬����
 * @author	Akito Mori
 * @date	06.03.11
 */
//============================================================================================
#ifndef _MYSIGN_H_
#define _MYSIGN_H_

//============================================================================================
//	��`
//============================================================================================

// �v���Z�X��`�f�[�^
extern GFL_PROC_RESULT MySignProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT MySignProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT MySignProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

// �|�C���^�Q�Ƃ����ł��邨�G�����{�[�h���[�N�\����
typedef struct MYSIGN_WORK MYSIGN_WORK;	



#endif