//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_album.c
 *	@brief
 *	@author Toru=Nagihashi
 *	@data		2009.12.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/main.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "sound/pm_wb_voice.h"
#include "system/bmp_winframe.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"
#include "system/poke2dgra.h"

//アーカイブ
#include "arc_def.h"
#include "item_icon.naix"
#include "arc\mystery.naix"
#include "msg/msg_mystery.h"
#include "font/font.naix"

//自分のモジュール
#include "mystery_util.h"
#include "net_app/mystery.h"
#include "mystery_debug.h"
#include "mystery_snd.h"
#include "mystery_inline.h"

//外部公開
#include "mystery_album.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  アルバム
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *          デバッグ
 */
//=============================================================================
#ifdef PM_DEBUG
#define DEBUG_SAVE  //STARTを押すとセーブして終了
#define DEBUG_DELETE  //SELECTを押すと強制消してセーブして終了
#endif //PM_DEBUG

#define MEMORY_LOAD_SILHOUETTE

//=============================================================================
/**
 *          定数
 */
//=============================================================================

#define MYSTERY_ALBUM_CARD_MAX         GIFT_DATA_MAX 
#define MYSTERY_ALBUM_PAGE_MAX          3
#define MYSTERY_ALBUM_PAGE_IN_CARD_NUM  (MYSTERY_ALBUM_CARD_MAX/MYSTERY_ALBUM_PAGE_MAX)

//メインBG面
#define MYSTERY_ALBUM_LIST_FRM        GFL_BG_FRAME0_M
#define MYSTERY_ALBUM_FONT_FRM        GFL_BG_FRAME1_M
#define MYSTERY_ALBUM_CARD_FRM        GFL_BG_FRAME2_M
#define MYSTERY_ALBUM_BACK_FRM        GFL_BG_FRAME3_M

//サブBG面
#define MYSTERY_ALBUM_CARD_FONT_S       GFL_BG_FRAME0_S
#define MYSTERY_ALBUM_CARD_FRM_S        GFL_BG_FRAME2_S
#define MYSTERY_ALBUM_CARD_BACK_S       GFL_BG_FRAME3_S

//メインBGパレット
#define MYSTERY_ALBUM_FONT_PLT              15
#define MYSTERY_ALBUM_TOP_FONT_PLT          3
#define MYSTERY_ALBUM_BG_BACK_PLT_M         0

//サブBGパレット
#define MYSTERY_ALBUM_BG_FRM_S_PLT          13
#define MYSTERY_ALBUM_BG_BACK_S_PLT         14
#define MYSTERY_ALBUM_BG_FONT_S_PLT         15
#define MYSTERY_ALBUM_BG_BACK_PLT_S         0

//メインOBJパレット
#define MYSTERY_ALBUM_OBJ_CURSOR_PLT        0
#define MYSTERY_ALBUM_OBJ_ICON_PLT          4
#define MYSTERY_ALBUM_OBJ_POKEICON_PLT      12

//サブOBJパレット
#define MYSTERY_ALBUM_OBJ_ICON_S_PLT        11
#define MYSTERY_ALBUM_OBJ_SILHOUETTE_S_PLT  14

//-------------------------------------
///	カーソル
//=====================================
typedef enum
{
  MYSTERY_CURSOR_LEFT_UP,
  MYSTERY_CURSOR_RIGHT_UP,
  MYSTERY_CURSOR_LEFT_DOWN,
  MYSTERY_CURSOR_RIGHT_DOWN,

  MYSTERY_CURSOR_MAX,
  MYSTERY_CURSOR_RETURN = MYSTERY_CURSOR_MAX,
} MYSTERY_CURSOR_POS;

//ｘがminより小さいならばmax ｘがmaxより大きいならばmin　さもなくばx
#define MATH_ROUND(x,min,max) (x>max?min:x<min?max:x)

//-------------------------------------
///	移動
//=====================================
#define MYSTERY_ALBUM_MOVE_SCROLL_DISTANCE  256
#define MYSTERY_ALBUM_MOVE_SCROLL_SYNC      16

#define MYSTERY_ALBUM_SWAP_SYNC             30
#define MYSTERY_ALBUM_DELETE_SYNC             30

#define MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK ( \
                                            GX_WND_PLANEMASK_BG1 | \
                                            GX_WND_PLANEMASK_BG2 | \
                                            GX_WND_PLANEMASK_BG3 | \
                                            GX_WND_PLANEMASK_OBJ )


#define MYSTERY_ALLOW_MAX (2)

//=============================================================================
/**
 *          構造体
 */
//=============================================================================
//-------------------------------------
///	カード情報
//=====================================
typedef struct 
{
  BOOL            is_exist;
  GIFT_PACK_DATA  data;
  GFL_BMP_DATA    *p_bmp;
  u32             res_cgx;
  u32             res_cel;
} MYSTERY_CARD_DATA;

//-------------------------------------
///	パレットアニメワークエリア
//=====================================
typedef struct 
{
  u16 plt[0x10];
  u16 plt_src[0x10];
  u16 plt_dst[0x10];
} PLTANM_WORK;


//-------------------------------------
///	アルバム構造体
//=====================================
struct _MYSTERY_ALBUM_WORK
{ 
  MYSTERY_ALBUM_SETUP setup;
  HEAPID              heapID;
  u16                 dummy;
  MYSTERY_CARD_DATA   data[ MYSTERY_ALBUM_CARD_MAX ];

  BOOL                    is_card_update;
  MYSTERY_CARD_WORK       *p_card[ MYSTERY_ALBUM_CARD_MAX ];
  MYSTERY_CARD_WORK       *p_anm_card;
  MYSTERY_CARD_RES        *p_card_res;
  MYSTERY_MSGWIN_WORK     *p_page_win;
  MYSTERY_MSGWINSET_WORK  *p_winset;

  //テキスト
  MYSTERY_TEXT_WORK         *p_text; 

  //選択肢
  MYSTERY_LIST_WORK         *p_list;

  MYSTERY_SEQ_WORK    *p_seq;        //シーケンス管理
  s32                 now_page;   //現在のページ
  MYSTERY_CURSOR_POS  cursor;     //カーソル
  BOOL                is_left;    //ページ書き込み場所
  BOOL                is_change;  //変更フラグ
  u32                 scroll_cnt;
  s8                  scroll_req; //1だと右-1だと左0だと動かない
  u8                  dummy2;
  u16                 swap_card_index;
  BOOL                is_swap;
  BOOL                key_btn_save; //もどるボタンにきたときの前のボタンを記憶  TRUEなら左 FALSEなら右
  u32                 cnt;

  GFL_CLWK            *p_cursor;  //カーソル
  GFL_CLWK            *p_swap_cursor;  //カーソル(swap)
  GFL_CLWK            *p_allow[MYSTERY_ALLOW_MAX];
  u32                 res_cursor_cgx;
  u32                 res_cursor_cel;
  u32                 res_cursor_plt;

  GFL_BMPWIN          *p_bmpwin[MYSTERY_CURSOR_MAX*2];  //表と裏
  GFL_CLWK            *p_icon[ MYSTERY_CURSOR_MAX*2 ];  //コピーされたポインタ
  u32                 res_cgx[MYSTERY_CURSOR_MAX*2];
  u32                 res_cel[MYSTERY_CURSOR_MAX*2];
  u32                 res_icon_plt[MYSTERY_CURSOR_MAX*2];//8本
  u32                 res_poke_icon_plt;    //3本

  //カーソルパレットフェード用
  u16 plt_cnt;
  PLTANM_WORK         pltanm[MYSTERY_ALLOW_MAX];

};

//=============================================================================
/**
 *          プロトタイプ
 */
//=============================================================================
//-------------------------------------
///	アルバムプライベート
//=====================================
static void Mystery_Album_InitDisplay( MYSTERY_ALBUM_WORK *p_wk, HEAPID heapID );
static void Mystery_Album_ExitDisplay( MYSTERY_ALBUM_WORK *p_wk );
static void Mystery_Album_CreateDisplay( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, BOOL is_left, u32 page, HEAPID heapID );
static void Mystery_Album_DeleteDisplay( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front );
static void Mystery_Album_CreateThumbnail( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, BOOL is_left, u32 page, u16 index, HEAPID heapID );
static void Mystery_Album_DeleteThumbnail( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, u16 index );
static void Mystery_Album_StartScrollPage( MYSTERY_ALBUM_WORK *p_wk, BOOL is_left );
static BOOL Mystery_Album_ScrollPage( MYSTERY_ALBUM_WORK *p_wk );
static void Mystery_Album_RemoveCard( MYSTERY_ALBUM_WORK *p_wk, u32 card_index );
static void Mystery_Album_SwapCard( MYSTERY_ALBUM_WORK *p_wk, u32 card_index1, u32 card_index2 );
static u32 Mystery_Album_GetPageMax( const MYSTERY_ALBUM_WORK *cp_wk );
static u32 Mystery_Album_GetDataNum( const MYSTERY_ALBUM_WORK *cp_wk );
static inline u32 Mystery_Album_GetCardIndex( u32 page, u32 cursor )
{ 
  return page * MYSTERY_CURSOR_MAX + cursor;
}
static inline u32 Mystery_Album_GetNowCardIndex( const MYSTERY_ALBUM_WORK *cp_wk )
{ 
  return Mystery_Album_GetCardIndex( cp_wk->now_page, cp_wk->cursor );
}
static inline u32 Mystery_Album_GetIndexToPage( u32 cursor )
{ 
  return cursor / MYSTERY_CURSOR_MAX;
}

