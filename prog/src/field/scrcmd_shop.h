//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  スクリプトコマンド：ショップ関連
 * @file   scrcmd_shop.h
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#pragma once


typedef struct {
  u16 id;       // どうぐID
  u32 price;    // 値段 or BPポイント
}SHOP_ITEM;

// 一度に並べることができる商品数
#define SHOP_ITEM_MAX   ( 40 )

//--------------------------------------------------------------------
/**
 * @brief 通常ショップ呼び出し 
 *
 * @param core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk );

