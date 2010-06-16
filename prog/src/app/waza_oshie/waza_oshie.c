//============================================================================================
/**
 * @file  waza_oshie.c
 * @brief �Z����/�v���o������
 * @author  Hiroyuki Nakamura / Akito Mori(WB�Ɉ��z��)
 * @date  09.10.13
 */
//============================================================================================
#include <gflib.h>
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_personal.h"

#define WAZA_OSHIE_H_GLOBAL
#include "app/waza_oshie.h"

#define LV_WAZA_OBOE_MAX  ( POKEPER_WAZAOBOE_TABLE_ELEMS )  // ���x���A�b�v�Ŋo����Z�� + �I�[�R�[�h


// �P�ɂ���Ɗo����Z���X�g���o�͂���
#define DEBUT_PRINT_WAZALIST  ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �Z�����f�[�^�̈�擾
 *
 * @param heap  �q�[�vID
 *
 * @return  �Z�����f�[�^�̈�
 */
//--------------------------------------------------------------------------------------------
WAZAOSHIE_DATA * WAZAOSHIE_DataAlloc( HEAPID heap )
{
  WAZAOSHIE_DATA * dat = GFL_HEAP_AllocMemory( heap, sizeof(WAZAOSHIE_DATA) );

  GFL_STD_MemFill( dat, 0, sizeof(WAZAOSHIE_DATA) );

  return dat;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�����f�[�^�̈���
 *
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void WAZAOSHIE_DataFree( WAZAOSHIE_DATA * dat )
{
  GFL_HEAP_FreeMemory( dat );
}

//--------------------------------------------------------------------------------------------
/**
 * �v���o���\�ȋZ���擾
 *
 * @param pp    �|�P�����f�[�^
 * @param heap  �q�[�vID
 *
 * @return  �擾�����Z�f�[�^
 *
 *  ����͊e����
 */
//--------------------------------------------------------------------------------------------
u16 * WAZAOSHIE_GetRemaindWaza( POKEMON_PARAM * pp, HEAPID heap )
{
  POKEPER_WAZAOBOE_CODE * get;
  u16 * ret;
  u16 waza[4];
  u16 mons;
  u8  lv;
  u8  form;
  u8  h, i, j;

  mons = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
  form = (u8)PP_Get( pp, ID_PARA_form_no, NULL );
  lv   = (u8)PP_Get( pp, ID_PARA_level, NULL );
  for( i=0; i<4; i++ ){
    waza[i] = (u16)PP_Get( pp, ID_PARA_waza1+i, NULL );
  }

  get = GFL_HEAP_AllocMemory( heap, LV_WAZA_OBOE_MAX * sizeof( POKEPER_WAZAOBOE_CODE ) );
  ret = GFL_HEAP_AllocMemory( heap, LV_WAZA_OBOE_MAX * 2 );

//  PokeFormNoWazaOboeDataGet( mons, form, get );
  POKE_PERSONAL_LoadWazaOboeTable( mons, form, get );  // alloc�ł͂Ȃ�Load�ɕύX

  j = 0;
  for( i=0; i<LV_WAZA_OBOE_MAX; i++ ){
    // �I�[�R�[�h�`�F�b�N
    if( POKEPER_WAZAOBOE_IsEndCode( get[i] ) ){
      ret[j] = WAZAOSHIE_TBL_MAX;
      break;
    // ���݂̃��x���Ŋo������Z��
    }else if( POKEPER_WAZAOBOE_GetLevel( get[i] ) > lv ){
      continue;
    }else{
      // ���łɊo���Ă��邩
      for( h=0; h<4; h++ ){
        if( POKEPER_WAZAOBOE_GetWazaID( get[i] ) == waza[h] ){ break; }
      }
      if( h == 4 ){
        // �e�[�u���ɓ����Z���Ȃ���
        for( h=0; h<j; h++ ){
          if( ret[h] == POKEPER_WAZAOBOE_GetWazaID( get[i] ) ){ break; }
        }
        if( h == j ){
          ret[j] = POKEPER_WAZAOBOE_GetWazaID( get[i] );
          j++;
        }
      }
    }
  }

#ifdef PM_DEBUG
#if DEBUT_PRINT_WAZALIST
  for(i=0;i<LV_WAZA_OBOE_MAX;i++){
    OS_Printf("%02d:wazano=%d\n",i,ret[i]);
  }
#endif
#endif

  GFL_HEAP_FreeMemory( get );

  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * ��������/�v���o����Z�����邩�`�F�b�N
 *
 * @param tbl   �Z�e�[�u��
 *
 * @retval  "TRUE = ����"
 * @retval  "FALSE = �Ȃ�"
 */
//--------------------------------------------------------------------------------------------
BOOL WAZAOSHIE_WazaTableChack( u16 * tbl )
{
  if( tbl[0] == WAZAOSHIE_TBL_MAX ){
    return FALSE;
  }
  return TRUE;
}
