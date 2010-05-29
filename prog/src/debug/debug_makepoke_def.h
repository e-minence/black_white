//======================================================================
/**
 * @file    debug_makepoke.c
 * @brief   デバッグ用ポケモンデータ生成ツール
 * @author  taya & iwasawa
 * @data    09/05/29
 */
//======================================================================

#pragma once

//--------------------------------------------------------------
/**
 *  Consts
 */
//--------------------------------------------------------------
enum {
  PROC_HEAP_SIZE = 0x20000,
  PRINT_FRAME = GFL_BG_FRAME1_M,

  PRINT_PALIDX  = 0,
  SKB_PALIDX1   = 1,
  SKB_PALIDX2   = 2,

  STRBUF_LEN = 64,
  NUMINPUT_KETA_MAX = 10,

  NUMBOX_MARGIN = 3,
  NUMBOX_CHAR_WIDTH = 8,


  COLIDX_BLACK=1,
  COLIDX_GRAY,
  COLIDX_RED_D,
  COLIDX_RED_L,
  COLIDX_BLUE_D,
  COLIDX_BLUE_L,
  COLIDX_YELLOW_D,
  COLIDX_YELLOW_L,
  COLIDX_GREEN_D,
  COLIDX_GREEN_L,
  COLIDX_ORANGE_D,
  COLIDX_ORANGE_L,
  COLIDX_PINK_D,
  COLIDX_PINK_L,
  COLIDX_WHITE,

  INPUTBOX_NUM_ARG_PER_RND = 0xffffffff,
  INPUTBOX_NUM_ARG_EXP = 0xfffffffe,
  INPUTBOX_NUM_ARG_DEPEND = 0xfffffffd,
  INPUTBOX_NUM_ARG_FORM = 0xfffffffc,

  DMPSTR_NULL = -1,
};

#define CALC_NUMBOX_WIDTH(keta)      ((keta)*NUMBOX_CHAR_WIDTH +NUMBOX_MARGIN*2)
#define CALC_STRBOX_WIDTH(letterCnt)   ((letterCnt)*8 +8)
#define CALC_CAP_BOX_MARGIN(letterCnt)  ((letterCnt*8)+4)
#define CALC_CAP_BOX_MARGIN_HALF(letterCnt)  ((letterCnt*4)+4)

enum {
  LINE_HEIGHT = 12,
  LINE_MARGIN = 2,
  LINE_DIFFER = LINE_HEIGHT + LINE_MARGIN,

  LY_LV1 = 4,
  LY_LV2 = LY_LV1+LINE_DIFFER,
  LY_LV3 = LY_LV2+LINE_DIFFER,
  LY_LV4 = LY_LV3+LINE_DIFFER + 3,
  LY_LV5 = LY_LV4+LINE_DIFFER,
  LY_LV6 = LY_LV5+LINE_DIFFER,
  LY_LV7 = LY_LV6+LINE_DIFFER,
  LY_LV8 = LY_LV7+LINE_DIFFER + 3,
  LY_LV9 = LY_LV8+LINE_DIFFER,
  LY_LV10 = LY_LV9+LINE_DIFFER,
  LY_LV11 = LY_LV10+LINE_DIFFER,
  LY_LV12 = LY_LV11+LINE_DIFFER,
  LY_LV13 = LY_LV12+LINE_DIFFER,

  LX_POKETYPE_CAP = 2,
  LY_POKETYPE_CAP = LY_LV1,
  LX_POKETYPE_BOX = LX_POKETYPE_CAP+CALC_CAP_BOX_MARGIN(4),
  LY_POKETYPE_BOX = LY_LV1,

  LX_LEVEL_CAP = 96,
  LY_LEVEL_CAP = LY_LV1,
  LX_LEVEL_BOX = LX_LEVEL_CAP+CALC_CAP_BOX_MARGIN(3),
  LY_LEVEL_BOX = LY_LEVEL_CAP,

  LX_EXP_CAP = 160,
  LY_EXP_CAP = LY_LV1,
  LX_EXP_BOX = LX_EXP_CAP+CALC_CAP_BOX_MARGIN_HALF(3),
  LY_EXP_BOX = LY_EXP_CAP,

