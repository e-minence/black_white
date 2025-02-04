/**
 *  @file mail.h
 *  @brief  メールデータ定義
 *          このヘッダにはMAILDATAにアクセスするための
 *          処理だけを追加してください。セーブデータなどは
 *          mail_util.hで。
 *
 *  @date 09.11.16
 *  @author Miyuki Iwasawa
 *
 *  ＊データ実体を定義する箇所以外で参照してはいけません
 *  仮想ポインタ経由でデータアクセスできれば良い箇所では
 *  savedata/mail_util.hをincludeしてください
 */

#pragma once

#include <strbuf.h>
#include "buflen.h"
#include "system/pms_data.h"
///定数定義
#include "savedata/mail_def.h"


//------------------------------------------------------------
/**
 * @brief メールデータ管理構造への不完全型ポインタ
 *
 * 中身は見えないけどポインタ経由で参照はできる
 */
//------------------------------------------------------------
typedef struct _MAIL_DATA MAIL_DATA;

//=============================================================
/**
 *  メールデータアクセス系関数
 */
//=============================================================
/**
 *  @brief  メールデータサイズ取得
 *
 *  ＊メールデータ一通のサイズ
 */
extern int MailData_GetDataWorkSize(void);

/**
 *  @brief  メールデータクリア(初期データセット)
 */
extern void MailData_Clear(MAIL_DATA* dat);

/**
 *  @brief  メールデータが有効かどうか返す
 *  @retval FALSE 無効
 *  @retval TRUE  有効
 */
extern BOOL MailData_IsEnable(MAIL_DATA* dat);

/**
 *  @brief  メールデータのワークを取得して返す
 *
 *  ＊呼び出し側が責任もって解放すること
 *  
 */
extern MAIL_DATA* MailData_CreateWork(HEAPID heapID);

/**
 *  @brief  メールデータの構造体コピー
 */
extern void MailData_Copy(MAIL_DATA* src,MAIL_DATA* dest);

/**
 *  @brief  メールデータの内容比較
 *
 * @retval  BOOL    一致していたらTRUEを返す
 */
extern BOOL MailData_Compare(MAIL_DATA* src1,MAIL_DATA* src2);

/**
 *  @brief  メールデータ　トレーナーIDアクセス
 */
extern u32  MailData_GetWriterID(const MAIL_DATA* dat);
extern void MailData_SetWriterID(MAIL_DATA* dat,u32 id);

/**
 *  @brief  メールデータ　ライター名アクセス
 */
extern STRCODE* MailData_GetWriterName(MAIL_DATA* dat);
extern void MailData_SetWriterName(MAIL_DATA* dat,STRCODE* name);

/**
 *  @brief  メールデータ　ライターの性別アクセス
 */
extern u8 MailData_GetWriterSex(const MAIL_DATA* dat);
extern void MailData_SetWriterSex(MAIL_DATA* dat,const u8 sex);

/**
 *  @brief  メールデータ　デザインNoアクセス
 */
extern u8 MailData_GetDesignNo(const MAIL_DATA* dat);
extern void MailData_SetDesignNo(MAIL_DATA* dat,const u8 design);

/**
 *  @brief  メールデータ　国コードアクセス
 */
extern u8 MailData_GetCountryCode(const MAIL_DATA* dat);
extern void MailData_SetCountryCode(MAIL_DATA* dat,const u8 code);

/**
 *  @brief  メールデータ　カセットバージョンアクセス
 */
extern u8 MailData_GetCasetteVersion(const MAIL_DATA* dat);
extern void MailData_SetCasetteVersion(MAIL_DATA* dat,const u8 version);


/**
 *  @brief  メールデータ　簡易文取得(インデックス指定版)
 */
extern PMS_DATA*  MailData_GetMsgByIndex(MAIL_DATA* dat,u8 index);
extern void MailData_SetMsgByIndex(MAIL_DATA* dat, const PMS_DATA* pms,u8 index);

/**
 *  @brief  メールデータ　簡易会話単語（簡易会話ではない）
 */
extern u16  MailData_GetPmsWord(const MAIL_DATA* dat);
extern void MailData_SetPmsWord( MAIL_DATA* dat, u16 word );
