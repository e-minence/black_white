//============================================================================================
/**
 * @file  btlv_input.c
 * @brief 戦闘下画面
 * @author  soga
 * @date  2009.06.29
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "btlv_effect.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "waza_tool/wazadata.h"
#include "system/bmp_oam.h"
#include "infowin/infowin.h"
#include "pokeicon/pokeicon.h"
#include "system/sdkdef.h"

#include "app/app_menu_common.h"

#include "battle/btl_field.h"

#include "btlv_input.h"
#include "data/btlv_input.cdat"
#include "btlv_finger_cursor.h"

#include "arc_def.h"
#include "battle/battgra_wb.naix"

#include "pm_define.h"

#include "sound/pm_sndsys.h"

#include "message.naix"
#include "msg/msg_btlv_input.h"

#include "data/rotate_sel.cdat"

extern  const BTLV_INPUT_KEYTBL  *PokeSeleMenuKey4Data[ 2 ][ WAZA_TARGET_MAX + 1 ];
extern  const BTLV_INPUT_KEYTBL  *PokeSeleMenuKey6Data[ 3 ][ WAZA_TARGET_MAX + 1 ];
extern  const u8 PokeSeleMenuKey4DataCount[ 2 ][ WAZA_TARGET_MAX + 1 ];
extern  const u8 PokeSeleMenuKey6DataCount[ 3 ][ WAZA_TARGET_MAX + 1 ];

//============================================================================================
/**
 *  定数宣言
 */
//============================================================================================

#define BTLV_INPUT_TCB_MAX ( 8 )    //使用するTCBのMAX
#define BTLV_INPUT_BUTTON_MAX ( 6 + 2 )  //押せるボタンのMAX値（3vs3時の対象選択6個＋キャンセル1個）
#define BG_CLEAR_CODE     (0x8000 / 0x20 - 1)   ///BGスクリーンのクリアコード（キャラクタの一番後ろを指定）

//カーソル表示にするボタン
#define BTLV_INPUT_CURSOR_ON_BUTTON ( PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT | \
                                      PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y )

#define INFOWIN_PAL_NO  ( 0x0f )  //情報ステータスバーパレット
#define BMPWIN_PAL_NO  ( 0x0d )   //BMPWINパレット

#define WAZATYPEICON_COMP_CHAR  ( 0 )
#define WAZATYPEICON_OAMSIZE    ( 32 * 8 )

#define STANDBY_PAL        ( 0x0003 )
#define STANDBY_FADE        ( 12 )
#define STANDBY_FADE_COLOR  ( 0x0842 )

//PP表示用カラー定義
#define MSGCOLOR_PP_WHITE   ( PRINTSYS_LSB_Make( 1,  2, 0 ) )
#define MSGCOLOR_PP_YELLOW  ( PRINTSYS_LSB_Make( 3,  4, 0 ) )
#define MSGCOLOR_PP_ORANGE  ( PRINTSYS_LSB_Make( 5,  6, 0 ) )
#define MSGCOLOR_PP_RED     ( PRINTSYS_LSB_Make( 7,  8, 0 ) )
#define MSGCOLOR_PP_GREEN   ( PRINTSYS_LSB_Make( 9, 10, 0 ) )

//TCB_TransformStandby2Command
#define TTS2C_SCROLL_COUNT ( 8 )
#define TTS2C_START_SCALE ( FX32_ONE * 3 )
#define TTS2C_END_SCALE ( FX32_ONE )
#define TTS2C_START_SCROLL_X ( 0 )
#define TTS2C_START_SCROLL_Y ( 0x1c0 )
#define TTS2C_SCALE_SPEED ( ( FX32_ONE * 2 ) / TTS2C_SCROLL_COUNT )
#define TTS2C_SCROLL_SPEED ( 64 / TTS2C_SCROLL_COUNT )
#define TTS2C_FRAME1_SCROLL_X ( 0 )
#define TTS2C_FRAME1_SCROLL_Y ( 0 )

//TCB_TransformCommand2Waza
#define TTC2W_SCROLL_COUNT ( 8 )
#define TTC2W_START_SCROLL_X ( 256 )
#define TTC2W_START_SCROLL_Y ( 0x1c0 )
#define TTC2W_SCROLL_SPEED ( 64 / TTC2W_SCROLL_COUNT )

//TCB_TransformWaza2Command
#define TTW2C_SCROLL_COUNT ( 8 )
#define TTW2C_START_SCROLL_X ( 0 )
#define TTW2C_START_SCROLL_Y ( 0x1c0 )
#define TTW2C_SCROLL_SPEED ( 64 / TTW2C_SCROLL_COUNT )

//TCB_TransformCommand2Standby
#define TTC2S_SCALE_COUNT ( 8 )
#define TTC2S_SCALE_SPEED ( ( FX32_ONE * 2 ) / TTC2S_SCALE_COUNT )
#define TTC2S_START_SCALE ( FX32_ONE )
#define TTC2S_END_SCALE   ( FX32_ONE * 3 )

//TCB_TransformStandby2BattleRecorder
#define TTS2BR_FRAME0_SCROLL_X ( 256 )
#define TTS2BR_FRAME0_SCROLL_Y ( 256 )

#define STANBY_POS_Y  ( 128 + 24 )
#define COMMAND_POS_Y  ( 128 + 40 )

#define BG_NO_FRAME ( -1 )

//TCB_SCREEN_ANIME用のスクロール値
enum{
  TSA_SCROLL_X = 0,
  TSA_SCROLL_Y,

  TSA_SCROLL_X0 = 0,
  TSA_SCROLL_Y0 = 0,
  TSA_SCROLL_X1 = 256,
  TSA_SCROLL_Y1 = 0,
  TSA_SCROLL_X2 = 0,
  TSA_SCROLL_Y2 = 192,
  TSA_SCROLL_X3 = 256,
  TSA_SCROLL_Y3 = 192,

  TSA_WAIT = 2,
};

typedef enum
{
  SCREEN_ANIME_DIR_FORWARD = 0,
  SCREEN_ANIME_DIR_BACKWARD,
}SCREEN_ANIME_DIR;

//TCB_BUTTON〜系定義
enum{
  BUTTON_ANIME_MAX = BTLV_INPUT_DIR_MAX,     //3対3時の攻撃対象選択がマックスのはず

  BUTTON2_APPEAR_ANIME = 8,
  BUTTON2_VANISH_ANIME = 9,

  BUTTON4_APPEAR_ANIME = 8,
  BUTTON4_VANISH_ANIME = 9,

  BUTTON6_APPEAR_ANIME = 8,
  BUTTON6_VANISH_ANIME = 9,

  //BUTTON_TYPE_YES_NO
  BUTTON2_X1 = 64,
  BUTTON2_Y1 = 56,
  BUTTON2_X2 = 128 + 64,
  BUTTON2_Y2 = 56,

  //BUTTON_TYPE_WAZA & DIR_4
  BUTTON4_X1 = 64,
  BUTTON4_Y1 = 56,
  BUTTON4_X2 = 128 + 64,
  BUTTON4_Y2 = 56,
  BUTTON4_X3 = 64,
  BUTTON4_Y3 = 64 + 40,
  BUTTON4_X4 = 128 + 64,
  BUTTON4_Y4 = 64 + 40,

  //BUTTON_TYPE_DIR_6
  BUTTON6_X1 = 64,
  BUTTON6_Y1 = 56,
  BUTTON6_X2 = 128 + 64,
  BUTTON6_Y2 = 56,
  BUTTON6_X3 = 64,
  BUTTON6_Y3 = 64 + 40,
  BUTTON6_X4 = 128 + 64,
  BUTTON6_Y4 = 64 + 40,
  BUTTON6_X5 = 64,
  BUTTON6_Y5 = 64 + 40,
  BUTTON6_X6 = 128 + 64,
  BUTTON6_Y6 = 64 + 40,
};

typedef enum
{
  BUTTON_TYPE_WAZA = 0,
  BUTTON_TYPE_DIR_4 = 0,
  BUTTON_TYPE_DIR_6 = 1,
  BUTTON_TYPE_YES_NO = 2,
  BUTTON_TYPE_MAX = 3,
}BUTTON_TYPE;

typedef enum
{
  BUTTON_ANIME_TYPE_APPEAR = 0,
  BUTTON_ANIME_TYPE_VANISH,
  BUTTON_ANIME_TYPE_MAX,
}BUTTON_ANIME_TYPE;

typedef enum
{
  BG_VISIBLE_OFF = 0,
  BG_VISIBLE_ON,
  BG_VISIBLE_NO_SET,
}BG_VISIBLE;

//技選択ボタン表示位置定義
enum
{
  BUTTON_UP_Y1 = 8 + 2,
  BUTTON_UP_Y2 = 24 + 2,

  BUTTON_DOWN_Y1 = 56 + 2,
  BUTTON_DOWN_Y2 = 72 + 2,

  WAZANAME_X1 = 64,
  WAZANAME_Y1 = BUTTON_UP_Y1,
  WAZANAME_X2 = 128 + WAZANAME_X1,
  WAZANAME_Y2 = BUTTON_UP_Y1,
  WAZANAME_X3 = WAZANAME_X1,
  WAZANAME_Y3 = BUTTON_DOWN_Y1,
  WAZANAME_X4 = WAZANAME_X2,
  WAZANAME_Y4 = BUTTON_DOWN_Y1,

  PPMSG_X1 = 49,
  PPMSG_Y1 = BUTTON_UP_Y2,
  PPMSG_X2 = 128 + PPMSG_X1,
  PPMSG_Y2 = BUTTON_UP_Y2,
  PPMSG_X3 = PPMSG_X1,
  PPMSG_Y3 = BUTTON_DOWN_Y2,
  PPMSG_X4 = PPMSG_X2,
  PPMSG_Y4 = BUTTON_DOWN_Y2,

  PP_X1 = 90,
  PP_Y1 = BUTTON_UP_Y2,
  PP_X2 = 128 + PP_X1,
  PP_Y2 = BUTTON_UP_Y2,
  PP_X3 = PP_X1,
  PP_Y3 = BUTTON_DOWN_Y2,
  PP_X4 = PP_X2,
  PP_Y4 = BUTTON_DOWN_Y2,
};

//攻撃対象選択ボタン表示位置定義
enum
{
  //4ボタン
  BUTTON4_UP_Y = 20,

  BUTTON4_DOWN_Y = 68,

  MONSNAME4_X1 = 64,
  MONSNAME4_Y1 = BUTTON4_UP_Y,
  MONSNAME4_X2 = 128 + MONSNAME4_X1,
  MONSNAME4_Y2 = BUTTON4_UP_Y,
  MONSNAME4_X3 = MONSNAME4_X1,
  MONSNAME4_Y3 = BUTTON4_DOWN_Y,
  MONSNAME4_X4 = MONSNAME4_X2,
  MONSNAME4_Y4 = BUTTON4_DOWN_Y,

  MONSNAME6_X1 = 48,
  MONSNAME6_Y1 = BUTTON4_UP_Y,
  MONSNAME6_X2 = 128,
  MONSNAME6_Y2 = BUTTON4_UP_Y,
  MONSNAME6_X3 = 208,
  MONSNAME6_Y3 = BUTTON4_UP_Y,
  MONSNAME6_X4 = MONSNAME6_X1,
  MONSNAME6_Y4 = BUTTON4_DOWN_Y,
  MONSNAME6_X5 = MONSNAME6_X2,
  MONSNAME6_Y5 = BUTTON4_DOWN_Y,
  MONSNAME6_X6 = MONSNAME6_X3,
  MONSNAME6_Y6 = BUTTON4_DOWN_Y,

  DIR_PLTT     = 0x70,
};

//技タイプアイコンの表示座標
enum
{
  WAZATYPE_X1 = 32,
  WAZATYPE_Y1 = 64,
  WAZATYPE_X2 = 128 + WAZATYPE_X1,
  WAZATYPE_Y2 = WAZATYPE_Y1,
  WAZATYPE_X3 = WAZATYPE_X1,
  WAZATYPE_Y3 = 112,
  WAZATYPE_X4 = WAZATYPE_X2,
  WAZATYPE_Y4 = WAZATYPE_Y3,

  WAZATYPE_PLTT = 0x90,

};

ALIGN4  static  const GFL_CLACTPOS WazaIconPos[] =
{
  { WAZATYPE_X1, WAZATYPE_Y1 },
  { WAZATYPE_X2, WAZATYPE_Y2 },
  { WAZATYPE_X3, WAZATYPE_Y3 },
  { WAZATYPE_X4, WAZATYPE_Y4 },
};

//ボールゲージ座標定義
enum
{
  BTLV_INPUT_BALLGAUGE_ENEMY_X        = ( 128 + ( 8 * 3 ) - 4 ),
  BTLV_INPUT_BALLGAUGE_ENEMY_Y_CLOSE  = ( 8 * 6 ),                  //コマンド選択時
  BTLV_INPUT_BALLGAUGE_ENEMY_Y_OPEN   = ( 8 * 3 ),                  //コマンド選択時以外
  BTLV_INPUT_BALLGAUGE_MINE_X         = ( 128 - ( 16 * 3 ) + 8 ),
//  BTLV_INPUT_BALLGAUGE_MINE_Y_CLOSE   = ( 8 * 16 ),                 //コマンド選択時
//  BTLV_INPUT_BALLGAUGE_MINE_Y_OPEN    = ( 8 * 17 ),                 //コマンド選択時以外
  BTLV_INPUT_BALLGAUGE_MINE_Y_CLOSE   = ( 8 * 17 ),                 //コマンド選択時
  BTLV_INPUT_BALLGAUGE_MINE_Y_OPEN    = ( 8 * 17 ),                 //コマンド選択時以外

  BALL_GAUGE_MOVE_VALUE               = 4,  //ボールゲージ移動量
};

typedef enum
{
  BALL_GAUGE_TYPE_MINE = 0,
  BALL_GAUGE_TYPE_ENEMY,
  BALL_GAUGE_TYPE_MAX,
}BALL_GAUGE_TYPE;

typedef enum
{
  BALL_GAUGE_MOVE_OPEN = 0,
  BALL_GAUGE_MOVE_CLOSE,
}BALL_GAUGE_MOVE_DIR;

//カーソルOBJ定義
enum
{
  BTLV_INPUT_CURSOR_LU = 0,
  BTLV_INPUT_CURSOR_RU,
  BTLV_INPUT_CURSOR_LD,
  BTLV_INPUT_CURSOR_RD,
  BTLV_INPUT_CURSOR_LD_2,
  BTLV_INPUT_CURSOR_RD_2,
  BTLV_INPUT_CURSOR_MAX,

  BTLV_INPUT_CURSOR_LU_ANIME = 0,
  BTLV_INPUT_CURSOR_LD_ANIME,
  BTLV_INPUT_CURSOR_RU_ANIME,
  BTLV_INPUT_CURSOR_RD_ANIME,
};

//ポケモンアイコン定義
enum
{
  BTLV_INPUT_POKE_ICON_SINGLE_X = 128,
  BTLV_INPUT_POKE_ICON_SINGLE_Y = 112,  //128 + 12,
  BTLV_INPUT_POKE_ICON_DOUBLE_X1 = 128 - 24,
  BTLV_INPUT_POKE_ICON_DOUBLE_Y1 = BTLV_INPUT_POKE_ICON_SINGLE_Y,
  BTLV_INPUT_POKE_ICON_DOUBLE_X2 = 128 + 24,
  BTLV_INPUT_POKE_ICON_DOUBLE_Y2 = BTLV_INPUT_POKE_ICON_SINGLE_Y,
  BTLV_INPUT_POKE_ICON_TRIPLE_X1 = 128 - 40,
  BTLV_INPUT_POKE_ICON_TRIPLE_Y1 = BTLV_INPUT_POKE_ICON_SINGLE_Y,
  BTLV_INPUT_POKE_ICON_TRIPLE_X2 = BTLV_INPUT_POKE_ICON_SINGLE_X,
  BTLV_INPUT_POKE_ICON_TRIPLE_Y2 = BTLV_INPUT_POKE_ICON_SINGLE_Y,
  BTLV_INPUT_POKE_ICON_TRIPLE_X3 = 128 + 40,
  BTLV_INPUT_POKE_ICON_TRIPLE_Y3 = BTLV_INPUT_POKE_ICON_SINGLE_Y,
};

//天候アイコン定義
enum
{
  BTLV_INPUT_WEATHER_ICON_MAX = 2,

  BTLV_INPUT_WEATHER_BASE = 14,   ///< 天候アイコン土台
  BTLV_INPUT_WEATHER_SHINE,       ///< はれ
  BTLV_INPUT_WEATHER_RAIN,        ///< あめ
  BTLV_INPUT_WEATHER_SNOW,        ///< あられ
  BTLV_INPUT_WEATHER_SAND,        ///< すなあらし
  BTLV_INPUT_WEATHER_MIST,        ///< きり

  BTLV_INPUT_WEATHER_X1 = 256 + 48,
  BTLV_INPUT_WEATHER_X2 = 256 - 32,

  BTLV_INPUT_WEATHER_Y = 32,

  BTLV_INPUT_WEATHER_MOVE_SPEED = 8,
};

//わるあがきボタン
enum
{
  BTLV_INPUT_WARUAGAKI_MAX = 1,

  BTLV_INPUT_WARUAGAKI_X = 128,
  BTLV_INPUT_WARUAGAKI_Y =  80,

  BTLV_INPUT_WARUAGAKI_MSG_X = 56,
  BTLV_INPUT_WARUAGAKI_MSG_Y =  8,

  BTLV_INPUT_WARUAGAKI_ANM = 20,

  BTLV_INPUT_WARUAGAKI_L = 14,
  BTLV_INPUT_WARUAGAKI_S = 10,
  BTLV_INPUT_WARUAGAKI_B =  0,

  BTLV_INPUT_WARUAGAKI_SIZE_X = 14,
  BTLV_INPUT_WARUAGAKI_SIZE_Y =  4,

  BTLV_INPUT_WARUAGAKI_8x4_START = 0xcd,
  BTLV_INPUT_WARUAGAKI_4x4_START = 0xed,
  BTLV_INPUT_WARUAGAKI_2x4_START = 0xfd,
};

enum
{
  BTLV_INPUT_FADE_OUT = 1,
  BTLV_INPUT_FADE_IN,
};

enum
{
  BTLV_INPUT_YESNO_MSG_X  = 128,
  BTLV_INPUT_YES_MSG_Y    = 16,
  BTLV_INPUT_NO_MSG_Y     = 64,
};

enum
{
  ROTATE_MONSNAME_X0 = 64,
  ROTATE_MONSNAME_X1 = 192,
  ROTATE_MONSNAME_X2 = 128,
  ROTATE_MONSNAME_Y0 = 12,
  ROTATE_MONSNAME_Y1 = 12,
  ROTATE_MONSNAME_Y2 = 60,

  ROTATE_POKEICON_X0 = 64,
  ROTATE_POKEICON_X1 = 192,
  ROTATE_POKEICON_X2 = 128,
  ROTATE_POKEICON_Y0 = 76,
  ROTATE_POKEICON_Y1 = 76,
  ROTATE_POKEICON_Y2 = 120,

  ROTATE_FRAME = 8,

  ROTATE_SCROLL_COUNT = 8,
  ROTATE_SCROLL_SPEED = 64 / ROTATE_SCROLL_COUNT,

  ROTATE_LEFT_BUTTON  = 5,
  ROTATE_RIGHT_BUTTON = 6,
};

enum
{
  BR_PLAY_CHAPTER_X = 11 * 8 + 4,
  BR_PLAY_CHAPTER_Y =  1 * 8,
  BR_MAX_CHAPTER_X  = 17 * 8 + 4,
  BR_MAX_CHAPTER_Y  =  1 * 8,
  BR_SLASH_X        = 15 * 8 + 4,
  BR_SLASH_Y        =  1 * 8,
  BR_MESSAGE_X      =  3 * 8,
  BR_MESSAGE_Y      =  7 * 8,

  BUFLEN_BR_CHAPTER = ( 3 * GLOBAL_MSGLEN + BUFLEN_EOM_SIZE ),
};

static  const GFL_CLACTPOS pokeicon_pos[ BTLV_INPUT_POKEICON_MAX ] =
{
  { ROTATE_POKEICON_X0, ROTATE_POKEICON_Y0 },
  { ROTATE_POKEICON_X1, ROTATE_POKEICON_Y1 },
  { ROTATE_POKEICON_X2, ROTATE_POKEICON_Y2 },
};

static  const int rotate_pos_table[][ BTLV_INPUT_POKEICON_MAX ] = {
  { 0, 1, 2 },
  { 2, 0, 1 },
  { 1, 2, 0 },
};

#define CURSOR_NOMOVE ( 0x0f )    //カーソルが一度も動いていないときのold_cursor_posの数値

static  const GFL_CLWK_DATA obj_param =
{
  0, 0,   //x, y
  0, 0, 0,  //アニメ番号、優先順位、BGプライオリティ
};

enum
{
  FINGER_CURSOR_LOOP  = 2,
  FINGER_CURSOR_FRAME = 7,
  FINGER_CURSOR_WAIT  = 16,
};

enum
{
  BTLV_INPUT_SHOOTER_ENERGY_X = 24 / 2,
  BTLV_INPUT_SHOOTER_ENERGY_Y = 0,
};

//通信アイコン定義
enum
{
  BTLV_INPUT_WIRELESS_ICON_PAL = 14 * 0x10,
};

//============================================================================================
/**
 *  構造体宣言
 */
//============================================================================================

typedef struct
{
  GFL_CLWK*   clwk;
  s16         pos_x;
  s16         pos_y;
}BTLV_INPUT_CLWK;

struct _BTLV_INPUT_WORK
{
  GFL_TCBSYS*                     tcbsys;
  void*                           tcbwork;
  GFL_TCB*                        tcb[ BTLV_INPUT_TCB_MAX ];
  BTLV_INPUT_TCB_CALLBACK_FUNC*   tcb_callback[ BTLV_INPUT_TCB_MAX ];
  ARCHANDLE*            handle;
  GAMEDATA*             gameData;
  BTLV_INPUT_TYPE       type;
  BtlCompetitor         comp;
  BTLV_INPUT_SCRTYPE    scr_type;
  GFL_MSGDATA*          msg;
  BTLV_FINGER_CURSOR_WORK*  bfcw;
  PALETTE_FADE_PTR      pfd;
  u32                   tcb_execute_flag    :1;
  u32                   tcb_execute_count   :3;
  u32                   center_button_type  :1;
  u32                   cursor_pos          :4;
  u32                   old_cursor_pos      :4;
  u32                   cursor_decide       :1;
  u32                   trainer_flag        :1;
  u32                   fade_flag           :2;
  u32                   rotate_flag         :2;
  u32                   rotate_scr          :2;
  u32                   before_select_dir   :2;
  u32                   seq_no              :4;
  u32                   demo_cursor_pos     :4;
  u32                   button_reaction     :1;

  u32                   b_button_flag       :1;
  u32                   decide_pos_flag     :1;     //決定ボタン位置記憶フラグ
  u32                   waza_target         :8;     ///<対象選択タイプ
  u32                   waza_cursor_pos     :8;     ///<技選択位置記憶ワーク
  u32                                       :14;

  int                   hit;

  BTLV_INPUT_DIR_PARAM  bidp[ 2 ][ BTLV_INPUT_DIR_MAX ];

  //OBJリソース
  u32                   objcharID;
  u32                   objplttID;
  u32                   objcellID;

  //カーソル
  u32                   cur_charID;
  u32                   cur_cellID;

  //ボールゲージOBJ
  GFL_CLUNIT*           ballgauge_clunit;
  BTLV_INPUT_CLWK       ballgauge_mine[ TEMOTI_POKEMAX ];
  BTLV_INPUT_CLWK       ballgauge_enemy[ TEMOTI_POKEMAX ];

  //カーソルOBJ
  GFL_CLUNIT*           cursor_clunit;
  BTLV_INPUT_CLWK       cursor[ BTLV_INPUT_CURSOR_MAX ];

  //ポケモンアイコン
  u32                   pokeicon_charID[ BTLV_INPUT_POKEICON_MAX ];
  u32                   pokeicon_plttID;
  u32                   pokeicon_cellID;
  GFL_CLUNIT*           pokeicon_clunit;
  BTLV_INPUT_CLWK       pokeicon_wk[ BTLV_INPUT_POKEICON_MAX ];

  //技タイプアイコンOBJ
  u32                   wazatype_charID[ PTL_WAZA_MAX ];
  u32                   wazatype_plttID;
  u32                   wazatype_cellID;
  GFL_CLUNIT*           wazatype_clunit;
  GFL_CLWK*             wazatype_wk[ PTL_WAZA_MAX ];

  //天候アイコン
  GFL_CLUNIT*           weather_clunit;
  GFL_CLWK*             weather_wk[ BTLV_INPUT_WEATHER_ICON_MAX ];

  //わるあがきボタン
  GFL_CLUNIT*           waruagaki_clunit;
  GFL_CLWK*             waruagaki_wk;
  BOOL                  waruagaki_flag;

  //フォント
  GFL_FONT*             font;

  //BMP
  GFL_BMPWIN*           bmp_win;
  GFL_BMP_DATA*         bmp_data;
  GFL_BMPWIN*           bmp_win_shooter;    //シューターエネルギー表示用BMPWIN
  GFL_BMP_DATA*         bmp_data_shooter;

  //メインループTCB
  GFL_TCB*              main_loop;      //scdにメインループが存在しないのでBTLV_EFFECTのTCBを間借りしてメインを回す
  BOOL                  main_loop_tcb_flag;

  HEAPID                heapID;