  LX_SEIKAKU_CAP = 2,
  LY_SEIKAKU_CAP = LY_LV2,
  LX_SEIKAKU_BOX = LX_SEIKAKU_CAP + CALC_CAP_BOX_MARGIN(4),
  LY_SEIKAKU_BOX = LY_SEIKAKU_CAP,

  LX_TOKUSEI_CAP = 88,
  LY_TOKUSEI_CAP = LY_LV2,
  LX_TOKUSEI_BOX = LX_TOKUSEI_CAP + CALC_CAP_BOX_MARGIN(4),
  LY_TOKUSEI_BOX = LY_TOKUSEI_CAP,
  
  LX_TOKUSEI_SW_BOX = LX_TOKUSEI_BOX + CALC_STRBOX_WIDTH(8)+4,
  LY_TOKUSEI_SW_BOX = LY_TOKUSEI_CAP,
  
  LX_TOKUSEI_3RD_CAP = 226,
  LY_TOKUSEI_3RD_CAP = LY_LV3,
  LX_TOKUSEI_3RD_BOX = LX_TOKUSEI_3RD_CAP + CALC_STRBOX_WIDTH(1),
  LY_TOKUSEI_3RD_BOX = LY_TOKUSEI_3RD_CAP,
  
  LX_SEX_FIX_CAP = 198,
  LY_SEX_FIX_CAP = LY_LV8,
  LX_SEX_FIX_BOX = LX_SEX_FIX_CAP+CALC_CAP_BOX_MARGIN(4),
  LY_SEX_FIX_BOX = LY_SEX_FIX_CAP,

  LX_ITEM_CAP = 2,
  LY_ITEM_CAP = LY_LV3,
  LX_ITEM_BOX = LX_ITEM_CAP + CALC_CAP_BOX_MARGIN(4),
  LY_ITEM_BOX = LY_ITEM_CAP,

  LX_WAZA1_CAP = 4,
  LY_WAZA1_CAP = LY_LV4,
  LX_WAZA1_BOX = LX_WAZA1_CAP + CALC_CAP_BOX_MARGIN(3),
  LY_WAZA1_BOX = LY_WAZA1_CAP,

  LX_WAZA2_CAP = 4,
  LY_WAZA2_CAP = LY_LV5,
  LX_WAZA2_BOX = LX_WAZA2_CAP + CALC_CAP_BOX_MARGIN(3),
  LY_WAZA2_BOX = LY_WAZA2_CAP,

  LX_WAZA3_CAP = 4,
  LY_WAZA3_CAP = LY_LV6,
  LX_WAZA3_BOX = LX_WAZA3_CAP + CALC_CAP_BOX_MARGIN(3),
  LY_WAZA3_BOX = LY_WAZA3_CAP,

  LX_WAZA4_CAP = 4,
  LY_WAZA4_CAP = LY_LV7,
  LX_WAZA4_BOX = LX_WAZA4_CAP + CALC_CAP_BOX_MARGIN(3),
  LY_WAZA4_BOX = LY_WAZA4_CAP,

  //-------------------------------------------------------------
  // PP欄レイアウト
  //-------------------------------------------------------------
  LX_PP_UPCNT_CAP = 100,
  LX_PP_UPCNT_BOX = LX_PP_UPCNT_CAP + CALC_CAP_BOX_MARGIN_HALF(6),
  LX_PP_EDIT_BOX = LX_PP_UPCNT_BOX + CALC_CAP_BOX_MARGIN_HALF(4),
  LX_PP_MAX_CAP = LX_PP_EDIT_BOX + CALC_CAP_BOX_MARGIN_HALF(5),
  LX_PP_MAX_BOX = LX_PP_MAX_CAP + 8,


  LX_PPCNT1_CAP = LX_PP_UPCNT_CAP,
  LY_PPCNT1_CAP = LY_LV4,
  LX_PPCNT1_BOX = LX_PP_UPCNT_BOX,
  LY_PPCNT1_BOX = LY_PPCNT1_CAP,

  LX_PPCNT2_CAP = LX_PP_UPCNT_CAP,
  LY_PPCNT2_CAP = LY_LV5,
  LX_PPCNT2_BOX = LX_PP_UPCNT_BOX,
  LY_PPCNT2_BOX = LY_PPCNT2_CAP,