//-------------------------------------
///	カードデータ
//=====================================
static void MYSTERY_CARD_DATA_Init( MYSTERY_CARD_DATA *p_wk, const GIFT_PACK_DATA *cp_data, const MYSTERY_ALBUM_SETUP *cp_setup, HEAPID heapID );
static void MYSTERY_CARD_DATA_Exit( MYSTERY_CARD_DATA *p_wk );
static BOOL MYSTERY_CARD_DATA_IsExist( const MYSTERY_CARD_DATA *cp_wk );
static const GFL_BMP_DATA *MYSTERY_CARD_DATA_GetBmp( const MYSTERY_CARD_DATA *cp_wk );
static ARCHANDLE * MYSTERY_CARD_DATA_GetArcHandle( const MYSTERY_CARD_DATA *cp_wk, HEAPID heapID );
static u32 MYSTERY_CARD_DATA_GetResPlt( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetPltOfs( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetResCgx( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetResCel( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetResAnm( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetType( const MYSTERY_CARD_DATA *cp_wk );
static const GIFT_PACK_DATA *MYSTERY_CARD_DATA_GetGiftBackData( const MYSTERY_CARD_DATA *cp_wk );
static BOOL MYSTERY_CARD_DATA_IsHave( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetThumbnailPltOfs( const MYSTERY_CARD_DATA *cp_wk );
static u32 MYSTERY_CARD_DATA_GetCardPltOfs( const MYSTERY_CARD_DATA *cp_wk );

//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_MoveCursor( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_SelectList( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_SwapCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_DeleteMsg( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	PLTANM
//=====================================
static void PLTANM_Load( PLTANM_WORK *p_wk, ARCHANDLE *p_handle, ARCDATID datID, u8 dst_plt_num, u8 src_plt_num, HEAPID heapID );
static void PLTANM_Main( PLTANM_WORK *p_wk, NNS_GFD_DST_TYPE type, u8 target_plt, u16 cnt );

//=============================================================================
/**
 *          データ
 */
//=============================================================================
//-------------------------------------
///	上画面サムネイルカード位置データ
//=====================================
typedef struct
{ 
  u8 card_x;  //カード開始座標Xキャラ単位
  u8 card_y;  //カード開始座標Yキャラ単位 
  u8 date_x;  //日付用BMPWIN開始Xキャラ単位
  u8 date_y;  //日付用BMPWIN開始Yキャラ単位
  s16 icon_x; //アイコン座標X
  s16 icon_y; //アイコン座標Y
} MYSTERY_ALBUM_THUMBNAIL_DATA;
static const MYSTERY_ALBUM_THUMBNAIL_DATA sc_thumbnail_data[MYSTERY_CURSOR_MAX] =
{ 
  //左上
  { 
    3,
    4,
    4,
    8,
    92,
    45,
  },
  //右上
  { 
    18,
    4,
    19,
    8,
    212,
    45,
  },
  //左下
  { 
    3,
    12,
    4,
    16,
    92,
    110,
  },
  //右下
  { 
    18,
    12,
    19,
    16,
    212,
    110,
  },
};
#define MYSTERY_ALBUM_CARD_BACKSURFACE_OFS  256
#define MYSTERY_ALBUM_THUMBNAIL_CARD_W      12
#define MYSTERY_ALBUM_THUMBNAIL_CARD_H      7
#define MYSTERY_ALBUM_THUMBNAIL_DATE_W      10
#define MYSTERY_ALBUM_THUMBNAIL_DATE_H      2
#define MYSTERY_ALBUM_THUMBNAIL_GETPOKE_PLT       0
#define MYSTERY_ALBUM_THUMBNAIL_NOGETPOKE_PLT     5
#define MYSTERY_ALBUM_THUMBNAIL_NONE_PLT     3

#define MYSTERY_ALBUM_CARD_WRITE_SRC_START_X      0
#define MYSTERY_ALBUM_CARD_WRITE_SRC_START_Y      2
#define MYSTERY_ALBUM_CARD_WRITE_SRC_W            32


//このあたりはリソースのキャラ番号をいじっています
//書き込みキャラ取得
#define MYSTERY_ALBUM_CARD_WRITE_SRC_GET_CHARA(x,y)    (MYSTERY_ALBUM_CARD_WRITE_SRC_START_X +  \
                                                    MYSTERY_ALBUM_CARD_WRITE_SRC_START_Y * \
                                                    MYSTERY_ALBUM_CARD_WRITE_SRC_W + \
                                                    y * MYSTERY_ALBUM_CARD_WRITE_SRC_W + x)

#define MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_START_X     0
#define MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_START_Y      12

//背面書き込みキャラ取得
#define MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_GET_CHARA(x,y) ( MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_START_X +  \
                                                    MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_START_Y * \
                                                    MYSTERY_ALBUM_CARD_WRITE_SRC_W + \
                                                    y * MYSTERY_ALBUM_CARD_WRITE_SRC_W + x)

#define MYSTERY_ALBUM_CARD_NUMBER_SRC_START_X     24
#define MYSTERY_ALBUM_CARD_NUMBER_SRC_START_Y     2
#define MYSTERY_ALBUM_CARD_NUMBER_W               2
#define MYSTERY_ALBUM_CARD_NUMBER_H               2

//数字キャラのトップ取得
#define MYSTERY_ALBUM_CARD_NUMBER_GET_TOP(i)  (MYSTERY_ALBUM_CARD_NUMBER_SRC_START_X + \
  ( MYSTERY_ALBUM_CARD_WRITE_SRC_W * MYSTERY_ALBUM_CARD_NUMBER_H * (i % 3)) + \
  + (MYSTERY_ALBUM_CARD_NUMBER_W * (i / 3)))

//数字書き込みキャラ取得
#define MYSTERY_ALBUM_CARD_NUMBER_GET_CHARA(i,x,y)    (MYSTERY_ALBUM_CARD_NUMBER_GET_TOP(i) + \
                                                      MYSTERY_ALBUM_CARD_NUMBER_SRC_START_Y * \
                                                      MYSTERY_ALBUM_CARD_WRITE_SRC_W + \
                                                      y * MYSTERY_ALBUM_CARD_WRITE_SRC_W + x)


//=============================================================================
/**
 *          パブリック
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  アルバム作成
 *
 *	@param	const MYSTERY_ALBUM_SETUP *cp_setup 設定構造体
 *	@param	heapID                              ヒープID 
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_ALBUM_WORK * MYSTERY_ALBUM_Init( const MYSTERY_ALBUM_SETUP *cp_setup, HEAPID heapID )
{ 
  u32 card_num  = 0;
  MYSTERY_ALBUM_WORK *  p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_ALBUM_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_ALBUM_WORK) );
  p_wk->is_left = TRUE;
  p_wk->setup   = *cp_setup;
  p_wk->heapID  = heapID;

  GFL_STD_MemFill32( p_wk->res_cgx, GFL_CLGRP_REGISTER_FAILED, MYSTERY_CURSOR_MAX*2*sizeof(u32) );
  GFL_STD_MemFill32( p_wk->res_icon_plt, GFL_CLGRP_REGISTER_FAILED, MYSTERY_CURSOR_MAX*2*sizeof(u32) );
  GFL_STD_MemFill32( p_wk->res_cel, GFL_CLGRP_REGISTER_FAILED, MYSTERY_CURSOR_MAX*2*sizeof(u32) );

  //カード初期化
  { 
    int i;
    GIFT_PACK_DATA data;
    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
      if( MYSTERYDATA_IsExistsCard( p_wk->setup.p_sv, i) )
      { 
        card_num++;
        MYSTERYDATA_GetCardData( p_wk->setup.p_sv, i, &data );
        MYSTERY_CARD_DATA_Init( &p_wk->data[i], &data, &p_wk->setup, heapID );
      }
    }   
  }

  { 
    MYSTERY_SEQ_FUNCTION  start_seq;
    if( p_wk->setup.mode == MYSTERY_ALBUM_MODE_VIEW )
    { 
      start_seq = SEQFUNC_MoveCursor;
    }
    else
    { 
      start_seq = SEQFUNC_DeleteMsg;
    }
    p_wk->p_seq = MYSTERY_SEQ_Init( p_wk, start_seq, heapID );
  }

  Mystery_Album_InitDisplay( p_wk, heapID );
  Mystery_Album_CreateDisplay( p_wk, TRUE, 0, p_wk->now_page, heapID );


  //WINSET
  { 
    static const MYSTERY_MSGWINSET_SETUP_TBL c_setup_tbl[] =
    { 
      { 
        1,
        1,
        30,
        2,
        syachi_mystery_album_001,
        NULL,
        MYSTERY_MSGWIN_POS_WH_CENTER,
        0, 0,
        PRINTSYS_MACRO_LSB( 0xE, 0xF, 0 )
      },
      {
        1,
        0x16,
        11,
        2,
        syachi_mystery_album_002,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        PRINTSYS_MACRO_LSB( 0xE, 0xF, 0xA )
      },
      { 
        0x17,
        0x14,
        7,
        2,
        syachi_mystery_album_003,
        NULL,
        MYSTERY_MSGWIN_POS_WH_CENTER,
        0, 0,
        PRINTSYS_MACRO_LSB( 0xE, 0xF, 2 )
      },
    };
    static const MYSTERY_MSGWINSET_SETUP_TBL c_setup_nocard_tbl[] =
    { 
      { 
        1,
        1,
        30,
        2,
        syachi_mystery_album_001,
        NULL,
        MYSTERY_MSGWIN_POS_WH_CENTER,
        0, 0,
        PRINTSYS_MACRO_LSB( 0xE, 0xF, 0 )
      },
      { 
        0x17,
        0x14,
        7,
        2,
        syachi_mystery_album_003,
        NULL,
        MYSTERY_MSGWIN_POS_WH_CENTER,
        0, 0,
        PRINTSYS_MACRO_LSB( 0xE, 0xF, 2 )
      },
    };

    const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl;
    u32 tbl_max;

    if( card_num > 0 )
    { 
      cp_tbl  = c_setup_tbl;
      tbl_max = NELEMS(c_setup_tbl);
    }
    else
    { 
      cp_tbl  = c_setup_nocard_tbl;
      tbl_max = NELEMS(c_setup_nocard_tbl);
    }
    p_wk->p_winset  = MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE_AUTO, cp_tbl, tbl_max, MYSTERY_ALBUM_FONT_FRM, MYSTERY_ALBUM_TOP_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_msg, p_wk->setup.p_font, heapID );
  }


  //下画面カードのリソース
  { 
    MYSTERY_CARD_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_CARD_SETUP) );

    setup.back_frm          = MYSTERY_ALBUM_CARD_FRM_S;
    setup.font_frm          = MYSTERY_ALBUM_CARD_FONT_S;
    setup.back_plt_num      = MYSTERY_ALBUM_BG_BACK_S_PLT;
    setup.font_plt_num      = MYSTERY_ALBUM_BG_FONT_S_PLT;
    setup.icon_obj_plt_num  = MYSTERY_ALBUM_OBJ_ICON_S_PLT; 
    setup.silhouette_obj_plt_num  = MYSTERY_ALBUM_OBJ_SILHOUETTE_S_PLT;
    setup.p_clunit          = p_wk->setup.p_clunit;
    setup.p_sv              = p_wk->setup.p_sv;
    setup.p_msg             = p_wk->setup.p_msg; 
    setup.p_font            = p_wk->setup.p_font; 
    setup.p_que             = p_wk->setup.p_que; 
    setup.p_word            = p_wk->setup.p_word;

    p_wk->p_card_res  = MYSTERY_CARD_RES_Init( &setup, heapID );
  }

  //下画面カード
  if( Mystery_Album_GetDataNum(p_wk) != 0 )
  {
    int i;
    const MYSTERY_CARD_DATA *p_data;

    for( i = 0; i < MYSTERY_ALBUM_CARD_MAX; i++ )
    { 
      p_data = &p_wk->data[ i ];
      if( MYSTERY_CARD_DATA_IsExist( p_data ) )
      { 
        p_wk->p_card[i]  = MYSTERY_CARD_Init( MYSTERY_CARD_DATA_GetGiftBackData( p_data ),
            p_wk->p_card_res, cp_setup->p_gamedata, heapID );
      }
    }
    MYSTERY_CARD_Trans( p_wk->p_card[0], MYSTERY_CARD_TRANS_TYPE_NORMAL );
    GFL_HEAP_CheckHeapSafe( HEAPID_MYSTERYGIFT );
  }

  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, 0 );
  GFL_BG_SetScroll( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, 0 );
  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  アルバム破棄
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_ALBUM_Exit( MYSTERY_ALBUM_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < MYSTERY_ALBUM_CARD_MAX; i++ )
  { 
    if( p_wk->p_card[i] )
    { 
      MYSTERY_CARD_Exit( p_wk->p_card[i] );
      p_wk->p_card[i]  = NULL;
    }
  }

  MYSTERY_CARD_RES_Exit( p_wk->p_card_res );

  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Exit( p_wk->p_text );
    p_wk->p_text  = NULL;
  }

  GFL_BG_SetScroll( MYSTERY_ALBUM_BACK_FRM, GFL_BG_SCROLL_X_SET, 0 );
  GFL_BG_SetScroll( MYSTERY_ALBUM_CARD_FRM, GFL_BG_SCROLL_X_SET, 0 );

  MYSTERY_MSGWINSET_Clear( p_wk->p_winset );
  MYSTERY_MSGWINSET_Exit( p_wk->p_winset );

  Mystery_Album_DeleteDisplay( p_wk, TRUE );
  Mystery_Album_DeleteDisplay( p_wk, FALSE );
  Mystery_Album_ExitDisplay( p_wk );

  //カード破棄
  { 
    int i;
    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
      MYSTERY_CARD_DATA_Exit( &p_wk->data[i]);
    }   
  }
  GFL_BG_ClearScreen( MYSTERY_ALBUM_CARD_FONT_S );
  GFL_BG_ClearScreen( MYSTERY_ALBUM_CARD_FRM_S );
  GFL_BG_LoadScreenV_Req( MYSTERY_ALBUM_CARD_FONT_S );
  GFL_BG_LoadScreenV_Req( MYSTERY_ALBUM_CARD_FRM_S );

  MYSTERY_SEQ_Exit( p_wk->p_seq );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  アルバム  メイン処理
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_ALBUM_Main( MYSTERY_ALBUM_WORK *p_wk )
{ 
  //メイン処理
  MYSTERY_SEQ_Main( p_wk->p_seq );
  MYSTERY_CARD_RES_PrintMain( p_wk->p_card_res );
  //下画面カード読み替え処理
  if( p_wk->is_card_update )
  { 
    BOOL is_exist = FALSE;
    MYSTERY_CARD_DATA *p_data;

    if( p_wk->p_anm_card )
    { 
      MYSTERY_CARD_EndEffect( p_wk->p_anm_card );
      p_wk->p_anm_card  = NULL;
    }

    //作成
    if( p_wk->cursor < MYSTERY_CURSOR_MAX )
    { 
      p_data = &p_wk->data[ Mystery_Album_GetNowCardIndex( p_wk ) ];
      if( MYSTERY_CARD_DATA_IsExist( p_data ) )
      { 
        is_exist  = TRUE;
        //高速化のためにBG読まない（すでに読んでいるので）
        MYSTERY_CARD_Trans( p_wk->p_card[ Mystery_Album_GetNowCardIndex( p_wk ) ], MYSTERY_CARD_TRANS_TYPE_NOBG );
      }
    }

    //作成できなかったらスクリーンクリア
    if( is_exist == FALSE )
    { 
      MYSTERY_CARD_RES_Clear( p_wk->p_card_res );
    }

    p_wk->is_card_update  = FALSE;
  }

  //パレットフェード
  { 
    int i;
    const u16 add = 0x400;

    //パレットアニメ
    if( p_wk->plt_cnt + add >= 0x10000 )
    {
      p_wk->plt_cnt = p_wk->plt_cnt+add-0x10000;
    }
    else
    {
      p_wk->plt_cnt += add;
    }

    PLTANM_Main( &p_wk->pltanm[0], NNS_GFD_DST_2D_OBJ_PLTT_MAIN, MYSTERY_ALBUM_OBJ_CURSOR_PLT, p_wk->plt_cnt );
    PLTANM_Main( &p_wk->pltanm[1], NNS_GFD_DST_2D_OBJ_PLTT_MAIN, MYSTERY_ALBUM_OBJ_CURSOR_PLT+2, p_wk->plt_cnt );

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  文字描画メイン
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_ALBUM_PrintMain( MYSTERY_ALBUM_WORK *p_wk )
{ 
  MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset );
  if( p_wk->p_page_win )
  { 
    MYSTERY_MSGWIN_PrintMain( p_wk->p_page_win );
  }


  { 
    int i;
    for( i = 0; i < MYSTERY_ALBUM_CARD_MAX; i++ )
    { 
      if( p_wk->p_card[i] )
      { 
        if( p_wk->p_card[i] )
        { 
          MYSTERY_CARD_Main( p_wk->p_card[i] );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  アルバム  終了取得
 *
 *	@param	const MYSTERY_ALBUM_WORK *cp_wk   ワーク
 *
 *	@return TRUEでアルバム終了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_ALBUM_IsEnd( const MYSTERY_ALBUM_WORK *cp_wk )
{ 
  return MYSTERY_SEQ_IsEnd( cp_wk->p_seq );
}
//=============================================================================
/**
 *  アルバムプライベート
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  画面初期化
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_InitDisplay( MYSTERY_ALBUM_WORK *p_wk, HEAPID heapID )
{ 
  int i;
  MYSTERY_ALBUM_THUMBNAIL_DATA  data;

  { 
    ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
    //BG
    //上画面
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fushigi_album_NCLR,
				PALTYPE_MAIN_BG, MYSTERY_ALBUM_BG_BACK_PLT_M*0x20, 10*0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_album_NCGR, 
				MYSTERY_ALBUM_BACK_FRM, 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_album_NSCR,
				MYSTERY_ALBUM_BACK_FRM, 0, 0, FALSE, heapID );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_album_NCGR, 
				MYSTERY_ALBUM_FONT_FRM, 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_album03_NSCR,
				MYSTERY_ALBUM_FONT_FRM, 0, 0, FALSE, heapID );
    GFL_BG_LoadScreenReq( MYSTERY_ALBUM_BACK_FRM );

    //下画面
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fushigi_album_NCLR,
				PALTYPE_SUB_BG, MYSTERY_ALBUM_BG_BACK_PLT_S*0x20, 10*0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_album_NCGR, 
				MYSTERY_ALBUM_CARD_BACK_S, 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_album02_NSCR,
				MYSTERY_ALBUM_CARD_BACK_S, 0, 0, FALSE, heapID );

    GFL_ARC_CloseDataHandle( p_handle );

  }
  //フォント
  {	
    ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );

    //上下画面フォントパレット
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
        PALTYPE_MAIN_BG, MYSTERY_ALBUM_FONT_PLT*0x20, 0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
        PALTYPE_SUB_BG, MYSTERY_ALBUM_BG_FONT_S_PLT*0x20, 0x20, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }
  { 
    //キャラを単位で読み込み
    //BG_FRAME_M_TEXT
    { 
      GFL_BG_FillCharacter( MYSTERY_ALBUM_FONT_FRM, 0, 1, 0 );
      GFL_BG_FillCharacter( MYSTERY_ALBUM_CARD_FRM, 0, 1, 0 );
    }
    BmpWinFrame_GraphicSet( MYSTERY_ALBUM_LIST_FRM, 1, MYSTERY_ALBUM_BG_FRM_S_PLT, MENU_TYPE_SYSTEM, heapID );
  }

  { 
    ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );
    //共通ポケアイコン用パレット
    p_wk->res_poke_icon_plt = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
				POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAIN, MYSTERY_ALBUM_OBJ_POKEICON_PLT*0x20, heapID );
    GFL_ARC_CloseDataHandle( p_handle );
  }

  //カード作成
  for( i = 0; i < MYSTERY_CURSOR_MAX*2; i++ )
  { 
    //貼る面が右側だったらオフセットでずらす
    if( i >= MYSTERY_CURSOR_MAX )
    { 
      //データ取得
      data  = sc_thumbnail_data[i-MYSTERY_CURSOR_MAX];

      data.card_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
      data.date_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
    }
    else
    { 
      //データ取得
      data  = sc_thumbnail_data[i];
    }

    //作成
    p_wk->p_bmpwin[i] = GFL_BMPWIN_Create( MYSTERY_ALBUM_CARD_FRM, 
        data.date_x, data.date_y, MYSTERY_ALBUM_THUMBNAIL_DATE_W, MYSTERY_ALBUM_THUMBNAIL_DATE_H,
        MYSTERY_ALBUM_FONT_PLT, GFL_BMP_CHRAREA_GET_F );
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin[i] );
    GFL_BG_LoadScreenReq( MYSTERY_ALBUM_CARD_FRM );
  }

  //ページ作成( 0/3 )
  p_wk->p_page_win  = MYSTERY_MSGWIN_Init( MYSTERY_MSGWIN_TRANS_MODE_AUTO, MYSTERY_ALBUM_FONT_FRM, 13, 22, 6, 2, MYSTERY_ALBUM_TOP_FONT_PLT, p_wk->setup.p_que, heapID );

  //カーソル作成
  { 
    { 
      ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      p_wk->res_cursor_plt  = GFL_CLGRP_PLTT_RegisterEx( p_handle, 
        NARC_mystery_album_cursor_NCLR, CLSYS_DRAW_MAIN, MYSTERY_ALBUM_OBJ_CURSOR_PLT*0x20, 0, 6, heapID );
      p_wk->res_cursor_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_mystery_album_cursor_NCER, NARC_mystery_album_cursor_NANR, heapID );
      p_wk->res_cursor_cgx	= GFL_CLGRP_CGR_Register( p_handle,
              NARC_mystery_album_cursor_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

      //カーソルのパレットフェード用にパレットをメモリ読み込み
      PLTANM_Load( &p_wk->pltanm[0], p_handle, NARC_mystery_album_cursor_NCLR, 0, 1, heapID );
      PLTANM_Load( &p_wk->pltanm[1], p_handle, NARC_mystery_album_cursor_NCLR, 2, 3, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    { 
      GFL_CLWK_DATA cldata;
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x  = sc_thumbnail_data[0].card_x*8 - 24;
      cldata.pos_y  = sc_thumbnail_data[0].card_y*8 - 8;
      cldata.bgpri  = MYSTERY_ALBUM_FONT_FRM;
      cldata.softpri  = 1;

      p_wk->p_cursor    = GFL_CLACT_WK_Create( p_wk->setup.p_clunit, 
          p_wk->res_cursor_cgx, p_wk->res_cursor_plt, p_wk->res_cursor_cel,
          &cldata, CLSYS_DEFREND_MAIN, heapID );

      cldata.anmseq = 2;
      p_wk->p_swap_cursor    = GFL_CLACT_WK_Create( p_wk->setup.p_clunit, 
          p_wk->res_cursor_cgx, p_wk->res_cursor_plt, p_wk->res_cursor_cel,
          &cldata, CLSYS_DEFREND_MAIN, heapID );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, FALSE );

      cldata.softpri  = 0;
      {
        int i;
        for( i = 0; i < MYSTERY_ALLOW_MAX; i++ )
        { 
          if( i == 0 )
          { 
            cldata.pos_x  = 16;
            cldata.pos_y  = 16;
            cldata.anmseq = 9;
          }
          else
          { 
            cldata.pos_x  = 240;
            cldata.pos_y  = 16;
            cldata.anmseq = 9;
          }
          p_wk->p_allow[i]          = GFL_CLACT_WK_Create( p_wk->setup.p_clunit, 
              p_wk->res_cursor_cgx, p_wk->res_cursor_plt, p_wk->res_cursor_cel,
              &cldata, CLSYS_DEFREND_MAIN, heapID );
          GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_allow[i], TRUE );
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[i], FALSE );

          if( i == 0 )
          { 
            GFL_CLACT_WK_SetFlip( p_wk->p_allow[i], CLWK_FLIP_H, TRUE );
          }
          else
          { 
            GFL_CLACT_WK_SetFlip( p_wk->p_allow[i], CLWK_FLIP_H, FALSE );
          }
        }
      }
    }
  }

  GFL_BG_SetVisible( MYSTERY_ALBUM_CARD_FRM, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  画面破棄
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_ExitDisplay( MYSTERY_ALBUM_WORK *p_wk )
{ 

  GFL_BG_FillCharacterRelease( MYSTERY_ALBUM_CARD_FRM, 1, 0 );
  GFL_BG_FillCharacterRelease( MYSTERY_ALBUM_FONT_FRM, 1, 0 );

  //カーソル破棄
  { 
    int i;
    for( i = 0; i < MYSTERY_ALLOW_MAX; i++ )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_allow[i] );
    }
    GFL_CLACT_WK_Remove( p_wk->p_swap_cursor );
    GFL_CLACT_WK_Remove( p_wk->p_cursor );
    GFL_CLGRP_CGR_Release( p_wk->res_cursor_cgx );
    GFL_CLGRP_CELLANIM_Release( p_wk->res_cursor_cel );
    GFL_CLGRP_PLTT_Release( p_wk->res_cursor_plt );
  }

  MYSTERY_MSGWIN_Exit( p_wk->p_page_win );
  p_wk->p_page_win  = NULL;
  GFL_CLGRP_PLTT_Release( p_wk->res_poke_icon_plt );

  { 
    int i;
    for( i = 0; i < MYSTERY_CURSOR_MAX*2; i++ )
    {
      GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  画面作成  
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	is_front                  表につくるか、裏に作るか
 *	@param	page                      作成するページ
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_CreateDisplay( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, BOOL is_left, u32 page, HEAPID heapID )
{
  int i;

  for( i = 0; i < MYSTERY_CURSOR_MAX; i++ )
  { 
    Mystery_Album_CreateThumbnail( p_wk, is_front, is_left, page, i, heapID );
  }

  //ページを表示
  { 
    int page_max;
    STRBUF *p_strbuf_src;
    STRBUF *p_strbuf_dst;

    p_strbuf_src  = GFL_MSG_CreateString( p_wk->setup.p_msg, syachi_mystery_album_004 );
    p_strbuf_dst  = GFL_MSG_CreateString( p_wk->setup.p_msg, syachi_mystery_album_004 );
    WORDSET_RegisterNumber( p_wk->setup.p_word, 0, page+1, 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    page_max =  Mystery_Album_GetPageMax( p_wk );
    WORDSET_RegisterNumber( p_wk->setup.p_word, 1, page_max, 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( p_wk->setup.p_word, p_strbuf_src, p_strbuf_dst );

    MYSTERY_MSGWIN_SetColor( p_wk->p_page_win, PRINTSYS_MACRO_LSB( 0xE, 0xF, 0xA ) );
    MYSTERY_MSGWIN_SetPos( p_wk->p_page_win, 0, 0, MYSTERY_MSGWIN_POS_WH_CENTER );
    MYSTERY_MSGWIN_PrintBuf( p_wk->p_page_win, p_strbuf_src, p_wk->setup.p_font );

    GFL_STR_DeleteBuffer( p_strbuf_src );
    GFL_STR_DeleteBuffer( p_strbuf_dst );

  }
  GFL_BG_LoadScreenReq( MYSTERY_ALBUM_BACK_FRM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  画面破棄
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk ワーク
 *	@param  is_front  表を破棄するか、裏を破棄するか
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_DeleteDisplay( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front )
{ 
  int i;
  for( i = 0; i < MYSTERY_CURSOR_MAX; i++ )
  { 
    Mystery_Album_DeleteThumbnail( p_wk, is_front, i );
  }
  GFL_BG_LoadScreenReq( MYSTERY_ALBUM_BACK_FRM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  サムネイルを作成
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	is_front                  全面か背面か
 *	@param	is_left                   左に動くか右に動くか
 *	@param	page                      何ページ目か
 *	@param  index                     何番目のサムネイルか
 *	@param	heapID                    ヒープID
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_CreateThumbnail( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, BOOL is_left, u32 page, u16 index, HEAPID heapID )
{
  MYSTERY_CARD_DATA *p_src;
  MYSTERY_ALBUM_THUMBNAIL_DATA  data;
  u32 dst_idx;


  //カードデータ取得
  p_src = &p_wk->data[ Mystery_Album_GetCardIndex( page, index ) ];

  //カードの中身があったら貼付け---
  //貼り付ける位置を取得
  dst_idx = index;

  //座標データ取得
  data  = sc_thumbnail_data[index];

  //貼る面が右側だったらオフセットでずらす
  if( (is_front && !p_wk->is_left)
      || (!is_front && p_wk->is_left) )
  { 
    dst_idx += MYSTERY_CURSOR_MAX;

    data.card_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
    data.date_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
  }

  //数字キャラ貼付け
  //カードのキャラをスクリーンに貼り付ける
  { 
    int j, k;
    u16 chr_num;
    u8 card_index;
    for( j = 0; j < MYSTERY_ALBUM_CARD_NUMBER_H; j++ )
    { 
      for( k = 0; k < MYSTERY_ALBUM_CARD_NUMBER_W; k++ )
      { 
        card_index  = Mystery_Album_GetCardIndex( page, index );
        chr_num = MYSTERY_ALBUM_CARD_NUMBER_GET_CHARA( card_index, k, j );

        NAGI_Printf( "cardindex %d top %d\n", card_index, MYSTERY_ALBUM_CARD_NUMBER_GET_TOP(card_index) );

        GFL_BG_FillScreen( MYSTERY_ALBUM_BACK_FRM, chr_num,
            data.card_x + k - MYSTERY_ALBUM_CARD_NUMBER_W, 
            data.card_y + j, 1, 1, MYSTERY_ALBUM_THUMBNAIL_NONE_PLT );
      }
    }
  }

  if( !MYSTERY_CARD_DATA_IsExist( p_src ) )
  {
    //カードの中身がなかったら、背面貼付け--
    //カードのキャラをスクリーンに貼り付ける
    { 
      int i,j;
      u16 chr_num;
      for( i = 0; i < MYSTERY_ALBUM_THUMBNAIL_CARD_H; i++ )
      { 
        for( j = 0; j < MYSTERY_ALBUM_THUMBNAIL_CARD_W; j++ )
        { 
          chr_num = MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_GET_CHARA( j, i );

          GFL_BG_FillScreen( MYSTERY_ALBUM_BACK_FRM, chr_num,
              data.card_x + j, data.card_y + i, 1, 1, MYSTERY_ALBUM_THUMBNAIL_NONE_PLT );
        }
      }
    }

    //日付BMPWINをクリア
    { 
      GFL_BMP_DATA  *p_dst  = GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[dst_idx] );
      GFL_BMP_Clear( p_dst, 0 );
      GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[dst_idx] );
    }


  }
  else
  { 
    //カードのキャラをスクリーンに貼り付ける
    { 
      int i,j;
      u16 chr_num;
      for( i = 0; i < MYSTERY_ALBUM_THUMBNAIL_CARD_H; i++ )
      { 
        for( j = 0; j < MYSTERY_ALBUM_THUMBNAIL_CARD_W; j++ )
        { 
          chr_num = MYSTERY_ALBUM_CARD_WRITE_SRC_GET_CHARA( j, i );

          GFL_BG_FillScreen( MYSTERY_ALBUM_BACK_FRM, chr_num,
              data.card_x + j, data.card_y + i, 1, 1, MYSTERY_ALBUM_THUMBNAIL_NOGETPOKE_PLT );
        }
      }
    }

    //タイプと取得状況によってカードをパレットチェンジ
    { 
      const u32 plt      = MYSTERY_CARD_DATA_GetThumbnailPltOfs( p_src );

      GFL_BG_ChangeScreenPalette( MYSTERY_ALBUM_BACK_FRM, data.card_x, data.card_y,
          MYSTERY_ALBUM_THUMBNAIL_CARD_W, MYSTERY_ALBUM_THUMBNAIL_CARD_H,
          plt );
    }

    //日付を貼り付ける
    { 
      const GFL_BMP_DATA  *cp_src  = MYSTERY_CARD_DATA_GetBmp( p_src );
      GFL_BMP_DATA  *p_dst  = GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[dst_idx] );
      GFL_BMP_Copy( cp_src, p_dst );
      GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[dst_idx] );
    }

    //アイコンを作成する
    //  日付のように、作成しておいて、設定するだけにしたかったのだが、
    //  リソースの関係でやむをえずここで作成する
    { 
      //リソース読みこみ
      {	
        ARCHANDLE	*	p_handle	= MYSTERY_CARD_DATA_GetArcHandle( p_src, heapID );

        if( MYSTERY_CARD_DATA_GetType(p_src) != MYSTERYGIFT_TYPE_POKEMON )
        { 
          p_wk->res_icon_plt[ dst_idx ]	= GFL_CLGRP_PLTT_RegisterEx( p_handle, 
              MYSTERY_CARD_DATA_GetResPlt(p_src), CLSYS_DRAW_MAIN, (MYSTERY_ALBUM_OBJ_ICON_PLT+dst_idx)*0x20, 0,1,heapID );
        }
        p_wk->res_cel[ dst_idx ]	= GFL_CLGRP_CELLANIM_Register( p_handle,
            MYSTERY_CARD_DATA_GetResCel(p_src), MYSTERY_CARD_DATA_GetResAnm(p_src), heapID );
        p_wk->res_cgx[ dst_idx ]	= GFL_CLGRP_CGR_Register( p_handle,
            MYSTERY_CARD_DATA_GetResCgx(p_src), FALSE, CLSYS_DRAW_MAIN, heapID );

        GFL_ARC_CloseDataHandle( p_handle );
      }
      //アイコン作成
      { 
        u32 plt;
        s8 ofs_x = 0, ofs_y = 0;
        GFL_CLWK_DATA cldata;
        GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
        cldata.pos_y  = data.icon_y;
        cldata.bgpri  = MYSTERY_ALBUM_FONT_FRM;

        //アイテムアイコンのリソースだけ、ずれているため
        if(MYSTERY_CARD_DATA_GetType(p_src) == MYSTERYGIFT_TYPE_ITEM )
        { 
          ofs_x   = 4;
          ofs_y   = 8;
        }

        if( is_front )
        { 
          cldata.pos_x = data.icon_x;
        }
        else
          { 
            if( is_left )
            { 
              cldata.pos_x = data.icon_x - MYSTERY_ALBUM_CARD_BACKSURFACE_OFS;
            }
            else if( !is_left )
            { 
              cldata.pos_x = data.icon_x + MYSTERY_ALBUM_CARD_BACKSURFACE_OFS;
            }
            else
            { 
              cldata.pos_x = data.icon_x;
            }
          }
        cldata.pos_x  += ofs_x;
        cldata.pos_y  += ofs_y;

        //ポケモンなら共通アイコンパレット
        //それ以外ならばそれぞれのパレット
        plt = ( MYSTERY_CARD_DATA_GetType(p_src) == MYSTERYGIFT_TYPE_POKEMON ) ?
          p_wk->res_poke_icon_plt: p_wk->res_icon_plt[ dst_idx ];

        p_wk->p_icon[ dst_idx ] = GFL_CLACT_WK_Create( p_wk->setup.p_clunit,
            p_wk->res_cgx[ dst_idx ], plt, p_wk->res_cel[ dst_idx ],
            &cldata, CLSYS_DEFREND_MAIN, p_wk->heapID );
        GFL_CLACT_WK_SetPlttOffs( p_wk->p_icon[ dst_idx ],
            MYSTERY_CARD_DATA_GetPltOfs( p_src ), CLWK_PLTTOFFS_MODE_OAM_COLOR );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[ dst_idx ], TRUE );
      }

      //入れ替えカーソル
      if( p_wk->is_swap && Mystery_Album_GetCardIndex( page, index ) == p_wk->swap_card_index )
      { 
        GFL_CLACTPOS  pos;
        pos.x = sc_thumbnail_data[index].card_x*8 - 24;
        pos.y = sc_thumbnail_data[index].card_y*8 - 8;
        if( is_front )
        { 
        }
        else
        { 
          if( is_left )
          { 
            pos.x -= MYSTERY_ALBUM_CARD_BACKSURFACE_OFS;
          }
          else if( !is_left )
          { 
            pos.x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS;
          }
        }
        GFL_CLACT_WK_SetPos( p_wk->p_swap_cursor, &pos, CLSYS_DEFREND_MAIN );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  サムネイルを破棄
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	is_front                  TRUEならば全面  FALSEならば背面
 *	@param  index                     その面のどのサムネイルか
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_DeleteThumbnail( MYSTERY_ALBUM_WORK *p_wk, BOOL is_front, u16 index )
{ 
  //貼り付ける位置を取得
  MYSTERY_ALBUM_THUMBNAIL_DATA  data;
  u16 dst_idx = index;

  //座標データ取得
  data  = sc_thumbnail_data[index];

  //貼る面が右側だったらオフセットでずらす
  if( (is_front && !p_wk->is_left)
      || (!is_front && p_wk->is_left) )
  { 
    dst_idx += MYSTERY_CURSOR_MAX;

    data.card_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
    data.date_x += MYSTERY_ALBUM_CARD_BACKSURFACE_OFS/8;
  }


  //カードのキャラを消す
  { 
    //カードのキャラをスクリーンに貼り付ける
    int i,j;
    u16 chr_num;
    for( i = 0; i < MYSTERY_ALBUM_THUMBNAIL_CARD_H; i++ )
    { 
      for( j = 0; j < MYSTERY_ALBUM_THUMBNAIL_CARD_W; j++ )
      { 
        chr_num = MYSTERY_ALBUM_CARD_WRITE_BLANK_SRC_GET_CHARA( j, i );
        GFL_BG_FillScreen( MYSTERY_ALBUM_BACK_FRM, chr_num,
            data.card_x + j, data.card_y + i, 1, 1, MYSTERY_ALBUM_THUMBNAIL_NONE_PLT );
      }
    }
  }

  //日付を消す
  { 
    GFL_BMP_DATA  *p_dst  = GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[dst_idx] );
    GFL_BMP_Clear( p_dst, 0 );
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[dst_idx] );
  }

  //アイコンを消す
  if( p_wk->p_icon[ dst_idx ] != NULL )
  { 
    GFL_CLACT_WK_Remove( p_wk->p_icon[ dst_idx ] );
    p_wk->p_icon[ dst_idx ] = NULL;

    //リソースを消す
    if( p_wk->res_icon_plt[dst_idx] != GFL_CLGRP_REGISTER_FAILED )
    { 
      GFL_CLGRP_PLTT_Release( p_wk->res_icon_plt[dst_idx] );
      p_wk->res_icon_plt[dst_idx] = GFL_CLGRP_REGISTER_FAILED;
    }
    if( p_wk->res_cgx[dst_idx] != GFL_CLGRP_REGISTER_FAILED )
    { 
      GFL_CLGRP_CGR_Release( p_wk->res_cgx[dst_idx] );
      p_wk->res_cgx[dst_idx] = GFL_CLGRP_REGISTER_FAILED;
    }
    if( p_wk->res_cel[dst_idx] != GFL_CLGRP_REGISTER_FAILED )
    { 
      GFL_CLGRP_CELLANIM_Release( p_wk->res_cel[dst_idx] );
      p_wk->res_cel[dst_idx] = GFL_CLGRP_REGISTER_FAILED;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ページ動作開始
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	is_left                   TRUEで左に動作  FALSEで右に動作
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_StartScrollPage( MYSTERY_ALBUM_WORK *p_wk, BOOL is_left )
{ 
  p_wk->scroll_req   = is_left? -1: 1;
  p_wk->scroll_cnt    = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ページ動作
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *
 *	@return TRUEで動作終了  FALSEで動作中
 */
//-----------------------------------------------------------------------------
static BOOL Mystery_Album_ScrollPage( MYSTERY_ALBUM_WORK *p_wk )
{ 
  if( p_wk->scroll_req != 0 )
  { 
    int i;
    GFL_CLACTPOS  pos;
    s32 value = p_wk->scroll_req * MYSTERY_ALBUM_MOVE_SCROLL_DISTANCE / MYSTERY_ALBUM_MOVE_SCROLL_SYNC;

    //CLWK
    for( i = 0; i < MYSTERY_CURSOR_MAX*2; i++ )
    { 
      if( p_wk->p_icon[i])
      { 
        //加算
        GFL_CLACT_WK_GetPos( p_wk->p_icon[i], &pos, CLSYS_DEFREND_MAIN );
        pos.x -= value;
        GFL_CLACT_WK_SetPos( p_wk->p_icon[i], &pos, CLSYS_DEFREND_MAIN );

        //カリング
        { 
          GFL_CLACTPOS  pos;
          GFL_CLACT_WK_GetPos( p_wk->p_icon[i], &pos, CLSYS_DEFREND_MAIN );
          if( -16 <= pos.x && pos.x <= 256+16
            && -16 <= pos.y && pos.y <= 192+16)
          { 
            GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], TRUE );
          }
          else
          { 
            GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], FALSE );
          }
        }
      }
    }
    //加算
    if( p_wk->is_swap )
    { 
      GFL_CLACT_WK_GetPos( p_wk->p_swap_cursor, &pos, CLSYS_DEFREND_MAIN );
      pos.x -= value;
      GFL_CLACT_WK_SetPos( p_wk->p_swap_cursor, &pos, CLSYS_DEFREND_MAIN );
      if( -12*8 <= pos.x && pos.x <= 256+12*8
          && -12*8 <= pos.y && pos.y <= 192+12*8)
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, TRUE );
      }
      else
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, FALSE );
      }
    }

    //BG
    GFL_BG_SetScroll( MYSTERY_ALBUM_BACK_FRM, GFL_BG_SCROLL_X_INC, value );
    GFL_BG_SetScroll( MYSTERY_ALBUM_CARD_FRM, GFL_BG_SCROLL_X_INC, value );

    //終了
    if( p_wk->scroll_cnt++ >= MYSTERY_ALBUM_MOVE_SCROLL_SYNC-1 )
    { 
      //終了時、どちらの面か決定する
      p_wk->is_left ^= 1;
      p_wk->scroll_req = 0;
    }

    return FALSE;
  }
  else
  { 
    return TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードを消去
 *  
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	card_index                消去するカードインデックス
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_RemoveCard( MYSTERY_ALBUM_WORK *p_wk, u32 card_index )
{ 
  //カード消去
  MYSTERYDATA_RemoveCardData( p_wk->setup.p_sv, card_index);

  //カードデータ作り直し
  { 
    int i;
    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
      MYSTERY_CARD_DATA_Exit( &p_wk->data[i]);
    }

    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
      if( MYSTERYDATA_IsExistsCard( p_wk->setup.p_sv, i) )
      { 
        GIFT_PACK_DATA data;
        MYSTERYDATA_GetCardData( p_wk->setup.p_sv, i, &data );
        MYSTERY_CARD_DATA_Init( &p_wk->data[i], &data, &p_wk->setup, p_wk->heapID );
      }
    }   
  }


  //下画面カードリソース
  {
    int i;
    if( p_wk->p_card[card_index] )
    { 
      MYSTERY_CARD_Exit( p_wk->p_card[card_index] );
      p_wk->p_card[card_index]  = NULL;
    }

    for( i = card_index; i < MYSTERY_ALBUM_CARD_MAX-1; i++ )
    {
      if( p_wk->p_card[i] == NULL )
      {
        MYSTERY_CARD_WORK *p_temp = p_wk->p_card[i];
        p_wk->p_card[i] = p_wk->p_card[i+1];
        p_wk->p_card[i+1] = p_temp;
      }
    }
  }

  p_wk->is_change = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードを入れ替え
 *
 *	@param	MYSTERY_ALBUM_WORK *p_wk  ワーク
 *	@param	card_index1               入れ替えるカード１
 *	@param	card_index2               入れ替えるカード２
 */
