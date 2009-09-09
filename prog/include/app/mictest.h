//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mictest.h
 *	@brief		�}�C�N�e�X�g��ʊO�����J�w�b�_�[
 *	@author		Toru=Nagihashi
 *	@data		2008.07.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MICTEST_H__
#define __MICTEST_H__

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern GFL_PROC_RESULT MicTestProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MicTestProc_Exit( GFL_PROC *proc,int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MicTestProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *mywk );


#endif		// __MICTEST_H__