  u8                    decide_pos[ BTLV_INPUT_POKEMON_MAX ][ BTLV_INPUT_SCRTYPE_MAX ];   //Aボタンで決定したボタン位置を記憶
  u8                    button_exist[ BTLV_INPUT_BUTTON_MAX ];  //押せるボタンかどうかチェック
  u8                    waza_exist[ PTL_WAZA_MAX ];             //技Infoに移行できるボタンかどうかチェック
  WazaID                decide_waza[ BTLV_INPUT_POKEMON_MAX ];
  WazaID                waza[ PTL_WAZA_MAX ];

  u8                    *cursor_mode;

  BtlvMcssPos           focus_pos;          //フォーカスをあわせるポケモンの位置
  int                   active_index;       //コマンド選択しているポケモンのインデックス
  BOOL                  henshin_flag;

  u16                   bag_pal[ 0x10 ];    //バッグボタンパレットを退避しておく

  //ローテーション用POKEMON_PARAM
#ifdef ROTATION_NEW_SYSTEM
  const BTL_POKEPARAM*  rotate_bpp[ BTLV_INPUT_POKEICON_MAX ];
  BOOL                  rotate_waza_exist[ BTL_ROTATE_NUM ][PTL_WAZA_MAX ];
#else
  const POKEMON_PARAM*  rotate_pp[ BTLV_INPUT_POKEICON_MAX ];
#endif

#ifdef PM_DEBUG
  BOOL                  timer_edit_flag;
#endif
};

typedef struct
{
  BTLV_INPUT_WORK*    biw;
  int                 seq_no;
  BtlvMcssPos         pos;
  ARCDATID            datID;
  int                 scr_x;
  int                 scr_y;
  int                 wait;
  STRBUF*             msg_src;
  BTLV_INPUT_SCRTYPE  scr_type;
}TCB_TRANSFORM_WORK;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  fx32              start_scale;
  fx32              end_scale;
  fx32              scale_speed;
  int               pos_y;
}TCB_SCALE_UP;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               seq_no;
  int               scroll_x;
  int               scroll_y;
  int               scroll_speed;
  int               scroll_count;
}TCB_SCROLL_UP;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               count;
  SCREEN_ANIME_DIR  dir;
  int               wait;
}TCB_SCREEN_ANIME;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               frame;
  int               scr_x;
  int               scr_y;
  BG_VISIBLE        frame0;
  BG_VISIBLE        frame1;
  BG_VISIBLE        frame2;
  BG_VISIBLE        frame3;
}TCB_SET_SCROLL;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  GFL_CLUNIT*       clunit;
  GFL_CLWK*         clwk[ BUTTON_ANIME_MAX ];
}TCB_BUTTON_ANIME;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               move_dir;    //移動方向
}TCB_BALL_GAUGE_MOVE;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
}TCB_FADE_ACT;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
}TCB_WEATHER_ICON_MOVE;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               seq_no;
  VecFx32           now_pos[ BTLV_INPUT_POKEICON_MAX ];
  VecFx32           end_pos[ BTLV_INPUT_POKEICON_MAX ];
  VecFx32           vector[ BTLV_INPUT_POKEICON_MAX ];
  int               frame;
}TCB_ROTATE_ACTION;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               seq_no;
  int               pltt;
  BOOL              waruagaki;
}TCB_BUTTON_REACTION;

typedef struct
{
  const GFL_CLACTPOS  pos[ BUTTON_ANIME_MAX ];          //座標
  int                 anm_no[ BUTTON_ANIME_TYPE_MAX ];  //アニメーションナンバー
}BUTTON_ANIME_PARAM;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  BtlBagMode        bagMode;
}TCB_BAGMODE_SET;

typedef struct
{
  BTLV_INPUT_WORK*  biw;
  int               max;
}TCB_WINDOW_MASK;

//============================================================================================
/**
 *  プロトタイプ宣言
 */
//============================================================================================
static  BTLV_INPUT_WORK*  BTLV_INPUT_InitCore( GAMEDATA* gameData, BTLV_INPUT_TYPE type, BtlCompetitor comp,
                                               PALETTE_FADE_PTR pfd, GFL_FONT* font, u8* cursor_flag, BtlBagMode bagMode,
                                               BOOL main_loop_tcb_flag, HEAPID heapID );
static  void  BTLV_INPUT_LoadResource( BTLV_INPUT_WORK* biw );
static  void  TCB_TransformStandby2Command( GFL_TCB* tcb, void* work );
static  void  TCB_TransformStandby2Waza( GFL_TCB* tcb, void* work );
static  void  TCB_TransformCommand2Waza( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Command( GFL_TCB* tcb, void* work );
static  void  TCB_TransformRotate2Command( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Dir( GFL_TCB* tcb, void* work );
static  void  TCB_TransformDir2Waza( GFL_TCB* tcb, void* work );
static  void  TCB_TransformCommand2Standby( GFL_TCB* tcb, void* work );
static  void  TCB_TransformWaza2Standby( GFL_TCB* tcb, void* work );
static  void  TCB_TransformStandby2YesNo( GFL_TCB* tcb, void* work );
static  void  TCB_TransformStandby2Rotate( GFL_TCB* tcb, void* work );
static  void  TCB_TransformStandby2BattleRecorder( GFL_TCB* tcb, void* work );
static  void  TCB_TransformStandby2PDC( GFL_TCB* tcb, void* work );
static  void  TCB_TransformCommand2Rotate( GFL_TCB* tcb, void* work );
static  void  TCB_TransformRotate2Rotate( GFL_TCB* tcb, void* work );
static  void  TCB_Transform_CB( GFL_TCB* tcb );
static  void  TCB_SetFocus( GFL_TCB* tcb, void* work );

static  void  SetupScaleChange( BTLV_INPUT_WORK* biw, fx32 start_scale, fx32 end_scale, fx32 scale_speed, int pos_y );
static  void  TCB_ScaleChange( GFL_TCB* tcb, void* work );
static  void  TCB_ScaleChange_CB( GFL_TCB* tcb );
static  void  SetupScrollUp( BTLV_INPUT_WORK* biw, int scroll_x, int scroll_y, int scroll_speed, int scroll_count );
static  void  TCB_ScrollUp( GFL_TCB* tcb, void* work );
static  void  TCB_ScrollUp_CB( GFL_TCB* tcb );
static  void  SetupScreenAnime( BTLV_INPUT_WORK* biw, int index, SCREEN_ANIME_DIR dir );
static  void  TCB_ScreenAnime( GFL_TCB* tcb, void* work );
static  void  TCB_ScreenAnime_CB( GFL_TCB* tcb );
static  void  SetupButtonAnime( BTLV_INPUT_WORK* biw, BUTTON_TYPE type, BUTTON_ANIME_TYPE anm_type );
static  void  TCB_ButtonAnime( GFL_TCB* tcb, void* work );
static  void  TCB_ButtonAnime_CB( GFL_TCB* tcb );
static  void  SetupBallGaugeMove( BTLV_INPUT_WORK* biw, BALL_GAUGE_MOVE_DIR dir );
static  void  TCB_BallGaugeMove( GFL_TCB* tcb, void* work );
static  void  TCB_BallGaugeMove_CB( GFL_TCB* tcb );
static  void  SetupSetScroll( BTLV_INPUT_WORK* biw, int frame, int scr_x, int scr_y,
                              BG_VISIBLE frame0, BG_VISIBLE frame1, BG_VISIBLE frame2, BG_VISIBLE frame3 );
static  void  TCB_SetScroll( GFL_TCB* tcb, void* work );
static  void  TCB_Fade( GFL_TCB* tcb, void* work );
static  void  TCB_Fade_CB( GFL_TCB* tcb );
static  void  TCB_WeatherIconMove( GFL_TCB* tcb, void* work );
static  void  TCB_WeatherIconMove_CB( GFL_TCB* tcb );
static  void  SetupRotateAction( BTLV_INPUT_WORK* biw, int dir );
static  void  TCB_RotateAction( GFL_TCB* tcb, void* work );
static  void  TCB_BagModeSet( GFL_TCB* tcb, void* work );
static  void  TCB_SetWindowMask( GFL_TCB* tcb, void* work );
static  void  TCB_ClearWindowMask( GFL_TCB* tcb, void* work );

static  void  BTLV_INPUT_MainTCB( GFL_TCB* tcb, void* work );
static  void  FontLenGet( const STRBUF *str, GFL_FONT *font, int *ret_dot_len, int *ret_char_len );
static  void  BTLV_INPUT_CreateWazaScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_WAZA_PARAM *biwp );
static  void  BTLV_INPUT_CreateDirScreen( BTLV_INPUT_WORK* biw, TCB_TRANSFORM_WORK* ttw, const BTLV_INPUT_SCENE_PARAM *bisp );
static  void  BTLV_INPUT_CreateYesNoScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_YESNO_PARAM *biyp );
static  void  BTLV_INPUT_CreateRotateScreen( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_CreateRotatePokeIcon( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_CreateBattleRecorderScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_BATTLE_RECORDER_PARAM *bibrp );
static  void  BTLV_INPUT_ClearScreen( BTLV_INPUT_WORK* biw );
static  PRINTSYS_LSB  PP_FontColorGet( int pp, int pp_max );
static  void  BTLV_INPUT_CreatePokeIcon( BTLV_INPUT_WORK* biw, BTLV_INPUT_COMMAND_PARAM* bicp );
static  void  BTLV_INPUT_DeletePokeIcon( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_SetDrawEnablePokeIcon( BTLV_INPUT_WORK* biw, BOOL flag );
static  void  BTLV_INPUT_CreateWeatherIcon( BTLV_INPUT_WORK* biw, BtlWeather btl_weather );
static  void  BTLV_INPUT_DeleteWeatherIcon( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_CreateBallGauge( BTLV_INPUT_WORK* biw, BALL_GAUGE_TYPE type );
static  void  BTLV_INPUT_DeleteBallGauge( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_CreateCursorOBJ( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_DeleteCursorOBJ( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_DeleteWazaTypeIcon( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_CreateWaruagakiButton( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_DeleteWaruagakiButton( BTLV_INPUT_WORK* biw );
static  int   BTLV_INPUT_CheckKey( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tp_tbl, const BTLV_INPUT_KEYTBL* key_tbl, const u8* move_tbl, int hit, BOOL henshin_flag );
static  int   BTLV_INPUT_CheckKeyBR( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_PutCursorOBJ( BTLV_INPUT_WORK* biw, const GFL_UI_TP_HITTBL* tp_tbl, const BTLV_INPUT_KEYTBL* key_tbl );
static  int   BTLV_INPUT_SetButtonReaction( BTLV_INPUT_WORK* biw, int hit, int pltt );
static  void  TCB_ButtonReaction( GFL_TCB* tcb, void* work );
static  void  TCB_ButtonReaction_CB( GFL_TCB* tcb );
static  void  BTLV_INPUT_PutShooterEnergy( BTLV_INPUT_WORK* biw, BTLV_INPUT_COMMAND_PARAM* bicp );
static  void  BTLV_INPUT_SetFocus( BTLV_INPUT_WORK* biw );
static  void  check_init_decide_pos( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_SetTCB( BTLV_INPUT_WORK* biw, GFL_TCB* tcb, BTLV_INPUT_TCB_CALLBACK_FUNC* callback_func );
static  int   BTLV_INPUT_SearchTCBIndex( BTLV_INPUT_WORK* biw, GFL_TCB* tcb );
static  void  BTLV_INPUT_FreeTCB( BTLV_INPUT_WORK* biw, GFL_TCB* tcb );
static  int   BTLV_INPUT_GetTCBIndex( BTLV_INPUT_WORK* biw );
static  void  BTLV_INPUT_FreeTCBAll( BTLV_INPUT_WORK* biw );
static  BOOL  get_cancel_flag( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tbl, int pos );
static  void  set_cursor_pos( BTLV_INPUT_WORK* biw );
static  void  change_bag_button_pal( BTLV_INPUT_WORK* biw );
static  void  pop_bag_button_pal( BTLV_INPUT_WORK* biw );
static  ARCDATID  get_command_screen( BTLV_INPUT_WORK* biw );

static  void  SePlayOpen( BTLV_INPUT_WORK* biw );
static  void  SePlaySelect( BTLV_INPUT_WORK* biw );
static  void  SePlayDecide( BTLV_INPUT_WORK* biw );
static  void  SePlayCancel( BTLV_INPUT_WORK* biw );
static  void  SePlayRotateSelect( BTLV_INPUT_WORK* biw );
static  void  SePlayRotateDecide( BTLV_INPUT_WORK* biw );
static  void  SePlayRotation( BTLV_INPUT_WORK* biw );
static  void  SePlayBeep( BTLV_INPUT_WORK* biw );

static  void  BTLV_INPUT_VBlank( GFL_TCB *tcb, void *work );

#ifdef PM_DEBUG
static  void  debug_timer_edit_check( BTLV_INPUT_WORK* biw );
#endif

//============================================================================================
/**
 *  @brief  システム初期化（汎用）
 *
 *  @param[in]  gameData      GAMEDATA構造体
 *  @param[in]  type          インターフェースタイプ
 *  @param[in]  pfd           パレットフェード管理構造体ポインタ
 *  @param[in]  font          使用するフォント
 *  @param[in]  cursor_flag   カーソル表示するかどうかフラグのポインタ（他のアプリとも共用するため）
 *  @param[in]  heapID        ヒープID
 *
 *  @retval システム管理構造体のポインタ
 */
//============================================================================================
BTLV_INPUT_WORK*  BTLV_INPUT_InitEx( GAMEDATA* gameData, BTLV_INPUT_TYPE type, PALETTE_FADE_PTR pfd, GFL_FONT* font, u8* cursor_flag, HEAPID heapID )
{
  return BTLV_INPUT_InitCore( gameData, type, BTL_COMPETITOR_WILD, pfd, font, cursor_flag, BBAG_MODE_NORMAL, FALSE, heapID );
}

//============================================================================================
/**
 *  @brief  システム初期化（戦闘専用）
 *
 *  @param[in]  gameData      GAMEDATA構造体
 *  @param[in]  type          インターフェースタイプ
 *  @param[in]  comp          対戦相手（野生orトレーナーor通信）
 *  @param[in]  pfd           パレットフェード管理構造体ポインタ
 *  @param[in]  font          使用するフォント
 *  @param[in]  cursor_flag   カーソル表示するかどうかフラグのポインタ（他のアプリとも共用するため）
 *  @param[in]  bagMdoe       バッグモード
 *  @param[in]  heapID        ヒープID
 *
 *  @retval システム管理構造体のポインタ
 */
//============================================================================================
BTLV_INPUT_WORK*  BTLV_INPUT_Init( GAMEDATA* gameData, BTLV_INPUT_TYPE type, BtlCompetitor comp, PALETTE_FADE_PTR pfd, GFL_FONT* font, u8* cursor_flag, BtlBagMode bagMode, HEAPID heapID )
{
  return BTLV_INPUT_InitCore( gameData, type, comp, pfd, font, cursor_flag, bagMode, TRUE, heapID );
}

//============================================================================================
/**
 *  @brief  システム初期化コア
 *
 *  @param[in]  gameData      GAMEDATA構造体
 *  @param[in]  type          インターフェースタイプ
 *  @param[in]  comp          対戦相手（野生orトレーナーor通信）
 *  @param[in]  pfd           パレットフェード管理構造体ポインタ
 *  @param[in]  font          使用するフォント
 *  @param[in]  cursor_flag   カーソル表示するかどうかフラグのポインタ（他のアプリとも共用するため）
 *  @param[in]  bagMdoe       バッグモード
 *  @param[in]  heapID        ヒープID
 *
 *  @retval システム管理構造体のポインタ
 */
//============================================================================================
static  BTLV_INPUT_WORK*  BTLV_INPUT_InitCore( GAMEDATA* gameData, BTLV_INPUT_TYPE type, BtlCompetitor comp,
                                               PALETTE_FADE_PTR pfd, GFL_FONT* font, u8* cursor_flag, BtlBagMode bagMode,
                                               BOOL main_loop_tcb_flag, HEAPID heapID )
{
  BTLV_INPUT_WORK *biw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_INPUT_WORK ) );

  biw->heapID = heapID;

  biw->tcbwork  = GFL_HEAP_AllocClearMemory( biw->heapID, GFL_TCB_CalcSystemWorkSize( BTLV_INPUT_TCB_MAX ) );
  biw->tcbsys   = GFL_TCB_Init( BTLV_INPUT_TCB_MAX, biw->tcbwork );

  biw->gameData = gameData;
  biw->font     = font;
  biw->type     = type;
  biw->comp     = comp;

  biw->cursor_mode = cursor_flag;

  biw->old_cursor_pos = CURSOR_NOMOVE;

  biw->main_loop_tcb_flag = main_loop_tcb_flag;

  //パレットフェード初期化
  biw->pfd = pfd;

  {
    int i;

    biw->objcharID        = GFL_CLGRP_REGISTER_FAILED;
    biw->objplttID        = GFL_CLGRP_REGISTER_FAILED;
    biw->objcellID        = GFL_CLGRP_REGISTER_FAILED;
    biw->cur_charID       = GFL_CLGRP_REGISTER_FAILED;
    biw->cur_cellID       = GFL_CLGRP_REGISTER_FAILED;
    biw->pokeicon_plttID  = GFL_CLGRP_REGISTER_FAILED;
    biw->pokeicon_cellID  = GFL_CLGRP_REGISTER_FAILED;
    biw->wazatype_plttID  = GFL_CLGRP_REGISTER_FAILED;
    biw->wazatype_cellID  = GFL_CLGRP_REGISTER_FAILED;
    for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
    {
      biw->pokeicon_charID[ i ] = GFL_CLGRP_REGISTER_FAILED;
    }
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      biw->wazatype_charID[ i ] = GFL_CLGRP_REGISTER_FAILED;
    }
  }

  biw->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_btlv_input_dat, biw->heapID );

  biw->hit = GFL_UI_TP_HIT_NONE;

  BTLV_INPUT_InitBG( biw );

  {
    TCB_BAGMODE_SET* tbs = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_BAGMODE_SET ) );
    tbs->biw      = biw;
    tbs->bagMode  = bagMode;
    BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_BagModeSet, tbs, 0 ), NULL );
  }

  return biw;
}

//============================================================================================
/**
 *  @brief  システム終了
 *
 *  @param[in]  biw システム管理構造体のポインタ
 */
//============================================================================================
void  BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw )
{
  if( biw == NULL ) return;

  BTLV_INPUT_FreeTCBAll( biw );
  BTLV_INPUT_ExitBG( biw );

  GFL_MSG_Delete( biw->msg );

  if( biw->bfcw )
  {
    BTLV_FINGER_CURSOR_Exit( biw->bfcw );
  }

  GFL_TCB_Exit( biw->tcbsys );
  GFL_HEAP_FreeMemory( biw->tcbwork );
  GFL_HEAP_FreeMemory( biw );
  biw = NULL;
}

//============================================================================================
/**
 *  @brief  システムメインループ
 *
 *  @param[in]  biw システム管理構造体のポインタ
 */
//============================================================================================
void  BTLV_INPUT_Main( BTLV_INPUT_WORK* biw )
{
  GFL_TCB_Main( biw->tcbsys );
  INFOWIN_Update();
}

//============================================================================================
/**
 *  @brief  システムメインループ（TCB版）
 *
 *  @param[in]  biw システム管理構造体のポインタ
 */
//============================================================================================
static  void  BTLV_INPUT_MainTCB( GFL_TCB* tcb, void* work )
{
  BTLV_INPUT_WORK* biw = (BTLV_INPUT_WORK *)work;
  GFL_TCB_Main( biw->tcbsys );
  INFOWIN_Update();
}

//============================================================================================
/**
 *  @brief  BG画面初期化
 *
 *  @param[in]  biw システム管理構造体のポインタ
 */
//============================================================================================
void  BTLV_INPUT_InitBG( BTLV_INPUT_WORK *biw )
{
  //ＢＧモード設定
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGModeDisp( &sysHeader, GFL_BG_SUB_DISP );
  }
  biw->handle           = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, biw->heapID );
  biw->wazatype_clunit  = GFL_CLACT_UNIT_Create( PTL_WAZA_MAX, 2, biw->heapID );
  biw->ballgauge_clunit = GFL_CLACT_UNIT_Create( TEMOTI_POKEMAX * 2, 2, biw->heapID );
  biw->cursor_clunit    = GFL_CLACT_UNIT_Create( BTLV_INPUT_CURSOR_MAX, 0, biw->heapID );
  biw->pokeicon_clunit  = GFL_CLACT_UNIT_Create( BTLV_INPUT_POKEICON_MAX, 2, biw->heapID );
  biw->weather_clunit   = GFL_CLACT_UNIT_Create( BTLV_INPUT_WEATHER_ICON_MAX, 2, biw->heapID );
  biw->waruagaki_clunit = GFL_CLACT_UNIT_Create( BTLV_INPUT_WARUAGAKI_MAX, 1, biw->heapID );

  BTLV_INPUT_SetFrame( biw );
  BTLV_INPUT_LoadResource( biw );

  BTLV_INPUT_CreateCursorOBJ( biw );

  //ビットマップ初期化
  biw->bmp_win = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 0, 4, 32, 12, BMPWIN_PAL_NO, GFL_BMP_CHRAREA_GET_B );
  biw->bmp_data = GFL_BMPWIN_GetBmp( biw->bmp_win );
  biw->bmp_win_shooter = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 3, 22, 3, 2, BMPWIN_PAL_NO, GFL_BMP_CHRAREA_GET_B );
  biw->bmp_data_shooter = GFL_BMPWIN_GetBmp( biw->bmp_win_shooter );
  GFL_BMP_Clear( biw->bmp_data, 0x00 );
  GFL_BMP_Clear( biw->bmp_data_shooter, 0x00 );
  GFL_BMPWIN_TransVramCharacter( biw->bmp_win );
  GFL_BMPWIN_TransVramCharacter( biw->bmp_win_shooter );
  GFL_BMPWIN_MakeScreen( biw->bmp_win );
  GFL_BMPWIN_MakeScreen( biw->bmp_win_shooter );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );

  //情報ステータスバー初期化
  INFOWIN_Init( GFL_BG_FRAME2_S, INFOWIN_PAL_NO, biw->gameData , biw->heapID );
  //情報ステータスバー表示
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );

  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  //受信強度アイコン表示
  GFL_NET_ReloadIconTopOrBottom( FALSE, biw->heapID );
  PaletteWorkSet_VramCopy( biw->pfd, FADE_SUB_OBJ, BTLV_INPUT_WIRELESS_ICON_PAL, 0x20 );

  //OBJカーソル表示処理をする
  biw->cursor_decide = 1;

  //メインループはTCBで行う
  if( biw->main_loop_tcb_flag == TRUE )
  {
    biw->main_loop = GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), BTLV_INPUT_MainTCB, biw, 0 );
  }

  BTLV_INPUT_CreateScreen( biw, BTLV_INPUT_SCRTYPE_STANDBY, NULL );
}

//============================================================================================
/**
 *  @brief  BG画面終了
 *
 *  @param[in]  biw システム管理構造体のポインタ
 */
//============================================================================================
void  BTLV_INPUT_ExitBG( BTLV_INPUT_WORK *biw )
{
  BTLV_INPUT_DeleteWazaTypeIcon( biw );
  BTLV_INPUT_DeleteBallGauge( biw );
  BTLV_INPUT_DeleteCursorOBJ( biw );
  BTLV_INPUT_DeletePokeIcon( biw );
  BTLV_INPUT_DeleteWeatherIcon( biw );

  if( biw->objcharID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CGR_Release( biw->objcharID );
    biw->objcharID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->objplttID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_PLTT_Release( biw->objplttID );
    biw->objplttID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->objcellID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CELLANIM_Release( biw->objcellID );
    biw->objcellID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->cur_charID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CGR_Release( biw->cur_charID );
    biw->cur_charID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->cur_cellID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CELLANIM_Release( biw->cur_cellID );
    biw->cur_cellID = GFL_CLGRP_REGISTER_FAILED;
  }

  {
    int i;

    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      if( biw->wazatype_charID[ i ] != GFL_CLGRP_REGISTER_FAILED )
      {
        GFL_CLGRP_CGR_Release( biw->wazatype_charID[ i ] );
        biw->wazatype_charID[ i ] = GFL_CLGRP_REGISTER_FAILED;
      }
    }
  }

  if( biw->wazatype_cellID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CELLANIM_Release( biw->wazatype_cellID );
    biw->wazatype_cellID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->wazatype_plttID  != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_PLTT_Release( biw->wazatype_plttID );
    biw->wazatype_plttID = GFL_CLGRP_REGISTER_FAILED;
  }

  if( biw->wazatype_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->wazatype_clunit );
    biw->wazatype_clunit = NULL;
  }
  if( biw->ballgauge_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->ballgauge_clunit );
    biw->ballgauge_clunit = NULL;
  }
  if( biw->cursor_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->cursor_clunit );
    biw->cursor_clunit = NULL;
  }
  if( biw->pokeicon_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->pokeicon_clunit );
    biw->pokeicon_clunit = NULL;
  }
  if( biw->weather_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->weather_clunit );
    biw->weather_clunit = NULL;
  }
  if( biw->waruagaki_clunit )
  {
    GFL_CLACT_UNIT_Delete( biw->waruagaki_clunit );
    biw->waruagaki_clunit = NULL;
  }

  if( biw->bmp_win )
  {
    GFL_BMPWIN_Delete( biw->bmp_win );
    biw->bmp_win = NULL;
  }
  if( biw->bmp_win_shooter )
  {
    GFL_BMPWIN_Delete( biw->bmp_win_shooter );
    biw->bmp_win_shooter = NULL;
  }

  INFOWIN_Exit();

  BTLV_INPUT_FreeFrame();

  if( biw->main_loop )
  {
    GFL_TCB_DeleteTask( biw->main_loop );
    biw->main_loop = NULL;
  }

  if( biw->handle )
  {
    GFL_ARC_CloseDataHandle( biw->handle );
    biw->handle = NULL;
  }

  biw->scr_type = BTLV_INPUT_SCRTYPE_STANDBY;
}