//-----------------------------------------------------------------------------
static void Mystery_Album_SwapCard( MYSTERY_ALBUM_WORK *p_wk, u32 card_index1, u32 card_index2 )
{ 
  MYSTERY_CARD_DATA temp;
  GIFT_PACK_DATA*   p_gift;

  //スワップ
  MYSTERYDATA_SwapCard( p_wk->setup.p_sv, card_index1, card_index2 );

  //データ作り直し
/*
  MYSTERY_CARD_DATA_Exit( &p_wk->data[ card_index1] );
  p_gift  = MYSTERYDATA_GetCardDataOld( p_wk->setup.p_sv, card_index1 );
  MYSTERY_CARD_DATA_Init( &p_wk->data[ card_index1], p_gift, &p_wk->setup, p_wk->heapID );

  MYSTERY_CARD_DATA_Exit( &p_wk->data[ card_index2] );
  p_gift  = MYSTERYDATA_GetCardDataOld( p_wk->setup.p_sv, card_index2 );
  MYSTERY_CARD_DATA_Init( &p_wk->data[ card_index2], p_gift, &p_wk->setup, p_wk->heapID );
*/
  //カード作り直し
  { 
    int i;
    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
        MYSTERY_CARD_DATA_Exit( &p_wk->data[i]);
    }

    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
    {
      if( MYSTERYDATA_IsExistsCard( p_wk->setup.p_sv, i) )
      { 
        GIFT_PACK_DATA data;
        MYSTERYDATA_GetCardData( p_wk->setup.p_sv, i, &data );
        MYSTERY_CARD_DATA_Init( &p_wk->data[i], &data, &p_wk->setup, p_wk->heapID );
      }
    }   
  }

  //下画面カード移動
  {
    MYSTERY_CARD_Exit( p_wk->p_card[card_index1] );
    MYSTERY_CARD_Exit( p_wk->p_card[card_index2] );

    p_wk->p_card[card_index1]  = MYSTERY_CARD_Init( 
        MYSTERY_CARD_DATA_GetGiftBackData( &p_wk->data[card_index1] ),
            p_wk->p_card_res, p_wk->setup.p_gamedata, p_wk->heapID );

    p_wk->p_card[card_index2]  = MYSTERY_CARD_Init( 
        MYSTERY_CARD_DATA_GetGiftBackData( &p_wk->data[card_index2] ),
            p_wk->p_card_res, p_wk->setup.p_gamedata, p_wk->heapID );
  }

  p_wk->is_change = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ページ最大数を取得
 *
 *	@param	const MYSTERY_ALBUM_WORK *cp_wk   ワーク
 *
 *	@return ページ最大数
 */
