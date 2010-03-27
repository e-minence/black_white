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
#include "app/mailbox.h"
#include "net_app/gtsnego.h"

/// �|�P���������̃^�C�v
typedef enum{
  POKEMONTRADE_TYPE_IRC,   ///< �ԊO��
  POKEMONTRADE_TYPE_UNION, ///< ���j�I��
  POKEMONTRADE_TYPE_WIFICLUB, ///< WIFI�N���u
  POKEMONTRADE_TYPE_GTSNEGO, ///< GTS�l�S�V�G�[�V����
  POKEMONTRADE_TYPE_VISUAL,
  POKEMONTRADE_TYPE_GTS = POKEMONTRADE_TYPE_VISUAL, ///< GTS
  POKEMONTRADE_TYPE_GTSUP,   ///< GTS��������
  POKEMONTRADE_TYPE_GTSDOWN,  ///< GTS�Ђ��Ƃ�
  POKEMONTRADE_TYPE_GTSMID,  ///< GTS�^��
  POKEMONTRADE_TYPE_GTSNEGODEMO,
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
  MAILBOX_PARAM aMailBox;
  POKEMONTRADE_MOVE ret;
  int after_mons_no;
  int cond;
  GAMEDATA* gamedata;
  EVENT_GTSNEGO_WORK* pNego;
  POKEPARTY* pParty;
  u16 selectBoxno;
  u16 selectIndex;
  BOOL bDebug;
  SHINKA_DEMO_PARAM* shinka_param;
  DWCSvlResult* pSvl;         //[in ]�C�V��SVL�F�؍\���� DWCSvlResult�͎��O�Ŋm�ۂ��Ă�������
} POKEMONTRADE_PARAM;


///�|�P�����f���Ăяo���p�p�����[�^
typedef struct{
  POKEMONTRADE_PARAM aParam;   // �������͓���Ȃ��ł�������
  GAMEDATA* gamedata;   ///< gamedata
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
extern const GFL_PROC_DATA PokemonTradeGTSProcData;
extern const GFL_PROC_DATA PokemonTradeGTSSendProcData;
extern const GFL_PROC_DATA PokemonTradeGTSRecvProcData;
extern const GFL_PROC_DATA PokemonTradeGTSMidProcData;