//============================================================================================
/**
 *  @brief  下画面BGフレーム設定
 */
//============================================================================================
void BTLV_INPUT_SetFrame( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < NELEMS( bibf ) ; i++ )
  {
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S + i, &bibf[ i ].bgcnt_table, bibf[ i ].bgcnt_mode );
    GFL_BG_ClearScreenCode( GFL_BG_FRAME0_S + i, BG_CLEAR_CODE );
    //GFL_BG_SetScroll( GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_X_SET, 0 );
    //GFL_BG_SetScroll( GFL_BG_FRAME0_S + i, GFL_BG_SCROLL_Y_SET, 0 );
    SetupSetScroll( biw, GFL_BG_FRAME0_S + i, 0, 0, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
  }
}

//============================================================================================
/**
 *  @brief  下画面BGフレーム設定解放
 *
 *  @param[in]  biw   システム管理構造体のポインタ
 */
//============================================================================================
void BTLV_INPUT_FreeFrame( void )
{
  int i;

  for( i = 0 ; i < NELEMS( bibf ) ; i++ ){
    GFL_BG_SetVisible( GFL_BG_FRAME0_S + i, VISIBLE_OFF );
    GFL_BG_FreeBGControl( GFL_BG_FRAME0_S + i );
  }
}

//============================================================================================
/**
 *  @brief  下画面フェードアウトリクエスト
 *
 *  @param[in]  biw   システム管理構造体のポインタ
 */
//============================================================================================
void BTLV_INPUT_SetFadeOut( BTLV_INPUT_WORK* biw, BOOL start )
{
  TCB_FADE_ACT* tfa = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_FADE_ACT ) );

  if( start == TRUE ){
    PaletteFadeReq( biw->pfd, PF_BIT_SUB_ALL, 0xffff, 1, 0, 16, 0, biw->tcbsys );
  }else{
    PaletteFadeReq( biw->pfd, PF_BIT_SUB_ALL, 0xffff, 1, STANDBY_FADE, 16, 0, biw->tcbsys );
  }
  biw->fade_flag = BTLV_INPUT_FADE_OUT;

  tfa->biw           = biw;

  BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_Fade, tfa, 0 ), TCB_Fade_CB );
}

//============================================================================================
/**
 *  @brief  下画面フェードインリクエスト
 */
//============================================================================================
void BTLV_INPUT_SetFadeIn( BTLV_INPUT_WORK* biw )
{
  TCB_FADE_ACT* tfa = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_FADE_ACT ) );

  if( GXS_GetMasterBrightness() <= 0 ){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0 );
  }else{
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB, 16, 0, 0 );
  }

  BTLV_INPUT_InitBG( biw );

  biw->fade_flag = BTLV_INPUT_FADE_IN;

  tfa->biw           = biw;

  BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_Fade, tfa, 0 ), TCB_Fade_CB );
}

//============================================================================================
/**
 *  @brief  下画面フェードチェック
 *
 *  @param[in]  biw   システム管理構造体のポインタ
 *
 *  @retval TRUE:フェード中 FALSE:フェード終了
 */
//============================================================================================
BOOL BTLV_INPUT_CheckFadeExecute( BTLV_INPUT_WORK* biw )
{
  return ( biw->fade_flag != 0 );
}

//============================================================================================
/**
 *  @brief  下画面生成
 *
 *  @param[in]  biw   システム管理構造体のポインタ
 *  @param[in]  type  生成するスクリーンタイプ
 *  @param[in]  param 生成に必要なパラメータ構造体のポインタ
 */
//============================================================================================
void BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type, void* param )
{
  BTLV_INPUT_ClearScreen( biw );

  biw->seq_no = 0;
  biw->b_button_flag = TRUE;

  switch( type ){
  case BTLV_INPUT_SCRTYPE_STANDBY:
    biw->hit = GFL_UI_TP_HIT_NONE;
    if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
    {
      SetupScaleChange( biw, FX32_ONE * 3, FX32_ONE * 3, 0, 128 + 24 );
      //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
      //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
      //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
      SetupSetScroll( biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_OFF, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
      PaletteFadeReq( biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, STANDBY_FADE, STANDBY_FADE_COLOR, biw->tcbsys );
    }
    else
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;

#if 1
      if( biw->main_loop_tcb_flag == TRUE )
      {
        BTLV_EFFECT_SetCameraWorkStop();
      }
#endif

      BTLV_INPUT_DeleteBallGauge( biw );
      BTLV_INPUT_DeletePokeIcon( biw );

      if( ( biw->scr_type == BTLV_INPUT_SCRTYPE_COMMAND ) || ( biw->scr_type == BTLV_INPUT_SCRTYPE_YES_NO ) )
      {
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformCommand2Standby, ttw, 1 ), TCB_Transform_CB );
      }
      else
      {
        ttw->scr_type = biw->scr_type;
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformWaza2Standby, ttw, 1 ), TCB_Transform_CB );
      }
      PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, 0, STANDBY_FADE, STANDBY_FADE_COLOR, biw->tcbsys );

      //カーソルを非表示にする（時間制限での強制終了時の対応）
      if( GFL_CLACT_UNIT_GetDrawEnable( biw->cursor_clunit ) )
      {
        biw->cursor_pos = 0;
        biw->old_cursor_pos = CURSOR_NOMOVE;
        biw->cursor_decide = 1;
        GFL_CLACT_UNIT_SetDrawEnable( biw->cursor_clunit, FALSE );
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_COMMAND:
    {
      BTLV_INPUT_COMMAND_PARAM* bicp = ( BTLV_INPUT_COMMAND_PARAM * )param;
      int i;
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );

      biw->focus_pos = bicp->pos;

      BTLV_INPUT_SetFocus( biw );
      BTLV_EFFECT_SetCameraWorkExecute( BTLV_EFFECT_CWE_NO_STOP );

      if( bicp->pos >= BTLV_MCSS_POS_A )
      {
        bicp->pos = ( bicp->pos - BTLV_MCSS_POS_A ) / 2;
      }

      biw->tcb_execute_flag = 1;
      biw->center_button_type = bicp->center_button_type;
      ttw->biw = biw;
      ttw->pos = bicp->pos;
      biw->active_index = bicp->pos;

      if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
      {
        check_init_decide_pos( biw );
      }

      for( i = 0 ; i < BTLV_INPUT_DIR_MAX ; i++ )
      {
        biw->bidp[ 0 ][ i ] = bicp->bidp[ 0 ][ i ];
        biw->bidp[ 1 ][ i ] = bicp->bidp[ 1 ][ i ];
      }

      for( i = 0 ; i < 4 ; i++ )
      {
        biw->button_exist[ i ] = TRUE;  //押せるボタンかどうかチェック
      }
      biw->button_exist[ 1 ] = BTLV_EFFECT_CheckItemEnable();  //押せるボタンかどうかチェック

      BTLV_INPUT_DeletePokeIcon( biw );
      BTLV_INPUT_CreatePokeIcon( biw, bicp );
      BTLV_INPUT_CreateWeatherIcon( biw, bicp->weather );

      ttw->datID = get_command_screen( biw );

      BTLV_INPUT_PutShooterEnergy( biw, bicp );
#ifdef ROTATION_NEW_SYSTEM
      if( ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) || ( biw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE ) )
#else
      if( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA )
#endif
      {
        BTLV_INPUT_SetDrawEnablePokeIcon( biw, FALSE );
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformWaza2Command, ttw, 1 ), TCB_Transform_CB );
      }
      else
      {
        BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_MINE );
        if( bicp->trainer_flag )
        {
          BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_ENEMY );
        }
        biw->trainer_flag = bicp->trainer_flag;
#ifdef ROTATION_NEW_SYSTEM
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Command, ttw, 1 ), TCB_Transform_CB );
#else
        if( biw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE )
        {
          BTVL_EFFECT_SetTCB( GFL_TCB_AddTask( biw->tcbsys, TCB_TransformRotate2Command, ttw, 1 ),
                              TCB_Transform_CB, GROUP_DEFAULT );
        }
        else
        {
          BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Command, ttw, 1 ), TCB_Transform_CB );
        }
#endif
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_WAZA:
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      BTLV_INPUT_WAZA_PARAM* biwp = ( BTLV_INPUT_WAZA_PARAM * )param;

      BTLV_INPUT_CreateWazaScreen( biw, ( const BTLV_INPUT_WAZA_PARAM * )param );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      ttw->pos = biwp->pos;
      biw->henshin_flag = biwp->henshin_flag;
      ttw->datID = get_command_screen( biw );

      {
        int i;
        for( i = 0 ; i <PTL_WAZA_MAX ; i++ )
        {
          biw->waza[ i ] = biwp->wazano[ i ];
        }
      }

      if( ( biw->main_loop_tcb_flag == TRUE ) &&
        ( ( biw->type == BTLV_INPUT_TYPE_DOUBLE ) || ( biw->type == BTLV_INPUT_TYPE_TRIPLE ) ) )
      {
        BTLV_EFFECT_SetCameraWorkSwitch( BTLV_EFFECT_CWE_NO_STOP );
        BTLV_EFFECT_Add( BTLEFF_3vs3_CAMERA_ZOOMOUT );
      }

      if( biw->scr_type == BTLV_INPUT_SCRTYPE_DIR )
      {
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformDir2Waza, ttw, 1 ), TCB_Transform_CB );
      }
      else if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
      {
        BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_MINE );
        if( biw->trainer_flag )
        {
          BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_ENEMY );
        }
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Waza, ttw, 1 ), TCB_Transform_CB );
      }
      else
      {
        BTLV_INPUT_DeletePokeIcon( biw );
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformCommand2Waza, ttw, 1 ), TCB_Transform_CB );
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_DIR:
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      BTLV_INPUT_SCENE_PARAM* bisp = ( BTLV_INPUT_SCENE_PARAM * )param;

      BTLV_INPUT_CreateDirScreen( biw, ttw, ( const BTLV_INPUT_SCENE_PARAM * )param );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      ttw->pos = bisp->pos;
      BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformWaza2Dir, ttw, 1 ), TCB_Transform_CB );
    }
    break;
  case BTLV_INPUT_SCRTYPE_YES_NO:
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      BTLV_INPUT_YESNO_PARAM* biyp = ( BTLV_INPUT_YESNO_PARAM * )param;

      BTLV_INPUT_CreateYesNoScreen( biw, ( const BTLV_INPUT_YESNO_PARAM * )param );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      biw->b_button_flag = biyp->b_cancel_flag;
      BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2YesNo, ttw, 1 ), TCB_Transform_CB );
    }
    break;
  case BTLV_INPUT_SCRTYPE_ROTATE:
#ifdef ROTATION_NEW_SYSTEM
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      BTLV_INPUT_ROTATE_PARAM* birp = ( BTLV_INPUT_ROTATE_PARAM * )param;
      int i, j;

      if( biw->scr_type != BTLV_INPUT_SCRTYPE_STANDBY )
      {
        biw->rotate_flag = 0;
        biw->rotate_scr = 0;
      }

      for( i = 0 ; i < BTL_ROTATE_NUM ; i++ )
      {
        biw->rotate_bpp[ i ] = birp->bpp[ i ];
        for( j = 0 ; j < PTL_WAZA_MAX ; j++ )
        {
          biw->rotate_waza_exist[ i ][ j ] = birp->waza_exist[ i ][ j ];
        }
      }

      BTLV_INPUT_CreateRotateScreen( biw );

      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
      {
        BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_MINE );
        if( biw->trainer_flag )
        {
          BTLV_INPUT_CreateBallGauge( biw, BALL_GAUGE_TYPE_ENEMY );
        }
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Rotate, ttw, 1 ), TCB_Transform_CB );
      }
      else
      {
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformCommand2Rotate, ttw, 1 ), TCB_Transform_CB );
      }
    }
#else
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      BTLV_INPUT_ROTATE_PARAM* birp = ( BTLV_INPUT_ROTATE_PARAM * )param;
      int i;

      biw->before_select_dir = birp->before_select_dir;
      biw->rotate_flag = 0;
      biw->rotate_scr = bird[ biw->before_select_dir ].init_scr;

      for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
      {
        biw->rotate_pp[ i ] = birp->pp[ i ];
        biw->button_exist[ i ] = button_exist_table[ biw->rotate_scr ][ i ];
      }

      BTLV_INPUT_CreateRotateScreen( biw );
      BTLV_INPUT_CreateRotatePokeIcon( biw );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2Rotate, ttw, 1 ), TCB_Transform_CB );
    }
#endif
    break;
  case BTLV_INPUT_SCRTYPE_BATTLE_RECORDER:
    {
      BTLV_INPUT_BATTLE_RECORDER_PARAM* bibrp = ( BTLV_INPUT_BATTLE_RECORDER_PARAM * )param;
      STRBUF* msg_src = NULL;
      int i;

      for( i = 0 ; i < 3 ; i++ )
      {
        biw->button_exist[ i ] = 1;
      }

      BTLV_INPUT_CreateBattleRecorderScreen( biw, bibrp );

      switch( bibrp->stop_flag ){
      case BTLV_INPUT_BR_STOP_KEY:    //キーによる中断
      case BTLV_INPUT_BR_STOP_BREAK:  //データ破壊による中断
      case BTLV_INPUT_BR_STOP_SKIP:   //チャプタースキップによる中断
        GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                              PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                              PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );
        msg_src = GFL_MSG_CreateString( biw->msg,  BI_BattleRecorderStopKey + bibrp->stop_flag - 1 );
        /*fall thru*/
      case BTLV_INPUT_BR_STOP_NONE:
      default:
        break;
      }

      if( biw->scr_type == BTLV_INPUT_SCRTYPE_STANDBY )
      {
        TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
        biw->tcb_execute_flag = 1;
        ttw->biw = biw;
        if( bibrp->stop_flag == BTLV_INPUT_BR_STOP_NONE )
        {
          ttw->scr_x = 0;
          ttw->scr_y = TTS2BR_FRAME0_SCROLL_Y;
        }
        else
        {
          ttw->scr_x = TTS2BR_FRAME0_SCROLL_X;
          ttw->scr_y = 0;
        }
        ttw->msg_src = msg_src;
        BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2BattleRecorder, ttw, 1 ), TCB_Transform_CB );
      }
      else
      {
        if( bibrp->stop_flag == BTLV_INPUT_BR_STOP_NONE )
        {
          //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_X_SET, 0 );
          //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, TTS2BR_FRAME0_SCROLL_Y );
          SetupSetScroll( biw, GFL_BG_FRAME0_S, 0, TTS2BR_FRAME0_SCROLL_Y,
                          BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
        }
        else
        {
          //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_X_SET, TTS2BR_FRAME0_SCROLL_X );
          //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, 0 );
          SetupSetScroll( biw, GFL_BG_FRAME0_S, TTS2BR_FRAME0_SCROLL_X, 0,
                          BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
        }
        if( msg_src )
        {
          PRINTSYS_Print( biw->bmp_data, BR_MESSAGE_X, BR_MESSAGE_Y, msg_src, biw->font );
          GFL_STR_DeleteBuffer( msg_src );
        }
        GFL_BMPWIN_MakeScreen( biw->bmp_win );
        GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
        GFL_BMPWIN_TransVramCharacter( biw->bmp_win );
      }
    }
    break;
  case BTLV_INPUT_SCRTYPE_PDC:
    {
      TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
      biw->tcb_execute_flag = 1;
      ttw->biw = biw;
      BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformStandby2PDC, ttw, 1 ), TCB_Transform_CB );
      biw->button_exist[ 0 ] = TRUE;  //押せるボタンかどうかチェック
      biw->button_exist[ 1 ] = TRUE;  //押せるボタンかどうかチェック
    }
    break;
  default:
    //ありえないSCRTYPEが指定されている
    GF_ASSERT( 0 );
    break;
  }
  biw->scr_type = type;
}

//============================================================================================
/**
 *  @brief  入力チェック
 *
 *  @param[in]  tp_tbl  タッチパネルテーブル
 *  @param[in]  key_tbl キー操作テーブル
 */
//============================================================================================
int BTLV_INPUT_CheckInput( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tp_tbl, const BTLV_INPUT_KEYTBL* key_tbl )
{
  int hit, hit_tp;

#ifdef PM_DEBUG
  debug_timer_edit_check( biw );

  //タイマー編集中は入力を無視
  if( biw->timer_edit_flag )
  {
    return  GFL_UI_TP_HIT_NONE;
  }
#endif

  //下画面変形中は入力を無視
  if( ( biw->tcb_execute_flag ) || ( PaletteFadeCheck( biw->pfd ) ) || ( biw->button_reaction ) )
  {
    return  GFL_UI_TP_HIT_NONE;
  }

  if( biw->hit != GFL_UI_TP_HIT_NONE )
  {
    biw->decide_pos_flag = 0;
    hit = biw->hit;
    biw->hit = GFL_UI_TP_HIT_NONE;
    return hit;
  }

  //CHANGE00
  // ゲージ見た目変更
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    // コマンド選択　技選択　ターゲット選択
    if( (biw->scr_type == BTLV_INPUT_SCRTYPE_COMMAND) ||
        (biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA) ||
        (biw->scr_type == BTLV_INPUT_SCRTYPE_DIR) ){
      BTLV_EFFECT_SwitchGaugeMode();
    }
  }

#ifndef ROTATION_NEW_SYSTEM
  //ローテーション選択は、別判定をする
  if( biw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE )
  {
    hit = GFL_UI_TP_HitTrg( RotateTouchData.hit_tbl );
    hit = BTLV_INPUT_CheckKey( biw, &RotateTouchData, RotateKeyData[ biw->rotate_scr ], NULL, hit );
    if( hit != GFL_UI_TP_HIT_NONE )
    {
      if( biw->button_exist[ hit ] == FALSE )
      {
        hit = GFL_UI_TP_HIT_NONE;
      }
      else
      {
        //けっていを選択
        if( hit == 2 )
        {
          SePlayRotateDecide( biw );
          hit = biw->rotate_flag + 1;
        }
        else
        {
          SePlayRotateSelect( biw );
          SetupRotateAction( biw, hit );
          hit = GFL_UI_TP_HIT_NONE;
        }
      }
    }
    if( hit != GFL_UI_TP_HIT_NONE )
    {
      hit = BTLV_INPUT_SetButtonReaction( biw, hit, RotateTouchData.button_pltt[ hit ] );
    }
    return hit;
  }
#endif

  GF_ASSERT( tp_tbl != NULL );
  GF_ASSERT( key_tbl != NULL );

  //バトルレコーダーは、別判定をする
  if( biw->scr_type == BTLV_INPUT_SCRTYPE_BATTLE_RECORDER )
  {
    hit = GFL_UI_TP_HitTrg( tp_tbl->hit_tbl );
    if( hit != GFL_UI_TP_HIT_NONE )
    {
      if( biw->button_exist[ hit ] == FALSE )
      {
        hit = GFL_UI_TP_HIT_NONE;
      }
      else
      {
        if( hit == BTLV_INPUT_BR_SEL_STOP )
        {
          SePlayCancel( biw );
        }
        else
        {
          SePlayDecide( biw );
        }
      }
    }
    else
    {
      hit = BTLV_INPUT_CheckKeyBR( biw );
    }
    if( hit != GFL_UI_TP_HIT_NONE )
    {
      hit = BTLV_INPUT_SetButtonReaction( biw, hit, tp_tbl->button_pltt[ hit ] );
    }
    return hit;
  }

  hit = hit_tp = GFL_UI_TP_HitTrg( tp_tbl->hit_tbl );
  hit = BTLV_INPUT_CheckKey( biw, tp_tbl, key_tbl, NULL, hit, biw->henshin_flag );

  if( hit != GFL_UI_TP_HIT_NONE )
  {
    int cont = GFL_UI_KEY_GetCont();
    if( ( biw->button_exist[ hit ] == FALSE ) && ( ( ( cont & PAD_BUTTON_L ) == 0 ) || ( biw->scr_type != BTLV_INPUT_SCRTYPE_WAZA ) ) )
    {
      if( ( biw->waza_exist[ hit ] ) && ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) )
      {
        SePlayBeep( biw );
      }
      hit = GFL_UI_TP_HIT_NONE;
    }
    else if( ( hit < 4 ) &&
             ( cont & PAD_BUTTON_L ) &&
             ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) &&
             ( biw->henshin_flag == TRUE ) )
    {
      hit = GFL_UI_TP_HIT_NONE;
    }
    else if( ( hit < 4 ) &&
             ( cont & PAD_BUTTON_L ) &&
             ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) &&
             ( biw->waza_exist[ hit ] == FALSE ) )
    {
      hit = GFL_UI_TP_HIT_NONE;
    }
    else
    {
      if( hit_tp != GFL_UI_TP_HIT_NONE )
      {
        if( ( get_cancel_flag( biw, tp_tbl, hit ) == FALSE ) && ( biw->decide_pos_flag == 0 ) )
        {
          SePlayDecide( biw );
          switch( biw->scr_type ){
          case BTLV_INPUT_SCRTYPE_COMMAND:
          case BTLV_INPUT_SCRTYPE_ROTATE:
          case BTLV_INPUT_SCRTYPE_WAZA:
            biw->decide_pos[ biw->active_index ][ biw->scr_type ] = hit;
            break;
          case BTLV_INPUT_SCRTYPE_DIR:
            {
              int i;
              int pos = 0;
              int pos_max = ( biw->type != BTLV_INPUT_TYPE_TRIPLE ) ?
                            PokeSeleMenuKey4DataCount[ biw->active_index ][ biw->waza_target ] :
                            PokeSeleMenuKey6DataCount[ biw->active_index ][ biw->waza_target ];
              const BTLV_INPUT_KEYTBL*  key_tbl = ( biw->type != BTLV_INPUT_TYPE_TRIPLE ) ?
                                                  PokeSeleMenuKey4Data[ biw->active_index ][ biw->waza_target ] :
                                                  PokeSeleMenuKey6Data[ biw->active_index ][ biw->waza_target ];

              for( i = 0 ; i < pos_max ; i++ )
              {
                if( key_tbl[ i ].a_button == hit )
                {
                  pos = i;
                  break;
                }
              }
              biw->decide_pos[ biw->active_index ][ biw->scr_type ] = pos |
                                                                      ( biw->decide_pos[ biw->active_index ]
                                                                                       [ BTLV_INPUT_SCRTYPE_WAZA ] << 4 );
            }
            break;
          default:
            break;
          }
        }
        else
        {
          SePlayCancel( biw );
        }
      }
    }
  }
  if( hit != GFL_UI_TP_HIT_NONE )
  {
    hit = BTLV_INPUT_SetButtonReaction( biw, hit, tp_tbl->button_pltt[ hit ] );
  }
  return hit;
}

//============================================================================================
/**
 *  @brief  入力チェック（デモ用）
 */
//============================================================================================
BOOL  BTLV_INPUT_CheckInputDemo( BTLV_INPUT_WORK* biw )
{
  BOOL  ret = FALSE;

  switch( biw->seq_no ){
  case 0:
    {
      int cursor_pos[][ 2 ] = { { 128, 72 }, { 64, 48 }, { 40, 152 } };
      if( biw->bfcw == NULL )
      {
        biw->bfcw = BTLV_FINGER_CURSOR_Init( biw->pfd, 0x0b, biw->heapID );
      }
      if( BTLV_FINGER_CURSOR_Create( biw->bfcw,
                                     cursor_pos[ biw->demo_cursor_pos ][ 0 ],
                                     cursor_pos[ biw->demo_cursor_pos ][ 1 ],
                                     FINGER_CURSOR_LOOP, FINGER_CURSOR_FRAME, FINGER_CURSOR_WAIT ) == FALSE )
      {
        break;
      }
    }
    biw->seq_no++;
    biw->demo_cursor_pos++;
    break;
  case 1:
    if( ( ret = BTLV_FINGER_CURSOR_CheckExecute( biw->bfcw ) ) == TRUE )
    {
      SePlayDecide( biw );
    }
    break;
  }

  return ret;
}

#ifdef ROTATION_NEW_SYSTEM
//============================================================================================
/**
 *  @brief  入力チェック（ローテーション専用）
 *
 *  @param[in]    biw     システム管理構造体へのポインタ
 *  @param[out]   dir     どっち回転させたか？
 *  @param[out]   select  どの技を選んだか？
 */
