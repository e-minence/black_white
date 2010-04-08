//======================================================================
/**
 * @file  field_mystery_gift.c
 * @brief �ӂ����f�[�^�ւ̃A�N�Z�X���b�p�[
 * @date  2010.02.14
 * @author  tamada GAMEFREAK inc.
 */
//======================================================================

#include <gflib.h>

#include "savedata/mystery_data.h"

#include "field_mystery_gift.h"

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief
 *
 * @param fd
 * @param index
 * @param gpd   ���蕨�f�[�^���󂯎�邽�߂̃��[�N
 *
 * @retval  gpd   ���p�\�ȏꍇ
 * @retval  NULL  ���p�\�łȂ��ꍇ
 */
//--------------------------------------------------------------
static GIFT_PACK_DATA * getEnableGiftData( MYSTERY_DATA * fd, int index, GIFT_PACK_DATA * gpd )
{
  if ( MYSTERYDATA_IsExistsCard( fd, index ) == FALSE ) return NULL;
  if ( MYSTERYDATA_IsHavePresent( fd, index ) == TRUE ) return NULL;

  if( FALSE == MYSTERYDATA_GetCardData( fd, index, gpd ) ){
    return NULL;
  }
  if ( gpd->gift_type == MYSTERYGIFT_TYPE_NONE ) return NULL;
  //���̂Ƃ���Ή�����f�[�^��GPower�܂�
  if ( gpd->gift_type > MYSTERYGIFT_TYPE_POWER ) return NULL;

  return gpd;
}

//--------------------------------------------------------------
/**
 * @brief ���p�\�ȑ��蕨�f�[�^�̎擾
 *
 * @param   fd      �s�v�c�Z�[�u�f�[�^�ւ̃|�C���^
 * @param   index   ���蕨�f�[�^�ʒu�̃C���f�b�N�X���󂯎�邽�߂̃|�C���^
 * @param   gpd     ���蕨�f�[�^���󂯎�邽�߂̃��[�N
 *
 * @retval    gpd     ���p�\�ȏꍇ
 * @retval    NULL    ���p�\�łȂ��ꍇ
 */
//--------------------------------------------------------------
GIFT_PACK_DATA * FIELD_MYSTERYDATA_GetGiftData( MYSTERY_DATA * fd, int * index, GIFT_PACK_DATA * gpd )
{
  int i;
  for (i = 0; i < GIFT_DATA_MAX; i ++ )
  {
    if (getEnableGiftData( fd, i, gpd ) != NULL)
    {
      *index = i;
      return gpd;
    }
  }
  return NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void FIELD_MYSTERYDATA_SetReceived( MYSTERY_DATA * fd, int index )
{
  MYSTERYDATA_SetHavePresent( fd, index );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u8 FIELD_MYSTERYDATA_GetGiftType( MYSTERY_DATA * fd )
{
  int index;
  GIFT_PACK_DATA aGift;
  const GIFT_PACK_DATA * gpd = FIELD_MYSTERYDATA_GetGiftData( fd, &index, &aGift );
  if ( gpd == NULL ) return MYSTERYGIFT_TYPE_NONE;
  return gpd->gift_type;
}

