//======================================================================
/**
 *
 * @file  mb_data_pt.c 
 * @brief wbダウンロードプレイ　HGSS用機種別コード
 * @author  ariizumi
 * @data  09/11/23
 */
//======================================================================
#include <gflib.h>
#include <string.h>
#include <backup_system.h>
#include <nitro/card/backup_ex.h>
#include "system/gfl_use.h"
#include "system/save_error_warning.h"
#include "./pt_save.h"
#include "arc_def.h"

#include "poke_tool/poke_tool.h"
#include "multiboot/mb_data_main.h"
#include "item/itemsym.h"
#include "./mb_data_gs.h"

#include "./gs_save.cdat"  //mb_data_gs.c 以外で読むな
#include "./mb_item.cdat"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================
enum GS_SAVE_TYPE
{
  GS_MAIN_FIRST = 0,
  GS_BOX_FIRST  = 1,
  GS_MAIN_SECOND = 2,
  GS_BOX_SECOND = 3,
};

//======================================================================
//  typedef struct
//======================================================================

//======================================================================
//  proto
//======================================================================

BOOL  MB_DATA_GS_LoadData( MB_DATA_WORK *dataWork );
BOOL  MB_DATA_GS_SaveData( MB_DATA_WORK *dataWork );
void  MB_DATA_GS_SetDataCRC( MB_DATA_WORK *dataWork , void *data , const u32 size );

u32   MB_DATA_GS_GetStartAddress( const GS_GMDATA_ID id );
static  BOOL MB_DATA_GS_CheckDataCorrect( GS_SAVE_FOOTER **pFooterArr , MB_DATA_WORK *dataWork );
static u8 MB_DATA_GS_CompareFooterData( GS_SAVE_FOOTER *fData , BOOL fCorr ,
                       GS_SAVE_FOOTER *sData , BOOL sCorr ,
                       u8 *pos );
static const BOOL MB_DATA_GS_AddItemFunc( MB_DATA_WORK *dataWork , u16 itemNo , GS_MINEITEM *mineItem , const u16 arrMax );
static void MB_DATA_GS_SortItem( MB_DATA_WORK *dataWork , GS_MINEITEM *mineItem , const u16 arrMax );
static const u16 MB_DATA_GS_GetItemNumFunc( MB_DATA_WORK *dataWork , u16 itemNo , GS_MINEITEM *mineItem , const u16 arrMax );



//  データの読み込み
BOOL  MB_DATA_GS_LoadData( MB_DATA_WORK *dataWork )
{
  const u32 saveSize = MB_DATA_GetSavedataSize( dataWork->cardType );
  switch( dataWork->subSeq )
  {
  case 0: //初期化およびデータ読み込み
    {
#if DLPLAY_FUNC_USE_PRINT
    char str[256];
    sprintf(str,"Start load data. size:[%d]", saveSize );
    //DLPlayFunc_PutString(str,dataWork->msgSys );
#endif
    {
      s32 lockID = OS_GetLockID();
      GF_ASSERT_HEAVY( lockID != OS_LOCK_ID_ERROR );
      CARD_LockRom( (u16)lockID );
      CARD_CheckPulledOut();  //抜き検出
      CARD_UnlockRom( (u16)lockID );
      OS_ReleaseLockID( (u16)lockID );
    }
    dataWork->pData    = GFL_HEAP_AllocClearMemory(  dataWork->heapId, saveSize );
    dataWork->pDataMirror= GFL_HEAP_AllocClearMemory(  dataWork->heapId, saveSize );
  
    dataWork->lockID_ = OS_GetLockID();
    GF_ASSERT_HEAVY( dataWork->lockID_ != OS_LOCK_ID_ERROR );
    //プラチナは4MBフラッシュ
    CARD_LockBackup( (u16)dataWork->lockID_ );
#if PM_DEBUG
    if( dataWork->isDummyCard == TRUE )
    {
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
      {
        CARD_IdentifyBackup( CARD_BACKUP_TYPE_FLASH_4MBITS );
      }
      else
      {
        CARD_IdentifyBackup( CARD_BACKUP_TYPE_FLASH_4MBITS_IRC );
      }
    }
    else
#endif
    {
#if PM_DEBUG
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
      {
        CARD_IdentifyBackup( CARD_BACKUP_TYPE_FLASH_4MBITS );
      }
      else
#endif
      {
        CARD_IdentifyBackup( CARD_BACKUP_TYPE_FLASH_4MBITS_IRC );
      }
    }
    CARD_ReadFlashAsync( 0x0000 , dataWork->pData , saveSize , NULL , NULL );
    dataWork->subSeq++;
    }
    break;
  case 1: //ミラーリングデータ読み込み
    //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        SaveErrorCall_Load();
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
      CARD_ReadFlashAsync( 0x40000 , dataWork->pDataMirror , saveSize , NULL , NULL );
      dataWork->subSeq++;
      
      //DLPlayFunc_PutString("Data1 load complete.",dataWork->msgSys );
    }

    break;
  case 2: //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        SaveErrorCall_Load();
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
      CARD_UnlockBackup( (u16)dataWork->lockID_ );
      OS_ReleaseLockID( (u16)dataWork->lockID_ );
      dataWork->subSeq++;
      
      //DLPlayFunc_PutString("Data2 load complete.",dataWork->msgSys );
    }
    break;
  case 3: //各ブロックのCRCチェック
    {
      //おそらくGSでは処理が変わる
      //各ブロックのCRCをチェック(一応通常データも見ておく
      u8 i;
      const u32 boxStartAdd = MB_DATA_GetStartAddress(GS_GMDATA_ID_BOXDATA , dataWork->cardType );
      const u32 itemStartAdd = MB_DATA_GetStartAddress(GS_GMDATA_ID_TEMOTI_ITEM , dataWork->cardType );
      const u32 mysteryStartAdd = MB_DATA_GetStartAddress(GS_GMDATA_ID_FUSHIGIDATA , dataWork->cardType );
      const u32 statusStartAdd = MB_DATA_GetStartAddress(GS_GMDATA_ID_PLAYER_DATA , dataWork->cardType );
      GS_SAVE_FOOTER *footer[4];
      footer[GS_MAIN_FIRST] = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER , dataWork->cardType ) ];
      footer[GS_BOX_FIRST]  = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER , dataWork->cardType ) ];
      footer[GS_MAIN_SECOND]= (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER , dataWork->cardType ) ];
      footer[GS_BOX_SECOND] = (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER , dataWork->cardType ) ];