//============================================================================================
BOOL  BTLV_INPUT_CheckInputRotate( BTLV_INPUT_WORK* biw, BtlRotateDir* dir, int* select )
{
  int hit;

#ifdef PM_DEBUG
  debug_timer_edit_check( biw );
  //タイマー編集中は入力を無視
  if( biw->timer_edit_flag )
  {
    return  GFL_UI_TP_HIT_NONE;
  }
#endif

  //下画面変形中は入力を無視
  if( ( biw->tcb_execute_flag ) ||
      ( PaletteFadeCheck( biw->pfd ) ) ||
      ( biw->button_reaction ) )
  {
    return  FALSE;
  }

  //CHANGE01
  // ゲージ見た目変更
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    BTLV_EFFECT_SwitchGaugeMode();
  }

  if( biw->hit != GFL_UI_TP_HIT_NONE )
  {
    int ret_hit = biw->hit;
    BTLV_INPUT_CheckWazaInfoModeMask( &ret_hit );
    if( ret_hit < 5 )
    {
      if( ( ret_hit < 4 ) && ( biw->waruagaki_flag == TRUE ) )
      {
        biw->hit = BTLV_INPUT_WARUAGAKI_BUTTON;
      }
      *select = biw->hit;
      *dir    = rotate_result[ biw->rotate_scr ];
      biw->hit = GFL_UI_TP_HIT_NONE;
      biw->decide_pos_flag = 0;
      PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, 0x3e00, 0, 0, 0, 0, biw->tcbsys );
      PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_OBJ, 0x0700, 0, 0, 0, 0, biw->tcbsys );
      return TRUE;
    }
    else
    {
      biw->cursor_pos = 0;
      biw->decide_pos[ biw->active_index ][biw->scr_type ] = 0;
      biw->decide_pos_flag = 0;
      SePlayRotateSelect( biw );
      SetupRotateAction( biw, ret_hit );
      biw->hit = GFL_UI_TP_HIT_NONE;
      return FALSE;
    }
  }

  if( biw->waruagaki_flag == TRUE )
  {
    hit = GFL_UI_TP_HitTrg( RotateTouchDataWaruagaki.hit_tbl );
    hit = BTLV_INPUT_CheckKey( biw, &RotateTouchDataWaruagaki, RotateKeyDataWaruagaki[ biw->rotate_scr ], NULL, hit,
                               BPP_HENSIN_Check( biw->rotate_bpp[ biw->rotate_scr ] ) );
  }
  else
  {
    hit = GFL_UI_TP_HitTrg( RotateTouchData.hit_tbl );
    hit = BTLV_INPUT_CheckKey( biw, &RotateTouchData, RotateKeyData[ biw->rotate_scr ], RotateMoveTbl, hit,
                               BPP_HENSIN_Check( biw->rotate_bpp[ biw->rotate_scr ] ) );
  }
  if( hit != GFL_UI_TP_HIT_NONE )
  {
    int cont = GFL_UI_KEY_GetCont();
    if( ( biw->button_exist[ hit ] == FALSE ) && ( ( cont & PAD_BUTTON_L ) == 0 ) )
    {
      if( biw->waza_exist[ hit ] )
      {
        SePlayBeep( biw );
      }
      hit = GFL_UI_TP_HIT_NONE;
    }
    else if( ( hit > 4 ) && ( biw->button_exist[ hit ] == FALSE ) )
    {
      hit = GFL_UI_TP_HIT_NONE;
    }
    else if( ( hit < 4 ) &&
             ( cont & PAD_BUTTON_L ) &&
           ( ( BPP_HENSIN_Check( biw->rotate_bpp[ biw->rotate_scr ] ) == TRUE ) || ( biw->waruagaki_flag == TRUE ) ) )
    {
      hit = GFL_UI_TP_HIT_NONE;
    }
    else if( ( hit < 4 ) &&
             ( cont & PAD_BUTTON_L ) &&
             ( biw->waza_exist[ hit ] == FALSE ) )
    {
      hit = GFL_UI_TP_HIT_NONE;
    }
  }
  if( hit != GFL_UI_TP_HIT_NONE )
  {
    if( biw->waruagaki_flag == TRUE )
    {
      BOOL  cancel_flag = get_cancel_flag( biw, &RotateTouchDataWaruagaki, hit );
      if( ( cancel_flag == FALSE ) && ( biw->decide_pos_flag == 0 ) )
      {
        biw->decide_pos[ biw->active_index ][ biw->scr_type ] = hit;
        SePlayRotateDecide( biw );
      }
      else if( ( cancel_flag == TRUE ) && ( biw->decide_pos_flag == 0 ) )
      {
        SePlayCancel( biw );
      }
      hit = BTLV_INPUT_SetButtonReaction( biw, hit, RotateTouchDataWaruagaki.button_pltt[ hit ] );
    }
    else
    {
      BOOL  cancel_flag = get_cancel_flag( biw, &RotateTouchData, hit );
      if( ( cancel_flag == FALSE ) && ( biw->decide_pos_flag == 0 ) )
      {
        biw->decide_pos[ biw->active_index ][ biw->scr_type ] = hit;
        SePlayRotateDecide( biw );
      }
      else if( ( cancel_flag == TRUE ) && ( biw->decide_pos_flag == 0 ) )
      {
        SePlayCancel( biw );
      }
      hit = BTLV_INPUT_SetButtonReaction( biw, hit, RotateTouchData.button_pltt[ hit ] );
    }
  }
  return ( hit != GFL_UI_TP_HIT_NONE );
}
#endif

//============================================================================================
/**
 *  @brief  下画面が変形中かどうかチェック
 *
 *  @param[in]  biw  下画面管理構造体
 *
 *  @retval TRUE:変形中 FALSE:変形終了
 */
//============================================================================================
BOOL  BTLV_INPUT_CheckExecute( BTLV_INPUT_WORK* biw )
{
  //if( ( biw->tcb_execute_flag ) || ( PaletteFadeCheck( biw->pfd ) ) || ( biw->button_reaction ) || BTLV_EFFECT_CheckExecute() )
  if( ( biw->tcb_execute_flag ) || ( PaletteFadeCheck( biw->pfd ) ) || ( biw->button_reaction ) )
  {
    return TRUE;
  }
  return FALSE;
}

//============================================================================================
/**
 *  @brief  下画面のスクリーンタイプを取得
 *
 *  @param[in]  biw  下画面管理構造体
 *
 *  @retval BTLV_INPUT_SCRTYPE
 */
//============================================================================================
BTLV_INPUT_SCRTYPE  BTLV_INPUT_GetScrType( BTLV_INPUT_WORK* biw )
{
  return biw->scr_type;
}

//============================================================================================
/**
 *  @brief  下画面リソースロード
 *
 *  @param[in]  biw  下画面管理構造体
 */
//============================================================================================
static  void  BTLV_INPUT_LoadResource( BTLV_INPUT_WORK* biw )
{
  GFL_ARCHDL_UTIL_TransVramBgCharacter( biw->handle, NARC_battgra_wb_battle_w_bg_NCGR,
                                        GFL_BG_FRAME0_S, 0, 0, FALSE, biw->heapID );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( biw->handle, NARC_battgra_wb_battle_w_bg3_NCGR,
                                        GFL_BG_FRAME3_S, 0, 0x8000, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg3_NSCR,
                                   GFL_BG_FRAME3_S, 0, 0, FALSE, biw->heapID );
  PaletteWorkSet_ArcHandle( biw->pfd, biw->handle, NARC_battgra_wb_battle_w_bg_NCLR,
                            biw->heapID, FADE_SUB_BG, 0x1e0, 0 );

  //バッグボタンパレットを退避しておく
  if( biw->main_loop_tcb_flag == TRUE )
  {
    u16*  pal = PaletteWorkDefaultWorkGet( BTLV_EFFECT_GetPfd(), FADE_SUB_BG );
    MI_CpuCopy16( &pal[ 0x10 * 2 ], &biw->bag_pal, 0x20 );
  }

  biw->objcharID = GFL_CLGRP_CGR_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCGR, FALSE,
                                           CLSYS_DRAW_SUB, biw->heapID );
  biw->objcellID = GFL_CLGRP_CELLANIM_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCER,
                                                NARC_battgra_wb_battle_w_obj_NANR, biw->heapID );
  biw->objplttID = GFL_CLGRP_PLTT_Register( biw->handle, NARC_battgra_wb_battle_w_obj_NCLR, CLSYS_DRAW_SUB, 0, biw->heapID );
  PaletteWorkSet_VramCopy( biw->pfd, FADE_SUB_OBJ,
                           GFL_CLGRP_PLTT_GetAddr( biw->objplttID, CLSYS_DRAW_SUB ) / 2, 0x20 * 8 );
  biw->cur_charID = GFL_CLGRP_CGR_Register( biw->handle, NARC_battgra_wb_battle_w_cursor_NCGR, FALSE,
                                           CLSYS_DRAW_SUB, biw->heapID );
  biw->cur_cellID = GFL_CLGRP_CELLANIM_Register( biw->handle, NARC_battgra_wb_battle_w_cursor_NCER,
                                                NARC_battgra_wb_battle_w_cursor_NANR, biw->heapID );

  {
    ARCHANDLE*  hdl;
    int         i;

    //技タイプアイコン
    hdl = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), GFL_HEAP_LOWID( biw->heapID ) );
    biw->wazatype_cellID = GFL_CLGRP_CELLANIM_Register( hdl,
                                                        APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_32K ),
                                                        APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_32K ),
                                                        biw->heapID );
    biw->wazatype_plttID = GFL_CLGRP_PLTT_Register( hdl,
                                                    APP_COMMON_GetPokeTypePltArcIdx(), CLSYS_DRAW_SUB,
                                                    0x20 * 8, biw->heapID );
    PaletteWorkSet_VramCopy( biw->pfd, FADE_SUB_OBJ,
                             GFL_CLGRP_PLTT_GetAddr( biw->wazatype_plttID, CLSYS_DRAW_SUB ) / 2, 0x20 * 3 );
    for( i = 0; i < PTL_WAZA_MAX ; i++ ){
      biw->wazatype_charID[ i ] = GFL_CLGRP_CGR_Register( hdl, APP_COMMON_GetPokeTypeCharArcIdx( POKETYPE_NORMAL ), FALSE,
                                                          CLSYS_DRAW_SUB, biw->heapID );
    }
    GFL_ARC_CloseDataHandle( hdl );
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（待機→コマンド選択）
 */
//============================================================================================
static  void  TCB_TransformStandby2Command( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID, GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1a_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_ON, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    change_bag_button_pal( ttw->biw );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win_shooter );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    PaletteFadeReqWrite( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win_shooter );
      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（待機→技選択）
 */
//============================================================================================
static  void  TCB_TransformStandby2Waza( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0a_NSCR,
                                     GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1h_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_OFF, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    //SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    PaletteFadeReqWrite( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      if( ( ttw->biw->type == BTLV_INPUT_TYPE_TRIPLE ) && ( ttw->pos != BTLV_MCSS_POS_C ) )
      {
        GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0b_NSCR,
                                         GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
      }
      else
      {
        GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0a_NSCR,
                                         GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
      }
      GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      SetupScrollUp( ttw->biw, TTC2W_START_SCROLL_X, TTC2W_START_SCROLL_Y, TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
      SetupBallGaugeMove( ttw->biw, BALL_GAUGE_MOVE_OPEN );
      SetupSetScroll( ttw->biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
      ttw->wait = 4;
      ttw->seq_no++;
    }
    break;
  case 2:
    if( --ttw->wait == 0 )
    {
      SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_APPEAR );
      ttw->seq_no++;
    }
    break;
  case 3:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（コマンド選択→技選択）
 */
//============================================================================================
static  void  TCB_TransformCommand2Waza( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID, GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    SetupScrollUp( ttw->biw, TTC2W_START_SCROLL_X, TTC2W_START_SCROLL_Y, TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
    SetupBallGaugeMove( ttw->biw, BALL_GAUGE_MOVE_OPEN );
    pop_bag_button_pal( ttw->biw );
    SetupSetScroll( ttw->biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
    ttw->wait = 4;
    ttw->seq_no++;
    break;
  case 1:
    if( --ttw->wait == 0 )
    {
      SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_APPEAR );
      ttw->seq_no++;
    }
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（技選択→コマンド選択）
 */
//============================================================================================
static  void  TCB_TransformWaza2Command( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID, GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1a_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win_shooter );
    change_bag_button_pal( ttw->biw );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X2 );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y2 );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X2, TSA_SCROLL_Y2,
                      BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
    SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_VANISH );
    ttw->wait = 4;
    ttw->seq_no++;
    break;
  case 2:
    if( --ttw->wait == 0 )
    {
      SetupScrollUp( ttw->biw, TTW2C_START_SCROLL_X, TTW2C_START_SCROLL_Y, TTW2C_SCROLL_SPEED, TTW2C_SCROLL_COUNT );
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_BACKWARD );
      SetupBallGaugeMove( ttw->biw, BALL_GAUGE_MOVE_CLOSE );
      ttw->seq_no++;
    }
    break;
  case 3:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      BTLV_INPUT_SetDrawEnablePokeIcon( ttw->biw, TRUE );
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win_shooter );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（ローテーション選択→コマンド選択）
 */
//============================================================================================
static  void  TCB_TransformRotate2Command( GFL_TCB* tcb, void* work )
{
#ifndef ROTATION_NEW_SYSTEM
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID, GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1a_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SetupScrollUp( ttw->biw, TTW2C_START_SCROLL_X, TTW2C_START_SCROLL_Y, TTW2C_SCROLL_SPEED, TTW2C_SCROLL_COUNT );
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_BACKWARD );
    SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_VANISH );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win_shooter );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win_shooter );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
#endif
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（技選択→攻撃対象選択）
 */
//============================================================================================
static  void  TCB_TransformWaza2Dir( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0a_NSCR,
                                   GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID,
                                   GFL_BG_FRAME0_S, 0x0400 + 0x40, 0x40 * 14, FALSE, ttw->biw->heapID );
  GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
  if( ttw->biw->type == BTLV_INPUT_TYPE_TRIPLE )
  {
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1c_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
  }
  else
  {
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1b_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
  }
  GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
  BTLV_INPUT_FreeTCB( ttw->biw, tcb );
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（攻撃対象選択→技選択）
 */
//============================================================================================
static  void  TCB_TransformDir2Waza( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, ttw->datID, GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X0, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
    ttw->seq_no++;
    break;
  case 1:
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1a_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
    GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
    BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（コマンド選択→スタンバイ）
 */
//============================================================================================
static  void  TCB_TransformCommand2Standby( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    SetupScaleChange( ttw->biw, TTC2S_START_SCALE, TTC2S_END_SCALE, TTC2S_SCALE_SPEED, COMMAND_POS_Y );
    ttw->seq_no++;
    break;
  case 1:
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    ttw->seq_no++;
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（技選択→スタンバイ）
 */
//============================================================================================
static  void  TCB_TransformWaza2Standby( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0a_NSCR,
                                     GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1g_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X2 );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y2 );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X2, TSA_SCROLL_Y2,
                      BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 1:
    if( ( ttw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) || ( ttw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE ) )
    {
      SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_VANISH );
    }
    ttw->seq_no++;
    ttw->wait = 4;
    break;
  case 2:
    if( --ttw->wait == 0 )
    {
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_BACKWARD );
      ttw->seq_no++;
    }
    break;
  case 3:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      SetupScaleChange( ttw->biw, TTC2S_START_SCALE, TTC2S_END_SCALE, TTC2S_SCALE_SPEED, COMMAND_POS_Y );
      ttw->seq_no++;
    }
    break;
  case 4:
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    ttw->seq_no++;
    break;
  case 5:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（スタンバイ→2択）
 */
//============================================================================================
static  void  TCB_TransformStandby2YesNo( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_OFF, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    pop_bag_button_pal( ttw->biw );
    PaletteFadeReq( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      ttw->seq_no++;
    }
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（スタンバイ→ローテーション）
 */
//============================================================================================
static  void  TCB_TransformStandby2Rotate( GFL_TCB* tcb, void* work )
{
#ifdef ROTATION_NEW_SYSTEM
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0a_NSCR,
                                     GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1h_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_OFF, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    //SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    PaletteFadeReqWrite( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0h_NSCR,
                                       GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
      GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      SetupScrollUp( ttw->biw, rotate_scroll_table[ ttw->biw->rotate_scr ][ 0 ],
                     rotate_scroll_table[ ttw->biw->rotate_scr ][ 1 ], TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
      SetupBallGaugeMove( ttw->biw, BALL_GAUGE_MOVE_OPEN );
      SetupSetScroll( ttw->biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
      ttw->wait = 4;
      ttw->seq_no++;
    }
    break;
  case 2:
      if( --ttw->wait == 0 )
      {
        SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_APPEAR );
        ttw->seq_no++;
      }
    break;
  case 3:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
#else
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_NO_SET, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    PaletteFadeReq( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
      SetupScrollUp( ttw->biw,
                     rotate_scroll_table[ ttw->biw->rotate_scr ][ 0 ], rotate_scroll_table[ ttw->biw->rotate_scr ][ 1 ],
                     ROTATE_SCROLL_SPEED, ROTATE_SCROLL_COUNT );
      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      ttw->seq_no++;
    }
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      int i;

      for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
      {
        if( ttw->biw->pokeicon_wk[ i ].clwk )
        {
          GFL_CLACT_WK_SetDrawEnable( ttw->biw->pokeicon_wk[ i ].clwk, TRUE );
        }
      }
      GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
#endif
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（スタンバイ→バトルレコーダー）
 */
//============================================================================================
static  void  TCB_TransformStandby2BattleRecorder( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0f_NSCR,
                                     GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1f_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_ON, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    PaletteFadeReq( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      if( ttw->msg_src )
      {
        PRINTSYS_Print( ttw->biw->bmp_data, BR_MESSAGE_X, BR_MESSAGE_Y, ttw->msg_src, ttw->biw->font );
        GFL_STR_DeleteBuffer( ttw->msg_src );
      }
      GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_X_SET, ttw->scr_x );
      //GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, ttw->scr_y );
      //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
      //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME0_S, ttw->scr_x, ttw->scr_y,
                      BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（スタンバイ→ポケモンドリームキャッチ）
 */
//============================================================================================
static  void  TCB_TransformStandby2PDC( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0g_NSCR,
                                     GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg1d_NSCR,
                                     GFL_BG_FRAME1_S, 0, 0, FALSE, ttw->biw->heapID );
    SePlayOpen( ttw->biw );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TTS2C_FRAME1_SCROLL_X );
    //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TTS2C_FRAME1_SCROLL_Y );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
    SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TTS2C_FRAME1_SCROLL_X, TTS2C_FRAME1_SCROLL_Y,
                    BG_VISIBLE_ON, BG_VISIBLE_OFF, BG_VISIBLE_NO_SET, BG_VISIBLE_ON );
    SetupScaleChange( ttw->biw, TTS2C_START_SCALE, TTS2C_END_SCALE, -TTS2C_SCALE_SPEED, STANBY_POS_Y );
    SetupScrollUp( ttw->biw, TTS2C_START_SCROLL_X, TTS2C_START_SCROLL_Y, TTS2C_SCROLL_SPEED, TTS2C_SCROLL_COUNT );
    PaletteFadeReq( ttw->biw->pfd, PF_BIT_SUB_BG, STANDBY_PAL, 1, STANDBY_FADE, 0, STANDBY_FADE_COLOR, ttw->biw->tcbsys );
    ttw->seq_no++;
    break;
  case 1:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
      GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（コマンド→ローテーション選択）
 */
//============================================================================================
static  void  TCB_TransformCommand2Rotate( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
     GFL_ARCHDL_UTIL_TransVramScreen( ttw->biw->handle, NARC_battgra_wb_battle_w_bg0h_NSCR,
                                      GFL_BG_FRAME0_S, 0, 0, FALSE, ttw->biw->heapID );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
    pop_bag_button_pal( ttw->biw );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    SetupScrollUp( ttw->biw, rotate_scroll_table[ ttw->biw->rotate_scr ][ 0 ],
                   rotate_scroll_table[ ttw->biw->rotate_scr ][ 1 ], TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
    SetupScreenAnime( ttw->biw, 0, SCREEN_ANIME_DIR_FORWARD );
    SetupBallGaugeMove( ttw->biw, BALL_GAUGE_MOVE_OPEN );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    SetupSetScroll( ttw->biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
    ttw->wait = 4;
    ttw->seq_no++;
    break;
  case 1:
    if( --ttw->wait == 0 )
    {
      SetupButtonAnime( ttw->biw, BUTTON_TYPE_WAZA, BUTTON_ANIME_TYPE_APPEAR );
      ttw->seq_no++;
    }
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  下画面変形タスク（ローテーション選択）
 */
//============================================================================================
static  void  TCB_TransformRotate2Rotate( GFL_TCB* tcb, void* work )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)work;

  switch( ttw->seq_no ){
  case 0:
    BTLV_INPUT_ClearScreen( ttw->biw );
    ttw->seq_no++;
    break;
  case 1:
    BTLV_INPUT_CreateRotateScreen( ttw->biw );
    GFL_BMPWIN_MakeScreen( ttw->biw->bmp_win );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
    SetupScrollUp( ttw->biw, rotate_scroll_table[ ttw->biw->rotate_scr ][ 0 ],
                   rotate_scroll_table[ ttw->biw->rotate_scr ][ 1 ], TTC2W_SCROLL_SPEED, TTC2W_SCROLL_COUNT );
    //GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
    //GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    SetupSetScroll( ttw->biw, BG_NO_FRAME, 0, 0, BG_VISIBLE_ON, BG_VISIBLE_ON, BG_VISIBLE_NO_SET, BG_VISIBLE_OFF );
    ttw->seq_no++;
    break;
  case 2:
  default:
    if( ttw->biw->tcb_execute_count == 0 )
    {
      GFL_BMPWIN_TransVramCharacter( ttw->biw->bmp_win );
      GFL_CLACT_UNIT_SetDrawEnable( ttw->biw->wazatype_clunit, TRUE );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X3 );
      //GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y3 );
      SetupSetScroll( ttw->biw, GFL_BG_FRAME1_S, TSA_SCROLL_X3, TSA_SCROLL_Y3,
                      BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET, BG_VISIBLE_NO_SET );
      BTLV_INPUT_FreeTCB( ttw->biw, tcb );
    }
    break;
  }
}

static  void  TCB_Transform_CB( GFL_TCB* tcb )
{
  TCB_TRANSFORM_WORK* ttw = (TCB_TRANSFORM_WORK *)GFL_TCB_GetWork( tcb );

  ttw->biw->tcb_execute_flag = 0;
}

//============================================================================================
/**
 *  @brief  スケール変更処理セットアップ
 *
 *  @param[in]  biw           システム管理構造体のポインタ
 *  @param[in]  start_scale   スケール初期値
 *  @param[in]  end_scale     最終的なスケール値
 *  @param[in]  scale_speed   スケール変更速度
 *  @param[in]  pos_y         スクロールY初期値
 */
//============================================================================================
static  void  SetupScaleChange( BTLV_INPUT_WORK* biw, fx32 start_scale, fx32 end_scale, fx32 scale_speed, int pos_y )
{
  TCB_SCALE_UP* tsu = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_SCALE_UP ) );

  tsu->biw          = biw;
  tsu->start_scale  = start_scale;
  tsu->end_scale    = end_scale;
  tsu->scale_speed  = scale_speed;
  tsu->pos_y        = pos_y;

  BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_ScaleChange, tsu, 0 ), TCB_ScaleChange_CB );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *  @brief  スケール変更処理タスク
 */
//============================================================================================
static  void  TCB_ScaleChange( GFL_TCB* tcb, void* work )
{
  TCB_SCALE_UP* tsu = ( TCB_SCALE_UP * )work;
  MtxFx22 mtx;

  tsu->start_scale += tsu->scale_speed;
  if( tsu->scale_speed > 0 )
  {
    tsu->pos_y -= 2;
  }
  else if( tsu->scale_speed < 0 )
  {
    tsu->pos_y += 2;
  }

  MTX_Scale22( &mtx, tsu->start_scale, tsu->start_scale );
  GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_X_SET, 128, &mtx, 256, 256 );
  GFL_BG_SetAffineScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, tsu->pos_y, &mtx, 256, 256 );
  if( tsu->start_scale == tsu->end_scale )
  {
    BTLV_INPUT_FreeTCB( tsu->biw, tcb );
  }
}

static  void  TCB_ScaleChange_CB( GFL_TCB* tcb )
{
  TCB_SCALE_UP* tsu = ( TCB_SCALE_UP * )GFL_TCB_GetWork( tcb );

  tsu->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  スクリーンスクロール処理セットアップ
 *
 *  @param[in]  biw           システム管理構造体のポインタ
 *  @param[in]  scroll_x      初期X座標
 *  @param[in]  scroll_y      初期Y座標
 *  @param[in]  scroll_speed  スクロールスピード
 *  @param[in]  scroll_count  スクロールカウント
 */
//============================================================================================
static  void  SetupScrollUp( BTLV_INPUT_WORK* biw, int scroll_x, int scroll_y, int scroll_speed, int scroll_count )
{
  TCB_SCROLL_UP* tsu = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_SCROLL_UP ) );

  tsu->biw          = biw;
  tsu->seq_no       = 0;
  tsu->scroll_x     = scroll_x;
  tsu->scroll_y     = scroll_y;
  tsu->scroll_speed = scroll_speed;
  tsu->scroll_count = scroll_count;

  BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_ScrollUp, tsu, 0 ), TCB_ScrollUp_CB );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *  @brief  スクリーンスクロール処理タスク
 */
//============================================================================================
static  void  TCB_ScrollUp( GFL_TCB* tcb, void* work )
{
  TCB_SCROLL_UP* tsu = ( TCB_SCROLL_UP * )work;

  if( tsu->seq_no == 0 )
  {
    GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_X_SET, tsu->scroll_x );
    GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, tsu->scroll_y );
    tsu->seq_no++;
    return;
  }

  tsu->scroll_y += tsu->scroll_speed;

  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, tsu->scroll_y );

  if( --tsu->scroll_count == 0 )
  {
    BTLV_INPUT_FreeTCB( tsu->biw, tcb );
  }
}

static  void  TCB_ScrollUp_CB( GFL_TCB* tcb )
{
  TCB_SCROLL_UP* tsu = ( TCB_SCROLL_UP * )GFL_TCB_GetWork( tcb );

  tsu->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  スクリーンアニメ処理セットアップ
 *
 *  @param[in]  biw   システム管理構造体のポインタ
 *  @param[in]  index 読み込むスクリーンリソースインデックス
 *  @param[in]  dir   アニメーションの向き
 */
//============================================================================================
static  void  SetupScreenAnime( BTLV_INPUT_WORK* biw, int index, SCREEN_ANIME_DIR dir )
{
  TCB_SCREEN_ANIME* tsa = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_SCREEN_ANIME ) );

  tsa->biw    = biw;
  tsa->count  = 0;
  tsa->dir    = dir;
  tsa->wait   = TSA_WAIT;

  //indexから表示するスクリーンを読み替える

  BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_ScreenAnime, tsa, 0 ), TCB_ScreenAnime_CB );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *  @brief  スクリーンアニメ処理タスク
 */
