//=============================================================================
/**
 * @file	  pokemontrade.h
 * @bfief	  �|�P��������
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================


#pragma once

#include "poke_tool/poke_tool.h"
#include "gamesystem/game_data.h"

/// �|�P���������̃^�C�v
typedef enum{
  POKEMONTRADE_IRC,   ///< �ԊO��
  POKEMONTRADE_UNION, ///< ���j�I��
  POKEMONTRADE_WIFICLUB, ///< WIFI�N���u
  POKEMONTRADE_GTSNEGO, ///< GTS�l�S�V�G�[�V����
  POKEMONTRADE_GTS, ///< GTS
  POKEMONTRADE_EVENT,  ///< �C�x���g�p
  POKEMONTRADE_TYPE_MAX,
} POKEMONTRADE_TYPE;

typedef struct _POKEMON_TRADE_WORK POKEMON_TRADE_WORK;
typedef struct _POKEMONTRADE_DEMO_WORK POKEMONTRADE_DEMO_WORK;



///�|�P���������Ăяo���p�p�����[�^
typedef struct{
  POKEMONTRADE_TYPE type;
  GAMEDATA* gamedata;
  void* pNego;
} POKEMONTRADE_PARAM;


///�|�P�����f���Ăяo���p�p�����[�^
typedef struct{
  POKEMON_PARAM* pMyPoke;      ///< ���[�U�[�̃|�P����
  POKEMON_PARAM* pNPCPoke;  ///< NPC�̃|�P����
  MYSTATUS* pMy;    ///<������MYSTATUS
  MYSTATUS* pNPC;   ///<NPC��MYSTATUS
  GAMEDATA* gamedata;
} POKEMONTRADE_DEMO_PARAM;


extern const GFL_PROC_DATA PokemonTradeProcData;
extern const GFL_PROC_DATA PokemonTradeIrcProcData;
extern const GFL_PROC_DATA PokemonTradeWiFiProcData;
extern const GFL_PROC_DATA PokemonTradeDemoProcData;