#if DEB_ARI
      for( i=0;i<4;i++ )
      {
        MB_DATA_TPrintf("footer[%d] g_count[%2d] b_count[None] size[%5d] MGNo[%d] blkID[%02x] crc[%d]\n"
              ,i ,footer[i]->g_count ,footer[i]->size
              ,footer[i]->magic_number ,footer[i]->blk_id ,footer[i]->crc);
      }
#endif
      //ROMチェック用にCRCを保存
      for( i=0;i<4;i++ )
      {
        dataWork->cardCrcTable[i] = footer[i]->crc;
      }

      {
        const BOOL isCorrectData = MB_DATA_GS_CheckDataCorrect( &footer[0] , dataWork );
        if( isCorrectData == TRUE )
        {
          if( dataWork->boxLoadPos == DDS_FIRST )
          {
            dataWork->pBoxData = dataWork->pData + boxStartAdd;
            dataWork->boxSavePos = DDS_SECOND;
          }
          else
          {
            dataWork->pBoxData = dataWork->pDataMirror + boxStartAdd;
            dataWork->boxSavePos = DDS_FIRST;
          }
          
          if( dataWork->mainLoadPos == DDS_FIRST )
          {
            dataWork->pItemData = dataWork->pData + itemStartAdd;
            dataWork->pMyStatus = dataWork->pData + statusStartAdd;
            dataWork->pMysteryData = dataWork->pData + mysteryStartAdd;
            dataWork->mainSavePos = DDS_SECOND;
          }
          else
          {
            dataWork->pItemData = dataWork->pDataMirror + itemStartAdd;
            dataWork->pMyStatus = dataWork->pDataMirror + statusStartAdd;
            dataWork->pMysteryData = dataWork->pDataMirror + mysteryStartAdd;
            dataWork->mainSavePos = DDS_FIRST;
          }
        }
        else
        {
          dataWork->errorState = DES_MISS_LOAD_BACKUP;
        }
      }
    }
    //GF_ASSERT_HEAVY( dataWork->pBoxData != NULL );
    dataWork->subSeq++;
    break;
  
  case 4: 
    //国コードチェック
    if( dataWork->errorState == DES_NONE )
    {
      GS_MYSTATUS *myStatus = (GS_MYSTATUS*)dataWork->pMyStatus;
      MB_DATA_TPrintf("RegionCode[%d]\n",myStatus->region_code);
      if( myStatus->region_code != PM_LANG )
      {
        dataWork->errorState = DES_ANOTHER_COUNTRY;
      }
    }
