//======================================================================
/**
 * @file	mb_poke_icon.c
 * @brief	マルチブート・ポケアイコン表示(子機は刺さっているROMからデータを抜く
 * @author	ariizumi
 * @data	09/11/20
 *
 * モジュール名：MB_ICON
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "mb_select_gra.naix"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "multiboot/mb_poke_icon.h"

#include "pokeicon/pokeicon.h"

#define USE_DUMMY_ICON (0)

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static u32 MB_ICON_GetCharRes_Func( POKEMON_PASO_PARAM *ppp );

//--------------------------------------------------------------
//	ARCHANDLEの取得
//--------------------------------------------------------------
ARCHANDLE* MB_ICON_GetArcHandle( HEAPID heapId , const DLPLAY_CARD_TYPE cardType )
{
  ARCHANDLE* arcHandle;
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKEICON ,heapId );
  //arcHandle = GFL_ARC_OpenDataHandleByFilePath("a/0/1/0",heapId);
#else                    //DL子機時処理
#if USE_DUMMY_ICON
    arcHandle = GFL_ARC_OpenDataHandle(ARCID_MB_SELECT,heapId);
#else  
  switch( cardType )
  {
  case CARD_TYPE_DP:   //ダイアモンド＆パール
    arcHandle = GFL_ARC_OpenDataHandleByFilePath("child_rom:/poketool/icongra/poke_icon.narc",heapId);
    break;
  case CARD_TYPE_PT:   //プラチナ

    arcHandle = GFL_ARC_OpenDataHandleByFilePath("child_rom:/poketool/icongra/pl_poke_icon.narc",heapId);
    break;
  case CARD_TYPE_GS:   //ゴールド＆シルバー
    arcHandle = GFL_ARC_OpenDataHandleByFilePath("child_rom:/a/0/2/0",heapId);
    break;
#if PM_DEBUG
  case CARD_TYPE_DUMMY:  //MBテストダミー
    arcHandle = GFL_ARC_OpenDataHandle(ARCID_MB_SELECT,heapId);
    break;
#endif
  }
#endif  //USE_DUMMY_ICON
#endif //MULTI_BOOT_MAKE

  return arcHandle;
}

//--------------------------------------------------------------
//	各種素材IDの取得
//--------------------------------------------------------------
u32 MB_ICON_GetPltResId( const DLPLAY_CARD_TYPE cardType )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  return POKEICON_GetPalArcIndex();
#else                    //DL子機時処理
#if USE_DUMMY_ICON
    return NARC_mb_select_gra_icon_dummy_NCLR;
#endif 
  switch( cardType )
  {
  case CARD_TYPE_DP:   //ダイアモンド＆パール
  case CARD_TYPE_PT:   //プラチナ
  case CARD_TYPE_GS:   //ゴールド＆シルバー
  	//NARC_poke_icon_poke_icon_NCLR = 0,
    return 0;
    break;
#if PM_DEBUG
  case CARD_TYPE_DUMMY:  //MBテストダミー
    return NARC_mb_select_gra_icon_dummy_NCLR;
    break;
#endif
  }
  return 0;
#endif //MULTI_BOOT_MAKE
}

u32 MB_ICON_GetCharResId( POKEMON_PASO_PARAM *ppp , const DLPLAY_CARD_TYPE cardType )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  return POKEICON_GetCgxArcIndex(ppp);
#else                    //DL子機時処理
#if USE_DUMMY_ICON
    return NARC_mb_select_gra_icon_dummy_NCGR;
#endif 
  switch( cardType )
  {
  case CARD_TYPE_DP:   //ダイアモンド＆パール
  case CARD_TYPE_PT:   //プラチナ
  case CARD_TYPE_GS:   //ゴールド＆シルバー
    return MB_ICON_GetCharRes_Func(ppp);
    break;
#if PM_DEBUG
  case CARD_TYPE_DUMMY:  //MBテストダミー
    return NARC_mb_select_gra_icon_dummy_NCGR;
    break;
#endif
  }
  return 0;

#endif //MULTI_BOOT_MAKE
}

u32 MB_ICON_GetCellResId( const DLPLAY_CARD_TYPE cardType )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  return POKEICON_GetCellArcIndex();
#else                    //DL子機時処理
#if USE_DUMMY_ICON
    return NARC_mb_select_gra_icon_dummy_NCER;
#endif 
  switch( cardType )
  {
  case CARD_TYPE_DP:   //ダイアモンド＆パール
  case CARD_TYPE_PT:   //プラチナ
  case CARD_TYPE_GS:   //ゴールド＆シルバー
  	//NARC_poke_icon_poke_icon_anm_NCER = 4,
    return 4;
    break;
#if PM_DEBUG
  case CARD_TYPE_DUMMY:  //MBテストダミー
    return NARC_mb_select_gra_icon_dummy_NCER;
    break;
#endif
  }
  return 0;
#endif //MULTI_BOOT_MAKE
}


u32 MB_ICON_GetAnmResId( const DLPLAY_CARD_TYPE cardType )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  return POKEICON_GetAnmArcIndex();
#else                    //DL子機時処理
#if USE_DUMMY_ICON
    return NARC_mb_select_gra_icon_dummy_NANR;
#endif 
  switch( cardType )
  {
  case CARD_TYPE_DP:   //ダイアモンド＆パール
  case CARD_TYPE_PT:   //プラチナ
  case CARD_TYPE_GS:   //ゴールド＆シルバー
  	//NARC_poke_icon_poke_icon_anm_NANR = 3,
    return 3;
    break;
#if PM_DEBUG
  case CARD_TYPE_DUMMY:  //MBテストダミー
    return NARC_mb_select_gra_icon_dummy_NANR;
    break;
#endif
  }
  return 0;
#endif //MULTI_BOOT_MAKE
}


const u8 MB_ICON_GetPalNumber( POKEMON_PASO_PARAM *ppp )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
  return POKEICON_GetPalNumGetByPPP(ppp);
#else                    //DL子機時処理
  return POKEICON_GetPalNumGetByPPP(ppp);
#endif //MULTI_BOOT_MAKE
}

//--------------------------------------------------------------
//	キャラ取得用分岐
//--------------------------------------------------------------
static u32 MB_ICON_GetCharRes_Func( POKEMON_PASO_PARAM *ppp )
{
  BOOL fast;
  u32  mons;
  u32  form;
  u32  egg;

  fast = PPP_FastModeOn( (POKEMON_PASO_PARAM *)ppp );

  form = PPP_Get( ppp, ID_PARA_form_no, NULL );
  mons = PPP_Get( ppp, ID_PARA_monsno, NULL );
  egg  = PPP_Get( ppp, ID_PARA_tamago_flag, NULL );
  
  form = POKETOOL_CheckPokeFormNo( mons , form );
  PPP_FastModeOff( (POKEMON_PASO_PARAM *)ppp, fast );

	if( egg == 1 )
	{
		if( mons == MONSNO_MANAFI )
		{
			//return NARC_poke_icon_poke_icon_mnf_NCGR;
			return 502;
		}
		else
		{
			//return NARC_poke_icon_poke_icon_tam_NCGR;
			return 501;
		}
	}

	if( form != 0 )
	{
		if( mons == MONSNO_DEOKISISU )
		{
			//return ( NARC_poke_icon_poke_icon_d01_NCGR + form - 1 );
			return ( 503 + form - 1 );
		}
		if( mons == MONSNO_ANNOON )
		{
			//return ( NARC_poke_icon_poke_icon_u02_NCGR + form - 1 );
			return ( 507 + form - 1 );
		}
		if( mons == MONSNO_MINOMUTTI )
		{
			//return ( NARC_poke_icon_poke_icon_455_01_NCGR + form - 1 );
			return ( 534 + form - 1 );
		}
		if( mons == MONSNO_MINOMADAMU )
		{
			//return ( NARC_poke_icon_poke_icon_457_01_NCGR + form - 1 );
			return ( 536 + form - 1 );
		}
		if( mons == MONSNO_KARANAKUSI )
		{
			//return ( NARC_poke_icon_poke_icon_458_01_NCGR + form - 1 );
			return ( 538 + form - 1 );
		}
		if( mons == MONSNO_TORITODON )
		{
			//return ( NARC_poke_icon_poke_icon_459_01_NCGR + form - 1 );
			return ( 539 + form - 1 );
		}
		if( mons == MONSNO_GIRATHINA )
		{
			//return ( NARC_poke_icon_poke_icon_509_01_NCGR + form - 1 );
			return ( 540 + form - 1 );
		}
		if( mons == MONSNO_SHEIMI )
		{
			//return ( NARC_poke_icon_poke_icon_516_01_NCGR + form - 1 );
			return ( 541 + form - 1 );
		}
		if( mons == MONSNO_ROTOMU )
		{
			//return ( NARC_poke_icon_poke_icon_519_01_NCGR + form - 1 );
			return ( 542 + form - 1 );
		}
	}

	if( mons > MONSNO_END ){ mons = 0; }
//	if( mons > MONSNO_DEOKISISU ){ mons = 0; }

	//return ( NARC_poke_icon_poke_icon_000_NCGR + mons );}
	return ( 7 + mons );
}