  LX_PPCNT3_CAP = LX_PP_UPCNT_CAP,
  LY_PPCNT3_CAP = LY_LV6,
  LX_PPCNT3_BOX = LX_PP_UPCNT_BOX,
  LY_PPCNT3_BOX = LY_PPCNT3_CAP,

  LX_PPCNT4_CAP = LX_PP_UPCNT_CAP,
  LY_PPCNT4_CAP = LY_LV7,
  LX_PPCNT4_BOX = LX_PP_UPCNT_BOX,
  LY_PPCNT4_BOX = LY_PPCNT4_CAP,

  LX_PPMAX1_CAP = LX_PP_MAX_CAP,
  LY_PPMAX1_CAP = LY_LV4,
  LX_PPMAX1_BOX =LX_PP_MAX_BOX,
  LY_PPMAX1_BOX = LY_PPMAX1_CAP,

  LX_PPMAX2_CAP = LX_PP_MAX_CAP,
  LY_PPMAX2_CAP = LY_LV5,
  LX_PPMAX2_BOX = LX_PP_MAX_BOX,
  LY_PPMAX2_BOX = LY_PPMAX2_CAP,

  LX_PPMAX3_CAP = LX_PP_MAX_CAP,
  LY_PPMAX3_CAP = LY_LV6,
  LX_PPMAX3_BOX = LX_PP_MAX_BOX,
  LY_PPMAX3_BOX = LY_PPMAX3_CAP,

  LX_PPMAX4_CAP = LX_PP_MAX_CAP,
  LY_PPMAX4_CAP = LY_LV7,
  LX_PPMAX4_BOX = LX_PP_MAX_BOX,
  LY_PPMAX4_BOX = LY_PPMAX4_CAP,

  LX_PPEDIT1_BOX = LX_PP_EDIT_BOX,
  LY_PPEDIT1_BOX = LY_PPMAX1_CAP,

  LX_PPEDIT2_BOX = LX_PP_EDIT_BOX,
  LY_PPEDIT2_BOX = LY_PPMAX2_CAP,

  LX_PPEDIT3_BOX = LX_PP_EDIT_BOX,
  LY_PPEDIT3_BOX = LY_PPMAX3_CAP,

  LX_PPEDIT4_BOX = LX_PP_EDIT_BOX,
  LY_PPEDIT4_BOX = LY_PPMAX4_CAP,

  //-------------------------------------------------------------
  // ステータス欄レイアウト
  //-------------------------------------------------------------
  STATUS_CAP_LX = 4,
  STATUS_CAP_RND_SPACE = CALC_CAP_BOX_MARGIN_HALF(4),     // キャプションと[rnd]列の間隔
  STATUS_RND_VAL_SPACE = CALC_CAP_BOX_MARGIN_HALF(3),     // [rnd]列と値部分の間隔
  STATUS_RND_EXP_SPACE = CALC_CAP_BOX_MARGIN_HALF(6),     // [rnd]値部分と[EXP]列の間隔
  STATUS_EXP_VAL_SPACE = CALC_CAP_BOX_MARGIN_HALF(3),     // [EXP]列と値部分の間隔
  STATUS_EXP_RESULT_SPACE = CALC_NUMBOX_WIDTH(3)+4,       // [EXP]値部分と計算結果の間隔

  // HP  -------------------------------------------------------------
  LX_HPVAL_CAP = STATUS_CAP_LX,
  LY_HPVAL_CAP = LY_LV8,
  LX_HPRND_CAP = LX_HPVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_HPRND_CAP = LY_LV8,
  LX_HPRND_BOX = LX_HPRND_CAP + STATUS_RND_VAL_SPACE,
  LY_HPRND_BOX = LY_LV8,
  LX_HPEXP_CAP = LX_HPRND_BOX + STATUS_RND_EXP_SPACE,
  LY_HPEXP_CAP = LY_LV8,
  LX_HPEXP_BOX = LX_HPEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_HPEXP_BOX = LY_LV8,
  LX_HPEDIT_BOX  = LX_HPEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_HPEDIT_BOX  = LY_LV8,
  LX_HPSLASH_CAP = LX_HPEDIT_BOX + CALC_NUMBOX_WIDTH(3)+2,
  LY_HPSLASH_CAP = LY_LV8,
  LX_HPVAL_BOX   = LX_HPSLASH_CAP + 8,
  LY_HPVAL_BOX   = LY_LV8,

