//=============================================================================
/**
 *
 *	@file		pms_select.c
 *	@brief  o^ΟέΘΥοbIπζΚ
 *	@author	hosaka genya
 *	@data		2009.10.20
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/pms_data.h"
#include "system/pms_draw.h"

#include "app/pms_select.h"

#include "system/main.h"

//=============================================================================
/**
 *								θθ`
 */
//=============================================================================
enum
{ 
  PMS_SELECT_HEAP_SIZE = 0x20000,
};

//=============================================================================
/**
 *								\’Μθ`
 */
//=============================================================================

//--------------------------------------------------------------
///	C[N
//==============================================================
struct _PMS_SELECT_WORK {
  int dmy;
};

const GFL_PROC_DATA ProcData_PMSSelect = {
  PMSSelect_Init,
  PMSSelect_Main,
  PMSSelect_Exit,
};

//=============================================================================
/**
 *							vg^CvιΎ
 */
//=============================================================================

//=============================================================================
/**
 *								OφJΦ
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  o^ΟέΘΥοbIπ ϊ»
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT PMSSelect_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  PMS_SELECT_WORK* wk;

  switch( *seq )
  {
  case 0:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PMS_SELECT, PMS_SELECT_HEAP_SIZE );
    wk = GFL_PROC_AllocWork( proc, sizeof(PMS_SELECT_WORK), HEAPID_PMS_SELECT );
    (*seq)++;
    break;
  case 1:
    return GFL_PROC_RES_FINISH;
    break;
  default : GF_ASSERT(0);
  }

  return GFL_PROC_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  o^ΟέΘΥοbIπ IΉ
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT PMSSelect_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  PMS_SELECT_WORK* wk = mywk;

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_PMS_SELECT );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  o^ΟέΘΥοbIπ ε
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT PMSSelect_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
#ifdef PM_DEBUG
  // fobOL[Ε²―ι
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }
#endif 

  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================
/**
 *								staticΦ
 */
//=============================================================================