//-----------------------------------------------------------------------------
static u32 Mystery_Album_GetPageMax( const MYSTERY_ALBUM_WORK *cp_wk )
{ 
  int cnt;
  u32 max;
#if 0 //カードがある分だけページが開くのではなく
      //つねに全てのページが開くようになりました

  cnt = Mystery_Album_GetDataNum( cp_wk );

  max = 1 + cnt / MYSTERY_ALBUM_PAGE_IN_CARD_NUM;
  max = MATH_IMin( max, MYSTERY_ALBUM_PAGE_MAX );

  return max;
#else
  return MYSTERY_ALBUM_PAGE_MAX;
#endif 
}
//----------------------------------------------------------------------------
/**
 *	@brief  データ数を取得
 *
 *	@param	const MYSTERY_ALBUM_WORK *cp_wk   ワーク
 *
 *	@return データ数を取得
 */
//-----------------------------------------------------------------------------
static u32 Mystery_Album_GetDataNum( const MYSTERY_ALBUM_WORK *cp_wk )
{ 

  int i;
  int cnt;

  cnt = 0;
  for( i = 0; i < MYSTERY_DATA_GetCardMax( cp_wk->setup.p_sv ); i++ )
  { 
    if( MYSTERY_CARD_DATA_IsExist(&cp_wk->data[i]))
    { 
      cnt++;
    }
  }
  return cnt;
}

//=============================================================================
/**
 *    カードデータ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  カードデータを作成
 *
 *	@param	MYSTERY_CARD_DATA *p_wk ワーク
 *	@param	*p_data                 データ
 *	@param  p_unit                  ユニット
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_CARD_DATA_Init( MYSTERY_CARD_DATA *p_wk, const GIFT_PACK_DATA *cp_data, const MYSTERY_ALBUM_SETUP *cp_setup, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_CARD_DATA) );

  if( cp_data )
  { 
    p_wk->is_exist  = TRUE;
    p_wk->data    = *cp_data;

    p_wk->p_bmp     = GFL_BMP_Create( MYSTERY_ALBUM_THUMBNAIL_DATE_W, MYSTERY_ALBUM_THUMBNAIL_DATE_H,
                      GFL_BMP_16_COLOR, heapID);

    //BMPに文字描画しておく
    { 
      STRBUF *p_strbuf_src;
      STRBUF *p_strbuf_dst;
      s32 y,m,d;

      y = MYSTERYDATA_GetYear( cp_data->recv_date );
      m = MYSTERYDATA_GetMonth( cp_data->recv_date );
      d = MYSTERYDATA_GetDay( cp_data->recv_date );

      p_strbuf_src  = GFL_MSG_CreateString( cp_setup->p_msg, syachi_mystery_album_005 );
      p_strbuf_dst  = GFL_MSG_CreateString( cp_setup->p_msg, syachi_mystery_album_005 );
      WORDSET_RegisterNumber( cp_setup->p_word, 0, y, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_RegisterNumber( cp_setup->p_word, 1, m, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_RegisterNumber( cp_setup->p_word, 2, d, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( cp_setup->p_word, p_strbuf_src, p_strbuf_dst );

      {
        s16 x, y;
        x = MYSTERY_ALBUM_THUMBNAIL_DATE_W * 8 / 2;
        y = MYSTERY_ALBUM_THUMBNAIL_DATE_H * 8 / 2;
        x -= PRINTSYS_GetStrWidth( p_strbuf_src, cp_setup->p_font, 0 ) / 2;
        y -= PRINTSYS_GetStrHeight( p_strbuf_src, cp_setup->p_font ) / 2;
        PRINTSYS_Print( p_wk->p_bmp, x, y, p_strbuf_src, cp_setup->p_font );
      }

      GFL_STR_DeleteBuffer( p_strbuf_src );
      GFL_STR_DeleteBuffer( p_strbuf_dst );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードデータを破棄
 *
 *	@param	MYSTERY_CARD_DATA *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_CARD_DATA_Exit( MYSTERY_CARD_DATA *p_wk )
{ 
  if( p_wk->is_exist )
  { 
    GFL_BMP_Delete( p_wk->p_bmp );
    GFL_STD_MemClear( p_wk, sizeof(MYSTERY_CARD_DATA) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  カードデータが存在するか
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return TRUEならば存在  FALSEならばなし
 */
//-----------------------------------------------------------------------------
static BOOL MYSTERY_CARD_DATA_IsExist( const MYSTERY_CARD_DATA *cp_wk )
{ 
  return cp_wk->is_exist;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINデータを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return GFL_BMP_DATA  BMP
 */
//-----------------------------------------------------------------------------
static const GFL_BMP_DATA *MYSTERY_CARD_DATA_GetBmp( const MYSTERY_CARD_DATA *cp_wk )
{ 
  return cp_wk->p_bmp;
}
//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブ作成
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *	@param	heapID  ヒープID
 *
 *	@return アーカイブ
 */
//-----------------------------------------------------------------------------
static ARCHANDLE * MYSTERY_CARD_DATA_GetArcHandle( const MYSTERY_CARD_DATA *cp_wk, HEAPID heapID )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    return GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );

  case MYSTERYGIFT_TYPE_ITEM:
    return GFL_ARC_OpenDataHandle( ARCID_ITEMICON, heapID );

  default:
    return GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return パレット
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetResPlt( const MYSTERY_CARD_DATA *cp_wk )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    return POKEICON_GetPalArcIndex();

  case MYSTERYGIFT_TYPE_ITEM:
    { 
      const GIFT_PACK_DATA *cp_data = MYSTERY_CARD_DATA_GetGiftBackData(cp_wk );
      const GIFT_PRESENT_ITEM *cp_item = &cp_data->data.item;
      return ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_PAL );
    }

  default:
    return NARC_mystery_fushigi_box_NCLR;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットオフセット
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return パレットオフセット
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetPltOfs( const MYSTERY_CARD_DATA *cp_wk )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    { 
      const GIFT_PACK_DATA *cp_data = MYSTERY_CARD_DATA_GetGiftBackData(cp_wk );
      const GIFT_PRESENT_POKEMON *cp_pokemon = &cp_data->data.pokemon;
      return POKEICON_GetPalNum( cp_pokemon->mons_no, cp_pokemon->form_no, cp_pokemon->sex, cp_pokemon->egg );
    }

  default:
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  キャラクターを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return キャラクターを取得
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetResCgx( const MYSTERY_CARD_DATA *cp_wk )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    { 
      const GIFT_PACK_DATA *cp_data = MYSTERY_CARD_DATA_GetGiftBackData(cp_wk );
      const GIFT_PRESENT_POKEMON *cp_pokemon = &cp_data->data.pokemon;
      return POKEICON_GetCgxArcIndexByMonsNumber( cp_pokemon->mons_no, cp_pokemon->form_no, cp_pokemon->sex, cp_pokemon->egg );
    }

  case MYSTERYGIFT_TYPE_ITEM:
    { 
      const GIFT_PACK_DATA *cp_data = MYSTERY_CARD_DATA_GetGiftBackData(cp_wk );
      const GIFT_PRESENT_ITEM *cp_item = &cp_data->data.item;
      return ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_CGX );
    }

  default:
    return NARC_mystery_fushigi_box_NCGR;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  セルデータを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return セルデータ
 */