  // POW -------------------------------------------------------------
  LX_POWVAL_CAP = STATUS_CAP_LX,
  LY_POWVAL_CAP = LY_LV9,
  LX_POWRND_CAP = LX_POWVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_POWRND_CAP = LY_LV9,
  LX_POWRND_BOX = LX_POWRND_CAP + STATUS_RND_VAL_SPACE,
  LY_POWRND_BOX = LY_POWRND_CAP,
  LX_POWEXP_CAP = LX_POWRND_BOX + STATUS_RND_EXP_SPACE,
  LY_POWEXP_CAP = LY_LV9,
  LX_POWEXP_BOX = LX_POWEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_POWEXP_BOX = LY_LV9,
  LX_POWVAL_BOX = LX_POWEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_POWVAL_BOX = LY_LV9,

  // DEF -------------------------------------------------------------
  LX_DEFVAL_CAP = STATUS_CAP_LX,
  LY_DEFVAL_CAP = LY_LV10,
  LX_DEFRND_CAP = LX_DEFVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_DEFRND_CAP = LY_LV10,
  LX_DEFRND_BOX = LX_DEFRND_CAP + STATUS_RND_VAL_SPACE,
  LY_DEFRND_BOX = LY_LV10,
  LX_DEFEXP_CAP = LX_DEFRND_BOX + STATUS_RND_EXP_SPACE,
  LY_DEFEXP_CAP = LY_LV10,
  LX_DEFEXP_BOX = LX_DEFEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_DEFEXP_BOX = LY_LV10,
  LX_DEFVAL_BOX = LX_DEFEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_DEFVAL_BOX = LY_LV10,

  // AGI -------------------------------------------------------------
  LX_AGIVAL_CAP = STATUS_CAP_LX,
  LY_AGIVAL_CAP = LY_LV11,
  LX_AGIRND_CAP = LX_AGIVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_AGIRND_CAP = LY_LV11,
  LX_AGIRND_BOX = LX_AGIRND_CAP + STATUS_RND_VAL_SPACE,
  LY_AGIRND_BOX = LY_LV11,
  LX_AGIEXP_CAP = LX_AGIRND_BOX + STATUS_RND_EXP_SPACE,
  LY_AGIEXP_CAP = LY_LV11,
  LX_AGIEXP_BOX = LX_AGIEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_AGIEXP_BOX = LY_LV11,
  LX_AGIVAL_BOX = LX_AGIEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_AGIVAL_BOX = LY_LV11,

  // SP-POW ----------------------------------------------------------
  LX_SPWVAL_CAP = STATUS_CAP_LX,
  LY_SPWVAL_CAP = LY_LV12,
  LX_SPWRND_CAP = LX_SPWVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_SPWRND_CAP = LY_LV12,
  LX_SPWRND_BOX = LX_SPWRND_CAP + STATUS_RND_VAL_SPACE,
  LY_SPWRND_BOX = LY_LV12,
  LX_SPWEXP_CAP = LX_SPWRND_BOX + STATUS_RND_EXP_SPACE,
  LY_SPWEXP_CAP = LY_LV12,
  LX_SPWEXP_BOX = LX_SPWEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_SPWEXP_BOX = LY_LV12,
  LX_SPWVAL_BOX = LX_SPWEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_SPWVAL_BOX = LY_LV12,

  // SP-DEF ----------------------------------------------------------
  LX_SDFVAL_CAP = STATUS_CAP_LX,
  LY_SDFVAL_CAP = LY_LV13,
  LX_SDFRND_CAP = LX_SDFVAL_CAP + STATUS_CAP_RND_SPACE,
  LY_SDFRND_CAP = LY_LV13,
  LX_SDFRND_BOX = LX_SDFRND_CAP + STATUS_RND_VAL_SPACE,
  LY_SDFRND_BOX = LY_LV13,
  LX_SDFEXP_CAP = LX_SDFRND_BOX + STATUS_RND_EXP_SPACE,
  LY_SDFEXP_CAP = LY_LV13,
  LX_SDFEXP_BOX = LX_SDFEXP_CAP + STATUS_EXP_VAL_SPACE,
  LY_SDFEXP_BOX = LY_LV13,
  LX_SDFVAL_BOX = LX_SDFEXP_BOX + STATUS_EXP_RESULT_SPACE,
  LY_SDFVAL_BOX = LY_LV13,


