//============================================================================
/**
 *  @file   zukan_detail_procsys.h
 *  @brief  �}�ӏڍ׉�ʋ���PROC
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *
 *  ���W���[�����FZKNDTL_PROC
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_detail);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  ZKNDTL_PROC_RES_CONTINUE,
  ZKNDTL_PROC_RES_FINISH,
}
ZKNDTL_PROC_RESULT;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
typedef struct _ZKNDTL_PROC ZKNDTL_PROC;

// �֐�
typedef ZKNDTL_PROC_RESULT (*ZKNDTL_PROC_FUNC)(ZKNDTL_PROC*, int*, void*, void*, ZKNDTL_COMMON_WORK*);
typedef void (*ZKNDTL_COMMAND_FUNC)(ZKNDTL_PROC*, int*, void*, void*, ZKNDTL_COMMON_WORK*, ZKNDTL_COMMAND);
typedef void (*ZKNDTL_DRAW_3D_FUNC)(ZKNDTL_PROC*, int*, void*, void*, ZKNDTL_COMMON_WORK*);

typedef struct
{
  ZKNDTL_PROC_FUNC     init_func;
  ZKNDTL_PROC_FUNC     main_func;
  ZKNDTL_PROC_FUNC     end_func;
  ZKNDTL_COMMAND_FUNC  cmd_func;
  ZKNDTL_DRAW_3D_FUNC  draw_3d_func;
}
ZKNDTL_PROC_DATA;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
extern ZKNDTL_PROC* ZKNDTL_PROC_SysCreateProc(const ZKNDTL_PROC_DATA* procdata, void* pwork, HEAPID heap_id);
extern void ZKNDTL_PROC_SysDeleteProc(ZKNDTL_PROC* proc);
extern BOOL ZKNDTL_PROC_SysMain(ZKNDTL_PROC* proc, ZKNDTL_COMMON_WORK* cmn);  // TRUE��Ԃ��܂ŌĂяo��������
extern void ZKNDTL_PROC_SysCommand(ZKNDTL_PROC* proc, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd);
extern void ZKNDTL_PROC_SysDraw3D(ZKNDTL_PROC* proc, ZKNDTL_COMMON_WORK* cmn);


extern void* ZKNDTL_PROC_AllocWork(ZKNDTL_PROC* proc, u32 size, HEAPID heap_id);
extern void ZKNDTL_PROC_FreeWork(ZKNDTL_PROC* proc);

