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
#include "demo/shinka_demo.h"

/// �|�P���������̃^�C�v
typedef enum{
  POKEMONTRADE_TYPE_IRC,   ///< �ԊO��
  POKEMONTRADE_TYPE_UNION, ///< ���j�I��
  POKEMONTRADE_TYPE_WIFICLUB, ///< WIFI�N���u
  POKEMONTRADE_TYPE_GTSNEGO, ///< GTS�l�S�V�G�[�V����
  POKEMONTRADE_TYPE_GTS, ///< GTS
  POKEMONTRADE_TYPE_EVENT,  ///< �C�x���g�p
  POKEMONTRADE_TYPE_MAX,
} POKEMONTRADE_TYPE;


/// �|�P���������̈ړ�����
typedef enum{
  POKEMONTRADE_MOVE_START,  ///< �J�n
  POKEMONTRADE_MOVE_EVOLUTION,   ///< �i��
  POKEMONTRADE_MOVE_MAIL,   ///< ���[��
  POKEMONTRADE_MOVE_END,   ///< �I��
  POKEMONTRADE_MOVE_MAX,
} POKEMONTRADE_MOVE;

typedef struct _POKEMON_TRADE_WORK POKEMON_TRADE_WORK;
typedef struct _POKEMONTRADE_DEMO_WORK POKEMONTRADE_DEMO_WORK;



///�|�P���������Ăяo���p�p�����[�^
typedef struct{
  POKEMONTRADE_MOVE ret;
  int after_mons_no;
  int cond;
  GAMEDATA* gamedata;
  void* pNego;
  POKEPARTY* pParty;
  u16 selectBoxno;
  u16 selectIndex;
  SHINKA_DEMO_PARAM* shinka_param;
} POKEMONTRADE_PARAM;


///�|�P�����f���Ăяo���p�p�����[�^
typedef struct{
  POKEMONTRADE_PARAM aParam;
  GAMEDATA* gamedata;
  POKEMON_PARAM* pMyPoke;      ///< ���[�U�[�̃|�P����
  POKEMON_PARAM* pNPCPoke;  ///< NPC�̃|�P����
  MYSTATUS* pMy;    ///<������MYSTATUS
  MYSTATUS* pNPC;   ///<NPC��MYSTATUS
} POKEMONTRADE_DEMO_PARAM;


extern const GFL_PROC_DATA PokemonTradeProcData;
extern const GFL_PROC_DATA PokemonTradeIrcProcData;
extern const GFL_PROC_DATA PokemonTradeWiFiProcData;
extern const GFL_PROC_DATA PokemonTradeDemoProcData;
extern const GFL_PROC_DATA PokemonTradeClubProcData;



