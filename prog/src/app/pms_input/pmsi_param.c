
#include <gflib.h>

#include "system\pms_data.h"
#include "system\pms_word.h"
//#include "savedata\zukanwork.h"
#include "savedata\config.h"

//#include "field\sysflag.h"

#include "savedata/save_tbl.h"
#include "field/eventwork.h"
#include "field/eventwork_def.h"

#include "pms_input_prv.h"
//==================================================================================
//==================================================================================
enum {
	PMS_TYPE_DEFAULT = PMS_TYPE_MAIL,
};

//==================================================================================
//==================================================================================

struct _PMSI_PARAM {
	// [IN]
	u8   input_mode;

	// [OUT]
	u8  cancel_flag;			///< 「やめる」でキャンセルされたフラグ
	u8  modified_flag;			///< キャンセルされず、かつ内容が更新されたかフラグ

	// [PRIVATE]
	u8  game_clear_flag : 1;
	u8  notedit_egnore_flag : 1;	///< ONにすると編集されていなくても普通に終わることが出来る
  u8  lock_flag : 1;      ///< ONにすると文章固定になる
  u8  picture_flag : 1;   ///< ONにするとデコメ利用可能
  u8  bit_padding : 4;

  u8  padding[1];

	const ZUKAN_SAVEDATA*   zukan_savedata;
	const PMSW_SAVEDATA*    pmsw_savedata;

	PMS_DATA   pms;		// 文章モード用
	PMS_WORD   word[PMS_INPUT_WORD_MAX];	// 単語モード用

	// ボックス壁紙パスワードとしての値（単語２つモード時のみ有効）
	u16        boxpwd_id[ PMS_INPUT_WORD_MAX ];

};


//==============================================================
// Prototype
//==============================================================







//==============================================================================================
//	入力画面を呼び出す側からのパラメータ操作
//==============================================================================================


//------------------------------------------------------------------
/**
 * 簡易会話入力画面パラメータ作成 
	*
	* @param   input_mode			入力モード（enum PMSI_MODE）
	* @param   guidance_type	説明文タイプ（enum PMSI_GUIDANCE）(PMSI_GUIDANCE_DEFAULTに限る)
	* @param   savedata			  セーブデータポインタ
  * @param   pms            文章を固定したい場合はPMS_DATAを指定。NULLにすれば固定化しない。
  * @param   picture_flag   TRUE:デコメ入力可能
	* @param   heapID				  作成用ヒープID
	*
	* @retval  PMSI_PARAM*		作成されたパラメータオブジェクトへのポインタ
	*/
//------------------------------------------------------------------
PMSI_PARAM*  PMSI_PARAM_Create( u32 input_mode, u32 guidance_type,
    PMS_DATA* pms, BOOL picture_flag, SAVE_CONTROL_WORK* savedata, u32 heapID )
{ 
	PMSI_PARAM* p = GFL_HEAP_AllocClearMemory( heapID, sizeof(PMSI_PARAM));

	p->input_mode = input_mode;
	
  p->zukan_savedata = SaveControl_DataPtrGet( savedata, GMDATA_ID_ZUKAN );

	p->pmsw_savedata = SaveData_GetPMSW(savedata);

  p->game_clear_flag = EVENTWORK_CheckEventFlag( SaveData_GetEventWork(savedata), SYS_FLAG_GAME_CLEAR );
	
  p->notedit_egnore_flag = FALSE;

	p->cancel_flag = TRUE;
	p->modified_flag = FALSE;
  p->picture_flag = picture_flag;
	
	if(input_mode == PMSI_MODE_SENTENCE)
	{
    if( pms == NULL )
    {
		  PMSDAT_Init( &p->pms, PMS_TYPE_DEFAULT );
    }
    else
    {
      GFL_STD_MemCopy( pms, &p->pms, sizeof( PMS_DATA ) );
      p->lock_flag = TRUE;
      HOSAKA_Printf("PMS INPUT IS LOCK MODE !!\n");
    }
	}
	else
	{
		int i;

    GF_ASSERT( pms == NULL ); ///< 単語モードは文章固定に未対応

		for(i=0; i<PMS_INPUT_WORD_MAX; i++)
		{
			p->word[i] = PMS_WORD_NULL;
		}
	}

	return p;
}