#if DEB_ARI
    //BOXの解析(テストデータ変換は別の場所で
    if( dataWork->pBoxData != NULL )
    {
      int i,j;
      MB_DATA_TPrintf("BoxData Analyze...\n");
      for(i=0;i<GS_BOX_MAX_TRAY;i++)
      {
        for(j=0;j<GS_BOX_MAX_POS;j++)
        {
          GS_BOX_DATA *boxData = (GS_BOX_DATA*)dataWork->pBoxData;
          POKEMON_PASO_PARAM *ppp = MB_DATA_GS_GetBoxPPP(dataWork,i,j);
          const u32 monsNo = PPP_Get( ppp , ID_PARA_monsno , NULL );
          MB_DATA_TPrintf("[%3d]",monsNo);
        }
        MB_DATA_TPrintf("\n");
      }
    }
    //アイテムデータの解析
    if( dataWork->pItemData != NULL )
    {
      int i;
      GS_MYITEM *myItem = (GS_MYITEM*)dataWork->pItemData;
      MB_DATA_TPrintf("ItemData Analyze...\n");

      MB_DATA_TPrintf("NormalItem\n");
      for(i=0;i<GS_BAG_NORMAL_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyNormalItem[i].id,myItem->MyNormalItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nEventItem\n");
      for(i=0;i<GS_BAG_EVENT_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyEventItem[i].id,myItem->MyEventItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nSkillItem\n");
      for(i=0;i<GS_BAG_WAZA_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MySkillItem[i].id,myItem->MySkillItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nSealItem\n");
      for(i=0;i<GS_BAG_SEAL_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MySealItem[i].id,myItem->MySealItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nDrugItem\n");
      for(i=0;i<GS_BAG_DRUG_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyDrugItem[i].id,myItem->MyDrugItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nNutsItem\n");
      for(i=0;i<GS_BAG_NUTS_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyNutsItem[i].id,myItem->MyNutsItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nBallItem\n");
      for(i=0;i<GS_BAG_BALL_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyBallItem[i].id,myItem->MyBallItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }

      MB_DATA_TPrintf("\nBattleItem\n");
      for(i=0;i<GS_BAG_BATTLE_ITEM_MAX;i++)
      {
        MB_DATA_TPrintf("[%3d:%3d]",myItem->MyBattleItem[i].id,myItem->MyBattleItem[i].no);
        if( (i%8)==7 ){MB_DATA_TPrintf("\n");}
      }
      
    }
#endif
    dataWork->subSeq++;
    break;
  case 5:
    dataWork->subSeq = 0;
    return TRUE;
    break;
  }
  return FALSE;
}

#define DLPLAY_CHECK_SAVE_TICK 0
#if DLPLAY_CHECK_SAVE_TICK
static OSTick dlplaySaveTick;
#endif

//セーブは１つの関数で、セーブ→同期待ち→ラストセーブを行う。
BOOL  MB_DATA_GS_SaveData( MB_DATA_WORK *dataWork )
{
  switch( dataWork->subSeq )
  {
  case 0: //データの加工
    {
      u8 i,bi;
      GS_BOX_DATA *pBox = (GS_BOX_DATA*)dataWork->pBoxData;
      {
        s32 lockID = OS_GetLockID();
        GF_ASSERT_HEAVY( lockID != OS_LOCK_ID_ERROR );
        CARD_LockRom( (u16)lockID );
        CARD_CheckPulledOut();  //抜き検出
        CARD_UnlockRom( (u16)lockID );
        OS_ReleaseLockID( (u16)lockID );
      }
      //ボックス使用ビットを立てる
      pBox->UseBoxBits = GS_BOX_TRAY_ALL_USE_BIT;
#if 0      
      //ポケモンデータの暗号化
      for( bi=0;bi<BOX_MAX_TRAY;bi++)
      {
        for( i=0;i<BOX_MAX_POS;i++ )
        {
          GS_POKEMON_PARAM *param = &pBox->ppp[bi][i];
          u16 sum;
          sum = GS_PokeParaCheckSum( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 );
          GS_CalcTool_Coded( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 , param->checksum );
        }
      }
      
      //ポケモンを消す(消した後に暗号化もするので、暗号化の後
      //正しい削除処理
      GF_ASSERT_HEAVY( dataWork->selectBoxNumber_ != SELECT_BOX_INVALID );
      for( i=0;i<BOX_MAX_POS;i++ )
      {
        GS_POKEMON_PARAM *param = &pBox->ppp[dataWork->selectBoxNumber_][i];
        PokePasoParaInit( param );
      }
#endif
      //個々のデータのCRC
      {
        MB_DATA_GS_SetDataCRC( dataWork , dataWork->pItemData , DLPLAY_GS_SAVESIZETABLE[GS_GMDATA_ID_TEMOTI_ITEM] );
        MB_DATA_GS_SetDataCRC( dataWork , dataWork->pMysteryData , DLPLAY_GS_SAVESIZETABLE[GS_GMDATA_ID_FUSHIGIDATA] );
      }
      //footerの加工
      //BOX
      {
        GS_SAVE_FOOTER *boxFooter,*mainFooter;
        if( dataWork->boxLoadPos == DDS_FIRST ){
          boxFooter = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER,dataWork->cardType) ];
        }
        else{
          boxFooter = (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER,dataWork->cardType) ];
        }

        if( dataWork->mainLoadPos == DDS_FIRST ){
          mainFooter = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER,dataWork->cardType) ];
          mainFooter->crc = MATH_CalcCRC16CCITT( &dataWork->crcTable_, dataWork->pData, mainFooter->size - sizeof(GS_SAVE_FOOTER) );
        }
        else{
          mainFooter = (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER,dataWork->cardType) ];
          mainFooter->crc = MATH_CalcCRC16CCITT( &dataWork->crcTable_, dataWork->pDataMirror, mainFooter->size - sizeof(GS_SAVE_FOOTER) );
        }

        boxFooter->crc = MATH_CalcCRC16CCITT( &dataWork->crcTable_, dataWork->pBoxData, boxFooter->size - sizeof(GS_SAVE_FOOTER) );
        if( boxFooter->g_count != mainFooter->g_count )
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
        }
        //GF_ASSERT_HEAVY( boxFooter->g_count == mainFooter->g_count );
        boxFooter->g_count++;
        mainFooter->g_count++;
        //boxFooter->b_count++;
        //mainFooter->b_count++;
      }
      
      //カードのロック
      dataWork->lockID_ = OS_GetLockID();
      if( dataWork->lockID_ == OS_LOCK_ID_ERROR )
      {
        SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
      }
      //GF_ASSERT_HEAVY( dataWork->lockID_ != OS_LOCK_ID_ERROR );
      CARD_LockBackup( (u16)dataWork->lockID_ );
      dataWork->isLockID = TRUE;
      dataWork->subSeq++;
      
    }
    break;

  case 1: //進行データのセーブ
    {
      const u32 saveAddress = ( dataWork->mainSavePos == DDS_FIRST ? 0x00000 : 0x40000 );
      const u32 saveSize = MB_DATA_GetStartAddress( GS_GMDATA_NORMAL_FOOTER+1 , dataWork->cardType );
      void *pData;
      if( dataWork->mainLoadPos == DDS_FIRST ){
        pData = dataWork->pData;
      }
      else{
        pData = dataWork->pDataMirror;
      }
      //セーブ開始！

      MB_DATA_TPrintf("[[%d]]\n",saveSize);
      CARD_WriteAndVerifyFlashAsync( saveAddress , pData , saveSize , NULL , NULL );
      
      //DLPlayFunc_PutString("saveMain......",dataWork->msgSys );
      dataWork->subSeq++;
#if DLPLAY_CHECK_SAVE_TICK
      dlplaySaveTick = OS_GetTick();
#endif
    }

    break;

  case 2: //BOXデータのCRCセットからセーブ(footerとか一緒に使うから
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      u32 saveAddress;
      const u32 saveSize = MB_DATA_GetBoxDataSize( dataWork->cardType )+sizeof( GS_SAVE_FOOTER );
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        if( CARD_GetResultCode() == CARD_RESULT_NO_RESPONSE )
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_WRITE);
        }
        else
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
        }
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );

      //セーブ開始！
      saveAddress = ( dataWork->boxSavePos == DDS_FIRST ? 0x00000 : 0x40000 );
      saveAddress += MB_DATA_GetStartAddress(GS_GMDATA_ID_BOXDATA,dataWork->cardType);

      CARD_WriteAndVerifyFlashAsync( saveAddress , dataWork->pBoxData , saveSize-1 , NULL , NULL );
      
      MB_DATA_TPrintf("[[%d]]\n",saveSize);
      //DLPlayFunc_PutString("saveBox......",dataWork->msgSys);
      dataWork->subSeq++;
