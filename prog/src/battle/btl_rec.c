//=============================================================================================
/**
 * @file  btl_rec.c
 * @brief ポケモンWB バトルシステム 録画データ生成
 * @author  taya
 *
 * @date  2009.12.06  作成
 */
//=============================================================================================


#include "btl_common.h"
#include "btl_util.h"
#include "btl_rec.h"




static inline u8 MakeRecFieldTag( BtlRecFieldType type, u8 numClient, u8 fChapter )
{
  GF_ASSERT(numClient < 16);
  GF_ASSERT(type < 8);
  GF_ASSERT(fChapter < 2);

  return ((fChapter&1) << 7) | ((type&0x07) << 4) | ((numClient)&0x0f);
}
static inline void ReadRecFieldTag( u8 tagCode, BtlRecFieldType* type, u8* numClient, u8* fChapter )
{
  *numClient = tagCode & 0x0f;
  *type = ((tagCode >> 4) & 0x07);
  *fChapter = ((tagCode>>7)&0x01);
}


static inline u8 MakeClientActionTag( u8 clientID, u8 numAction )
{
  GF_ASSERT(clientID < 8);
  GF_ASSERT(numAction < 32);

  return (clientID << 5) | (numAction);
}
static inline void ReadClientActionTag( u8 tagCode, u8* clientID, u8* numAction )
{
  *clientID  = ((tagCode>>5) & 0x07);
  *numAction = (tagCode & 0x01f);
}

static inline u8 MakeRotationTag( u8 clientID, BtlRotateDir dir )
{
  GF_ASSERT(clientID < 8);
  GF_ASSERT(dir < 32);

  return (clientID << 5) | (dir);
}
static inline void ReadRotationTag( u8 tagCode, u8* clientID, BtlRotateDir* dir )
{
  *clientID  = ((tagCode>>5) & 0x07);
  *dir = (tagCode & 0x01f);
}


/*===========================================================================================*/
/*                                                                                           */
/* クライアント用                                                                            */
/*                                                                                           */
/* 対戦時：録画データの蓄積／再生時：録画データの読み取り                                    */
/*                                                                                           */
/*===========================================================================================*/

enum {
  BTL_REC_SIZE = 4096,
};


struct _BTL_REC {
//  u16  bufSize;
  u16  writePtr;
  u8   fSizeOver;
  u8   dmy;
  const void* readData;
  u8   buf[ BTL_REC_SIZE ];
};


//=============================================================================================
/**
 * インスタンス生成
 *
 * @param   heapID
 *
 * @retval  BTL_REC*
 */
//=============================================================================================
BTL_REC* BTL_REC_Create( HEAPID heapID )
{
  BTL_REC* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_REC) );
  return wk;
}
//=============================================================================================
/**
 * インスタンス破棄
 *
 * @param   wk
 */
//=============================================================================================
void BTL_REC_Delete( BTL_REC* wk )
{
  GFL_HEAP_FreeMemory( wk );
}
//=============================================================================================
/**
 * データ書き込み
 *
 * @param   wk
 * @param   data
 * @param   size
 */
//=============================================================================================
void BTL_REC_Write( BTL_REC* wk, const void* data, u32 size )
{
  if( wk->fSizeOver == FALSE )
  {
    u32 endPtr = wk->writePtr + size;
    if( endPtr <= sizeof(wk->buf) )
    {
      GFL_STD_MemCopy( data, &wk->buf[wk->writePtr], size );
      wk->writePtr = endPtr;
    }
    else
    {
      wk->fSizeOver = TRUE;
    }
  }
}
//=============================================================================================
/**
 * 最後まで正常に書き込まれているかチェック
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_REC_IsCorrect( const BTL_REC* wk )
{
  return !(wk->fSizeOver);
}

//=============================================================================================
/**
 * 書き込みデータ取得
 *
 * @param   wk
 * @param   size
 *
 * @retval  const void*
 */
//=============================================================================================
const void* BTL_REC_GetDataPtr( const BTL_REC* wk, u32* size )
{
  *size = wk->writePtr;
  return wk->buf;
}

//=============================================================================================
/**
 * 読み込み開始
 *
 * @param   wk
 * @param   recordData
 * @param   dataSize
 */
//=============================================================================================
void BTL_RECREADER_Init( BTL_RECREADER* wk, const void* recordData, u32 dataSize )
{
  wk->recordData = recordData;
  wk->dataSize = dataSize;
  wk->readPtr = 0;
  BTL_Printf("RECREADRE Init dataSize=%d\n", wk->dataSize);
}

//=============================================================================================
/**
 * アクションデータ１件読み込み
 *
 * @param   wk
 * @param   clientID    対象クライアントID
 * @param   numAction   [out] アクションデータ数
 * @param   fChapter    [out] チャプター（区切り）コードが埋め込まれていたらTRUE
 *
 * @retval  const BTL_ACTION_PARAM*   読み込んだアクションデータ先頭
 */
