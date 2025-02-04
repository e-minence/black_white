//============================================================================================
/**
 * @file  mystatus.c
 * @brief 自分状態データアクセス用ソース
 * @author  tamada GAME FREAK inc.
 * @date  2005.10.27
 */
//============================================================================================
#include <gflib.h>
#include "savedata/mystatus.h"
#include "savedata/wifihistory.h"
#include "mystatus_local.h"
#include "system/main.h"
#include "print/str_tool.h"

#define TR_LOW_MASK   (0xffff)



//============================================================================================
//============================================================================================

//構造体と構造体のサイズ定義がずれた場合コンパイルエラー
SDK_COMPILER_ASSERT( sizeof(MYSTATUS) == MYSTATUS_SAVE_SIZE );

//============================================================================================
//
//  セーブデータシステムが依存する関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief 自分状態保持ワークのサイズ取得
 * @return  int   サイズ（バイト単位）
 */
//----------------------------------------------------------
int MyStatus_GetWorkSize(void)
{
  return sizeof(MYSTATUS);
}

//----------------------------------------------------------
/**
 * @brief 自分状態保持ワークの確保
 * @param heapID    メモリ確保をおこなうヒープ指定
 * @return  MYSTATUS  取得したワークへのポインタ
 */
//----------------------------------------------------------
MYSTATUS * MyStatus_AllocWork(u32 heapID)
{
  MYSTATUS * my;
  my = GFL_HEAP_AllocMemory(heapID, sizeof(MYSTATUS));
  MyStatus_Init(my);

  return my;
}

//----------------------------------------------------------
/**
 * @brief MYSTATUSのコピー
 * @param from  コピー元MYSTATUSへのポインタ
 * @param to    コピー先MYSTATUSへのポインタ
 */
//----------------------------------------------------------
void MyStatus_Copy(const MYSTATUS * from, MYSTATUS * to)
{
  GFL_STD_MemCopy(from, to, sizeof(MYSTATUS));
}

//============================================================================================
//
//  MYSTATUS操作のための関数
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief   自分状態データの初期化
 * @param my    自分状態保持ワークへのポインタ
 */
