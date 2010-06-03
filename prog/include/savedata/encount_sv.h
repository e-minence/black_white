//=============================================================================
/**
 * @file	encount_sv.h
 * @brief	�G���J�E���g�֘A�Z�[�u�f�[�^�A�N�Z�X�֘A
 * @author	Miyuki Iwasawa
 */
//=============================================================================
#pragma once

#include "savedata/save_control.h"

//�ړ��|�P�����p�����[�^�h�c
#define MP_PARAM_ZONE_ID	(1)		//�]�[���h�c
#define MP_PARAM_POW_RND	(2)		//�p���[����
#define MP_PARAM_PER_RND	(3)		//������
#define MP_PARAM_MONSNO		(4)		//�����X�^�[�i���o�[
#define MP_PARAM_HP			  (5)		//�g�o
#define MP_PARAM_LV			  (6)		//�k�u
#define MP_PARAM_COND		  (7)		//��Ԉُ�
#define MP_PARAM_ENC		  (8)		//�G���J�E���g�t���O�i���݈ړ������ǂ����j
#define MP_PARAM_MV_TYPE	(9)		//�ړ��^�C�v
#define MP_PARAM_SEIKAKU	(10)  //���i

typedef struct ENC_SV_DATA_tag * ENC_SV_PTR;
typedef struct MV_POKE_DATA_tag * MPD_PTR;

extern int EncDataSave_GetWorkSize(void);
extern ENC_SV_PTR EncDataSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv);
extern void EncDataSave_Init(ENC_SV_PTR outEncData);

extern void EncDataSave_UpdateGenerate( SAVE_CONTROL_WORK * sv );
extern u8 EncDataSave_GetGenerateZoneIdx( ENC_SV_PTR inEncData );

extern void EncDataSave_UpdatePlayerZoneHist(ENC_SV_PTR ioEncData, const int inZoneID);
extern int EncDataSave_GetPlayerOldZone(ENC_SV_PTR inEncData);

extern u8 EncDataSave_GetMovePokeState(ENC_SV_PTR inEncData, const u8 inTargetPoke);
extern void EncDataSave_SetMovePokeState(ENC_SV_PTR inEncData, const u8 inTargetPoke,u8 status);

extern u8 EncDataSave_GetMovePokeZoneIdx(ENC_SV_PTR inEncData, const u8 inTargetPoke);
extern void EncDataSave_SetMovePokeZoneIdx(	ENC_SV_PTR outEncData,
											const u8 inTargetPoke,
											const u8 inZoneIndex	);
extern u8 EncDataSave_IsMovePokeValid(ENC_SV_PTR inEncData, const u8 inTargetPoke);
extern void EncDataSave_ClearMovePokeData(MPD_PTR outMPData);
extern MPD_PTR	EncDataSave_GetMovePokeDataPtr(ENC_SV_PTR inEncData, const u8 inTargetPoke);
extern u32 EncDataSave_GetMovePokeDataParam(const MPD_PTR inMPData,
											const u8 inParamID);
extern void EncDataSave_SetMovePokeDataParam(	MPD_PTR outMPData,
												const u8 inParamID,
												const u32 inVal	);
extern void EncDataSave_SetSprayCnt(ENC_SV_PTR inEncData,u8 count);
extern BOOL EncDataSave_DecSprayCnt(ENC_SV_PTR inEncData);
extern BOOL EncDataSave_CanUseSpray(ENC_SV_PTR inEncData);

