//=============================================================================
/**
 *
 *	@file		intro_mcss.c
 *	@brief
 *	@author		hosaka genya
 *	@data		2009.12.18
 *
 */
//=============================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "intro_mcss.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  MCSS_ID_MAX = 8,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _INTRO_MCSS_WORK
{
  MCSS_SYS_WORK* mcss;
  MCSS_WORK* mcss_work[ MCSS_ID_MAX ];
};


//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================



//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS����
 *
 *	@param	HEAPID heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
INTRO_MCSS_WORK* INTRO_MCSS_Create( HEAPID heap_id )
{
  INTRO_MCSS_WORK* wk;

  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INTRO_MCSS_WORK) );

  wk->mcss = MCSS_Init( MCSS_ID_MAX, heap_id );

  MCSS_SetTextureTransAdrs( wk->mcss, 0 );
  MCSS_SetOrthoMode( wk->mcss );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�I��
 *
 *	@param	INTRO_MCSS_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_Exit( INTRO_MCSS_WORK* wk )
{
  int i;

  for( i=0; i<MCSS_ID_MAX; i++ )
  {
    if( wk->mcss_work[i] )
    {
      MCSS_Del( wk->mcss, wk->mcss_work[i] );
    }
  }

  MCSS_Exit( wk->mcss );
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�又��
 *
 *	@param	INTRO_MCSS_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_Main( INTRO_MCSS_WORK* wk )
{
  MCSS_Main( wk->mcss );
  MCSS_Draw( wk->mcss );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS���[�N����
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	px
 *	@param	py
 *	@param	pz
 *	@param	MCSS_ADD_WORK* add
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_Add( INTRO_MCSS_WORK* wk, fx32 px, fx32 py, fx32 pz, const MCSS_ADD_WORK* add, u8 id )
{
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};

  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] == NULL );

  wk->mcss_work[id] = MCSS_Add( wk->mcss, px, py, pz, add );

  MCSS_SetScale( wk->mcss_work[id], &scale );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�w�胏�[�N�̕\����\���ؑ�
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	is_visible
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetVisible( INTRO_MCSS_WORK* wk, BOOL is_visible, u8 id )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );

  if( is_visible )
  {
    MCSS_SetVanishFlag( wk->mcss_work[id] );
  }
  else
  {
    MCSS_ResetVanishFlag( wk->mcss_work[id] );
  }
}




