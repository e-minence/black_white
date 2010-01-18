//============================================================================
/**
 *  @file   shinka_demo.h
 *  @brief  �i���f��
 *  @author Koji Kawada
 *  @data   2010.01.13
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO
 */
//============================================================================

#pragma once

#include "poke_tool/pokeparty.h"

typedef struct _SHINKA_DEMO_PARAM SHINKA_DEMO_PARAM;

FS_EXTERN_OVERLAY(shinka_demo);
extern  const GFL_PROC_DATA   ShinkaDemoProcData;
extern  SHINKA_DEMO_PARAM*  SHINKADEMO_AllocParam( HEAPID heapID, const POKEPARTY* ppt, u16 after_mons_no, u8 pos, u8 cond );
extern  void                SHINKADEMO_FreeParam( SHINKA_DEMO_PARAM* sdp );