#if DLPLAY_CHECK_SAVE_TICK
      {
        const OSTick nowTick = OS_GetTick();
        MB_DATA_TPrintf("-SAVE TICK CHECK- SAVE_MAIN[%d]\n",OS_TicksToMilliSeconds( nowTick - dlplaySaveTick ));
      }
#endif

    }
    break;
  case 3: //書き込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        if( CARD_GetResultCode() == CARD_RESULT_NO_RESPONSE )
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_WRITE);
        }
        else
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
        }
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
      //DLPlayFunc_PutString("Save complete.",dataWork->msgSys );
      //ここでLockは保持したまま
//      CARD_UnlockBackup( (u16)dataWork->lockID_ );
//      OS_ReleaseLockID( (u16)dataWork->lockID_ );
      dataWork->isFinishSaveFirst = TRUE;
      dataWork->subSeq++;
#if DLPLAY_CHECK_SAVE_TICK
      {
        const OSTick nowTick = OS_GetTick();
        MB_DATA_TPrintf("-SAVE TICK CHECK- SAVE_WAIT[%d]\n",OS_TicksToMilliSeconds( nowTick - dlplaySaveTick ));
      }
#endif
    }
    break;
  case 4: //最後の同期を待つ
    if( dataWork->permitLastSaveFirst == TRUE )
    {
      dataWork->subSeq++;
    }
    break;
  case 5: //最後のセーブ
    {
      u32 saveAddress;
      const u32 saveSize = MB_DATA_GetBoxDataSize( dataWork->cardType )+sizeof( GS_SAVE_FOOTER );
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        if( CARD_GetResultCode() == CARD_RESULT_NO_RESPONSE )
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_WRITE);
        }
        else
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
        }
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );

      //セーブ開始！
      saveAddress = ( dataWork->boxSavePos == DDS_FIRST ? 0x00000 : 0x40000 );
      saveAddress += MB_DATA_GetStartAddress(GS_GMDATA_ID_BOXDATA,dataWork->cardType);
      saveAddress += saveSize-1;

      CARD_WriteAndVerifyFlashAsync( saveAddress , dataWork->pBoxData+saveSize-1 , 1 , NULL , NULL );
      
      //DLPlayFunc_PutString("saveBox......",dataWork->msgSys );
      dataWork->subSeq++;
#if DLPLAY_CHECK_SAVE_TICK
      {
        const OSTick nowTick = OS_GetTick();
        MB_DATA_TPrintf("-SAVE TICK CHECK- SAVE_LAST[%d]\n",OS_TicksToMilliSeconds( nowTick - dlplaySaveTick ));
      }