//============================================================================================
static  void  TCB_ScreenAnime( GFL_TCB* tcb, void* work )
{
  TCB_SCREEN_ANIME* tsa = ( TCB_SCREEN_ANIME * )work;
  static  const int TSA_scroll_table[ 2 ][ 2 ][ 2 ] = {
    {
      { TSA_SCROLL_X1, TSA_SCROLL_Y1 },
      { TSA_SCROLL_X2, TSA_SCROLL_Y2 },
    },
    {
      { TSA_SCROLL_X1, TSA_SCROLL_Y1 },
      { TSA_SCROLL_X0, TSA_SCROLL_Y0 },
    }
  };

  if( --tsa->wait )
  {
    return;
  }

  tsa->wait = TSA_WAIT;

  GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_scroll_table[ tsa->dir ][ tsa->count ][ TSA_SCROLL_X ] );
  GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_scroll_table[ tsa->dir ][ tsa->count++ ][ TSA_SCROLL_Y ] );

  if( tsa->count >= 2 )
  {
    BTLV_INPUT_FreeTCB( tsa->biw, tcb );
  }
}

static  void  TCB_ScreenAnime_CB( GFL_TCB* tcb )
{
  TCB_SCREEN_ANIME* tsa = ( TCB_SCREEN_ANIME * )GFL_TCB_GetWork( tcb );

  tsa->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  ボタンアニメ処理セットアップ
 *
 *  @param[in]  biw       システム管理構造体のポインタ
 *  @param[in]  type      ボタンタイプ
 *  @param[in]  anm_type  ボタンアニメタイプ
 */
//============================================================================================
static  void  SetupButtonAnime( BTLV_INPUT_WORK* biw, BUTTON_TYPE type, BUTTON_ANIME_TYPE anm_type )
{
  TCB_BUTTON_ANIME* tba = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_BUTTON_ANIME ) );
  GFL_CLWK_DATA button = {
    0, 0,     //x, y
    0, 0, 2,  //アニメ番号、優先順位、BGプライオリティ
  };
  static  const BUTTON_ANIME_PARAM bap[ BUTTON_TYPE_MAX ] = {
    {
      //BUTTON_TYPE_WAZA & BUTTON_TYPE_DIR_4
      {
        { BUTTON4_X1, BUTTON4_Y1 },
        { BUTTON4_X2, BUTTON4_Y2 },
        { BUTTON4_X3, BUTTON4_Y3 },
        { BUTTON4_X4, BUTTON4_Y4 },
        { -1, -1 },
        { -1, -1 },
      },
      { BUTTON4_APPEAR_ANIME, BUTTON4_VANISH_ANIME },
    },
    {
      //BUTTON_TYPE_DIR_6
      {
        { BUTTON6_X1, BUTTON6_Y1 },
        { BUTTON6_X2, BUTTON6_Y2 },
        { BUTTON6_X3, BUTTON6_Y3 },
        { BUTTON6_X4, BUTTON6_Y4 },
        { BUTTON6_X5, BUTTON6_Y5 },
        { BUTTON6_X6, BUTTON6_Y6 },
      },
      { BUTTON6_APPEAR_ANIME, BUTTON6_VANISH_ANIME },
    },
    {
      //BUTTON_TYPE_YES_NO
      {
        { BUTTON2_X1, BUTTON2_Y1 },
        { BUTTON2_X2, BUTTON2_Y2 },
        { -1, -1 },
        { -1, -1 },
        { -1, -1 },
        { -1, -1 },
      },
      { BUTTON2_APPEAR_ANIME, BUTTON2_VANISH_ANIME },
    },
  };
  int i;

  tba->biw = biw;
  tba->clunit = GFL_CLACT_UNIT_Create( BUTTON_ANIME_MAX, 0, biw->heapID );

  for( i = 0 ; i < BUTTON_ANIME_MAX ; i++ )
  {
    if( bap[ type ].pos[ i ].x == -1 )
    {
      tba->clwk[ i ] = NULL;
      continue;
    }
    tba->clwk[ i ] = GFL_CLACT_WK_Create( tba->clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                          &button, CLSYS_DEFREND_SUB, biw->heapID );
    GFL_CLACT_WK_SetPos( tba->clwk[ i ], &bap[ type ].pos[ i ], CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetAffineParam( tba->clwk[ i ], CLSYS_AFFINETYPE_DOUBLE );
    GFL_CLACT_WK_SetAutoAnmFlag( tba->clwk[ i ], TRUE );
    GFL_CLACT_WK_SetAnmSeq( tba->clwk[ i ], bap[ type ].anm_no[ anm_type ] );
  }

  BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_ButtonAnime, tba, 0 ), TCB_ButtonAnime_CB );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *  @brief  ボタンアニメ処理タスク
 */
//============================================================================================
static  void  TCB_ButtonAnime( GFL_TCB* tcb, void* work )
{
  TCB_BUTTON_ANIME* tba = ( TCB_BUTTON_ANIME * )work;
  int i;

  for( i = 0 ; i < BUTTON_ANIME_MAX ; i++ )
  {
    if( tba->clwk[ i ] )
    {
      if( GFL_CLACT_WK_CheckAnmActive( tba->clwk[ i ] ) )
      {
        return;
      }
    }
  }
  BTLV_INPUT_FreeTCB( tba->biw, tcb );
}

static  void  TCB_ButtonAnime_CB( GFL_TCB* tcb )
{
  TCB_BUTTON_ANIME* tba = ( TCB_BUTTON_ANIME * )GFL_TCB_GetWork( tcb );
  int i;

  for( i = 0 ; i < BUTTON_ANIME_MAX ; i++ )
  {
    if( tba->clwk[ i ] )
    {
      GFL_CLACT_WK_Remove( tba->clwk[ i ] );
    }
  }
  GFL_CLACT_UNIT_Delete( tba->clunit );

  tba->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  ボールゲージ移動処理セットアップ
 *
 *  @param[in]  biw  システム管理構造体のポインタ
 *  @param[in]  dir  移動方向
 */
//============================================================================================
static  void  SetupBallGaugeMove( BTLV_INPUT_WORK* biw, BALL_GAUGE_MOVE_DIR dir )
{
  TCB_BALL_GAUGE_MOVE* tbgm = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_BALL_GAUGE_MOVE ) );

  tbgm->biw           = biw;
  tbgm->move_dir      = dir;

  BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_BallGaugeMove, tbgm, 0 ), TCB_BallGaugeMove_CB );

  biw->tcb_execute_count++;
}

//============================================================================================
/**
 *  @brief  ボールゲージ移動処理タスク
 */
//============================================================================================
static  void  TCB_BallGaugeMove( GFL_TCB* tcb, void* work )
{
  TCB_BALL_GAUGE_MOVE*  tbgm = ( TCB_BALL_GAUGE_MOVE * )work;
  BTLV_INPUT_CLWK*      bib;
  GFL_CLACTPOS          pos;
  int                   i, type;
  BOOL                  flag = FALSE;

  static  const int ball_gauge_move_param[ 2 ][ 2 ] = {
    //MINE
    { BTLV_INPUT_BALLGAUGE_MINE_Y_OPEN, BTLV_INPUT_BALLGAUGE_MINE_Y_CLOSE },
    //ENEMY
    { BTLV_INPUT_BALLGAUGE_ENEMY_Y_OPEN, BTLV_INPUT_BALLGAUGE_ENEMY_Y_CLOSE },
  };

  for( type = BALL_GAUGE_TYPE_MINE ; type < BALL_GAUGE_TYPE_MAX ; type++ )
  {
    int move_pos = ball_gauge_move_param[ type ][ tbgm->move_dir ];

    if( type == BALL_GAUGE_TYPE_MINE )
    {
      bib = tbgm->biw->ballgauge_mine;
    }
    else
    {
      if( tbgm->biw->trainer_flag == 0 )
      {
        break;
      }
      bib = tbgm->biw->ballgauge_enemy;
    }
    for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
    {
      GF_ASSERT( bib[ i ].clwk != NULL );
      GFL_CLACT_WK_GetPos( bib[ i ].clwk, &pos, CLSYS_DEFREND_SUB );
      if( pos.y > move_pos )
      {
        pos.y -= BALL_GAUGE_MOVE_VALUE;
        flag = TRUE;
      }
      else if( pos.y < move_pos )
      {
        pos.y += BALL_GAUGE_MOVE_VALUE;
        flag = TRUE;
      }
      GFL_CLACT_WK_SetPos( bib[ i ].clwk, &pos, CLSYS_DEFREND_SUB );
    }
  }
  if( flag == FALSE )
  {
    BTLV_INPUT_FreeTCB( tbgm->biw, tcb );
  }
}

static  void  TCB_BallGaugeMove_CB( GFL_TCB* tcb )
{
  TCB_BALL_GAUGE_MOVE*  tbgm = ( TCB_BALL_GAUGE_MOVE * )GFL_TCB_GetWork( tcb );

  tbgm->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  画面スクロール値セット
 */
//============================================================================================
static  void  SetupSetScroll( BTLV_INPUT_WORK* biw, int frame, int scr_x, int scr_y,
                              BG_VISIBLE frame0, BG_VISIBLE frame1, BG_VISIBLE frame2, BG_VISIBLE frame3 )
{
  TCB_SET_SCROLL* tss = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_SET_SCROLL ) );

  tss->biw    = biw;
  tss->frame  = frame;
  tss->scr_x  = scr_x;
  tss->scr_y  = scr_y;
  tss->frame0 = frame0;
  tss->frame1 = frame1;
  tss->frame2 = frame2;
  tss->frame3 = frame3;

  BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_SetScroll, tss, 0 ), NULL );
}

static  void  TCB_SetScroll( GFL_TCB* tcb, void* work )
{
  TCB_SET_SCROLL* tss = ( TCB_SET_SCROLL* )work;

  if( tss->frame != BG_NO_FRAME )
  {
    GFL_BG_SetScroll( tss->frame, GFL_BG_SCROLL_X_SET, tss->scr_x );
    GFL_BG_SetScroll( tss->frame, GFL_BG_SCROLL_Y_SET, tss->scr_y );
  }

  if( tss->frame0 != BG_VISIBLE_NO_SET )
  {
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, tss->frame0 );
  }
  if( tss->frame1 != BG_VISIBLE_NO_SET )
  {
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, tss->frame1 );
  }
  if( tss->frame2 != BG_VISIBLE_NO_SET )
  {
    GFL_BG_SetVisible( GFL_BG_FRAME2_S, tss->frame2 );
  }
  if( tss->frame3 != BG_VISIBLE_NO_SET )
  {
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, tss->frame3 );
  }

  BTLV_INPUT_FreeTCB( tss->biw, tcb );
}

//============================================================================================
/**
 *  @brief  画面フェード
 */
//============================================================================================
static  void  TCB_Fade( GFL_TCB* tcb, void* work )
{
  TCB_FADE_ACT* tfa = ( TCB_FADE_ACT* )work;

  if( ( !PaletteFadeCheck( tfa->biw->pfd ) ) && ( GFL_FADE_CheckFade() == FALSE ) )
  {
    if( tfa->biw->fade_flag == BTLV_INPUT_FADE_OUT )
    {
      BTLV_INPUT_ExitBG( tfa->biw );
    }
    BTLV_INPUT_FreeTCB( tfa->biw, tcb );
  }
}

static  void  TCB_Fade_CB( GFL_TCB* tcb )
{
  TCB_FADE_ACT* tfa = ( TCB_FADE_ACT* )GFL_TCB_GetWork( tcb );

  tfa->biw->fade_flag = 0;
}

//============================================================================================
/**
 *  @brief  天候アイコン移動
 */
//============================================================================================
static  void  TCB_WeatherIconMove( GFL_TCB* tcb, void* work )
{
  TCB_WEATHER_ICON_MOVE*  twim = ( TCB_WEATHER_ICON_MOVE* )work;
  GFL_CLACTPOS  pos;

  GFL_CLACT_WK_GetPos( twim->biw->weather_wk[ 0 ], &pos, CLSYS_DEFREND_SUB );
  pos.x -= BTLV_INPUT_WEATHER_MOVE_SPEED;
  GFL_CLACT_WK_SetPos( twim->biw->weather_wk[ 0 ], &pos, CLSYS_DEFREND_SUB );
  GFL_CLACT_WK_GetPos( twim->biw->weather_wk[ 1 ], &pos, CLSYS_DEFREND_SUB );
  pos.x -= BTLV_INPUT_WEATHER_MOVE_SPEED;
  GFL_CLACT_WK_SetPos( twim->biw->weather_wk[ 1 ], &pos, CLSYS_DEFREND_SUB );

  if( pos.x == BTLV_INPUT_WEATHER_X2 )
  {
    BTLV_INPUT_FreeTCB( twim->biw, tcb );
  }
}

static  void  TCB_WeatherIconMove_CB( GFL_TCB* tcb )
{
  TCB_WEATHER_ICON_MOVE*  twim = ( TCB_WEATHER_ICON_MOVE* )GFL_TCB_GetWork( tcb );

  twim->biw->tcb_execute_count--;
}

//============================================================================================
/**
 *  @brief  ローテーションセットアップ
 *
 *  @param[in]  biw  システム管理構造体のポインタ
 *  @param[in]  dir  どっち回転か？（0：左回り　1:右回り）
 */
//============================================================================================
static  void  SetupRotateAction( BTLV_INPUT_WORK* biw, int dir )
{
#ifdef ROTATION_NEW_SYSTEM
  TCB_TRANSFORM_WORK* ttw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_TRANSFORM_WORK ) );
  int eff = rotate_camera_focus[ biw->rotate_scr ][ dir - 5 ];

  biw->rotate_scr = rotate_screen_table[ biw->rotate_scr ][ dir - 5 ];

  biw->tcb_execute_flag = 1;
  ttw->biw = biw;
  BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_TransformRotate2Rotate, ttw, 1 ), TCB_Transform_CB );

  BTLV_EFFECT_SetCameraWorkSwitch( BTLV_EFFECT_CWE_NO_STOP );
  BTLV_EFFECT_Add( eff );
#else
  int i, j;
  int old_rotate_pos = biw->rotate_flag;
  TCB_ROTATE_ACTION*  tra = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_ROTATE_ACTION ) );

  BTLV_INPUT_ClearScreen( biw );

  if( biw->rotate_flag )
  {
    biw->rotate_flag = 0;
    biw->rotate_scr = bird[ biw->before_select_dir ].init_scr;
    if( ( dir == 0 ) && ( ( biw->rotate_scr == ROTATE_SCR_C ) || ( biw->rotate_scr == ROTATE_SCR_C_ALL ) ) )
    {
      biw->cursor_pos = 1;
    }
  }
  else
  {
    if( dir )
    {
      biw->rotate_flag = 1;
      biw->rotate_scr = bird[ biw->before_select_dir ].right_scr;
    }
    else
    {
      biw->rotate_flag = 2;
      biw->rotate_scr = bird[ biw->before_select_dir ].left_scr;
    }
  }

  SetupScrollUp( biw, rotate_scroll_table[ biw->rotate_scr ][ 0 ], rotate_scroll_table[ biw->rotate_scr ][ 1 ],
                 ROTATE_SCROLL_SPEED, ROTATE_SCROLL_COUNT );

  for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
  {
    int now_pos = rotate_pos_table[ old_rotate_pos ][ i ];
    int end_pos = rotate_pos_table[ biw->rotate_flag ][ i ];

    tra->now_pos[ now_pos ].x = pokeicon_pos[ i ].x << FX32_SHIFT;
    tra->now_pos[ now_pos ].y = pokeicon_pos[ i ].y << FX32_SHIFT;
    tra->now_pos[ now_pos ].z = 0;
    tra->end_pos[ end_pos ].x = pokeicon_pos[ i ].x << FX32_SHIFT;
    tra->end_pos[ end_pos ].y = pokeicon_pos[ i ].y << FX32_SHIFT;
    tra->end_pos[ end_pos ].z = 0;
  }
  for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
  {
    BTLV_EFFTOOL_CalcMoveVector( &tra->now_pos[ i ], &tra->end_pos[ i ], &tra->vector[ i ], ROTATE_FRAME << FX32_SHIFT );
  }

  tra->biw = biw;
  tra->frame = ROTATE_FRAME;

  GFL_TCB_AddTask( biw->tcbsys, TCB_RotateAction, tra, 0 );
  biw->tcb_execute_flag = 1;
#endif
}

//============================================================================================
/**
 *  @brief  ローテーション
 */
//============================================================================================
static  void  TCB_RotateAction( GFL_TCB* tcb, void* work )
{
#ifndef ROTATION_NEW_SYSTEM
  TCB_ROTATE_ACTION*  tra = ( TCB_ROTATE_ACTION* )work;

  switch( tra->seq_no ){
  case 0:
    {
      int           i;
      GFL_CLACTPOS  pos;

      for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
      {
        if( tra->biw->pokeicon_wk[ i ].clwk )
        {
          BTLV_EFFTOOL_CheckVecMove( &tra->now_pos[ i ], &tra->vector[ i ], &tra->end_pos[ i ], NULL );
          pos.x = tra->now_pos[ i ].x >> FX32_SHIFT;
          pos.y = tra->now_pos[ i ].y >> FX32_SHIFT;
          GFL_CLACT_WK_SetPos( tra->biw->pokeicon_wk[ i ].clwk, &pos, CLSYS_DEFREND_SUB );
        }
      }
      tra->frame--;
      if( tra->frame == 0 )
      {
        tra->seq_no++;
      }
    }
    break;
  default:
  case 1:
    if( tra->biw->tcb_execute_flag == 1 )
    {
      int i;

      for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
      {
        tra->biw->button_exist[ i ] = button_exist_table[ tra->biw->rotate_scr ][ i ];
      }
      BTLV_INPUT_CreateRotateScreen( tra->biw );
      GFL_BMPWIN_MakeScreen( tra->biw->bmp_win );
      GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
      GFL_BMPWIN_TransVramCharacter( tra->biw->bmp_win );
      tra->biw->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( tra );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
#endif
}

//============================================================================================
/**
 *  @brief  バッグモードをBTLV_EFFECTにセット
 */
//============================================================================================
static  void  TCB_BagModeSet( GFL_TCB* tcb, void* work )
{
  TCB_BAGMODE_SET*  tbs = ( TCB_BAGMODE_SET* )work;
  if( BTLV_EFFECT_GetEffectWork() != NULL )
  {
    BTLV_EFFECT_SetBagMode( tbs->bagMode );
    BTLV_INPUT_FreeTCB( tbs->biw, tcb );
  }
}

//============================================================================================
/**
 *  @brief  ウインドウマスクをセット
 */
//============================================================================================
static  void  TCB_SetWindowMask( GFL_TCB* tcb, void* work )
{
  TCB_WINDOW_MASK* twm = ( TCB_WINDOW_MASK* )work;

  GXS_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );
  G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_ALL, ( twm->max > 1 ) );
  G2S_SetWnd1InsidePlane( GX_WND_PLANEMASK_ALL, ( twm->max == 3 ) );
  G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_ALL, FALSE );
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ, -8 );

  BTLV_INPUT_FreeTCB( twm->biw, tcb );
}
//============================================================================================
/**
 *  @brief  ウインドウマスクをクリア
 */
//============================================================================================
static  void  TCB_ClearWindowMask( GFL_TCB* tcb, void* work )
{
  TCB_WINDOW_MASK* twm = ( TCB_WINDOW_MASK* )work;

  GXS_SetVisibleWnd( GX_WNDMASK_NONE );
  G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
  G2S_SetWnd1InsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
  G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_NONE, 0 );

  BTLV_INPUT_FreeTCB( twm->biw, tcb );
}

//--------------------------------------------------------------
/**
 * @brief 文字列の長さを取得する
 *
 * @param[in] str           文字列へのポインタ
 * @param[in] font          フォントタイプ
 * @param[in] ret_dot_len   ドット幅代入先
 * @param[in] ret_char_len  キャラ幅代入先
 */
//--------------------------------------------------------------
static void FontLenGet( const STRBUF *str, GFL_FONT *font, int *ret_dot_len, int *ret_char_len )
{
  int dot_len, char_len;

  //文字列のドット幅から、使用するキャラ数を算出する
  dot_len = PRINTSYS_GetStrWidth( str, font, 0 );
  char_len = dot_len / 8;
  if( FX_ModS32( dot_len, 8 ) != 0 ){
    char_len++;
  }

  *ret_dot_len = dot_len;
  *ret_char_len = char_len;
}

