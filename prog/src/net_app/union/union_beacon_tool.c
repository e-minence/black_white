//=============================================================================
/**
 * @file	union_beacon_tool.c
 * @brief	ユニオンルームビーコン処理
 * @author	Akito Mori
 * @date		2006.03.16
 */
//=============================================================================

#include <gflib.h>

#include "arc_def.h"
#include "test_graphic/trbgra.naix"
#include "test_graphic/trfgra.naix"

#include "print/wordset.h"

#include "net_app/union/union_beacon_tool.h"
#include "field/fldmmdl.h"
#include "msg/msg_trtype.h"
#include "tr_tool/trtype_def.h"

#define UNION_VIEW_TYPE_NUM	( 16 )

// ユニオンルームで表示するトレーナーのタイプ見た目＆名前＆トレーナーグラフィックテーブル
static const struct{
  u16 objcode;
  u8 msg_type;
  u8 tr_type;
}UnionViewTable[] = {
	{ BOY2,		MSG_TRTYPE_TANPAN,		TRTYPE_TANPAN	},	///< たんパンこぞう
	{ TRAINERM,		MSG_TRTYPE_ELITEM,		TRTYPE_ELITEM	},	///< エリートトレーナー
	{ RANGERM,		MSG_TRTYPE_RANGERM,		TRTYPE_RANGERM	},	///< ポケモンレンジャー
	{ BREEDERM,		MSG_TRTYPE_BREEDERM,		TRTYPE_BREEDERM	},	///< ポケモンブリーダー
	{ ASSISTANTM,	MSG_TRTYPE_SCIENTISTM,		TRTYPE_SCIENTISTM	},	///< けんきゅういん
	{ MOUNTMAN,	MSG_TRTYPE_MOUNT,		TRTYPE_MOUNT	},	///< やまおとこ
	{ BADMAN,	MSG_TRTYPE_HEADS,		TRTYPE_HEADS	},	///< スキンヘッズ
	{ BABYBOY2,	MSG_TRTYPE_KINDERGARTENM,		TRTYPE_KINDERGARTENM	},	///< えんじ

	{ GIRL2,		MSG_TRTYPE_MINI,		TRTYPE_MINI	},	///< ミニスカート
	{ TRAINERW,		MSG_TRTYPE_ELITEW,		TRTYPE_ELITEW	},	///< エリートトレーナー
	{ RANGERW,		MSG_TRTYPE_RANGERW,		TRTYPE_RANGERW	},	///< ポケモンレンジャー
	{ BREEDERW,		MSG_TRTYPE_BREEDERW,		TRTYPE_BREEDERW	},	///< ポケモンブリーダー
	{ ASSISTANTW,		MSG_TRTYPE_SCIENTISTW,		TRTYPE_SCIENTISTW	},	///< けんきゅういん
	{ AMBRELLA,		MSG_TRTYPE_PARASOL,		TRTYPE_PARASOL	},	///< パラソルおねえさん
	{ NURSE,	MSG_TRTYPE_NURSE,		TRTYPE_NURSE	},	///< ナース
	{ BABYGIRL2,	MSG_TRTYPE_KINDERGARTENW,		TRTYPE_KINDERGARTENW	},	///< えんじ
};


//==================================================================
/**
 * トレーナータイプのテーブルINDEXからOBJCODEを取得する
 * @param   view_index		テーブルINDEX
 * @retval  int		OBJCODE
 */
//==================================================================
int UnionView_GetObjCode(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return BOY2;
  }
  return UnionViewTable[view_index].objcode;
}

//==================================================================
/**
 * トレーナータイプのテーブルINDEXからTRTYPEを取得する
 * @param   view_index		テーブルINDEX
 * @retval  int		TRTYPE
 */
//==================================================================
int UnionView_GetTrType(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return TRTYPE_TANPAN;
  }
  return UnionViewTable[view_index].tr_type;
}

//==================================================================
/**
 * トレーナータイプのテーブルINDEXからMSGTYPEを取得する
 * @param   view_index		テーブルINDEX
 * @retval  int		TRTYPE
 */
//==================================================================
int UnionView_GetMsgType(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return MSG_TRTYPE_TANPAN;
  }
  return UnionViewTable[view_index].msg_type;
}

//==================================================================
/**
 * OBJCODEからIndexを逆引きする
 * @param   objcode		OBJCODE
 * @retval  int		Index
 */
//==================================================================
int UnionView_Objcode_to_Index(u16 objcode)
{
  int i;
  
  for(i = 0; i < NELEMS(UnionViewTable); i++){
    if(UnionViewTable[i].objcode == objcode){
      return i;
    }
  }
  GF_ASSERT_MSG(0, "objcode = %d\n", objcode);
  return 0;
}

//==================================================================
/**
 * TrTypeからIndexを逆引きする
 * @param   trtype  トレーナータイプ
 * @retval  int		  Index
 */
//==================================================================
int UnionView_TrType_to_Index(u8 trtype)
{
  int i;
  
  for(i = 0; i < NELEMS(UnionViewTable); i++){
    if(UnionViewTable[i].tr_type == trtype){
      return i;
    }
  }
  GF_ASSERT_MSG(0, "tr_type = %d\n", trtype);
  return 0;
}

