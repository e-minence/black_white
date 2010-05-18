//======================================================================
/**
 * @file	script_hideitem.c
 * @brief	�X�N���v�g����F�B���A�C�e���֘A
 * @date	05.08.04
 *
 * 01.11.07	Sousuke Tamada
 * 03.04.15	Satoshi Nohara
 * 05.04.25 Hiroyuki Nakamura
 *
 * 09.09.13 tamada  �B���A�C�e���֘A��Ɨ�������
 */
//======================================================================

#include <gflib.h>
#include "script.h"

#include "script_hideitem.h"

#include "eventwork_def.h"



//FLAG_HIDEITEM_AREA_START�Q�Ƃ̂���
#include "../../../resource/fldmapdata/flagwork/flag_define.h"

//ID_HIDE_ITEM_OFFSET�Q�Ƃ̂���
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"

#include "arc/fieldmap/zone_id.h"

//============================================================================================
//	�B���A�C�e���f�[�^
//============================================================================================

typedef struct {
  u16 index;
  u8 world_flag;
  u8 revival_flag;
  u16 zone_id;
  u16 x, z;
}HIDE_ITEM_DATA;

//static const u16 hide_item_data[];
#include "../../../resource/fldmapdata/script/scr_hideitem/hide_item.cdat"

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief �X�N���v�gID����B���A�C�e��ID���擾
 * @param scr_id  �X�N���v�gID
 * @param �B���A�C�e���f�[�^�̃C���f�b�N�X
 */
//--------------------------------------------------------------
u16 HIDEITEM_GetIDByScriptID( u16 scr_id )
{
  if ( scr_id < ID_HIDE_ITEM_OFFSET || scr_id > ID_HIDE_ITEM_OFFSET_END )
  {
    GF_ASSERT_MSG( 0, "not hide-item script id(%d)!!\n", scr_id );
    return 0;
  }
  return scr_id - ID_HIDE_ITEM_OFFSET;
}

//--------------------------------------------------------------
/**
 * @brief �X�N���v�gID����B���A�C�e���ɑΉ������t���OID���擾
 * @param scr_id  �X�N���v�gID
 * @return  u16 �C�x���g�t���ONo
 */
//--------------------------------------------------------------
u16 HIDEITEM_GetFlagNoByScriptID( u16 scr_id )
{
  u16 item_id = HIDEITEM_GetIDByScriptID( scr_id );
  const HIDE_ITEM_DATA * data = &hide_item_data[ item_id ];
  return FLAG_HIDEITEM_AREA_START + data->index;
}

//--------------------------------------------------------------
/**
 * @brief �B���A�C�e����������
 * @param ev
 */
//--------------------------------------------------------------
void HIDEITEM_Revival( EVENTWORK * ev )
{
  int i;
  if ( EVENTWORK_PopTimeRequest( ev ) == FALSE ) return;
  for ( i = 0; i < NELEMS(hide_item_data); i++ )
  {
    const HIDE_ITEM_DATA * data = &hide_item_data[i];
    if ( data->revival_flag == 0 ) continue;
    if ( GFUser_GetPublicRand( 100 ) >= 20 ) continue;
    EVENTWORK_ResetEventFlag( ev, FLAG_HIDEITEM_AREA_START + data->index );
  }
}


//--------------------------------------------------------------
/**
 * @brief           �B���A�C�e���e�[�u���𓾂�
 * @param[out]      �e�[�u���̗v�f��
 * @return          �e�[�u���̐擪�A�h���X
 */
//--------------------------------------------------------------
const void* HIDEITEM_GetTable( u16* num )
{
  *num = NELEMS(hide_item_data);
  return hide_item_data;
}





