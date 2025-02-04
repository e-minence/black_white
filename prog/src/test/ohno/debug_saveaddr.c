


//=============================================================================
/**
 * @file	  debug_saveaddr.c
 * @brief	  PDWでアクセスする物のアドレスリスト
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

#if PM_DEBUG

#include "arc_def.h"

#include "system/main.h"  //HEAPID
#include "debug_message.naix"
#include "debug/cgearpict.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "net/network_define.h"
#include "savedata/wifilist.h"
#include "msg/debug/msg_d_ohno.h"
#include "gamesystem/game_data.h"

#include "savedata/dreamworld_data.h"
#include "savedata/mystatus.h"
#include "../../savedata/mystatus_local.h"
#include "savedata/record.h"
#include "savedata/c_gear_data.h"
#include "savedata/c_gear_picture.h"
#include "savedata/save_tbl.h"


typedef struct _DEBUGSAVEADDR_WORK DEBUGSAVEADDR_WORK;

typedef void (StateFunc)(DEBUGSAVEADDR_WORK* pState);



static void _changeState(DEBUGSAVEADDR_WORK* pWork,StateFunc* state);
static void _changeStateDebug(DEBUGSAVEADDR_WORK* pWork,StateFunc* state, int line);

#define _NET_DEBUG (1)

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



struct _DEBUGSAVEADDR_WORK {
	GFL_FONT		  *fontHandle;
	GFL_MSGDATA		*mm;
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	HEAPID heapID;
	STRBUF			*strbuf;
	STRBUF			*strbufEx;
  WORDSET			*WordSet;								// メッセージ展開用ワークマネージャー
  GAMEDATA      *pGameData;
	SAVE_CONTROL_WORK* pSaveData;
	StateFunc* state;      ///< ハンドルのプログラム状態
	vu32 count;
  u8* pCGearWork;

	void* buffer;
	int bufferSize;
	int hz;
	int req;
	int getdataCount;
	BOOL bEnd;
//	DWCGHTTPPost post;
	u8 mac[6];
//	u8 buffer[0x102];
};



static void _msgPrint(DEBUGSAVEADDR_WORK* pWork,int msg)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbuf);
	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}

static void _msgPrintNo(DEBUGSAVEADDR_WORK* pWork,int msg,int no)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbufEx);

	WORDSET_RegisterNumber(pWork->WordSet, 0, no,
												 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
	WORDSET_ExpandStr( pWork->WordSet, pWork->strbuf,pWork->strbufEx );


	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}


//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(DEBUGSAVEADDR_WORK* pWork,StateFunc state)
{
	pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef _NET_DEBUG
static void _changeStateDebug(DEBUGSAVEADDR_WORK* pWork,StateFunc state, int line)
{
#ifdef DEBUG_ONLY_FOR_ohno
	OS_TPrintf("ghttp: %d\n",line);
#endif
	_changeState(pWork, state);
}
#endif





#if 1    //図鑑のバイナリデータを作る

#include "include/savedata/zukan_wp_savedata.h"
#include "zukan_gra.naix"

// セーブデータ
typedef struct  {
	// カスタムグラフィックキャラ
	u8	customChar[ZUKANWP_SAVEDATA_CHAR_SIZE];
	// カスタムグラフィックパレット
	u16	customPalette[ZUKANWP_SAVEDATA_PAL_SIZE];
	// データ有無フラグ
	BOOL	flg;
} TESTZUKAN_DATA;



static void _dataPrint(u8* pU8, int size)
{
  int i;

  for(i=0;i<size;i++){
    OS_TPrintf("0x%x ",pU8[i]);
    if((i%16)==15){
      OS_TPrintf("\n");
    }
  }
}



static BOOL zukandummydata( DEBUGSAVEADDR_WORK *pWork )
{
  int size,i;
  u16 crc=0;
  ARCHANDLE* p_handle;
  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  NNSG2dScreenData* pScrData;
  void* pArc;
  u8* pPic = GFL_HEAP_AllocMemory(GFL_HEAPID_APP,ZUKANWP_SAVEDATA_CHAR_SIZE+ZUKANWP_SAVEDATA_PAL_SIZE*2);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, GFL_HEAPID_APP );

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_zukan_gra_top_zkn_top_dummy_NCGR, FALSE, GFL_HEAPID_APP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    _dataPrint(charData->pRawData, ZUKANWP_SAVEDATA_CHAR_SIZE);
    GFL_STD_MemCopy(charData->pRawData, pPic, ZUKANWP_SAVEDATA_CHAR_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_zukan_gra_top_zkn_top_bg_NCLR, FALSE, GFL_HEAPID_APP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    _dataPrint(palData->pRawData, ZUKANWP_SAVEDATA_PAL_SIZE*2);
    GFL_STD_MemCopy(palData->pRawData, &pPic[ZUKANWP_SAVEDATA_CHAR_SIZE], ZUKANWP_SAVEDATA_PAL_SIZE*2);
  }
  GFL_HEAP_FreeMemory(pArc);

  GFL_ARC_CloseDataHandle( p_handle );

  size = ZUKANWP_SAVEDATA_CHAR_SIZE+ZUKANWP_SAVEDATA_PAL_SIZE*2;
  crc = GFL_STD_CrcCalc( pPic, size );

  OS_TPrintf("-----%x \n",crc);

  GFL_HEAP_FreeMemory(pPic);

  return( FALSE );
}


#endif


#if 1

static BOOL cgeardummydata( DEBUGSAVEADDR_WORK *pWork )
{
  int i;
  u16 crc=0;
  ARCHANDLE* p_handle;
  void* pCRC;
  int size = CGEAR_PICTURTE_CHAR_SIZE+CGEAR_PICTURTE_PAL_SIZE+CGEAR_PICTURTE_SCR_SIZE;
  int sizeh = size/2;
  CGEAR_PICTURE_SAVEDATA* pPic;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(pWork->pGameData);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_DEBUG_CGEAR_PICT, GFL_HEAPID_APP );

  pCRC = GFL_ARCHDL_UTIL_Load( p_handle, NARC_cgearpict_mun_decal01_bin, FALSE, GFL_HEAPID_APP);

  crc = GFL_STD_CrcCalc( pCRC, size );

  
  pWork->pCGearWork = GFL_HEAP_AllocMemory(GFL_HEAPID_APP,SAVESIZE_EXTRA_CGEAR_PICTURE);
  pPic = (CGEAR_PICTURE_SAVEDATA*)pWork->pCGearWork;

  CGEAR_SV_SetCGearPictureONOFF(CGEAR_SV_GetCGearSaveData(pSave),TRUE);  //CGEARデカール有効

  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, GFL_HEAPID_APP,
                             pWork->pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE);
  GFL_STD_MemCopy(pCRC, pPic->picture, size);
  CGEAR_SV_SetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave),crc);  //CGEARデカール有効
  GAMEDATA_ExtraSaveAsyncStart(pWork->pGameData, SAVE_EXTRA_ID_CGEAR_PICUTRE);

  while(1){
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(pWork->pGameData);
    if(SAVE_RESULT_OK==GAMEDATA_ExtraSaveAsyncMain(pWork->pGameData,SAVE_EXTRA_ID_CGEAR_PICUTRE)){
      break;
    }
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
  return TRUE;
}

#endif

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータのアドレス
 * @retval  none
 */
