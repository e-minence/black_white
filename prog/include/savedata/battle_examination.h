//======================================================================
/**
 * @file   battle_examination.h
 * @brief  バトル検定セーブデータ
 * @author k.ohno
 * @date	 10.01.08
 */
//======================================================================


#include "savedata/bsubway_savedata.h"
#include "battle/bsubway_battle_data.h"


//typedef _BSUBWAY_PARTNER_DATA BSUBWAY_PARTNER_DATA;

#define BATTLE_EXAMINATION_MAGIC_KEY (3432)   //u16
#define BATTLE_EXAMINATION_USED_MAGIC_KEY (2010)   //u16

#define BATTLE_EXAMINATION_MAX (5)   ///< バトル検定の人数
#define BATTLE_EXAMINATION_TITLE_MSG_MAX (16+1)   ///< メッセージの数

//通信で使用するので外部公開していますが、アクセス関数を使用してください
typedef struct {
  BSUBWAY_PARTNER_DATA trainer[BATTLE_EXAMINATION_MAX];
  STRCODE titleName[BATTLE_EXAMINATION_TITLE_MSG_MAX];    //34byte
  u16 bActive;  ///< マジックキー
  u16 dataNo;  ///< データ番号  最上位BITはシングルかダブルか シングルはBITが立っていない
  u16 crc;
}BATTLE_EXAMINATION_SAVEDATA;

extern int BATTLE_EXAMINATION_SAVE_GetWorkSize(void);
extern BATTLE_EXAMINATION_SAVEDATA* BATTLE_EXAMINATION_SAVE_AllocWork(HEAPID heapID);
extern void BATTLE_EXAMINATION_SAVE_Init(BATTLE_EXAMINATION_SAVEDATA* pSV);
extern BATTLE_EXAMINATION_SAVEDATA *BATTLE_EXAMINATION_SAVE_GetSvPtr( SAVE_CONTROL_WORK * p_sv);
extern BSUBWAY_PARTNER_DATA *BATTLE_EXAMINATION_SAVE_GetData(BATTLE_EXAMINATION_SAVEDATA* pSV, const u32 inIdx);
extern void BATTLE_EXAMINATION_SAVE_Write(GAMEDATA *gamedata , BATTLE_EXAMINATION_SAVEDATA* pBattleEx, HEAPID heapID);
extern BOOL BATTLE_EXAMINATION_SAVE_IsInData(BATTLE_EXAMINATION_SAVEDATA* pSV);
extern BOOL BATTLE_EXAMINATION_SAVE_IsSingleBattle(BATTLE_EXAMINATION_SAVEDATA* pSV);
extern u16 BATTLE_EXAMINATION_SAVE_GetDataNo(BATTLE_EXAMINATION_SAVEDATA* pSV);
extern void BATTLE_EXAMINATION_SAVE_SetDataUsedKey(BATTLE_EXAMINATION_SAVEDATA* pSV);

