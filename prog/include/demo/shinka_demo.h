//============================================================================================
/**
 * @file    shinka_demo.h
 * @brief   12�������p�ɊԂɍ��킹�ō�����i���f���i�f���ƌĂׂ�㕨����Ȃ��̂ŁA���Ȃ����Ă��������j
 * @author  soga
 * @date    2009.12.09
 */
//============================================================================================

#pragma once

#include "poke_tool/pokeparty.h"

typedef struct _SHINKA_DEMO_PARAM SHINKA_DEMO_PARAM;

FS_EXTERN_OVERLAY(shinka_demo);
extern  const GFL_PROC_DATA   ShinkaDemoProcData;
extern  SHINKA_DEMO_PARAM*  SHINKADEMO_AllocParam( HEAPID heapID, const POKEPARTY* ppt, u16 after_mons_no, u8 pos, u8 cond );
extern  BOOL                SHINKADEMO_CheckExecute( SHINKA_DEMO_PARAM* param );
