//=============================================================================
/**
 *
 *	@file		bag_param.c
 *	@brief
 *	@author		hosaka genya
 *	@data		2009.10.21
 *
 */
//=============================================================================
#include "app/bag.h"
//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

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

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�b�O�p�����[�^����
 *
 *	@param	GAMEDATA* gmData  �Q�[���f�[�^
 *	@param	ITEMCHECK_WORK* icwk �A�C�e���`�F�b�N���[�N
 *	@param	mode �o�b�O�N�����[�h
 *
 *	@retval BAG_PARAM* �o�b�O�p�����[�^(ALLOC�ς�)
 */
//-----------------------------------------------------------------------------
BAG_PARAM* BAG_CreateParam( GAMEDATA* gmData, const ITEMCHECK_WORK* icwk, BAG_MODE mode )
{
  SAVE_CONTROL_WORK* saveControl;
  BAG_PARAM * bag;

  saveControl = GAMEDATA_GetSaveControlWork( gmData );

  bag = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(BAG_PARAM));

  bag->p_config     = SaveData_GetConfig( saveControl );
  bag->p_mystatus   = GAMEDATA_GetMyStatus( gmData );
  bag->p_bagcursor  = GAMEDATA_GetBagCursor( gmData );
  bag->p_myitem     = GAMEDATA_GetMyItem( gmData );
  
  GFL_STD_MemCopy(icwk , &bag->icwk, sizeof(ITEMCHECK_WORK));

  bag->mode       = mode;
  bag->cycle_flg  = ( PLAYERWORK_GetMoveForm( GAMEDATA_GetMyPlayerWork(gmData) ) == PLAYER_MOVE_FORM_CYCLE );
  
  return bag;
}