//=============================================================================================
const BTL_ACTION_PARAM* BTL_RECREADER_ReadAction( BTL_RECREADER* wk, u8 clientID, u8 *numAction, u8* fChapter )
{
  BtlRecFieldType type;
  u8 numClient, readClientID, readNumAction;
  u32 i;


  BTL_N_Printf( DBGSTR_REC_ReadActStart, wk->readPtr);
  while( wk->readPtr < wk->dataSize )
  {
    ReadRecFieldTag( wk->recordData[wk->readPtr++], &type, &numClient, fChapter );
    if( (wk->readPtr >= wk->dataSize) ){ break; }
    if( type != BTL_RECFIELD_ACTION )
    {
      BTL_N_Printf( DBGSTR_REC_ReadActSkip, numClient);
      wk->readPtr += numClient;
    }
    else
    {
      const BTL_ACTION_PARAM* returnPtr = NULL;
      BTL_Printf( DBGSTR_REC_SeekClient, numClient);
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[wk->readPtr++], &readClientID, &readNumAction );
        if( (wk->readPtr >= wk->dataSize) ){ break; }
        if( readClientID != clientID )
        {
          wk->readPtr += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        }
        else
        {
          returnPtr = (const BTL_ACTION_PARAM*)(&wk->recordData[wk->readPtr]);
          GFL_STD_MemCopy( returnPtr, wk->buf, readNumAction * sizeof(BTL_ACTION_PARAM) );
          returnPtr = (const BTL_ACTION_PARAM*)(wk->buf);
          BTL_N_Printf( DBGSTR_REC_ReadActParam, wk->readPtr, returnPtr->gen.cmd, returnPtr->fight.waza);
          wk->readPtr += (sizeof(BTL_ACTION_PARAM) * readNumAction);
          *numAction = readNumAction;
        }
      }
      if( returnPtr ){
        return returnPtr;
      }
    }
    if( (wk->readPtr >= wk->dataSize) ){ break; }
  }


  GF_ASSERT_MSG(0, "不正なデータ読み取り clientID=%d, type=%d, readPtr=%d, datSize=%d",
    clientID, type, wk->readPtr, wk->dataSize);
  return NULL;
}

//=============================================================================================
/**
 * ローテーションデータ１件読み込み
 *
 * @param   wk
 * @param   clientID   対象クライアントID
 *
 * @result  BtlRotateDir  ローテーション方向
 *
 */
//=============================================================================================
BtlRotateDir BTL_RECREADER_ReadRotation( BTL_RECREADER* wk, u8 clientID )
{
  BtlRecFieldType type;
  u8 numClient, readClientID, readNumAction, fChapter;
  u32 i;

  BTL_Printf("rec seek start RP= %d\n", wk->readPtr);
  while( wk->readPtr < wk->dataSize )
  {
    ReadRecFieldTag( wk->recordData[wk->readPtr++], &type, &numClient, &fChapter );
    if( (wk->readPtr >= wk->dataSize) ){ break; }
    if( type != BTL_RECFIELD_ROTATION )
    {
      u8 readClientID, readNumAction;
      // クライアント数分、アクションパラメータスキップ
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[wk->readPtr++], &readClientID, &readNumAction );
        wk->readPtr += (readNumAction * sizeof(BTL_ACTION_PARAM));
        if( (wk->readPtr >= wk->dataSize) ){ break; }
      }
    }
    else
    {
      u8 readClientID;
      BtlRotateDir dir;
      for(i=0; i<numClient; ++i)
      {
        ReadRotationTag( wk->recordData[wk->readPtr+i], &readClientID, &dir );
        if( readClientID == clientID ){
          break;
        }
      }
      wk->readPtr += numClient;
      if( i != numClient ){
        return dir;
      }
    }
  }
  GF_ASSERT_MSG(0, "不正なデータ読み取り clientID=%d, dataType=%d\n", clientID, type);
  return BTL_ROTATEDIR_STAY;
}

//=============================================================================================
/**
 * 記録されているターン数を数える
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_RECREADER_GetTurnCount( const BTL_RECREADER* wk )
{
  u32 p = 0;
  u32 turnCount = 0;

  BtlRecFieldType type;
  u8 fChapter, numClient, readClientID, readNumAction;

  while( p < wk->dataSize )
  {
    ReadRecFieldTag( wk->recordData[p++], &type, &numClient, &fChapter );
    if( fChapter ){
      ++turnCount;
    }
    if( (p >= wk->dataSize) ){ break; }
    if( type != BTL_RECFIELD_ACTION )
    {
      p += numClient;
    }
    else
    {
      u32 i;
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[p++], &readClientID, &readNumAction );
        p += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        if( (p >= wk->dataSize) ){ break; }
      }
    }
  }
  return turnCount;
}



/*===========================================================================================*/
/*                                                                                           */
/* サーバー用                                                                                */
/*                                                                                           */
/* クライアントに送信するデータを生成するためのツール類                                      */
/*                                                                                           */
/*===========================================================================================*/