#endif
    }
    break;

  case 6: //最終確認
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      GS_SAVE_FOOTER *boxFooter,*mainFooter;
      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        if( CARD_GetResultCode() == CARD_RESULT_NO_RESPONSE )
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_WRITE);
        }
        else
        {
          SaveErrorCall_Save(GFL_SAVEERROR_DISABLE_READ);
        }
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
      //DLPlayFunc_PutString("Save complete!!.",dataWork->msgSys );
      //ここでLockは保持したまま
      CARD_UnlockBackup( (u16)dataWork->lockID_ );
      OS_ReleaseLockID( (u16)dataWork->lockID_ );
      dataWork->isLockID = FALSE;
      dataWork->subSeq++;
      dataWork->isFinishSaveSecond = TRUE;  //2番目は無い
      dataWork->isFinishSaveAll_ = TRUE;

      //セーブ位置の入れ替えとCRCの更新
      if( dataWork->boxLoadPos == DDS_FIRST )
      {
        boxFooter = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER,dataWork->cardType) ];
      }
      else
      {
        boxFooter = (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_BOX_FOOTER,dataWork->cardType) ];
      }
      if( dataWork->mainLoadPos == DDS_FIRST )
      {
        mainFooter = (GS_SAVE_FOOTER*)&dataWork->pData[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER,dataWork->cardType) ];
      }
      else
      {
        mainFooter = (GS_SAVE_FOOTER*)&dataWork->pDataMirror[ MB_DATA_GetStartAddress(GS_GMDATA_NORMAL_FOOTER,dataWork->cardType) ];
      }

      if( dataWork->mainSavePos == DDS_FIRST )
      {
        dataWork->cardCrcTable[GS_MAIN_FIRST] = mainFooter->crc;
        dataWork->mainSavePos = DDS_SECOND;
      }
      else
      {
        dataWork->cardCrcTable[GS_MAIN_SECOND] = mainFooter->crc;
        dataWork->mainSavePos = DDS_FIRST;
      }
      if( dataWork->boxSavePos == DDS_FIRST )
      {
        dataWork->cardCrcTable[GS_BOX_FIRST] = boxFooter->crc;
        dataWork->boxSavePos = DDS_SECOND;
      }
      else
      {
        dataWork->cardCrcTable[GS_BOX_SECOND] = boxFooter->crc;
        dataWork->boxSavePos = DDS_FIRST;
      }
    }
    break;
  }
  return FALSE;
}

void MB_DATA_GS_SetDataCRC( MB_DATA_WORK *dataWork , void *data , const u32 size )
{
  u32 *crc = (u32*)((u32)data + size-4 );
  *crc = MATH_CalcCRC16CCITT( &dataWork->crcTable_ , data , size-4 );
}

//ROMのCRCチェック
BOOL MB_DATA_GS_LoadRomCRC( MB_DATA_WORK *dataWork )
{
  switch( dataWork->subSeq )
  {
  case 0: //初期化およびデータ読み込み
    {
      const u32 footerAdr = MB_DATA_GetStartAddress( GS_GMDATA_NORMAL_FOOTER , dataWork->cardType );
      dataWork->pDataCrcCheck = GFL_HEAP_AllocClearMemory( dataWork->heapId , sizeof(GS_SAVE_FOOTER) );

      dataWork->lockID_ = OS_GetLockID();
      CARD_LockBackup( (u16)dataWork->lockID_ );

      CARD_ReadFlashAsync( footerAdr , dataWork->pDataCrcCheck , sizeof(GS_SAVE_FOOTER) , NULL , NULL );
      dataWork->subSeq++;
    }
    break;
  case 1: 
    //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      const u32 footerAdr = MB_DATA_GetStartAddress( GS_GMDATA_BOX_FOOTER , dataWork->cardType );
      GS_SAVE_FOOTER *footer = dataWork->pDataCrcCheck;
      dataWork->loadCrcTable[GS_MAIN_FIRST] = footer->crc;
      
      //次は表box
      CARD_ReadFlashAsync( footerAdr , dataWork->pDataCrcCheck , sizeof(GS_SAVE_FOOTER) , NULL , NULL );
      dataWork->subSeq++;
      
    }
    break;
  case 2: 
    //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      const u32 footerAdr = MB_DATA_GetStartAddress( GS_GMDATA_NORMAL_FOOTER , dataWork->cardType );
      GS_SAVE_FOOTER *footer = dataWork->pDataCrcCheck;
      dataWork->loadCrcTable[GS_BOX_FIRST] = footer->crc;
      
      //次は裏通常
      CARD_ReadFlashAsync( footerAdr+0x40000 , dataWork->pDataCrcCheck , sizeof(GS_SAVE_FOOTER) , NULL , NULL );
      dataWork->subSeq++;
      
    }
    break;
  case 3: 
    //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      const u32 footerAdr = MB_DATA_GetStartAddress( GS_GMDATA_BOX_FOOTER , dataWork->cardType );
      GS_SAVE_FOOTER *footer = dataWork->pDataCrcCheck;
      dataWork->loadCrcTable[GS_MAIN_SECOND] = footer->crc;
      
      //次は裏Box
      CARD_ReadFlashAsync( footerAdr+0x40000 , dataWork->pDataCrcCheck , sizeof(GS_SAVE_FOOTER) , NULL , NULL );
      dataWork->subSeq++;
    }
    break;
  case 4: 
    //読み込みの完了を待つ
    if( CARD_TryWaitBackupAsync() == TRUE )
    {
      GS_SAVE_FOOTER *footer = dataWork->pDataCrcCheck;
      dataWork->loadCrcTable[GS_BOX_SECOND] = footer->crc;

      if( CARD_GetResultCode() != CARD_RESULT_SUCCESS )
      {
        SaveErrorCall_Load();
      }
      //GF_ASSERT_HEAVY( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
      CARD_UnlockBackup( (u16)dataWork->lockID_ );
      OS_ReleaseLockID( (u16)dataWork->lockID_ );

      GFL_HEAP_FreeMemory( dataWork->pDataCrcCheck );
      dataWork->pDataCrcCheck = NULL;

      MB_DATA_TPrintf("LOAD CRC[%5d][%5d][%5d][%5d]\n",dataWork->loadCrcTable[0],dataWork->loadCrcTable[1],dataWork->loadCrcTable[2],dataWork->loadCrcTable[3]);
      MB_DATA_TPrintf("CARD CRC[%5d][%5d][%5d][%5d]\n",dataWork->cardCrcTable[0],dataWork->cardCrcTable[1],dataWork->cardCrcTable[2],dataWork->cardCrcTable[3]);
      dataWork->subSeq++;
    }
    break;
  case 5:
    dataWork->subSeq = 0;
    return TRUE;
    break;
  }
  return FALSE;
}