//----------------------------------------------------------
void MyStatus_Init(MYSTATUS * my)
{
  GFL_STD_MemClear(my, sizeof(MYSTATUS));

  // カセットの言語コードを埋め込む
  my->region_code = PM_LANG;

  // RomCode
  MyStatus_SetRomCode( my, PM_VERSION );

#ifdef PM_DEBUG
  {
    int cont = GFL_UI_KEY_GetCont();

    // デバッグで他国の人になりすませる。
    if(cont&PAD_BUTTON_L){
      MyStatus_SetRegionCode(my, LANG_JAPAN );
    }else if(cont&PAD_BUTTON_R){
      MyStatus_SetRegionCode(my, LANG_ENGLISH );
    }else if(cont&PAD_BUTTON_SELECT){
      MyStatus_SetRegionCode(my, LANG_FRANCE   );
    }else if(cont&PAD_BUTTON_START){
      MyStatus_SetRegionCode(my, LANG_ITALY  );
    }else if(cont&PAD_KEY_UP){
      MyStatus_SetRegionCode(my, LANG_GERMANY );
    }else if(cont&PAD_KEY_LEFT){
      MyStatus_SetRegionCode(my, LANG_SPAIN  );
    }
  }

  // デフォルト名を入れておく (taya)
  {
    static const STRCODE default_name[] = {
      0x30d6, 0x30e9, 0x30c3, 0x30af, 0xffff,
    };
    STRTOOL_Copy( default_name, my->name, NELEMS(my->name) );
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *  @brief  名前が０で埋められてりうかチェック(データが入っていない常態かをチェックする)
 *
 *  @param  my  Myステータスワーク
 *
 *  @retval TRUE  クリア状態（全部0）
 *  @retval FALSE 何か入っている
 */
//-----------------------------------------------------------------------------
BOOL MyStatus_CheckNameClear( const MYSTATUS * my )
{
  int i;

  for( i=0; i<PERSON_NAME_SIZE + EOM_SIZE; i++ ){
    if( my->name[ i ] != 0 ){
      return FALSE;
    }
  }

  return TRUE;
}

//----------------------------------------------------------
/**
 * @brief 自分の名前セット
 * @param my    自分状態保持ワークへのポインタ
 * @param name  名前文字列へのポインタ
 */
//----------------------------------------------------------
void MyStatus_SetMyName(MYSTATUS * my, const STRCODE * name)
{
  STRTOOL_Copy( name, my->name, PERSON_NAME_SIZE + EOM_SIZE );
}

//----------------------------------------------------------
/**
 * @brief 自分の名前セット(STRBUF版）
 * @param my    自分状態保持ワークへのポインタ
 * @param str   名前保持バッファへのポインタ
 */
//----------------------------------------------------------
void MyStatus_SetMyNameFromString(MYSTATUS * my, const STRBUF * str)
{
  GFL_STR_GetStringCode(str, my->name, PERSON_NAME_SIZE + EOM_SIZE);
}

//----------------------------------------------------------
/**
 * @brief 自分の名前取得
 * @param my    自分状態保持ワークへのポインタ
 * @return  STRCODE   名前文字列へのポインタ
 */
//----------------------------------------------------------
const STRCODE * MyStatus_GetMyName(const MYSTATUS * my)
{
  return my->name;
}

//----------------------------------------------------------
/**
 * @brief 自分の名前取得（STRCODEにコピー）
 * @param my    自分状態保持ワークへのポインタ
 * @param buf   コピーする先のSTRCODEへのポインタ（各自で内容は確保してある事が前提）
 * @return  none
 */
//----------------------------------------------------------
void MyStatus_CopyNameStrCode( const MYSTATUS * my, STRCODE *buf, int dest_length )
{
  GF_ASSERT(dest_length >= PERSON_NAME_SIZE + EOM_SIZE);
  STRTOOL_Copy(my->name, buf, dest_length);
}

//----------------------------------------------------------
/**
 * @brief 自分の名前取得（STRBUFにコピー）
 * @param my    自分状態保持ワークへのポインタ
 * @param buf   コピーする先のSTRBUFへのポインタ（各自で内容は確保してある事が前提）
 * @return  none
 */
//----------------------------------------------------------
void MyStatus_CopyNameString( const MYSTATUS * my, STRBUF *buf )
{
  GFL_STR_SetStringCode(buf, my->name);
}

//----------------------------------------------------------
/**
 * @brief 自分の名前取得（STRBUFを生成）
 * @param my    自分状態保持ワークへのポインタ
 * @param heapID  STRBUFを生成するヒープのID
 * @return  STRBUF  名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF * MyStatus_CreateNameString(const MYSTATUS * my, int heapID)
{
  STRBUF * tmpBuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heapID);
  MyStatus_CopyNameString( my, tmpBuf );
  return tmpBuf;
}


//----------------------------------------------------------
/**
 * @brief 自分のＩＤセット
 * @param my    自分状態保持ワークへのポインタ
 * @param id    ＩＤ指定
 */
//----------------------------------------------------------
void MyStatus_SetID(MYSTATUS * my, u32 id)
{
  my->id = id;
}
//----------------------------------------------------------
/**
 * @brief 自分のＩＤ取得
 * @param my    自分状態保持ワークへのポインタ
 * @return  int   自分のＩＤ
 */
//----------------------------------------------------------
u32 MyStatus_GetID(const MYSTATUS * my)
{
  return my->id;
}

//----------------------------------------------------------
/**
 * @brief 自分のＩＤ取得(下位２バイト)
 * @param my    自分状態保持ワークへのポインタ
 * @return  int   自分のＩＤ
 */
//----------------------------------------------------------
u16 MyStatus_GetID_Low(const MYSTATUS * my)
{
  return (u16)(my->id&TR_LOW_MASK);
}



//---------------------------------------------------------------------------
/**
 * @brief GTS・Wifiバトルタワー用Idの取得
 * @param sysdt   システムデータへのポインタ
 * @param profileId [out]GameSpyプロファイルIDを出力するポインタ
 * @param FriendKey [out]ともだちコードを出力するポインタ
 * @param u64     ともだちコードの値（初めて取得したFriendKeyをずっと保持）
 */
//---------------------------------------------------------------------------
s32 MyStatus_GetProfileID( const MYSTATUS* my )
{
  return my->profileID;
}

//---------------------------------------------------------------------------
/**
 * @brief GTS・Wifiバトルタワー用Idの設定(初回のみ代入できる）
 * @param sysdt   システムデータへのポインタ
 * @param none
 */
//---------------------------------------------------------------------------
void MyStatus_SetProfileID( MYSTATUS* my, s32 profileID )
{
  // GTS・Wifiバトルタワー用のID・ともだちコードが
  // 一回も登録されていなかった場合登録する
  if( my->profileID==0 ){
    // 一度登録したら、GameSpyIdが変更になっても変わらない
    my->profileID = profileID;
  }
}


//----------------------------------------------------------
/**
 * @brief 自分の性別セット
 * @param my    自分状態保持ワークへのポインタ
 * @param sex   自分の性別
 */
//----------------------------------------------------------
void MyStatus_SetMySex(MYSTATUS * my, int sex)
{
  my->sex = sex;
}
//----------------------------------------------------------
/**
 * @brief 自分の性別取得
 * @param my    自分状態保持ワークへのポインタ
 * @return  sex   自分の性別
 */
//----------------------------------------------------------
u32 MyStatus_GetMySex(const MYSTATUS * my)
{
  return my->sex;
}

//==============================================================================
/**
 * $brief   自分がトレーナーとして見られる時の番号を返す（ユニオンルーム用）
 *
 * @param   my    自分状態保持ワークへのポインタ
 *
 * @retval  u8    トレーナーとしての自分の見た目
 */
//==============================================================================
u8 MyStatus_GetTrainerView( const MYSTATUS *my )
{
  return my->trainer_view;
}

//==============================================================================
/**
 * $brief   自分がトレーナーとして見られる時の番号をセット（ユニオンルーム用）
 *
 * @param   my    自分状態保持ワークへのポインタ
 * @param   view  トレーナー見た目番号
 *
 * @retval  none
 */
//==============================================================================
void MyStatus_SetTrainerView( MYSTATUS *my, u8 view )
{
  GF_ASSERT( view<MYSTATUS_UNIONVIEW_MAX*2 ); // 男女８つずつ

  my->trainer_view = view;
}


//==============================================================================
/**
 * $brief   ＲＯＭバージョンコードを返す
 *
 * @param   my    自分状態保持ワークへのポインタ
 *
 * @retval  u8    0:ダイヤ 1:パール
 */
//==============================================================================
u8  MyStatus_GetRomCode( const MYSTATUS * my )
{
  return my->rom_code;
}

//==============================================================================
/**
 * $brief   ＲＯＭバージョンコードを設定する
 *
 * @param   my    自分状態保持ワークへのポインタ
 *
 * @param  rom_code   0:ダイヤ 1:パール
 */
//==============================================================================
void  MyStatus_SetRomCode( MYSTATUS * my, u8 rom_code )
{
  my->rom_code = rom_code;
}


//==============================================================================
/**
 * $brief   リージョンコードを返す
 *
 * @param   my    自分状態保持ワークへのポインタ
 *
 * @retval  u8    リージョンコード
 */
//==============================================================================
u8  MyStatus_GetRegionCode( const MYSTATUS * my )
{
  return my->region_code;
}

//==============================================================================
/**
 * $brief   リージョンコードを設定する
 *
 * @param   my    自分状態保持ワークへのポインタ
 *
 * @param  region_code
 */
//==============================================================================
void  MyStatus_SetRegionCode( MYSTATUS * my, u8 region_code )
{
  my->region_code = region_code;
}

//----------------------------------------------------------
/**
 * @brief 自分の国コードを取得
 * @param my      自分状態保持ワークへのポインタ
 * @return  int     国指定コード
 */
//----------------------------------------------------------
int MyStatus_GetMyNation(const MYSTATUS * my)
{
  return my->nation;
}

//----------------------------------------------------------
/**
 * @brief 自分の地域コードを取得
 * @param my      自分状態保持ワークへのポインタ
 * @return  int     地域指定コード
 */
//----------------------------------------------------------
int MyStatus_GetMyArea(const MYSTATUS * my)
{
  return my->area;
}

//----------------------------------------------------------
/**
 * @brief 自分の地域コードを設定
 * @param my      自分状態保持ワークへのポインタ
 * @param nation  
 * @param area    
 */
//----------------------------------------------------------
void MyStatus_SetMyNationArea(MYSTATUS * my, int nation, int area)
{
  GF_ASSERT(nation < WIFI_NATION_MAX);
  GF_ASSERT(area < WIFI_AREA_MAX);

  my->nation = nation;
  my->area = area;
}



//----------------------------------------------------------
/**
 * @brief myStatus同士が同じものかどうかを調べる
 * @param my    自分状態保持ワークへのポインタ
 * @param target  相手の自分状態保持ワークへのポインタ
 * @return  一致したらTRUE
 */
//----------------------------------------------------------
BOOL MyStatus_Compare(const MYSTATUS * my, const MYSTATUS * target)
{
  if(0 == GFL_STD_MemComp(my->name, target->name, PERSON_NAME_SIZE)){
        if(my->id == target->id){
            return TRUE;
        }
    }
    return FALSE;
}



// 外部参照インデックスを作る時のみ有効(ゲーム中は無効)
#ifdef CREATE_INDEX
void *Index_Get_Mystatus_Name_Offset(MYSTATUS *my){ return &my->name; }
void *Index_Get_Mystatus_Id_Offset(MYSTATUS *my){ return &my->id; }
void *Index_Get_Mystatus_Sex_Offset(MYSTATUS *my){ return &my->sex; }
#endif
