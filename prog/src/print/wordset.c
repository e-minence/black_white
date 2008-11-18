//============================================================================================
/**
 * @file	wordset.c
 * @brief	単語モジュール処理
 * @author	taya
 * @date	2005.09.28
 */
//============================================================================================
#include <gflib.h>

#include "print/printsys.h"
#include "print/wordset.h"

#include "arc_def.h"
#include "message.naix"

/*----------------------------------*/
/** 文法性                          */
/*----------------------------------*/
enum {
	WORDSET_GRAMMER_NONE,		///< 使用しない
	WORDSET_GRAMMER_MALE,		///< 男性
	WORDSET_GRAMMER_FEMALE,		///< 女性
	WORDSET_GRAMMER_NEUTRAL,	///< 中性
};


/*----------------------------------*/
/** 単語パラメータ                  */
/*----------------------------------*/
typedef struct {
	u8  def_article;		///< 定冠詞ＩＤ
	u8  indef_article;		///< 不定冠詞ＩＤ
	u8  preposition;		///< 前置詞ＩＤ
	u8  grammer : 7;		///< 文法性
	u8  form : 1;			///< 単数・複数（0 = 単数）
}WORDSET_PARAM;


typedef struct {
	WORDSET_PARAM	param;
	STRBUF*			str;
}WORD;



struct _WORDSET{

	u32			max;
	u32			heapID;
	WORD*		word;
	STRBUF*		tmpBuf;

};



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void InitParam(WORDSET_PARAM* param);
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param );



//======================================================================================================
// システム初期化・終了
//======================================================================================================

//------------------------------------------------------------------
/**
 * 単語セットモジュール作成
 *
 * @param   heapID			作成先ヒープＩＤ
 *
 * @retval  WORDSET*		単語セット
 */
//------------------------------------------------------------------
WORDSET*  WORDSET_Create( HEAPID heapID )
{
	return WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_DEFAULT_BUFLEN, heapID );
}

//------------------------------------------------------------------
/**
 * 単語セットモジュール作成（登録単語数・文字列長の指定版）
 *
 * @param   word_max		
 * @param   buflen			
 * @param   heapID			
 *
 * @retval  WORDSET*		
 */
//------------------------------------------------------------------
WORDSET* WORDSET_CreateEx( u32 word_max, u32 buflen, HEAPID heapID )
{
	GF_ASSERT(word_max);
	GF_ASSERT(buflen);

	{
		WORDSET* wordset = NULL;
		u32 i;

		do {

			WORDSET* wordset = GFL_HEAP_AllocMemory( heapID, sizeof(WORDSET) );
			if( wordset == NULL ){ break; }

			wordset->max = word_max;
			wordset->heapID = heapID;

			wordset->tmpBuf = GFL_STR_CreateBuffer( buflen, heapID );
			if( wordset->tmpBuf == NULL ){ break; }

			wordset->word = GFL_HEAP_AllocMemory( heapID, sizeof(WORD)*word_max );
			if( wordset->word == NULL ){ break; }

			for(i=0; i<word_max; i++)
			{
				InitParam(&(wordset->word[i].param));
				wordset->word[i].str = GFL_STR_CreateBuffer( buflen, heapID );
				if( wordset->word[i].str == NULL ){
					break;
				}
			}

			if( i == word_max )
			{
				return wordset;
			}

		}while(0);

		if( wordset )
		{
			WORDSET_Delete(wordset);
		}
		return NULL;
	}
}

//------------------------------------------------------------------
/**
 * 単語セット破棄
 *
 * @param   wordset		
 *
 */
//------------------------------------------------------------------
void WORDSET_Delete( WORDSET* wordset )
{
	u32 i;

	GF_ASSERT(wordset->max);

	if( wordset->word )
	{
		for(i=0; i<wordset->max; i++)
		{
			if( wordset->word[i].str )
			{
				GFL_STR_DeleteBuffer( wordset->word[i].str );
			}
			else
			{
				break;
			}
		}
		GFL_HEAP_FreeMemory( wordset->word );
	}

	if( wordset->tmpBuf )
	{
		GFL_STR_DeleteBuffer(wordset->tmpBuf);
	}

	wordset->max = 0;
	GFL_HEAP_FreeMemory(wordset);
}


//------------------------------------------------------------------
/**
 * 単語パラメータ初期化
 *
 * @param   param		パラメータ構造体へのポインタ
 *
 */
//------------------------------------------------------------------
static void InitParam(WORDSET_PARAM* param)
{
	// 現状はただのゼロクリア
	GFL_STD_MemClear( param, sizeof(param) );
}



//======================================================================================================
// 単語セットモジュールに各種単語を登録する
//======================================================================================================

//------------------------------------------------------------------
/**
 * 単語文字列＆パラメータを指定バッファに登録
 *
 * @param   wordset		単語セットモジュール
 * @param   bufID		バッファＩＤ
 * @param   str			文字列
 * @param   param		文字列に付随するパラメータ
 *
 */
//------------------------------------------------------------------
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param )
{
	GF_ASSERT_MSG( bufID < wordset->max, "bufID=%d, wordmax=%d", bufID, wordset->max );

	if( bufID < wordset->max )
	{
		if( param != NULL )
		{
			wordset->word[bufID].param = *param;
		}
		GFL_STR_CopyBuffer( wordset->word[bufID].str, str );
	}
}