//------------------------------------------------------------------------------

#include "savedata/situation.h"
#include "savedata/mystery_data.h"
#include "savedata/record.h"
#include "system\pms_data.h"
#include "savedata/worldtrade_data.h"
#include "savedata/sodateya_work.h"
#include "savedata/misc.h"
#include "savedata/misc_local.h"
#include "savedata/intrude_save.h"
#include "savedata/system_data_local.h"
#include "savedata/dreamworld_data_local.h"
#include "poke_tool/pokeparty.h"
#include "savedata/wifihistory.h"
#include "savedata/wifihistory_local.h"
#include "savedata/playtime.h"
#include "savedata/playtime_local.h"
#include "savedata/regulation.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_local.h"
#include "savedata/myitem_savedata_local.h"
#include "net/dreamworld_netdata.h"
#include "savedata/zukan_savedata_local.h"
#include "savedata/symbol_save.h"

#include "savedata/wifi_negotiation.h"

#include "savedata/wifilist.h"
#include "savedata/wifilist_local.h"


extern MYITEM_PTR* SaveData_GetMyItem(SAVE_CONTROL_WORK * sv);
extern WIFI_LIST* SaveData_GetWifiListData(SAVE_CONTROL_WORK * sv);



static void _keyWait(DEBUGSAVEADDR_WORK* pWork)
{
  {
    int i;
    u32 size;
    u8* pAddr;
    u8* topAddr = (u8*)SaveControl_GetSaveWorkAdrs(pWork->pSaveData, &size);

    //zukandummydata(pWork);
    cgeardummydata(pWork);
    
    OS_TPrintf("SAVESIZE ,%x\n", size);

    {
      u8 data[]="abcdefg";
      OS_TPrintf("CRC ,%x %d\n", GFL_STD_CrcCalc(data,7) , sizeof(REGULATION_CARDDATA));
    }
    {
      DREAM_WORLD_SERVER_DOWNLOAD_DATA buff;

      GFL_STD_MemFill(&buff, 0, sizeof(buff));

      // buff に値を設定
//      buff.findPokemon = 100;
//      buff.findPokemon = 50;
 //     buff.findPokemonForm = 1;
      
      OS_TPrintf("CRC ,%x %d\n", GFL_STD_CrcCalc(&buff,sizeof(buff)-2) , sizeof(buff));
    }
    

    {  //シンボルエンカウント
      SYMBOL_SAVE_WORK* pSym = SymbolSave_GetSymbolData(pWork->pSaveData);

      SymbolSave_WorkInit(pSym);
      SymbolSave_SetFreeZone(pSym, 12, 12, 0, 0, 0, SYMBOL_ZONE_TYPE_FREE_SMALL);
      SymbolSave_SetFreeZone(pSym, 13, 13, 0, 0, 0, SYMBOL_ZONE_TYPE_FREE_SMALL);
      SymbolSave_SetFreeZone(pSym, 14, 14, 0, 0, 0, SYMBOL_ZONE_TYPE_FREE_SMALL);
      {
        SYMBOL_POKEMON aSymbol;
        SymbolSave_GetSymbolPokemon(pSym, 0, &aSymbol);
        OS_TPrintf("mons %d",aSymbol.monsno);
        OS_TPrintf("waza %d ",aSymbol.wazano);
        OS_TPrintf("fem %d",aSymbol.sex);
        OS_TPrintf("move %d",aSymbol.move_type);
        OS_TPrintf("form %d\n",aSymbol.form_no);
      }
      {
        SYMBOL_POKEMON aSymbol;
        SymbolSave_GetSymbolPokemon(pSym, 1, &aSymbol);
        OS_TPrintf("mons %d",aSymbol.monsno);
        OS_TPrintf("waza %d ",aSymbol.wazano);
        OS_TPrintf("fem %d",aSymbol.sex);
        OS_TPrintf("move %d",aSymbol.move_type);
        OS_TPrintf("form %d\n",aSymbol.form_no);
      }
      {
        SYMBOL_POKEMON aSymbol;
        SymbolSave_GetSymbolPokemon(pSym, 2, &aSymbol);
        OS_TPrintf("mons %d",aSymbol.monsno);
        OS_TPrintf("waza %d ",aSymbol.wazano);
        OS_TPrintf("fem %d",aSymbol.sex);
        OS_TPrintf("move %d",aSymbol.move_type);
        OS_TPrintf("form %d\n",aSymbol.form_no);
      }
    }
      
    
/*
    for(i=0;i<26;i++){
      REGULATION* pData = GFL_ARC_UTIL_Load(ARCID_REGULATION, i, FALSE, HEAPID_PROC );

      OS_TPrintf("read %d \n",i);

      OS_TPrintf("LOHI %d %d\n",Regulation_GetParam(pData, REGULATION_NUM_LO), Regulation_GetParam(pData, REGULATION_NUM_HI));
      GFL_HEAP_FreeMemory(pData);
    }
*/
    /*
    {//ミステリー関連 
      MYSTERY_DATA *pMystery = MYSTERYDATA_GetCardDataOld(pWork->pSaveData); 
      GIFT_PACK_DATA  *pGift  = MYSTERYDATA_GetCardData( pMystery, 0 );
      pAddr = (u8*)pGift;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","MYSTERY", (u32)pAddr-(u32)topAddr, sizeof(GIFT_PACK_DATA));
      OS_TPrintf( "download %d  pack%d present%d\n", sizeof(DOWNLOAD_GIFT_DATA), sizeof(GIFT_PACK_DATA), sizeof(GIFT_PRESENT) );
    }
*/
    
    {//DreamWorld関連
      DREAMWORLD_SAVEDATA* pDW = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
      pAddr = (u8*)DREAMWORLD_SV_GetSleepPokemon(pDW);
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","SLEEP_POKEMON", (u32)pAddr-(u32)topAddr, POKETOOL_GetWorkSize());
      pAddr = (u8*)&pDW->pokemonIn;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","ISIN_SLEEP_POKEMON", (u32)pAddr-(u32)topAddr, sizeof(u8));
      pAddr = (u8*)&pDW->bAccount;  //ここは並びが変更されるとまずい
      pAddr++;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","FURNITURENO", (u32)pAddr-(u32)topAddr, sizeof(u8));
      
    }
    {//MISC
      MISC* pMisc = SaveData_GetMisc(pWork->pSaveData);
      pAddr = (u8*)&pMisc->gold;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYER_GOLD", (u32)pAddr-(u32)topAddr, sizeof(pMisc->gold));
      pAddr = (u8*)&pMisc->badge;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYER_BADGE_BIT", (u32)pAddr-(u32)topAddr, sizeof(pMisc->badge));
    }


    {//Myステータス
      MYSTATUS* pMy = SaveData_GetMyStatus(pWork->pSaveData);
      pAddr = (u8*)&pMy->name;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYER_NAME", (u32)pAddr-(u32)topAddr, sizeof(pMy->name));
      pAddr = (u8*)&pMy->id;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "PLAYER_ID",(u32)pAddr-(u32)topAddr,sizeof(pMy->id));
      pAddr = (u8*)&pMy->sex;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "PLAYER_SEX",((u32)pAddr-(u32)topAddr), sizeof(pMy->sex));
      pAddr = (u8*)&pMy->nation;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","MY_NATION", (u32)pAddr-(u32)topAddr, sizeof(pMy->nation));
      pAddr = (u8*)&pMy->area;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","MY_AREA", (u32)pAddr-(u32)topAddr, sizeof(pMy->area));
    }
    {//PLAYTIME
      PLAYTIME * playtime = SaveData_GetPlayTime(pWork->pSaveData);

      pAddr = (u8*)&playtime->hour;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYTIME_HOUR", (u32)pAddr-(u32)topAddr, sizeof(playtime->hour));
      pAddr = (u8*)&playtime->minute;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYTIME_MINUTE", (u32)pAddr-(u32)topAddr, sizeof(playtime->minute));
      pAddr = (u8*)&playtime->second;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PLAYTIME_SECOND", (u32)pAddr-(u32)topAddr, sizeof(playtime->second));
    }



    { //GTSネゴ履歴
      WIFI_NEGOTIATION_SAVEDATA* pGTSNEGO = WIFI_NEGOTIATION_SV_GetSaveData(pWork->pSaveData);

      for(i=0;i<WIFI_NEGOTIATION_DATAMAX;i++){
        MYSTATUS* pMys = WIFI_NEGOTIATION_SV_GetMyStatus_DEBUG(pGTSNEGO, i);
        pAddr = (u8*)&pMys->profileID;
        OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "GTSNEGO_GSID",(u32)pAddr-(u32)topAddr,sizeof(pMys->profileID));
      }
    }