//--------------------------------------------------------------
/**
 * @brief   技選択画面BG生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateWazaScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_WAZA_PARAM *biwp )
{
  int i;
  int dot_len, char_len;
  ARCDATID  datID;
  STRBUF *wazaname_p;
  STRBUF *wazaname_src;
  STRBUF *pp_p;
  STRBUF *pp_src;
  STRBUF *ppmsg_src;
  WORDSET *wordset;
  PRINTSYS_LSB color;
  static const int wazaname_pos[ PTL_WAZA_MAX ][ 2 ] =
  {
    { WAZANAME_X1, WAZANAME_Y1 },
    { WAZANAME_X2, WAZANAME_Y2 },
    { WAZANAME_X3, WAZANAME_Y3 },
    { WAZANAME_X4, WAZANAME_Y4 },
  };
  int ppmsg_pos[ PTL_WAZA_MAX ][ 2 ] =
  {
    { PPMSG_X1, PPMSG_Y1 },
    { PPMSG_X2, PPMSG_Y2 },
    { PPMSG_X3, PPMSG_Y3 },
    { PPMSG_X4, PPMSG_Y4 },
  };
  int pp_pos[ PTL_WAZA_MAX ][ 2 ] =
  {
    { PP_X1, PP_Y1 },
    { PP_X2, PP_Y2 },
    { PP_X3, PP_Y3 },
    { PP_X4, PP_Y4 },
  };

  wazaname_p = GFL_STR_CreateBuffer( BUFLEN_WAZA_NAME, biw->heapID );
  wazaname_src = GFL_MSG_CreateString( biw->msg, BI_WazaNameMsg );
  ppmsg_src = GFL_MSG_CreateString( biw->msg, BI_PPMsg );
  wordset = WORDSET_Create( biw->heapID );
  pp_p = GFL_STR_CreateBuffer( BUFLEN_BI_WAZAPP, biw->heapID );
  pp_src = GFL_MSG_CreateString( biw->msg,  BI_PPNowMaxMsg );

  GFL_CLACT_UNIT_SetDrawEnable( biw->wazatype_clunit, FALSE );

  for(i = 0; i < PTL_WAZA_MAX; i++)
  {
    if( biwp->wazano[ i ] )
    {
      //技タイプアイコン
      {
        void *arc_data;
        void *obj_vram;
        NNSG2dImageProxy image;
        NNSG2dCharacterData *char_data;
        int waza_type;
        static  const ARCDATID  waza_type_pltt[]=
        {
          NARC_battgra_wb_waza_w_01_NCLR,
          NARC_battgra_wb_waza_w_07_NCLR,
          NARC_battgra_wb_waza_w_10_NCLR,
          NARC_battgra_wb_waza_w_08_NCLR,
          NARC_battgra_wb_waza_w_09_NCLR,
          NARC_battgra_wb_waza_w_13_NCLR,
          NARC_battgra_wb_waza_w_12_NCLR,
          NARC_battgra_wb_waza_w_14_NCLR,
          NARC_battgra_wb_waza_w_16_NCLR,
          NARC_battgra_wb_waza_w_02_NCLR,
          NARC_battgra_wb_waza_w_03_NCLR,
          NARC_battgra_wb_waza_w_05_NCLR,
          NARC_battgra_wb_waza_w_04_NCLR,
          NARC_battgra_wb_waza_w_06_NCLR,
          NARC_battgra_wb_waza_w_15_NCLR,
          NARC_battgra_wb_waza_w_11_NCLR,
          NARC_battgra_wb_waza_w_17_NCLR,
        };
        waza_type = WAZADATA_GetParam( biwp->wazano[ i ], WAZAPARAM_TYPE );
        datID = waza_type_pltt[ waza_type ];

        biw->wazatype_wk[ i ] = GFL_CLACT_WK_Create( biw->wazatype_clunit, biw->wazatype_charID[ i ],
                                                     biw->wazatype_plttID, biw->wazatype_cellID,
                                                     &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
        GFL_CLACT_WK_SetPos( biw->wazatype_wk[ i ], &WazaIconPos[ i ], CLSYS_DEFREND_SUB );

        arc_data = GFL_ARC_UTIL_LoadOBJCharacter( APP_COMMON_GetArcId(),
                                                  APP_COMMON_GetPokeTypeCharArcIdx( waza_type ), WAZATYPEICON_COMP_CHAR,
                                                  &char_data, GFL_HEAP_LOWID( biw->heapID ) );

        GFL_CLACT_WK_SetPlttOffs( biw->wazatype_wk[ i ], APP_COMMON_GetPokeTypePltOffset( waza_type ), CLWK_PLTTOFFS_MODE_PLTT_TOP );
        obj_vram = G2S_GetOBJCharPtr();
        GFL_CLACT_WK_GetImgProxy( biw->wazatype_wk[ i ],  &image );
        MI_CpuCopy16( char_data->pRawData,
                      ( void * )( ( u32 )obj_vram + image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DSUB ] ),
                      WAZATYPEICON_OAMSIZE );

        GFL_HEAP_FreeMemory( arc_data );
      }

      {
        u8 letter, shadow, back;
        GFL_FONTSYS_GetColor( &letter, &shadow, &back );

        //BMPWIN：技名
        GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                              PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                              PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );
        WORDSET_RegisterWazaName( wordset, 0, biwp->wazano[ i ] );
        WORDSET_ExpandStr( wordset, wazaname_p, wazaname_src );
        FontLenGet( wazaname_p, biw->font, &dot_len, &char_len );
        PRINTSYS_Print( biw->bmp_data, wazaname_pos[ i ][ 0 ] - ( dot_len / 2 ), wazaname_pos[ i ][ 1 ],
                        wazaname_p, biw->font );

        WORDSET_RegisterNumber(wordset, 0, biwp->pp[ i ],     2, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );
        WORDSET_RegisterNumber(wordset, 1, biwp->ppmax[ i ],  2, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );
        WORDSET_ExpandStr(wordset, pp_p, pp_src);
        color = PP_FontColorGet( biwp->pp[ i ], biwp->ppmax[ i ] );


        GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( color ), PRINTSYS_LSB_GetS( color ), PRINTSYS_LSB_GetB( color ) );

        //BMPWIN：PP
        PRINTSYS_Print( biw->bmp_data, ppmsg_pos[ i ][ 0 ], ppmsg_pos[ i ][ 1 ], ppmsg_src, biw->font );
        FontLenGet( pp_p, biw->font, &dot_len, &char_len );
        PRINTSYS_Print( biw->bmp_data, pp_pos[ i ][ 0 ] - ( dot_len / 2 ), pp_pos[ i ][ 1 ], pp_p, biw->font );
        GFL_FONTSYS_SetColor( letter, shadow, back );
      }
      biw->button_exist[ i ] = ( biwp->pp[ i ] != 0 );  //押せるボタンかどうかチェック
      biw->waza_exist[ i ] = ( biwp->wazano[ i ] != 0 );
    }
    else
    {
      datID = NARC_battgra_wb_waza_w_00_NCLR;
    }
    {
      NNSG2dPaletteData* palData;
      void* dat = GFL_ARC_UTIL_LoadPalette( ARCID_BATTGRA, datID, &palData, GFL_HEAP_LOWID( biw->heapID ) );

      PaletteWorkSet( biw->pfd, palData->pRawData, FADE_SUB_BG, WAZATYPE_PLTT + i * 0x10, 0x20 );

      GFL_HEAP_FreeMemory( dat );
    }

  }
  biw->button_exist[ i ] = TRUE;  //押せるボタンかどうかチェック
  //3vs3のときは移動ボタンも存在するので、2個分押せるボタンにする
  if( biw->type == BTLV_INPUT_TYPE_TRIPLE )
  {
    biw->button_exist[ i + 1 ] = TRUE;  //押せるボタンかどうかチェック
  }

  WORDSET_Delete( wordset );
  GFL_STR_DeleteBuffer( wazaname_p );
  GFL_STR_DeleteBuffer( ppmsg_src );
  GFL_STR_DeleteBuffer( wazaname_src );
  GFL_STR_DeleteBuffer( pp_src );
  GFL_STR_DeleteBuffer( pp_p );
}

//--------------------------------------------------------------
/**
 * @brief   攻撃対象選択画面BG生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateDirScreen( BTLV_INPUT_WORK* biw, TCB_TRANSFORM_WORK* ttw, const BTLV_INPUT_SCENE_PARAM *bisp )
{
  int i;
  int dot_len, char_len;
  STRBUF *monsname_p;
  STRBUF *monsname_src;
  WORDSET *wordset;
  static  const int monsname_pos[ 2 ][ BTLV_INPUT_DIR_MAX ][ 2 ] =
  {
    {
      { MONSNAME4_X3, MONSNAME4_Y3 },
      { MONSNAME4_X2, MONSNAME4_Y2 },
      { MONSNAME4_X4, MONSNAME4_Y4 },
      { MONSNAME4_X1, MONSNAME4_Y1 },
      { NULL, NULL },
      { NULL, NULL },
    },
    {
      { MONSNAME6_X4, MONSNAME6_Y4 },
      { MONSNAME6_X3, MONSNAME6_Y3 },
      { MONSNAME6_X5, MONSNAME6_Y5 },
      { MONSNAME6_X2, MONSNAME6_Y2 },
      { MONSNAME6_X6, MONSNAME6_Y6 },
      { MONSNAME6_X1, MONSNAME6_Y1 },
    },
  };
  int max = ( biw->type == BTLV_INPUT_TYPE_TRIPLE ) ? BTLV_INPUT_DIR_MAX : 4;
  int type = ( biw->type == BTLV_INPUT_TYPE_TRIPLE ) ? BUTTON_TYPE_DIR_6 : BUTTON_TYPE_DIR_4;
  u8 letter, shadow, back;

  GFL_FONTSYS_GetColor( &letter, &shadow, &back );

  monsname_p = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, GFL_HEAP_LOWID(biw->heapID) );
  wordset = WORDSET_Create( GFL_HEAP_LOWID(biw->heapID) );

  GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                        PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                        PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );

  for( i = 0 ; i < max ; i++){
    if( bisp->bidp[ i ].hp ){
      //BMPWIN：ポケモン名
      monsname_src = GFL_MSG_CreateString( biw->msg, BI_TargetPokemonMaleMsg + bisp->bidp[ i ].sex );
      WORDSET_RegisterPokeNickName( wordset, 0, bisp->bidp[ i ].pp );
      WORDSET_ExpandStr( wordset, monsname_p, monsname_src );
      FontLenGet( monsname_p, biw->font, &dot_len, &char_len );
      PRINTSYS_Print( biw->bmp_data, monsname_pos[ type ][ i ][ 0 ] - ( dot_len / 2 ),
                      monsname_pos[ type ][ i ][ 1 ], monsname_p, biw->font );
      GFL_STR_DeleteBuffer( monsname_src );
    }
    biw->button_exist[ i ] = bisp->bidp[ i ].exist;  //押せるボタンかどうかチェック
  }
  biw->button_exist[ i ] = TRUE;  //押せるボタンかどうかチェック
  GFL_FONTSYS_SetColor( letter, shadow, back );

  WORDSET_Delete( wordset );
  GFL_STR_DeleteBuffer( monsname_p );

  //選択枠表示
  {
    int       pos = ( bisp->pos - BTLV_MCSS_POS_A ) / 2;
    static  const ARCDATID  datID[ 2 ][ 3 ][ WAZA_TARGET_MAX + 1 ] = {
      //BUTTON_TYPE_DIR_4
      {
        //BTLV_MCSS_POS_A
        {
          NARC_battgra_wb_btl_sel_normal_l_NSCR,  ///< 自分以外１体（選択）
          NARC_battgra_wb_btl_sel_mine_sel_NSCR,  ///< 自分を含む味方１体（選択）
          NARC_battgra_wb_btl_sel_mine_r_NSCR,    ///< 自分以外の味方１体（選択）
          NARC_battgra_wb_btl_sel_enemy_sel_NSCR, ///< 相手側１体（選択）
          NARC_battgra_wb_btl_sel_nomine_l_NSCR,  ///< 自分以外全て
          NARC_battgra_wb_btl_sel_enemy_NSCR,     ///< 相手側全ポケ
          NARC_battgra_wb_btl_sel_mine_NSCR,      ///< 味方側全ポケ
          NARC_battgra_wb_btl_sel_mine_l_NSCR,    ///< 自分のみ
          NARC_battgra_wb_btl_sel_all_NSCR,       ///< 場に出ているポケモン全て
          NARC_battgra_wb_btl_sel_mine_l_NSCR,    ///< 相手側１体ランダム
          NARC_battgra_wb_btl_sel_all_NSCR,       ///< 場全体（天候など）
          NARC_battgra_wb_btl_sel_enemy_NSCR,     ///< 敵側陣営
          NARC_battgra_wb_btl_sel_mine_NSCR,      ///< 自分側陣営
          NARC_battgra_wb_btl_sel_mine_l_NSCR,    ///< ゆびをふるなど特殊型
          NARC_battgra_wb_btl_sel_normal_l_NSCR,  ///< 自分以外１体（遠距離）
        },
        //BTLV_MCSS_POS_C
        {
          NARC_battgra_wb_btl_sel_normal_r_NSCR,  ///< 自分以外１体（選択）
          NARC_battgra_wb_btl_sel_mine_sel_NSCR,  ///< 自分を含む味方１体（選択）
          NARC_battgra_wb_btl_sel_mine_l_NSCR,    ///< 自分以外の味方１体（選択）
          NARC_battgra_wb_btl_sel_enemy_sel_NSCR, ///< 相手側１体（選択）
          NARC_battgra_wb_btl_sel_nomine_r_NSCR,  ///< 自分以外全て
          NARC_battgra_wb_btl_sel_enemy_NSCR,     ///< 相手側全ポケ
          NARC_battgra_wb_btl_sel_mine_NSCR,      ///< 味方側全ポケ
          NARC_battgra_wb_btl_sel_mine_r_NSCR,    ///< 自分のみ
          NARC_battgra_wb_btl_sel_all_NSCR,       ///< 場に出ているポケモン全て
          NARC_battgra_wb_btl_sel_mine_r_NSCR,    ///< 相手側１体ランダム
          NARC_battgra_wb_btl_sel_all_NSCR,       ///< 場全体（天候など）
          NARC_battgra_wb_btl_sel_enemy_NSCR,     ///< 敵側陣営
          NARC_battgra_wb_btl_sel_mine_NSCR,      ///< 自分側陣営
          NARC_battgra_wb_btl_sel_mine_r_NSCR,    ///< ゆびをふるなど特殊型
          NARC_battgra_wb_btl_sel_normal_r_NSCR,  ///< 自分以外１体（遠距離）
        },
        //BTLV_MCSS_POS_E
        {
          0xffffffff,                             ///< 自分以外１体（選択）
          0xffffffff,                             ///< 自分以外全て
          0xffffffff,                             ///< 相手側１体（選択）
          0xffffffff,                             ///< 相手側全ポケ
          0xffffffff,                             ///< 相手側１体ランダム
          0xffffffff,                             ///< 自分を含む味方１体（選択）
          0xffffffff,                             ///< 自分以外の味方１体（選択）
          0xffffffff,                             ///< 自分のみ
          0xffffffff,                             ///< 場に出ているポケモン全て
          0xffffffff,                             ///< 味方側全ポケ
          0xffffffff,                             ///< 自分側陣営
          0xffffffff,                             ///< 敵側陣営
          0xffffffff,                             ///< 場全体（天候など）
          0xffffffff,                             ///< ゆびをふるなど特殊型
          0xffffffff,                             ///< 自分以外１体（遠距離）
        },
      },
      //BUTTON_TYPE_DIR_6
      {
        //BTLV_MCSS_POS_A
        {
          NARC_battgra_wb_btl_sel3_normal_l_NSCR,     ///< 自分以外１体（選択）
          NARC_battgra_wb_btl_sel3_mine_sel_l_NSCR,   ///< 自分を含む味方１体（選択）
          NARC_battgra_wb_btl_sel3_pair_l_NSCR,       ///< 自分以外の味方１体（選択）
          NARC_battgra_wb_btl_sel3_enemy_sel_l_NSCR,  ///< 相手側１体（選択）
          NARC_battgra_wb_btl_sel3_nomine_l_NSCR,     ///< 自分以外全て
          NARC_battgra_wb_btl_sel3_enemy_l_NSCR,      ///< 相手側全て
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 味方側全ポケ
          NARC_battgra_wb_btl_sel3_mine_l_NSCR,       ///< 自分のみ
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場に出ているポケモン全て
          NARC_battgra_wb_btl_sel3_mine_l_NSCR,       ///< 相手側１体ランダム
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場全体（天候など）
          NARC_battgra_wb_btl_sel3_enemy_c_NSCR,      ///< 敵側陣営
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 自分側陣営
          NARC_battgra_wb_btl_sel3_mine_l_NSCR,       ///< ゆびをふるなど特殊型
          NARC_battgra_wb_btl_sel3_long_l_NSCR,       ///< 自分以外１体（遠距離）
        },
        //BTLV_MCSS_POS_C
        {
          NARC_battgra_wb_btl_sel3_normal_c_NSCR,     ///< 自分以外１体（選択）
          NARC_battgra_wb_btl_sel3_mine_sel_c_NSCR,   ///< 自分を含む味方１体（選択）
          NARC_battgra_wb_btl_sel3_pair_c_NSCR,       ///< 自分以外の味方１体（選択）
          NARC_battgra_wb_btl_sel3_enemy_sel_c_NSCR,  ///< 相手側１体（選択）
          NARC_battgra_wb_btl_sel3_nomine_c_NSCR,     ///< 自分以外全て
          NARC_battgra_wb_btl_sel3_enemy_c_NSCR,      ///< 相手側全て
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 味方側全て
          NARC_battgra_wb_btl_sel3_mine_c_NSCR,       ///< 自分のみ
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場に出ているポケモン全て
          NARC_battgra_wb_btl_sel3_mine_c_NSCR,       ///< 相手側１体ランダム
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場全体（天候など）
          NARC_battgra_wb_btl_sel3_enemy_c_NSCR,      ///< 敵側陣営
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 自分側陣営
          NARC_battgra_wb_btl_sel3_mine_c_NSCR,       ///< ゆびをふるなど特殊型
          NARC_battgra_wb_btl_sel3_normal_c_NSCR,     ///< 自分以外１体（遠距離）
        },
        //BTLV_MCSS_POS_E
        {
          NARC_battgra_wb_btl_sel3_normal_r_NSCR,     ///< 自分以外１体（選択）
          NARC_battgra_wb_btl_sel3_mine_sel_r_NSCR,   ///< 自分を含む味方１体（選択）
          NARC_battgra_wb_btl_sel3_pair_r_NSCR,       ///< 自分以外の味方１体（選択）
          NARC_battgra_wb_btl_sel3_enemy_sel_r_NSCR,  ///< 相手側１体（選択）
          NARC_battgra_wb_btl_sel3_nomine_r_NSCR,     ///< 自分以外全て
          NARC_battgra_wb_btl_sel3_enemy_r_NSCR,      ///< 相手側全て
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 味方側全て
          NARC_battgra_wb_btl_sel3_mine_r_NSCR,       ///< 自分のみ
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場に出ているポケモン全て
          NARC_battgra_wb_btl_sel3_mine_r_NSCR,       ///< 相手側１体ランダム
          NARC_battgra_wb_btl_sel3_all_NSCR,          ///< 場全体（天候など）
          NARC_battgra_wb_btl_sel3_enemy_c_NSCR,      ///< 敵側陣営
          NARC_battgra_wb_btl_sel3_mine_NSCR,         ///< 自分側陣営
          NARC_battgra_wb_btl_sel3_mine_r_NSCR,       ///< ゆびをふるなど特殊型
          NARC_battgra_wb_btl_sel3_long_r_NSCR,       ///< 自分以外１体（遠距離）
        },
      },
    };
    biw->waza_target = bisp->waza_target;
    ttw->datID = datID[ type ][ pos ][ bisp->waza_target ];
  }
  //パレット転送
  {
    NNSG2dPaletteData* palData;
    void* dat;
    int max = ( biw->type == BTLV_INPUT_TYPE_TRIPLE ) ? 3 : 2;
    int i;

    for( i = 0 ; i < max ; i++ )
    {
      dat = GFL_ARC_UTIL_LoadPalette( ARCID_BATTGRA, NARC_battgra_wb_sel_up_NCLR, &palData, GFL_HEAP_LOWID( biw->heapID ) );
      PaletteWorkSet( biw->pfd, palData->pRawData, FADE_SUB_BG, DIR_PLTT + i * 0x10, 0x20 );
      GFL_HEAP_FreeMemory( dat );
    }
    for( ; i < max * 2 ; i++ )
    {
      dat = GFL_ARC_UTIL_LoadPalette( ARCID_BATTGRA, NARC_battgra_wb_sel_down_NCLR, &palData, GFL_HEAP_LOWID( biw->heapID ) );
      PaletteWorkSet( biw->pfd, palData->pRawData, FADE_SUB_BG, DIR_PLTT + i * 0x10, 0x20 );
      GFL_HEAP_FreeMemory( dat );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   YES/NO選択画面BG生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateYesNoScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_YESNO_PARAM *biyp )
{
  STRBUF*       src;
  int           dot_len, char_len;

  GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                        PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                        PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );

  FontLenGet( biyp->yes_msg, biw->font, &dot_len, &char_len );
  PRINTSYS_Print( biw->bmp_data, BTLV_INPUT_YESNO_MSG_X - ( dot_len / 2 ), BTLV_INPUT_YES_MSG_Y, biyp->yes_msg, biw->font );

  FontLenGet( biyp->no_msg, biw->font, &dot_len, &char_len );
  PRINTSYS_Print( biw->bmp_data, BTLV_INPUT_YESNO_MSG_X - ( dot_len / 2 ), BTLV_INPUT_NO_MSG_Y, biyp->no_msg, biw->font );

  //2個分押せる状態にする
  biw->button_exist[ 0 ] = TRUE;  //押せるボタンかどうかチェック
  biw->button_exist[ 1 ] = TRUE;  //押せるボタンかどうかチェック

  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg1d_NSCR,
                                   GFL_BG_FRAME1_S, 0, 0, FALSE, biw->heapID );
}

//--------------------------------------------------------------
/**
 * @brief   ROTATE画面BG生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateRotateScreen( BTLV_INPUT_WORK* biw )
{
#ifdef ROTATION_NEW_SYSTEM

  BTLV_INPUT_WAZA_PARAM biwp;
  int i;

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    biwp.wazano[ i ]  = 0;
    biwp.pp[ i ]      = 0;
    biwp.ppmax[ i ]   = 0;
  }

  for( i = 0 ; i < BPP_WAZA_GetCount( biw->rotate_bpp[ biw->rotate_scr ] ) ; i++ )
  {
    biwp.wazano[ i ] = BPP_WAZA_GetParticular( biw->rotate_bpp[ biw->rotate_scr ], i, &biwp.pp[ i ], &biwp.ppmax[ i ] );
  }
  BTLV_INPUT_CreateWazaScreen( biw, &biwp );

  biw->button_exist[ ROTATE_LEFT_BUTTON ]   = rotate_button_exist_table[ biw->rotate_scr ][ 0 ];
  biw->button_exist[ ROTATE_RIGHT_BUTTON ]  = rotate_button_exist_table[ biw->rotate_scr ][ 1 ];

  biw->waruagaki_flag = TRUE;
  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    if( biw->rotate_waza_exist[ biw->rotate_scr ][ i ] == TRUE )
    {
      biw->waruagaki_flag = FALSE;
    }
  }
  if( BPP_IsDead( biw->rotate_bpp[ biw->rotate_scr ] ) )
  {
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      biw->button_exist[ i ] = FALSE;
    }
    biw->waruagaki_flag = FALSE;
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, 0x3e00, 0, 8, 8, 0, biw->tcbsys );
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_OBJ, 0x0700, 0, 8, 8, 0, biw->tcbsys );
  }
  else if( biw->waruagaki_flag == TRUE )
  {
    biw->button_exist[ 0 ] = TRUE;
    BTLV_INPUT_CreateWaruagakiButton( biw );
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, 0x3e00, 0, 8, 8, 0, biw->tcbsys );
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_OBJ, 0x0700, 0, 8, 8, 0, biw->tcbsys );
  }
  else
  {
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, 0x3e00, 0, 0, 0, 0, biw->tcbsys );
    PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_OBJ, 0x0700, 0, 0, 0, 0, biw->tcbsys );
  }
#else
  STRBUF *monsname_p;
  STRBUF *monsname_src;
  WORDSET *wordset;
  static  const int monsname_pos[ BTLV_INPUT_POKEICON_MAX ][ 2 ] =
  {
    { ROTATE_MONSNAME_X0, ROTATE_MONSNAME_Y0 },
    { ROTATE_MONSNAME_X1, ROTATE_MONSNAME_Y1 },
    { ROTATE_MONSNAME_X2, ROTATE_MONSNAME_Y2 },
  };
  int dot_len, char_len;
  int i;

  monsname_p = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, GFL_HEAP_LOWID( biw->heapID ) );
  wordset = WORDSET_Create( GFL_HEAP_LOWID( biw->heapID ) );

  for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
  {
    int pos = rotate_pos_table[ biw->rotate_flag ][ i ];
    if( biw->rotate_pp[ pos ] )
    {
      int sex = PP_Get( biw->rotate_pp[ pos ], ID_PARA_sex, NULL );
      //BMPWIN：ポケモン名
      monsname_src = GFL_MSG_CreateString( biw->msg, BI_TargetPokemonMaleMsg + sex );
      WORDSET_RegisterPokeNickName( wordset, 0, biw->rotate_pp[ pos ] );
      WORDSET_ExpandStr( wordset, monsname_p, monsname_src );
      FontLenGet( monsname_p, biw->font, &dot_len, &char_len );
      PRINTSYS_Print( biw->bmp_data, monsname_pos[ i ][ 0 ] - ( dot_len / 2 ), monsname_pos[ i ][ 1 ], monsname_p, biw->font );
      GFL_STR_DeleteBuffer( monsname_src );
    }
  }

  WORDSET_Delete( wordset );
  GFL_STR_DeleteBuffer( monsname_p );

  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg0c_NSCR,
                                   GFL_BG_FRAME0_S, 0, 0, FALSE, biw->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( biw->handle, NARC_battgra_wb_battle_w_bg1e_NSCR,
                                   GFL_BG_FRAME1_S, 0, 0, FALSE, biw->heapID );
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ROTATE画面PokeIcon生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateRotatePokeIcon( BTLV_INPUT_WORK* biw )
{
#ifndef ROTATION_NEW_SYSTEM
  int i;
  ARCHANDLE*  hdl = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID( biw->heapID ) );

  biw->pokeicon_cellID = GFL_CLGRP_CELLANIM_Register( hdl, POKEICON_GetCellSubArcIndex(), POKEICON_GetAnmSubArcIndex(),
                                                        biw->heapID );
  biw->pokeicon_plttID = GFL_CLGRP_PLTT_RegisterComp( hdl, POKEICON_GetPalArcIndex(), CLSYS_DRAW_SUB,
                                                        0x20 * 12, biw->heapID );
  PaletteWorkSet_VramCopy( biw->pfd, FADE_SUB_OBJ,
                           GFL_CLGRP_PLTT_GetAddr( biw->pokeicon_plttID, CLSYS_DRAW_SUB ) / 2, 0x20 * 3 );

  for( i = 0 ; i < BTLV_INPUT_POKEICON_MAX ; i++ )
  {
    if( biw->rotate_pp[ i ] )
    {
      int mons_no = PP_Get( biw->rotate_pp[ i ], ID_PARA_monsno, NULL );
      int form_no = PP_Get( biw->rotate_pp[ i ], ID_PARA_form_no, NULL );
      int sex     = PP_Get( biw->rotate_pp[ i ], ID_PARA_sex, NULL );
      biw->pokeicon_charID[ i ] = GFL_CLGRP_CGR_Register( hdl,
                                                          POKEICON_GetCgxArcIndexByMonsNumber( mons_no,
                                                                                               form_no,
                                                                                               sex,
                                                                                               FALSE ),
                                                          FALSE, CLSYS_DRAW_SUB, biw->heapID );
      biw->pokeicon_wk[ i ].clwk = GFL_CLACT_WK_Create( biw->pokeicon_clunit, biw->pokeicon_charID[ i ],
                                                        biw->pokeicon_plttID, biw->pokeicon_cellID,
                                                        &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
      GFL_CLACT_WK_SetPos( biw->pokeicon_wk[ i ].clwk, &pokeicon_pos[ i ], CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_SetAutoAnmFlag( biw->pokeicon_wk[ i ].clwk, TRUE );
      GFL_CLACT_WK_SetAnmSeq( biw->pokeicon_wk[ i ].clwk, 1 );
      GFL_CLACT_WK_SetPlttOffs( biw->pokeicon_wk[ i ].clwk,
                                POKEICON_GetPalNum( mons_no, form_no, sex, FALSE ), CLWK_PLTTOFFS_MODE_OAM_COLOR );
      GFL_CLACT_WK_SetDrawEnable( biw->pokeicon_wk[ i ].clwk, FALSE );
    }
  }
  GFL_ARC_CloseDataHandle( hdl );
#endif
}

//--------------------------------------------------------------
/**
 * @brief   バトルレコーダー画面BG生成
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateBattleRecorderScreen( BTLV_INPUT_WORK* biw, const BTLV_INPUT_BATTLE_RECORDER_PARAM *bibrp )
{
  STRBUF *chapter_p;
  STRBUF *chapter_src;
  STRBUF *msg_src;
  WORDSET *wordset;
  PRINTSYS_LSB color;

  wordset = WORDSET_Create( biw->heapID );
  chapter_p = GFL_STR_CreateBuffer( BUFLEN_BR_CHAPTER, biw->heapID );
  chapter_src = GFL_MSG_CreateString( biw->msg,  BI_BattleRecorderChapter );

  {
    u8 letter, shadow, back;

    GFL_FONTSYS_GetColor( &letter, &shadow, &back );
    GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_GREEN ),
                          PRINTSYS_LSB_GetS( MSGCOLOR_PP_GREEN ),
                          PRINTSYS_LSB_GetB( MSGCOLOR_PP_GREEN ) );
    WORDSET_RegisterNumber(wordset, 0, bibrp->max_chapter,  3, STR_NUM_DISP_ZERO, STR_NUM_CODE_ZENKAKU );
    WORDSET_ExpandStr(wordset, chapter_p, chapter_src);
    PRINTSYS_Print( biw->bmp_data, BR_MAX_CHAPTER_X, BR_MAX_CHAPTER_Y, chapter_p, biw->font );

    if( bibrp->play_chapter != bibrp->view_chapter )
    {
      GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_RED ),
                            PRINTSYS_LSB_GetS( MSGCOLOR_PP_RED ),
                            PRINTSYS_LSB_GetB( MSGCOLOR_PP_RED ) );
    }

    WORDSET_RegisterNumber(wordset, 0, bibrp->view_chapter, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_ZENKAKU );
    WORDSET_ExpandStr(wordset, chapter_p, chapter_src);
    PRINTSYS_Print( biw->bmp_data, BR_PLAY_CHAPTER_X, BR_PLAY_CHAPTER_Y, chapter_p, biw->font );

    GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );

    msg_src = GFL_MSG_CreateString( biw->msg,  BI_BattleRecorderChapterSlash );
    PRINTSYS_Print( biw->bmp_data, BR_SLASH_X, BR_SLASH_Y, msg_src, biw->font );
    GFL_STR_DeleteBuffer( msg_src );

    GFL_FONTSYS_SetColor( letter, shadow, back );
  }

#if 0
  switch( bibrp->stop_flag ){
  case BTLV_INPUT_BR_STOP_KEY:    //キーによる中断
  case BTLV_INPUT_BR_STOP_BREAK:  //データ破壊による中断
  case BTLV_INPUT_BR_STOP_SKIP:   //チャプタースキップによる中断
    GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );
    msg_src = GFL_MSG_CreateString( biw->msg,  BI_BattleRecorderStopKey + bibrp->stop_flag - 1 );
    PRINTSYS_Print( biw->bmp_data, BR_MESSAGE_X, BR_MESSAGE_Y, msg_src, biw->font );
    GFL_STR_DeleteBuffer( msg_src );
    GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_X_SET, TSA_SCROLL_X1 );
    GFL_BG_SetScroll( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, TSA_SCROLL_Y1 );
    /*fall thru*/
  case BTLV_INPUT_BR_STOP_NONE:
  default:
    break;
  }
