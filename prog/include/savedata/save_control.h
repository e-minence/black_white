//==============================================================================
/**
 * @file	save_control.h
 * @brief	�Z�[�u�f�[�^�Ǘ��w�b�_
 * @author	matsuda
 * @date	2008.08.28(��)
 */
//==============================================================================
#ifndef __SAVE_CONTROL_H__
#define __SAVE_CONTROL_H__

#include <gflib.h>
#include <backup_system.h>


//==============================================================================
//	�萔��`
//==============================================================================
///�Z�[�u�V�X�e���Ŏg�p����q�[�v�T�C�Y�@���T�C�Y��ς���ꍇ�A���c�܂ŕ񍐂����肢���܂�
#define HEAPSIZE_SAVE       (0x25000)

//--------------------------------------------------------------
//	first_status�̌���BIT
//--------------------------------------------------------------
#define FST_NORMAL_NG_BIT			(1<<0)		//�ʏ�Z�[�u�F�~���[�����ONG
#define FST_NORMAL_BREAK_BIT		(1<<1)		//�ʏ�Z�[�u�F�j��
#define FST_FRONTIER_NG_BIT			(1<<2)		//�O���t�����e�B�A�Z�[�u�F�~���[�����ONG
#define FST_FRONTIER_BREAK_BIT		(1<<3)		//�O���t�����e�B�A�Z�[�u�F�j��
#define FST_VIDEO_NG_BIT			(1<<4)		//�O���r�f�I�Z�[�u�F�~���[�����ONG
#define FST_VIDEO_BREAK_BIT			(1<<5)		//�O���r�f�I�Z�[�u�F�j��

//--------------------------------------------------------------
//  �O���Z�[�u�ԍ�
//--------------------------------------------------------------
//��SaveParamExtraTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
typedef enum{
  SAVE_EXTRA_ID_REC_MINE,
  SAVE_EXTRA_ID_REC_DL_0,
  SAVE_EXTRA_ID_REC_DL_1,
  SAVE_EXTRA_ID_REC_DL_2,
  SAVE_EXTRA_ID_CGEAR_PICUTRE,
  SAVE_EXTRA_ID_BATTLE_EXAMINATION,
  SAVE_EXTRA_ID_STREAMING,
  SAVE_EXTRA_ID_ZUKAN_WALLPAPER,
  
  SAVE_EXTRA_ID_MAX,
}SAVE_EXTRA_ID;

//==============================================================================
//	�^��`
//==============================================================================
///�Z�[�u�f�[�^�Ǘ����[�N�^�̕s��`�^
typedef struct _SAVE_CONTROL_WORK SAVE_CONTROL_WORK;

//--------------------------------------------------------------
//  GDS�p�ɒǉ��@��check�@�^��f�[�^�̈��������m��ׁ̈A�������
//--------------------------------------------------------------
///�O���Z�[�u�̔F�؃L�[�̌^
typedef u32 EX_CERTIFY_SAVE_KEY;
///�O���Z�[�u�Ƀf�[�^�����݂��Ȃ��ꍇ�̔F�؃L�[
#define EX_CERTIFY_SAVE_KEY_NO_DATA		(0xffffffff)


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern SAVE_CONTROL_WORK * SaveControl_SystemInit(HEAPID heap_id);
extern SAVE_CONTROL_WORK * SaveControl_GetPointer(void);
extern LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl);
extern u32 SaveControl_GetSaveAsyncMain_WritingSize(SAVE_CONTROL_WORK *ctrl);
extern void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id);
extern const void * SaveControl_GetSaveWorkAdrs(SAVE_CONTROL_WORK *ctrl, u32 *get_size);
extern BOOL SaveControl_NewDataFlagGet(const SAVE_CONTROL_WORK *ctrl);
extern u32 SaveControl_GetLoadResult(const SAVE_CONTROL_WORK * sv);
extern BOOL SaveData_GetExistFlag(const SAVE_CONTROL_WORK * sv);
extern BOOL SaveControl_IsOverwritingOtherData(const SAVE_CONTROL_WORK * sv);
extern void SaveControl_ClearData(SAVE_CONTROL_WORK * ctrl);
extern void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_GetActualSize(SAVE_CONTROL_WORK *ctrl, u32 *actual_size, u32 *total_size);

//--------------------------------------------------------------
//  �O���Z�[�u�֘A
//--------------------------------------------------------------
extern LOAD_RESULT SaveControl_Extra_Load(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, HEAPID heap_id);
extern void SaveControl_Extra_Unload(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
extern BOOL SaveControl_Extra_CheckLoad(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
extern LOAD_RESULT SaveControl_Extra_LoadWork(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, HEAPID heap_id, void *work, u32 work_size);
extern void SaveControl_Extra_SystemSetup(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, 
  HEAPID heap_id, void *work, u32 work_size);
extern void SaveControl_Extra_UnloadWork(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
extern void SaveControl_Extra_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
extern SAVE_RESULT SaveControl_Extra_SaveAsyncMain(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
extern void * SaveControl_Extra_DataPtrGet(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, GFL_SVDT_ID gmdata_id);
extern void SaveControl_Extra_ClearData(SAVE_CONTROL_WORK * ctrl, SAVE_EXTRA_ID extra_id);
extern void SaveControl_Extra_Erase(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, u32 heap_temp_id);


#ifdef PM_DEBUG
extern GFL_SAVEDATA * DEBUG_SaveData_PtrGet(void);
#endif


#endif	//__SAVE_CONTROL_H__