//データの関連性、整合性をチェックする
static  BOOL MB_DATA_GS_CheckDataCorrect( GS_SAVE_FOOTER **pFooterArr , MB_DATA_WORK *dataWork )
{
  u8 i;
  u8  mainDataNum,boxDataNum;
  BOOL isCorrect[4];  //データ成否チェック
  //まずデータ単体のチェック
  for( i=0;i<4;i++ )
  {
    const u32 mainEndAdd = MB_DATA_GetStartAddress( GS_GMDATA_NORMAL_FOOTER+1 , dataWork->cardType );
    const u32 boxStartAdd = MB_DATA_GetStartAddress( GS_GMDATA_ID_BOXDATA , dataWork->cardType );
    const u32 boxEndAdd = MB_DATA_GetStartAddress( GS_GMDATA_BOX_FOOTER+1 , dataWork->cardType );
    const u32 saveSize = ( i%2==0 ? mainEndAdd : boxEndAdd - boxStartAdd );
    const u32 addOfs = ( i%2==0 ? 0 : boxStartAdd );
    //データフッタから整合性をチェック
    //マジックナンバー
    if( pFooterArr[i]->magic_number == -1 ){
      MB_DATA_TPrintf("Data[%d] is blank\n");
      isCorrect[i] = FALSE;
      continue;
    }
    else if( pFooterArr[i]->magic_number != MAGIC_NUMBER_PT &&
             pFooterArr[i]->magic_number != MAGIC_NUMBER_KR ){
      MB_DATA_TPrintf("Data[%d] is invalid MAGIC_NUMBER[%x]\n",i,pFooterArr[i]->magic_number);
      isCorrect[i] = FALSE;
      continue;
    }
    //ブロックID(MAINは0 BOXは1
    if( pFooterArr[i]->blk_id != ( i%2==0 ? 0 : 1 ) )
    {
      MB_DATA_TPrintf(" BlockID is NG!!\n");
      isCorrect[i] = FALSE;
      continue;
    }

    //サイズのチェック
    if( pFooterArr[i]->size != saveSize )
    {
      MB_DATA_TPrintf(" Datasize is NG!!\n");
      isCorrect[i] = FALSE;
      continue;
    }
    //CRCチェック
    {
      //サイズが正しいのを確認してからやらないとマズイ
      const u16 crc = MATH_CalcCRC16CCITT( &dataWork->crcTable_ 
          , ( i<2 ? dataWork->pData+addOfs : dataWork->pDataMirror+addOfs )
          , pFooterArr[i]->size - sizeof(GS_SAVE_FOOTER) );
      MB_DATA_TPrintf("Data[%d] crc[%d]:[%d]\n",i,pFooterArr[i]->crc,crc);
      if( pFooterArr[i]->crc != crc ){
        MB_DATA_TPrintf(" crc check is NG!!\n");
        isCorrect[i] = FALSE;
        continue;
      }
    }

    isCorrect[i] = TRUE;
  }
  mainDataNum = MB_DATA_GS_CompareFooterData( pFooterArr[ GS_MAIN_FIRST ] ,isCorrect[ GS_MAIN_FIRST ],
                           pFooterArr[ GS_MAIN_SECOND ] ,isCorrect[ GS_MAIN_SECOND ] ,
                           &dataWork->mainLoadPos );
  boxDataNum = MB_DATA_GS_CompareFooterData(  pFooterArr[ GS_BOX_FIRST ] ,isCorrect[ GS_BOX_FIRST ],
                           pFooterArr[ GS_BOX_SECOND ] ,isCorrect[ GS_BOX_SECOND ] ,
                           &dataWork->boxLoadPos );

  if( mainDataNum == 0 || boxDataNum == 0 )
  {
    return FALSE;
  }
  else if( mainDataNum == 1 && boxDataNum == 1 )
  {
    if( dataWork->mainLoadPos == dataWork->boxLoadPos )
    {
      //初回セーブ
      return TRUE;
    }
    else
    {
      //片側破壊
      return FALSE;
    }
  }
  else if( mainDataNum == 1 && boxDataNum == 2 )
  {
    //進行データ破壊
    return FALSE;
  }
  else if( mainDataNum == 2 && boxDataNum == 1 )
  {
    if( pFooterArr[ dataWork->mainLoadPos*2 ]->g_count ==
      pFooterArr[ dataWork->boxLoadPos*2+1]->g_count )
    {
      //初回セーブ後、全体セーブなし
      return TRUE;
    }
    else
    {
      //BOXデータ破壊
      return FALSE;
    }
  }
  else
  {
    if( pFooterArr[ dataWork->mainLoadPos*2 ]->g_count ==
      pFooterArr[ dataWork->boxLoadPos*2+1]->g_count )
    {
      //問題なし
      return TRUE;
    }
    else
    {
      //進行とBOXの間でセーブ中断
      return FALSE;
    }
  }
  return TRUE;
}

