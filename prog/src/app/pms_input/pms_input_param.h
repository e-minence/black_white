//============================================================================================
/**
	* @file	pms_input_param.h
	* @bfief	�ȈՉ�b���͉�ʁi�O����������n�����p�����[�^�I�u�W�F�N�g�̑��쏈���j
	* @author	taya
	* @date	06.01.24
	*/
//============================================================================================
#ifndef __PMS_INPUT_PARAM_H__
#define __PMS_INPUT_PARAM_H__

#include "app\pms_input.h"
#include "system\pms_word.h"

extern u32 PMSI_PARAM_GetInputMode( const PMSI_PARAM* p );
extern u32 PMSI_PARAM_GetGuidanceType( const PMSI_PARAM* p );
extern int PMSI_PARAM_GetWindowType( const PMSI_PARAM* p );
//extern const PMS_DUMMY_ZUKAN*  PMSI_PARAM_GetZukanSaveData( const PMSI_PARAM* p );
extern const PMSW_SAVEDATA* PMSI_PARAM_GetPMSW_SaveData( const PMSI_PARAM* p );
extern BOOL PMSI_PARAM_GetGameClearFlag( const PMSI_PARAM* p );
extern void PMSI_PARAM_GetInitializeData( const PMSI_PARAM* p, PMS_WORD* word, PMS_DATA* pms );
extern BOOL PMSI_PARAM_CheckModifiedByEditDatas( const PMSI_PARAM* p, const PMS_WORD* word, const PMS_DATA* pms );
extern void PMSI_PARAM_WriteBackData( PMSI_PARAM* p, const PMS_WORD* word, const PMS_DATA* pms );
extern BOOL PMSI_PARAM_GetNotEditEgnoreFlag( const PMSI_PARAM* p );
extern BOOL PMSI_PARAM_GetLockFlag( const PMSI_PARAM* p );
extern BOOL PMSI_PARAM_GetPictureFlag( const PMSI_PARAM* p );
extern int PMSI_PARAM_GetKTStatus(const PMSI_PARAM* p);
extern void PMSI_PARAM_SetKTStatus(const PMSI_PARAM* p,int param);



#endif