#endif

  WORDSET_Delete( wordset );
  GFL_STR_DeleteBuffer( chapter_src );
  GFL_STR_DeleteBuffer( chapter_p );

  //パレットコピー
  {
    u16*  pal = PaletteWorkDefaultWorkGet( BTLV_EFFECT_GetPfd(), FADE_SUB_BG );

    PaletteWorkSet( biw->pfd, &pal[ 0x20 ], FADE_SUB_BG, 0x70, 0x20 );
  }
}

//--------------------------------------------------------------
/**
 * @brief   画面クリア（画面推移時に消さなければならないスクリーンを消す）
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_ClearScreen( BTLV_INPUT_WORK* biw )
{
  int i;

  biw->decide_pos_flag = 0;

  for( i = 0 ; i < BTLV_INPUT_BUTTON_MAX ; i++ )
  {
    biw->button_exist[ i ] = FALSE;
  }

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    biw->waza_exist[ i ] = FALSE;
  }

  //技選択
  GFL_BMP_Clear( biw->bmp_data, 0x00 );
  GFL_BMPWIN_TransVramCharacter( biw->bmp_win );

  //シューターエネルギー
  GFL_BMP_Clear( biw->bmp_data_shooter, 0x00 );
  GFL_BMPWIN_TransVramCharacter( biw->bmp_win_shooter );

  BTLV_INPUT_DeleteWazaTypeIcon( biw );
  BTLV_INPUT_DeleteWaruagakiButton( biw );

  BTLV_INPUT_DeleteWeatherIcon( biw );

  //攻撃対象選択
  GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0, 32, 2, 32, 32, 0 );

  //パレット暗くする処理を解除
  PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_BG, 0x3e00, 0, 0, 0, 0, biw->tcbsys );
  PaletteFadeReqWrite( biw->pfd, PF_BIT_SUB_OBJ, 0x0700, 0, 0, 0, 0, biw->tcbsys );
}

//--------------------------------------------------------------
/**
 * @brief   PPの表示色を取得する
 *
 * @param   pp      現在のPP
 * @param   pp_max    最大PP
 *
 * @retval  カラー
 */
//--------------------------------------------------------------
static PRINTSYS_LSB PP_FontColorGet(int pp, int pp_max)
{
  if( pp == 0 ){
    return MSGCOLOR_PP_RED;
  }
  if( pp_max == pp ){
    return MSGCOLOR_PP_WHITE;
  }
  if( pp_max <= 2 ){
    if( pp == 1 ){
      return MSGCOLOR_PP_ORANGE;
    }
  }
  else if( pp_max <= 7 ){
    switch( pp ){
    case 1:
      return MSGCOLOR_PP_ORANGE;
    case 2:
      return MSGCOLOR_PP_YELLOW;
    }
  }
  else{
    if( pp <= ( pp_max / 4 ) ){
      return MSGCOLOR_PP_ORANGE;
    }
    if( pp <= ( pp_max / 2 ) ){
      return MSGCOLOR_PP_YELLOW;
    }
  }
  return MSGCOLOR_PP_WHITE;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコン生成
 *
 * @param[in] biw   システム管理構造体のポインタ
 * @param[in] bicp  ポケモンアイコン生成用パラメータのポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreatePokeIcon( BTLV_INPUT_WORK* biw, BTLV_INPUT_COMMAND_PARAM* bicp )
{
    ARCHANDLE*  hdl;
    int         i;
    int         max = ( biw->type == BTLV_INPUT_TYPE_ROTATION ) ? 2 : biw->type + 1;
    int         mask = 0;

#ifdef ROTATION_NEW_SYSTEM
    //シングルでは出さないように仕様変更（後でまた出すことになると困るので、処理自体は残しておく）
    //新しいローテションバトルは1vs1仕様なのでこちらも出さないようにする
    if( ( biw->type == BTLV_INPUT_TYPE_SINGLE ) || ( biw->type == BTLV_INPUT_TYPE_ROTATION ) )
    {
      return;
    }
#else
    //シングルでは出さないように仕様変更（後でまた出すことになると困るので、処理自体は残しておく）
    if( biw->type == BTLV_INPUT_TYPE_SINGLE )
    {
      return;
    }
#endif

    hdl = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID( biw->heapID ) );

    biw->pokeicon_cellID = GFL_CLGRP_CELLANIM_Register( hdl, POKEICON_GetCellSubArcIndex(), POKEICON_GetAnmSubArcIndex(),
                                                        biw->heapID );
    biw->pokeicon_plttID = GFL_CLGRP_PLTT_RegisterComp( hdl, POKEICON_GetPalArcIndex(), CLSYS_DRAW_SUB,
                                                        0x20 * 11, biw->heapID );
    PaletteWorkSet_VramCopy( biw->pfd, FADE_SUB_OBJ,
                             GFL_CLGRP_PLTT_GetAddr( biw->pokeicon_plttID, CLSYS_DRAW_SUB ) / 2, 0x20 * 3 );

    G2S_SetWnd0Position( 1, 0, 0, 0 );
    G2S_SetWnd1Position( 1, 0, 0, 0 );

    //ウインドウマスクでアイコンを暗くする実験
    if( max > 1 )
    {
      TCB_WINDOW_MASK* twm = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_WINDOW_MASK ) );
      twm->biw = biw;
      twm->max = max;
      BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_SetWindowMask, twm, 0 ), NULL );
    }

    for( i = 0; i < max ; i++ ){
      if( bicp->mons_no[ i ] )
      {
        static  const GFL_CLACTPOS pos[ 4 ][ 3 ] = {
          //SINGLE
          {
            { BTLV_INPUT_POKE_ICON_SINGLE_X, BTLV_INPUT_POKE_ICON_SINGLE_Y },
            { -1, -1 },
            { -1, -1 },
          },
          //DOUBLE
          {
            { BTLV_INPUT_POKE_ICON_DOUBLE_X1, BTLV_INPUT_POKE_ICON_DOUBLE_Y1 },
            { BTLV_INPUT_POKE_ICON_DOUBLE_X2, BTLV_INPUT_POKE_ICON_DOUBLE_Y2 },
            { -1, -1 },
          },
          //TRIPLE
          {
            { BTLV_INPUT_POKE_ICON_TRIPLE_X1, BTLV_INPUT_POKE_ICON_TRIPLE_Y1 },
            { BTLV_INPUT_POKE_ICON_TRIPLE_X2, BTLV_INPUT_POKE_ICON_TRIPLE_Y2 },
            { BTLV_INPUT_POKE_ICON_TRIPLE_X3, BTLV_INPUT_POKE_ICON_TRIPLE_Y3 },
          },
          //ROTATION
          {
            { BTLV_INPUT_POKE_ICON_DOUBLE_X1, BTLV_INPUT_POKE_ICON_DOUBLE_Y1 },
            { BTLV_INPUT_POKE_ICON_DOUBLE_X2, BTLV_INPUT_POKE_ICON_DOUBLE_Y2 },
            { -1, -1 },
          },
        };

        biw->pokeicon_charID[ i ] = GFL_CLGRP_CGR_Register( hdl,
                                                            POKEICON_GetCgxArcIndexByMonsNumber( bicp->mons_no[ i ],
                                                                                                 bicp->form_no[ i ],
                                                                                                 bicp->sex[ i ],
                                                                                                 FALSE ),
                                                            FALSE, CLSYS_DRAW_SUB, biw->heapID );
        biw->pokeicon_wk[ i ].clwk = GFL_CLACT_WK_Create( biw->pokeicon_clunit, biw->pokeicon_charID[ i ],
                                                          biw->pokeicon_plttID, biw->pokeicon_cellID,
                                                          &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
        GFL_CLACT_WK_SetPos( biw->pokeicon_wk[ i ].clwk, &pos[ biw->type ][ i ], CLSYS_DEFREND_SUB );
        GFL_CLACT_WK_SetAutoAnmFlag( biw->pokeicon_wk[ i ].clwk, TRUE );
        if( bicp->pos == i )
        {
          GFL_CLACT_WK_SetAnmSeq( biw->pokeicon_wk[ i ].clwk, 1 );
        }
        else
        {
          if( mask == 0 )
          {
            G2S_SetWnd0Position( pos[ biw->type ][ i ].x - 16, pos[ biw->type ][ i ].y - 16,
                                 pos[ biw->type ][ i ].x + 16, pos[ biw->type ][ i ].y + 16 );
            mask++;
          }
          else
          {
            G2S_SetWnd1Position( pos[ biw->type ][ i ].x - 16, pos[ biw->type ][ i ].y - 16,
                                 pos[ biw->type ][ i ].x + 16, pos[ biw->type ][ i ].y + 16 );
          }
        }
        GFL_CLACT_WK_SetPlttOffs( biw->pokeicon_wk[ i ].clwk,
                                  POKEICON_GetPalNum( bicp->mons_no[ i ], bicp->form_no[ i ], bicp->sex[ i ], FALSE ),
                                  CLWK_PLTTOFFS_MODE_OAM_COLOR );
      }
    }
    GFL_ARC_CloseDataHandle( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコン削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeletePokeIcon( BTLV_INPUT_WORK* biw )
{
  BOOL  ret = TRUE;
  int i;

  for( i = 0 ; i < 3 ; i++ )
  {
    if( biw->pokeicon_wk[ i ].clwk )
    {
      GFL_CLACT_WK_Remove( biw->pokeicon_wk[ i ].clwk );
      GFL_CLGRP_CGR_Release( biw->pokeicon_charID[ i ] );
      biw->pokeicon_wk[ i ].clwk = NULL;
      biw->pokeicon_charID[ i ] = GFL_CLGRP_REGISTER_FAILED;
      ret = FALSE;
    }
  }

  if( biw->pokeicon_cellID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_CELLANIM_Release( biw->pokeicon_cellID );
    biw->pokeicon_cellID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( biw->pokeicon_plttID != GFL_CLGRP_REGISTER_FAILED )
  {
    GFL_CLGRP_PLTT_Release( biw->pokeicon_plttID );
    biw->pokeicon_plttID = GFL_CLGRP_REGISTER_FAILED;
  }
  if( ret == FALSE )
  {
    TCB_WINDOW_MASK* twm = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_WINDOW_MASK ) );
    twm->biw = biw;
    BTLV_INPUT_SetTCB( biw, GFUser_VIntr_CreateTCB( TCB_ClearWindowMask, twm, 0 ), NULL );
  }
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコン表示／非表示
 *
 * @param[in] biw   システム管理構造体のポインタ
 * @param[in] flag  TRUE:表示 FALSE:非表示
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_SetDrawEnablePokeIcon( BTLV_INPUT_WORK* biw, BOOL flag )
{
  GFL_CLACT_UNIT_SetDrawEnable( biw->pokeicon_clunit, flag );
}

//--------------------------------------------------------------
/**
 * @brief   天候アイコン生成
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateWeatherIcon( BTLV_INPUT_WORK* biw, BtlWeather btl_weather )
{
  int pal[] = { 0, 1, 3, 2, 1 };
  int ofs_x[] = {   5,   5,  1,  1,   1 };
  int ofs_y[] = { -12, -12, -8, -8, -12 };
  GFL_CLACTPOS  weather;

  if( btl_weather == BTL_WEATHER_NONE )
  {
    return;
  }

  biw->weather_wk[ 0 ] = GFL_CLACT_WK_Create( biw->weather_clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                              &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
  weather.x = BTLV_INPUT_WEATHER_X1 + ofs_x[ btl_weather - 1 ];
  weather.y = BTLV_INPUT_WEATHER_Y + ofs_y[ btl_weather - 1 ];
  GFL_CLACT_WK_SetPos( biw->weather_wk[ 0 ], &weather, CLSYS_DEFREND_SUB );
  GFL_CLACT_WK_SetAutoAnmFlag( biw->weather_wk[ 0 ], TRUE );
  GFL_CLACT_WK_SetAnmSeq( biw->weather_wk[ 0 ], BTLV_INPUT_WEATHER_BASE + btl_weather );

  biw->weather_wk[ 1 ] = GFL_CLACT_WK_Create( biw->weather_clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                              &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
  weather.x = BTLV_INPUT_WEATHER_X1;
  weather.y = BTLV_INPUT_WEATHER_Y;
  GFL_CLACT_WK_SetPos( biw->weather_wk[ 1 ], &weather, CLSYS_DEFREND_SUB );
  GFL_CLACT_WK_SetAutoAnmFlag( biw->weather_wk[ 1 ], TRUE );
  GFL_CLACT_WK_SetAnmSeq( biw->weather_wk[ 1 ], BTLV_INPUT_WEATHER_BASE );
  GFL_CLACT_WK_SetPlttOffs( biw->weather_wk[ 1 ], pal[ btl_weather - 1 ], CLWK_PLTTOFFS_MODE_OAM_COLOR );

  //GFL_CLACT_WK_SetDrawEnable( biw->weather_wk[ 1 ], FALSE );

  {
    TCB_WEATHER_ICON_MOVE*  twim = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_WEATHER_ICON_MOVE ) );
    twim->biw = biw;

    BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_WeatherIconMove, twim, 0 ), TCB_WeatherIconMove_CB );

    biw->tcb_execute_count++;
  }
}

//--------------------------------------------------------------
/**
 * @brief   天候アイコン削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeleteWeatherIcon( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < BTLV_INPUT_WEATHER_ICON_MAX ; i++ )
  {
    if( biw->weather_wk[ i ] )
    {
      GFL_CLACT_WK_Remove( biw->weather_wk[ i ] );
      biw->weather_wk[ i ] = NULL;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   ボールゲージ生成
 *
 * @param[in] biw   システム管理構造体のポインタ
 * @param[in] type  ボールゲージタイプ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateBallGauge( BTLV_INPUT_WORK* biw, BALL_GAUGE_TYPE type )
{
  BTLV_INPUT_CLWK*  bib;
  int               i;
  GFL_CLACTPOS      pos;
  int               pos_ofs;
  int               anm_ofs;

  if( type == BALL_GAUGE_TYPE_ENEMY )
  {
    bib = biw->ballgauge_enemy;
    pos.x = BTLV_INPUT_BALLGAUGE_ENEMY_X;
    pos.y = BTLV_INPUT_BALLGAUGE_ENEMY_Y_CLOSE;
    pos_ofs = -8;
    anm_ofs = 4;
  }
  else
  {
    bib = biw->ballgauge_mine;
    pos.x = BTLV_INPUT_BALLGAUGE_MINE_X;
    pos.y = BTLV_INPUT_BALLGAUGE_MINE_Y_CLOSE;
    pos_ofs = 16;
    anm_ofs = 0;
  }

  for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
  {
    GF_ASSERT( bib[ i ].clwk == NULL )
    bib[ i ].clwk = GFL_CLACT_WK_Create( biw->ballgauge_clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                          &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
    GFL_CLACT_WK_SetPos( bib[ i ].clwk, &pos, CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetAutoAnmFlag( bib[ i ].clwk, TRUE );
    GFL_CLACT_WK_SetAnmSeq( bib[ i ].clwk, biw->bidp[ type ][ i ].status + anm_ofs );
    pos.x += pos_ofs;
  }
}

//--------------------------------------------------------------
/**
 * @brief   ボールゲージ削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeleteBallGauge( BTLV_INPUT_WORK* biw )
{
  BTLV_INPUT_CLWK* bib;
  int                   i;
  int                   type;

  for( type = 0 ; type < 2 ; type++ )
  {
    if( type )
    {
      bib = biw->ballgauge_enemy;
    }
    else
    {
      bib = biw->ballgauge_mine;
    }

    for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
    {
      if( bib[ i ].clwk )
      {
        GFL_CLACT_WK_Remove( bib[ i ].clwk );
        bib[ i ].clwk = NULL;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   カーソルOBJ生成
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateCursorOBJ( BTLV_INPUT_WORK* biw )
{
  int           i;
  int           anm_no[] = {
    BTLV_INPUT_CURSOR_LU_ANIME,
    BTLV_INPUT_CURSOR_RU_ANIME,
    BTLV_INPUT_CURSOR_LD_ANIME,
    BTLV_INPUT_CURSOR_RD_ANIME,
    BTLV_INPUT_CURSOR_LD_ANIME,
    BTLV_INPUT_CURSOR_RD_ANIME,
  };

  for( i = 0 ; i < BTLV_INPUT_CURSOR_MAX ; i++ )
  {
    GF_ASSERT( biw->cursor[ i ].clwk == NULL )
    biw->cursor[ i ].clwk = GFL_CLACT_WK_Create( biw->cursor_clunit, biw->cur_charID, biw->objplttID, biw->cur_cellID,
                                                 &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( biw->cursor[ i ].clwk, TRUE );
    GFL_CLACT_WK_SetAnmSeq( biw->cursor[ i ].clwk, anm_no[ i ] );
    GFL_CLACT_WK_SetDrawEnable( biw->cursor[ i ].clwk, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * @brief   カーソルOBJ削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeleteCursorOBJ( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < BTLV_INPUT_CURSOR_MAX ; i++ )
  {
    if( biw->cursor[ i ].clwk )
    {
      GFL_CLACT_WK_Remove( biw->cursor[ i ].clwk );
      biw->cursor[ i ].clwk = NULL;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   技タイプアイコン削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeleteWazaTypeIcon( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    if( biw->wazatype_wk[ i ] )
    {
      GFL_CLACT_WK_Remove( biw->wazatype_wk[ i ] );
      biw->wazatype_wk[ i ] = NULL;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   わるあがきボタン生成
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_CreateWaruagakiButton( BTLV_INPUT_WORK* biw )
{
  int               i;
  GFL_CLACTPOS      pos;

  GF_ASSERT( biw->waruagaki_wk == NULL );

  biw->waruagaki_wk = GFL_CLACT_WK_Create( biw->waruagaki_clunit, biw->objcharID, biw->objplttID, biw->objcellID,
                                           &obj_param, CLSYS_DEFREND_SUB, biw->heapID );
  pos.x = BTLV_INPUT_WARUAGAKI_X;
  pos.y = BTLV_INPUT_WARUAGAKI_Y;
  GFL_CLACT_WK_SetPos( biw->waruagaki_wk, &pos, CLSYS_DEFREND_SUB );
  GFL_CLACT_WK_SetAutoAnmFlag( biw->waruagaki_wk, TRUE );
  GFL_CLACT_WK_SetAnmSeq( biw->waruagaki_wk, BTLV_INPUT_WARUAGAKI_ANM );

  //わるあがき技名
  {
    STRBUF* wazaname_p    = GFL_STR_CreateBuffer( BUFLEN_WAZA_NAME, biw->heapID );
    STRBUF* wazaname_src  = GFL_MSG_CreateString( biw->msg, BI_WazaNameMsg );
    WORDSET*  wordset     = WORDSET_Create( biw->heapID );
    GFL_BMP_DATA* bmp     = GFL_BMP_Create( BTLV_INPUT_WARUAGAKI_SIZE_X, BTLV_INPUT_WARUAGAKI_SIZE_Y,
                                            GFL_BMP_16_COLOR, biw->heapID );
    u8 letter, shadow, back;
    int dot_len, char_len;

    GFL_BMP_Clear( bmp, 0 );

    GFL_FONTSYS_GetColor( &letter, &shadow, &back );
    GFL_FONTSYS_SetColor( BTLV_INPUT_WARUAGAKI_L, BTLV_INPUT_WARUAGAKI_S, BTLV_INPUT_WARUAGAKI_B );

    WORDSET_RegisterWazaName( wordset, 0, WAZANO_WARUAGAKI );
    WORDSET_ExpandStr( wordset, wazaname_p, wazaname_src );
    FontLenGet( wazaname_p, biw->font, &dot_len, &char_len );
    PRINTSYS_Print( bmp, BTLV_INPUT_WARUAGAKI_MSG_X - ( dot_len / 2 ), BTLV_INPUT_WARUAGAKI_MSG_Y,
                    wazaname_p, biw->font );
    GFL_FONTSYS_SetColor( letter, shadow, back );

    {
      void *obj_vram;
      NNSG2dImageProxy image;
      u8* bmp_data = GFL_BMP_GetCharacterAdrs( bmp );
      int i;

      obj_vram = G2S_GetOBJCharPtr();
      GFL_CLACT_WK_GetImgProxy( biw->waruagaki_wk, &image );

      for( i = 0 ; i < 4 ; i++ )
      {
        MI_CpuCopy16( &bmp_data[ BTLV_INPUT_WARUAGAKI_SIZE_X * 0x20 * i ],
                    (void*)( (u32)obj_vram + ( BTLV_INPUT_WARUAGAKI_8x4_START + 8 * i ) * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DSUB ] ),
                    0x20 * 8 );
        MI_CpuCopy16( &bmp_data[ 8 * 0x20 + BTLV_INPUT_WARUAGAKI_SIZE_X * 0x20 * i ],
                    (void*)( (u32)obj_vram + ( BTLV_INPUT_WARUAGAKI_4x4_START + 4 * i ) * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DSUB ] ),
                    0x20 * 4 );
        MI_CpuCopy16( &bmp_data[ 12 * 0x20 + BTLV_INPUT_WARUAGAKI_SIZE_X * 0x20 * i ],
                    (void*)( (u32)obj_vram + ( BTLV_INPUT_WARUAGAKI_2x4_START + 2 * i ) * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DSUB ] ),
                    0x20 * 2 );
      }
    }

    GFL_STR_DeleteBuffer( wazaname_p );
    GFL_STR_DeleteBuffer( wazaname_src );
    WORDSET_Delete( wordset );
    GFL_BMP_Delete( bmp );
  }
}

//--------------------------------------------------------------
/**
 * @brief   技タイプアイコン削除
 *
 * @param[in] biw   システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_DeleteWaruagakiButton( BTLV_INPUT_WORK* biw )
{
  if( biw->waruagaki_wk != NULL )
  {
    GFL_CLACT_WK_Remove( biw->waruagaki_wk );
    biw->waruagaki_wk = NULL;
  }
}

//--------------------------------------------------------------
/**
 * @brief   キー操作チェック
 *
 * @param[in] biw           システム管理構造体のポインタ
 * @param[in] tp_tbl        タッチパネルテーブル（カーソルの表示位置決定に使用）
 * @param[in] key_tbl       キー操作テーブル
 * @param[in] hit           キータッチがあったかどうか？
 * @param[in] henshin_flag  へんしんしているかどうか？
 */
//--------------------------------------------------------------
static  int   BTLV_INPUT_CheckKey( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tp_tbl, const BTLV_INPUT_KEYTBL* key_tbl, const u8* move_tbl, int hit, BOOL henshin_flag )
{
  int trg   = GFL_UI_KEY_GetTrg();
  int cont  = GFL_UI_KEY_GetCont();
  BOOL decide_flag = FALSE;

  //Bボタン無効ならBボタンを押していないことにする
  if( biw->b_button_flag == FALSE )
  {
    trg &= ( PAD_BUTTON_B ^ 0xffffffff );
  }

  if( biw->cursor_decide )
  {
    biw->cursor_decide = 0;
    set_cursor_pos( biw );
    BTLV_INPUT_PutCursorOBJ( biw, tp_tbl->hit_tbl, key_tbl );
    GFL_CLACT_UNIT_SetDrawEnable( biw->cursor_clunit, TRUE );
  }

  if( hit != GFL_UI_TP_HIT_NONE )
  {
    *(biw->cursor_mode) = 0;
    biw->cursor_pos = 0;
    biw->old_cursor_pos = CURSOR_NOMOVE;
    BTLV_INPUT_PutCursorOBJ( biw, tp_tbl->hit_tbl, key_tbl );
    return hit;
  }

  if( trg )
  {
    if( *(biw->cursor_mode) )
    {
      s8 move_pos = BTLV_INPUT_NOMOVE;
      const BTLV_INPUT_KEYTBL* tbl = &key_tbl[ biw->cursor_pos ];

      switch( trg ){
      case PAD_KEY_UP:
        move_pos = tbl->up;
        break;
      case PAD_KEY_DOWN:
        move_pos = tbl->down;
        break;
      case PAD_KEY_LEFT:
        move_pos = tbl->left;
        break;
      case PAD_KEY_RIGHT:
        move_pos = tbl->right;
        break;
      case PAD_BUTTON_B:
        if( tbl->b_button < 0 )
        {
          if( biw->center_button_type == BTLV_INPUT_CENTER_BUTTON_MODORU )
          {
            move_pos = tbl->b_button * -1;
          }
        }
        else
        {
          move_pos = tbl->b_button;
        }
        break;
      }
      if( trg == PAD_BUTTON_A )
      {
        hit = tbl->a_button;
        if( ( hit < 4 ) &&
            ( cont & PAD_BUTTON_L ) &&
            ( biw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE ) &&
          ( ( henshin_flag == TRUE ) || ( biw->waruagaki_flag == TRUE ) ) )
        {
          //ローテーション選択でLボタンを押しながらわるあがきを選択したか変身中なら何もしない
          ;
        }
        else if( ( hit < 4 ) &&
                 ( cont & PAD_BUTTON_L ) &&
                 ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) &&
                 ( henshin_flag == TRUE ) )
        { 
          //技選択で変身中にLボタンを押しながら技選択したなら何もしない
          ;
        }
        else if( biw->button_exist[ hit ] == TRUE )
        {
          if( get_cancel_flag( biw, tp_tbl, hit ) == FALSE )
          {
            SePlayDecide( biw );
          }
          else
          {
            SePlayCancel( biw );
          }
          decide_flag = TRUE;
        }
        else if( ( hit < 4 ) &&
                 ( cont & PAD_BUTTON_L ) &&
                 ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) &&
                 ( biw->waza_exist[ hit ] == TRUE ) )
        {
          SePlayDecide( biw );
        }
      }
      else
      {
        if( move_pos != BTLV_INPUT_NOMOVE )
        {
          SePlaySelect( biw );
          if( move_pos < 0 )
          {
            if( ( -move_pos != biw->old_cursor_pos ) && ( biw->old_cursor_pos != CURSOR_NOMOVE ) )
            {
              move_pos = biw->old_cursor_pos;
            }
            else
            {
              move_pos *= -1;
            }
          }
          else if( move_pos & BTLV_INPUT_MOVETBL )
          {
            int i = 0;
            GF_ASSERT( move_tbl != NULL );
            move_pos &= ( BTLV_INPUT_MOVETBL ^ 0xff );
            while( move_tbl[ i ] != 0xff )
            {
              if( biw->old_cursor_pos == move_tbl[ i ] )
              {
                move_pos = biw->old_cursor_pos;
                break;
              }
              i++;
            }
          }
          biw->old_cursor_pos = biw->cursor_pos;
          biw->cursor_pos = move_pos;
          if( trg == PAD_BUTTON_B )
          {
            if( move_pos != CURSOR_NOMOVE )
            {
              SePlayCancel( biw );
            }
            hit = move_pos;
            decide_flag = TRUE;
          }
        }
      }
    }
    else
    {
      if( trg & BTLV_INPUT_CURSOR_ON_BUTTON )
      {
        *(biw->cursor_mode) = 1;
        set_cursor_pos( biw );
        SePlaySelect( biw );
      }
    }
    BTLV_INPUT_PutCursorOBJ( biw, tp_tbl->hit_tbl, key_tbl );
  }

  if( decide_flag == TRUE )
  {
    switch( biw->scr_type ){
    case BTLV_INPUT_SCRTYPE_WAZA:
      if( get_cancel_flag( biw, tp_tbl, biw->cursor_pos ) == FALSE )
      {
        biw->decide_waza[ biw->active_index ] = biw->waza[ biw->cursor_pos ];
      }
      /*fallthru*/
    case BTLV_INPUT_SCRTYPE_COMMAND:
    case BTLV_INPUT_SCRTYPE_ROTATE:
      if( get_cancel_flag( biw, tp_tbl, biw->cursor_pos ) == FALSE )
      {
        biw->decide_pos[ biw->active_index ][ biw->scr_type ] = biw->cursor_pos;
      }
      biw->decide_pos_flag = 1;
      break;
    case BTLV_INPUT_SCRTYPE_DIR:
      if( get_cancel_flag( biw, tp_tbl, hit ) == FALSE )
      {
        biw->decide_pos[ biw->active_index ][ biw->scr_type ] = biw->cursor_pos |
                                                                ( biw->decide_pos[ biw->active_index ]
                                                                                 [ BTLV_INPUT_SCRTYPE_WAZA ] << 4 );
      }
      biw->decide_pos_flag = 1;
      break;
    default:
      break;
    }
    biw->cursor_pos = 0;
    biw->old_cursor_pos = CURSOR_NOMOVE;
    biw->cursor_decide = 1;
    GFL_CLACT_UNIT_SetDrawEnable( biw->cursor_clunit, FALSE );
  }

  return hit;
}

