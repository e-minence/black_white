//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_data_util.c
 *	@brief  �ӂ����f�[�^�֗��֐�
 *	@author	Toru=Nagihashi
 *	@data		2010.03.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��


//���W���[��
#include "item/itemsym.h"
#include "field/gpower_id.h"

//�����̃��W���[��
#include "net_app/mystery.h"

//�O�����J
#include "net_app/mystery_data_util.h"

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
 *	@brief  �s������������C������֐�
 *
 *	@param	DOWNLOAD_GIFT_DATA *p_data  �C������f�[�^
 *
 *	@return �s���J�E���^�i�s�������Ȃ�O  �����̕s���������邽�тɁ{�P�j
 */
//-----------------------------------------------------------------------------
u32 MYSTERYDATA_ModifyDownloadData( DOWNLOAD_GIFT_DATA *p_data, GAMEDATA *p_gamedata, HEAPID heapID )
{ 
  u32 dirty = 0;

  //�����e�L�X�g���s��
  { 
    int i;
    BOOL is_darty = TRUE;
    //EOM�������Ă���ΐ���Ƃ݂Ȃ�
    for( i = 0; i < GIFT_DATA_CARD_TEXT_MAX+EOM_SIZE; i++ )
    { 
      if( p_data->event_text[i] == GFL_STR_GetEOMCode() )
      { 
        is_darty  = FALSE;
      }
    }

    if( is_darty )
    { 
      dirty++;
      p_data->event_text[0] = L'�@';
      p_data->event_text[1] = GFL_STR_GetEOMCode();
      OS_TPrintf( "DOWNLOAD_GIFT_DATA:�����e�L�X�g���s���̂��ߋ󕶎��ɂ��܂���\n" );
    }
  }

  //�t���f�[�^���`�F�b�N
  dirty += MYSTERYDATA_ModifyGiftData( &p_data->data, p_gamedata, heapID );

  return dirty;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �s������������C������֐�
 *
 *	@param	GIFT_PACK_DATA *p_data  �C������f�[�^
 *
 *	@return �s���J�E���^�i�s�������Ȃ�O  �����̕s���������邽�тɁ{�P�j
 */
//-----------------------------------------------------------------------------
u32 MYSTERYDATA_ModifyGiftData( GIFT_PACK_DATA *p_data, GAMEDATA *p_gamedata, HEAPID heapID )
{ 
  u32 dirty = 0;

  //�C�x���g�^�C�g�����s��
  { 
    int i;
    BOOL is_darty = TRUE;
    //EOM�������Ă���ΐ���Ƃ݂Ȃ�
    for( i = 0; i < GIFT_DATA_CARD_TITLE_MAX+EOM_SIZE; i++ )
    { 
      if( p_data->event_name[i] == GFL_STR_GetEOMCode() )
      { 
        is_darty  = FALSE;
      }
    }

    if( is_darty )
    { 
      dirty++;
      p_data->event_name[0] = L'�@';
      p_data->event_name[1] = GFL_STR_GetEOMCode();
      OS_TPrintf( "GIFT_PACK_DATA:�C�x���g�^�C�g�����s���̂��ߋ󕶎��ɂ��܂���\n" );
    }
  }

  //�C�x���gID
  { 
    if( !(0 <= p_data->event_id && p_data->event_id < MYSTERY_DATA_MAX_EVENT ) )
    { 
      p_data->event_id  = 0;
      dirty++;
      OS_TPrintf( "GIFT_PACK_DATA:�C�x���gID���s���̂��߂O�ɂ��܂���\n" );
    }
  }

  //�M�t�g�^�C�v�`�F�b�N
  { 
    if( !(MYSTERYGIFT_TYPE_NONE < p_data->gift_type &&  p_data->gift_type < MYSTERYGIFT_TYPE_MAX) )
    { 
      p_data->gift_type = MYSTERYGIFT_TYPE_NONE;
      dirty++;
      OS_TPrintf( "GIFT_PACK_DATA:�M�t�g�^�C�v���s���̂��߂O�ɂ��܂���\n" );
    }
  }

  //���g�`�F�b�N
  { 
    switch( p_data->gift_type )
    { 
    case MYSTERYGIFT_TYPE_POKEMON:
      { 
        GIFT_PRESENT_POKEMON  *p_poke = &p_data->data.pokemon;
        POKEMON_PARAM* pp = MYSTERY_CreatePokemon( p_data, heapID, p_gamedata );
        if( pp == NULL )
        { 
          dirty++;
          OS_TPrintf( "GIFT_PACK_DATA:�|�P�����f�[�^���s���ł���\n" ); 
        }
        else
        { 
          GFL_HEAP_FreeMemory( pp );
        }
      }
      break;
    case MYSTERYGIFT_TYPE_ITEM:
      { 
        GIFT_PRESENT_ITEM *p_item = &p_data->data.item;
        if( !(0 <= p_item->itemNo && p_item->itemNo < ITEM_DATA_MAX) )
        {   
          p_item->itemNo  = ITEM_MONSUTAABOORU;
          dirty++;
          OS_TPrintf( "GIFT_PACK_DATA:�A�C�e��NO���s���̂��߃����X�^�[�{�[���ɂ��܂���\n" );
        }
      }
      break;
    case MYSTERYGIFT_TYPE_POWER:
      { 
        GIFT_PRESENT_POWER *p_power = &p_data->data.gpower; //@todo
        if( !( 0 <= p_power->type && p_power->type < GPOWER_ID_MAX) )
        { 
          dirty++;
          OS_TPrintf( "GIFT_PACK_DATA:G�p���[���s���ł���\n" ); 
        }
      }
      break;
    }
  }

  return dirty;
}



