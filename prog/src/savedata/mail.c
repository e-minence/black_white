/**
 *  @file mail.c
 *  @brief  メールセーブデータ　制御
 *  @author Miyuki Iwasawa
 *  @date 06.02.09
 *
 *  メールセーブデータ反映手順
 *  1,MAIL_SearchNullID()で空き領域を検索、データID取得
 *  2,MailData_CreateWork()でダミーデータ作成用ワークエリアを取得
 *  3,MailData_CreateDataFromSave()などを使ってワークにデータを構築
 *  4,MAIL_AddDataFromWork()と取得済みのデータID,ワークのデータを使って、
 *  　セーブ領域にデータを反映させる
 *  5,ダミーワークエリアを開放する
 */

#include "gflib.h"
#include "savedata/save_tbl.h"
#include "gamesystem/game_data.h"

#include "savedata/mail.h"
#include "savedata/mail_util.h"
#include "savedata/mystatus.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "pokeicon/pokeicon.h"

#include "system/pms_data.h"

#include "poke_icon.naix"
#include "poke_tool/monsno_def.h"

//==============================================================================================
// 構造体宣言
//==============================================================================================

//=============================================================
///メールデータ型
// 
//  size=56byte
//
//  構造体サイズを4byte境界に合わせるため、MAIL_ICON配列要素数に+1して
//  パディングを行っています
//=============================================================
typedef struct _MAIL_DATA{
  u32 writerID; ///<トレーナーID 4                         4
  u8  sex;      ///<主人公の性別 1                         5
  u8  region;   ///<国コード 1                             6
  u8  version;  ///<カセットバージョン 1                   7
  u8  design;   ///<デザインナンバー 1                     8
  STRCODE name[PERSON_NAME_SIZE+EOM_SIZE];  ///< 名前16byte   24
  u16     padding[3];                       ///< パディング６バイト   30
  u16     pmsword;                          ///< 簡易単語 32

  PMS_DATA  msg[MAILDAT_MSGMAX];            //<文章データ 56
}_MAIL_DATA;

//_MAIL_DATA構造体のサイズに変更があったら、
//直値指定しているPOKEMON_PARAM構造体メンバにも変更がかかるので、
//チェックをする
#ifdef DEBUG_ONLY_FOR_sogabe
SDK_COMPILER_ASSERT( sizeof(_MAIL_DATA) == 56 );
#endif

//=============================================================
///メールデータセーブデータブロック定義構造体
//=============================================================
typedef struct _MAIL_BLOCK{

  MAIL_DATA paso[MAIL_STOCK_PASOCOM];   // メール最大数20個

}_MAIL_BLOCK;


///ポケモンアイコンの最大CGXID(金銀でアイコンが増えるならここも変える必要がある)
#define ICON_CGXID_MAX    (NARC_poke_icon_poke_icon_656_freeze_f_NCGR)

/**
 *  @brief  メールデータサイズ取得
 *
 *  ＊メールデータ一通のサイズ
 */
int MailData_GetDataWorkSize(void)
{
  return sizeof(MAIL_DATA);
}
/**
 *  @brief  メールデータクリア(初期データセット)
 */