#if 1
    { //ジオネット
      WIFI_HISTORY* pHis = SaveData_GetWifiHistory(pWork->pSaveData);
#if 0
      for(i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++){
        MYSTATUS* pMys = &pHis->aUnitedPeople[i].aMyStatus;
        pAddr = (u8*)&pMys->profileID;
        OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "GTS_GSID",(u32)pAddr-(u32)topAddr,sizeof(pMys->profileID));
    //    OS_TPrintf(" id %d \n",pMys->profileID);
      }
#endif 
      pAddr = (u8*)&pHis->myCountryCount;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","GTS_COUNTRYCOUNT", (u32)pAddr-(u32)topAddr, sizeof(pHis->myCountryCount));
    }
#endif

    {  //FRIENDCODE

      

      

    }

    
    {//レコード
      short* rec = (short*)SaveData_GetRecord(pWork->pSaveData);
      int num = RECORD_GetWorkSize();

      GF_ASSERT(!(num%2));  //レコードデータは４バイトアライメントでないと

      num = num /2;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","RECORDTOP_ADDR", (u32)&rec[0]-(u32)topAddr, 4);
      OS_TPrintf("\"%s\",\"\",\"%d\"\n","RECORDSIZE", (u32)RECORD_GetWorkSize() );

      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","RECORD_CRC", (u32)&rec[num-2]-(u32)topAddr , 2);
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","RECORD_CODED_NUM", (u32)&rec[num-1]-(u32)topAddr, 2);

      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","RECID_CAPTURE_POKE", (u32)&rec[RECID_CAPTURE_POKE]-(u32)topAddr, 4);
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","RECID_FISHING_SUCCESS", (u32)&rec[RECID_FISHING_SUCCESS]-(u32)topAddr, 4);

    }
    
    {//シンボルエンカウント
//      SYSTEMDATA* pSys = SaveData_GetSystemData(pWork->pSaveData);
      SYMBOL_SAVE_WORK* pSym = SymbolSave_GetSymbolData(pWork->pSaveData);

      pAddr = (u8*)pSym;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "SYMBOL_SAVE_WORK",(u32)pAddr-(u32)topAddr,sizeof(SYMBOL_SAVE_WORK));

    }

    //フレンドProfileID
    {
      WIFI_LIST* pWifi = SaveData_GetWifiListData(pWork->pSaveData);

      for(i=0;i<32;i++){
        pAddr = (u8*)&pWifi->friendData[i].GameSyncID;

  //      OS_TPrintf("%d \n",pWifi->friendData[i].GameSyncID);
        OS_TPrintf("\"%s%2d\",\"0x%x\",\"%d\"\n", "FRIEND_GAMESYNCID",
                   i,(u32)pAddr-(u32)topAddr,sizeof(pWifi->friendData[i].GameSyncID));
      }
    }
    


    
    {//システム
//      SYSTEMDATA* pSys = SaveData_GetSystemData(pWork->pSaveData);

//      pAddr = (u8*)&pSys->profileId;
//      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","PROFILE_ID", (u32)pAddr-(u32)topAddr, sizeof(pSys->profileId));
      MYSTATUS* pMy = SaveData_GetMyStatus(pWork->pSaveData);

      pAddr = (u8*)&pMy->profileID;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n", "PROFILE_ID",(u32)pAddr-(u32)topAddr,sizeof(pMy->profileID));

    }

    {//アイテム
      MYITEM* pMyItem = (MYITEM*)SaveData_GetMyItem(pWork->pSaveData);

      pAddr = (u8*)pMyItem;
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","MYBAG_START", (u32)pAddr-(u32)topAddr, sizeof(ITEM_ST));

      OS_TPrintf("\"%s\",\"0\",\"%d\"\n","ITEMST_TOTAL_NUM", (u32)BAG_TOTAL_NUM );
      OS_TPrintf("\"%s\",\"0\",\"%d\"\n","ITEM_MAX", (u32)ITEM_DATA_MAX);

      pAddr = (u8*)&pMyItem->MyNutsItem[0];
      
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","NUTSBAG_START", (u32)pAddr-(u32)topAddr, sizeof(ITEM_ST));

      OS_TPrintf("\"%s\",\"0\",\"%d\"\n","BAG_NUTS_ITEM_MAX ", (u32)BAG_NUTS_ITEM_MAX );
      OS_TPrintf("\"%s\",\"0\",\"%d\"\n","ITEM_NUTS_MAX", (u32)ITEM_NUTS_MAX);
      
    }

    {//図鑑
      ZUKAN_SAVEDATA *pZukan = ZUKAN_SAVEDATA_GetZukanSave( pWork->pSaveData );

      pAddr = (u8*)&pZukan->get_flag[0];
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","POKEMONGET_BIT_ARRAY", (u32)pAddr-(u32)topAddr, sizeof(u32)*POKEZUKAN_ARRAY_LEN);
      
      OS_TPrintf("\"%s\",\"0x%x\",\"%d\"\n","ZUKANSAVE_ZENKOKU_MONSMAX", (u32)0, ZUKANSAVE_ZENKOKU_MONSMAX);
      

    }

    
    {//手持ちポケモン
      POKEPARTY* Party = SaveData_GetTemotiPokemon(pWork->pSaveData);
      for(i = 0 ; i < 6 ; i++){
        pAddr = (u8*)PokeParty_GetMemberPointer( Party, i );
        OS_TPrintf("\"%s%d\",\"0x%x\",\"%d\"\n","PARTY_POKEMON",i, (u32)pAddr-(u32)topAddr, POKETOOL_GetWorkSize());
      }
    }

    {//ボックスポケモン
      BOX_MANAGER* pBox = BOX_DAT_InitManager( GFL_HEAPID_APP , pWork->pSaveData);
      int j,i;
      for(i=0;i<BOX_MAX_TRAY;i++){
        for(j=0;j<BOX_MAX_POS;j++){
          pAddr = (u8*)BOXDAT_GetPokeDataAddress(pBox, i, j);
          OS_TPrintf("\"%s%d\",\"0x%x\",\"%d\"\n","BOX_POKEMON",i*BOX_MAX_POS+j, (u32)pAddr-(u32)topAddr, POKETOOL_GetPPPWorkSize());
        }
      }
    }
  }

  {
    int size,sizetotal=0;
    size = Situation_GetWorkSize();
    OS_TPrintf("%d Situation_GetWorkSize\n",size);
    sizetotal+=size;

    size = MYSTERYDATA_GetWorkSize();
    OS_TPrintf("%d MYSTERYDATA_GetWorkSize\n",size);
    OS_TPrintf("%d MYSTERYDATA  GIFT_PRESENT_POKEMON\n",sizeof(GIFT_PRESENT_POKEMON));
    sizetotal+=size;

    size = RECORD_GetWorkSize();
    OS_TPrintf("%d RECORD_GetWorkSize\n",size);
    sizetotal+=size;

    size = PMSW_GetSaveDataSize();
    OS_TPrintf("%d PMSW_GetSaveDataSize\n",size);
    sizetotal+=size;

    size = WorldTradeData_GetWorkSize();
    OS_TPrintf("%d WorldTradeData_GetWorkSize\n",size);
    sizetotal+=size;

    size = SODATEYA_WORK_GetWorkSize();
    OS_TPrintf("%d SODATEYA_WORK_GetWorkSize\n",size);
    sizetotal+=size;

    size = MISC_GetWorkSize();
    OS_TPrintf("%d MISC_GetWorkSize\n",size);
    sizetotal+=size;

    size = IntrudeSave_GetWorkSize();
    OS_TPrintf("%d IntrudeSave_GetWorkSize\n",size);
    sizetotal+=size;

    size = DREAMWORLD_SV_GetWorkSize();
    OS_TPrintf("%d DREAMWORLD_SV_GetWorkSize\n",size);
    sizetotal+=size;

    OS_TPrintf("%d total\n",sizetotal);
  }

  _CHANGE_STATE(NULL);

}




