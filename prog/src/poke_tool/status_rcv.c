//============================================================================================
/**
 * @file  status_rcv.c
 * @brief �A�C�e���g�p���̃|�P�����񕜏���
 * @author  Hiroyuki Nakamura
 * @date  05.12.01
 *        09.08.06 GS���ڐA
 */
//============================================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/status_rcv.h"

//============================================================================================
//  �萔��`
//============================================================================================

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//============================================================================================
//============================================================================================


//--------------------------------------------------------------------------------------------
/**
 * PP�񕜃`�F�b�N
 *
 * @param pp    �񕜂���|�P�����̃f�[�^
 * @param pos   �񕜈ʒu�i�Z�ʒu�Ȃǁj
 *
 * @retval  "TRUE = �g�p�\"
 * @retval  "FALSE = �g�p�s��"
 */
//--------------------------------------------------------------------------------------------
u8 PP_RcvCheck( POKEMON_PARAM * pp, u32 pos )
{
  u16 waza;
  u8  npp;
  u8  ppc;

  waza = (u16)PP_Get( pp, ID_PARA_waza1+pos, NULL );
  if( waza == 0 ){
    return FALSE;
  }
  npp = (u8)PP_Get( pp, ID_PARA_pp1+pos, NULL );
  ppc = (u8)PP_Get( pp, ID_PARA_pp_count1+pos, NULL );

  if( npp < WAZADATA_GetMaxPP( waza, ppc ) ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * PP��
 *
 * @param pp    �񕜂���|�P�����̃f�[�^
 * @param pos   �񕜈ʒu�i�Z�ʒu�Ȃǁj
 * @param rcv   �񕜒l
 *
 * @retval  "TRUE = �g�p�\"
 * @retval  "FALSE = �g�p�s��"
 */
//--------------------------------------------------------------------------------------------
u8 PP_Recover( POKEMON_PARAM * pp, u32 pos, u32 rcv )
{
  u16 waza;
  u8  npp;
  u8  mpp;

  waza = (u16)PP_Get( pp, ID_PARA_waza1+pos, NULL );
  if( waza == 0 ){
    return FALSE;
  }
  npp  = (u8)PP_Get( pp, ID_PARA_pp1+pos, NULL );
  mpp  = (u8)WAZADATA_GetMaxPP( waza, PP_Get(pp,ID_PARA_pp_count1+pos,NULL) );

  if( npp < mpp ){
    if( rcv == PP_RCV_ALL ){
      npp = mpp;
    }else{
      npp += rcv;
      if( npp > mpp ){
        npp = mpp;
      }
    }
    PP_Put( pp, ID_PARA_pp1+pos, npp );
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**�|�P�����񕜏���(�P�́E�S��
 */
//--------------------------------------------------------------------------------------------
void STATUS_RCV_PokeParam_RecoverAll(POKEMON_PARAM * pp)
{
  int j;
  u32 buf;
  //HP�S��
  buf = PP_Get( pp, ID_PARA_hpmax, NULL );
  PP_Put(pp, ID_PARA_hp, buf);
  //��Ԉُ��
  buf = 0;
  PP_Put(pp, ID_PARA_condition, buf);
  //PP�S��
  for (j = 0; j < 4; j++) {
    if (PP_RcvCheck(pp,j) == TRUE) {
      PP_Recover(pp, j, PP_RCV_ALL);
    }
  }
}

void STATUS_RCV_PokeParty_RecoverAll(POKEPARTY * party)
{
  int i,total;
  POKEMON_PARAM * pp;

  total = PokeParty_GetPokeCount(party);
  for (i = 0; i < total; i++) {
    pp = PokeParty_GetMemberPointer(party, i);
    if (PP_Get(pp, ID_PARA_poke_exist, NULL) == FALSE) {
      continue;
    }
    STATUS_RCV_PokeParam_RecoverAll( pp );
  }
}




