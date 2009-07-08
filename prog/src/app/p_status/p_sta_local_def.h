//======================================================================
/**
 * @file  p_sta_local_def.c
 * @brief ポケモンステータス ローカル定義
 * @author  ariizumi
 * @data  09/07/01
 *
 * モジュール名：POKE_STATUS
 */
//======================================================================
#pragma once

#include "app/p_status.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "sound/pm_sndsys.h"
#include "debug/debugwin_sys.h"

#include "p_status_gra.naix"
#include "msg/msg_pokestatus.h"

#pragma mark[>define BG
//BG面定義
#define PSTATUS_BG_MAIN_BG (GFL_BG_FRAME3_M)
#define PSTATUS_BG_PLATE   (GFL_BG_FRAME2_M)
#define PSTATUS_BG_PARAM   (GFL_BG_FRAME1_M)
#define PSTATUS_BG_3D      (GFL_BG_FRAME0_M)

#define PSTATUS_BG_SUB_BG     (GFL_BG_FRAME3_S)
#define PSTATUS_BG_SUB_PLATE  (GFL_BG_FRAME2_S)

//BGパレット
#define PSTATUS_BG_PLT_BAR  (0x9)
#define PSTATUS_BG_PLT_FONT (0xe)

//メインページの幅(キャラ単位)
#define PSTATUS_MAIN_PAGE_WIDTH (19)

#pragma mark[>define Font
//文字列共通オフセット
#define PSTATUS_STR_OFS_X (2)
#define PSTATUS_STR_OFS_Y (2)

#pragma mark[>define OBJ
#define PSTATUS_OBJPLT_ICON (0x0)
#define PSTATUS_OBJPLT_RIBBON_BAR (0xA)

//OBJリソースIdx
enum PSTATUS_CEL_RESOURCE
{
  SCR_PLT_ICON,
  
  SCR_NCG_ICON,
  
  SCR_ANM_ICON,
  
  SCR_MAX ,
  
  SCR_PLT_START = SCR_PLT_ICON,
  SCR_PLT_END = SCR_PLT_ICON,
  SCR_NCG_START = SCR_NCG_ICON,
  SCR_NCG_END = SCR_NCG_ICON,
  SCR_ANM_START = SCR_ANM_ICON,
  SCR_ANM_END = SCR_ANM_ICON,
  
};

//アイコンのセルのアニメ種類
enum PSTATUS_BARICON_ANIME
{
  SBA_PAGE1_NORMAL,
  SBA_PAGE1_SELECT,
  SBA_PAGE2_NORMAL,
  SBA_PAGE2_SELECT,
  SBA_PAGE3_NORMAL,
  SBA_PAGE3_SELECT,
  SBA_CHECK,
  SBA_BUTTON_UP,
  SBA_BUTTON_DOWN,
  SBA_BUTTON_EXIT,
  SBA_BUTTON_RETURN,
};

//BARのアイコン(ページ以外
enum PSTATUS_BARICON_TYPE
{
  SBT_PAGE1,
  SBT_PAGE2,
  SBT_PAGE3,
  SBT_CHECK,
  SBT_CURSOR_UP,
  SBT_CURSOR_DOWN,
  SBT_EXIT,
  SBT_RETURN,
  
  SBT_MAX,
};

//BARの座標
#define PSTATUS_BAR_CELL_Y (168)
#define PSTATUS_BAR_CELL_PAGE1_X (0)
#define PSTATUS_BAR_CELL_PAGE2_X (40)
#define PSTATUS_BAR_CELL_PAGE3_X (80)
#define PSTATUS_BAR_CELL_CHECK_X (120)
#define PSTATUS_BAR_CELL_CURSOR_UP_X (144)
#define PSTATUS_BAR_CELL_CURSOR_DOWN_X (168)
#define PSTATUS_BAR_CELL_CURSOR_EXIT (200)
#define PSTATUS_BAR_CELL_CURSOR_RETURN (232)

#pragma mark[>define

typedef enum
{
  PPT_INFO,     //ポケモン情報
  PPT_SKILL,    //技
  PPT_RIBBON,   //リボン
  
  PPT_MAX,
}PSTATUS_PAGE_TYPE;

typedef struct _PSTATUS_SUB_WORK PSTATUS_SUB_WORK;
typedef struct _PSTATUS_INFO_WORK PSTATUS_INFO_WORK;
typedef struct _PSTATUS_RIBBON_WORK PSTATUS_RIBBON_WORK;

typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  
  PSTATUS_DATA *psData;
  
  u8  dataPos;  //何番目か？
  u8  befDataPos;
  u8  scrollCnt;
  u32 befVCount;
    
  PSTATUS_PAGE_TYPE page;
  PSTATUS_PAGE_TYPE befPage;

  //3D
  GFL_G3D_CAMERA    *camera;
  GFL_BBD_SYS       *bbdSys;
  
  //MSG系
  GFL_MSGDATA *msgHandle;
  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;

  //サブワーク
  PSTATUS_SUB_WORK *subWork;
  PSTATUS_INFO_WORK *infoWork;
  PSTATUS_RIBBON_WORK *ribbonWork;
  
  //Cell系
  u32 cellRes[SCR_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkBarIcon[SBT_MAX];

#if PM_DEBUG
  POKEMON_PARAM *debPp;
#endif

}PSTATUS_WORK;