static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0123_E,			// テクスチャパレットスロット
		GX_OBJVRAMMODE_CHAR_1D_32K,	// メインOBJマッピングモード
		GX_OBJVRAMMODE_CHAR_1D_32K,		// サブOBJマッピングモード
};


static GFL_PROC_RESULT DebugSaveAddrProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, 0x70000 );//テスト

	{
		DEBUGSAVEADDR_WORK* pWork = GFL_PROC_AllocWork( proc, sizeof(DEBUGSAVEADDR_WORK), HEAPID_PROC );


    pWork->pGameData = GAMEDATA_Create( GFL_HEAPID_APP );
    pWork->pSaveData = SaveControl_GetPointer();  //デバッグ
		pWork->heapID = HEAPID_PROC;

		GFL_DISP_SetBank( &vramBank );

		//バックグラウンドの色を入れておく
		GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x5ad6, 2);
	
		// 各種効果レジスタ初期化
		G2_BlendNone();

		// BGsystem初期化
		GFL_BG_Init( pWork->heapID );
		GFL_BMPWIN_Init( pWork->heapID );
		GFL_FONTSYS_Init();

			//ＢＧモード設定
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// 個別フレーム設定
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// 上下画面設定
		GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 4, 12, 24, 16, 0, GFL_BMP_CHRAREA_GET_F );
		pWork->bmp = GFL_BMPWIN_GetBmp( pWork->win);
		GFL_BMP_Clear( pWork->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( pWork->win );
		GFL_BMPWIN_TransVramCharacter( pWork->win );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
																			GFL_FONT_LOADTYPE_FILE, FALSE, pWork->heapID );

		pWork->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_ohno_dat, pWork->heapID );
		pWork->strbuf = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->strbufEx = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->WordSet    = WORDSET_Create( pWork->heapID );

	//フォントパレット転送
		GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
																	0, 0x20, HEAPID_PROC);

		
		_CHANGE_STATE(_keyWait);
	}
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DebugSaveAddrProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	DEBUGSAVEADDR_WORK* pWork = mywk;
	StateFunc* state = pWork->state;

	if( state ){
		state( pWork );
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DebugSaveAddrProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	DEBUGSAVEADDR_WORK* pWork = mywk;

	GFL_STR_DeleteBuffer(pWork->strbuf);
	GFL_STR_DeleteBuffer(pWork->strbufEx);
	GFL_MSG_Delete(pWork->mm);
	GFL_FONT_Delete(pWork->fontHandle);

	WORDSET_Delete( pWork->WordSet );

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_PROC);  //テスト
  return GFL_PROC_RES_FINISH;
}