//-----------------------------------------------------------------------------}
static u32 MYSTERY_CARD_DATA_GetResCel( const MYSTERY_CARD_DATA *cp_wk )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    return POKEICON_GetCellArcIndex();

  case MYSTERYGIFT_TYPE_ITEM:
    return NARC_item_icon_itemicon_NCER;

  default:
    return NARC_mystery_fushigi_box_NCER;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  アニメデータを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk ワーク
 *
 *	@return アニメデータ
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetResAnm( const MYSTERY_CARD_DATA *cp_wk )
{ 
  switch( MYSTERY_CARD_DATA_GetType(cp_wk) )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    return POKEICON_GetAnmArcIndex();

  case MYSTERYGIFT_TYPE_ITEM:
    return NARC_item_icon_itemicon_NANR;

  default:
    return NARC_mystery_fushigi_box_NANR;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードの種類取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return タイプ取得
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetType( const MYSTERY_CARD_DATA *cp_wk )
{ 
  return cp_wk->data.gift_type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードのデータを取得
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return データ
 */
//-----------------------------------------------------------------------------
static const GIFT_PACK_DATA *MYSTERY_CARD_DATA_GetGiftBackData( const MYSTERY_CARD_DATA *cp_wk )
{ 
  return &cp_wk->data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  貰っているかどうか
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk 
 *
 *	@return TRUEで受取済み  FALSEでまだ受け取っていない
 */
//-----------------------------------------------------------------------------
static BOOL MYSTERY_CARD_DATA_IsHave( const MYSTERY_CARD_DATA *cp_wk )
{ 
  return cp_wk->data.have;
}
//----------------------------------------------------------------------------
/**
 *	@brief  サムネイルのパレットオフセット
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return カードのパレット
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetThumbnailPltOfs( const MYSTERY_CARD_DATA *cp_wk )
{ 
  enum
  { 
    TYPE_PLT_TBL_DONTHAVE_OFS  = 5,
  };
  //もらった場合のテーブル。もらっていない場合は＋TYPE_PLT_TBL_DONTHAVE_OFS
  static const sc_type_plt_tbl[]  =
  { 
    0,//なにもない
    0,//ポケモン
    1,//道具
    2,//GPOWER
  };

  const u32 type      = MYSTERY_CARD_DATA_GetType( cp_wk );
  const u32 have_ofs  = MYSTERY_CARD_DATA_IsHave( cp_wk ) ? 0: TYPE_PLT_TBL_DONTHAVE_OFS;
  GF_ASSERT( type < NELEMS(sc_type_plt_tbl) );
  return sc_type_plt_tbl[ type ] + have_ofs;
}
//----------------------------------------------------------------------------
/**
 *	@brief  カードのパレットオフセット
 *
 *	@param	const MYSTERY_CARD_DATA *cp_wk  ワーク
 *
 *	@return カードのパレット
 */
//-----------------------------------------------------------------------------
static u32 MYSTERY_CARD_DATA_GetCardPltOfs( const MYSTERY_CARD_DATA *cp_wk )
{ 
  enum
  { 
    TYPE_PLT_TBL_HAVE_OFS  = 3,
  };
  //もらっていない場合のテーブル。もらってる場合は＋TYPE_PLT_TBL_HAVE_OFS
  static const sc_type_plt_tbl[]  =
  { 
    0,//なにもない
    0,//ポケモン
    1,//道具
    2,//GPOWER
  };

  const u32 type      = MYSTERY_CARD_DATA_GetType( cp_wk );
  const u32 have_ofs  = MYSTERY_CARD_DATA_IsHave( cp_wk ) ? TYPE_PLT_TBL_HAVE_OFS: 0;
  GF_ASSERT( type < NELEMS(sc_type_plt_tbl) );
  return sc_type_plt_tbl[ type ] + have_ofs;
}
//=============================================================================
/**
 *    シーケンス
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  カーソル移動
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MoveCursor( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_MOVE,
    SEQ_SCROLL,
  };


  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_MOVE:
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[0], TRUE );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[1], TRUE );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, TRUE );
    }
#if 0
    //カーソル、矢印出現
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, TRUE );
      if( p_wk->cursor == MYSTERY_CURSOR_RETURN )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow, FALSE );
      }
      else
      { 
        if( p_wk->cursor == MYSTERY_CURSOR_LEFT_UP || p_wk->cursor == MYSTERY_CURSOR_LEFT_DOWN )
        { 
          GFL_CLACT_WK_SetTypePos( p_wk->p_allow,  16, CLSYS_DEFREND_MAIN, CLSYS_MAT_X );
          GFL_CLACT_WK_SetFlip( p_wk->p_allow, CLWK_FLIP_H, TRUE );
        }
        else if( p_wk->cursor == MYSTERY_CURSOR_RIGHT_UP || p_wk->cursor == MYSTERY_CURSOR_RIGHT_DOWN )
        { 
          GFL_CLACT_WK_SetTypePos( p_wk->p_allow,  240, CLSYS_DEFREND_MAIN, CLSYS_MAT_X );
          GFL_CLACT_WK_SetFlip( p_wk->p_allow, CLWK_FLIP_H, FALSE );
        }
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow, TRUE );
      }
    }
#endif

    //入力
    { 
      const int key = GFL_UI_KEY_GetRepeat();
      BOOL is_update  = FALSE;
      s8  scroll  = 0;  //1だと右へ-1だと左へ

      //タッチ入力
      if( GFL_UI_TP_GetTrg() )
      { 
        u16 idx = Mystery_Album_GetNowCardIndex( p_wk );
        p_wk->p_anm_card  = p_wk->p_card[ idx ];
        if( p_wk->p_anm_card )
        { 
          MYSTERY_CARD_StartEffect( p_wk->p_anm_card );
        }
      }

      //キー入力
      if( key == PAD_BUTTON_L )
      { 
        scroll  = -1;
      }
      else if( key == PAD_BUTTON_R )
      {          
        scroll  = 1; 
      }
      else if( key & PAD_KEY_UP )
      { 
        PMSND_PlaySE( MYSTERY_SE_SELCT );
        switch( p_wk->cursor )
        { 
        case MYSTERY_CURSOR_LEFT_UP:
          if( p_wk->is_swap )
          {
            p_wk->cursor  = MYSTERY_CURSOR_LEFT_DOWN;
          }
          else
          { 
            p_wk->key_btn_save  = TRUE;
            p_wk->cursor  = MYSTERY_CURSOR_RETURN;
          }
          break;

        case MYSTERY_CURSOR_RIGHT_UP:
          if( p_wk->is_swap )
          { 
            p_wk->cursor  = MYSTERY_CURSOR_RIGHT_DOWN;
          }
          else
          { 
            p_wk->key_btn_save  = FALSE;
            p_wk->cursor  = MYSTERY_CURSOR_RETURN;
          }
          break;

        case MYSTERY_CURSOR_LEFT_DOWN:
          p_wk->cursor  = MYSTERY_CURSOR_LEFT_UP;
          break;

        case MYSTERY_CURSOR_RIGHT_DOWN:
          p_wk->cursor  = MYSTERY_CURSOR_RIGHT_UP;
          break;
            
        case MYSTERY_CURSOR_RETURN:
          if( p_wk->key_btn_save )
          { 
            p_wk->cursor  = MYSTERY_CURSOR_LEFT_DOWN;
          }
          else
          { 
            p_wk->cursor  = MYSTERY_CURSOR_RIGHT_DOWN;
          }
          break;
        }
        is_update     = TRUE;
      }
      else if( key & PAD_KEY_DOWN )
      { 
        PMSND_PlaySE( MYSTERY_SE_SELCT );
        switch( p_wk->cursor )
        { 
        case MYSTERY_CURSOR_LEFT_UP:
          p_wk->cursor  = MYSTERY_CURSOR_LEFT_DOWN;
          break;

        case MYSTERY_CURSOR_RIGHT_UP:
          p_wk->cursor  = MYSTERY_CURSOR_RIGHT_DOWN;
          break;

        case MYSTERY_CURSOR_LEFT_DOWN:
          if( p_wk->is_swap )
          { 
            p_wk->cursor  = MYSTERY_CURSOR_LEFT_UP;
          }
          else
          { 
            p_wk->key_btn_save  = TRUE;
            p_wk->cursor  = MYSTERY_CURSOR_RETURN;
          }
          break;
        case MYSTERY_CURSOR_RIGHT_DOWN:
          if( p_wk->is_swap )
          { 
            p_wk->cursor  = MYSTERY_CURSOR_RIGHT_UP;
          }
          else
          { 
            p_wk->key_btn_save  = FALSE;
            p_wk->cursor  = MYSTERY_CURSOR_RETURN;
          }
          break;

        case MYSTERY_CURSOR_RETURN:
          if( p_wk->key_btn_save )
          { 
            p_wk->cursor  = MYSTERY_CURSOR_LEFT_UP;
          }
          else
          { 
            p_wk->cursor  = MYSTERY_CURSOR_RIGHT_UP;
          }
          break;
        }
        is_update     = TRUE;
      }
      else if( key & PAD_KEY_LEFT )
      {
        const int page_max  = Mystery_Album_GetPageMax( p_wk );

        if( (p_wk->cursor == MYSTERY_CURSOR_LEFT_UP
          || p_wk->cursor == MYSTERY_CURSOR_LEFT_DOWN) )
        { 
          scroll  = -1;
        }
        else if( p_wk->cursor != MYSTERY_CURSOR_RETURN )
        { 
          PMSND_PlaySE( MYSTERY_SE_SELCT );
        }

        if( p_wk->cursor != MYSTERY_CURSOR_RETURN )
        { 
          if( p_wk->cursor == MYSTERY_CURSOR_LEFT_UP || p_wk->cursor == MYSTERY_CURSOR_RIGHT_UP )
          { 
            p_wk->cursor  -=1;
            p_wk->cursor  = MATH_ROUND(p_wk->cursor,MYSTERY_CURSOR_LEFT_UP ,MYSTERY_CURSOR_RIGHT_UP);
          }else
          { 
            p_wk->cursor  -=1;
            p_wk->cursor  = MATH_ROUND(p_wk->cursor,MYSTERY_CURSOR_LEFT_DOWN, MYSTERY_CURSOR_RIGHT_DOWN);
          }
          is_update     = TRUE;
        }
      }
      else if( key & PAD_KEY_RIGHT )
      { 
        const int page_max  = Mystery_Album_GetPageMax( p_wk );

        if( (p_wk->cursor == MYSTERY_CURSOR_RIGHT_UP
          || p_wk->cursor == MYSTERY_CURSOR_RIGHT_DOWN) )
        { 
          scroll  = 1;
        }
        else if( p_wk->cursor != MYSTERY_CURSOR_RETURN )
        { 
          PMSND_PlaySE( MYSTERY_SE_SELCT );
        }
        if( p_wk->cursor != MYSTERY_CURSOR_RETURN )
        {
          if( p_wk->cursor == MYSTERY_CURSOR_LEFT_UP || p_wk->cursor == MYSTERY_CURSOR_RIGHT_UP )
          { 
            p_wk->cursor  +=1;
            p_wk->cursor  = MATH_ROUND(p_wk->cursor,MYSTERY_CURSOR_LEFT_UP ,MYSTERY_CURSOR_RIGHT_UP);
          }else
          { 
            p_wk->cursor  +=1;
            p_wk->cursor  = MATH_ROUND(p_wk->cursor,MYSTERY_CURSOR_LEFT_DOWN, MYSTERY_CURSOR_RIGHT_DOWN);
          }
          is_update = TRUE;
        }
      }
      else if( key & PAD_BUTTON_DECIDE )
      {  
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[0], FALSE );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[1], FALSE );

        if( p_wk->cursor == MYSTERY_CURSOR_RETURN )
        { 
          if( !p_wk->is_swap )
          { 
            PMSND_PlaySE( MYSTERY_SE_DECIDE );
            MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
          }
        }
        else
        { 
          const u32 card_index  = Mystery_Album_GetNowCardIndex( p_wk );
          const MYSTERY_CARD_DATA *cp_data  = &p_wk->data[ card_index ];
          if( MYSTERY_CARD_DATA_IsExist(cp_data) )
          { 
            PMSND_PlaySE( MYSTERY_SE_DECIDE );
            //スワップモードならば入れ替えへ
            if( p_wk->is_swap )
            { 
              MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_SwapCard );
            }
            else
            { 
              MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_SelectList );
            }
          }
        }
      }
      else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL )
      { 
        PMSND_PlaySE( MYSTERY_SE_CANCEL );
        //スワップモードならば、キャンセル
        if( p_wk->is_swap )
        { 
          if( p_wk->p_text )
          { 
            MYSTERY_TEXT_Exit( p_wk->p_text );
            p_wk->p_text  = NULL;
          }

          GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, FALSE );
          p_wk->is_swap = FALSE;
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[0], FALSE );
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[1], FALSE );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
        }
      }
#ifdef DEBUG_SAVE
      else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
      { 
#ifdef DEBUG_SET_SAVEDATA
        DOWNLOAD_GIFT_DATA data;
        MYSTERY_DEBUG_SetGiftData( &data );
        MYSTERYDATA_SetCardData( p_wk->setup.p_sv, &data );
#endif //DEBUG_SET_SAVEDATA

        p_wk->is_change = TRUE;
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
      }
#endif //DEBUG_SAVE

#ifdef DEBUG_DELETE
      else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
      { 
        int i;
        for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->setup.p_sv ); i++ )
        { 
          MYSTERYDATA_RemoveCardData(p_wk->setup.p_sv, i );
        }
        p_wk->is_change = TRUE;
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
      }
#endif //DEBUG_DELETE

      //スクロール
      if( scroll != 0 )
      { 
        const int page_max  = Mystery_Album_GetPageMax( p_wk );
        u32 next_page;
        BOOL is_left;

        if( scroll < 0 )
        { 
          if( p_wk->now_page != 0 )
          { 
            next_page = p_wk->now_page-1;
          }
          else
          { 
            next_page  = page_max-1;
          }
          is_left = TRUE;
        }
        else if( scroll > 0 )
        { 
          if( p_wk->now_page < page_max-1 )
          { 
            next_page = p_wk->now_page+1;
          }
          else
          { 
            next_page  = 0;
          }

          is_left = FALSE;
        }

        PMSND_PlaySE( MYSTERY_SE_PAGE );
        Mystery_Album_StartScrollPage( p_wk, is_left );
        Mystery_Album_DeleteDisplay( p_wk, FALSE );
        Mystery_Album_CreateDisplay( p_wk, FALSE, is_left, next_page, p_wk->heapID );

        p_wk->now_page  = next_page;

        GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, FALSE );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[0], FALSE );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_allow[1], FALSE );

        p_wk->plt_cnt = 0;

        p_wk->is_card_update  = TRUE;
        *p_seq  = SEQ_SCROLL;
      }

      //カーソルの移動
      if( is_update )
      { 
        GFL_CLACTPOS  pos;
        if( p_wk->cursor < MYSTERY_CURSOR_MAX )
        { 
          pos.x  = sc_thumbnail_data[ p_wk->cursor ].card_x*8 - 24;
          pos.y  = sc_thumbnail_data[ p_wk->cursor ].card_y*8 - 8;

          GFL_CLACT_WK_SetAnmSeq( p_wk->p_cursor, 0);
        }
        else
        { 
          pos.x = 176;
          pos.y = 152;
          GFL_CLACT_WK_SetAnmSeq( p_wk->p_cursor, 11);
        }

        GFL_CLACT_WK_SetPos( p_wk->p_cursor, &pos, CLSYS_DEFREND_MAIN );
        p_wk->plt_cnt = 0;

        p_wk->is_card_update  = TRUE;
      }
    }
    break;

  case SEQ_SCROLL:
    if( Mystery_Album_ScrollPage( p_wk ) )
    { 
      //p_wk->is_card_update  = TRUE;
      *p_seq  = SEQ_MOVE;
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト選択
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SelectList( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  {   
    SEQ_INIT,
    SEQ_SELECT_INIT,
    SEQ_SELECT_WAIT,

    SEQ_DELETE_NONE_MSG,
    SEQ_WAIT_DELETE_NONE_MSG,
    SEQ_NEXT_MOVE,
    SEQ_NEXT_SWAP,
    SEQ_NEXT_DELETE,
  };
  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text  = MYSTERY_TEXT_Init( MYSTERY_ALBUM_LIST_FRM, MYSTERY_ALBUM_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_font, HEAPID_MYSTERYGIFT );
      MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, 1, MYSTERY_ALBUM_BG_FRM_S_PLT );
    }
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_017, MYSTERY_TEXT_TYPE_QUE );

    *p_seq  = SEQ_SELECT_INIT;
    break;

  case SEQ_SELECT_INIT:
    { 
      MYSTERY_LIST_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.p_msg   = p_wk->setup.p_msg;
      setup.p_font  = p_wk->setup.p_font;
      setup.p_que   = p_wk->setup.p_que;
      setup.strID[0]= syachi_mystery_menu_008;
      setup.strID[1]= syachi_mystery_menu_009;
      setup.strID[2]= syachi_mystery_menu_010;
      setup.list_max= 3;
      setup.frm     = MYSTERY_ALBUM_LIST_FRM;
      setup.font_plt= MYSTERY_ALBUM_FONT_PLT;
      setup.frm_plt = MYSTERY_ALBUM_BG_FRM_S_PLT;
      setup.frm_chr = 1;
      p_wk->p_list  = MYSTERY_LIST_Init( &setup, p_wk->heapID ); 
      *p_seq  = SEQ_SELECT_WAIT;
    }
    break;

  case SEQ_SELECT_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      p_wk->plt_cnt = 0x7FFF;
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        if( ret == 0 )
        { 
          //いれかえ
          p_wk->is_swap = TRUE;

          { 
            GFL_CLACTPOS  pos;
            pos.x  = sc_thumbnail_data[ p_wk->cursor ].card_x*8 - 24;
            pos.y  = sc_thumbnail_data[ p_wk->cursor ].card_y*8 - 8;

            GFL_CLACT_WK_SetPos( p_wk->p_swap_cursor, &pos, CLSYS_DEFREND_MAIN );
            GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, TRUE );
          }

          p_wk->swap_card_index = Mystery_Album_GetNowCardIndex( p_wk );
          NAGI_Printf( "入れ替えるカード %d\n", p_wk->swap_card_index );
          if( p_wk->p_list )
          { 
            MYSTERY_LIST_Exit( p_wk->p_list );
            p_wk->p_list  = NULL;
          }

          if( p_wk->p_text )
          { 
            MYSTERY_TEXT_Exit( p_wk->p_text );
            p_wk->p_text  = NULL;
          }

          if( p_wk->p_text == NULL )
          { 
            p_wk->p_text  = MYSTERY_TEXT_InitOneLine( MYSTERY_ALBUM_LIST_FRM, MYSTERY_ALBUM_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_font, HEAPID_MYSTERYGIFT );
            MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, 1, MYSTERY_ALBUM_BG_FRM_S_PLT );
          }
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_020, MYSTERY_TEXT_TYPE_QUE );
          *p_seq        = SEQ_NEXT_SWAP;
        }
        else if( ret == 1 )
        { 
          //すてる
          const u32 card_index  = Mystery_Album_GetNowCardIndex( p_wk );
          const MYSTERY_CARD_DATA *cp_data  = &p_wk->data[ card_index ];
          if( MYSTERY_CARD_DATA_IsHave( cp_data ))
          { 
            //すてられる
            *p_seq  = SEQ_NEXT_DELETE;
          }
          else
          { 
            //まだもらってない
            *p_seq  = SEQ_DELETE_NONE_MSG;
          }
        }
        else if( ret == 2 )
        { 
          //もどる
          *p_seq  = SEQ_NEXT_MOVE;
        }
      }
    }
    break;
    
  case SEQ_DELETE_NONE_MSG:
    if( p_wk->p_list )
    { 
      MYSTERY_LIST_Exit( p_wk->p_list );
      p_wk->p_list  = NULL;
    }
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_018, MYSTERY_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_WAIT_DELETE_NONE_MSG;
    break;

  case SEQ_WAIT_DELETE_NONE_MSG:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      *p_seq  = SEQ_NEXT_MOVE;
    }
    break;

  case SEQ_NEXT_MOVE:
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    if( p_wk->p_list )
    { 
      MYSTERY_LIST_Exit( p_wk->p_list );
      p_wk->p_list  = NULL;
    }
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    break;

  case SEQ_NEXT_SWAP:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    break;

  case SEQ_NEXT_DELETE:
    MYSTERY_LIST_Exit( p_wk->p_list );
    p_wk->p_list  = NULL;
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DeleteCard );
    break;
  }

  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  カード消去
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_YESNO_MSG,
    SEQ_YESNO_INIT,
    SEQ_YESNO_MAIN,
    SEQ_INIT,
    SEQ_EFFECT_FADEOUT,
    SEQ_DELETE,
    SEQ_EFFECT_FADEIN,
    SEQ_EFFECT_WAIT,
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_EXIT,
  };

  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;
  const u32 card_index  = Mystery_Album_GetNowCardIndex( p_wk );

  switch( *p_seq )
  { 
  case SEQ_YESNO_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_021, MYSTERY_TEXT_TYPE_STREAM ); 

    *p_seq  = SEQ_YESNO_INIT;
    break;

  case SEQ_YESNO_INIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_LIST_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.p_msg   = p_wk->setup.p_msg;
      setup.p_font  = p_wk->setup.p_font;
      setup.p_que   = p_wk->setup.p_que;
      setup.strID[0]= syachi_mystery_01_021_yes;
      setup.strID[1]= syachi_mystery_01_021_no;
      setup.list_max= 2;
      setup.frm     = MYSTERY_ALBUM_LIST_FRM;
      setup.font_plt= MYSTERY_ALBUM_FONT_PLT;
      setup.frm_plt = MYSTERY_ALBUM_BG_FRM_S_PLT;
      setup.frm_chr = 1;
      p_wk->p_list  = MYSTERY_LIST_Init( &setup, p_wk->heapID ); 

      *p_seq  = SEQ_YESNO_MAIN;
    }
    break;

  case SEQ_YESNO_MAIN:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      p_wk->plt_cnt = 0x7FFF;
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        MYSTERY_LIST_Exit( p_wk->p_list );
        p_wk->p_list  = NULL;

        if( ret == 0 )
        { 
          //はい
          *p_seq  = SEQ_INIT;
        }
        else
        { 
          //いいえ
          if( p_wk->p_text )
          { 
            MYSTERY_TEXT_Exit( p_wk->p_text );
            p_wk->p_text  = NULL;
          }
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
        }
      }
    }
    break;

  case SEQ_INIT:
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
    { 
      const u8      src_pos = card_index % MYSTERY_CURSOR_MAX;
      GFL_RECT  src_rect;
      src_rect.left = (sc_thumbnail_data[ src_pos ].card_x)*8;
      src_rect.right = (sc_thumbnail_data[ src_pos ].card_x + MYSTERY_ALBUM_THUMBNAIL_CARD_W)*8;
      src_rect.top = (sc_thumbnail_data[ src_pos ].card_y)*8;
      src_rect.bottom = (sc_thumbnail_data[ src_pos ].card_y + MYSTERY_ALBUM_THUMBNAIL_CARD_H)*8;
      G2_SetWnd0InsidePlane( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, TRUE );
      G2_SetWnd0Position( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );
    }
    G2_SetWndOutsidePlane( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, FALSE );
    *p_seq  = SEQ_EFFECT_FADEOUT;
    break;

  case SEQ_EFFECT_FADEOUT:
    {
      int fade  = 0 + 16 * p_wk->cnt / MYSTERY_ALBUM_DELETE_SYNC;
      G2_SetBlendBrightness( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, fade );
      if( p_wk->cnt++ > MYSTERY_ALBUM_DELETE_SYNC )
      { 
        p_wk->cnt = 0;
        *p_seq  = SEQ_DELETE;
      }
    }
    break;

  case SEQ_DELETE:
    //削除
    Mystery_Album_RemoveCard( p_wk, card_index );

    //画面作り直し
    Mystery_Album_DeleteDisplay( p_wk, TRUE );
    Mystery_Album_CreateDisplay( p_wk, TRUE, 0, p_wk->now_page, p_wk->heapID );

    p_wk->is_card_update  = TRUE;
    *p_seq  = SEQ_EFFECT_FADEIN;
    break;

  case SEQ_EFFECT_FADEIN:
    {
      int fade  = 16 - 16 * p_wk->cnt / MYSTERY_ALBUM_DELETE_SYNC;
      G2_SetBlendBrightness( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, fade );
      if( p_wk->cnt++ > MYSTERY_ALBUM_SWAP_SYNC )
      { 
        p_wk->cnt = 0;
        *p_seq  = SEQ_EFFECT_WAIT;
      }
    }
    break;

  case SEQ_EFFECT_WAIT:
    //一つもなくなったらA:整理するを消す
    if( MYSTERYDATA_IsExistsCardAll(p_wk->setup.p_sv) == FALSE )
    { 
      MYSTERY_MSGWINSET_ClearOne( p_wk->p_winset, 1 );
    }

    GX_SetVisibleWnd( GX_WNDMASK_NONE );
    *p_seq  = SEQ_MSG_INIT;
    break;

  case SEQ_MSG_INIT:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_019, MYSTERY_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_MSG_WAIT:
#ifdef BUGFIX_BTS7724_20100712
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
#else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
#endif //BUGFIX_BTS7724_20100712
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    break;
  }


  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }  
  
}
//----------------------------------------------------------------------------
/**
 *	@brief  カード入れ替え
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SwapCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_EFFECT_INIT,
    SEQ_EFFECT_FADEOUT,
    SEQ_DELETE,
    SEQ_EFFECT_FADEIN,
    SEQ_EFFECT_WAIT,
    SEQ_EXIT,
  };
  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;
  const u32 card_index  = Mystery_Album_GetNowCardIndex( p_wk );

  switch( *p_seq )
  { 
  case SEQ_INIT:
    //スワップ元と先が同じならば終了
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    p_wk->is_swap = FALSE; 
    if( card_index == p_wk->swap_card_index )
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, FALSE );
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    }
    else
    { 
      *p_seq  = SEQ_EFFECT_INIT;
    }
    break;

  case SEQ_EFFECT_INIT:
    { 
      const u8      src_pos = card_index % MYSTERY_CURSOR_MAX;
      const u8      dst_pos = p_wk->swap_card_index % MYSTERY_CURSOR_MAX;
      const BOOL    is_wnd0 = Mystery_Album_GetIndexToPage( card_index ) == p_wk->now_page;
      const BOOL    is_wnd1 = Mystery_Album_GetIndexToPage( p_wk->swap_card_index ) == p_wk->now_page;

      NAGI_Printf( "入れ替え %d %d \n", card_index, p_wk->swap_card_index );

      {
        int wnd = 0;
        if( is_wnd0 )
        { 
          wnd |= GX_WNDMASK_W0;
        }
        if( is_wnd1 )
        { 
          wnd |= GX_WNDMASK_W1;
        }
        GX_SetVisibleWnd( wnd );
      }

      if( is_wnd0 )
      { 
        GFL_RECT  src_rect;
        src_rect.left = (sc_thumbnail_data[ src_pos ].card_x)*8;
        src_rect.right = (sc_thumbnail_data[ src_pos ].card_x + MYSTERY_ALBUM_THUMBNAIL_CARD_W)*8;
        src_rect.top = (sc_thumbnail_data[ src_pos ].card_y)*8;
        src_rect.bottom = (sc_thumbnail_data[ src_pos ].card_y + MYSTERY_ALBUM_THUMBNAIL_CARD_H)*8;
        G2_SetWnd0InsidePlane( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, TRUE );
        G2_SetWnd0Position( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );
      }
      if( is_wnd1 )
      { 
        GFL_RECT  dst_rect;
        dst_rect.left = (sc_thumbnail_data[ dst_pos ].card_x)*8;
        dst_rect.right = (sc_thumbnail_data[ dst_pos ].card_x + MYSTERY_ALBUM_THUMBNAIL_CARD_W)*8;
        dst_rect.top = (sc_thumbnail_data[ dst_pos ].card_y)*8;
        dst_rect.bottom = (sc_thumbnail_data[ dst_pos ].card_y + MYSTERY_ALBUM_THUMBNAIL_CARD_H)*8;
        G2_SetWnd1InsidePlane( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, TRUE );
        G2_SetWnd1Position( dst_rect.left, dst_rect.top, dst_rect.right, dst_rect.bottom );
      }
      G2_SetWndOutsidePlane( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, FALSE );
    }
    *p_seq  = SEQ_EFFECT_FADEOUT;
    break;

  case SEQ_EFFECT_FADEOUT:
    {
      int fade  = 0 + 16 * p_wk->cnt / MYSTERY_ALBUM_SWAP_SYNC;
      G2_SetBlendBrightness( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, fade );
      if( p_wk->cnt++ > MYSTERY_ALBUM_SWAP_SYNC )
      { 
        p_wk->cnt = 0;
        *p_seq  = SEQ_DELETE;
      }
    }
    break;

  case SEQ_DELETE:
    //スワップ
    Mystery_Album_SwapCard( p_wk, card_index, p_wk->swap_card_index );
    NAGI_Printf( "入れ替えます 今の%d 前の%d\n", card_index, p_wk->swap_card_index );

    PMSND_PlaySE( MYSTERY_SE_SWAP_CARD );

    //画面作り直し
#if 0
    Mystery_Album_DeleteDisplay( p_wk, TRUE );
    Mystery_Album_CreateDisplay( p_wk, TRUE, 0, p_wk->now_page, p_wk->heapID );
#else
    { 
      const u32 swap_page   = Mystery_Album_GetIndexToPage( p_wk->swap_card_index );
      const u32 swap_index  = p_wk->swap_card_index % MYSTERY_CURSOR_MAX;

      const BOOL  now_is_front = Mystery_Album_GetIndexToPage( card_index ) == p_wk->now_page;
      const BOOL  swap_is_front = Mystery_Album_GetIndexToPage( p_wk->swap_card_index ) == p_wk->now_page;

      //全面だったら消す
      if( now_is_front )
      { 
        Mystery_Album_DeleteThumbnail( p_wk, TRUE, p_wk->cursor );
        Mystery_Album_CreateThumbnail( p_wk, TRUE, 0, p_wk->now_page, p_wk->cursor, p_wk->heapID );
      }
      if( swap_is_front )
      { 
        Mystery_Album_DeleteThumbnail( p_wk, TRUE, swap_index );
        Mystery_Album_CreateThumbnail( p_wk, TRUE, 0, swap_page, swap_index, p_wk->heapID );
      }
      GFL_BG_LoadScreenReq( MYSTERY_ALBUM_BACK_FRM );
    }
#endif 

    p_wk->is_card_update  = TRUE;
    *p_seq  = SEQ_EFFECT_FADEIN;
    break;

  case SEQ_EFFECT_FADEIN:
    {
      int fade  = 16 - 16 * p_wk->cnt / MYSTERY_ALBUM_SWAP_SYNC;
      G2_SetBlendBrightness( MYSTERY_ALBUM_SWAP_BRIGHTNESS_MASK, fade );
      if( p_wk->cnt++ > MYSTERY_ALBUM_SWAP_SYNC )
      { 
        p_wk->cnt = 0;
        *p_seq  = SEQ_EFFECT_WAIT;
      }
    }
    break;

  case SEQ_EFFECT_WAIT:
    GX_SetVisibleWnd( GX_WNDMASK_NONE );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    p_wk->is_swap = FALSE;
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_swap_cursor, FALSE );
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  終了処理orセーブ
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_CHECK_MODE,

    SEQ_DELETE_MSG_INIT,
    SEQ_DELETE_YESNO_INIT,
    SEQ_DELETE_YESNO_MAIN,

    SEQ_CHECK_CHANGE,
    SEQ_SAVE_MSG,
    SEQ_SAVE_MSG_WAIT,
    SEQ_SAVE_INIT,
    SEQ_SAVE_MAIN,
    SEQ_EXIT,
  };

  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    *p_seq  = SEQ_CHECK_MODE;
    break;

  case SEQ_CHECK_MODE:
    if( p_wk->setup.mode == MYSTERY_ALBUM_MODE_VIEW )
    { 
      *p_seq  = SEQ_CHECK_CHANGE;
    }
    else
    { 
      if( MYSTERYDATA_CheckCardDataSpace( p_wk->setup.p_sv ) )
      { 
        *p_seq  = SEQ_CHECK_CHANGE;
      }
      else
      { 
        *p_seq  = SEQ_DELETE_MSG_INIT;
      }
    }
    break;


  case SEQ_DELETE_MSG_INIT:
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text  = MYSTERY_TEXT_Init( MYSTERY_ALBUM_LIST_FRM, MYSTERY_ALBUM_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_font, HEAPID_MYSTERYGIFT );
      MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, 1, MYSTERY_ALBUM_BG_FRM_S_PLT ); 
    }
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_006, MYSTERY_TEXT_TYPE_STREAM ); 
    *p_seq  = SEQ_DELETE_YESNO_INIT;
    break;

  case SEQ_DELETE_YESNO_INIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_LIST_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.p_msg   = p_wk->setup.p_msg;
      setup.p_font  = p_wk->setup.p_font;
      setup.p_que   = p_wk->setup.p_que;
      setup.strID[0]= syachi_mystery_01_021_yes;
      setup.strID[1]= syachi_mystery_01_021_no;
      setup.list_max= 2;
      setup.frm     = MYSTERY_ALBUM_LIST_FRM;
      setup.font_plt= MYSTERY_ALBUM_FONT_PLT;
      setup.frm_plt = MYSTERY_ALBUM_BG_FRM_S_PLT;
      setup.frm_chr = 1;
      p_wk->p_list  = MYSTERY_LIST_Init( &setup, p_wk->heapID ); 

      *p_seq  = SEQ_DELETE_YESNO_MAIN;
    }
    break;

  case SEQ_DELETE_YESNO_MAIN:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      p_wk->plt_cnt = 0x7FFF;
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        MYSTERY_LIST_Exit( p_wk->p_list );
        p_wk->p_list  = NULL;

        if( ret == 0 )
        { 
          //はい
          *p_seq  = SEQ_CHECK_CHANGE;
        }
        else
        { 
          //いいえ
          if( p_wk->p_text )
          { 
            MYSTERY_TEXT_Exit( p_wk->p_text );
            p_wk->p_text  = NULL;
          }
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
        }
      }
    }
    break;

  case SEQ_CHECK_CHANGE:
    if( p_wk->is_change )
    { 
      *p_seq  = SEQ_SAVE_MSG;
    }
    else
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_SAVE_MSG:
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text  = MYSTERY_TEXT_Init( MYSTERY_ALBUM_LIST_FRM, MYSTERY_ALBUM_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_font, HEAPID_MYSTERYGIFT );
      MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, 1, MYSTERY_ALBUM_BG_FRM_S_PLT ); 
    }
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_016, MYSTERY_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_SAVE_MSG_WAIT;
    break;

  case SEQ_SAVE_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      *p_seq  = SEQ_SAVE_INIT;
    }
    break;

  case SEQ_SAVE_INIT:
    { 
      BOOL is_ok  = TRUE;

      if( p_wk->p_anm_card )
      { 
        if( !MYSTERY_CARD_IsEndEffect(p_wk->p_anm_card) )
        { 
          is_ok = FALSE;
        }
      }

      if( is_ok )
      { 
        MYSTERYDATA_SaveAsyncStart( p_wk->setup.p_sv,p_wk->setup.p_gamedata );
        *p_seq  = SEQ_SAVE_MAIN;
      }
    }
    break;

  case SEQ_SAVE_MAIN:
    { 
      SAVE_RESULT result  = MYSTERYDATA_SaveAsyncMain(p_wk->setup.p_sv,p_wk->setup.p_gamedata);
      if( result == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:

    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }

    MYSTERY_SEQ_End( p_seqwk );
    break;
  }

  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }  
}
//----------------------------------------------------------------------------
/**
 *	@brief  メッセージ消去
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk   シーケンス管理
 *	@param	*p_seq              シーケンス
 *	@param	*p_wk_adrs          ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DeleteMsg( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  MYSTERY_ALBUM_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text  = MYSTERY_TEXT_Init( MYSTERY_ALBUM_LIST_FRM, MYSTERY_ALBUM_FONT_PLT, p_wk->setup.p_que, p_wk->setup.p_font, HEAPID_MYSTERYGIFT );
    }
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, 1, MYSTERY_ALBUM_BG_FRM_S_PLT );
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->setup.p_msg, syachi_mystery_01_005, MYSTERY_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
      {
        *p_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }

    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_MoveCursor );
    break;
  }

  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }  
}

//----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメ読み込み
 *
 *	@param	PLTANM_WORK *p_wk ワーク
 *	@param	*p_handle         読み込むハンドル
 *	@param	datID             読み込むパレット
 *	@param	dst_plt_num       変更前色のパレット行
 *	@param	src_plt_num       変更後色のパレット行
 *	@param	start_col         開始列
 *	@param	end_col           終了列
 *	@param	heapID            ヒープID
 */
//-----------------------------------------------------------------------------
static void PLTANM_Load( PLTANM_WORK *p_wk, ARCHANDLE *p_handle, ARCDATID datID, u8 dst_plt_num, u8 src_plt_num, HEAPID heapID )
{ 
  void *p_buff;
  NNSG2dPaletteData *p_plt;
  const u16 *cp_plt_adrs;

  GFL_STD_MemClear( p_wk, sizeof(PLTANM_WORK) );

  //もとのパレットから色情報を保存
  p_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, datID, &p_plt, heapID );
  cp_plt_adrs = p_plt->pRawData;
  GFL_STD_MemCopy( (u8*)cp_plt_adrs + dst_plt_num * 0x20, p_wk->plt_dst, sizeof(u16) * 0x10 );
  GFL_STD_MemCopy( (u8*)cp_plt_adrs + src_plt_num * 0x20, p_wk->plt_src, sizeof(u16) * 0x10 );

  //パレット破棄
  GFL_HEAP_FreeMemory( p_buff );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメ転送メイン
 *
 *	@param	PLTANM_WORK *p_wk ワーク
 *	@param	type              転送先
 *	@param	target_plt        どこのパレット行に転送するか
 *	@param  cnt               カウンタ
 */
//-----------------------------------------------------------------------------
static void PLTANM_Main( PLTANM_WORK *p_wk, NNS_GFD_DST_TYPE type, u8 target_plt, u16 cnt )
{ 
  MYSTERY_UTIL_MainPltAnmLine( type, p_wk->plt, cnt, target_plt, p_wk->plt_src, p_wk->plt_dst );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  カード
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *          定数
 */
//=============================================================================
#define MYSTERY_CARD_SILHOUETTE_EV1 (2)
#define MYSTERY_CARD_SILHOUETTE_EV2 (12)

#define MYSTERY_CARD_EFFECT_SYNC    (60)

#define MYSTERY_CARD_EFFECT_START_SEQ    (1)

#define MYSTERY_CARD_SILHOUETTE_POS_X (184)
#define MYSTERY_CARD_SILHOUETTE_POS_Y (112)

//=============================================================================
/**
 *          構造体
 */
//=============================================================================
//-------------------------------------
///	ポケモンが揺れ動く
//=====================================
typedef struct 
{ 
  u16 now_angle;
  u16 init_angle;  //初期の回転角度
  u16 next_angle; //次の回転角度
  s8  next_dir;   //次の回転方向
  u8  seq;        //シーケンス
  u16 cnt;        //回数
  u16 max_cnt;    //最大
  u32 sync;       //シンク
  u32 next_sync;   //次動作するシンク
  GFL_POINT init_pos; //初期座標
  GFL_POINT next_pos; //次の座標
  GFL_POINT now_pos; //現在座標
} MOVE_SHAKE_WORK;

//-------------------------------------
///	カードリソースワーク
//=====================================
struct _MYSTERY_CARD_RES
{
  MYSTERY_MSGWINSET_WORK  *p_winset;

  u32 res_icon_plt;
  u32 res_icon_cgx;
  u32 res_icon_cel;
  u32 res_silhouette_plt;
  u32 res_silhouette_cgx;
  u32 res_silhouette_cel;
  GFL_CLWK  *p_icon;
  GFL_CLWK  *p_silhouette;

  //シルエット用ポケモンパレット
  u16 plt[0x10];
  u16 plt_silhouette[0x10];
  u16 plt_pokemon[0x10];
  BOOL is_main;
  u32 plt_num;
  u32 silhouette_bg_pri;
  u32 font_bg_pri;
  u16 back_frm;
  u16 font_frm;


  CLSYS_DRAW_TYPE draw_type;
  PALTYPE         paltype;

  MYSTERY_CARD_SETUP  setup;
} ;


//-------------------------------------
///	カードワーク
//=====================================
struct _MYSTERY_CARD_WORK
{ 
  MYSTERY_CARD_DATA       data;

  u16 seq;
  u16 cnt;

  BOOL is_egg;
  u16 mons_no;
  u16 form_no;
  u32 voice_player;

  MOVE_SHAKE_WORK move_shake;
  
  HEAPID  heapID;
  MYSTERY_CARD_RES  *p_res;
  const GIFT_PACK_DATA *cp_data;
  GAMEDATA *p_gamedata;
  GFL_TCB *p_trans_task;
  BOOL    is_task_end;
  MYSTERY_CARD_TRANS_TYPE trans_type;

  //シルエットメインメモリ読み込み用
  void *p_chr_buff;
  NNSG2dCharacterData *p_chr_data;
  void *p_plt_buff;
  NNSG2dPaletteData *p_plt_data;

  MYSTERY_MSGWINBUFF_WORK  *p_winbuff;
};

//=============================================================================
/**
 *          プロトタイプ
 */
//=============================================================================
static void MYSTERY_CARD_LoadResourceBG( const MYSTERY_CARD_SETUP *cp_setup, const MYSTERY_CARD_DATA *cp_data, HEAPID heapID );
static void MYSTERY_CARD_ChangePalettleBG( const MYSTERY_CARD_SETUP *cp_setup, const MYSTERY_CARD_DATA *cp_data, HEAPID heapID );
static void MYSTERY_CARD_LoadResourceOBJ( MYSTERY_CARD_WORK *p_wk, MYSTERY_CARD_RES *p_res, HEAPID heapID );
//シルエット動作関数
static void MOVE_SHAKE_Init( MOVE_SHAKE_WORK *p_wk, u16 init_angle, s16 x, s16 y );
static BOOL MOVE_SHAKE_Main( MOVE_SHAKE_WORK *p_wk, u16 *p_angle, GFL_POINT *p_pos );
//=============================================================================
/**
 *          リソース
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  リソース作成
 *
 *	@param	const MYSTERY_CARD_SETUP *cp_setup  設定
 *	@param	heapID  ヒープID
 *
 *	@return リソースワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_CARD_RES * MYSTERY_CARD_RES_Init( const MYSTERY_CARD_SETUP *cp_setup, HEAPID heapID )
{ 
  MYSTERY_CARD_RES *p_wk;
  p_wk = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_CARD_RES) );
  p_wk->plt_num   = cp_setup->silhouette_obj_plt_num;
  p_wk->back_frm  = cp_setup->back_frm;
  p_wk->font_frm  = cp_setup->font_frm;
  p_wk->setup     = *cp_setup;

  //描画先をチェック
  if( cp_setup->back_frm < GFL_BG_FRAME0_S )
  { 
    p_wk->paltype   = PALTYPE_MAIN_BG;
    p_wk->draw_type = CLSYS_DRAW_MAIN;
    p_wk->is_main = TRUE;
  }
  else
  { 
    p_wk->paltype   = PALTYPE_SUB_BG;
    p_wk->draw_type = CLSYS_DRAW_SUB;
    p_wk->is_main = FALSE;
  }

  //BMPWIN作成
  {
    MYSTERY_MSGWINSET_SETUP_TBL tbl[] =
    { 
      //不思議カード
      { 
        6,
        2,
        10,
        2,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      },
      //イベント名
      { 
        2,
        6,
        28,
        2,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_DEFAULT_COLOR
      },
      //本文
      { 
        2,
        9,
        28,
        10,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_DEFAULT_COLOR
      },
      //もらったひづけ
      { 
        4,
        20,
        14,
        2,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      },
      //日付
      { 
        18,
        20,
        11,
        2,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_W_BACK,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      }
    };

    p_wk->p_winset  = MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE_MANUAL, tbl, NELEMS(tbl), p_wk->setup.font_frm, p_wk->setup.font_plt_num, p_wk->setup.p_que, p_wk->setup.p_msg, p_wk->setup.p_font, heapID );

  }

  //アイコン作成
  { 
    //リソース読み込み
    { 
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

      //あとでCGXだけ読み替えるため、最初はプレゼントボックスを作っておく
      p_wk->res_icon_plt	= GFL_CLGRP_PLTT_Register( p_handle, 
            NARC_mystery_fushigi_box_NCLR, p_wk->draw_type, cp_setup->icon_obj_plt_num*0x20,heapID );

      p_wk->res_icon_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_mystery_fushigi_box_NCER, NARC_mystery_fushigi_box_NANR, heapID );
      p_wk->res_icon_cgx	= GFL_CLGRP_CGR_Register( p_handle,
          NARC_mystery_fushigi_box_NCGR, FALSE, p_wk->draw_type, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    //CLWK作成
    { 
      GFL_CLWK_DATA cldata;
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x  = 216;
      cldata.pos_y  = 22;

      p_wk->p_icon    = GFL_CLACT_WK_Create( cp_setup->p_clunit, 
          p_wk->res_icon_cgx, p_wk->res_icon_plt, p_wk->res_icon_cel,
          &cldata, p_wk->draw_type, heapID );
      GFL_CLACT_WK_SetObjMode( p_wk->p_icon, GX_OAM_MODE_NORMAL );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon, FALSE );
    }
  }

  //シルエット作成
  { 
    { 
      ARCHANDLE *p_handle = POKE2DGRA_OpenHandle( heapID );

      //あとでCGXだけ読み替えるためなんの絵でもよい
      p_wk->res_silhouette_plt  = POKE2DGRA_OBJ_PLTT_Register( p_handle, 1, 0, 0, 0, POKEGRA_DIR_FRONT, 0, p_wk->draw_type, cp_setup->silhouette_obj_plt_num*0x20, heapID );
      p_wk->res_silhouette_cel	= POKE2DGRA_OBJ_CELLANM_Register( 1, 0, 0, 0, POKEGRA_DIR_FRONT, 0, APP_COMMON_MAPPING_128K, p_wk->draw_type, heapID );
      p_wk->res_silhouette_cgx	= POKE2DGRA_OBJ_CGR_Register( p_handle, 1, 0, 0, 0, POKEGRA_DIR_FRONT, 0, POKEGRA_DIR_FRONT, p_wk->draw_type, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    { 
      GFL_CLWK_DATA cldata;
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x  = MYSTERY_CARD_SILHOUETTE_POS_X;
      cldata.pos_y  = MYSTERY_CARD_SILHOUETTE_POS_Y;
      cldata.bgpri  = cp_setup->back_frm % GFL_BG_FRAME0_S;
      p_wk->silhouette_bg_pri = cldata.bgpri;
      p_wk->font_bg_pri = cp_setup->font_frm % GFL_BG_FRAME0_S;
      p_wk->p_silhouette   = GFL_CLACT_WK_Create( cp_setup->p_clunit, 
          p_wk->res_silhouette_cgx, p_wk->res_silhouette_plt, p_wk->res_silhouette_cel,
          &cldata, p_wk->draw_type, heapID );
      GFL_CLACT_WK_SetAffineParam( p_wk->p_silhouette, CLSYS_AFFINETYPE_DOUBLE );
      GFL_CLACT_WK_SetObjMode( p_wk->p_silhouette, GX_OAM_MODE_XLU );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_silhouette, FALSE );
    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  リソース破棄
 *
 *	@param	MYSTERY_CARD_RES *p_res ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_RES_Exit( MYSTERY_CARD_RES *p_wk )
{ 
  //シルエット破棄
  { 
    GFL_CLACT_WK_Remove( p_wk->p_silhouette );
    GFL_CLGRP_CGR_Release( p_wk->res_silhouette_cgx );
    GFL_CLGRP_CELLANIM_Release( p_wk->res_silhouette_cel );
    GFL_CLGRP_PLTT_Release( p_wk->res_silhouette_plt );
  }

  //アイコン破棄
  { 
    GFL_CLACT_WK_Remove( p_wk->p_icon );
    GFL_CLGRP_CGR_Release( p_wk->res_icon_cgx );
    GFL_CLGRP_CELLANIM_Release( p_wk->res_icon_cel );
    GFL_CLGRP_PLTT_Release( p_wk->res_icon_plt ); 
  }

  MYSTERY_MSGWINSET_Exit( p_wk->p_winset );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  文字描画
 *
 *	@param	MYSTERY_CARD_RES *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_RES_PrintMain( MYSTERY_CARD_RES *p_wk )
{ 
  MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset );
}
//=============================================================================
/**
 *          パブリック
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  カード  作成
 *
 *	@param	const GIFT_PACK_DATA *cp_data カードに表示するデータ
 *	@param	heapID                        ヒープID 
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_CARD_WORK * MYSTERY_CARD_Init( const GIFT_PACK_DATA *cp_data, MYSTERY_CARD_RES *p_res, GAMEDATA *p_gamedata, HEAPID heapID )
{ 

  MYSTERY_CARD_WORK * p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_CARD_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_CARD_WORK) );
  p_wk->heapID    = heapID;
  p_wk->cp_data   = cp_data;
  p_wk->p_res     = p_res;
  p_wk->p_gamedata= p_gamedata;

  //リソース取得のためにデータ作成
  { 
    MYSTERY_ALBUM_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
    setup.mode  = MYSTERY_ALBUM_MODE_VIEW;
    setup.p_clunit  = p_res->setup.p_clunit;
    setup.p_sv      = p_res->setup.p_sv;
    setup.p_font    = p_res->setup.p_font;
    setup.p_que     = p_res->setup.p_que;
    setup.p_word    = p_res->setup.p_word;
    setup.p_msg     = p_res->setup.p_msg;
    setup.p_gamedata  = p_gamedata;
 
    MYSTERY_CARD_DATA_Init( &p_wk->data, cp_data, &setup, heapID );
  }

//文章作成
  {
    MYSTERY_MSGWINSET_PRINT_TBL tbl[] =
    { 
      //不思議カード
      { 
        TRUE,
        syachi_mystery_album_006,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      },
      //イベント名
      { 
        TRUE,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_DEFAULT_COLOR
      },
      //本文
      { 
        TRUE,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_DEFAULT_COLOR
      },
      //もらったひづけ
      { 
        TRUE,
        syachi_mystery_album_007,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      },
      //日付
      { 
        TRUE,
        0,
        NULL,
        MYSTERY_MSGWIN_POS_ABSOLUTE,
        0, 0,
        MYSTERY_MSGWIN_WHITE_COLOR
      }
    };
    STRBUF  *p_wordbuf;
    s32 y,m,d;

    tbl[1].p_strbuf = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TITLE_MAX+1, GFL_HEAP_LOWID(heapID) );
    GFL_STR_SetStringCodeOrderLength( tbl[1].p_strbuf, p_wk->cp_data->event_name, GIFT_DATA_CARD_TITLE_MAX );

    { 
      u16 strID;
      if( p_wk->cp_data->have )
      { 
        strID = syachi_mystery_card_txt_00_02 + p_wk->cp_data->card_message;
      }
      else
      { 
        strID = syachi_mystery_card_txt_00_01 + p_wk->cp_data->card_message;
      }
        tbl[2].p_strbuf = GFL_MSG_CreateString( p_res->setup.p_msg, strID );
    }
    tbl[4].p_strbuf = GFL_MSG_CreateString( p_res->setup.p_msg, syachi_mystery_album_008 );

    y = MYSTERYDATA_GetYear( p_wk->cp_data->recv_date );
    m = MYSTERYDATA_GetMonth( p_wk->cp_data->recv_date );
    d = MYSTERYDATA_GetDay( p_wk->cp_data->recv_date );
    p_wordbuf = GFL_MSG_CreateString( p_res->setup.p_msg, syachi_mystery_album_008 );
    WORDSET_RegisterNumber( p_res->setup.p_word, 0, y, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( p_res->setup.p_word, 1, m, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( p_res->setup.p_word, 2, d, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( p_res->setup.p_word, tbl[4].p_strbuf, p_wordbuf );

    p_wk->p_winbuff  =  MYSTERY_MSGWINSET_CreateBuff( p_res->p_winset, heapID );
    MYSTERY_MSGWINSET_PrintBuff( p_wk->p_winbuff, tbl, p_res->setup.p_que );

    GFL_STR_DeleteBuffer( tbl[1].p_strbuf );
    GFL_STR_DeleteBuffer( tbl[2].p_strbuf );
    GFL_STR_DeleteBuffer( tbl[4].p_strbuf );
    GFL_STR_DeleteBuffer( p_wordbuf );
  }

#ifdef MEMORY_LOAD_SILHOUETTE
  //シルエットがある場合事前にメモリ転送
  //シルエット作成
  if( MYSTERY_CARD_DATA_GetType(&p_wk->data) == MYSTERYGIFT_TYPE_POKEMON )
  { 
    { 
      const GIFT_PRESENT_POKEMON  *cp_pokemon = &p_wk->cp_data->data.pokemon;
      POKEMON_PARAM* p_pp = Mystery_CreatePokemon( p_wk->cp_data, GFL_HEAP_LOWID(heapID), p_wk->p_gamedata );
      ARCHANDLE *p_handle = POKE2DGRA_OpenHandle( GFL_HEAP_LOWID(heapID) );


      p_wk->mons_no = cp_pokemon->mons_no;
      p_wk->form_no = cp_pokemon->form_no;
      p_wk->is_egg  = cp_pokemon->egg;

      //パレット転送
      { 
	      u16 plt	= POKEGRA_GetPalArcIndex( 
	          POKEGRA_GetArcID(), 
            PP_Get( p_pp, ID_PARA_monsno, NULL ),
            PP_Get( p_pp, ID_PARA_form_no, NULL ), 
            PP_Get( p_pp, ID_PARA_sex, NULL ),
            PP_CheckRare( p_pp ), 
            POKEGRA_DIR_FRONT,
            PP_Get( p_pp, ID_PARA_tamago_flag, NULL ) 
            );
        p_wk->p_plt_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, plt, &p_wk->p_plt_data, p_wk->heapID );
      }

      //キャラ転送
      { 
        p_wk->p_chr_buff  = POKE2DGRA_LoadCharacterPPP( &p_wk->p_chr_data, PP_GetPPPPointerConst(p_pp), POKEGRA_DIR_FRONT, p_wk->heapID );
      }

      GFL_HEAP_FreeMemory( p_pp );
      GFL_ARC_CloseDataHandle( p_handle );
    }
  }
#endif //MEMORY_LOAD_SILHOUETTE

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  カード  破棄
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_Exit( MYSTERY_CARD_WORK *p_wk )
{ 
  if( p_wk->p_trans_task )
  { 
    GFL_TCB_DeleteTask( p_wk->p_trans_task );
    p_wk->p_trans_task  = NULL;
  }
#ifdef MEMORY_LOAD_SILHOUETTE
  if( p_wk->p_plt_buff )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_plt_buff );
  }
  if( p_wk->p_chr_buff )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_chr_buff );
  }
#endif //MEMORY_LOAD_SILHOUETTE

  MYSTERY_MSGWINSET_DeleteBuff( p_wk->p_winbuff );
  MYSTERY_CARD_DATA_Exit( &p_wk->data );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  カード  メイン処理
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_Main( MYSTERY_CARD_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT = MYSTERY_CARD_EFFECT_START_SEQ,
    SEQ_START_FADEIN,
    SEQ_WAIT_FADEIN,
    SEQ_START_VOICE,
    SEQ_WAIT_VOICE,
    SEQ_START_FADEOUT,
    SEQ_WAIT_FADEOUT,
    SEQ_EXIT,
  };

  MYSTERY_CARD_RES  *p_res  = p_wk->p_res;

  { 
    NNS_GFD_DST_TYPE  gfd_type;
    void (*ChangeBlendAlpha)( int, int );
    if( p_res->is_main )
    { 
      gfd_type  = NNS_GFD_DST_2D_OBJ_PLTT_MAIN;
      ChangeBlendAlpha  = G2_ChangeBlendAlpha;
    }
    else
    { 
      gfd_type  = NNS_GFD_DST_2D_OBJ_PLTT_SUB;
      ChangeBlendAlpha  = G2S_ChangeBlendAlpha;
    }
	
	  switch( p_wk->seq )
	  { 
	  case SEQ_INIT:
	    GFL_CLACT_WK_SetBgPri( p_res->p_silhouette, p_res->font_bg_pri );
	    p_wk->seq = SEQ_START_FADEIN;
	    break;
	  case SEQ_START_FADEIN:
	    //パレットフェード
	    { 
	      int i;
	      u16 fade = 0 + 0xFFFF/2 * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
#if 0
	      for( i = 0; i < 0x10; i++ )
	      { 
	        MYSTERY_UTIL_MainPltAnm( gfd_type, &p_wk->plt[i], fade, p_wk->plt_num, i, p_wk->plt_pokemon[i], p_wk->plt_silhouette[i] );
	      }
#else
        MYSTERY_UTIL_MainPltAnmLine( gfd_type, p_res->plt, fade, p_res->plt_num, p_res->plt_pokemon, p_res->plt_silhouette );
#endif
	
	    }
	    //アルファフェード
	    { 
	      int ev1, ev2;
	      ev1 = MYSTERY_CARD_SILHOUETTE_EV1 + (16-MYSTERY_CARD_SILHOUETTE_EV1) * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
	      ev2 = MYSTERY_CARD_SILHOUETTE_EV2 + (0-MYSTERY_CARD_SILHOUETTE_EV2) * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
	      ChangeBlendAlpha( ev1, ev2 );
	    }
	    //カウンタ
	    if( p_wk->cnt++ > MYSTERY_CARD_EFFECT_SYNC )
	    { 
	      p_wk->cnt = 0;
	      p_wk->seq = SEQ_WAIT_FADEIN;
	    }
	    break;
	  case SEQ_WAIT_FADEIN:
	    p_wk->seq = SEQ_START_VOICE;
	    break;
	  case SEQ_START_VOICE:
      if( p_wk->is_egg == FALSE )
      { 
        p_wk->voice_player  = PMV_PlayVoice( p_wk->mons_no, p_wk->form_no );
      }
      { 
        GFL_CLACTPOS  pos;
        GFL_CLACT_WK_GetPos( p_res->p_silhouette, &pos, p_res->draw_type );
        MOVE_SHAKE_Init( &p_wk->move_shake, GFL_CLACT_WK_GetRotation( p_res->p_silhouette ), pos.x, pos.y );
      }
	    p_wk->seq = SEQ_WAIT_VOICE;
	    break;
	  case SEQ_WAIT_VOICE:
      {
        BOOL ret = TRUE;
        u16 angle;
        GFL_POINT     pos;
        GFL_CLACTPOS  clpos;

        ret &= MOVE_SHAKE_Main( &p_wk->move_shake, &angle, &pos );
//        GFL_CLACT_WK_SetRotation( p_res->p_silhouette, angle );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_res->p_silhouette, &clpos, p_res->draw_type );

        if( p_wk->is_egg == FALSE )
        { 
          ret &= !PMVOICE_CheckPlay( p_wk->voice_player );
        }

        if( ret )
        { 
          p_wk->seq = SEQ_START_FADEOUT;
        }
      }
	    break;
	  case SEQ_START_FADEOUT:
	    //パレットフェード
	    { 
	      int i;
	      u16 fade = 0xFFFF/2 - 0xFFFF/2 * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
#if 0
	      for( i = 0; i < 0x10; i++ )
	      { 
	        MYSTERY_UTIL_MainPltAnm( gfd_type, &p_wk->plt[i], fade, p_wk->plt_num, i, p_wk->plt_pokemon[i], p_wk->plt_silhouette[i] );
	      }
#else
        MYSTERY_UTIL_MainPltAnmLine( gfd_type, p_res->plt, fade, p_res->plt_num, p_res->plt_pokemon, p_res->plt_silhouette );
#endif
	    }
	    //アルファフェード
	    { 
	      int ev1, ev2;
	      ev1 = 16 + (MYSTERY_CARD_SILHOUETTE_EV1-16) * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
	      ev2 = 0 + (MYSTERY_CARD_SILHOUETTE_EV2-0) * p_wk->cnt / MYSTERY_CARD_EFFECT_SYNC;
	      ChangeBlendAlpha( ev1, ev2 );
	    }
	    //カウンタ
	    if( p_wk->cnt++ > MYSTERY_CARD_EFFECT_SYNC )
	    { 
	      p_wk->cnt = 0;
	      p_wk->seq = SEQ_WAIT_FADEOUT;
	    }
	    break;
	  case SEQ_WAIT_FADEOUT:
	    p_wk->seq = SEQ_EXIT;
	    break;
	  case SEQ_EXIT:
	    GFL_CLACT_WK_SetBgPri( p_res->p_silhouette, p_res->silhouette_bg_pri );
	    p_wk->seq = 0;
	    break;
	  }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  リソース読み込み
 *
 *	@param	const MYSTERY_CARD_SETUP *cp_setup 設定
 */
//-----------------------------------------------------------------------------
static void MYSTERY_CARD_LoadResourceBG( const MYSTERY_CARD_SETUP *cp_setup, const MYSTERY_CARD_DATA *cp_data, HEAPID heapID )
{ 
  PALTYPE         paltype;

  //描画先をチェック
  if( cp_setup->back_frm < GFL_BG_FRAME0_S )
  { 
    paltype   = PALTYPE_MAIN_BG;
  }
  else
  { 
    paltype   = PALTYPE_SUB_BG;
  }

  //BG読み込み追加
  {
    //上画面ならばもらったとき。下画面ならばアルバムの中
    const u16 load_scr  = cp_setup->back_frm > GFL_BG_FRAME3_M ?
      NARC_mystery_fushigi_card_album_NSCR: NARC_mystery_fushigi_card_view_NSCR;

    const u16 src_ofs = MYSTERY_CARD_DATA_GetCardPltOfs( cp_data );

    ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
    //BG
    GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_mystery_fushigi_card_NCLR,
        paltype, src_ofs*0x20, cp_setup->back_plt_num*0x20, 0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_card_NCGR, 
        cp_setup->back_frm, 0, 0, FALSE, heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, load_scr,
        cp_setup->back_frm, 0, 0, FALSE, heapID );
    GFL_BG_ChangeScreenPalette( cp_setup->back_frm, 0, 0,34,24, cp_setup->back_plt_num );

    GFL_BG_LoadScreenReq( cp_setup->back_frm );

    GFL_ARC_CloseDataHandle( p_handle ); 
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ読み込み
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 *	@param	*p_res                  リソース
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_CARD_ChangePalettleBG( const MYSTERY_CARD_SETUP *cp_setup, const MYSTERY_CARD_DATA *cp_data, HEAPID heapID )
{
  PALTYPE         paltype;

  //描画先をチェック
  if( cp_setup->back_frm < GFL_BG_FRAME0_S )
  { 
    paltype   = PALTYPE_MAIN_BG;
  }
  else
  { 
    paltype   = PALTYPE_SUB_BG;
  }

  {
    //上画面ならばもらったとき。下画面ならばアルバムの中
    const u16 load_scr  = cp_setup->back_frm > GFL_BG_FRAME3_M ?
      NARC_mystery_fushigi_card_album_NSCR: NARC_mystery_fushigi_card_view_NSCR;

    const u16 src_ofs = MYSTERY_CARD_DATA_GetCardPltOfs( cp_data );

    ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
    //BG
    GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_mystery_fushigi_card_NCLR,
        paltype, src_ofs*0x20, cp_setup->back_plt_num*0x20, 0x20, heapID );
    GFL_BG_ChangeScreenPalette( cp_setup->back_frm, 0, 0,34,24, cp_setup->back_plt_num );

    GFL_BG_LoadScreenReq( cp_setup->back_frm );

    GFL_ARC_CloseDataHandle( p_handle ); 
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ読み込み
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 *	@param	*p_res                  リソース
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_CARD_LoadResourceOBJ( MYSTERY_CARD_WORK *p_wk, MYSTERY_CARD_RES *p_res, HEAPID heapID )
{ 
  //アイコン作成
  { 
    { 
      ARCHANDLE	*	p_handle	= MYSTERY_CARD_DATA_GetArcHandle( &p_wk->data, heapID );

      //パレット転送
      { 
        void *p_plt_buff;
        NNSG2dPaletteData *p_plt_data;

        p_plt_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, MYSTERY_CARD_DATA_GetResPlt(&p_wk->data), &p_plt_data, p_wk->heapID );

        NAGI_Printf( "size %d\n", p_plt_data->szByte );
        {
          int i;
          u16 plt;
          for( i = 0; i < 0x10; i++ )
          { 
            plt = ((u16*)p_plt_data->pRawData)[i];
            NAGI_Printf( "color %d \n", plt );
          }
        }
#if 0
        GFL_CLGRP_PLTT_Replace( p_res->res_icon_plt, p_plt_data->pRawData, 3 );
#else
      { 
        u32 plt_adrs  = p_res->setup.icon_obj_plt_num * 0x20;
        if( p_res->draw_type == CLSYS_DRAW_MAIN )
        { 
          plt_adrs  += HW_OBJ_PLTT;
        }
        else
        { 
          plt_adrs  += HW_DB_OBJ_PLTT;
        }
        GFL_STD_MemCopy( p_plt_data->pRawData, (void*)plt_adrs, 0x20*3 );
      }
#endif

        GFL_HEAP_FreeMemory( p_plt_buff );
      }

      //キャラ転送
      { 
        void *p_chr_buff;
        NNSG2dCharacterData *p_chr_data;

        p_chr_buff  = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, MYSTERY_CARD_DATA_GetResCgx(&p_wk->data), 0, &p_chr_data, p_wk->heapID );
        GFL_CLGRP_CGR_ReplaceEx( p_res->res_icon_cgx, p_chr_data->pRawData, 32*4*4, 0, p_res->draw_type );

        GFL_HEAP_FreeMemory( p_chr_buff );
      }

      GFL_ARC_CloseDataHandle( p_handle );
    }

    //CLWK設定
    { 
      GFL_CLACTPOS pos;
      pos.x  = 216;
      pos.y  = 22;

      //アイテムアイコンのリソースだけ、ずれているため
      if(MYSTERY_CARD_DATA_GetType(&p_wk->data) == MYSTERYGIFT_TYPE_ITEM )
      { 
        pos.x   += 4;
        pos.y   += 8;
      }
      GFL_CLACT_WK_SetPlttOffs( p_res->p_icon,
              MYSTERY_CARD_DATA_GetPltOfs( &p_wk->data ), CLWK_PLTTOFFS_MODE_OAM_COLOR );
      GFL_CLACT_WK_SetPos( p_res->p_icon, &pos, p_res->draw_type );
      GFL_CLACT_WK_SetDrawEnable( p_res->p_icon, TRUE );
    }
  }

  //シルエット作成
  if( MYSTERY_CARD_DATA_GetType(&p_wk->data) == MYSTERYGIFT_TYPE_POKEMON )
  { 
#ifdef MEMORY_LOAD_SILHOUETTE
    GFL_CLGRP_PLTT_Replace( p_res->res_silhouette_plt, p_wk->p_plt_data->pRawData, 1 );

    //ついでにフェード用に色を保存
    GFL_STD_MemCopy( p_wk->p_plt_data->pRawData, p_res->plt_pokemon, sizeof(u16) * 0x10 );
    GFL_STD_MemFill16( p_res->plt_silhouette, GX_RGB( 0, 0, 0), sizeof(u16) * 0x10 );

    //キャラ転送
    GFL_CLGRP_CGR_ReplaceEx( p_res->res_silhouette_cgx, p_wk->p_chr_data->pRawData, 32*12*12, 0, p_res->draw_type );

#else //MEMORY_LOAD_SILHOUETTE
    { 
      const GIFT_PRESENT_POKEMON  *cp_pokemon = &p_wk->cp_data->data.pokemon;
      POKEMON_PARAM* p_pp = Mystery_CreatePokemon( p_wk->cp_data, GFL_HEAP_LOWID(heapID), p_wk->p_gamedata );
      ARCHANDLE *p_handle = POKE2DGRA_OpenHandle( GFL_HEAP_LOWID(heapID) );

      p_wk->mons_no = cp_pokemon->mons_no;
      p_wk->form_no = cp_pokemon->form_no;
      p_wk->is_egg  = cp_pokemon->egg;

      //パレット転送
      { 

        void *p_plt_buff;
        NNSG2dPaletteData *p_plt_data;

	      u16 plt	= POKEGRA_GetPalArcIndex( 
            PP_Get( p_pp, ID_PARA_monsno, NULL ),
            PP_Get( p_pp, ID_PARA_form_no, NULL ), 
            PP_Get( p_pp, ID_PARA_sex, NULL ),
            PP_CheckRare( p_pp ), 
            POKEGRA_DIR_FRONT,
            PP_Get( p_pp, ID_PARA_tamago_flag, NULL ) 
            );
        p_plt_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, plt, &p_plt_data, heapID );
        GFL_CLGRP_PLTT_Replace( p_res->res_silhouette_plt, p_plt_data->pRawData, 1 );

        //ついでにフェード用に色を保存
        GFL_STD_MemCopy( p_plt_data->pRawData, p_res->plt_pokemon, sizeof(u16) * 0x10 );
        GFL_STD_MemFill16( p_res->plt_silhouette, GX_RGB( 0, 0, 0), sizeof(u16) * 0x10 );

        GFL_HEAP_FreeMemory( p_plt_buff );
      }

      //キャラ転送
      { 
        void *p_chr_buff;
        NNSG2dCharacterData *p_chr_data;
        p_chr_buff  = POKE2DGRA_LoadCharacterPPP( &p_chr_data, PP_GetPPPPointerConst(p_pp), POKEGRA_DIR_FRONT, heapID );
        GFL_CLGRP_CGR_ReplaceEx( p_res->res_silhouette_cgx, p_chr_data->pRawData, 32*12*12, 0, p_res->draw_type );

        GFL_HEAP_FreeMemory( p_chr_buff );
      }

      GFL_HEAP_FreeMemory( p_pp );
      GFL_ARC_CloseDataHandle( p_handle );
    }
#endif //MEMORY_LOAD_SILHOUETTE

    //CLWK設定
    { 
      GFL_CLACT_WK_SetRotation( p_res->p_silhouette, 0 );
      GFL_CLACT_WK_SetDrawEnable( p_res->p_silhouette, TRUE );
    }

    //パレットフェード設定
    { 
      int i;
      u16 *p_plt_addr;
      
      if( p_res->setup.back_frm < GFL_BG_FRAME0_S )
      { 
        p_plt_addr  = (u16*)(HW_OBJ_PLTT + GFL_CLGRP_PLTT_GetAddr( p_res->res_silhouette_plt, p_res->draw_type ));
      }
      else
      { 
        p_plt_addr  = (u16*)(HW_DB_OBJ_PLTT + GFL_CLGRP_PLTT_GetAddr( p_res->res_silhouette_plt, p_res->draw_type ));
      }

      for( i = 0; i < 0x10; i++ )
      { 
        p_plt_addr[i] = p_res->plt_silhouette[i];
      }
    }

    if( p_res->setup.back_frm < GFL_BG_FRAME0_S )
    { 
      u8 back_frm = p_res->setup.back_frm - GFL_BG_FRAME0_M;
      u8 font_frm = p_res->setup.font_frm - GFL_BG_FRAME0_M;
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_NONE,
          (1 << back_frm) | (1 << font_frm),
          MYSTERY_CARD_SILHOUETTE_EV1,
          MYSTERY_CARD_SILHOUETTE_EV2
          );
    }
    else
    { 
      u8 back_frm = p_res->setup.back_frm - GFL_BG_FRAME0_S;
      u8 font_frm = p_res->setup.font_frm - GFL_BG_FRAME0_S;
      G2S_SetBlendAlpha(
          GX_BLEND_PLANEMASK_NONE,
          (1 << back_frm) | (1 << font_frm),
          MYSTERY_CARD_SILHOUETTE_EV1,
          MYSTERY_CARD_SILHOUETTE_EV2
          );
    }
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( p_res->p_silhouette, FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  エフェクト開始
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_StartEffect( MYSTERY_CARD_WORK *p_wk )
{ 
  if( MYSTERY_CARD_DATA_GetType(&p_wk->data) == MYSTERYGIFT_TYPE_POKEMON )
  { 
    if( p_wk->seq == 0 )
    { 
      p_wk->seq     = MYSTERY_CARD_EFFECT_START_SEQ;
      p_wk->cnt     = 0;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  エフェクト終了
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_EndEffect( MYSTERY_CARD_WORK *p_wk )
{ 
  if( MYSTERY_CARD_DATA_GetType(&p_wk->data) == MYSTERYGIFT_TYPE_POKEMON )
  { 
    int i;
    u16 *p_plt_addr;
    void (*ChangeBlendAlpha)( int, int );

    p_wk->seq     = 0;
    p_wk->cnt     = 0;

    GFL_CLACT_WK_SetBgPri( p_wk->p_res->p_silhouette, p_wk->p_res->silhouette_bg_pri );

    if( p_wk->p_res->is_main )
    { 
      p_plt_addr  = (u16*)(HW_OBJ_PLTT + GFL_CLGRP_PLTT_GetAddr( p_wk->p_res->res_silhouette_plt, p_wk->p_res->draw_type ));
      ChangeBlendAlpha  = G2_ChangeBlendAlpha;
    }
    else
    { 
      p_plt_addr  = (u16*)(HW_DB_OBJ_PLTT + GFL_CLGRP_PLTT_GetAddr( p_wk->p_res->res_silhouette_plt, p_wk->p_res->draw_type ));
      ChangeBlendAlpha  = G2S_ChangeBlendAlpha;
    }

    ChangeBlendAlpha( 
        MYSTERY_CARD_SILHOUETTE_EV1,
        MYSTERY_CARD_SILHOUETTE_EV2
        );

    GFL_STD_MemFill16( p_wk->p_res->plt_silhouette, GX_RGB( 0, 0, 0), sizeof(u16) * 0x10 );
    GFL_STD_MemCopy16( p_wk->p_res->plt_silhouette, p_plt_addr, sizeof(u16) * 0x10 );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  エフェクト終了待ち
 *
 *	@param	const MYSTERY_CARD_WORK *cp_wk ワーク
 *	@retval TRUEエフェクト終了  FALSEエフェクト動作中
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_CARD_IsEndEffect( const MYSTERY_CARD_WORK *cp_wk )
{ 
  return cp_wk->seq == 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ワークサイズを取得
 *
 *	@return ワークサイズ
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_CARD_GetWorkSize( void )
{ 
  return sizeof(MYSTERY_CARD_WORK);
}

static void MYSTERY_CARD_TransTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{ 
  MYSTERY_CARD_WORK *p_wk = p_wk_adrs;

  if( p_wk->is_task_end == FALSE )
  {
    p_wk->is_task_end = TRUE;

    //BG転送
    switch( p_wk->trans_type )
    {
    case MYSTERY_CARD_TRANS_TYPE_NORMAL:
      MYSTERY_CARD_LoadResourceBG( &p_wk->p_res->setup, &p_wk->data, p_wk->heapID );
      break;
    case MYSTERY_CARD_TRANS_TYPE_NOBG:
      MYSTERY_CARD_ChangePalettleBG( &p_wk->p_res->setup, &p_wk->data, p_wk->heapID );
      break;
    }

    //OBJ反映
    MYSTERY_CARD_LoadResourceOBJ( p_wk, p_wk->p_res, p_wk->heapID );

    //BMPWIN転送
    MYSTERY_MSGWINSET_SetBuff( p_wk->p_winbuff );
    MYSTERY_MSGWINSET_Trans( p_wk->p_res->p_winset );

    GFL_BG_SetVisible( p_wk->p_res->font_frm, TRUE );
    GFL_BG_SetVisible( p_wk->p_res->back_frm, TRUE );

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  画面反映
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 *	@param  type                    転送タイプ
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_Trans( MYSTERY_CARD_WORK *p_wk, MYSTERY_CARD_TRANS_TYPE type )
{ 
  p_wk->trans_type  = type;
  if( p_wk->p_trans_task == NULL )
  { 
    p_wk->p_trans_task  = GFUser_VIntr_CreateTCB( MYSTERY_CARD_TransTask, p_wk, 0 );
  }
  else
  { 
    p_wk->is_task_end = FALSE;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  画面クリア
 *
 *	@param	MYSTERY_CARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_CARD_RES_Clear( MYSTERY_CARD_RES *p_wk )
{ 
//  GFL_BG_ClearScreen( MYSTERY_ALBUM_CARD_FRM_S );
  GFL_BG_ClearScreen( MYSTERY_ALBUM_CARD_FONT_S );
//  GFL_BG_LoadScreenReq( MYSTERY_ALBUM_CARD_FRM_S );
  GFL_BG_LoadScreenReq( MYSTERY_ALBUM_CARD_FONT_S );
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon, FALSE );
  { 
    GFL_CLACTPOS pos;
    pos.x  = MYSTERY_CARD_SILHOUETTE_POS_X;
    pos.y  = MYSTERY_CARD_SILHOUETTE_POS_Y;
    GFL_CLACT_WK_SetPos( p_wk->p_silhouette, &pos, p_wk->draw_type );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_silhouette, FALSE );
  }

  GFL_BG_SetVisible( MYSTERY_ALBUM_CARD_FRM_S, FALSE );
}

//=============================================================================
/**
 *      PRIVATE
 */
//=============================================================================


#define MOVE_SHAKE_ROTATE_NUM (11)

static const u16 sc_next_angle[ MOVE_SHAKE_ROTATE_NUM ] =
{ 
  0xFFFF*(0+10)/360, 0xFFFF*(360-10)/360,0xFFFF*360/360,
};
static const s8 sc_next_dir[ MOVE_SHAKE_ROTATE_NUM ] =
{ 
  1,-1, 1,
};
static const s8 sc_next_sync[ MOVE_SHAKE_ROTATE_NUM ] =
{ 
  2,3,3,2,2,1,1,1,0,0,10,
};
static const GFL_POINT sc_next_pos[ MOVE_SHAKE_ROTATE_NUM ] =
{ 
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X + 5,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X - 5,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X + 3,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X - 3,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X + 2,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X - 2,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X - 1,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X + 1,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X - 1,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
  { 
    MYSTERY_CARD_SILHOUETTE_POS_X,
    MYSTERY_CARD_SILHOUETTE_POS_Y,
  },
};

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	MOVE_SHAKE_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MOVE_SHAKE_Init( MOVE_SHAKE_WORK *p_wk, u16 init_angle, s16 x, s16 y )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_SHAKE_WORK) );
  p_wk->max_cnt     = MOVE_SHAKE_ROTATE_NUM;
  p_wk->init_angle  = init_angle;
  p_wk->next_angle  = init_angle;
  p_wk->init_pos.x  = x;
  p_wk->init_pos.y  = y;
  p_wk->next_pos    = p_wk->init_pos;

  p_wk->now_angle = p_wk->init_angle;
  p_wk->now_pos   = p_wk->init_pos;

  NAGI_Printf( "init 0x%x\n", init_angle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  メイン処理
 *
 *	@param	MOVE_SHAKE_WORK *p_wk ワーク
 *	@param  p_angle 回転受け取り
 *	@param  p_pos   座標受け取り
 *
 *	@return TRUEで  FALSEで
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_SHAKE_Main( MOVE_SHAKE_WORK *p_wk, u16 *p_angle, GFL_POINT *p_pos )
{ 

  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_CHECK,
    SEQ_END,
  };
  BOOL ret  =FALSE;

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    p_wk->init_angle  = p_wk->next_angle;
    p_wk->next_angle  = sc_next_angle[ p_wk->cnt ];
    p_wk->next_dir    = sc_next_dir[ p_wk->cnt ];
    p_wk->next_sync   = sc_next_sync[ p_wk->cnt ];

    p_wk->init_pos    = p_wk->next_pos;
    p_wk->next_pos    = sc_next_pos[ p_wk->cnt ];
    p_wk->sync        = 0;

    if( p_wk->next_dir > 0 )
    { 
      p_wk->init_angle  = p_wk->init_angle == 0xFFFF? 0: p_wk->init_angle;
      p_wk->next_angle  = p_wk->next_angle == 0x0? 0xFFFF: p_wk->next_angle;
    }
    else
    { 
      p_wk->init_angle  = p_wk->init_angle == 0 ? 0xFFFF: p_wk->init_angle;
      p_wk->next_angle  = p_wk->next_angle == 0xFFFF ? 0x0: p_wk->next_angle;
    }

    p_wk->seq++;
    break;
  case SEQ_MAIN:
    {
      s8 dir    = p_wk->init_angle < p_wk->next_angle ? 1: -1;
      u16 diff  = p_wk->next_angle - p_wk->init_angle;
      s32  pos_dir;

      if( p_wk->next_dir != dir )
      { 
        diff = 0xFFFF - diff;
      }

      //回転動作
//      p_wk->now_angle = p_wk->init_angle + 
//        p_wk->next_dir * ( diff * p_wk->sync / p_wk->next_sync);

      pos_dir = p_wk->next_pos.x - p_wk->init_pos.x;
      pos_dir = MATH_IAbs(pos_dir) / pos_dir;

      //座標動作
      p_wk->now_pos.x = p_wk->init_pos.x + pos_dir * ((MATH_IAbs( p_wk->next_pos.x - p_wk->init_pos.x ) * p_wk->sync) / p_wk->next_sync);
      p_wk->now_pos.y = p_wk->init_pos.y + pos_dir * ((MATH_IAbs( p_wk->next_pos.y - p_wk->init_pos.y ) * p_wk->sync) / p_wk->next_sync);

      if( p_wk->sync++ >= p_wk->next_sync )
      { 
        p_wk->now_angle = p_wk->next_angle;
        p_wk->now_pos   = p_wk->next_pos;

        p_wk->seq++;
      }
    }
    break;
  case SEQ_CHECK:
    if( ++p_wk->cnt < p_wk->max_cnt )
    { 
      p_wk->seq = SEQ_INIT;
    }
    else
    { 
      p_wk->seq++;
    }
    break;

  case SEQ_END:
    ret = TRUE;
  }

  if( p_angle )
  { 
    *p_angle  = p_wk->now_angle;
  }

  if( p_pos )
  { 
    *p_pos  = p_wk->now_pos;
  }

  return ret;
}
