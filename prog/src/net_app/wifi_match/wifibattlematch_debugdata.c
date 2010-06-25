//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_debugdata.c
 *	@brief  �f�o�b�O�p�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2010.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifdef PM_DEBUG

#include "wifibattlematch_debugdata.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
DEBUGWIN_REPORT_DATA *DEBUGWIN_REPORT_DATA_GetInstance( void )
{
  static DEBUGWIN_REPORT_DATA g_debug_report_data = {0};
  return &g_debug_report_data;
}


DEBUGWIN_BTLBGM_DATA *DEBUGWIN_BTLBGM_DATA_GetInstance( void )
{
  static DEBUGWIN_BTLBGM_DATA g_debug_btlbgm_data = {0};
  return &g_debug_btlbgm_data;
}

BOOL *DEBUGWIN_ATLASDIRTY_GetFlag( void )
{
  static BOOL s_atlas_dirty_flag  = FALSE;
  return &s_atlas_dirty_flag;
}

BOOL *DEBUGWIN_SERVERTIME_GetFlag( void )
{
  static BOOL s_serverttime_flag  = FALSE;
  return &s_serverttime_flag;
}
#endif //PM_DEBUG
