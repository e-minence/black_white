//=============================================================================================
/**
 * @file  btl_tables.c
 * @brief �|�P����WB �Ă��Ƃ��ȃT�C�Y�̃e�[�u����VRAM_H�ɒu���ăA�N�Z�X����
 * @author  taya
 *
 * @date  2010.05.27  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "item\itemsym.h"
#include "item\item.h"


#include "btl_common.h"
#include "btl_tables.h"


/**
 *  �A���R�[�����O�Ώۂ̃��U������
 */
BOOL BTL_TABLES_IsMatchEncoreFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_ANKOORU,  WAZANO_OUMUGAESI,
    WAZANO_HENSIN,  WAZANO_MONOMANE, WAZANO_SUKETTI,
  };
  u32 i;

  for(i=0; i<NELEMS(table); ++i)
  {
    if( table[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}