// プロセス定義データ
const GFL_PROC_DATA DebugSaveAddrProcData = {
  DebugSaveAddrProc_Init,
  DebugSaveAddrProc_Main,
  DebugSaveAddrProc_End,
};

#endif


#if 0  //デバッグ用使う事があるかもしれないのでここに置く


//#if 0
#if DEBUG_ONLY_FOR_ohno
//--------------------------------------------------------------
/**
 * @brief   デバッグ ポケモンをセーブデータに入れる
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
#include "savedata/dreamworld_data.h"
#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "savedata/record.h"
#include "savedata/misc.h"
#include "savedata/undata_update.h"
#include "savedata/wifihistory.h"
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  int i;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData(wk->gmSys);

    {//MISC
      MISC* pMisc = SaveData_GetMisc(GAMEDATA_GetSaveControlWork(gamedata));
      MISC_SetGold( pMisc, GFUser_GetPublicRand( 12345 ));
      OS_TPrintf("pMisc gold=%d\n",MISC_GetGold(pMisc));
    }

    {//レコード
      RECORD* pRec = GAMEDATA_GetRecordPtr(GAMESYSTEM_GetGameData(wk->gmSys));

      RECORD_Set(pRec, RECID_FISHING_SUCCESS, GFUser_GetPublicRand( 12345 ));
      RECORD_Set(pRec, RECID_WIFI_TRADE, GFUser_GetPublicRand( 12345 ));
      RECORD_Set(pRec, RECID_COMM_TRADE, GFUser_GetPublicRand( 12345 ));
      RECORD_Set(pRec, RECID_IRC_TRADE, GFUser_GetPublicRand( 12345 ));
      RECORD_Set(pRec, RECID_CAPTURE_POKE, GFUser_GetPublicRand( 12345 ));
      OS_TPrintf("RECID_FISHING_SUCCESS=%d\n",RECORD_Get(pRec,RECID_FISHING_SUCCESS));
      OS_TPrintf("RECID_CAPTURE_POKE=%d\n",RECORD_Get(pRec,RECID_CAPTURE_POKE));
      OS_TPrintf("RECID_WIFI_TRADE=%d\n",RECORD_Get(pRec,RECID_WIFI_TRADE));
      OS_TPrintf("RECID_COMM_TRADE=%d\n",RECORD_Get(pRec,RECID_COMM_TRADE));
      OS_TPrintf("RECID_IRC_TRADE=%d\n",RECORD_Get(pRec,RECID_IRC_TRADE));

    }
    {//国連
      WIFI_HISTORY* pHis = SaveData_GetWifiHistory(GAMEDATA_GetSaveControlWork(gamedata));
      for(i=0;i<20;i++){
        UNITEDNATIONS_SAVE add_data;
        MYSTATUS aMy;
        aMy.profileID= GFUser_GetPublicRand(3333);
        aMy.nation = i+1;
        MyStatus_Copy( &aMy , &add_data.aMyStatus);
        add_data.recvPokemon = 1;  //貰ったポケモン
        add_data.sendPokemon = 100;  //あげたポケモン
        add_data.favorite = 1;   //趣味
        add_data.countryCount = 2;  //交換した国の回数
        add_data.nature = 1;   //性格
        UNDATAUP_Update(pHis, &add_data);
      }
      for(i=0;i<20;i++){
        MYSTATUS* pMy = WIFIHISTORY_GetUnMyStatus(pHis, i);
        OS_TPrintf("GTSID = %d\n",pMy->profileID);
      }
    }
  }

 // GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpEasy( wk->gmSys, wk->heapID ) );
  return( FALSE );
}


//--------------------------------------------------------------
/**
 * @brief     CGearの絵をROMから読み出しセーブ領域に入れる
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval    BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
//#endif
//#if 1
#else

static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));

  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  NNSG2dScreenData* scrData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_CGEAR_PICTURE);
  CGEAR_PICTURE_SAVEDATA* pPic=(CGEAR_PICTURE_SAVEDATA*)pCGearWork;

  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, HEAPID_FIELDMAP,
                             pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );
  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->picture, CGEAR_DECAL_SIZE_MAX);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    GFL_STD_MemCopy(palData->pRawData, pPic->palette, CGEAR_PICTURTE_PAL_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_c_gear00_NSCR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedScreenData( pArc, &scrData ) ){
    TOMOYA_Printf( "scrData size %d == %d\n", scrData->szByte, CGEAR_PICTURTE_SCR_SIZE );
    GFL_STD_MemCopy(scrData->rawData, pPic->scr, CGEAR_PICTURTE_SCR_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);


  GFL_ARC_CloseDataHandle( p_handle );

  crc = GFL_STD_CrcCalc( pPic, CGEAR_PICTURTE_ALL_SIZE );

  GAMEDATA_ExtraSaveAsyncStart(GAMESYSTEM_GetGameData(gameSys),SAVE_EXTRA_ID_CGEAR_PICUTRE);
  while(1){
    if(SAVE_RESULT_OK==GAMEDATA_ExtraSaveAsyncMain(GAMESYSTEM_GetGameData(gameSys),SAVE_EXTRA_ID_CGEAR_PICUTRE)){
      break;
    }
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
  GFL_HEAP_FreeMemory(pCGearWork);
  CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(pSave), TRUE);  //CGEAR表示有効
  CGEAR_SV_SetCGearPictureONOFF(CGEAR_SV_GetCGearSaveData(pSave),TRUE);  //CGEARデカール有効
  CGEAR_SV_SetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave),crc);  //CGEARデカール有効

  return( FALSE );
}
#endif
//#endif

#if 0    //図鑑テスト

#include "include/savedata/zukan_wp_savedata.h"
// セーブデータ
typedef struct  {
  // カスタムグラフィックキャラ
  u8  customChar[ZUKANWP_SAVEDATA_CHAR_SIZE];
  // カスタムグラフィックパレット
  u16 customPalette[ZUKANWP_SAVEDATA_PAL_SIZE];
  // データ有無フラグ
  BOOL  flg;
} TESTZUKAN_DATA;



static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  int size,i;
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));
  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  NNSG2dScreenData* pScrData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_ZUKAN_WALLPAPER);
  TESTZUKAN_DATA* pPic=(TESTZUKAN_DATA*)pCGearWork;

  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_ZUKAN_WALLPAPER, HEAPID_FIELDMAP,
                             pCGearWork,SAVESIZE_EXTRA_ZUKAN_WALLPAPER);

  pPic->flg=TRUE;

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->customChar, ZUKANWP_SAVEDATA_CHAR_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    u16 * pal = (u16 *)palData->pRawData;
    for(i=0;i<16;i++){
      GFL_STD_MemCopy( &pal[16*i], &pPic->customPalette[16*i], 32);
    }
  }
  GFL_HEAP_FreeMemory(pArc);

  GFL_ARC_CloseDataHandle( p_handle );

  size = ZUKANWP_SAVEDATA_CHAR_SIZE+ZUKANWP_SAVEDATA_PAL_SIZE*2;
  crc = GFL_STD_CrcCalc( pPic, size );

  {
    int i;
    u8* pU8 = (u8*)pPic;
    for(i=0;i<size;i++){
      OS_TPrintf("0x%x ",pU8[i]);
      if((i%16)==15){
        OS_TPrintf("\n");
      }
    }
  }

  OS_TPrintf("-----%x \n",crc);

  SaveControl_Extra_SaveAsyncInit(pSave,SAVE_EXTRA_ID_ZUKAN_WALLPAPER);
  while(1){
    if(SAVE_RESULT_OK==SaveControl_Extra_SaveAsyncMain(pSave,SAVE_EXTRA_ID_ZUKAN_WALLPAPER)){
      break;
    }
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_ZUKAN_WALLPAPER);


  GFL_HEAP_FreeMemory(pCGearWork);

  return( FALSE );
}
#endif




#endif
