//============================================================================================
/**
 * @file	scr_castle_sub.h
 * @bfief	スクリプトコマンド：バトルステージ
 * @author	Satoshi Nohara
 * @date	07.07.24
 *
 * スクリプト関連とは切り離したい処理を置く
 */
//============================================================================================
#ifndef SCR_CASTLE_SUB_H
#define SCR_CASTLE_SUB_H

#include "../frontier/castle_def.h"

//============================================================================================
//
//	通信用ワーク
//
//============================================================================================
typedef struct{
	u8	seq;
	u8	recieve_count;
	u16 dummy;

	u16 mine_monsno[CASTLE_COMM_POKE_NUM];			//自分のポケモンナンバー格納
	u16 pair_monsno[CASTLE_COMM_POKE_NUM];			//相手のポケモンナンバー格納

	u16* ret_wk;
}FLDSCR_CASTLE_COMM;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern void CommFldCastleRecvMonsNo(int id_no,int size,void *pData,void *work);


#endif	/* SCR_CASTLE_SUB_H */



