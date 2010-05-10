//=============================================================================
/**
 *
 *	@file		intro_mcss.c
 *	@brief  �C���g���f�� MCSS���b�p�[
 *	@author	hosaka genya
 *	@data		2009.12.18
 *
 */
//=============================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// MCSS �|�P����
#include "system/mcss_tool.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

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

#define	POKE_ANIME_WAIT_MIN		( 200 )
#define	POKE_ANIME_WAIT_VAL		( 20 )


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
  HEAPID          heap_id;
  MCSS_SYS_WORK*  mcss;
  MCSS_WORK*      mcss_work[ MCSS_ID_MAX ];
	BOOL	animeFlag[ MCSS_ID_MAX ];
	u32	pokeAnimeWait;
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
static void McssCallBackFrame( u32 data, fx32 currentFrame );


//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS����
 *
 *	@param	HEAPID heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
INTRO_MCSS_WORK* INTRO_MCSS_Create( HEAPID heap_id, INTRO_SCENE_ID scene )
{
  INTRO_MCSS_WORK* wk;

  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INTRO_MCSS_WORK) );

  wk->mcss    = MCSS_Init( MCSS_ID_MAX, heap_id );
  wk->heap_id = heap_id;

  // intro_g3d�ƕ��s�����邽�߂ɂ��炵����
	if( scene != INTRO_SCENE_ID_05_RETAKE_YESNO ){
	  MCSS_SetTextureTransAdrs( wk->mcss, 0x70000 );
	}
	MCSS_SetTexPaletteTransAdrs( wk->mcss, 0x4000 );
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

	wk->animeFlag[id] = FALSE;
	MCSS_SetAnimCtrlCallBack( wk->mcss_work[id], (u32)&wk->animeFlag[id], McssCallBackFrame, 0 );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS���[�N�����i�|�P����)
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	px
 *	@param	py
 *	@param	pz
 *	@param	monsno
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_AddPoke( INTRO_MCSS_WORK* wk, fx32 px, fx32 py, fx32 pz, int monsno, u8 id )
{
  MCSS_ADD_WORK   add;
  POKEMON_PARAM*  pp;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] == NULL );

  pp = PP_Create( monsno, 0, 0, wk->heap_id );
      
  MCSS_TOOL_MakeMAWPP( pp, &add, MCSS_DIR_FRONT );

  GFL_HEAP_FreeMemory( pp );

  wk->mcss_work[id] = MCSS_Add( wk->mcss, px, py, pz, &add );
  MCSS_SetScale( wk->mcss_work[id], &scale );
	MCSS_SetAnmStopFlag( wk->mcss_work[id] );

	wk->animeFlag[id] = FALSE;
	wk->pokeAnimeWait = POKE_ANIME_WAIT_MIN + POKE_ANIME_WAIT_VAL * GFL_STD_MtRand( 5 );
	MCSS_SetAnimCtrlCallBack( wk->mcss_work[id], (u32)&wk->animeFlag[id], McssCallBackFrame, 0 );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�w�胏�[�N�̕\��/��\���ؑ�
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

  if( is_visible == TRUE )
  {
    MCSS_ResetVanishFlag( wk->mcss_work[id] );
  }
  else
  {
    MCSS_SetVanishFlag( wk->mcss_work[id] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�A�j���[�V�����C���f�b�N�X
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	id
 *	@param	anm_idx 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetAnimeIndex( INTRO_MCSS_WORK* wk, u8 id, int anm_idx )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );

  MCSS_SetAnimeIndex( wk->mcss_work[id], anm_idx );
	MCSS_ResetAnmStopFlag( wk->mcss_work[id] );

	wk->animeFlag[id] = FALSE;
	MCSS_SetAnimCtrlCallBack( wk->mcss_work[id], (u32)&wk->animeFlag[id], McssCallBackFrame, 0 );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�A�j���[�V�����I���`�F�b�N
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	id
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL INTRO_MCSS_CheckAnime( INTRO_MCSS_WORK * wk, u8 id )
{
	return wk->animeFlag[id];
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�A�j���[�V�����I���`�F�b�N�p�t���O�����Z�b�g
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	id
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_ResetAnimeFlag( INTRO_MCSS_WORK * wk, u8 id )
{
	wk->animeFlag[id] = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�g��k���Z�b�g
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	id
 *	@param	VecFx32* scale 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetScale( INTRO_MCSS_WORK* wk, u8 id, VecFx32* scale )
{ 
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );

  MCSS_SetScale( wk->mcss_work[id], scale );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�A���t�@�u�����h�Z�b�g
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	id
 *	@param	alpha 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetAlpha( INTRO_MCSS_WORK* wk, u8 id, u8 alpha )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );
    
  // �u�����h���[�h�A�Ώۖʎw��
  G2_SetBlendAlpha( GX_PLANEMASK_BG0, GX_PLANEMASK_BG3|GX_PLANEMASK_BG0, 0, 0 );

  // �l�̐���
  alpha = MATH_CLAMP( alpha, 0, 31 );

  HOSAKA_Printf("alpha=%d \n", alpha );

  MCSS_SetAlpha( wk->mcss_work[id], alpha );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���p�`�t���O
 *
 *	@param	INTRO_MCSS_WORK* wk
 *	@param	is_mepachi_flag 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetMepachi( INTRO_MCSS_WORK* wk, u8 id, BOOL is_mepachi_flag )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );

  if( is_mepachi_flag )
  {
    MCSS_SetMepachiFlag( wk->mcss_work[id] );
  }
  else
  {
    MCSS_ResetMepachiFlag( wk->mcss_work[id] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�p���b�g�t�F�[�h
 *
 *  @param[in]	id        �������[�N�Ǘ�ID
 *  @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 *  @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 *  @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 *  @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MCSS_SetPaletteFade( INTRO_MCSS_WORK* wk, u8 id, u8 start_evy, u8 end_evy, u8 wait, u32 rgb )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->mcss );
  GF_ASSERT( id < MCSS_ID_MAX );
  GF_ASSERT( wk->mcss_work[id] );

  MCSS_SetPaletteFade( wk->mcss_work[id], start_evy, end_evy, wait, rgb );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �A�j���I�����ɌĂ΂��R�[���o�b�N�֐�
 *	@retval
 */
//-----------------------------------------------------------------------------
static void McssCallBackFrame( u32 data, fx32 currentFrame )
{
	BOOL * animeFlag = (BOOL *)data;
	*animeFlag = TRUE;
}

// ���m��p...
BOOL INTRO_MCSS_MoveX( INTRO_MCSS_WORK * wk, u8 id, fx32 mx, fx32 px )
{
	BOOL	flg;
	VecFx32	pos;

	MCSS_GetPosition( wk->mcss_work[id], &pos );

	flg = TRUE;
	pos.x += mx;
	if( pos.x != px ){
		if( ( mx > 0 && pos.x > px ) ||
				( mx < 0 && pos.x < px ) ){
			pos.x = px;
		}else{
			flg = FALSE;
		}
	}

	MCSS_SetPosition( wk->mcss_work[id], &pos );

	if( wk->animeFlag[id] == TRUE ){
		// �~�߂��Ⴄ�Ɩڃp�`�������Ȃ��Ȃ�̂ŁA��~�A�j���ɐ؂�ւ���
		// �A�j���͋����I�ɒ�~��Ԃɂ��Ă���
//		INTRO_MCSS_SetAnimeIndex( wk, id, 1 );
//		wk->animeFlag[id] = TRUE;
		MCSS_SetAnmStopFlag( wk->mcss_work[id] );
	}

	return flg;
}

BOOL INTRO_MCSS_PokeFall( INTRO_MCSS_WORK * wk, fx32 my, fx32 end )
{
	BOOL	flg;
	VecFx32	pos;

	MCSS_GetPosition( wk->mcss_work[1], &pos );
	pos.y -= my;
	if( pos.y <= end ){
		pos.y = end;
		flg = TRUE;
	}else{
		flg = FALSE;
	}
	MCSS_SetPosition( wk->mcss_work[1], &pos );

	return flg;
}

// �|�P�����A�j���Ď�
void INTRO_MCSS_PokeAnime( INTRO_MCSS_WORK * wk )
{
	if( wk->animeFlag[1] == FALSE ){
		return;
	}

	if( wk->pokeAnimeWait == 0 ){
		wk->pokeAnimeWait = POKE_ANIME_WAIT_MIN + POKE_ANIME_WAIT_VAL * GFL_STD_MtRand( 5 );
		INTRO_MCSS_SetAnimeIndex( wk, 1, 0 );
		return;
	}

	MCSS_SetAnmStopFlag( wk->mcss_work[1] );
	wk->pokeAnimeWait--;
}
