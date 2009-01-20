#ifndef	_TRAINER_CARD_H_
#define _TRAINER_CARD_H_

#include "buflen.h"
#include "savedata/playtime.h"

#define SIGN_SIZE_X	(24)
#define SIGN_SIZE_Y	(8)

#define COUNT_LV0	(100)
#define COUNT_LV1	(140)
#define COUNT_LV2	(170)
#define COUNT_LV3	(190)
#define COUNT_LV4	(200)

#define UNION_TR_NONE	(0xff)

typedef enum{
	TR_CARD_RANK_NORMAL = 0,
	TR_CARD_RANK_BRO,
	TR_CARD_RANK_KAP,
	TR_CARD_RANK_SIL,
	TR_CARD_RANK_GOL,
	TR_CARD_RANK_BLACK,
}TR_CARD_RANK;

//extern  const PROC_DATA TrCardProcData;		// プロセス定義データ
extern GFL_PROC_RESULT TrCardSysProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardSysProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardSysProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

typedef struct TR_BADGE_DATA_tag
{
	int BadgeHold:1;			//バッジ所持フラグ
	int BadgeScruchCount:31;	//磨き具合
	
}TR_BADGE_DATA;

typedef struct TR_CARD_DATA_tag
{
	u8 Version;			//バージョン
	u8 CountryCode;		//国コード
	u8 GymReaderMask;	//ジムリーダー８人顔マスクフラグ(金銀では未参照)
	u8 CardRank;		//カードランク
	
	u8 BrushValid:1;		//バッジ磨きアプリ有効フラグ
	u8 TimeUpdate:1;		//時間更新フラグ
	u8 TrSex:1;			//性別
	u8 PokeBookFlg:1;	//図鑑所持フラグ
	u8 MySignValid:1;	//サインデータ有効/無効フラグ(金銀のみ有効)
	u8 Dummy:3;
	u8 UnionTrNo;		//ユニオントレーナーナンバー（0〜15）指定無しのときはUNION_TR_NONE(0xff)

#if 0	//DP時のソース
	u8 Dummy2[2];
#else	//金銀のソース
	u16	gs_badge;	//金銀バッジ入手フラグ(16bit) DP&PではDummyとして参照されません
#endif
	
	STRCODE TrainerName[PERSON_NAME_SIZE+EOM_SIZE];	//トレーナー名
	const PLAYTIME *PlayTime;	//プレイ時間構造体（通信時は時間更新が行われないのでNULLをセットする）
	
	u32 Money;			//おこづかい
	u32 PokeBook;		//図鑑モンスター数
	u32 Score;			//スコア
	u16 TrainerID;		//トレーナーID
	
	u16 PlayTime_h;		//プレイ時間（時）
	u16 ClearTime_h;	//クリア時間（時）
	
	u8 PlayTime_m;		//プレイ時間（分）
	u8 Start_y;			//開始年
	u8 Start_m;			//開始月
	u8 Start_d;			//開始日
	
	u8 Clear_y;			//クリア年
	u8 Clear_m;			//クリア月
	u8 Clear_d;			//クリア日
	u8 ClearTime_m;	//クリア時間（分）
	
	u32 CommNum;		//通信回数
	u32 CommBattleWin;	//対戦勝ち数
	u32 CommBattleLose;	//対戦負け数
	u32 CommTrade;		//交換回数

	//4byte*8=32byte
	TR_BADGE_DATA	BadgeData[8];	//DP&Pバッジデータ(金銀は参照しません)
	
	u8	SignRawData[SIGN_SIZE_X*SIGN_SIZE_Y*8];	//サインデータ
	u16	_xor;			// 検証用
	u16 reached;		// 到着フラグ(通信時に使用）
}TR_CARD_DATA;

typedef struct TRCARD_CALL_PARAM_tag{
	TR_CARD_DATA	TrCardData;

	const SAVE_CONTROL_WORK*	savedata;	///<セーブデータへのポインタ
	int					value;		///<リターン値
}TRCARD_CALL_PARAM;

#endif //_TRAINER_CARD_H_
