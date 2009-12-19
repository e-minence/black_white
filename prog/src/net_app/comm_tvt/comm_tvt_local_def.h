//======================================================================
/**
 * @file	comm_tvt_local_def.h
 * @brief	通信TVT定義
 * @author	ariizumi
 * @data	09/12/16
 *
 * モジュール名：CTVT_
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define CTVT_USE_DEBUG (1)

#if CTVT_USE_DEBUG
#define CTVT_TPrintf(...) (void)((OS_TFPrintf(1,__VA_ARGS__)))
#define CTVT_Printf(...)  (void)((OS_FPrintf(1,__VA_ARGS__)))
#else
#define CTVT_TPrintf(...) ((void)0)
#define CTVT_Printf(...)  ((void)0)
#endif //DEB_ARI


#define CTVT_FRAME_MAIN_DRAW    (GFL_BG_FRAME2_M)
#define CTVT_FRAME_MAIN_CAMERA  (GFL_BG_FRAME3_M)

#define CTVT_FRAME_SUB_MSG   (GFL_BG_FRAME0_S)
#define CTVT_FRAME_SUB_BAR   (GFL_BG_FRAME1_S)
#define CTVT_FRAME_SUB_MISC  (GFL_BG_FRAME2_S)
#define CTVT_FRAME_SUB_BG    (GFL_BG_FRAME3_S)

#define CTVT_PAL_BG_SUB_BG    (0) //5本
#define CTVT_PAL_BG_BUTTON_NONE (1)
#define CTVT_PAL_BG_BUTTON_ACTIVE (3)
#define CTVT_PAL_BG_BUTTON_DISABLE (4)

#define CTVT_PAL_BG_SUB_BAR   (5) //1本

#define CTVT_PAL_OBJ_MAIN_COMMON (0) //5本

#define CTVT_PAL_OBJ_SUB_COMMON  (0) //5本
#define CTVT_PAL_OBJ_SUB_BARICON  (5) //3本

#define CTVT_BUTTON_TALK (PAD_BUTTON_A|PAD_BUTTON_R)

#define CTVT_MEMBER_NUM (4)


//スクリーンのバッファサイズ
#define CTVT_BUFFER_SCR_SIZE (256*192*2)  //0x18000

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//上画面のモード(1枚2分割4分割
typedef enum
{
  CTDM_SINGLE,
  CTDM_DOUBLE,
  CTDM_FOUR,

  CTDM_MAX, //サイズとかの配列に使ってるので注意
}COMM_TVT_DISP_MODE;

//操作のモード
typedef enum
{
  CTM_NONE, 
  CTM_TALK, //会話
  CTM_CALL, //呼び出し
  CTM_DRAW, //落書き

  CTM_END,

}COMM_TVT_MODE;

//セルリソース
typedef enum
{
  CTOR_COMMON_M_PLT,
  CTOR_COMMON_S_PLT,
  CTOR_BAR_BUTTON_S_PLT,
  CTOR_BAR_BUTTON_M_PLT,

  CTOR_COMMON_M_NCG,
  CTOR_COMMON_S_NCG,
  CTOR_BAR_BUTTON_S_NCG,
  CTOR_BAR_BUTTON_M_NCG,

  CTOR_COMMON_M_ANM,
  CTOR_COMMON_S_ANM,
  CTOR_BAR_BUTTON_ANM,  //上下共通

  CTOR_MAX,
}COMM_TVT_OBJ_RES;
#define CTOR_PLT_TOP (CTOR_COMMON_M_PLT)
#define CTOR_NCG_TOP (CTOR_COMMON_M_NCG)
#define CTOR_ANM_TOP (CTOR_COMMON_M_ANM)

//セルアニメインデックス
typedef enum
{
  CTOAM_PAUSE,
  CTOAM_PAUSE_ACTIVE,
  CTOAM_PLAY,
  CTOAM_PLAY_ACTIVE,
  CTOAM_SLIDER,
  CTOAM_SLIDER_ACTIVE,
  CTOAM_YOBIDASHI,
  CTOAM_YOBIDASHI_ACTIVE,
  CTOAM_YOBIDASHI_BAR,
  CTOAM_CHECK,
  CTOAM_CHECK_SELECT,
  CTOAM_CHECK_NONE,
  CTOAM_SCROLL_BAR,
  CTOAM_SCROLL_BAR_ACTIVE,
  CTOAM_PEN,
  CTOAM_SUPOITO,
  CTOAM_KESIGOMU,
}COMM_TVT_OBJ_ANM_MAIN;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _COMM_TVT_WORK COMM_TVT_WORK;
typedef struct _CTVT_CAMERA_WORK CTVT_CAMERA_WORK;
typedef struct _CTVT_COMM_WORK CTVT_COMM_WORK;

typedef struct _CTVT_TALK_WORK CTVT_TALK_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//システム取得系
extern CTVT_CAMERA_WORK* COMM_TVT_GetCameraWork( COMM_TVT_WORK *work );
extern CTVT_COMM_WORK* COMM_TVT_GetCommWork( COMM_TVT_WORK *work );

//数値取得系
extern const HEAPID COMM_TVT_GetHeapId( const COMM_TVT_WORK *work );
extern ARCHANDLE* COMM_TVT_GetArcHandle( const COMM_TVT_WORK *work );
extern const u32 COMM_TVT_GetObjResIdx( const COMM_TVT_WORK *work , const COMM_TVT_OBJ_RES resIdx );
extern GFL_CLUNIT* COMM_TVT_GetCellUnit( const COMM_TVT_WORK *work );
extern const BOOL COMM_TVT_GetUpperFade( const COMM_TVT_WORK *work );
extern void COMM_TVT_SetUpperFade( COMM_TVT_WORK *work , const BOOL flg );

extern const u8   COMM_TVT_GetConnectNum( const COMM_TVT_WORK *work );
extern void   COMM_TVT_SetConnectNum( COMM_TVT_WORK *work , const u8 num);
extern const COMM_TVT_DISP_MODE COMM_TVT_GetDispMode( const COMM_TVT_WORK *work );
extern const BOOL COMM_TVT_IsDoubleMode( const COMM_TVT_WORK *work );
extern void COMM_TVT_SetDoubleMode( COMM_TVT_WORK *work , const BOOL flg );
extern const u8   COMM_TVT_GetSelfIdx( const COMM_TVT_WORK *work );
extern void COMM_TVT_SetSelfIdx( COMM_TVT_WORK *work , const u8 idx );

//UTIL
extern const BOOL COMM_TVT_IsTwlMode( void );