//=============================================================================================
/**
 * RECTOOL 初期化
 *
 * @param   recTool
 * @param   fChapter    チャプター（ターン先頭）フラグ
 */
//=============================================================================================
void BTL_RECTOOL_Init( BTL_RECTOOL* recTool, BOOL fChapter )
{
  GFL_STD_MemClear( recTool, sizeof(BTL_RECTOOL) );
  recTool->fChapter = fChapter;
}

//=============================================================================================
/**
 * RECTOOL アクション選択データを１クライアント分、追加
 *
 * @param   recTool
 * @param   clientID
 * @param   action
 * @param   numAction
 */
//=============================================================================================
void BTL_RECTOOL_PutSelActionData( BTL_RECTOOL* recTool, u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction )
{
  // ヘッダ記述用に 1byte 空けておく
  if( recTool->writePtr == 0 ){
    recTool->type = BTL_RECFIELD_ACTION;
    recTool->writePtr++;
  }

  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( (recTool->clientBit & (1 << clientID)) == 0 )
  {
    u32 endPtr = recTool->writePtr + ((sizeof(BTL_ACTION_PARAM) * numAction) + 1);
    if( endPtr <= sizeof(recTool->buffer) )
    {
      recTool->clientBit |= (1 << clientID);
      recTool->numClients++;

      recTool->buffer[ recTool->writePtr ] = MakeClientActionTag( clientID, numAction );
      GFL_STD_MemCopy( action, &recTool->buffer[recTool->writePtr+1], sizeof(BTL_ACTION_PARAM)*numAction );
      recTool->writePtr = endPtr;
    }
    else
    {
      GF_ASSERT(0);   // データ容量が多すぎる
      recTool->fError = 1;
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "client_%d のデータ書き込みが２度発生している", clientID);
  }
}
//=============================================================================================
/**
 * RECTOOL アクション選択データの書き込み処理を終了
 *
 * @param   recTool
 * @param   dataSize  [out] クライアントに送信するデータサイズが入る
 *
 * @retval  void*   正常終了ならクライアントに送信するデータポインタ／エラー時はNULL
 */
//=============================================================================================
void* BTL_RECTOOL_FixSelActionData( BTL_RECTOOL* recTool, u32* dataSize )
{
  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( recTool->fError == 0 )
  {
    recTool->buffer[0] = MakeRecFieldTag( recTool->type, recTool->numClients, recTool->fChapter );
    *dataSize = recTool->writePtr;
    return recTool->buffer;
  }
  return NULL;
}

//=============================================================================================
/**
 * [RECTOOL] ローテーション選択データを１クライアント分、追加
 *
 * @param   recTool
 * @param   clientID
 * @param   dir
 */
//=============================================================================================
void BTL_RECTOOL_PutRotationData( BTL_RECTOOL* recTool, u8 clientID, BtlRotateDir dir )
{
  // ヘッダ記述用に 1byte 空けておく
  if( recTool->writePtr == 0 ){
    recTool->type = BTL_RECFIELD_ROTATION;
    recTool->writePtr++;
  }

  GF_ASSERT(recTool->type == BTL_RECFIELD_ROTATION);

  if( (recTool->clientBit & (1 << clientID)) == 0 )
  {
    if( recTool->writePtr < sizeof(recTool->buffer) )
    {
      recTool->clientBit |= (1 << clientID);
      recTool->numClients++;
      recTool->buffer[ recTool->writePtr++ ] = MakeRotationTag( clientID, dir );
    }
    else
    {
      GF_ASSERT(0);   // データ容量が多すぎる
      recTool->fError = 1;
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "client_%d のデータ書き込みが２度発生している", clientID);
  }
}

//=============================================================================================
/**
 * [RECTOOL] ローテーション選択データの書き込みを終了
 *
 * @param   recTool
 * @param   dataSize  [out] クライアントに送信するデータサイズが入る
 *
 * @retval  void*   正常終了ならクライアントに送信するデータポインタ／エラー時はNULL
 */
//=============================================================================================
void* BTL_RECTOOL_FixRotationData( BTL_RECTOOL* recTool, u32* dataSize )
{
  GF_ASSERT(recTool->type == BTL_RECFIELD_ROTATION);

  if( recTool->fError == 0 )
  {
    recTool->buffer[0] = MakeRecFieldTag( recTool->type, recTool->numClients, recTool->fChapter );
    *dataSize = recTool->writePtr;
    return recTool->buffer;
  }
  return NULL;
}

