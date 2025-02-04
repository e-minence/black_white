//============================================================================================
/**
 * @file  palanm.h
 * @brief パレットフェード処理
 * @author  Hiroyuki Nakamura
 * @date  2004.11.24
 */
//============================================================================================
#ifndef PALANM_H
#define PALANM_H

#include <tcb.h>

#undef GLOBAL
#ifdef PALANM_H_GLOBAL
#define GLOBAL  /*  */
#else
#define GLOBAL  extern
#endif


//============================================================================================
//  定数定義
//============================================================================================
// PaletteFadeReqで使用
#define PF_BIT_MAIN_BG    ( 0x0001 )  // メイン画面の通常BGパレット
#define PF_BIT_SUB_BG   ( 0x0002 )  // サブ画面の通常BGパレット
#define PF_BIT_MAIN_OBJ   ( 0x0004 )  // メイン画面の通常OBJパレット
#define PF_BIT_SUB_OBJ    ( 0x0008 )  // サブ画面の通常OBJパレット
#define PF_BIT_MAIN_BG_EX0  ( 0x0010 )  // メイン画面の拡張BGパレット0
#define PF_BIT_MAIN_BG_EX1  ( 0x0020 )  // メイン画面の拡張BGパレット1
#define PF_BIT_MAIN_BG_EX2  ( 0x0040 )  // メイン画面の拡張BGパレット2
#define PF_BIT_MAIN_BG_EX3  ( 0x0080 )  // メイン画面の拡張BGパレット3
#define PF_BIT_SUB_BG_EX0 ( 0x0100 )  // サブ画面の拡張BGパレット0
#define PF_BIT_SUB_BG_EX1 ( 0x0200 )  // サブ画面の拡張BGパレット1
#define PF_BIT_SUB_BG_EX2 ( 0x0400 )  // サブ画面の拡張BGパレット2
#define PF_BIT_SUB_BG_EX3 ( 0x0800 )  // サブ画面の拡張BGパレット3
#define PF_BIT_MAIN_OBJ_EX  ( 0x1000 )  // メイン画面の拡張OBJパレット
#define PF_BIT_SUB_OBJ_EX ( 0x2000 )  // サブ画面の拡張OBJパレット

//通常パレットすべて
#define PF_BIT_NORMAL_ALL (PF_BIT_MAIN_BG|\
               PF_BIT_SUB_BG|\
               PF_BIT_MAIN_OBJ|\
               PF_BIT_SUB_OBJ)

#define PF_BIT_MAIN_ALL   (PF_BIT_MAIN_BG|\
               PF_BIT_MAIN_OBJ)

#define PF_BIT_SUB_ALL    (PF_BIT_SUB_BG|\
               PF_BIT_SUB_OBJ)

// データ参照用定義
typedef enum {
  FADE_MAIN_BG = 0,
  FADE_SUB_BG,
  FADE_MAIN_OBJ,
  FADE_SUB_OBJ,

  FADE_MAIN_BG_EX0,
  FADE_MAIN_BG_EX1,
  FADE_MAIN_BG_EX2,
  FADE_MAIN_BG_EX3,
  FADE_SUB_BG_EX0,
  FADE_SUB_BG_EX1,
  FADE_SUB_BG_EX2,
  FADE_SUB_BG_EX3,
  FADE_MAIN_OBJ_EX,
  FADE_SUB_OBJ_EX
}FADEREQ;

// バッファ選択
typedef enum{
  FADEBUF_TRANS,    ///<転送バッファを対象
  FADEBUF_SRC,    ///<転送元バッファを対象
  FADEBUF_ALL,    ///<全てのバッファを対象
}FADEBUF;

#define NORMAL_PALETTE_SIZE ( 4 )   // BGx2 + OBJx2
#define EXTRA_PALETTE_SIZE  ( 10 )    // BGx8 + OBJx2
#define ALL_PALETTE_SIZE  ( NORMAL_PALETTE_SIZE + EXTRA_PALETTE_SIZE )

// 計算データ
typedef struct {
  u16 fade_bit;

  u16 wait:6;
  u16 now_evy:5;
  u16 end_evy:5;

  u16 next_rgb:15;
  u16 direction:1;    //0:evy inc  1:evy dec

  u16 fade_value:5;   //ﾌｪｰﾄﾞをする時の1SyncにEVYに対して加減算する値
  u16 wait_cnt:6;
  u16 dmy:5;
}FADE_REQ_PARAM;