//------------------------------------------------------------------
/**
 * 任意文字列をパラメータ付きで登録
 *
 * @param   wordset			単語セット
 * @param   bufID			バッファID
 * @param   word			文字列
 * @param   sex				性別コード
 * @param	singular_flag	単／複（TRUEで単数）
 * @param   lang			言語コード
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* word, u32 sex, BOOL singular_flag, u32 lang )
{
	WORDSET_PARAM  param;

	InitParam( &param );
	param.grammer = sex;
	param.form = !singular_flag;

	RegisterWord( wordset, bufID, word, &param );
}


//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録
 *
 * @param   bufID		バッファID
 * @param   ppp			ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
	GFL_MSGDATA* mdat;
	u32 monsno;

	mdat = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat, GetHeapLowID(wordset->heapID) );

	// [[[ここでポケモンの性別等をチェックできる]]]
	monsno = PokeParaGet( pp, ID_PARA_monsno, NULL );
	GFL_MSG_GetString( mdat, monsno, wordset->tmpBuf );
	RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);

	GFL_MSG_Delete( mdat );
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのニックネームを登録
 *
 * @param   bufID		バッファID
 * @param   ppp			ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeNickName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
	// [[[ここでポケモンの性別等をチェックできる]]]
	PokeParaGet( pp, ID_PARA_nickname_buf, wordset->tmpBuf );
	RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンの親名を登録
 *
 * @param   bufID		バッファID
 * @param   ppp		ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeOyaName( WORDSET* wordset,  u32 bufID, const POKEMON_PARAM* pp )
{
	// [[[ここで親の性別もチェックできる]]]
	PokeParaGet( pp, ID_PARA_oyaname_buf, wordset->tmpBuf );
	RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);

}
//------------------------------------------------------------------
/**
 * 指定バッファにワザ名を登録
 *
 * @param   bufID			バッファID
 * @param   wazaID			ワザID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWazaName( WORDSET* wordset, u32 bufID, u32 wazaID )
{
	GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
					NARC_message_wazaname_dat, GetHeapLowID(wordset->heapID) );
	if( man )
	{
		GFL_MSG_GetString( man, wazaID, wordset->tmpBuf );
		RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
		GFL_MSG_Delete( man );
	}
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのとくせい名を登録
 *
 * @param   bufID			バッファID
 * @param   tokuseiID		とくせいID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTokuseiName( WORDSET* wordset, u32 bufID, u32 tokuseiID )
{
	GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
					NARC_message_tokusei_dat, GetHeapLowID(wordset->heapID) );
	if( man )
	{
		GFL_MSG_GetString( man, tokuseiID, wordset->tmpBuf );
		RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
		GFL_MSG_Delete( man );
	}
}
//------------------------------------------------------------------
/**
 * 指定バッファに数字を登録
 *
 * @param   bufID		バッファID
 * @param   number		数値
 * @param   keta		桁数
 * @param   dispType	整形タイプ
 * @param   codeType	文字コードタイプ
 *
 * dispType :  STR_NUM_DISP_LEFT    左詰め
 *             STR_NUM_DISP_RIGHT   右詰め（スペース埋め）
 *             STR_NUM_DISP_ZERO    右詰め（ゼロ埋め）
 *
 *
 * codeType :  STR_NUM_CODE_DEFAULT  ローカライズ時のデフォルト幅（日本＝全角幅／海外＝半角幅）になる
 *             STR_NUM_CODE_ZENKAKU  全角幅指定
 *             STR_NUM_CODE_HANKAKU  半角幅指定
 */
//------------------------------------------------------------------
void WORDSET_RegisterNumber( WORDSET* wordset, u32 bufID, s32 number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType )
{
	STRTOOL_SetNumber( wordset->tmpBuf, number, keta, dispType, codeType );
	RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}



//======================================================================================================
// 文字列展開
//======================================================================================================

//------------------------------------------------------------------
/**
 * 登録された単語を使って文字列展開する
 *
 * @param   dst		展開先バッファ
 * @param   src		展開元文字列
 *
 */
//------------------------------------------------------------------
void WORDSET_ExpandStr( const WORDSET* wordset, STRBUF* dstbuf, const STRBUF* srcbuf )
{
	const STRCODE* src;
	STRCODE *dst;
	STRCODE EOM_Code, TAG_Code;

	src = GFL_STR_GetStringCodePointer( srcbuf );
	GFL_STR_ClearBuffer( dstbuf );
	EOM_Code = GFL_STR_GetEOMCode();
	TAG_Code = PRINTSYS_GetTagStartCode();

	while( *src != EOM_Code )
	{
		if( *src == TAG_Code )
		{
			if( PRINTSYS_IsWordSetTagType(src) )
			{
				u32 word_id;

				word_id = PRINTSYS_GetTagParam(src, 0);

				GF_ASSERT( word_id < wordset->max );

				// [[[海外版ではパラメータの解釈が必要]]]
				GFL_STR_AddString( dstbuf, wordset->word[ word_id ].str );
				src = PRINTSYS_SkipTag( src );
			}
			else
			{
				const STRCODE* p = src;

				src = PRINTSYS_SkipTag( src );
				while(p < src)
				{
					GFL_STR_AddCode( dstbuf, *p++ );
				}
			}
		}
		else
		{
			GFL_STR_AddCode( dstbuf, *src++ );
		}
	}
}




//======================================================================================================
// バッファクリア
//======================================================================================================

//------------------------------------------------------------------
/**
 * 内部バッファを全て空文字列でクリアする
 *
 * @param   wordset		WORDSETへのポインタ
 *
 */
//------------------------------------------------------------------
void WORDSET_ClearAllBuffer( WORDSET* wordset )
{
	u32 i;
	for(i=0; i<wordset->max; i++)
	{
		GFL_STR_ClearBuffer( wordset->word[i].str );
	}
}