//------------------------------------------------------------------
/**
	* 簡易会話入力画面パラメータ破棄
	*
	* @param   p		パラメータオブジェクトへのポインタ
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_Delete( PMSI_PARAM* p )
{
	GFL_HEAP_FreeMemory(p);
}



//------------------------------------------------------------------
/**
	* 簡易会話入力画面の初期状態に使用するパラメータセット（単語１つモード用）
	*
	* @param   p			パラメータオブジェクト
	* @param   word		セットする簡易会話単語コード
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_SetInitializeDataSingle( PMSI_PARAM* p, PMS_WORD word )
{
	p->word[0] = word;
}
//------------------------------------------------------------------
/**
	* 簡易会話入力画面の初期状態に使用するパラメータセット（単語２つモード用）
	*
	* @param   p			パラメータオブジェクト
	* @param   word0		セットする簡易会話単語コード１
	* @param   word1		セットする簡易会話単語コード２
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_SetInitializeDataDouble( PMSI_PARAM* p, PMS_WORD word0, PMS_WORD word1 )
{
	p->word[0] = word0;
	p->word[1] = word1;
}
//------------------------------------------------------------------
/**
	* 簡易会話入力画面の初期状態に使用するパラメータセット（文章モード用）
	*
	* @param   p		パラメータオブジェクト
	* @param   pms		セットする簡易会話データポインタ
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_SetInitializeDataSentence( PMSI_PARAM* p, const PMS_DATA* pms )
{
	p->pms = *pms;
}

//------------------------------------------------------------------
/**
	* 一度結果を受け取った後、もう１度、入力画面用パラメータとして使えるようにする
	*
	* @param   p		パラメータオブジェクト
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_SetReuseState( PMSI_PARAM* p )
{
	p->cancel_flag = TRUE;
	p->modified_flag = FALSE;
}


//------------------------------------------------------------------
/**
	* 編集しなくても普通に終わるフラグをたてる
	*
	* @param   p		
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_SetNotEditEgnore( PMSI_PARAM* p )
{
	p->notedit_egnore_flag = TRUE;
}
//------------------------------------------------------------------
/**
	* 簡易会話入力画面で、入力をキャンセルされたかどうか判定
	*
	* @param   p			パラメータオブジェクト
	*
	* @retval  BOOL		TRUEでキャンセルされた
	*/
//------------------------------------------------------------------
BOOL PMSI_PARAM_CheckCanceled( const PMSI_PARAM* p )
{
	return p->cancel_flag;
}
//------------------------------------------------------------------
/**
	* 簡易会話入力画面の終了後、初期状態から変更がなされたかチェック
	*
	* @param   p			パラメータオブジェクト
	*
	* @retval  BOOL		TRUEで変更された
	*/
//------------------------------------------------------------------
BOOL PMSI_PARAM_CheckModified( const PMSI_PARAM* p )
{
	return p->modified_flag;
}


//------------------------------------------------------------------
/**
	* 入力画面で入力されたデータを取得（単語１つモード）
	*
	* @param   p			パラメータオブジェクト
	*
	* @retval  PMS_WORD		入力データ（単語コード）
	*/
//------------------------------------------------------------------
PMS_WORD  PMSI_PARAM_GetInputDataSingle( const PMSI_PARAM* p )
{
	return p->word[0];
}

