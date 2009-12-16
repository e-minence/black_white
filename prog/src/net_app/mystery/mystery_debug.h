//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_debug.h
 *	@brief  �ӂ����p�f�o�b�O
 *	@author	Toru=Nagihashi
 *	@date		2009.12.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef PM_DEBUG

#define DEBUG_SET_SAVEDATA


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
//=============================================================================
/**
 *    DEBUG
 */
//=============================================================================
#ifdef DEBUG_SET_SAVEDATA
#include "poke_tool/monsno_def.h"
#include "savedata/mystery_data.h"
static inline void MYSTERY_DEBUG_SetGiftData( DOWNLOAD_GIFT_DATA *p_data )
{ 
  GFL_STD_MemClear( p_data, sizeof(DOWNLOAD_GIFT_DATA) );
  
  //�|�P�����f�[�^
  p_data->data.data.pokemon.mons_no     = MONSNO_MYUU;
  p_data->data.data.pokemon.form_no     = 0;
  p_data->data.data.pokemon.oyaname[0]  = L'�f';
  p_data->data.data.pokemon.oyaname[1]  = L'�o';
  p_data->data.data.pokemon.oyaname[2]  = L'�b';
  p_data->data.data.pokemon.oyaname[3]  = L'�O';
  p_data->data.data.pokemon.oyaname[4]  = 0xffff;


  //�C�x���g�^�C�g��
  p_data->data.event_name[ 0] = L'��';
  p_data->data.event_name[ 1] = L'��';
  p_data->data.event_name[ 2] = L'��';
  p_data->data.event_name[ 3] = L'��';
  p_data->data.event_name[ 4] = L'�P';
  p_data->data.event_name[ 5] = L'�O';
  p_data->data.event_name[ 6] = L'��';
  p_data->data.event_name[ 7] = L'��';
  p_data->data.event_name[ 8] = L'��';
  p_data->data.event_name[ 9] = L'��';
  p_data->data.event_name[10] = L'��';
  p_data->data.event_name[11] = L'�@';
  p_data->data.event_name[12] = L'�~';
  p_data->data.event_name[13] = L'��';
  p_data->data.event_name[14] = L'�E';
  p_data->data.event_name[15] = 0xffff;

  //��M��������
  p_data->data.recv_date      = (2010<<MYSTERYGIFT_DATE_SHIFT_YEAR) | (12<<MYSTERYGIFT_DATE_SHIFT_MONTH) | (30<<MYSTERYGIFT_DATE_SHIFT_DAY);

  //�C�x���gID
  p_data->data.event_id       = 1;

  //�f�[�^
  p_data->data.gift_type      = MYSTERYGIFT_TYPE_POKEMON;
  p_data->data.icon_no        = MONSNO_MYUU;
  p_data->data.form           = 0;
  p_data->data.card_message   = 4;
  
  //��x������M�t���O
  p_data->data.only_one_flag  = 1;

  //�󂯎�������ǂ���
  p_data->data.have  = 0;

  //�Ώۃo�[�W����
  p_data->version    = 0;

  //�����e�L�X�g
  {
    p_data->event_text[ 0] = L'�|';
    p_data->event_text[ 1] = L'�P';
    p_data->event_text[ 2] = L'��';
    p_data->event_text[ 3] = L'��';
    p_data->event_text[ 4] = L'��';
    p_data->event_text[ 5] = L'��';
    p_data->event_text[ 6] = L'��';
    p_data->event_text[ 7] = L'��';
    p_data->event_text[ 8] = L'�@';
    p_data->event_text[ 9] = L'�P';
    p_data->event_text[10] = L'�O';
    p_data->event_text[11] = L'��';
    p_data->event_text[12] = L'��';
    p_data->event_text[13] = L'��';
    p_data->event_text[14] = L'��';
    p_data->event_text[15] = L'��';
    p_data->event_text[16] = L'��';
    p_data->event_text[17] = 0xfffe;
    p_data->event_text[18] = L'��';
    p_data->event_text[19] = L'��';
    p_data->event_text[20] = L'��';
    p_data->event_text[21] = L'��';
    p_data->event_text[22] = L'�@';
    p_data->event_text[23] = L'��';
    p_data->event_text[24] = L'��';
    p_data->event_text[25] = L'��';
    p_data->event_text[26] = L'��';
    p_data->event_text[27] = L'��';
    p_data->event_text[28] = L'�|';
    p_data->event_text[29] = L'�P';
    p_data->event_text[30] = L'��';
    p_data->event_text[31] = L'��';
    p_data->event_text[32] = 0xfffe;
    p_data->event_text[33] = L'�~';
    p_data->event_text[34] = L'��';
    p_data->event_text[35] = L'�E';
    p_data->event_text[36] = L'��';
    p_data->event_text[37] = L'�@';
    p_data->event_text[38] = L'�v';
    p_data->event_text[39] = L'��';
    p_data->event_text[40] = L'�[';
    p_data->event_text[41] = L'��';
    p_data->event_text[42] = L'�g';
    p_data->event_text[43] = L'�I';
    p_data->event_text[44] = 0xffff;
  }
}
#endif //DEBUG_SET_SAVEDATA
#endif //PM_DEBUG