typedef struct {
  u16 * def_wk;
  u16 * trans_wk;

  u32 siz;  // 通常パレットは16個、拡張パレットは256個単位
/*
  通常パレット32個 : siz = 16*2*2
  拡張パレット1024個 : siz = 256*4*2
*/
  FADE_REQ_PARAM  prm;

}FADE_REQ_EX2;


// 拡張パレットの転送アドレス
#define EX_PAL_SLOT0_ADRS ( 0x0000 )    // 拡張パレット０
#define EX_PAL_SLOT1_ADRS ( 0x2000 )    // 拡張パレット１
#define EX_PAL_SLOT2_ADRS ( 0x4000 )    // 拡張パレット２
#define EX_PAL_SLOT3_ADRS ( 0x6000 )    // 拡張パレット３

// パレット１本分のサイズ
#define FADE_PAL_ONE_SIZE ( 32 )    // 通常パレット 16x2
#define FADE_EXPAL_ONE_SIZE ( 512 )   // 拡張パレット 256x2
// パレット１６本分のサイズ
#define FADE_PAL_ALL_SIZE ( FADE_PAL_ONE_SIZE * 16 )    // 通常パレット 16x2x16
#define FADE_EXPAL_ALL_SIZE ( FADE_EXPAL_ONE_SIZE * 16 )  // 拡張パレット 256x2x16

///EVY最大値
#define EVY_MAX       (16)

//--------------------------------------------------------------
//  計算
//--------------------------------------------------------------
#define FADE_CHANGE( before, after, evy )         \
      ( before + ( ( after - before ) * evy >> 4 ) )

//==============================================================================
//  不定形ポインタ型定義
//==============================================================================
// パレットフェードデータ構造体の不定形ポインタ
typedef struct _PALETTE_FADE_DATA * PALETTE_FADE_PTR;


//============================================================================================
//  外部関数宣言
//============================================================================================
extern PALETTE_FADE_PTR PaletteFadeInit(int heap_id);
extern void PaletteFadeFree(PALETTE_FADE_PTR pfd);

extern void PaletteFadeWorkAdrSet(PALETTE_FADE_PTR pfd, FADEREQ req, void * def_wk,  void * trans_wk, u32 siz );
extern void PaletteFadeWorkAllocSet(PALETTE_FADE_PTR pfd, FADEREQ req, u32 siz, HEAPID heap );
extern void PaletteFadeWorkAllocFree(PALETTE_FADE_PTR pfd, FADEREQ req);
extern void PaletteWorkSet(PALETTE_FADE_PTR pfd, const void * dat, FADEREQ req, u16 pos, u16 siz );
extern void PaletteWorkSet_Arc(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap, FADEREQ req, u32 trans_size, u16 pos);
extern void PaletteWorkSet_ArcHandle(PALETTE_FADE_PTR pfd, ARCHANDLE* handle, ARCDATID dataIdx,  HEAPID heap, FADEREQ req, u32 trans_size, u16 pos);
extern void PaletteWorkSet_VramCopyEx(PALETTE_FADE_PTR pfd, FADEREQ req, u32 trans_size, u16 pos);
extern u8   PaletteFadeReq(PALETTE_FADE_PTR pfd, u16 req_bit, u16 fade_bit, s8 wait,  u8 start_evy, u8 end_evy, u16 next_rgb, GFL_TCBSYS *tcbsys );
extern u8   PaletteFadeReqWrite(PALETTE_FADE_PTR pfd, u16 req_bit, u16 fade_bit, s8 wait,  u8 start_evy, u8 end_evy, u16 next_rgb, GFL_TCBSYS *tcbsys );
extern void PaletteFadeTrans(PALETTE_FADE_PTR pfd);
extern u16  PaletteFadeCheck(PALETTE_FADE_PTR pfd);
extern void PaletteTransSwitch(PALETTE_FADE_PTR pfd, u8 flag);
extern void PaletteAreaClear( u16 bit, HEAPID heap );
extern void PaletteWork_Clear(PALETTE_FADE_PTR pfd, FADEREQ req, FADEBUF select,  u16 clear_code, u16 start, u16 end);
extern void PaletteTrans_AutoSet(PALETTE_FADE_PTR pfd, int on_off);
extern void SoftFade(const u16 *src, u16 *dest, u16 col_num, u8 evy, u16 next_rgb);

void PaletteWorkSet_VramCopy(PALETTE_FADE_PTR pfd, FADEREQ req, u16 pos, u32 trans_size);

