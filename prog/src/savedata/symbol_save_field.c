//==============================================================================
/**
 * @file    symbol_save_field.c
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^�Ffieldmap�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.03.03(��)
 *
 * �t�B�[���h�ł����g�p���Ȃ��V���{���G���J�E���g�Z�[�u�A�N�Z�X�Q
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_notwifi.h"
#include "savedata/symbol_save_field.h"
#include "symbol_save_local.h"


//==================================================================
/**
 * �V���{���|�P������o�^
 *
 * @param   symbol_save		�V���{���Z�[�u�̈�ւ̃|�C���^
 * @param   monsno        �|�P�����ԍ�
 * @param   wazano		    �Z�ԍ�
 * @param   sex		        ����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
 * @param   form_no		    �t�H�����ԍ�
 * @param   zone_type     SYMBOL_ZONE_TYPE
 * @return  BOOL          TRUE�̂Ƃ��A�o�^�ł���
 */
//==================================================================
BOOL SymbolSave_Field_Set( SYMBOL_SAVE_WORK *symbol_save,
    u16 monsno, u16 wazano, u8 sex, u8 form_no, u8 move_type, SYMBOL_ZONE_TYPE zone_type )
{
  if ( zone_type == SYMBOL_ZONE_TYPE_FREE_LARGE || zone_type == SYMBOL_ZONE_TYPE_FREE_SMALL )
  {
    //�}�b�v�g���`�F�b�N�Ȃǂ��܂߂Ă܂����Ă��܂�
    SymbolSave_SetFreeZone( symbol_save, monsno, wazano, sex, form_no, move_type, zone_type );
  }
  else
  {
    u32 no, start, end;
    no = SymbolSave_CheckSpace( symbol_save, zone_type );
    if ( no == SYMBOL_SPACE_NONE ) return FALSE;
#if 0
    if ( no == SYMBOL_SPACE_NONE ) {
      SymbolSave_DataShift( symbol_save, start );
      no = end - 1;
    }
#endif
    SymbolSave_Local_Decode(symbol_save);  //�Í�������
    symbol_save->symbol_poke[ no ].monsno = monsno;
    symbol_save->symbol_poke[ no ].wazano = wazano;
    symbol_save->symbol_poke[ no ].sex = sex;
    symbol_save->symbol_poke[ no ].form_no = form_no;
    symbol_save->symbol_poke[ no ].move_type = move_type;
    SymbolSave_Local_Encode(symbol_save);  //�ĂшÍ���
  }
  return TRUE;
}

//==================================================================
/**
 * �V���{���|�P�����̈ړ�
 *
 * @param   symbol_save
 * @param   now_no          �ړ��Ώۃ|�P�����̔z�uNo.
 *
 * �z�uNo.���猻�݂̃]�[���^�C�v���������ʂ��A�K�؂Ȉړ����s��
 */
//==================================================================
BOOL SymbolSave_Field_MoveAuto( SYMBOL_SAVE_WORK *symbol_save, u32 now_no )
{
  SYMBOL_ZONE_TYPE now_type = SYMBOLZONE_GetZoneTypeFromNumber( now_no );
  SYMBOL_ZONE_TYPE new_type;
  u32 new_no;

  switch ( now_type )
  {
  case SYMBOL_ZONE_TYPE_KEEP_LARGE: //�L�[�v�]�[���i��j���t���[�]�[���i��j
    new_type = SYMBOL_ZONE_TYPE_FREE_LARGE;
    break;
  case SYMBOL_ZONE_TYPE_KEEP_SMALL: //�L�[�v�]�[���i���j���t���[�]�[���i���j
    new_type = SYMBOL_ZONE_TYPE_FREE_SMALL;
    break;
  case SYMBOL_ZONE_TYPE_FREE_LARGE: //�t���[�]�[���i��j���L�[�v�]�[���i��j
    new_type = SYMBOL_ZONE_TYPE_KEEP_LARGE;
    break;
  case SYMBOL_ZONE_TYPE_FREE_SMALL: //�t���[�]�[���i���j���L�[�v�]�[���i���j
    new_type = SYMBOL_ZONE_TYPE_KEEP_SMALL;
    break;
  default:
    GF_ASSERT( 0 );
    return FALSE;
  }

  new_no = SymbolSave_CheckSpace(symbol_save, new_type);
#ifdef  PM_DEBUG
  {
    static const char * const typenames[] = { "Keep Large", "Keep Small", "Free Large", "Free Small" };
    TAMADA_Printf("%s(%3d)-->%s(%3d)", typenames[now_type], now_no, typenames[new_type], new_no);
  }
#endif

  if(new_no == SYMBOL_SPACE_NONE){
    TAMADA_Printf(":Failure\n");
    return FALSE;

  } else {
    SymbolSave_Local_Decode(symbol_save);  //�Í�������
    //�V�����ꏊ�ɑ������
    symbol_save->symbol_poke[new_no] = symbol_save->symbol_poke[now_no];
    SymbolSave_Local_Encode(symbol_save);  //�ĂшÍ���
    //���̃]�[����O�l��
    SymbolSave_DataShift(symbol_save, now_no);
    TAMADA_Printf(":Success\n");
    return TRUE;
  }
}

