//======================================================================
/**
 * @file    itemmenu.h
 * @brief   アイテムメニュー
 * @author  k.ohno
 * @date    2009.06.30
 */
//======================================================================

#pragma once

#include <gflib.h>
#include "app/bag.h"


// メニュー項目
enum {
  BAG_MENU_TSUKAU = 0,  ///< つかう
  BAG_MENU_ORIRU,       ///< おりる
  BAG_MENU_MIRU,        ///< みる
  BAG_MENU_SUTERU,      ///< すてる
  BAG_MENU_TOUROKU,     ///< とうろく
  BAG_MENU_KAIZYO,      ///< かいじょ
  BAG_MENU_MOTASERU,    ///< もたせる
  BAG_MENU_KETTEI,      ///< けってい
  BAG_MENU_YAMERU,      ///< やめる
  BAG_MENU_URU,         ///< うる

  BAG_MENU_MAX
};

// メニュー項目格納場所
enum{
  BAG_MENU_USE=0,     // つかう・みる・おりる
  BAG_MENU_GIVE,      // もたせる
  BAG_MENU_SUB,       // すてる・とうろく・かいじょ
  BAG_MENU_CANCEL,
  BAG_MENUTBL_MAX,
};

