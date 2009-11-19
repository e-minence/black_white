//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldskill_mapeff.c
 *	@brief  �t�B�[���h�Z�@�]�[���G�t�F�N�g
 *	@author	tomoya takahashi
 *	@date		2009.11.19
 *
 *	���W���[�����FFIELDSKILL_MAPEFF
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "fieldskill_mapeff.h"

#ifdef PM_DEBUG

#define DEBUG_FLASH_START_FAR

#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�X�L���@�]�[�����Ƃ̌���
//=====================================
struct _FIELDSKILL_MAPEFF 
{
	FIELD_FLASH * field_flash;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �t���b�V�����[�N�Ǘ�
static void SKILL_MAPEFF_CreateFlash( FIELDSKILL_MAPEFF* p_wk, FIELDSKILL_MAPEFF_MSK msk, HEAPID heapID );
static void SKILL_MAPEFF_DeleteFlash( FIELDSKILL_MAPEFF* p_wk );
static void SKILL_MAPEFF_MainFlash( FIELDSKILL_MAPEFF* p_wk );
static void SKILL_MAPEFF_DrawFlash( FIELDSKILL_MAPEFF* p_wk );




//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@���ʊǗ��V�X�e���@����
 *
 *  @param  msk       �ݒ�}�X�N
 *	@param	heapID    �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FIELDSKILL_MAPEFF* FIELDSKILL_MAPEFF_Create( FIELDSKILL_MAPEFF_MSK msk, HEAPID heapID )
{
  FIELDSKILL_MAPEFF* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDSKILL_MAPEFF) );

  // �t���b�V������
  SKILL_MAPEFF_CreateFlash( p_wk, msk, heapID );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j������
 *
 *	@param	p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDSKILL_MAPEFF_Delete( FIELDSKILL_MAPEFF* p_wk )
{
  // �t���b�V���j��
  SKILL_MAPEFF_DeleteFlash( p_wk );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDSKILL_MAPEFF_Main( FIELDSKILL_MAPEFF* p_wk )
{
  // �t���b�V�����C��
  SKILL_MAPEFF_MainFlash( p_wk );


#ifdef DEBUG_FLASH_START_FAR
  // �t���b�V������
  // tomoya takahashi
  if( FIELDSKILL_MAPEFF_IsFlash( p_wk ) )
  {
    FIELD_FLASH* p_flash;
    u32 status;

    p_flash = FIELDSKILL_MAPEFF_GetFlash( p_wk );
    status  = FIELD_FLASH_GetStatus( p_flash );
    
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
    {
      if( status == FIELD_FLASH_STATUS_NEAR )
      {
        FIELD_FLASH_Control( p_flash, FIELD_FLASH_REQ_FADEOUT );
      }
      else if( status == FIELD_FLASH_STATUS_FAR )
      {
        FIELD_FLASH_Control( p_flash, FIELD_FLASH_REQ_ON_NEAR );
      }
    }
  }
#endif

}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�擮��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDSKILL_MAPEFF_Draw( FIELDSKILL_MAPEFF* p_wk )
{
  // �t���b�V���`��
  SKILL_MAPEFF_DrawFlash( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V�������삵�Ă��邩�`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ���Ă���
 *	@retval FALSE   ���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELDSKILL_MAPEFF_IsFlash( const FIELDSKILL_MAPEFF* cp_wk )
{
  if( cp_wk->field_flash )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V�����[�N�擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �t���b�V�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_FLASH* FIELDSKILL_MAPEFF_GetFlash( const FIELDSKILL_MAPEFF* cp_wk )
{
  GF_ASSERT( cp_wk->field_flash );
  return cp_wk->field_flash;
}




//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V���V�X�e���̐���
 *
 *	@param	p_wk    ���[�N
 *	@param	msk     �}�X�N
 *	@param  heapID  �q�[�vID
 *
 *	�t���b�V���́A�����ӏ������Ȃ��̂ŁA�K�v�ȂƂ��ɂ�����������
 */
//-----------------------------------------------------------------------------
static void SKILL_MAPEFF_CreateFlash( FIELDSKILL_MAPEFF* p_wk, FIELDSKILL_MAPEFF_MSK msk, HEAPID heapID )
{
  if( (msk & (FIELDSKILL_MAPEFF_MSK_FLASH_NEAR|FIELDSKILL_MAPEFF_MSK_FLASH_FAR)) )
  {
    p_wk->field_flash = FIELD_FLASH_Create( heapID );

    // �����ݒ肳��Ă���
    GF_ASSERT( (msk & (FIELDSKILL_MAPEFF_MSK_FLASH_NEAR|FIELDSKILL_MAPEFF_MSK_FLASH_FAR)) != (FIELDSKILL_MAPEFF_MSK_FLASH_NEAR|FIELDSKILL_MAPEFF_MSK_FLASH_FAR) );
    

#ifdef DEBUG_FLASH_START_FAR
    // @TODO�@���FAR�Ŏn�܂�
    FIELD_FLASH_Control( p_wk->field_flash, FIELD_FLASH_REQ_ON_FAR );
#else
    // FAR��D��
    if( msk & FIELDSKILL_MAPEFF_MSK_FLASH_FAR )
    {
      FIELD_FLASH_Control( p_wk->field_flash, FIELD_FLASH_REQ_ON_FAR );
    }
    else
    {
      FIELD_FLASH_Control( p_wk->field_flash, FIELD_FLASH_REQ_ON_NEAR );
    }
#endif
  }
  else
  {
    p_wk->field_flash = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V�����[�N��j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SKILL_MAPEFF_DeleteFlash( FIELDSKILL_MAPEFF* p_wk )
{
  FIELD_FLASH_Delete( p_wk->field_flash );
  p_wk->field_flash = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V�����C������
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void SKILL_MAPEFF_MainFlash( FIELDSKILL_MAPEFF* p_wk )
{
  FIELD_FLASH_Update( p_wk->field_flash );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V���`�揈��
 *
 *	@param	p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SKILL_MAPEFF_DrawFlash( FIELDSKILL_MAPEFF* p_wk )
{
  FIELD_FLASH_Draw( p_wk->field_flash );
}