//------------------------------------------------------------------
/**
	* 入力画面で入力されたデータを取得（単語２つモード）
	*
	* @param   p		パラメータオブジェクト
	* @param   dst		入力されたデータを受け取るバッファ
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_GetInputDataDouble( const PMSI_PARAM* p,  PMS_WORD* dst )
{
	dst[0] = p->word[0];
	dst[1] = p->word[1];
}

//------------------------------------------------------------------
/**
	* 入力画面で入力されたデータを取得（文章モード）
	*
	* @param   p		パラメータオブジェクト
	* @param   pms		入力されたデータを受け取る構造体アドレス
	*
	*/
//------------------------------------------------------------------
void PMSI_PARAM_GetInputDataSentence( const PMSI_PARAM* p, PMS_DATA* pms )
{
	PMSDAT_Copy( pms, &(p->pms) );
}







//==============================================================================================
//	入力画面内部からのパラメータ操作
//==============================================================================================

u32 PMSI_PARAM_GetInputMode( const PMSI_PARAM* p )
{
	return p->input_mode;
}
u32 PMSI_PARAM_GetGuidanceType( const PMSI_PARAM* p )
{
	return PMSI_GUIDANCE_DEFAULT;
}

int PMSI_PARAM_GetWindowType( const PMSI_PARAM* p )
{
	return 0;  //syachiではありません
}

const ZUKAN_SAVEDATA*  PMSI_PARAM_GetZukanSaveData( const PMSI_PARAM* p )
{
	return p->zukan_savedata;
}

const PMSW_SAVEDATA* PMSI_PARAM_GetPMSW_SaveData( const PMSI_PARAM* p )
{
	return p->pmsw_savedata;
}

BOOL PMSI_PARAM_GetGameClearFlag( const PMSI_PARAM* p )
{
	return p->game_clear_flag;
}

BOOL PMSI_PARAM_GetNotEditEgnoreFlag( const PMSI_PARAM* p )
{
	return p->notedit_egnore_flag;
}

BOOL PMSI_PARAM_GetLockFlag( const PMSI_PARAM* p )
{
  return p->lock_flag;
}

BOOL PMSI_PARAM_GetPictureFlag( const PMSI_PARAM* p )
{
  return p->picture_flag;
}


void PMSI_PARAM_GetInitializeData( const PMSI_PARAM* p, PMS_WORD* word, PMS_DATA* pms )
{
	switch( p->input_mode ){
	case PMSI_MODE_SINGLE:
		word[0] = p->word[0];
		break;
	case PMSI_MODE_DOUBLE:
		word[0] = p->word[0];
		word[1] = p->word[1];
		break;
	case PMSI_MODE_SENTENCE:
		*pms = p->pms;
		break;
	}
}

BOOL PMSI_PARAM_CheckModifiedByEditDatas( const PMSI_PARAM* p, const PMS_WORD* word, const PMS_DATA* pms )
{
	switch( p->input_mode ){
	case PMSI_MODE_SINGLE:
		return word[0] == p->word[0];

	case PMSI_MODE_DOUBLE:
		return ((word[0] == p->word[0]) && (word[1] == p->word[1]));

	case PMSI_MODE_SENTENCE:
	default:
		return PMSDAT_Compare( &p->pms, pms );
	}

}

void PMSI_PARAM_WriteBackData( PMSI_PARAM* p, const PMS_WORD* word, const PMS_DATA* pms )
{
	int i;

	p->modified_flag = (PMSI_PARAM_CheckModifiedByEditDatas(p, word, pms) == FALSE);

  HOSAKA_Printf("PMSI_PARAM_WriteBackData modified_flag=%d \n", p->modified_flag);
	p->cancel_flag = FALSE;

	for(i=0; i<PMS_INPUT_WORD_MAX; i++)
	{
		p->word[i] = word[i];
	}
	p->pms = *pms;
}

int PMSI_PARAM_GetKTStatus(const PMSI_PARAM* p)
{
	return GFL_UI_CheckTouchOrKey();
}

void PMSI_PARAM_SetKTStatus(const PMSI_PARAM* p,int param)
{
	GFL_UI_SetTouchOrKey(param);
}