void MailData_Clear(MAIL_DATA* dat)
{
  int i;
  
  dat->writerID = 0;
  dat->sex      = PM_MALE;
  dat->region   = CasetteLanguage;  
  dat->version  = CasetteVersion;   
  dat->design   = MAIL_DESIGN_NULL;

  GFL_STD_MemFill16(dat->name, GFL_STR_GetEOMCode(), sizeof(dat->name));

  dat->pmsword = PMS_WORD_NULL;
  for(i = 0;i < MAILDAT_MSGMAX;i++){
    PMSDAT_Clear(&dat->msg[i]);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
  SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  メールデータが有効かどうか返す
 *  @retval FALSE 無効
 *  @retval TRUE  有効
 */
BOOL MailData_IsEnable(MAIL_DATA* dat)
{
  if( dat->design >= MAIL_DESIGN_START &&
    dat->design <= MAIL_DESIGN_END){
    return TRUE;
  }
  return FALSE;
}

/**
 *  @brief  メールデータのワークを取得して返す
 *
 *  ＊呼び出し側が責任もって解放すること
 *  
 */
MAIL_DATA* MailData_CreateWork(HEAPID heapID)
{
  MAIL_DATA* p;

  p = GFL_HEAP_AllocMemoryLo(heapID,sizeof(MAIL_DATA)); 
  MailData_Clear(p);

  return p;
}

/**
 *  @brief  メールデータの構造体コピー
 */
void MailData_Copy(MAIL_DATA* src,MAIL_DATA* dest)
{
  MI_CpuCopy8(src,dest,sizeof(MAIL_DATA));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
  SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  メールデータの内容比較
 *
 *  @retval BOOL    一致していたらTRUEを返す
 */
BOOL MailData_Compare(MAIL_DATA* src1,MAIL_DATA* src2)
{
  int i;
  
  if( (src1->writerID != src2->writerID) ||
    (src1->sex != src2->sex) ||
    (src1->region != src2->region) ||
    (src1->version != src2->version) ||
    (src1->design != src2->design) ||
    (src1->pmsword != src2->pmsword) ){
    return FALSE;
  }
  if( GFL_STD_MemComp( src1->name,src2->name,sizeof(src1->name) ) != 0){
    return FALSE;
  }
  for(i = 0;i < MAILDAT_MSGMAX;i++){
    if(!PMSDAT_Compare( &src1->msg[i], &src2->msg[i] )){
      return FALSE;
    }
  }
  return TRUE;
}

/**
 *  @brief  デザインNo,ポケモンのポジション、セーブデータ指定してメールを新規作成状態に初期化
 *  @param  dat データを作成するMAIL_DATA構造体型へのポインタ
 *  @param  design_no メールのイメージNo
 *  @param  pos   メールを持たせるポケモンの手持ち内のポジション
 *  @param  save  セーブデータへのポインタ
 */
void MAILDATA_CreateFromSaveData(MAIL_DATA* dat,u8 design_no,u8 pos, GAMEDATA* gamedata)
{
  MYSTATUS  *my;
  
  MailData_Clear(dat);
  dat->design = design_no;

  //セーブデータから
  my    = GAMEDATA_GetMyStatus( gamedata );

  //自機の名前
  GFL_STD_MemCopy( MyStatus_GetMyName(my), dat->name, sizeof(dat->name));
  //性別
  dat->sex = (u8)MyStatus_GetMySex(my);
  //トレーナーID
  dat->writerID = MyStatus_GetID(my);

}

/**
 *  @brief  メールデータ　トレーナーID取得
 */
u32 MailData_GetWriterID(const MAIL_DATA* dat)
{
  return dat->writerID;
}
/**
 *  @brief  メールデータ　トレーナーID適用
 */
void MailData_SetWriterID(MAIL_DATA* dat,u32 id)
{
  dat->writerID = id;
}

/**
 *  @brief  メールデータ　ライター名取得
 */
STRCODE* MailData_GetWriterName(MAIL_DATA* dat)
{
  return &(dat->name[0]);
}
/**
 *  @brief  メールデータ　ライター名適用
 */
void MailData_SetWriterName(MAIL_DATA* dat,STRCODE* name)
{
  GFL_STD_MemCopy( name, dat->name, BUFLEN_PERSON_NAME);
}

/**
 *  @brief  メールデータ　ライターの性別を取得
 */
u8  MailData_GetWriterSex(const MAIL_DATA* dat)
{
  return dat->sex;
}
/**
 *  @brief  メールデータ　ライターの性別を適用
 */
void MailData_SetWriterSex(MAIL_DATA* dat,const u8 sex)
{
  dat->sex = sex;
}

/**
 *  @brief  メールデータ　デザインNo取得
 */
u8  MailData_GetDesignNo(const MAIL_DATA* dat)
{
  return dat->design;
}
/**
 *  @brief  メールデータ　デザインNo適用
 */
void MailData_SetDesignNo(MAIL_DATA* dat,const u8 design)
{
  if(design >= MAIL_DESIGN_MAX){
    return;
  }
  dat->design = design;
}

/**
 *  @brief  メールデータ　国コード取得
 */
u8  MailData_GetCountryCode(const MAIL_DATA* dat)
{
  return dat->region;
}
/**
 *  @brief  メールデータ　国コード適用
 */
void MailData_SetCountryCode(MAIL_DATA* dat,const u8 code)
{
  dat->region = code;
}

/**
 *  @brief  メールデータ　カセットバージョン取得
 */
u8  MailData_GetCasetteVersion(const MAIL_DATA* dat)
{
  return dat->version;
}
/**
 *  @brief  メールデータ　カセットバージョン適用
 */
void MailData_SetCasetteVersion(MAIL_DATA* dat,const u8 version)
{
  dat->version = version;
}


/**
 *  @brief  メールデータ　簡易単語取得（会話ではない）
 */
u16 MailData_GetPmsWord(const MAIL_DATA* dat)
{
  return dat->pmsword;
}

/**
 *  @brief  メールデータ　簡易単語セット（会話ではない）
 */
void MailData_SetPmsWord( MAIL_DATA* dat, u16 word )
{
  dat->pmsword = word;
}

/**
 *  @brief  メールデータ　簡易文取得(インデックス指定版)
 */
PMS_DATA* MailData_GetMsgByIndex(MAIL_DATA* dat,u8 index)
{
  if(index < MAILDAT_MSGMAX){
    return &(dat->msg[index]);
  }else{
    return &(dat->msg[0]);
  }
}
/**
 *  @brief  メールデータ　簡易文セット(インデックス指定版)
 */
void MailData_SetMsgByIndex(MAIL_DATA* dat, const PMS_DATA* pms,u8 index)
{
  if(index >= MAILDAT_MSGMAX){
    return;
  }
  PMSDAT_Copy(&dat->msg[index],pms);
}


//=================================================================
//
//=================================================================
//ローカル関数プロトタイプ
static int mail_GetNullData(MAIL_DATA* array,int num);
static int mail_GetNumEnable(MAIL_DATA* array,int num);
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* bloc,MAILBLOCK_ID blockID,int dataID);

//=============================================================================================
/**
 * @brief GAMEDATAからメールデータブロックを返す
 *
 * @param   gamedata    
 *
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
MAIL_BLOCK* GAMEDATA_GetMailBlock( GAMEDATA *gamedata )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  return SaveControl_DataPtrGet( sv, GMDATA_ID_MAILDATA );
  
}
/**
 *  @brief  メールセーブデータブロックサイズ取得
 *
 *  ＊メールデータ一通のサイズではないので注意！
 */
int MAIL_GetBlockWorkSize(void)
{
  return sizeof(MAIL_DATA)*MAIL_STOCK_MAX;
}
/**
 *  @brief  メールセーブデータブロック初期化
 */
void MAIL_Init(MAIL_BLOCK* dat)
{
  int i = 0;

  for(i = 0;i < MAIL_STOCK_PASOCOM;i++){
    MailData_Clear(&dat->paso[i]);
  }
}

/**
 *  @brief  空いているメールデータIDを取得
 *
 *  @param  id 追加したいメールブロックID
 *
 *  @return int データを追加できる場合は参照ID
 *        追加できない場合はマイナス値が返る
 */
int MAIL_SearchNullID(MAIL_BLOCK* block,MAILBLOCK_ID id)
{
  return mail_GetNullData(block->paso,MAIL_STOCK_PASOCOM);
}

/**
 *  @brief  メールデータを削除
 *
 *  @param  blockID ブロックのID
 *  @param  dataID  データID
 */
void MAIL_DelMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
  MAIL_DATA* pd = NULL;
  
  pd = mail_GetAddress(block,blockID,dataID);
  if(pd != NULL){
    MailData_Clear(pd);
  }
}

/**
 *  @brief  メールデータをセーブブロックに追加
 *
 *  ＊引き渡したMAIL_DATA構造体型データの中身がセーブデータに反映されるので
 *  　おかしなデータを入れないように注意！
 */
void MAIL_AddMailFormWork(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* src)
{
  MAIL_DATA* pd = NULL;
  
  pd = mail_GetAddress(block,blockID,dataID);
  if(pd != NULL){
    MailData_Copy(src,pd);
  }
}

/**
 *  @brief  指定ブロックに有効データがいくつあるか返す
 */
int MAIL_GetEnableDataNum(MAIL_BLOCK* block,MAILBLOCK_ID blockID)
{
  return mail_GetNumEnable(block->paso,MAIL_STOCK_PASOCOM);
}

/**
 *  @brief  メールデータのコピーを取得
 *
 *  ＊内部でメモリを確保するので、呼び出し側が責任持って領域を開放すること
 *  ＊無効IDを指定した場合、空データを返す
 */
MAIL_DATA* MAIL_AllocMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,HEAPID heapID)
{
  MAIL_DATA* src = NULL;
  MAIL_DATA* dest;
  
  src = mail_GetAddress(block,blockID,dataID);
  dest = MailData_CreateWork(heapID);
  if(src != NULL){
    MailData_Copy(src,dest);
  }
  return dest;
}

