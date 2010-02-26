//============================================================================================
/**
 * @file		pokemontrade_color.c
 * @brief		�|�P�����J���[�\��
 * @author	k.ohno
 * @date		09.10.05
 */
//============================================================================================
#include <gflib.h>

#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_tool.h"
#include "pokeicon/pokeicon.h"
#include "pokemontrade_local.h"
#include "pm_define.h"

// �g���C�A�C�R���ɕ`�悷��|�P�����h�b�g�̃J���[
// �ǉ��ǎ��Ƌ��p�ł�
static const u8 TrayPokeDotColorPos[] = {
//  ��    ��    ��    ��    ��    ��    ��    �D    ��    ��
	0x0e, 0x0f, 0x05, 0x02, 0x0d, 0x0c, 0x06, 0x0b, 0x0a, 0x09,
};


static u8 _getColorIndex(POKEMON_PASO_PARAM * ppp,HEAPID heapID)
{

	POKEMON_PERSONAL_DATA * ppd;
	BOOL fast;
  u32	mons;
  u16	color=0xf;

  {
    fast = PPP_FastModeOn( ppp );
    mons = PPP_Get( ppp, ID_PARA_monsno, NULL );
    if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
      if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) == 0 ){
        u16 frm = PPP_Get( ppp, ID_PARA_form_no, NULL );
        ppd = POKE_PERSONAL_OpenHandle( mons, frm, heapID );
        color = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_color );
        POKE_PERSONAL_CloseHandle( ppd );
      }
      else{
        if( mons == MONSNO_MANAFI ){
          color = POKEPER_COLOR_BLUE;
        }else{
          color = POKEPER_COLOR_WHITE;
        }
      }
    }
    PPP_FastModeOff( ppp, fast );
  }
  return color;

}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �|�P�����h�b�g���{�b�N�X�o�b�t�@�ɓ����
 * @param	  POKEMON_TRADE_WORK
 * @param	  tray	�g���C�ԍ�
 * @param	  �����o�b�t�@
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokemonTrade_TrayPokeDotPut( POKEMON_TRADE_WORK* pWork, u32 tray, u8 * buf )
{
	POKEMON_PASO_PARAM * ppp;
  u8	i;

  for( i = 0; i < BOX_MAX_POS ; i++ ){
    ppp = BOXDAT_GetPokeDataAddress( pWork->pBox, tray, i );
    buf[i] = _getColorIndex(ppp, pWork->heapID);
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �莝���|�P�����h�b�g���{�b�N�X�o�b�t�@�ɓ����
 * @param	  POKEMON_TRADE_WORK
 * @param	  tray	�g���C�ԍ�
 * @param	  �����o�b�t�@
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokemonTrade_PartyPokeDotPut( POKEMON_TRADE_WORK* pWork, POKEPARTY* pParty, u8 * buf )
{
	POKEMON_PASO_PARAM * ppp;
  u8	i;
  int num = PokeParty_GetPokeCount(pParty);
    

  for( i = 0; i <  TEMOTI_POKEMAX; i++ ){
    if( num > i ){
      ppp = PP_GetPPPPointer( PokeParty_GetMemberPointer( pParty , i ) );
      buf[i] = _getColorIndex(ppp,pWork->heapID);
    }else{
      buf[i] = 0xf;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �莝���|�P�����J���[���{�b�N�X�o�b�t�@�ɓ����
 * @param	  POKEMON_TRADE_WORK
 * @param	  tray	�g���C�ԍ�
 * @param	  �����o�b�t�@
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokemonTrade_SetMyPokeColor( POKEMON_TRADE_WORK* pWork  )
{
  int i;

  OS_TPrintf("%d    \n",pWork->BOX_TRAY_MAX);
  
  for(i = 0; i < pWork->BOX_TRAY_MAX; i++){
    PokemonTrade_TrayPokeDotPut( pWork, i, &pWork->FriendPokemonCol[0][i*BOX_MAX_POS] );
  }
  PokemonTrade_PartyPokeDotPut(pWork, pWork->pMyParty,
                               &pWork->FriendPokemonCol[0][BOX_MAX_TRAY*BOX_MAX_POS] );


  if(!POKEMONTRADEPROC_IsNetworkMode(pWork)){
    for(i = 0; i < pWork->BOX_TRAY_MAX; i++){
      PokemonTrade_TrayPokeDotPut( pWork, i, &pWork->FriendPokemonCol[1][i*BOX_MAX_POS] );
    }
    PokemonTrade_PartyPokeDotPut(pWork, pWork->pMyParty,
                                 &pWork->FriendPokemonCol[1][BOX_MAX_TRAY*BOX_MAX_POS] );
  }


  
}

