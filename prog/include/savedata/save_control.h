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

///�t���b�V���S�̂̃T�C�Y
#define SAVEFLASH_SIZE      (0x80000)   ///< 4Mbit�t���b�V��
///�t���b�V���̏����o�׏�Ԃ̏����l
#define SAVEFLASH_INIT_PARAM  (0xff)

//--------------------------------------------------------------
//	first_status�̌���BIT
//--------------------------------------------------------------
#define FST_NORMAL_NG_BIT			(1<<0)		//�ʏ�Z�[�u�F�~���[�����ONG
#define FST_NORMAL_BREAK_BIT		(1<<1)		//�ʏ�Z�[�u�F�j��
#define FST_EXTRA_START         (2)
#define FST_EXTRA_REC_MINE_BREAK_BIT		(1<<2)		//�O���Z�[�u�F�����̘^��F�j��
#define FST_EXTRA_REC_DL_0_BREAK_BIT		(1<<3)		//�O���Z�[�u�F�^��DL0�F�j��
#define FST_EXTRA_REC_DL_1_BREAK_BIT		(1<<4)		//�O���Z�[�u�F�^��DL1�F�j��
#define FST_EXTRA_REC_DL_2_BREAK_BIT		(1<<5)		//�O���Z�[�u�F�^��DL2�F�j��
#define FST_EXTRA_CGEAR_PICTURE_BREAK_BIT		    (1<<6)		//�O���Z�[�u�FCGEAR�F�j��
#define FST_EXTRA_BATTLE_EXAMINATION_BREAK_BIT	(1<<7)		//�O���Z�[�u�F�o�g������F�j��
#define FST_EXTRA_STREAMING_BREAK_BIT		        (1<<8)		//�O���Z�[�u�F�~���[�W�J���F�j��
#define FST_EXTRA_ZUKAN_WALLPAPER_BREAK_BIT	  	(1<<9)		//�O���Z�[�u�F�}�ӕǎ��F�j��
#define FST_EXTRA_DENDOU_BREAK_BIT	  	(1<<10)		//�O���Z�[�u�F�a������F�j��
#define FST_OUTSIDE_MYSTERY_BREAK_BIT	  	(1<<11)	//�Ǘ��O�Z�[�u�F�s�v�c�ȑ��蕨�F�j��

//--------------------------------------------------------------
//  �O���Z�[�u�ԍ�
//--------------------------------------------------------------
//��SaveParamExtraTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
//��FST_EXTRA_�ւ̒ǉ������邱�ƁI
typedef enum{
  SAVE_EXTRA_ID_REC_MINE,
  SAVE_EXTRA_ID_REC_DL_0,
  SAVE_EXTRA_ID_REC_DL_1,
  SAVE_EXTRA_ID_REC_DL_2,
  SAVE_EXTRA_ID_CGEAR_PICUTRE,
  SAVE_EXTRA_ID_BATTLE_EXAMINATION,
  SAVE_EXTRA_ID_MUSICAL_DIST,
  SAVE_EXTRA_ID_ZUKAN_WALLPAPER,
  SAVE_EXTRA_ID_DENDOU,
  
  SAVE_EXTRA_ID_MAX,
}SAVE_EXTRA_ID;

//==============================================================================
//	�^��`
//==============================================================================
///�Z�[�u�f�[�^�Ǘ����[�N�^�̕s��`�^
typedef struct _SAVE_CONTROL_WORK SAVE_CONTROL_WORK;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern SAVE_CONTROL_WORK * SaveControl_SystemInit(HEAPID heap_id);
extern void SaveControl_SystemExit(void);
extern SAVE_CONTROL_WORK * SaveControl_GetPointer(void);
extern LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_SaveAsyncCancel(SAVE_CONTROL_WORK *ctrl);
extern u32 SaveControl_GetSaveAsyncMain_WritingSize(SAVE_CONTROL_WORK *ctrl);
extern void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id);
extern const void * SaveControl_GetSaveWorkAdrs(SAVE_CONTROL_WORK *ctrl, u32 *get_size);
extern BOOL SaveControl_NewDataFlagGet(const SAVE_CONTROL_WORK *ctrl);
extern u32 SaveControl_GetLoadResult(const SAVE_CONTROL_WORK * sv);
extern BOOL SaveData_GetExistFlag(SAVE_CONTROL_WORK * sv);
extern BOOL SaveControl_IsOverwritingOtherData(SAVE_CONTROL_WORK * sv);
extern BOOL SaveData_GetOutsideExistFlag(SAVE_CONTROL_WORK * sv);
extern void SaveData_SetOutsideExistFlag(SAVE_CONTROL_WORK * sv, BOOL outside_data_exists);
extern BOOL SaveData_GetOutsideBreakFlag(SAVE_CONTROL_WORK * sv);
extern void SaveData_SetOutsideBreakFlag(SAVE_CONTROL_WORK * sv, BOOL outside_data_break);
extern void SaveControl_ClearData(SAVE_CONTROL_WORK * ctrl);
extern void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_GetActualSize(SAVE_CONTROL_WORK *ctrl, u32 *actual_size, u32 *total_size);
extern BOOL SaveControl_PageFlashLoad(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdataid, BOOL side_a_or_b, void *dst, u32 load_size);

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
extern GFL_SAVEDATA * DEBUG_SaveDataExtra_PtrGet(SAVE_EXTRA_ID extra_id);
extern void DEBUG_OutsideSave_Brea(int mirror);
#endif


#endif	//__SAVE_CONTROL_H__

