//============================================================================================
/**
 * @file	  gbs_main.c
 * @brief	  �O���[�o���o�g���X�e�[�V����
 * @author	k.ohno
 * @date	  09.08.27
 */
//============================================================================================
#include <gflib.h>
#include "arc_def.h"
#include "net/network_define.h"
#include "savedata/save_tbl.h"



//--------------------------------------------------------------------------------------------
/**
 * @brief	  �O���[�o���o�g���X�e�[�V�����v���Z�X�F������
 * @param  	proc	�v���Z�X�ւ̃|�C���^
 * @param	  seq		�V�[�P���X�p���[�N�ւ̃|�C���^
 * @param	  pwk		�e�̃��[�N
 * @param	  mywk	�����̃��[�N
 * @return	PROC_RES_CONTINUE	����p����
 * @return	PROC_RES_FINISH		����I��
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT GlobalBattleStationProc_Init(GFL_PROC *proc, int * seq, void *pwk, void *mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �O���[�o���o�g���X�e�[�V�����v���Z�X�F���C��
 * @param  	proc	�v���Z�X�ւ̃|�C���^
 * @param	  seq		�V�[�P���X�p���[�N�ւ̃|�C���^
 * @param	  pwk		�e�̃��[�N
 * @param	  mywk	�����̃��[�N
 * @return	PROC_RES_CONTINUE	����p����
 * @return	PROC_RES_FINISH		����I��
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT GlobalBattleStationProc_Main(GFL_PROC *proc, int * seq, void *pwk, void *mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �O���[�o���o�g���X�e�[�V�����v���Z�X�F�I��
 * @param  	proc	�v���Z�X�ւ̃|�C���^
 * @param	  seq		�V�[�P���X�p���[�N�ւ̃|�C���^
 * @param	  pwk		�e�̃��[�N
 * @param	  mywk	�����̃��[�N
 * @return	PROC_RES_CONTINUE	����p����
 * @return	PROC_RES_FINISH		����I��
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT GlobalBattleStationProc_End(GFL_PROC *proc, int * seq, void *pwk, void *mywk )
{
	return GFL_PROC_RES_FINISH;
}






//------------------------------------------------------------------
///	�v���Z�X�f�[�^
//------------------------------------------------------------------
const GFL_PROC_DATA GlobalBattleStationProcData = {
	GlobalBattleStationProc_Init,
	GlobalBattleStationProc_Main,
	GlobalBattleStationProc_End,
};
