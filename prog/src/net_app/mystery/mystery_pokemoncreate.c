//============================================================================================
/**
 * @file    mystery_pokemoncreate.c
 * @bfief   �s�v�c�ʐM�f�[�^����|�P���������֐�
 * @author  k.ohno
 * @date    10.01.27
 */
//============================================================================================


#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "savedata/mystery_data.h"



POKEMON_PARAM* MYSTERY_PokemonCreate(const GIFT_PRESENT_POKEMON* pGift, HEAPID heapID)
{
  POKEMON_PARAM* pp;
  u16 level=pGift->level;

  
  if(pGift->mons_no > MONSNO_END){  //�����X�^�[�ԍ������݂��Ȃ����͍��Ȃ�
    return NULL;
  }
  if(pGift->mons_no == 0){  //�����X�^�[�ԍ������݂��Ȃ����͍��Ȃ�
    return NULL;
  }
  if(pGift->mons_no == 0){  //�����X�^�[�ԍ������݂��Ȃ����͍��Ȃ�
    return NULL;
  }
  if(pGift->level > 100){  //���x��100����͍��Ȃ�
    return NULL;
  }
  
  if(pGift->level == 0){  //���x���O�w��͗���
    level = GFUser_GetPublicRand(99)+1;
  }

  
  pp = PP_Create( pGift->mons_no, level, u64 id, HEAPID heapID );

  

  PP_SetupEx

  

}



