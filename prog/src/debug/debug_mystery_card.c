//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_mystery_card.c
 *	@brief  �f�o�b�O�p�ӂ����Ȃ�������̍쐬  
 *	@author	Toru=Nagihashi
 *	@date		2010.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//���W���[��
#include "poke_tool/monsno_def.h"
#include "item/itemsym.h"

//�Z�[�u�f�[�^
#include "savedata/mystery_data.h"

//�O�����J
#include "debug/debug_mystery_card.h"
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

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P��������̃f�[�^���쐬
 *
 *	@param	GIFT_PACK_DATA *p_data  �f�[�^
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftPokeData( GIFT_PACK_DATA *p_data, u32 event_id )
{ 
  GFL_STD_MemClear( p_data, sizeof(GIFT_PACK_DATA) );
  //�|�P�����쐬
  { 
    GIFT_PRESENT_POKEMON  *p_poke = &p_data->data.pokemon;
    p_poke->mons_no = MONSNO_KORATTA;
    p_poke->form_no = 0;
    p_poke->waza1   = 1;
    p_poke->oyaname[0]  = L'�f';
    p_poke->oyaname[1]  = L'�o';
    p_poke->oyaname[2]  = L'�b';
    p_poke->oyaname[3]  = L'�O';
    p_poke->oyaname[4]  = 0xffff;
    p_poke->oyaname[4]  = 0xffff;
  }
  p_data->gift_type     = MYSTERYGIFT_TYPE_POKEMON;

  //���ʕ���
  DEBUG_MYSTERY_SetGiftCommonData( p_data, event_id, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^���쐬
 *
 *	@param	GIFT_PACK_DATA *p_data  �f�[�^ 
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftItemData( GIFT_PACK_DATA *p_data, u32 event_id )
{ 
  GFL_STD_MemClear( p_data, sizeof(GIFT_PACK_DATA) );
  //����쐬
  { 
    GIFT_PRESENT_ITEM  *p_item = &p_data->data.item;
    p_item->itemNo    = ITEM_MONSUTAABOORU;
    p_item->movieflag = FALSE;
  }
  p_data->gift_type     = MYSTERYGIFT_TYPE_ITEM;

  //���ʕ���
  DEBUG_MYSTERY_SetGiftCommonData( p_data, event_id, TRUE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʕ����̃f�[�^�쐬
 *
 *	@param	DOWNLOAD_GIFT_DATA *p_data  �f�[�^
 *	@param	event_id
 *	@param	only_one_flag
 *	@param	version 
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftCommonData( GIFT_PACK_DATA *p_data, u32 event_id, BOOL only_one_flag )
{ 
  //�C�x���g�^�C�g��
  p_data->event_name[ 0] = L'�f';
  p_data->event_name[ 1] = L'�o';
  p_data->event_name[ 2] = L'�b';
  p_data->event_name[ 3] = L'�O';
  p_data->event_name[ 4] = L'�C';
  p_data->event_name[ 5] = L'�x';
  p_data->event_name[ 6] = L'��';
  p_data->event_name[ 7] = L'�g';
  p_data->event_name[ 8] = L'�^';
  p_data->event_name[ 9] = L'�C';
  p_data->event_name[10] = L'�g';
  p_data->event_name[11] = L'��';
  p_data->event_name[12] = 0xffff;

  //��M��������
  {
    RTCDate date;
    GFL_RTC_GetDate( &date );
    p_data->recv_date      = ((2000+date.year)<<MYSTERYGIFT_DATE_SHIFT_YEAR) | (date.month<<MYSTERYGIFT_DATE_SHIFT_MONTH) | (date.day<<MYSTERYGIFT_DATE_SHIFT_DAY);
  }

  //�C�x���gID
  p_data->event_id       = event_id;

  //�f�[�^
  p_data->card_message   = 0;
  
  //��x������M�t���O
  p_data->only_one_flag  = only_one_flag;

  //�󂯎�������ǂ���
  p_data->have  = 0;
}