extern void PaletteGrayScale(u16* pal, int pal_size);
extern void PaletteGrayScaleFlip(u16* pal, int pal_size);
extern void PaletteGrayScaleShadeTable(u16* pal, int pal_size,u8 *shade_table);
extern void PaletteColorChange(u16* pal, int pal_size, int rp, int gp, int bp);
extern void PokeColorChange(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap, FADEREQ req, u32 trans_size, u16 pos, int r, int g, int b);
extern void SoftFadePfd(PALETTE_FADE_PTR pfd, FADEREQ req, u16 start_pos,  u16 col_num, u8 evy, u16 next_rgb);
extern void ColorConceChange(const u16 *src, u16 *dest, u16 fade_bit, u8 evy, u16 next_rgb);
extern void ColorConceChangePfd(PALETTE_FADE_PTR pfd, FADEREQ req, u16 fade_bit,  u8 evy, u16 next_rgb);
extern void PaletteWorkCopy(PALETTE_FADE_PTR pfd, FADEREQ src_req, u16 src_pos,  FADEREQ dest_req, u16 dest_pos, u16 siz );
extern u16* PaletteWorkDefaultWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type );
extern u16* PaletteWorkTransWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type );
extern u16  PaletteWork_ColorGet(PALETTE_FADE_PTR pfd, FADEREQ req, FADEBUF select, u16 color_pos);
extern void PaletteWorkSetEx_Arc(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap,  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos);
extern void PaletteWorkSetEx_ArcHandle(PALETTE_FADE_PTR pfd, ARCHANDLE* handle, ARCDATID dataIdx, HEAPID heap,  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos);
extern void PaletteWorkSetEx_ArcWork(u32 fileIdx, u32 dataIdx, HEAPID heap, u32 trans_size,  u16 read_pos, void *dest);
extern void PaletteFadeForceStop(PALETTE_FADE_PTR pfd);

/*

extern PALETTE_FADE_PTR PALFADE_Init(int heap_id);
extern void PALFADE_Exit(PALETTE_FADE_PTR pfd);

PALFADE_SetWorkAdr();
PALFADE_AllocWork();
PALFADE_FreeWork();
PALFADE_SetWork();
PALFADE_SetWork_Arc();
PALFADE_SetWork_ArcHandle();
PALFADE_SetWork_VramCopyEx();
PALFADE_Req();
PALFADE_ReqWrite();
PALFADE_Trans();
PALFADE_Check();
PALFADE_SwitchTrans();
PALFADE_ClearTrans();
PALFADE_ClearWork();
PALFADE_SetAutoTrans();
PALFADE_SoftFade();

void PALFADE_CopyVram(PALETTE_FADE_PTR pfd, FADEREQ req, u16 pos, u32 trans_size);

extern void PALFADE_CalcGlayScale(u16* pal, int pal_size);
extern void PALFADE_FlipGlayScale(u16* pal, int pal_size);
extern void PALFADE_ChangeColorRGB(u16* pal, int pal_size, int rp, int gp, int bp);
extern void PALFADE_ChangePokeColor(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap, FADEREQ req, u32 trans_size, u16 pos, int r, int g, int b);
extern void PALFADE_ReqSoftFade(PALETTE_FADE_PTR pfd, FADEREQ req, u16 start_pos,  u16 col_num, u8 evy, u16 next_rgb);
extern void PALFADE_CangeColorConce(const u16 *src, u16 *dest, u16 fade_bit, u8 evy, u16 next_rgb);
//ここまでかえた
extern void ColorConceChangePfd(PALETTE_FADE_PTR pfd, FADEREQ req, u16 fade_bit,  u8 evy, u16 next_rgb);
extern void PaletteWorkCopy(PALETTE_FADE_PTR pfd, FADEREQ src_req, u16 src_pos,  FADEREQ dest_req, u16 dest_pos, u16 siz );
extern u16* PaletteWorkDefaultWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type );
extern u16* PaletteWorkTransWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type );
extern u16  PaletteWork_ColorGet(PALETTE_FADE_PTR pfd, FADEREQ req, FADEBUF select, u16 color_pos);
extern void PaletteWorkSetEx_Arc(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap,  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos);
extern void PaletteWorkSetEx_ArcHandle(PALETTE_FADE_PTR pfd, ARCHANDLE* handle, ARCDATID dataIdx, HEAPID heap,  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos);
extern void PaletteWorkSetEx_ArcWork(u32 fileIdx, u32 dataIdx, HEAPID heap, u32 trans_size,  u16 read_pos, void *dest);
extern void PaletteFadeForceStop(PALETTE_FADE_PTR pfd);


*/


#undef GLOBAL
#endif  /* PALANM_H */
