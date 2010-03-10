//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_flash.c
 *	@brief  �t���b�V�����[�N
 *	@author	tomoya takahashi
 *	@date		2009.11.18
 *
 *	���W���[�����FFIELD_FLASH
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "fld_wipe_3dobj.h"

#include "field_flash.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// NEAR scale alpha
#define FLASH_NEAR_SCALE  ( 0x3000 )
#define FLASH_NEAR_ALPHA  ( 31 )

// FAR scale alpha
#define FLASH_FAR_SCALE  ( 0xAA00 )
#define FLASH_FAR_ALPHA  ( 31 )

// OFF scale
#define FLASH_OFF_SCALE  ( 0 )

// �t�F�[�h����
#define FLASH_FADE_TIME ( 15 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�X�P�[���l�Ǘ�
//=====================================
typedef struct 
{
  
  fx32 scale;
  u8   alpha;

  // �t�F�[�h
  u16 count;
  u16 count_max;
  fx32 start_scale;
  fx32 move_scale;

  s16 start_alpha;
  s16 move_alpha;
  
} FLASH_SCALE;


//-------------------------------------
///	�t���b�V���Ǘ����[�N
//=====================================
struct _FIELD_FLASH 
{
  FLD_WIPEOBJ*  p_wipe;
  FLASH_SCALE   scale;
  u16           status;
  u16           req;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	���N�G�X�g�̔��f
//=====================================
static void FLASH_REQ_OnNear( FIELD_FLASH* p_wk );
static void FLASH_REQ_FadeOut( FIELD_FLASH* p_wk );
static void FLASH_REQ_OnFar( FIELD_FLASH* p_wk );
static void FLASH_REQ_Off( FIELD_FLASH* p_wk );
 
//-------------------------------------
///	�X�P�[���Ǘ�
//=====================================
static void FLASH_SCALE_Init( FLASH_SCALE* p_wk );
static void FLASH_SCALE_Exit( FLASH_SCALE* p_wk );
static void FLASH_SCALE_StartFade( FLASH_SCALE* p_wk, fx32 start, fx32 end, u8 start_alpha, u8 end_alpha, u32 time );
static BOOL FLASH_SCALE_MainFade( FLASH_SCALE* p_wk );
static BOOL FLASH_SCALE_IsFade( const FLASH_SCALE* cp_wk );
static fx32 FLASH_SCALE_GetScale( const FLASH_SCALE* cp_wk );
static void FLASH_SCALE_SetScale( FLASH_SCALE* p_wk, fx32 scale );
static u8 FLASH_SCALE_GetAlpha( const FLASH_SCALE* cp_wk );
static void FLASH_SCALE_SetAlpha( FLASH_SCALE* p_wk, u8 alpha );



//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V���V�X�e������
 *
 *	@param	heapID  �q�[�vID
 *
 *	@return
 */
//-----------------------------------------------------------------------------
FIELD_FLASH* FIELD_FLASH_Create( HEAPID heapID )
{
  FIELD_FLASH* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_FLASH) );

  p_wk->p_wipe  = FLD_WIPEOBJ_Create( heapID );
  p_wk->status  = FIELD_FLASH_STATUS_NONE;
  p_wk->req     = FIELD_FLASH_REQ_MAX;

  FLASH_SCALE_Init( &p_wk->scale );

  return p_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V�����[�N�@�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FLASH_Delete( FIELD_FLASH* p_wk )
{
  // NULL�Ȃ烀�V
  if(p_wk==NULL){ return; };
  
  FLASH_SCALE_Exit( &p_wk->scale );
  FLD_WIPEOBJ_Delete( p_wk->p_wipe );
  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V��  �X�V
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FLASH_Update( FIELD_FLASH* p_wk )
{
  // NULL�Ȃ烀�V
  if(p_wk==NULL){ return; };

  // ���N�G�X�g����
  if( p_wk->req != FIELD_FLASH_REQ_MAX )
  {
    static void (*const cpFunc[])( FIELD_FLASH* ) =
    {
      FLASH_REQ_OnNear,
      FLASH_REQ_FadeOut,
      FLASH_REQ_OnFar,
      FLASH_REQ_Off,
    };
    cpFunc[ p_wk->req ]( p_wk );
    p_wk->req = FIELD_FLASH_REQ_MAX;
  }

  if( p_wk->status == FIELD_FLASH_STATUS_FADEOUT )
  {
    BOOL result;
    // �t�F�[�h����
    result = FLASH_SCALE_MainFade( &p_wk->scale );
    
    if( result )
    {
      p_wk->status = FIELD_FLASH_STATUS_FAR;
    }
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V���@�`��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FLASH_Draw( FIELD_FLASH* p_wk )
{
  // NULL�Ȃ烀�V
  if(p_wk==NULL){ return; };
  
  // �`��
  FLD_WIPEOBJ_Main( p_wk->p_wipe, FLASH_SCALE_GetScale( &p_wk->scale ), FLASH_SCALE_GetAlpha( &p_wk->scale ) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V���@�Ǘ�
 *
 *	@param	p_wk  ���[�N
 *	@param	req   ���N�G�X�g
 */
//-----------------------------------------------------------------------------
void FIELD_FLASH_Control( FIELD_FLASH* p_wk, FIELD_FLASH_REQ req )
{
  GF_ASSERT( p_wk );

  p_wk->req = req;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V����Ԃ̎擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return ���
 */
//-----------------------------------------------------------------------------
FIELD_FLASH_STATUS FIELD_FLASH_GetStatus( const FIELD_FLASH* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->status;
}





//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  ������
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FLASH_SCALE_Init( FLASH_SCALE* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(FLASH_SCALE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FLASH_SCALE_Exit( FLASH_SCALE* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(FLASH_SCALE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �t�F�[�h�J�n
 *
 *	@param	p_wk      ���[�N
 *	@param	start     �J�n�X�P�[��
 *	@param	end       �I���X�P�[��
 *	@param	time      �t�F�[�h�V���N
 */
//-----------------------------------------------------------------------------
static void FLASH_SCALE_StartFade( FLASH_SCALE* p_wk, fx32 start, fx32 end, u8 start_alpha, u8 end_alpha, u32 time )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( time > 0 );
  
  p_wk->scale       = start;
  p_wk->start_scale = start;
  p_wk->move_scale  = end - start;
  p_wk->alpha       = start_alpha;
  p_wk->start_alpha = start_alpha;
  p_wk->move_alpha  = end_alpha - start_alpha;
  p_wk->count       = 0;
  p_wk->count_max   = time;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �t�F�[�h���C��
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE  �t�F�[�h����
 *	@retval FALSE �t�F�[�h�r��
 */
//-----------------------------------------------------------------------------
static BOOL FLASH_SCALE_MainFade( FLASH_SCALE* p_wk )
{
  if( p_wk->count < p_wk->count_max )
  {
    p_wk->count++;

    // �X�P�[���l�����߂�
    p_wk->scale = p_wk->start_scale + FX_Div( FX_Mul(p_wk->move_scale, p_wk->count<<FX32_SHIFT), p_wk->count_max << FX32_SHIFT );

    // ���l�����߂�
    p_wk->alpha = p_wk->start_alpha + ((p_wk->move_alpha * p_wk->count) / p_wk->count_max);
    return FALSE; 
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �t�F�[�h���`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *  
 *	@retval TRUE    �t�F�[�h��
 *	@retval FALSE   �t�F�[�h���ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL FLASH_SCALE_IsFade( const FLASH_SCALE* cp_wk )
{
  if( cp_wk->count < cp_wk->count_max )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �X�P�[���l�擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �X�P�[���l
 */
//-----------------------------------------------------------------------------
static fx32 FLASH_SCALE_GetScale( const FLASH_SCALE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->scale;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�P�[�����[�N  �X�P�[���l�ݒ�
 *	@param	p_wk    ���[�N
 *	@param	scale   �X�P�[���l
 */
//-----------------------------------------------------------------------------
static void FLASH_SCALE_SetScale( FLASH_SCALE* p_wk, fx32 scale )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( FLASH_SCALE_IsFade(p_wk) == FALSE );

  p_wk->scale = scale;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A���t�@�l�̎擾
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static u8 FLASH_SCALE_GetAlpha( const FLASH_SCALE* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���l�̐ݒ�
 *
 *	@param	p_wk      ���[�N
 *	@param	alpha     ��
 */
//-----------------------------------------------------------------------------
static void FLASH_SCALE_SetAlpha( FLASH_SCALE* p_wk, u8 alpha )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( FLASH_SCALE_IsFade(p_wk) == FALSE );

  p_wk->alpha = alpha;
}





//----------------------------------------------------------------------------
/**
 *	@brief  ON���N�G�X�g�̔��f
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FLASH_REQ_OnNear( FIELD_FLASH* p_wk )
{
  // ON
  p_wk->status = FIELD_FLASH_STATUS_NEAR;
  FLASH_SCALE_SetScale( &p_wk->scale, FLASH_NEAR_SCALE );
  FLASH_SCALE_SetAlpha( &p_wk->scale, FLASH_NEAR_ALPHA );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g�̔��f
 *
 *	@param	p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void FLASH_REQ_FadeOut( FIELD_FLASH* p_wk )
{
  // 
  p_wk->status = FIELD_FLASH_STATUS_FADEOUT;
  FLASH_SCALE_StartFade( &p_wk->scale, FLASH_NEAR_SCALE, FLASH_FAR_SCALE, FLASH_NEAR_ALPHA, FLASH_FAR_ALPHA, FLASH_FADE_TIME );

  // �t���b�V���g�p��
  PMSND_PlaySE( SEQ_SE_FLD_127 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�[�̃t���b�V�����C�v��ݒ�
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void FLASH_REQ_OnFar( FIELD_FLASH* p_wk )
{
  // ON
  p_wk->status = FIELD_FLASH_STATUS_FAR;
  FLASH_SCALE_SetScale( &p_wk->scale, FLASH_FAR_SCALE );
  FLASH_SCALE_SetAlpha( &p_wk->scale, FLASH_FAR_ALPHA );
}

//----------------------------------------------------------------------------
/**
 *	@brief  OFF���N�G�X�g�̔��f
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FLASH_REQ_Off( FIELD_FLASH* p_wk )
{
  // OFF
  p_wk->status = FIELD_FLASH_STATUS_NONE;
  FLASH_SCALE_SetScale( &p_wk->scale, FLASH_OFF_SCALE );
}

