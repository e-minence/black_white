//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_fk_sound_anime.c
 *	@brief  �l�V���@�����f���p�@�T�E���h�A�j���[�V�����V�X�e��
 *	@author	tomoya takahashi
 *	@date		2009.12.24
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>


#include  "sound/pm_sndsys.h"

#include  "system/ica_anime.h"

#include "field_fk_sound_anime.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	SE�Đ��萔
//=====================================
#define FIELD_FK_SOUND_ANIME_SE_PLAY_ID (FX32_CONST( 1 ))
#define FIELD_FK_SOUND_ANIME_SE_STOP_ID (FX32_CONST( 2 ))


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�l�V���@�T�E���h�A�j���[�V����
//=====================================
struct _FIELD_FK_SOUND_ANIME 
{
  ICA_ANIME* p_anime;
  fx32 frame;
  FIELD_FK_SOUND_ANIME_DATA param;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void FK_SOUND_ANIME_PlaySE( const FIELD_FK_SOUND_ANIME* cp_wk, u32 idx );
static void FK_SOUND_ANIME_StopSE( const FIELD_FK_SOUND_ANIME* cp_wk, u32 idx );

//----------------------------------------------------------------------------
/**
 *	@brief  �l�V���@SE�A�j���[�V�����V�X�e���@�쐬
 *
 *	@param	cp_param        SE�p�����[�^
 *	@param	arcID           �A�[�J�C�uID
 *	@param	dataID          �f�[�^ID
 *	@param	buf_interval    �X�g���[�~���O�ǂݍ��݁@�P��
 *	@param	heapID          �q�[�vID
 *
 *	@return 
 */
//-----------------------------------------------------------------------------
FIELD_FK_SOUND_ANIME* FIELD_FK_SOUND_ANIME_Create( const FIELD_FK_SOUND_ANIME_DATA* cp_param, u32 arcID, u32 dataID, int buf_interval, HEAPID heapID )
{
  FIELD_FK_SOUND_ANIME* p_wk;

  // �������m��
  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_FK_SOUND_ANIME) );
  
  //���̓ǂݍ���
  p_wk->frame = 0;

  // �A�j���[�V�����ǂݍ���
  p_wk->p_anime = ICA_ANIME_CreateStreamingAlloc( heapID, arcID, dataID, buf_interval );

  GFL_STD_MemCopy( cp_param, &p_wk->param, sizeof(FIELD_FK_SOUND_ANIME_DATA) );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�V���@SE�A�j���[�V�����V�X�e���@�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FK_SOUND_ANIME_Delete( FIELD_FK_SOUND_ANIME* p_wk )
{
  //�A�j���[�V�����j��
  ICA_ANIME_Delete( p_wk->p_anime );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�V���@SE�A�j���[�V�����V�X�e���@�X�V����
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    �A�j���[�V��������
 *	@retval FALSE   �A�j���[�V�����r��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_FK_SOUND_ANIME_Update( FIELD_FK_SOUND_ANIME* p_wk )
{
  BOOL result;
  fx32 max_frame;

  max_frame = ICA_ANIME_GetMaxFrame( p_wk->p_anime )<<FX32_SHIFT;

  // ���łɊ������Ă��Ȃ����H
  if( max_frame <= (p_wk->frame + FIELD_FK_SOUND_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = max_frame - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += FIELD_FK_SOUND_ANIME_SPEED;
  }


  // �A�j���[�V������i�߁A�J�������W��ݒ�
  ICA_ANIME_SetAnimeFrame( p_wk->p_anime, p_wk->frame );

  {
    VecFx32 trans;
    VecFx32 scale;
    VecFx32 rotate;
    BOOL result;

    result = ICA_ANIME_GetTranslate( p_wk->p_anime, &trans );
    GF_ASSERT( result );
    result =ICA_ANIME_GetScale( p_wk->p_anime, &scale );
    GF_ASSERT( result );
    result = ICA_ANIME_GetRotate( p_wk->p_anime, &rotate );
    GF_ASSERT( result );

    if( trans.x == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_X );
    }else if( trans.x == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_X );
    }
    if( trans.y == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Y );
    }else if( trans.y == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Y );
    }
    if( trans.z == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Z );
    }else if( trans.z == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Z );
    }

    if( scale.x == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_X );
    }else if( scale.x == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_X );
    }
    if( scale.y == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Y );
    }else if( scale.y == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Y );
    }
    if( scale.z == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Z );
    }else if( scale.z == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Z );
    }

    if( rotate.x == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_X );
    }else if( rotate.x == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_X );
    }
    if( rotate.y == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Y );
    }else if( rotate.y == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Y );
    }
    if( rotate.z == FIELD_FK_SOUND_ANIME_SE_PLAY_ID ){
      FK_SOUND_ANIME_PlaySE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Z );
    }else if( rotate.z == FIELD_FK_SOUND_ANIME_SE_STOP_ID ){
      FK_SOUND_ANIME_StopSE( p_wk, FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Z );
    }
  }
  
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V���������`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    �A�j���[�V�����r��
 *	@retval FALSE   �A�j���[�V��������
 */
//-----------------------------------------------------------------------------
BOOL FIELD_FK_SOUND_ANIME_IsAnime( const FIELD_FK_SOUND_ANIME* cp_wk )
{
  BOOL result;
  fx32 max_frame;

  max_frame = ICA_ANIME_GetMaxFrame( cp_wk->p_anime )<<FX32_SHIFT;

  // ���łɊ������Ă��Ȃ����H
  if( max_frame <= (cp_wk->frame + FIELD_FK_SOUND_ANIME_SPEED) )
  {
    result = TRUE;
  }
  else
  {
    result = FALSE;
  }

  return result;
}







//-----------------------------------------------------------------------------
/**
 *    private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  SE�̍Đ�
 *
 *	@param	cp_wk ���[�N
 *	@param	idx   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FK_SOUND_ANIME_PlaySE( const FIELD_FK_SOUND_ANIME* cp_wk, u32 idx )
{
  GF_ASSERT( idx < FIELD_FK_SOUND_ANIME_SE_MAX );
  
  if( cp_wk->param.se_tbl[ idx ] != FIELD_FK_SOUND_ANIME_SE_NONE )
  {
    PMSND_PlaySE( cp_wk->param.se_tbl[ idx ] );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  SE���~����
 *
 *	@param	cp_wk   ���[�N
 *	@param	idx     �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FK_SOUND_ANIME_StopSE( const FIELD_FK_SOUND_ANIME* cp_wk, u32 idx )
{
  GF_ASSERT( idx < FIELD_FK_SOUND_ANIME_SE_MAX );
  
  if( cp_wk->param.se_tbl[ idx ] != FIELD_FK_SOUND_ANIME_SE_NONE )
  {
    SEPLAYER_ID playerID = PMSND_GetSE_DefaultPlayerID( cp_wk->param.se_tbl[ idx ] );
    
    PMSND_StopSE_byPlayerID( playerID );
  }
}