/**
 *  @brief  メールデータのコピーを取得
 *
 *  ＊あらかじめ確保したMAIL_DATA型メモリにセーブデータをコピーして取得
 */
void MAIL_GetMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* dest)
{
  MAIL_DATA* src = NULL;
  
  src = mail_GetAddress(block,blockID,dataID);
  if(src == NULL){
    MailData_Clear(dest);
  }else{
    MailData_Copy(src,dest);
  }
}

//=================================================================
//
//=================================================================

/**
 *  @brief  手持ちブロックの空きを探して返す
 *
 *  @param  array メールデータ配列へのポインタ
 *  @param  num   第一引数で渡した配列の要素数
 */
static int mail_GetNullData(MAIL_DATA* array,int num)
{
  int i = 0;

  for(i = 0;i < num;i++){
    if(!MailData_IsEnable(&array[i])){
      return i;
    }
  }
  return MAILDATA_NULLID;
}

/**
 *  @brief  有効データの数を探して返す
 */
static int mail_GetNumEnable(MAIL_DATA* array,int num)
{
  int i = 0;
  int ct = 0;
  
  for(i = 0;i < num;i++){
    if(MailData_IsEnable(&array[i])){
      ct++;
    }
  }
  return ct;
}

/**
 *  @brief  指定IDを持つブロック内のメールデータへのポインタを返す
 */
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
  if(dataID < MAIL_STOCK_PASOCOM){
    return &(block->paso[dataID]);
  }
  return NULL;
}