//データの新しいほうと、正しい個数を取得
static u8 MB_DATA_GS_CompareFooterData( GS_SAVE_FOOTER *fData , BOOL fCorr ,
                       GS_SAVE_FOOTER *sData , BOOL sCorr ,
                       u8 *pos )
{
  if( fCorr == TRUE && sCorr == TRUE )
  {
    //両方正しい
    if( fData->g_count != sData->g_count )
    {
      //グローバルが違うので全体セーブ後
      if( fData->g_count > sData->g_count )
      {
        GF_ASSERT_HEAVY( fData->g_count > sData->g_count );
        *pos = DDS_FIRST;
      }
      else if( fData->g_count < sData->g_count )
      {
        GF_ASSERT_HEAVY( fData->g_count < sData->g_count );
        *pos = DDS_SECOND;
      }
      else
      {
        //ありえないエラー(一応2Mフラッシュがあるらしい・・・
        GF_ASSERT_HEAVY( FALSE );
        *pos = DDS_FIRST;
        return 0;
      }
    }
    else
    {
      //グローバルが一緒な時は部分セーブ
      if( fData->g_count > sData->g_count )
      {
        *pos = DDS_FIRST;
      }
      else if( fData->g_count < sData->g_count )
      {
        *pos = DDS_SECOND;
      }
      else
      {
        //ありえないエラー(一応2Mフラッシュがあるらしい・・・
        GF_ASSERT_HEAVY( FALSE );
        *pos = DDS_FIRST;
        return 0;
      }
    }
    return 2;
  }
  else
  if( fCorr == TRUE && sCorr == FALSE )
  {
    //片方だけ生きてる
    *pos = DDS_FIRST;
    return 1;
  }
  else if( fCorr == FALSE && sCorr == TRUE )
  {
    //片方だけ生きてる
    *pos = DDS_SECOND;
    return 1;
  }
  else
  {
    //両方アウト
    *pos = DDS_ERROR;
    return 0;
  }
}

//各データブロックの開始位置を求める。フッタも一つのブロックとして計算する
u32   MB_DATA_GS_GetStartAddress( const GS_GMDATA_ID id )
{
  u32 temp = 0;
  u8 i;
  for( i=0;i<id;i++ ){
    temp += DLPLAY_GS_SAVESIZETABLE[i];
  }
  OS_TPrintf("[%d][%x]\n",id,temp);
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//  各種ROMごとの数値取得関数
//////////////////////////////////////////////////////////////////////////////////////////////

void* MB_DATA_GS_GetBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx )
{
  GS_BOX_DATA *boxData = (GS_BOX_DATA*)dataWork->pBoxData;
  GF_ASSERT_HEAVY( dataWork->pBoxData != NULL );
  
  return &boxData->btd[tray].ppp[idx];

}

u16* MB_DATA_GS_GetBoxName( MB_DATA_WORK *dataWork , const u8 tray )
{
  GS_BOX_DATA *boxData = (GS_BOX_DATA*)dataWork->pBoxData;
  GF_ASSERT_HEAVY( dataWork->pBoxData != NULL );

  return boxData->trayName[tray];
}

void  MB_DATA_GS_ClearBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx )
{
  GS_BOX_DATA *boxData = (GS_BOX_DATA*)dataWork->pBoxData;
  GF_ASSERT_HEAVY( dataWork->pBoxData != NULL );
  
  PPP_Clear( (POKEMON_PASO_PARAM*)&boxData->btd[tray].ppp[idx] );
}


void  MB_DATA_GS_AddItem( MB_DATA_WORK *dataWork , u16 itemNo )
{
  GS_MYITEM *myItem = (GS_MYITEM*)dataWork->pItemData;
  GF_ASSERT_HEAVY( dataWork->pItemData != NULL );
  
  switch( gsItemPocketArr[itemNo] )
  {
  case MB_ITEM_POCKET_NONE:
    GF_ASSERT_MSG_HEAVY(0,"ItemTypeInvalid!\n");
    break;
  case MB_ITEM_POCKET_NORMAL:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyNormalItem , GS_BAG_NORMAL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_EVENT:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyEventItem , GS_BAG_EVENT_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_WAZA:
    {
      const BOOL ret = MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MySkillItem , GS_BAG_WAZA_ITEM_MAX );
      if( ret == TRUE )
      {
        MB_DATA_GS_SortItem( dataWork , myItem->MySkillItem , GS_BAG_WAZA_ITEM_MAX );
      }
    }
    break;
  case MB_ITEM_POCKET_SEAL:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MySealItem , GS_BAG_SEAL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_DRUG:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyDrugItem , GS_BAG_DRUG_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_NUTS:
    {
      const BOOL ret = MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyNutsItem , GS_BAG_NUTS_ITEM_MAX );
      if( ret == TRUE )
      {
        MB_DATA_GS_SortItem( dataWork , myItem->MyNutsItem , GS_BAG_NUTS_ITEM_MAX );
      }
    }
    break;
  case MB_ITEM_POCKET_BALL:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyBallItem , GS_BAG_BALL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_BATTLE:
    MB_DATA_GS_AddItemFunc( dataWork , itemNo , myItem->MyBattleItem , GS_BAG_BATTLE_ITEM_MAX );
    break;
  }
  
  
}

