//==============================================================================
/**
 * @file	save_control.h
 * @brief	セーブデータ管理ヘッダ
 * @author	matsuda
 * @date	2008.08.28(木)
 */
//==============================================================================
#ifndef __SAVE_CONTROL_H__
#define __SAVE_CONTROL_H__

#include <backup_system.h>


//==============================================================================
//	定数定義
//==============================================================================
///セーブシステムで使用するヒープサイズ　※サイズを変える場合、松田まで報告をお願いします
#define HEAPSIZE_SAVE       (0x25000)

//--------------------------------------------------------------
//	first_statusの結果BIT
//--------------------------------------------------------------
#define FST_NORMAL_NG_BIT			(1<<0)		//通常セーブ：ミラーリングNG
#define FST_NORMAL_BREAK_BIT		(1<<1)		//通常セーブ：破壊
#define FST_FRONTIER_NG_BIT			(1<<2)		//外部フロンティアセーブ：ミラーリングNG
#define FST_FRONTIER_BREAK_BIT		(1<<3)		//外部フロンティアセーブ：破壊
#define FST_VIDEO_NG_BIT			(1<<4)		//外部ビデオセーブ：ミラーリングNG
#define FST_VIDEO_BREAK_BIT			(1<<5)		//外部ビデオセーブ：破壊

//--------------------------------------------------------------
//  外部セーブ番号
//--------------------------------------------------------------
//※SaveParamExtraTblと並びを同じにしておくこと！！
typedef enum{
  SAVE_EXTRA_ID_REC_MINE,
  SAVE_EXTRA_ID_REC_DL_0,
  SAVE_EXTRA_ID_REC_DL_1,
  SAVE_EXTRA_ID_REC_DL_2,
  SAVE_EXTRA_ID_STREAMING,
  
  SAVE_EXTRA_ID_MAX,
}SAVE_EXTRA_ID;

//==============================================================================
//	型定義
//==============================================================================
///セーブデータ管理ワーク型の不定形型
typedef struct _SAVE_CONTROL_WORK SAVE_CONTROL_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern SAVE_CONTROL_WORK * SaveControl_SystemInit(int heap_id);
extern SAVE_CONTROL_WORK * SaveControl_GetPointer(void);
extern LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl);
extern void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id);
extern BOOL SaveControl_NewDataFlagGet(SAVE_CONTROL_WORK *ctrl);
extern u32 SaveControl_GetLoadResult(const SAVE_CONTROL_WORK * sv);
extern BOOL SaveData_GetExistFlag(const SAVE_CONTROL_WORK * sv);
extern void SaveControl_ClearData(SAVE_CONTROL_WORK * ctrl);
extern void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_GetActualSize(SAVE_CONTROL_WORK *ctrl, u32 *actual_size, u32 *total_size);

//--------------------------------------------------------------
//  外部セーブ関連
//--------------------------------------------------------------
extern LOAD_RESULT SaveControl_Extra_Load(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, int heap_id);
extern void SaveControl_Extra_Unload(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id);
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