  LX_TYPE_CAP = 112,
  LY_TYPE_CAP = LY_LV3,
  LX_TYPE_BOX = LX_TYPE_CAP + CALC_CAP_BOX_MARGIN(3),
  LY_TYPE_BOX = LY_TYPE_CAP,

  LX_DEFAULT_BTN = LX_PP_MAX_BOX + CALC_NUMBOX_WIDTH(2) + 2,
  LY_DEFAULT_BTN = LY_LV4,

  LX_HATAKU_BTN = LX_DEFAULT_BTN,
  LY_HATAKU_BTN = LY_LV5+8,

  LX_HANERU_BTN = LX_DEFAULT_BTN,
  LY_HANERU_BTN = LY_LV6+8,

  LX_PER_RND_CAP = 156,
  LY_PER_RND_CAP = LY_LV9,
  LX_PER_RND_BOX = LX_PER_RND_CAP+4,
  LY_PER_RND_BOX = LY_PER_RND_CAP+10,
  
  LX_FORM_CAP = 156,
  LY_FORM_CAP = LY_LV11,
  LX_FORM_BOX = LX_FORM_CAP + CALC_STRBOX_WIDTH(4),
  LY_FORM_BOX = LY_FORM_CAP,

  LX_NICKNAME_CAP = 4,
  LY_NICKNAME_CAP = 4,
  LX_NICKNAME_BOX = LX_NICKNAME_CAP + CALC_STRBOX_WIDTH(6),
  LY_NICKNAME_BOX = LY_NICKNAME_CAP,

  LX_POKERUS_CAP = 156,
  LY_POKERUS_CAP = LY_LV1,
  LX_POKERUS_BOX = LX_POKERUS_CAP + CALC_STRBOX_WIDTH(5),
  LY_POKERUS_BOX = LY_POKERUS_CAP,
  
  LX_SICK_CAP = 4,
  LY_SICK_CAP = LY_LV2,
  LX_SICK_BOX = LX_SICK_CAP + CALC_STRBOX_WIDTH(4),
  LY_SICK_BOX = LY_SICK_CAP,

  LX_NATSUKI_CAP = LX_SICK_BOX + CALC_STRBOX_WIDTH(4)+8,
  LY_NATSUKI_CAP = LY_LV2,
  LX_NATSUKI_BOX = LX_NATSUKI_CAP + CALC_STRBOX_WIDTH(4),
  LY_NATSUKI_BOX = LY_NATSUKI_CAP,
  
  LX_ID_H_CAP = 4,
  LY_ID_H_CAP = LY_LV3,
  LX_ID_H_BOX = LX_ID_H_CAP + CALC_STRBOX_WIDTH(2),
  LY_ID_H_BOX = LY_ID_H_CAP,
  LX_ID_L_CAP = LX_ID_H_BOX + CALC_NUMBOX_WIDTH(6),
  LY_ID_L_CAP = LY_LV3,
  LX_ID_L_BOX = LX_ID_L_CAP + CALC_STRBOX_WIDTH(2),
  LY_ID_L_BOX = LY_ID_L_CAP,
  
  LX_PER_RND_H_CAP = 4,
  LY_PER_RND_H_CAP = LY_LV4,
  LX_PER_RND_H_BOX = LX_PER_RND_H_CAP + CALC_STRBOX_WIDTH(2),
  LY_PER_RND_H_BOX = LY_PER_RND_H_CAP,
  LX_PER_RND_L_CAP = LX_PER_RND_H_BOX + CALC_NUMBOX_WIDTH(6),
  LY_PER_RND_L_CAP = LY_LV4,
  LX_PER_RND_L_BOX = LX_PER_RND_L_CAP + CALC_STRBOX_WIDTH(2),
  LY_PER_RND_L_BOX = LY_PER_RND_L_CAP,
  