static const BOOL MB_DATA_GS_AddItemFunc( MB_DATA_WORK *dataWork , u16 itemNo , GS_MINEITEM *mineItem , const u16 arrMax )
{
  u16 i;
  u16 emptyIdx = 0xFFFF;
  MB_DATA_TPrintf("AddItem[%3d(%d)] ",itemNo,gsItemPocketArr[itemNo]);
  for( i=0;i<arrMax;i++ )
  {
    if( mineItem[i].id == itemNo )
    {
      MB_DATA_TPrintf("Add[%3d:%3d]->",emptyIdx,mineItem[i].no);
      if( mineItem[i].no < 999 )
      {
        mineItem[i].no++;
      }
      MB_DATA_TPrintf("[%3d:%3d]\n",emptyIdx,mineItem[i].no);
      return FALSE;
    }
    else
    if( mineItem[i].id == 0 &&
        emptyIdx == 0xFFFF )
    {
      emptyIdx = i;
    }
  }
  if( emptyIdx != 0xFFFF )
  {
    mineItem[emptyIdx].id = itemNo;
    mineItem[emptyIdx].no = 1;
    MB_DATA_TPrintf("New[%3d:%3d]\n",emptyIdx,mineItem[emptyIdx].no);
    return TRUE;
  }
  return FALSE;
}


static void MB_DATA_GS_SortItem( MB_DATA_WORK *dataWork , GS_MINEITEM *mineItem , const u16 arrMax )
{
  u32 i, j;

  for( i=0; i<arrMax-1; i++ )
  {
    for( j=i+1; j<arrMax; j++ )
    {
      if( mineItem[i].no == 0 || ( mineItem[j].no != 0 && mineItem[i].id > mineItem[j].id ) )
      {
        const u16 tempId = mineItem[i].id;
        const u16 tempNo = mineItem[i].no;
        mineItem[i].id = mineItem[j].id;
        mineItem[i].no = mineItem[j].no;
        mineItem[j].id = tempId;
        mineItem[j].no = tempNo;
      }
    }
  }
}

const u16 MB_DATA_GS_GetItemNum( MB_DATA_WORK *dataWork , const u16 itemNo )
{
  GS_MYITEM *myItem = (GS_MYITEM*)dataWork->pItemData;
  GF_ASSERT_HEAVY( dataWork->pItemData != NULL );
  
  switch( gsItemPocketArr[itemNo] )
  {
  case MB_ITEM_POCKET_NONE:
    GF_ASSERT_MSG_HEAVY(0,"ItemTypeInvalid!\n");
    break;
  case MB_ITEM_POCKET_NORMAL:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyNormalItem , GS_BAG_NORMAL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_EVENT:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyEventItem , GS_BAG_EVENT_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_WAZA:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MySkillItem , GS_BAG_WAZA_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_SEAL:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MySealItem , GS_BAG_SEAL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_DRUG:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyDrugItem , GS_BAG_DRUG_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_NUTS:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyNutsItem , GS_BAG_NUTS_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_BALL:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyBallItem , GS_BAG_BALL_ITEM_MAX );
    break;
  case MB_ITEM_POCKET_BATTLE:
    return MB_DATA_GS_GetItemNumFunc( dataWork , itemNo , myItem->MyBattleItem , GS_BAG_BATTLE_ITEM_MAX );
    break;
  }
  return 0;
}

static const u16 MB_DATA_GS_GetItemNumFunc( MB_DATA_WORK *dataWork , u16 itemNo , GS_MINEITEM *mineItem , const u16 arrMax )
{
  u16 i;
  for( i=0;i<arrMax;i++ )
  {
    if( mineItem[i].id == itemNo )
    {
      return mineItem[i].no;
    }
  }
  return 0;
}

const BOOL MB_DATA_GS_CheckLockCapsule( MB_DATA_WORK *dataWork )
{
  GS_FUSHIGI_DATA *mystery = (GS_FUSHIGI_DATA*)dataWork->pMysteryData;
  GS_GIFT_CARD *data = &mystery->rockcapcard;
  GF_ASSERT_HEAVY( dataWork->pMysteryData != NULL );
  
  MB_DATA_TPrintf("-----MysteryData-----\n");
  MB_DATA_TPrintf("gift_type [%d]\n",data->gift_type);
  MB_DATA_TPrintf("itemNo    [%d]\n",data->data.item.itemNo);
  MB_DATA_TPrintf("movieflag [%d]\n",data->data.item.movieflag);
  MB_DATA_TPrintf("-----MysteryData-----\n");
  
  if( data->gift_type == GS_MYSTERYGIFT_TYPE_ITEM &&
      data->data.item.itemNo == ITEM_ROKKUKAPUSERU &&
      data->re_dealed_count == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

void MB_DATA_GS_RemoveLockCapsule( MB_DATA_WORK *dataWork )
{
  GS_FUSHIGI_DATA *mystery = (GS_FUSHIGI_DATA*)dataWork->pMysteryData;
  GS_GIFT_CARD *data = &mystery->rockcapcard;
  GF_ASSERT_HEAVY( dataWork->pMysteryData != NULL );
  
  //本来の用途と違うが、これを設定する
  data->re_dealed_count = 1;
}