//--------------------------------------------------------------
/**
 * @brief   キー操作チェック（バトルレコーダー専用）
 *
 * @param[in] biw     システム管理構造体のポインタ
 */
//--------------------------------------------------------------
static  int   BTLV_INPUT_CheckKeyBR( BTLV_INPUT_WORK* biw )
{
  int rp = GFL_UI_KEY_GetRepeat();

  if( rp & PAD_BUTTON_B )
  {
    SePlayCancel( biw );
    return BTLV_INPUT_BR_SEL_STOP;
  }
  else if( rp & PAD_KEY_LEFT )
  {
    SePlayDecide( biw );
    return BTLV_INPUT_BR_SEL_REW;
  }
  else if( rp & PAD_KEY_RIGHT )
  {
    SePlayDecide( biw );
    return BTLV_INPUT_BR_SEL_FF;
  }
  return GFL_UI_TP_HIT_NONE;
}

//--------------------------------------------------------------
/**
 * @brief   カーソルOBJ表示
 *
 * @param[in] biw     システム管理構造体のポインタ
 * @param[in] tp_tbl  タッチパネルテーブル（カーソルの表示位置決定に使用）
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_PutCursorOBJ( BTLV_INPUT_WORK* biw, const GFL_UI_TP_HITTBL* tp_tbl, const BTLV_INPUT_KEYTBL* key_tbl )
{
  int i;

  for( i = BTLV_INPUT_CURSOR_LU ; i < BTLV_INPUT_CURSOR_MAX ; i++ )
  {
    const BTLV_INPUT_KEYTBL*  key = &key_tbl[ biw->cursor_pos ];

    if( key->cur_pos[ i ] >= 0 )
    {
      const GFL_UI_TP_HITTBL*  hit = &tp_tbl[ key->cur_pos[ i ] ];
      GFL_CLACTPOS pos;

      switch( i ){
      case BTLV_INPUT_CURSOR_LU:
        pos.x = hit->rect.left;
        pos.y = hit->rect.top;
        break;
      case BTLV_INPUT_CURSOR_RU:
        pos.x = hit->rect.right;
        pos.y = hit->rect.top;
        break;
      case BTLV_INPUT_CURSOR_LD:
        pos.x = hit->rect.left;
        pos.y = hit->rect.bottom;
        break;
      case BTLV_INPUT_CURSOR_RD:
        pos.x = hit->rect.right;
        pos.y = hit->rect.bottom;
        break;
      case BTLV_INPUT_CURSOR_LD_2:
        pos.x = hit->rect.left;
        pos.y = hit->rect.bottom;
        break;
      case BTLV_INPUT_CURSOR_RD_2:
        pos.x = hit->rect.right;
        pos.y = hit->rect.bottom;
        break;
      }
      GFL_CLACT_WK_SetDrawEnable( biw->cursor[ i ].clwk, *(biw->cursor_mode) );
      GFL_CLACT_WK_SetPos( biw->cursor[ i ].clwk, &pos, CLSYS_DEFREND_SUB );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( biw->cursor[ i ].clwk, FALSE );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   ボタン押したリアクション
 *
 * @param[in] biw   システム管理構造体のポインタ
 * @param[in] hit   押されたボタン
 * @param[in] pltt  ボタンに対応したパレットテーブル
 */
//--------------------------------------------------------------
static  int  BTLV_INPUT_SetButtonReaction( BTLV_INPUT_WORK* biw, int hit, int pltt )
{
  if( pltt < 0 )
  {
    return hit;
  }

  if( ( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) && ( hit < 4 ) &&
      ( ( biw->scr_type == BTLV_INPUT_SCRTYPE_WAZA ) || ( biw->scr_type == BTLV_INPUT_SCRTYPE_ROTATE ) ) )
  {
    biw->hit = BTLV_INPUT_SetWazaInfoModeMask( hit );
  }
  else
  {
    biw->hit = hit;
  }

  {
    TCB_BUTTON_REACTION*  tbr = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( biw->heapID ), sizeof( TCB_BUTTON_REACTION ) );
    biw->button_reaction = ( ( pltt & 0x10000 ) == 0 );

    tbr->biw        = biw;
    tbr->seq_no     = 0;
    tbr->pltt       = pltt & 0xffff;
    tbr->waruagaki  = ( ( biw->waruagaki_flag == TRUE ) && ( tbr->pltt == 0 ) );

    BTLV_INPUT_SetTCB( biw, GFL_TCB_AddTask( biw->tcbsys, TCB_ButtonReaction, tbr, 0 ), TCB_ButtonReaction_CB );
  }
  return GFL_UI_TP_HIT_NONE;
}

//--------------------------------------------------------------
/**
 * @brief   ボタン押したリアクションタスク
 */
//--------------------------------------------------------------
static  void  TCB_ButtonReaction( GFL_TCB* tcb, void* work )
{
  TCB_BUTTON_REACTION*  tbr = ( TCB_BUTTON_REACTION* )work;

  switch( tbr->seq_no ){
  case 0:
    if( tbr->waruagaki == TRUE )
    {
      PaletteFadeReq( tbr->biw->pfd, PF_BIT_SUB_OBJ, 0x0002, 0, 0, 8, 0x7fff, tbr->biw->tcbsys );
    }
    else
    {
      PaletteFadeReq( tbr->biw->pfd, PF_BIT_SUB_BG, tbr->pltt, 0, 0, 8, 0x7fff, tbr->biw->tcbsys );
    }
    tbr->seq_no++;
    break;
  case 1:
    if( !PaletteFadeCheck( tbr->biw->pfd ) )
    {
      if( tbr->waruagaki == TRUE )
      {
        PaletteFadeReq( tbr->biw->pfd, PF_BIT_SUB_OBJ, 0x0002, 0, 8, 0, 0x7fff, tbr->biw->tcbsys );
      }
      else
      {
        PaletteFadeReq( tbr->biw->pfd, PF_BIT_SUB_BG, tbr->pltt, 0, 8, 0, 0x7fff, tbr->biw->tcbsys );
      }
      tbr->seq_no++;
    }
    break;
  case 2:
    if( !PaletteFadeCheck( tbr->biw->pfd ) )
    {
      BTLV_INPUT_FreeTCB( tbr->biw, tcb );
    }
    break;
  }
}

static  void  TCB_ButtonReaction_CB( GFL_TCB* tcb )
{
  TCB_BUTTON_REACTION*  tbr = ( TCB_BUTTON_REACTION* )GFL_TCB_GetWork( tcb );

  tbr->biw->button_reaction = 0;
}

//--------------------------------------------------------------
/**
 * @brief   シューターエネルギー表示
 */
//--------------------------------------------------------------
static  void  BTLV_INPUT_PutShooterEnergy( BTLV_INPUT_WORK* biw, BTLV_INPUT_COMMAND_PARAM* bicp )
{
  //使用できない場合はエネルギー表示なし
  if( BTLV_EFFECT_CheckShooterEnable() == FALSE )
  {
    return;
  }
  {
    WORDSET*  wordset = WORDSET_Create( biw->heapID );
    STRBUF*   se_p = GFL_STR_CreateBuffer( BUFLEN_BI_PP_NUM, biw->heapID );
    STRBUF*   se_src = GFL_MSG_CreateString( biw->msg,  BI_ShooterEnergy );
    u8 letter, shadow, back;
    int dot_len, char_len;

    GFL_FONTSYS_GetColor( &letter, &shadow, &back );
    GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetS( MSGCOLOR_PP_WHITE ),
                          PRINTSYS_LSB_GetB( MSGCOLOR_PP_WHITE ) );

    WORDSET_RegisterNumber( wordset, 0, bicp->shooterEnergy,  2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );
    WORDSET_ExpandStr( wordset, se_p, se_src);

    FontLenGet( se_p, biw->font, &dot_len, &char_len );
    PRINTSYS_Print( biw->bmp_data_shooter, BTLV_INPUT_SHOOTER_ENERGY_X - ( dot_len / 2 ), BTLV_INPUT_SHOOTER_ENERGY_Y,
                    se_p, biw->font );
    GFL_FONTSYS_SetColor( letter, shadow, back );

    WORDSET_Delete( wordset );
    GFL_STR_DeleteBuffer( se_src );
    GFL_STR_DeleteBuffer( se_p );
  }
}

//=============================================================================================
//  カメラフォーカス
//=============================================================================================
static  void  BTLV_INPUT_SetFocus( BTLV_INPUT_WORK* biw )
{
  BTLV_EFFECT_SetCameraWorkSwitch( BTLV_EFFECT_CWE_NO_STOP );
  BTLV_EFFECT_SetCameraFocus( biw->focus_pos, BTLEFF_CAMERA_MOVE_INTERPOLATION, 10, 0, 8 );
}

//=============================================================================================
//  ボタン決定位置記憶初期化チェック
//=============================================================================================
static  void  check_init_decide_pos( BTLV_INPUT_WORK* biw )
{
  BOOL  flag;

  flag = BTLV_GAUGE_CheckAppearFlag( BTLV_EFFECT_GetGaugeWork(), biw->focus_pos ) |
         BTLV_MCSS_CheckHengeFlag( BTLV_EFFECT_GetMcssWork(), biw->focus_pos );

  if( flag == TRUE )
  {
    int i;

    for( i = 0 ; i < BTLV_INPUT_SCRTYPE_MAX ; i++ )
    {
      biw->decide_pos[ biw->active_index ][ i ] = 0;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  空いているTCBIndexを取得してGFL_TCBをセット
 *
 *  @param[in]  tcb       セットするGFL_TCB
 *  @param[in]  callback  解放されるときに呼び出されるコールバック関数
 */
//-----------------------------------------------------------------------------
static  void   BTLV_INPUT_SetTCB( BTLV_INPUT_WORK* biw, GFL_TCB* tcb, BTLV_INPUT_TCB_CALLBACK_FUNC* callback_func )
{
  int index = BTLV_INPUT_GetTCBIndex( biw );

  biw->tcb[ index ] = tcb;
  biw->tcb_callback[ index ] = callback_func;
}

//----------------------------------------------------------------------------
/**
 *  @brief  指定されたGFL_TCBのTCBIndexを取得
 *
 *  @param[in]  tcb 取得するGFL_TCB
 */
//-----------------------------------------------------------------------------
static  int   BTLV_INPUT_SearchTCBIndex( BTLV_INPUT_WORK* biw, GFL_TCB* tcb )
{
  int i;

  for( i = 0 ; i < BTLV_INPUT_TCB_MAX ; i++ )
  {
    if( biw->tcb[ i ] == tcb )
    {
      break;
    }
  }

  GF_ASSERT( i != BTLV_INPUT_TCB_MAX );

  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  解放されていないTCBを解放
 *
 *  @param[in]  tcb 解放するTCB
 */
//-----------------------------------------------------------------------------
static  void  BTLV_INPUT_FreeTCB( BTLV_INPUT_WORK* biw, GFL_TCB* tcb )
{
  int index = BTLV_INPUT_SearchTCBIndex( biw, tcb );
  if( tcb )
  {
    void* work = GFL_TCB_GetWork( tcb );
    if( biw->tcb_callback[ index ] )
    {
      biw->tcb_callback[ index ]( tcb );
    }
    if( work )
    {
      GFL_HEAP_FreeMemory( work );
    }
    GFL_TCB_DeleteTask( tcb );
    biw->tcb[ index ] = NULL;
    biw->tcb_callback[ index ] = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  空いているTCBIndexを取得
 *
 *  @param[in]  biw システム管理構造体
 */
//-----------------------------------------------------------------------------
static  int   BTLV_INPUT_GetTCBIndex( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < BTLV_INPUT_TCB_MAX ; i++ )
  {
    if( biw->tcb[ i ] == NULL )
    {
      break;
    }
  }

  GF_ASSERT( i != BTLV_INPUT_TCB_MAX );
  if( i == BTLV_INPUT_TCB_MAX )
  {
    BTLV_INPUT_FreeTCB( biw, biw->tcb[ 0 ] );
    i = 0;
  }

  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  解放されていないすべてのTCBを解放
 *
 *  @param[in]  biw システム管理構造体
 */
//-----------------------------------------------------------------------------
static  void  BTLV_INPUT_FreeTCBAll( BTLV_INPUT_WORK* biw )
{
  int i;

  for( i = 0 ; i < BTLV_INPUT_TCB_MAX ; i++ )
  {
    if( biw->tcb[ i ] )
    {
      BTLV_INPUT_FreeTCB( biw, biw->tcb[ i ] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  キャンセルフラグ取得
 *
 *  @param[in]  biw システム管理構造体
 *  @param[in]  tbl タッチパネルテーブル
 *  @param[in]  pos タッチパネルポジション
 *
 *  @retval TRUE:キャンセルボタン　FALSE:キャンセルボタンではない
 */
//-----------------------------------------------------------------------------
static  BOOL  get_cancel_flag( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tbl, int pos )
{
  if( ( biw->center_button_type == BTLV_INPUT_CENTER_BUTTON_ESCAPE ) ||
      ( biw->scr_type != BTLV_INPUT_SCRTYPE_COMMAND ) )
  {
    return tbl->cancel_flag[ pos ] & TRUE;
  }
  else
  {
    BOOL  ret = ( tbl->cancel_flag[ pos ] & CANCEL_FLAG_MASK ) >> CANCEL_FLAG_SHIFT;
    if( ret == TRUE )
    {
      biw->decide_pos[ biw->active_index ][ biw->scr_type ] = 0;
    }
    return ret;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  記憶しているカーソルポジションをbiw->cursor_posにセット
 *
 *  @param[in]  biw システム管理構造体
 */
//-----------------------------------------------------------------------------
static  void  set_cursor_pos( BTLV_INPUT_WORK* biw )
{
  switch( biw->scr_type ){
  case BTLV_INPUT_SCRTYPE_WAZA:
    if( biw->decide_waza[ biw->active_index ] != biw->waza[ biw->decide_pos[ biw->active_index ][ biw->scr_type ] ] )
    {
      biw->decide_pos[ biw->active_index ][ BTLV_INPUT_SCRTYPE_DIR ] = 0;
    }
    /*fallthru*/
  case BTLV_INPUT_SCRTYPE_COMMAND:
  case BTLV_INPUT_SCRTYPE_ROTATE:
    biw->cursor_pos = biw->decide_pos[ biw->active_index ][ biw->scr_type ];
    break;
  case BTLV_INPUT_SCRTYPE_DIR:
    {
      int waza_pos = ( biw->decide_pos[ biw->active_index ][ biw->scr_type ] & 0xf0 ) >> 4;

      if( biw->decide_pos[ biw->active_index ][ BTLV_INPUT_SCRTYPE_WAZA ] == waza_pos )
      {
        biw->cursor_pos = biw->decide_pos[ biw->active_index ][ biw->scr_type ] & 0x0f;
      }
      else
      {
        biw->cursor_pos = 0;
      }
    }
    break;
  default:
    biw->cursor_pos = 0;
    break;
  }
}

//=============================================================================================
//  バッグボタンパレットをシューターモードにあわせて変更
//=============================================================================================
static  void  change_bag_button_pal( BTLV_INPUT_WORK* biw )
{
  if( BTL_MAIN_GetCompetitor( BTLV_EFFECT_GetMainModule() ) == BTL_COMPETITOR_DEMO_CAPTURE )  ///< 捕獲デモ
  {
    return;
  }
  if( BTLV_EFFECT_CheckItemEnable() == FALSE )
  {
    u16 pal[ 0x10 ];
    SoftFade( biw->bag_pal, pal, 0x10, 8, 0 );
    PaletteWorkSet( BTLV_EFFECT_GetPfd(), &pal, FADE_SUB_BG, 0x20, 0x20 );
  }
}

//=============================================================================================
//  退避していたバッグボタンパレットをリロード
//=============================================================================================
static  void  pop_bag_button_pal( BTLV_INPUT_WORK* biw )
{
  if( biw->main_loop_tcb_flag == TRUE )
  {
    PaletteWorkSet( BTLV_EFFECT_GetPfd(), &biw->bag_pal, FADE_SUB_BG, 0x20, 0x20 );
  }
}

//=============================================================================================
//  コマンドスクリーンを取得
//=============================================================================================
static  ARCDATID  get_command_screen( BTLV_INPUT_WORK* biw )
{
  if( biw->center_button_type == BTLV_INPUT_CENTER_BUTTON_ESCAPE )
  {
    if( biw->type == BTLV_INPUT_TYPE_TRIPLE )
    {
      if( BTLV_EFFECT_GetBagMode() == BBAG_MODE_SHOOTER )
      {
        if( biw->focus_pos == BTLV_MCSS_POS_C )
        {
          return NARC_battgra_wb_battle_w_bg0i_NSCR;
        }
        else
        {
          return NARC_battgra_wb_battle_w_bg0c_NSCR;
        }
      }
      else
      {
        if( biw->focus_pos == BTLV_MCSS_POS_C )
        {
          return NARC_battgra_wb_battle_w_bg0a_NSCR;
        }
        else
        {
          return NARC_battgra_wb_battle_w_bg0b_NSCR;
        }
      }
    }
    else
    {
      if( BTLV_EFFECT_GetBagMode() == BBAG_MODE_SHOOTER )
      {
        return NARC_battgra_wb_battle_w_bg0i_NSCR;
      }
      else
      {
        return NARC_battgra_wb_battle_w_bg0a_NSCR;
      }
    }
  }
  else
  {
    if( ( biw->type == BTLV_INPUT_TYPE_TRIPLE ) && ( biw->focus_pos != BTLV_MCSS_POS_C ) )
    {
      if( BTLV_EFFECT_GetBagMode() == BBAG_MODE_SHOOTER )
      {
        return NARC_battgra_wb_battle_w_bg0k_NSCR;
      }
      else
      {
        return NARC_battgra_wb_battle_w_bg0e_NSCR;
      }
    }
    else
    {
      if( BTLV_EFFECT_GetBagMode() == BBAG_MODE_SHOOTER )
      {
        return NARC_battgra_wb_battle_w_bg0j_NSCR;
      }
      else
      {
        return NARC_battgra_wb_battle_w_bg0d_NSCR;
      }
    }
  }
  //当たり障りのないものを返しておく
  return NARC_battgra_wb_battle_w_bg0a_NSCR;
}

//=============================================================================================
//  オープン音再生
//=============================================================================================
static  void  SePlayOpen( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;
  {
    PMSND_PlaySE( SEQ_SE_OPEN2 );
  }
}

//=============================================================================================
//  選択音再生
//=============================================================================================
static  void  SePlaySelect( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;
  {
    PMSND_PlaySE( SEQ_SE_SELECT1 );
  }
}

//=============================================================================================
//  決定音再生
//=============================================================================================
static  void  SePlayDecide( BTLV_INPUT_WORK* biw )
{
  if( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER )
  {
    if( ( biw->comp == BTL_COMPETITOR_COMM ) ){ return; }
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
  else
  {
    // 録画データ再生時に、とくせいウィンドウSEと音がかぶらないようにSEを替える
    PMSND_PlaySE( SEQ_SE_DECIDE1 );
  }
}

//=============================================================================================
//  キャンセル音再生
//=============================================================================================
static  void  SePlayCancel( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;

  PMSND_PlaySE( SEQ_SE_CANCEL2 );
}

//=============================================================================================
//  ローテーション選択音
//=============================================================================================
static  void  SePlayRotateSelect( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;

  PMSND_PlaySE( SEQ_SE_ROTATION_S );
}

//=============================================================================================
//  ローテーション決定音
//=============================================================================================
static  void  SePlayRotateDecide( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;

  PMSND_PlaySE( SEQ_SE_DECIDE2 );
}

//=============================================================================================
//  ローテーション音
//=============================================================================================
static  void  SePlayRotation( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;

  PMSND_PlaySE( SEQ_SE_ROTATION_B );
}

//=============================================================================================
//  ビープ音再生
//=============================================================================================
static  void  SePlayBeep( BTLV_INPUT_WORK* biw )
{
  if( ( biw->comp == BTL_COMPETITOR_COMM ) && ( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER ) ) return;

  PMSND_PlaySE( SEQ_SE_BEEP );
}

#ifdef PM_DEBUG
static  void  debug_timer_edit_check( BTLV_INPUT_WORK* biw )
{
  if( biw->scr_type != BTLV_INPUT_SCRTYPE_BATTLE_RECORDER )
  {
    int cont  = GFL_UI_KEY_GetCont();
    int trg   = GFL_UI_KEY_GetTrg();

    //タイマー一時停止
    if( ( cont & PAD_BUTTON_L ) && ( trg & PAD_BUTTON_X ) )
    {
      BTLV_TIMER_SwitchTimerStopFlag( BTLV_EFFECT_GetTimerWork() );
    }
    //タイマーエディット開始
    if( ( cont & PAD_BUTTON_L ) && ( trg & PAD_BUTTON_Y ) && ( biw->timer_edit_flag == 0 ) )
    {
      biw->timer_edit_flag = BTLV_TIMER_SwitchTimerEditFlag( BTLV_EFFECT_GetTimerWork() );
    }
    //タイマーエディット終了
    if( ( cont & PAD_BUTTON_START ) && ( biw->timer_edit_flag ) )
    {
      BTLV_TIMER_SwitchTimerEditFlag( BTLV_EFFECT_GetTimerWork() );
      biw->timer_edit_flag = 0;
    }
  }
}
#endif