  LX_RARE_CAP = 156,
  LY_RARE_CAP = LY_LV3,
  LX_RARE_BOX = LX_RARE_CAP + 20,
  LY_RARE_BOX = LY_RARE_CAP,
  
  LX_RARE_SET_CAP = 156,
  LY_RARE_SET_CAP = LY_LV3,
  LX_RARE_SET_BTN = LX_RARE_BOX + CALC_NUMBOX_WIDTH(1)+2, 
  LY_RARE_SET_BTN = LY_LV3,
  
  LX_SEX_CAP = 156,
  LY_SEX_CAP = LY_LV4,
  LX_SEX_BOX = LX_SEX_CAP+CALC_STRBOX_WIDTH(4),
  LY_SEX_BOX = LY_SEX_CAP,
  
  LX_SEX_SET_CAP = LX_SEX_BOX+CALC_STRBOX_WIDTH(1)+4,
  LY_SEX_SET_CAP = LY_LV4,
  LX_SEX_SET_BOX = LX_SEX_BOX+CALC_STRBOX_WIDTH(1)+4,
  LY_SEX_SET_BOX = LY_LV4,

  LX_EVENTGET_CAP   = 4,
  LY_EVENTGET_CAP   = LY_LV5,
  LX_EVENTGET_BOX   = LX_EVENTGET_CAP + CALC_STRBOX_WIDTH(7)+4,
  LY_EVENTGET_BOX   = LY_EVENTGET_CAP,

  LX_TAMAGO_CAP = LX_EVENTGET_BOX+CALC_STRBOX_WIDTH(1)+4,
  LY_TAMAGO_CAP = LY_LV5,
  LX_TAMAGO_BOX = LX_TAMAGO_CAP + 30,
  LY_TAMAGO_BOX = LY_TAMAGO_CAP,

  LX_GET_LEVEL_CAP = 4,
  LY_GET_LEVEL_CAP = LY_LV7,
  LX_GET_LEVEL_BOX = LX_GET_LEVEL_CAP + CALC_STRBOX_WIDTH(6),
  LY_GET_LEVEL_BOX = LY_GET_LEVEL_CAP,

  LX_GET_BALL_CAP = LX_GET_LEVEL_BOX + CALC_NUMBOX_WIDTH(4),
  LY_GET_BALL_CAP = LY_LV7,
  LX_GET_BALL_BOX = LX_GET_BALL_CAP + CALC_STRBOX_WIDTH(3),
  LY_GET_BALL_BOX = LY_GET_BALL_CAP,

  LX_GET_CASETTE_CAP = 4,
  LY_GET_CASETTE_CAP = LY_LV8,
  LX_GET_CASETTE_BOX = LX_GET_CASETTE_CAP + CALC_STRBOX_WIDTH(4),
  LY_GET_CASETTE_BOX = LY_GET_CASETTE_CAP,

  LX_COUNTRY_CAP = LX_GET_CASETTE_BOX + CALC_STRBOX_WIDTH(6)+4,
  LY_COUNTRY_CAP = LY_LV8,
  LX_COUNTRY_BOX = LX_COUNTRY_CAP + CALC_STRBOX_WIDTH(2),
  LY_COUNTRY_BOX = LY_COUNTRY_CAP,
  
  LX_OYA_SEX_CAP = LX_COUNTRY_BOX + CALC_STRBOX_WIDTH(5)+4,
  LY_OYA_SEX_CAP = LY_LV8,
  LX_OYA_SEX_BOX = LX_OYA_SEX_CAP + CALC_STRBOX_WIDTH(5),
  LY_OYA_SEX_BOX = LY_OYA_SEX_CAP,

  LX_GET_PLACE_CAP = 4,
  LY_GET_PLACE_CAP = LY_LV9,
  LX_GET_PLACE_BOX = LX_GET_PLACE_CAP + CALC_STRBOX_WIDTH(9),
  LY_GET_PLACE_BOX = LY_GET_PLACE_CAP,
  LX_GET_PLACE_TAG_CAP = 4,
  LY_GET_PLACE_TAG_CAP = LY_LV9,
  LX_GET_PLACE_TAG_BOX = LX_GET_PLACE_BOX + CALC_NUMBOX_WIDTH(5)+4,
  LY_GET_PLACE_TAG_BOX = LY_GET_PLACE_BOX,

  LX_GET_YEAR_CAP  = 4,
  LY_GET_YEAR_CAP  = LY_LV10,
  LX_GET_YEAR_BOX  = LX_GET_YEAR_CAP + CALC_STRBOX_WIDTH(1),
  LY_GET_YEAR_BOX  = LY_GET_YEAR_CAP,
  LX_GET_MONTH_CAP = LX_GET_YEAR_BOX + CALC_STRBOX_WIDTH(4),
  LY_GET_MONTH_CAP = LY_LV10,
  LX_GET_MONTH_BOX = LX_GET_MONTH_CAP + CALC_STRBOX_WIDTH(1),
  LY_GET_MONTH_BOX = LY_GET_MONTH_CAP,
  LX_GET_DAY_CAP   = LX_GET_MONTH_BOX + CALC_STRBOX_WIDTH(4),
  LY_GET_DAY_CAP   = LY_LV10,
  LX_GET_DAY_BOX   = LX_GET_DAY_CAP + CALC_STRBOX_WIDTH(1),
  LY_GET_DAY_BOX   = LY_GET_DAY_CAP,

  LX_BIRTH_PLACE_CAP = 4,
  LY_BIRTH_PLACE_CAP = LY_LV11,
  LX_BIRTH_PLACE_BOX = LX_BIRTH_PLACE_CAP + CALC_STRBOX_WIDTH(9),
  LY_BIRTH_PLACE_BOX = LY_BIRTH_PLACE_CAP,
  LX_BIRTH_PLACE_TAG_CAP = 4,
  LY_BIRTH_PLACE_TAG_CAP = LY_LV11,
  LX_BIRTH_PLACE_TAG_BOX = LX_BIRTH_PLACE_BOX + CALC_NUMBOX_WIDTH(5)+4,
  LY_BIRTH_PLACE_TAG_BOX = LY_BIRTH_PLACE_BOX,
  LX_BIRTH_YEAR_CAP  = 4,
  LY_BIRTH_YEAR_CAP  = LY_LV12,
  LX_BIRTH_YEAR_BOX  = LX_BIRTH_YEAR_CAP + CALC_STRBOX_WIDTH(1),
  LY_BIRTH_YEAR_BOX  = LY_BIRTH_YEAR_CAP,
  LX_BIRTH_MONTH_CAP = LX_BIRTH_YEAR_BOX + CALC_STRBOX_WIDTH(4),
  LY_BIRTH_MONTH_CAP = LY_LV12,
  LX_BIRTH_MONTH_BOX = LX_BIRTH_MONTH_CAP + CALC_STRBOX_WIDTH(1),
  LY_BIRTH_MONTH_BOX = LY_BIRTH_MONTH_CAP,
  LX_BIRTH_DAY_CAP   = LX_BIRTH_MONTH_BOX + CALC_STRBOX_WIDTH(4),
  LY_BIRTH_DAY_CAP   = LY_LV12,
  LX_BIRTH_DAY_BOX   = LX_BIRTH_DAY_CAP + CALC_STRBOX_WIDTH(1),
  LY_BIRTH_DAY_BOX   = LY_BIRTH_DAY_CAP,
};

typedef enum {
  INPUTBOX_TYPE_STR,      ///< 入力対応文字列
  INPUTBOX_TYPE_NUM,      ///< 入力対応数値
  INPUTBOX_TYPE_SWITCH,   ///< タッチ対応トグルスイッチ
  INPUTBOX_TYPE_FIXVAL,   ///< 表示するだけの数値
  INPUTBOX_TYPE_FIXSTR,   ///< 表示するだけの文字列
  INPUTBOX_TYPE_BTN,      ///< タッチ反応で特殊処理するためのボタン

  INPUTBOX_TYPE_NICKNAME,
}InputBoxType;

#define FIXSTR_CODE_PLACE (0xFFFF)

typedef enum {

  INPUTBOX_ID_POKETYPE=0,
  INPUTBOX_ID_LEVEL,
  INPUTBOX_ID_EXP,
  INPUTBOX_ID_SEX_FIX,
  INPUTBOX_ID_SEIKAKU,
  INPUTBOX_ID_TOKUSEI,
  INPUTBOX_ID_TOKUSEI_SWITCH,
  INPUTBOX_ID_TOKUSEI_3RD,
  INPUTBOX_ID_ITEM,
  INPUTBOX_ID_WAZA1,
  INPUTBOX_ID_WAZA2,
  INPUTBOX_ID_WAZA3,
  //10
  INPUTBOX_ID_WAZA4,
  INPUTBOX_ID_PPCNT1,
  INPUTBOX_ID_PPCNT2,
  INPUTBOX_ID_PPCNT3,
  INPUTBOX_ID_PPCNT4,
  INPUTBOX_ID_PPMAX1,
  INPUTBOX_ID_PPMAX2,
  INPUTBOX_ID_PPMAX3,
  INPUTBOX_ID_PPMAX4,
  INPUTBOX_ID_PPEDIT1,
  //20
  INPUTBOX_ID_PPEDIT2,
  INPUTBOX_ID_PPEDIT3,
  INPUTBOX_ID_PPEDIT4,
  INPUTBOX_ID_HPRND,
  INPUTBOX_ID_HPEXP,
  INPUTBOX_ID_HPVAL,
  INPUTBOX_ID_HPEDIT,
  INPUTBOX_ID_POWRND,
  INPUTBOX_ID_POWEXP,
  INPUTBOX_ID_POWVAL,
  //30
  INPUTBOX_ID_DEFRND,
  INPUTBOX_ID_DEFEXP,
  INPUTBOX_ID_DEFVAL,
  INPUTBOX_ID_AGIRND,
  INPUTBOX_ID_AGIEXP,
  INPUTBOX_ID_AGIVAL,
  INPUTBOX_ID_SPWRND,
  INPUTBOX_ID_SPWEXP,
  INPUTBOX_ID_SPWVAL,
  INPUTBOX_ID_SDFRND,
  //40
  INPUTBOX_ID_SDFEXP,
  INPUTBOX_ID_SDFVAL,
  INPUTBOX_ID_TYPE1,
  INPUTBOX_ID_TYPE2,
  INPUTBOX_ID_DEF_BUTTON,
  INPUTBOX_ID_HATAKU_BUTTON,
  INPUTBOX_ID_HANERU_BUTTON,
  INPUTBOX_ID_FORM,

  //2ページ目
  INPUTBOX_ID_NICKNAME,
  INPUTBOX_ID_NICKNAME_FLG,
  INPUTBOX_ID_POKERUS,
  INPUTBOX_ID_SICK,
  INPUTBOX_ID_NATSUKI,
  
  INPUTBOX_ID_OYAID_H,
  INPUTBOX_ID_OYAID_L,
  INPUTBOX_ID_PER_RND_H,
  INPUTBOX_ID_PER_RND_L,
  INPUTBOX_ID_RARE,
  INPUTBOX_ID_RARE_SET_BUTTON,
  INPUTBOX_ID_SEX,
  INPUTBOX_ID_SEX_SET,

  INPUTBOX_ID_EVENTGET_FLAG,
  INPUTBOX_ID_TAMAGO,
  
  INPUTBOX_ID_GET_LEVEL,
  INPUTBOX_ID_GET_BALL,
  INPUTBOX_ID_GET_CASETTE,
  INPUTBOX_ID_COUNTRY,
  INPUTBOX_ID_OYA_SEX,

  INPUTBOX_ID_GET_PLACE,
  INPUTBOX_ID_GET_PLACE_TAG,
  INPUTBOX_ID_GET_YEAR,
  INPUTBOX_ID_GET_MONTH,
  INPUTBOX_ID_GET_DAY,
  INPUTBOX_ID_BIRTH_PLACE,
  INPUTBOX_ID_BIRTH_PLACE_TAG,
  INPUTBOX_ID_BIRTH_YEAR,
  INPUTBOX_ID_BIRTH_MONTH,
  INPUTBOX_ID_BIRTH_DAY,

  INPUTBOX_ID_MAX,

}InputBoxID;

enum {
  PAGE_0,
  PAGE_1,
  PAGE_MAX,
};


typedef enum {
  SWITCH_STRNUM_DEFAULT = 2,
  SWITCH_STRNUM_SICK = 6,
}SwitchBoxStrNum;


