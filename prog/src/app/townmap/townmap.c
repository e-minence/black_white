//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		map_main.c
 *	@brief	�^�E���}�b�v���C��
 *	@author	Toru=Nagihashi
 *	@data		2009.07.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "townmap_gra.naix"
#include "message.naix"
#include "font/font.naix"

//	common
#include "app/app_menu_common.h"

//	Module
#include "print/gf_font.h"
#include "print/printsys.h"
#include "field/zonedata.h"
#include "app/townmap_data_sys.h"
#include "ui/touchbar.h"	//�^�b�`�o�[

//mine
#include "townmap_grh.h"
#include "app/townmap.h"
#include "app/townmap_util.h"

//sound
#include "townmap_snd.h"

//debug
#include "debug/debugwin_sys.h"
#include "debug/debug_str_conv.h"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	SWITCH
//=====================================
#ifdef PM_DEBUG
#define DEBUG_MENU_USE
#define DEBUG_PRINT_USE
#define DEBUG_POS_CHECK
#define DEBUG_GAMESYS_NONE
#define DEBUG_CURSOR_POS
#define DEBUG_NOT_PULL
#define DEBUG_OBJ_ALLVISIBLE
static GFL_POINT s_debug_pos	=
{	
	80, 25
};

static GFL_POINT s_cursor_pos   =
{ 
  0, 0
};
static BOOL s_is_print_debug  = FALSE;
static BOOL s_is_notpull_debug  = FALSE;
static BOOL	s_is_place_visible_debug  = FALSE;
#endif //PM_DEBUG


//-------------------------------------
///	COMMON
//=====================================
#define RULE_PLACE_MSG_MAX	(6)

#define MAP_SCALE_MOVE_LIMIT_TOP		(-192/2)
#define MAP_SCALE_MOVE_LIMIT_LEFT		(-256/2)
#define MAP_SCALE_MOVE_LIMIT_BOTTOM	(MAP_SCALE_MOVE_LIMIT_TOP+192 - (APPBAR_MENUBAR_H*GFL_BG_1CHRDOTSIZ))
#define MAP_SCALE_MOVE_LIMIT_RIGHT	(MAP_SCALE_MOVE_LIMIT_LEFT+256)

#define MAP_SCALE_CENTER_DISTANCE_UP			(70)
#define MAP_SCALE_CENTER_DISTANCE_DOWN		(20)

//OBJ�D��x
enum
{	
	OBJ_PRIORITY_CURSOR,
	OBJ_PRIORITY_FRONTEND,
	OBJ_PRIORITY_MARK,
	OBJ_PRIORITY_EFFECT,
	OBJ_PRIORITY_MAPOBJ,  //BG���Ⴄ
};

//���S���W
static const GFL_POINT sc_center_pos =
{	
	256/2, 192/2
};

//-------------------------------------
///	PLACE
//=====================================
#define PLACE_PULL_R				(12)
#define PLACE_PULL_STRONG		(FX32_CONST(1))	//�z�����ދ����i�J�[�\�����キ�j
#define PLACEDATA_ANIME_SEQ_VANISH	(0xFF)
enum
{	
	//���т̓^�E���}�b�v�p�p�����[�^(TOWNMAP_DATA_PARAM)�ɏ�����i�J�n�I�t�Z�b�g�̂݁j
	PLACE_DATA_PARAM_POS_X,
	PLACE_DATA_PARAM_POS_Y,
	PLACE_DATA_PARAM_CURSOR_X,
	PLACE_DATA_PARAM_CURSOR_Y,
	PLACE_DATA_PARAM_HIT_START_X,
	PLACE_DATA_PARAM_HIT_START_Y,
	PLACE_DATA_PARAM_HIT_END_X,
	PLACE_DATA_PARAM_HIT_END_Y,
	PLACE_DATA_PARAM_HIT_WIDTH,
	
	PLACE_DATA_PARAM_MAX
};

//-------------------------------------
///	APPBAR
//=====================================
//���\�[�X
enum
{	
	APPBAR_RES_SCALE_PLT,
	APPBAR_RES_SCALE_CHR,
	APPBAR_RES_SCALE_CEL,

	APPBAR_RES_MAX
};
//�J�X�^���{�^��
#define TOUCHBAR_ICON_SCALE	(TOUCHBAR_ICON_CUTSOM1)

//�͈�
#define APPBAR_MENUBAR_H	(3)
#define APPBAR_MENUBAR_Y	(21)

//-------------------------------------
///	CURSOR
//=====================================
//CLWK
enum
{	
	CURSOR_CLWK_CURSOR,
	CURSOR_CLWK_RING,
	
	CURSOR_CLWK_MAX
};
//�ʒu
#define CURSOR_POS_DEFAULT_X		(128)
#define CURSOR_POS_DEFAULT_Y		(96)
//���x
#define CURSOR_ADD_SPEED				(3)
//����
#define CURSOR_MOVE_LIMIT_TOP			(0+8)
#define CURSOR_MOVE_LIMIT_BOTTOM	(192- (APPBAR_MENUBAR_H*GFL_BG_1CHRDOTSIZ))
#define CURSOR_MOVE_LIMIT_LEFT		(0)
#define CURSOR_MOVE_LIMIT_RIGHT		(256-8)

#define CURSOR_MAP_MOVE_RANGE_TOP						(16)
#define CURSOR_MAP_MOVE_RANGE_BOTTOM_APPBAR	((APPBAR_MENUBAR_Y)*GFL_BG_1CHRDOTSIZ)
#define CURSOR_MAP_MOVE_RANGE_BOTTOM				(CURSOR_MAP_MOVE_RANGE_BOTTOM_APPBAR-32)
#define CURSOR_MAP_MOVE_RANGE_LEFT					(16)
#define CURSOR_MAP_MOVE_RANGE_RIGHT					(256-16)

//-------------------------------------
///		INFO
//=====================================
//�����ʔ͈�
#define INFO_BMPWIN_X	(0)
#define INFO_BMPWIN_Y	(0)
#define INFO_BMPWIN_W	(32)
#define INFO_BMPWIN_H	(24)

//�����ʒu
#define INFO_STR_PLACE_CENTERING	//OFF�ɂ����INFO_STR_PLACE_X�̒�`���g�����W�w�肵�܂�
#define INFO_STR_PLACE_X	(16)

#define INFO_STR_PLACE_Y	(48)
#define INFO_STR_GUIDE_X	(28)//(32)
#define INFO_STR_GUIDE_Y	(80)
#define INFO_STR_PLACE1_X	(16)
#define INFO_STR_PLACE1_Y	(128)
#define INFO_STR_PLACE2_X	(16)
#define INFO_STR_PLACE2_Y	(144)
#define INFO_STR_PLACE3_X	(16)
#define INFO_STR_PLACE3_Y	(160)
#define INFO_STR_PLACE4_X	(136)
#define INFO_STR_PLACE4_Y	(128)
#define INFO_STR_PLACE5_X	(136)
#define INFO_STR_PLACE5_Y	(144)
#define INFO_STR_PLACE6_X	(136)
#define INFO_STR_PLACE6_Y	(160)

//-------------------------------------
///	MSGWND
//=====================================
#define MSGWND_BMPWIN_SKY_X	(0)
#define MSGWND_BMPWIN_SKY_Y	(21)
#define MSGWND_BMPWIN_SKY_W	(24)
#define MSGWND_BMPWIN_SKY_H	(3)

#define MSGWND_STR_SKY_X	(16)
#define MSGWND_STR_SKY_Y	(0)

//-------------------------------------
///	�n���E�B���h�E	PLACEWND
//=====================================
#define PLACEWND_BMPWIN_X	(0)
#define PLACEWND_BMPWIN_Y	(0)
#define PLACEWND_BMPWIN_W	(24)
#define PLACEWND_BMPWIN_H	(3)

#define PLACEWND_STR_X		(16)
#define PLACEWND_STR_Y		(2)

#define PLACEWND_WND_POS_X		(128)
#define PLACEWND_WND_POS_Y		(96)

//-------------------------------------
///	MAP
//=====================================
#define MAP_SCALE_SYNC	(10)

//-------------------------------------
///	UI
//=====================================
#define UI_DRAG_MOVE_RATE						(FX32_CONST(1))	

#define UI_SLIDE_MOVE_DISTANCE_MIN	(FX32_CONST(2))	//���̋����ȏ�ŃX���C�h����ΗL��

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
typedef struct 
{
	u32						res[APPBAR_RES_MAX];
	TOUCHBAR_WORK *p_touchbar;
	GAMEDATA			*p_gamedata;
  GFL_CLWK      *p_scale;
  TOUCHBAR_ICON	icon;
  u32           seq;
} APPBAR_WORK;

//-------------------------------------
///	�ꏊ���
//=====================================
typedef struct 
{
	GFL_CLWK						*p_clwk;

	//�I���W�i���f�[�^
	const TOWNMAP_DATA	*cp_data;
	u32									data_idx;

	//�g��k���X�N���[�������Ƃ��̃f�[�^
	s32		param[PLACE_DATA_PARAM_MAX];
	u8		non_active_anm;
	u8		active_anm;
	u8		now_anm;
  u8    dummy;

	fx32	scale;
	BOOL	is_arrive;	//���B�t���O
	BOOL	is_sky;			//����ׂ�t���OTOWNMAP_DATA_PARAM_SKY_FLAG�̉��H�l
  BOOL  is_hide;    //�B���Ă��邩�ǂ���
} PLACE_DATA;
//-------------------------------------
///	�}�[�L���O
//=====================================
typedef struct
{
	GFL_CLWK	*p_clwk;
	const PLACE_DATA *cp_data;
} PLACE_MARK;
//-------------------------------------
///	�ꏊ����
//=====================================
typedef struct
{
  GAMEDATA        *p_gamedata;
	GFL_CLSYS_REND	*p_rend;
	PLACE_DATA			*p_place;
	u32							data_num;
	GFL_POINT				pos;	//PLACE_DATA�̊���W

	s16	limit_top;
	s16	limit_left;
	s16 limit_right;
	s16	limit_bottom;

	PLACE_MARK	player;
} PLACE_WORK;
//-------------------------------------
///	�J�[�\������
//=====================================
typedef struct 
{
	GFL_CLWK	*p_clwk[CURSOR_CLWK_MAX];
	GFL_CLACTPOS	pos;
	BOOL					is_trg;
	BOOL					is_pull;
	const PLACE_DATA *cp_target;
} CURSOR_WORK;
//-------------------------------------
///	�n�}����	
//=====================================
typedef struct 
{
	u8	map_frm;
	u8	road_frm;
	u16	sync;

	s16	limit_top;
	s16	limit_left;
	s16 limit_right;
	s16	limit_bottom;

	BOOL	is_scale_end;
	BOOL	is_scale_req;
	fx32	now_scale;
	fx32	next_scale;
	fx32	init_scale;

	GFL_POINT	pos;
	GFL_POINT	next_pos;
	GFL_POINT	init_pos;

} MAP_WORK;
//-------------------------------------
///	���ʏ��
//=====================================
typedef struct
{
	GFL_BMPWIN	*p_bmpwin;
	STRBUF			*p_strbuf;			//�ėp�o�b�t�@
	GFL_FONT		*p_font;				//�󂯎��
	const GFL_MSGDATA	*cp_place_msg;		//�󂯎��
	const GFL_MSGDATA	*cp_guide_msg;		//�󂯎��
} INFO_WORK;
//-------------------------------------
///	���b�Z�[�W�\���E�B���h�E
//=====================================
typedef struct
{
	GFL_BMPWIN*				p_bmpwin;
	STRBUF*						p_strbuf;
	GFL_FONT*				  p_font;
  const GFL_MSGDATA*			cp_msg;
} MSGWND_WORK;
//-------------------------------------
///	�n���E�B���h�E
//=====================================
typedef struct 
{
	MSGWND_WORK	msgwnd;
	GFL_CLWK		*p_clwk;	//�󂯎��
	const PLACE_DATA *cp_data;
	BOOL				is_update;
	BOOL				is_start;
} PLACEWND_WORK;
//-------------------------------------
///	UI
//=====================================
typedef struct 
{
	VecFx32	start;
	VecFx32	end;
	u32 slide_sync;
	BOOL is_slide;
	BOOL is_reset;
	VecFx32	v;
} UI_WORK;
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param_adrs );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param_adrs;
}; 
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{
	//�O���t�B�b�N�V�X�e��
	TOWNMAP_GRAPHIC_SYS	*p_grh;

	//�V�[�P���X
	SEQ_WORK			seq;

	//�A�v���P�[�V�����o�[
	APPBAR_WORK		appbar;

	//�f�[�^
	TOWNMAP_DATA	*p_data;

	//�J�[�\��
	CURSOR_WORK		cursor;

	//�ꏊ
	PLACE_WORK		place;

	//�n�}
	MAP_WORK			map;

	//���ʏ��
	INFO_WORK			info;

	//����ԁH�̃��b�Z�[�W��
	MSGWND_WORK		msgwnd;

	//�n���E�B���h�E
	PLACEWND_WORK	placewnd;

	//�^�b�`
	UI_WORK				ui;

	//���ʗ��p�V�X�e��
	GFL_FONT			*p_font;
	GFL_MSGDATA		*p_place_msg;
	GFL_MSGDATA		*p_guide_msg;

	//����
	TOWNMAP_PARAM	*p_param;

	//�I�𒆂̂���
	const PLACE_DATA *cp_select;

	//�g�咆���ǂ���
	BOOL	is_scale;
	
  //�^�X�N
	GFL_TCB	*p_vblank_task;

  //VBlank�^�X�N�ւ̃��N�G�X�g
  BOOL is_scale_end_req;

#ifdef PM_DEBUG
	BOOL	is_arrive_debug;
	BOOL	is_checkpos_debug;
	u32		checkpos_num_debug;
#endif

} TOWNMAP_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT TOWNMAP_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT TOWNMAP_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT TOWNMAP_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param_adrs, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
//-------------------------------------
///	APPBAR
//=====================================
static void APPBAR_Init( APPBAR_WORK *p_wk, TOWNMAP_MODE mode, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, GAMEDATA *p_gamedata, HEAPID heapID );
static void APPBAR_Exit( APPBAR_WORK *p_wk );
static void APPBAR_Main( APPBAR_WORK *p_wk, const MAP_WORK *cp_map );
static TOUCHBAR_ICON APPBAR_GetTrg( const APPBAR_WORK *cp_wk );
//-------------------------------------
///	CURSOR
//=====================================
static void CURSOR_Init( CURSOR_WORK *p_wk, GFL_CLWK *p_cursor, GFL_CLWK *p_ring, HEAPID heapID );
static void CURSOR_Exit( CURSOR_WORK *p_wk );
static void CURSOR_Main( CURSOR_WORK *p_wk, const PLACE_WORK *cp_place );
static void CURSOR_GetPos( const CURSOR_WORK *cp_wk, GFL_POINT *p_pos );
static void CURSOR_SetPos( CURSOR_WORK *p_wk, const GFL_POINT *cp_pos );
static BOOL CURSOR_GetTrg( const CURSOR_WORK *cp_wk );
static BOOL CURSOR_GetPointTrg( const CURSOR_WORK *cp_wk, u32 *p_x, u32 *p_y );
static int  CURSOR_GetContButton( const CURSOR_WORK *cp_wk );
static void CURSOR_SetTarget( CURSOR_WORK *p_wk, const PLACE_DATA *cp_data, const GFL_POINT *cp_wld );
static const PLACE_DATA * CURSOR_GetTarget( const CURSOR_WORK *cp_wk );
static void CURSOR_SetPullEnable( CURSOR_WORK *p_wk, BOOL on_off );
//-------------------------------------
///	PLACE
//=====================================
static void PLACE_Init( PLACE_WORK *p_wk, u16 now_zone_ID, u16 esc_zone_ID, const TOWNMAP_DATA *cp_data, const TOWNMAP_GRAPHIC_SYS *cp_grh, GAMEDATA *p_gamedata, HEAPID heapID, BOOL is_debug );
static void PLACE_Exit( PLACE_WORK *p_wk );
static void PLACE_Main( PLACE_WORK *p_wk );
static void PLACE_Scale( PLACE_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_next );
static void PLACE_SetVisible( PLACE_WORK *p_wk, BOOL is_visible );
static void PLACE_SetVisibleForce( PLACE_WORK *p_wk, BOOL is_visible );
static void PLACE_SetWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos );
static void PLACE_GetWldPos( const PLACE_WORK *cp_wk, GFL_POINT *p_pos );
static void PLACE_AddWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos );
static const PLACE_DATA* PLACE_Hit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor );
static const PLACE_DATA* PLACE_PullHit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor );
static s32 PLACEDATA_GetParam( const PLACE_DATA *cp_wk, TOWNMAP_DATA_PARAM param );
static void PLACE_Active( PLACE_WORK *p_wk, const PLACE_DATA *cp_data );
static void PLACE_NonActive( PLACE_WORK *p_wk );
static const PLACE_DATA *PLACE_GetDataByZoneID( const PLACE_WORK *cp_wk, u16 zoneID, u16 escapeID );
static BOOL PLACEDATA_IsVisiblePlaceName( const PLACE_DATA *cp_wk );
static void PlaceData_Init( PLACE_DATA *p_wk, const TOWNMAP_DATA *cp_data, u32 data_idx, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, GAMEDATA *p_gamedata, HEAPID heapID, BOOL is_debug );
static void PlaceData_Exit( PLACE_DATA *p_wk );
static void PlaceData_SetVisible( PLACE_DATA *p_wk, BOOL is_visible );
static void PlaceData_SetVisibleForce( PLACE_DATA *p_wk, BOOL is_visible );
static BOOL PlaceData_IsHit( const PLACE_DATA *cp_wk, const GFL_POINT *cp_pos, const GFL_POINT *cp_wld_pos );
static BOOL PlaceData_IsPullHit( const PLACE_DATA *cp_wk, const GFL_POINT *cp_pos, const GFL_POINT *cp_wld_pos, u32 *p_distance );
static void	PlaceData_Scale( PLACE_DATA *p_wk, fx32 scale, const GFL_POINT *cp_center_next );
static void PlaceData_SetPos(PLACE_DATA *p_wk, const GFL_POINT *cp_wld );
static void PlaceData_SetAnmSeq(PLACE_DATA *p_wk, u16 seq );
static void PlaceData_Active( PLACE_DATA *p_wk );
static void PlaceData_NonActive( PLACE_DATA *p_wk );
//-------------------------------------
///	PLACEMARK
//=====================================
static void PLACEMARK_Init( PLACE_MARK *p_wk, const PLACE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, HEAPID heapID );
static void PLACEMARK_Exit( PLACE_MARK *p_wk );
static void PLACEMARK_SetPos( PLACE_MARK *p_wk, const GFL_POINT *cp_wld );
static void PLACEMARK_SetVisible( PLACE_MARK *p_wk, BOOL is_visible );
//-------------------------------------
///	MAP
//=====================================
static void MAP_Init( MAP_WORK *p_wk, u8 map_frm, u8 road_frm, HEAPID heapID );
static void MAP_Exit( MAP_WORK *p_wk );
static void MAP_Main( MAP_WORK *p_wk, PLACE_WORK *p_place );
static void MAP_Scale( MAP_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_start, const GFL_POINT *cp_center_end );
static BOOL MAP_IsScaleEnd( const MAP_WORK *cp_wk );
static BOOL MAP_IsScale( const MAP_WORK *cp_wk );
static void MAP_SetWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos );
static void MAP_GetWldPos( const MAP_WORK *cp_wk, GFL_POINT *p_pos );
static void MAP_AddWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos );
static void MAP_SetVisibleRoadFrm( const MAP_WORK *cp_wk, BOOL is_visible );
//-------------------------------------
///	INFO
//=====================================
static void INFO_Init( INFO_WORK *p_wk, u8 frm, GFL_FONT *p_font, const GFL_MSGDATA *cp_place_msg, const GFL_MSGDATA *cp_guide_msg, HEAPID heapID );
static void INFO_Exit( INFO_WORK *p_wk );
static void INFO_Main( INFO_WORK *p_wk );
static void INFO_Update( INFO_WORK *p_wk, const PLACE_DATA *cp_data );
static void INFO_Clear( INFO_WORK *p_wk );
//-------------------------------------
///	MSGWND
//=====================================
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, GFL_FONT *p_font, const GFL_MSGDATA *cp_msg, u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCode( MSGWND_WORK* p_wk, const STRCODE *cp_code, u16 x, u16 y );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//-------------------------------------
///	PLACEWND
//=====================================
static void PLACEWND_Init( PLACEWND_WORK *p_wk, u8 bgframe, GFL_FONT *p_font, const GFL_MSGDATA *cp_msg, GFL_CLWK *p_clwk, HEAPID heapID );
static void PLACEWND_Exit( PLACEWND_WORK *p_wk );
static void PLACEWND_Main( PLACEWND_WORK *p_wk );
static void PLACEWND_Start( PLACEWND_WORK *p_wk, const PLACE_DATA *cp_data );
static void PLACEWND_SetVisible( PLACEWND_WORK *p_wk, BOOL is_visible );
//-------------------------------------
///	UI
//=====================================
static void UI_Init( UI_WORK *p_wk, HEAPID heapID );
static void UI_Exit( UI_WORK *p_wk );
static void UI_Main( UI_WORK *p_wk );
static BOOL UI_GetSlide( const UI_WORK *cp_wk, VecFx32 *p_vec, u32 *p_sync );
static void UI_ResetSlide( UI_WORK *p_wk );
//-------------------------------------
///	ETC
//=====================================
static void BMP_DrawLine( GFL_BMP_DATA *p_bmp, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u8 plt_num );
static void BMP_DrawBoldLine( GFL_BMP_DATA *p_bmp, const GFL_POINT *cp_start, const GFL_POINT *cp_end, int w, u8 plt_num );

static void TownMap_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs );
//=============================================================================
/**
 *						DEBUG_PRINT
 */
//=============================================================================
#ifdef DEBUG_PRINT_USE
//debug�p
#include "system/net_err.h"	//VRAM�ޔ�p�A�h���X��Ⴄ����
#include <wchar.h>					//wcslen
//-------------------------------------
///	�ۑ����@
//=====================================
typedef enum
{
	DEBUGPRINT_SAVE_TYPE_NONE,
	DEBUGPRINT_SAVE_TYPE_NETERR,
	DEBUGPRINT_SAVE_TYPE_HEAP,

} DEBUGPRINT_SAVE_TYPE;

//-------------------------------------
///	�f�o�b�O�v�����g�p���
//=====================================
typedef struct
{
	GFL_BMP_DATA *p_bmp;
	GFL_FONT*			p_font;

	DEBUGPRINT_SAVE_TYPE	save;
	u8	frm;
	u8	dummy[3];

  u8  *p_char_temp_area;      ///<�L�����N�^VRAM�ޔ��
  u16 *p_scrn_temp_area;      ///<�X�N���[��VRAM�ޔ��
  u16 *p_pltt_temp_area;      ///<�p���b�gVRAM�ޔ��

 	u8  font_col_bkup[3];
	u8	dummy2;
  u8  prioryty_bkup;
  u8  scroll_x_bkup;
  u8  scroll_y_bkup;
	u8	dummy3;

	BOOL is_open;

	HEAPID heapID;
} DEBUG_PRINT_WORK;

static DEBUG_PRINT_WORK *sp_dp_wk;

//NETERR�̕���0x4000�܂ł����m�ۂł��܂���
//#define DEBUGPRINT_CHAR_TEMP_AREA (NETERR_PUSH_CHARVRAM_SIZE)
#define DEBUGPRINT_CHAR_TEMP_AREA (0x8000)
#define DEBUGPRINT_SCRN_TEMP_AREA (NETERR_PUSH_SCRNVRAM_SIZE)
#define DEBUGPRINT_PLTT_TEMP_AREA (NETERR_PUSH_PLTTVRAM_SIZE)
#define DEBUGPRINT_WIDTH  (32)
//#define DEBUGPRINT_HEIGHT (18)
#define DEBUGPRINT_HEIGHT (24)

static const u16 sc_debug_print_plt[0xF]	=
{	
	0xFFFF,0xFFFF,0x7fff,0x001F,0
};

static DEBUG_PRINT_WORK * DEBUGPRINT_Init( u8 frm, DEBUGPRINT_SAVE_TYPE	save, HEAPID heapID );
static void DEBUGPRINT_Exit( DEBUG_PRINT_WORK *p_wk );
static void DEBUGPRINT_Open( DEBUG_PRINT_WORK *p_wk );
static BOOL DEBUGPRINT_IsOpen( DEBUG_PRINT_WORK *p_wk );
static void DEBUGPRINT_Close( DEBUG_PRINT_WORK *p_wk );
static void DEBUGPRINT_Print( DEBUG_PRINT_WORK *p_wk, const u16 *cp_str, u16 x, u16 y );
static void DEBUGPRINT_PrintNumber( DEBUG_PRINT_WORK *p_wk, const u16 *cp_str, int number, u16 x, u16 y );
static void DEBUGPRINT_Clear( DEBUG_PRINT_WORK *p_wk );
static void DEBUGPRINT_ClearRange( DEBUG_PRINT_WORK *p_wk, s16 x, s16 y, u16 w, u16 h );
//�g��
static void DEBUGPRINT_Update( DEBUG_PRINT_WORK *p_print, TOWNMAP_WORK *p_wk );
#else
#define DEBUGPRINT_Init(...)				((void)0)
#define DEBUGPRINT_Exit(...)				((void)0)
#define DEBUGPRINT_Open(...)				((void)0)
#define DEBUGPRINT_IsOpen(...)			((void)0)
#define DEBUGPRINT_Close(...)				((void)0)
#define DEBUGPRINT_Print(...)				((void)0)
#define DEBUGPRINT_PrintNumber(...) ((void)0)
#define DEBUGPRINT_Clear(...)				((void)0)
#define DEBUGPRINT_ClearRange(...)	((void)0)
#define DEBUGPRINT_Update(...)			((void)0)
#endif //DEBUG_PRINT_USE

//=============================================================================
/**
 *						DEBUG_MENU
 */
//=============================================================================
#ifdef DEBUG_PRINT_USE
static void DEBUGPRINT_POS_Update( DEBUG_PRINT_WORK *p_print, TOWNMAP_WORK *p_wk );
#else
#define DEBUGPRINT_POS_Update(...)			((void)0)
#endif //DEBUG_PRINT_USE

//=============================================================================
/**
 *						DEBUG_MENU
 */
//=============================================================================
#ifdef DEBUG_MENU_USE
static void DEBUGMENU_UPDATE_Print( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_Print( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_CheckPos( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_CheckPos( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_CheckPosNum( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_CheckPosNum( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_NotPullFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_NotPullFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
#endif //DEBUG_MENU_USE

//=============================================================================
/**
 *						DATA
 */
//=============================================================================
//=============================================================================
/**
 *						GLOBAL
 */
//=============================================================================
//-------------------------------------
///	PROC�f�[�^�O�����J
//=====================================
const GFL_PROC_DATA	TownMap_ProcData	=
{
	TOWNMAP_PROC_Init,
	TOWNMAP_PROC_Main,
	TOWNMAP_PROC_Exit,
};
//=============================================================================
/**
 *						PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PROC������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  BOOL is_debug;

	TOWNMAP_WORK	*p_wk;
	TOWNMAP_PARAM	*p_param	= p_param_adrs;
	u16	data_len;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TOWNMAP, 0x60000 );

	//���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(TOWNMAP_WORK), HEAPID_TOWNMAP );
	GFL_STD_MemClear( p_wk, sizeof(TOWNMAP_WORK) );
	p_wk->p_param	= p_param;

  is_debug  = p_wk->p_param->mode == TOWNMAP_MODE_DEBUGSKY;

	//���ʃV�X�e���쐬---------------------
	//�t�H���g
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
    NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_TOWNMAP );

	//�n�����b�Z�[�W�f�[�^
	p_wk->p_place_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, HEAPID_TOWNMAP );
	//�K�C�h���b�Z�[�W�f�[�^
	p_wk->p_guide_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_townmap_dat, HEAPID_TOWNMAP );

	//���W���[���쐬----------------------
	//�O���t�B�b�N�쐬
	p_wk->p_grh	= TOWNMAP_GRAPHIC_Init( HEAPID_TOWNMAP );

	//�}�b�v�f�[�^�쐬
	p_wk->p_data	= TOWNMAP_DATA_Alloc( HEAPID_TOWNMAP );

#ifdef PM_DEBUG
  { 
    int i;

    u16 zoneID;
    u16 hide_flag;
    for( i = 0; i < TOWNMAP_DATA_MAX; i++ )
    { 
      zoneID    = TOWNMAP_DATA_GetParam( p_wk->p_data, i, TOWNMAP_DATA_PARAM_ZONE_ID );
      hide_flag = TOWNMAP_DATA_GetParam( p_wk->p_data, i, TOWNMAP_DATA_PARAM_HIDE_FLAG );
      NAGI_Printf( "ID%d hide%d\n", zoneID, hide_flag );
    }
  }
#endif

	//�V�[�P���X�쐬
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );

	//�A�v���P�[�V�����o�[�쐬
	{	
		GAMEDATA	*p_gdata;

		p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
		if( p_wk->p_param->mode == TOWNMAP_MODE_MAP )
		{	
		}
		else
		{	

			//����ԃ��[�h�Ȃ�΃��b�Z�[�W�ʒǉ�
			MSGWND_Init( &p_wk->msgwnd,
					TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_M),
					p_wk->p_font,
					p_wk->p_guide_msg,
					MSGWND_BMPWIN_SKY_X,
					MSGWND_BMPWIN_SKY_Y,
					MSGWND_BMPWIN_SKY_W,
					MSGWND_BMPWIN_SKY_H,
					HEAPID_TOWNMAP
					);

			GFL_FONTSYS_SetColor( 0xf, 1, 0 );
			MSGWND_Print( &p_wk->msgwnd, 0, MSGWND_STR_SKY_X, MSGWND_STR_SKY_Y );
			GFL_FONTSYS_SetDefaultColor();
		}

		APPBAR_Init( &p_wk->appbar,p_wk->p_param->mode,
				TOWNMAP_GRAPHIC_GetUnit( p_wk->p_grh, TOWNMAP_OBJ_CLUNIT_DEFAULT ),
				TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_BAR_M),
				TOWNMAP_BG_PAL_M_15, TOWNMAP_OBJ_PAL_M_08, p_gdata,
				HEAPID_TOWNMAP );
	}

	//�n���E�B���h�E�쐬
	PLACEWND_Init( &p_wk->placewnd, 
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_M),
			p_wk->p_font, p_wk->p_place_msg, 
			TOWNMAP_GRAPHIC_GetClwk( p_wk->p_grh, TOWNMAP_OBJ_CLWK_WINDOW ),
			HEAPID_TOWNMAP );
	//FONT�ʂ����炷
	GFL_BG_SetScroll( TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_M),
					GFL_BG_SCROLL_Y_SET, -4 );

	//�ꏊ�쐬
	{	
		GAMEDATA	*p_gdata;
#ifdef DEBUG_GAMESYS_NONE
		if( p_wk->p_param->p_gamesys == NULL )
		{	
			p_gdata = NULL;
		}
		else
		{	
			p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
		}
#else
		p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
#endif //DEBUG_GAMESYS_NONE
		PLACE_Init( &p_wk->place, p_param->zoneID, p_param->escapeID, p_wk->p_data, p_wk->p_grh,
				p_gdata, HEAPID_TOWNMAP, is_debug );
	}

	//�J�[�\���쐬
	CURSOR_Init( &p_wk->cursor, TOWNMAP_GRAPHIC_GetClwk( p_wk->p_grh, TOWNMAP_OBJ_CLWK_CURSOR ),
			TOWNMAP_GRAPHIC_GetClwk( p_wk->p_grh, TOWNMAP_OBJ_CLWK_RING_CUR ), HEAPID_TOWNMAP );
	{	
		const PLACE_DATA *cp_data	= NULL;
		GFL_POINT pos;
		cp_data	= PLACE_GetDataByZoneID( &p_wk->place, p_param->zoneID, p_param->escapeID );
		if( cp_data )
		{	
			pos.x	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_CURSOR_X );
			pos.y	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_CURSOR_Y );
		}
		else
		{	
			pos.x	= 128;
			pos.y	= 96;
		}
		CURSOR_SetPos( &p_wk->cursor, &pos );
	}

	//�n�}�쐬
	MAP_Init( &p_wk->map,
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_MAP_M ),
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_ROAD_M ),
			HEAPID_TOWNMAP );

	//���ʏ��쐬
	INFO_Init( &p_wk->info, TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_S ),
			p_wk->p_font, p_wk->p_place_msg, p_wk->p_guide_msg, HEAPID_TOWNMAP );

	//�^�b�`
	UI_Init( &p_wk->ui, HEAPID_TOWNMAP );


  //��������
  {
    const PLACE_DATA *cp_data;
    cp_data	= PLACE_Hit( &p_wk->place, &p_wk->cursor );
    if( cp_data != NULL )
    {
      PLACEWND_Start( &p_wk->placewnd, cp_data );
      INFO_Update( &p_wk->info, cp_data );
      p_wk->cp_select	= cp_data;
      {
        GFL_POINT wld;
        PLACE_GetWldPos(&p_wk->place, &wld );
				CURSOR_SetTarget( &p_wk->cursor, cp_data, &wld );
      }
    }
  }

#ifdef DEBUG_MENU_USE

	//�f�o�b�O���j���[
	DEBUGWIN_InitProc( TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_DEBUG_S ),
			p_wk->p_font );
	DEBUGWIN_ChangeLetterColor( 0, 0, 31 );
	DEBUGWIN_ChangeSelectColor( 31, 0, 31 );
	DEBUGWIN_AddGroupToTop( 0 , "�^�E���}�b�v" , HEAPID_TOWNMAP );
#endif //DEBUG_MENU_USE

	//�f�o�b�O�\��
#ifdef DEBUG_PRINT_USE
	sp_dp_wk	= DEBUGPRINT_Init( TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_M ), 
			DEBUGPRINT_SAVE_TYPE_HEAP, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_Print, DEBUGMENU_DRAW_Print, p_wk, 0, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_ArriveFlag, DEBUGMENU_DRAW_ArriveFlag, p_wk, 0, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_NotPullFlag, DEBUGMENU_DRAW_NotPullFlag, p_wk, 0, HEAPID_TOWNMAP );
//	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_VisiblePlace, DEBUGMENU_DRAW_VisiblePlace, p_wk, 0, HEAPID_TOWNMAP );

#ifdef DEBUG_POS_CHECK
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_CheckPos, DEBUGMENU_DRAW_CheckPos, p_wk, 0, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_CheckPosNum, DEBUGMENU_DRAW_CheckPosNum, p_wk, 0, HEAPID_TOWNMAP );
#endif //DEBUG_POS_CHECK

#endif //DEBUG_PRINT_USE

  if( GFL_NET_IsInit() )
  { 
    GFL_NET_ChangeIconPosition(GFL_WICON_POSX,GFL_WICON_POSY);
    GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_TOWNMAP );
    GFL_NET_ReloadIcon();
  }

  p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(TownMap_VBlankTask, p_wk, 0 );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;


  //VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���W���[���j��-------------

	//�f�o�b�O�j��
#ifdef DEBUG_PRINT_USE
	DEBUGPRINT_Exit(sp_dp_wk);
#endif //DEBUG_PRINT_USE
#ifdef DEBUG_MENU_USE
	DEBUGWIN_RemoveGroup( 0 );
	DEBUGWIN_ExitProc();
#endif //DEBUG_MENU_USE

	//�^�b�`�j��
	UI_Exit( &p_wk->ui );

	//���ʏ��j��
	INFO_Exit( &p_wk->info );

	//�n�}�j��
	MAP_Exit( &p_wk->map );

	//�ꏊ�j��
	PLACE_Exit( &p_wk->place );

	//�J�[�\���j��
	CURSOR_Exit( &p_wk->cursor );

	//�n���E�B���h�E�j��
	PLACEWND_Exit( &p_wk->placewnd );

	//����ԃ��[�h�Ȃ�΃��b�Z�[�W�ʔj��
	if( p_wk->p_param->mode != TOWNMAP_MODE_MAP )
	{	
		MSGWND_Exit( &p_wk->msgwnd );
	}
	//�A�v���P�[�V�����o�[�j��
	APPBAR_Exit( &p_wk->appbar );

	//�V�[�P���X�j��
	SEQ_Exit( &p_wk->seq );	

	//�}�b�v�f�[�^�j��
	TOWNMAP_DATA_Free( p_wk->p_data );

	//�O���t�B�b�N�j��
	TOWNMAP_GRAPHIC_Exit( p_wk->p_grh );

	//���ʃV�X�e���j��-------------
	//���b�Z�[�W�j��
	GFL_MSG_Delete( p_wk->p_guide_msg );
	GFL_MSG_Delete( p_wk->p_place_msg );

	//�t�H���g�j��
	GFL_FONT_Delete( p_wk->p_font );

	//���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap(HEAPID_TOWNMAP );

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�`��
	TOWNMAP_GRAPHIC_Draw( p_wk->p_grh );

#ifdef PM_DEBUG
#if 0
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
	{	
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
		{	
			s_debug_pos.y--;
			NAGI_Printf( "debug pos x%d y%d\n", s_debug_pos.x, s_debug_pos.y );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
		{	
			s_debug_pos.y++;
			NAGI_Printf( "debug pos x%d y%d\n", s_debug_pos.x, s_debug_pos.y );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
		{	
			s_debug_pos.x--;
			NAGI_Printf( "debug pos x%d y%d\n", s_debug_pos.x, s_debug_pos.y );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
		{	
			s_debug_pos.x++;
			NAGI_Printf( "debug pos x%d y%d\n", s_debug_pos.x, s_debug_pos.y );
		}
	}
#endif
#endif //PM_DEBUG

	//�I��
	if( SEQ_IsEnd( &p_wk->seq ) )
	{	
		return GFL_PROC_RES_FINISH;
	}
	else
	{	
		return GFL_PROC_RES_CONTINUE;
	}
}
//=============================================================================
/**
 *						SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	*p_param_adrs		�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param_adrs, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_param_adrs	= p_param_adrs;

	//�Z�b�g
	SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Main( SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_param_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *					SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�hOUT
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	TOWNMAP_WORK	*p_wk	= p_param_adrs;

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		SEQ_SetNext( p_seqwk, SEQFUNC_Main );
		break;

	default:
		GF_ASSERT(0);
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�hIN
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	TOWNMAP_WORK	*p_wk	= p_param_adrs;

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		SEQ_End( p_seqwk );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_param_adrs;
	BOOL is_map_move	= FALSE;

	//�J�[�\���̒n�}�ړ�
	if( p_wk->is_scale && !MAP_IsScale( &p_wk->map ) )
	{	
		GFL_POINT	pos;
		GFL_POINT	add	= {0,0};
		int btn;
		CURSOR_GetPos( &p_wk->cursor, &pos );
		btn	= CURSOR_GetContButton( &p_wk->cursor );

		//�J�[�\������
		if( pos.y < CURSOR_MAP_MOVE_RANGE_TOP )
		{	
			if( btn & PAD_KEY_UP )
			{	
				add.y -= 4;
				is_map_move	= TRUE;
			}
		}
		if( CURSOR_MAP_MOVE_RANGE_BOTTOM_APPBAR >= pos.y && pos.y > CURSOR_MAP_MOVE_RANGE_BOTTOM )
		{	
			if( btn & PAD_KEY_DOWN )
			{	
				add.y += 4;
				is_map_move	= TRUE;
			}
		}
		if( pos.x < CURSOR_MAP_MOVE_RANGE_LEFT )
		{	
			if( btn & PAD_KEY_LEFT )
			{	
				add.x -= 4;
				is_map_move	= TRUE;
			}
		}
		if( pos.x > CURSOR_MAP_MOVE_RANGE_RIGHT )
		{	
			if( btn & PAD_KEY_RIGHT )
			{	
				add.x += 4;
				is_map_move	= TRUE;
			}
		}
		//�X���C�h�ł̓���
		if( !MAP_IsScale( &p_wk->map ) )
		{	
			BOOL is_slide;
			VecFx32	v;
			u32 sync;

			is_slide	= UI_GetSlide( &p_wk->ui, &v, &sync );
			if( is_slide )
			{	
				add.x	= -FX_Mul(v.x, UI_DRAG_MOVE_RATE)>> FX32_SHIFT;
				add.y	= -FX_Mul(v.y, UI_DRAG_MOVE_RATE)>> FX32_SHIFT;

				UI_ResetSlide( &p_wk->ui );

				is_map_move	= TRUE;
			}
		}

		//�ړ�����
		if( is_map_move )
		{	
			GFL_POINT	pos;
			PLACE_GetWldPos( &p_wk->place, &pos );

			MAP_AddWldPos( &p_wk->map, &add );
			//OBJ��BG�͕������t
			add.x	*= -1;
			add.y *= -1;
			PLACE_AddWldPos( &p_wk->place, &add );


      //�ړ����͑I������Ȃ�
      if( p_wk->cp_select )
      { 
        INFO_Clear( &p_wk->info );
        PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
        PLACE_NonActive( &p_wk->place );
        CURSOR_SetTarget( &p_wk->cursor, NULL, NULL );
        p_wk->cp_select	= NULL;
      }
		}
	}

	//�J�[�\���̏ꏊ�����蔻��
	//�}�b�v�ړ����͓����蔻�肵�Ȃ�
	if( !is_map_move && !MAP_IsScale( &p_wk->map ) )
	{	
    BOOL select_enable  = FALSE;
		//�I������
		if( p_wk->p_param->mode == TOWNMAP_MODE_SKY || p_wk->p_param->mode == TOWNMAP_MODE_DEBUGSKY )
		{
			const PLACE_DATA *cp_data;
			const PLACE_DATA *cp_now;
			cp_data	= CURSOR_GetTarget( &p_wk->cursor );
			cp_now	= PLACE_Hit( &p_wk->place, &p_wk->cursor );

			if( cp_data != NULL && cp_data == cp_now && CURSOR_GetTrg( &p_wk->cursor )
					&& PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_SKY_FLAG  ) )
			{	
				p_wk->p_param->select	= TOWNMAP_SELECT_SKY;
				p_wk->p_param->zoneID	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID );
				p_wk->p_param->grid.x	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_WARP_X );
				p_wk->p_param->grid.y	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_WARP_Y );

				PMSND_PlaySE( TOWNMAP_SE_DECIDE );
				SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
			}
		}

		//�����蔻��
    //�^�b�`���[�h�Ȃ�΁A�^�b�`���Ȃ��Ƃ����蔻�肵�Ȃ�
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
    { 
      select_enable = TRUE;
    }
    else
    { 
      select_enable = CURSOR_GetTrg( &p_wk->cursor );
    }


    if( select_enable )
		{	
			const PLACE_DATA *cp_data;
			cp_data	= PLACE_Hit( &p_wk->place, &p_wk->cursor );
			if( cp_data != p_wk->cp_select && cp_data != NULL )
			{	
				GFL_POINT wld;

				PMSND_PlaySE( TOWNMAP_SE_SELECT );
				INFO_Update( &p_wk->info, cp_data );
				PLACE_Active( &p_wk->place, cp_data );
				p_wk->cp_select	= cp_data;

				PLACE_GetWldPos(&p_wk->place, &wld );
				CURSOR_SetTarget( &p_wk->cursor, cp_data, &wld );

				//�g�咆�͂łȂ�
				if( !p_wk->is_scale )
				{	
					PLACEWND_Start( &p_wk->placewnd, cp_data );
				}
			}
			else if( cp_data == NULL && p_wk->cp_select )
			{	
				INFO_Clear( &p_wk->info );
				PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
				PLACE_NonActive( &p_wk->place );
				CURSOR_SetTarget( &p_wk->cursor, NULL, NULL );
				p_wk->cp_select	= NULL;
			}
		}
	}


	//�g�咆�͂łȂ�
	if( p_wk->is_scale )
	{	
		PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
	}

	//�}�b�v���蒆�͈�������Ȃ�
	CURSOR_SetPullEnable( &p_wk->cursor, !is_map_move );

	///���W���[�����C������
	APPBAR_Main( &p_wk->appbar, &p_wk->map );

	if( !MAP_IsScale( &p_wk->map ) )
	{	
		CURSOR_Main( &p_wk->cursor, &p_wk->place );
	}
	PLACE_Main( &p_wk->place );
	MAP_Main( &p_wk->map, &p_wk->place );
	PLACEWND_Main( &p_wk->placewnd );
	UI_Main( &p_wk->ui );

	//�A�v���P�[�V�����o�[�I��
	switch( APPBAR_GetTrg( &p_wk->appbar ) )
	{	
	case TOUCHBAR_ICON_CLOSE:
		p_wk->p_param->select	= TOWNMAP_SELECT_CLOSE;

		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
		break;
	case TOUCHBAR_ICON_RETURN:
		p_wk->p_param->select	= TOWNMAP_SELECT_RETURN;

		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
		break;
	case TOUCHBAR_ICON_SCALE:
		if(p_wk->is_scale == FALSE && !MAP_IsScale( &p_wk->map ))
		{	
			GFL_POINT next_pos;
			CURSOR_GetPos( &p_wk->cursor, &next_pos );

      //�g��
			MAP_Scale( &p_wk->map, FX32_CONST(2), &sc_center_pos, &next_pos );


			PMSND_PlaySE( TOWNMAP_SE_SCALEUP );

      //�ړ����͑I������Ȃ�
      if( p_wk->cp_select )
      { 
        INFO_Clear( &p_wk->info );
        PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
        PLACE_NonActive( &p_wk->place );
        CURSOR_SetTarget( &p_wk->cursor, NULL, NULL );
        p_wk->cp_select	= NULL;
      }

			//PLACE_SetVisibleForce( &p_wk->place, FALSE );

			p_wk->is_scale	= TRUE;
		}
    else 
		if(p_wk->is_scale == TRUE && !MAP_IsScale( &p_wk->map ) )
		{	
			GFL_POINT src_pos;
			MAP_GetWldPos( &p_wk->map, &src_pos );
			//NAGI_Printf( "WLD X%d Y%d\n", src_pos.x, src_pos.y );
			src_pos.x	= src_pos.x + 256/2;
			src_pos.y	= src_pos.y + 192/2;

      //�k��
			MAP_Scale( &p_wk->map, FX32_CONST(1), &src_pos, &sc_center_pos );
			

			PMSND_PlaySE( TOWNMAP_SE_SCALEDOWN );

      //�ړ����͑I������Ȃ�
      if( p_wk->cp_select )
      { 
        INFO_Clear( &p_wk->info );
        PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
        PLACE_NonActive( &p_wk->place );
        CURSOR_SetTarget( &p_wk->cursor, NULL, NULL );
        p_wk->cp_select	= NULL;
      }			
      
      //PLACE_SetVisibleForce( &p_wk->place, FALSE );

			p_wk->is_scale	= FALSE;
		}
		break;
	}

	//�X�P�[���I����̏ꏊ�\������
	if( MAP_IsScaleEnd( &p_wk->map ) )
	{	
//    PLACE_SetVisibleForce( &p_wk->place, TRUE );
//    p_wk->is_scale_end_req  = TRUE;
	}
}
//=============================================================================
/**
 *	APPBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	������
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	p_unit						APPBAR��OBJ�����pp_unit
 *	@param	bar_frm						�g�p����BG��
 *	@param	bg_plt						�g�p����BG�p���b�g�ԍ�
 *	@param	obj_plt						�g�p����OBJ�p���b�g�ԍ�
 *	@param	cp_gamedata				�Q�[���f�[�^�iY�{�^���o�^�m�F�p�j
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Init( APPBAR_WORK *p_wk, TOWNMAP_MODE mode, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, GAMEDATA *p_gamedata, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
	p_wk->p_gamedata	= p_gamedata;

	//�J�X�^���{�^���̃��\�[�X
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

		p_wk->res[APPBAR_RES_SCALE_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				NARC_townmap_gra_tmap_obj_NCLR, CLSYS_DRAW_MAIN, (0)*0x20,
				0, 4, heapID );	

		p_wk->res[APPBAR_RES_SCALE_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_townmap_gra_tmap_obj_d_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->res[APPBAR_RES_SCALE_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_townmap_gra_tmap_obj_d_NCER,
				NARC_townmap_gra_tmap_obj_d_NANR, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

  //�X�P�[���{�^��
  { 
    GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = TOUCHBAR_ICON_X_05;
    cldata.pos_y  = TOUCHBAR_ICON_Y;
    cldata.anmseq = 7;
    p_wk->p_scale = GFL_CLACT_WK_Create( p_unit, p_wk->res[APPBAR_RES_SCALE_CHR],
        p_wk->res[APPBAR_RES_SCALE_PLT], p_wk->res[APPBAR_RES_SCALE_CEL], &cldata, 0, heapID );

    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_scale, TRUE );
    GFL_CLACT_WK_StopAnm( p_wk->p_scale );
  }

	//�^�b�`�o�[������
	{	
		TOUCHBAR_SETUP	touchbar_setup;
		TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
		{	
			{	
				TOUCHBAR_ICON_RETURN,
				{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
			},
			{	
				TOUCHBAR_ICON_CLOSE,
				{	TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
			},
		};
	
		//�ݒ�\����
		//�����قǂ̑����{���\�[�X���������
		GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

		touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
		touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
		touchbar_setup.p_unit		= p_unit;							//OBJ�ǂݍ��݂̂��߂�CLUNIT
		touchbar_setup.bar_frm	= bar_frm;						//BG�ǂݍ��݂̂��߂�BG��
		touchbar_setup.bg_plt		= bg_plt;			//BG��گ�
		touchbar_setup.obj_plt	= obj_plt + 4;		//OBJ��گ�
		touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

		p_wk->p_touchbar	= TOUCHBAR_Init( &touchbar_setup, heapID );

    TOUCHBAR_SetSoftPriorityAll( p_wk->p_touchbar, 10 );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�j��
 *
 *	@param	APPBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Exit( APPBAR_WORK *p_wk )
{	
	//�^�b�`�o�[�j��
	TOUCHBAR_Exit( p_wk->p_touchbar );

  { 
    GFL_CLACT_WK_Remove( p_wk->p_scale );
  }
  
	//OBJ
	{	
		//�J�X�^���{�^�����\�[�X�j��
		GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_SCALE_CEL] );
		GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_SCALE_CHR] );
		GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_SCALE_PLT] );
	}

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );

}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	���C������
 *
 *	@param	APPBAR_WORK *p_wk			���[�N
 *	@param	CURSOR_WORK	cp_cursor	�J�[�\���Ń{�^���������̂����������邽��
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Main( APPBAR_WORK *p_wk, const MAP_WORK *cp_map )
{	
	TOUCHBAR_Main( p_wk->p_touchbar );
	
  p_wk->icon  = TOUCHBAR_SELECT_NONE;
  { 
    enum
    { 
      SEQ_TOUCH,
      SEQ_ANM,
    };

    switch( p_wk->seq )
    { 
    case SEQ_TOUCH:
      { 
        u32 x, y;
        GFL_CLACTPOS pos;
        BOOL is_push  = FALSE;

        GFL_CLACT_WK_GetPos( p_wk->p_scale, &pos, 0 );

        if( GFL_UI_TP_GetPointTrg( &x, &y ) )
        { 
          if( ((u32)( x - pos.x) <= (u32)(TOUCHBAR_ICON_WIDTH))
              &	((u32)( y - pos.y) <= (u32)(TOUCHBAR_ICON_HEIGHT)) )
          { 
            GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
            is_push = TRUE;
          }
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
        { 
          GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
          is_push = TRUE;
        }

        //�^�b�`����
        if( is_push && !GFL_CLACT_WK_CheckAnmActive( p_wk->p_scale ) && !MAP_IsScale(cp_map) )
        {	
          if( GFL_CLACT_WK_GetAnmSeq( p_wk->p_scale) == 7 ) //�g��
          { 
            GFL_CLACT_WK_SetAnmSeq( p_wk->p_scale, 9 );
            GFL_CLACT_WK_StartAnm( p_wk->p_scale );
          
          }
          else
          { 
            GFL_CLACT_WK_SetAnmSeq( p_wk->p_scale, 10 );
            GFL_CLACT_WK_StartAnm( p_wk->p_scale );
          }
          p_wk->icon  = TOUCHBAR_ICON_SCALE;
          p_wk->seq = SEQ_ANM;
        }
      }
      break;

    case SEQ_ANM:
      if( !GFL_CLACT_WK_CheckAnmActive( p_wk->p_scale ) )
      { 
        if( GFL_CLACT_WK_GetAnmSeq( p_wk->p_scale) == 9 )
        { 
          GFL_CLACT_WK_SetAnmSeq( p_wk->p_scale, 8 );
        }
        else
        { 
          GFL_CLACT_WK_SetAnmSeq( p_wk->p_scale, 7 );
        }

        p_wk->seq = SEQ_TOUCH;
      }
      break;
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_ICON APPBAR_GetTrg( const APPBAR_WORK *cp_wk )
{	
	TOUCHBAR_ICON	icon;
	icon	= TOUCHBAR_GetTrg( cp_wk->p_touchbar );
  if( icon == TOUCHBAR_SELECT_NONE )
  { 
    icon  = cp_wk->icon;
  }
	return icon;
}
//=============================================================================
/**
 *	CURSOR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\��������
 *
 *	@param	CURSOR_WORK *p_wk	���[�N
 *	@param	*p_cursor					�J�[�\��CLWK
 *	@param	*p_ring						�����OCLWK
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void CURSOR_Init( CURSOR_WORK *p_wk, GFL_CLWK *p_cursor, GFL_CLWK *p_ring, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(CURSOR_WORK) );
	p_wk->p_clwk[CURSOR_CLWK_CURSOR]	= p_cursor;
	p_wk->p_clwk[CURSOR_CLWK_RING]		= p_ring;
	p_wk->pos.x	= CURSOR_POS_DEFAULT_X;
	p_wk->pos.y	= CURSOR_POS_DEFAULT_Y;

	{
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_CURSOR], &p_wk->pos, 0 );
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_RING], &p_wk->pos, 0 );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_RING], FALSE );

		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[CURSOR_CLWK_CURSOR],	TOWNMAP_BG_PRIORITY_TOP_M );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[CURSOR_CLWK_CURSOR], OBJ_PRIORITY_CURSOR );

		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[CURSOR_CLWK_RING],	TOWNMAP_BG_PRIORITY_BAR_M+1 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[CURSOR_CLWK_RING], OBJ_PRIORITY_EFFECT );

		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[CURSOR_CLWK_RING], TRUE );
	}

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\���j��
 *
 *	@param	CURSOR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void CURSOR_Exit( CURSOR_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(CURSOR_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\�����C������
 *
 *	@param	CURSOR_WORK *p_wk		���[�N
 *	@param	p_place	�z�����ޏ����̂��ߏ������炤
 *
 */
//-----------------------------------------------------------------------------
static void CURSOR_Main( CURSOR_WORK *p_wk, const PLACE_WORK *cp_place )
{	
	VecFx32	norm	= {0,0,0};
	VecFx32	pull	= {0,0,0};
	BOOL is_move	= FALSE;

	//���̖��t���[���N���A����
	p_wk->is_trg	= FALSE;

	//�J�[�\���ړ�����
	if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
	{	
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		norm.y	-= FX32_ONE;
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
	{	
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		norm.y	+= FX32_ONE;
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{	
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		norm.x	-= FX32_ONE;
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{	
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		norm.x	+= FX32_ONE;
		is_move	= TRUE;
	}

	//�z�����܂�鏈��
	//�z�����ݔ͈͓��Ȃ�΋z�����܂��
#ifdef DEBUG_NOT_PULL
  if( !s_is_notpull_debug &&  p_wk->is_pull && GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
#else
	if( p_wk->is_pull && GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
#endif
	{	
		const PLACE_DATA* cp_placedata;
		cp_placedata	= PLACE_PullHit( cp_place, p_wk );
		if( cp_placedata != NULL )
		{	
			
			GFL_POINT	wld_pos;
			VecFx32	place_pos;
			VecFx32	now_pos;

			PLACE_GetWldPos( cp_place, &wld_pos );
			place_pos.x	= FX32_CONST(PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_CURSOR_X )+wld_pos.x);
			place_pos.y	= FX32_CONST(PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_CURSOR_Y )+wld_pos.y);
			place_pos.z	= 0;

			now_pos.x	= FX32_CONST(p_wk->pos.x);
			now_pos.y	= FX32_CONST(p_wk->pos.y);
			now_pos.z	= 0;

			VEC_Subtract( &place_pos, &now_pos, &pull );

			//�������ȏ�̋����Ȃ�΂��̂܂�
			if( VEC_Mag(&pull) > PLACE_PULL_STRONG )
			{	
				VEC_Normalize( &pull, &pull );
				GFL_CALC3D_VEC_MulScalar( &pull, PLACE_PULL_STRONG, &pull );
			}
			is_move	= TRUE;
		}
	}

	//�ړ��v�Z
	if( is_move )
	{	
		VecFx32	add;
		VEC_Normalize( &norm, &norm );

		add.x	= (norm.x * CURSOR_ADD_SPEED)	+ pull.x;
		add.y	= (norm.y * CURSOR_ADD_SPEED)	+ pull.y;
		add.z	=	0;

		if( 0 < add.x && add.x < FX32_ONE )
		{	
			add.x	= FX32_ONE;
		}
		else if( -FX32_ONE < add.x && add.x < 0 )
		{	
			add.x = -FX32_ONE;
		}
		if( 0 < add.y && add.y < FX32_ONE )
		{	
			add.y	= FX32_ONE;
		}
		else if( -FX32_ONE < add.y && add.y < 0 )
		{	
			add.y = -FX32_ONE;
		}

		p_wk->pos.x	+= add.x >> FX32_SHIFT; 
		p_wk->pos.y	+= add.y >> FX32_SHIFT;
		p_wk->pos.x	= MATH_CLAMP( p_wk->pos.x, CURSOR_MOVE_LIMIT_LEFT, CURSOR_MOVE_LIMIT_RIGHT );
		p_wk->pos.y	= MATH_CLAMP( p_wk->pos.y, CURSOR_MOVE_LIMIT_TOP, CURSOR_MOVE_LIMIT_BOTTOM );
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_CURSOR], &p_wk->pos, 0 );
	}

	//����
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
	{	

    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		p_wk->is_trg	= TRUE;
	}

	//�^�b�`����
	{	
		u32 x, y;
		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

			//�^�b�`�͈͂̒��Ȃ��
			if( ((u32)( x - CURSOR_MOVE_LIMIT_LEFT) <= (u32)(CURSOR_MOVE_LIMIT_RIGHT - CURSOR_MOVE_LIMIT_LEFT))
					&	((u32)( y - CURSOR_MOVE_LIMIT_TOP) <= (u32)(CURSOR_MOVE_LIMIT_BOTTOM - CURSOR_MOVE_LIMIT_TOP)))
			{	
				p_wk->is_trg	= TRUE;
				p_wk->pos.x	= x;
				p_wk->pos.y	= y;
			}
		}
	}

#ifdef PM_DEBUG
  s_cursor_pos.x  = p_wk->pos.x;
  s_cursor_pos.y  = p_wk->pos.y;
#endif 


  //�^�b�`or�L�[���[�h�̃J�[�\��ON,OFF
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\���ʒu���擾
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 *	@param	*p_pos										�ʒu�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void CURSOR_GetPos( const CURSOR_WORK *cp_wk, GFL_POINT *p_pos )
{	
	p_pos->x	= cp_wk->pos.x;
	p_pos->y	= cp_wk->pos.y;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\���ʒu��ݒ�
 *
 *	@param	CURSOR_WORK *p_wk		���[�N
 *	@param	GFL_POINT *cp_pos		�ݒ�ʒu
 */
//-----------------------------------------------------------------------------
static void CURSOR_SetPos( CURSOR_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	p_wk->pos.x	= cp_pos->x;
	p_wk->pos.y	= cp_pos->y;
	GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_CURSOR], &p_wk->pos, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\��������擾
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�ŉ�����	FALSE�ŉ����ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL CURSOR_GetTrg( const CURSOR_WORK *cp_wk )
{	
	return cp_wk->is_trg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	������Ԃ��擾
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 *	@param	*p_x		X�󂯎��
 *	@param	*p_y		Y�󂯎��
 *
 *	@return	TRUE�ŉ�����	FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL CURSOR_GetPointTrg( const CURSOR_WORK *cp_wk, u32 *p_x, u32 *p_y )
{	
	if( CURSOR_GetTrg( cp_wk ) )
	{	
		GFL_POINT	pos;
		CURSOR_GetPos( cp_wk, &pos );
		if( p_x )
		{	
			*p_x	= pos.x;
		}
		if( p_y )
		{	
			*p_y	= pos.y;
		}
		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������Ԏ擾
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 *
 *	@return	�{�^��
 */
//-----------------------------------------------------------------------------
static int  CURSOR_GetContButton( const CURSOR_WORK *cp_wk )
{	
	return GFL_UI_KEY_GetCont();
}
//----------------------------------------------------------------------------
/**
 *	@brief	���oOBJ���o��
 *
 *	@param	CURSOR_WORK *p_wk	���[�N
 *	@param	cp_place					�ꏊ	NULL�Ȃ�΃^�[�Q�b�g�Ȃ�
 *	@param	WLD								�ꏊ���[���h���W
 */
//-----------------------------------------------------------------------------
static void CURSOR_SetTarget( CURSOR_WORK *p_wk, const PLACE_DATA *cp_data, const GFL_POINT *cp_wld )
{	
	//�^�[�Q�b�g��NULL�Ȃ�ΐݒ�
	if( cp_data == NULL )
	{	
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_RING], FALSE );
		p_wk->cp_target	= NULL;
	}
	else if( p_wk->cp_target	!= cp_data )
	{	
		//�^�[�Q�b�g���ύX���ꂽ��ݒ�
		GFL_CLACTPOS pos;
		pos.x	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_CURSOR_X ) + cp_wld->x;
		pos.y	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_CURSOR_Y ) + cp_wld->y;
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_RING], &pos, 0 );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_RING], TRUE );
		GFL_CLACT_WK_ResetAnm( p_wk->p_clwk[CURSOR_CLWK_RING] );

		p_wk->cp_target	= cp_data;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�[�Q�b�g���擾
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 *
 *	@return	NULL�Ȃ�΃^�[�Q�b�g���Ȃ�	NULL�ł͂Ȃ��Ȃ�΃^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
static const PLACE_DATA * CURSOR_GetTarget( const CURSOR_WORK *cp_wk )
{	
	return cp_wk->cp_target;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\��	�������܂ꓮ�쒆����
 *
 *	@param	const CURSOR_WORK *cp_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void CURSOR_SetPullEnable( CURSOR_WORK *p_wk, BOOL on_off )
{
	p_wk->is_pull	= on_off;
}
//=============================================================================
/**
 *					PLACE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	������
 *
 *	@param	PLACE_WORK *p_wk			���[�N
 *	@param	TOWNMAP_DATA *cp_data	�f�[�^
 *	@param	p_grh									�O���t�B�b�N
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PLACE_Init( PLACE_WORK *p_wk, u16 now_zone_ID, u16 esc_zone_ID, const TOWNMAP_DATA *cp_data, const TOWNMAP_GRAPHIC_SYS *cp_grh, GAMEDATA *p_gamedata, HEAPID heapID, BOOL is_debug )
{	
  GFL_CLUNIT  *p_clunit = TOWNMAP_GRAPHIC_GetUnit( cp_grh, TOWNMAP_OBJ_CLUNIT_DEFAULT );

	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(PLACE_WORK) );
  p_wk->p_gamedata  = p_gamedata;

	//�쐬����CLWK�̐����v�Z
	{	
		p_wk->data_num	= TOWNMAP_DATA_MAX;
	}
	
	//�ݒ肳��Ă��鐔��CLWK���o�����߁A�o�b�t�@�쐬
	p_wk->p_place	= GFL_HEAP_AllocMemory( heapID, sizeof(PLACE_DATA) * p_wk->data_num );
	GFL_STD_MemClear( p_wk->p_place, sizeof(PLACE_DATA) * p_wk->data_num );

	//�ꏊ�쐬
	{	
		int i;
		u32 chr, cel, plt;

		//���\�[�X�擾
		TOWNMAP_GRAPHIC_GetObjResource( cp_grh, &chr, &cel, &plt );
		//������
		for( i = 0; i < p_wk->data_num; i++ )
		{	
			PlaceData_Init( &p_wk->p_place[i], cp_data, i, p_clunit, chr, cel, plt, p_gamedata, heapID, is_debug );
		}
	}

	//Player�̍쐬
	{	
		u32 chr, cel, plt;
		const PLACE_DATA *cp_placedata	= NULL;
		//�]�[��ID�ɑΉ�����ꏊ���擾
		cp_placedata	= PLACE_GetDataByZoneID( p_wk, now_zone_ID, esc_zone_ID );

		NAGI_Printf( "�]�[��ID now%d esc%d\n", esc_zone_ID, now_zone_ID );

		//���\�[�X�擾
		TOWNMAP_GRAPHIC_GetObjResource( cp_grh, &chr, &cel, &plt );
		//�쐬
		PLACEMARK_Init( &p_wk->player, cp_placedata, p_clunit, chr, cel, plt, heapID );
	}


	//�ړ�����
	p_wk->limit_top			= 0;
	p_wk->limit_left		= 0;
	p_wk->limit_right		= 0;
	p_wk->limit_bottom	= 0;


	//�ŏ��̓m���A�N�e�B�u
	PLACE_NonActive( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�j��
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void PLACE_Exit( PLACE_WORK *p_wk )
{	

	//�v���C���[�j��
	{
		PLACEMARK_Exit( &p_wk->player );
	}

	//CLWK�j��
	{	
		int i;
		for( i = 0; i < p_wk->data_num; i++  )
		{	
			PlaceData_Exit( &p_wk->p_place[i] );
		}
	}

	//�o�b�t�@�N���A
	GFL_HEAP_FreeMemory( p_wk->p_place );

	GFL_STD_MemClear( p_wk, sizeof(PLACE_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	���C������
 *
 *	@param	PLACE_WORK *p_wk		���[�N
 *	@param	CURSOR_WORK *cp_wk	�J�[�\�����
 *
 */
//-----------------------------------------------------------------------------
static void PLACE_Main( PLACE_WORK *p_wk )
{	
#ifdef PM_DEBUG
#if 0
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
	{	
		const PLACE_DATA *cp_data	= &p_wk->p_place[7];
		
		NAGI_Printf( "Pos sx%d yx%d ex%d ey%d w%d\n",
				PLACEDATA_GetParam( cp_data,TOWNMAP_DATA_PARAM_HIT_START_X ),
				PLACEDATA_GetParam( cp_data,TOWNMAP_DATA_PARAM_HIT_START_Y ),
				PLACEDATA_GetParam( cp_data,TOWNMAP_DATA_PARAM_HIT_END_X ),
				PLACEDATA_GetParam( cp_data,TOWNMAP_DATA_PARAM_HIT_END_Y ),
				PLACEDATA_GetParam( cp_data,TOWNMAP_DATA_PARAM_HIT_WIDTH )
				);

		NAGI_Printf( "WLD X%d Y%d\n", p_wk->pos.x, p_wk->pos.y );
	}
#endif
#endif //PM_DEBUG
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�X�P�[���A�b�v�J�n
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void PLACE_Scale( PLACE_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_next )
{	

  OS_Printf( "pla now X%d Y%d sca%f\n", cp_center_next->x, cp_center_next->y, 
      FX_FX32_TO_F32(scale) );
	//�g�傷��Ɠ���͈͂��ύX
	{	
    if( scale == FX32_ONE )
    {
			p_wk->limit_top			= 0;
			p_wk->limit_left		= 0;
			p_wk->limit_right		= 0;
			p_wk->limit_bottom	=	0;
    }
    else
		{	
      p_wk->limit_top			= MAP_SCALE_MOVE_LIMIT_TOP + 24;
			p_wk->limit_left		= MAP_SCALE_MOVE_LIMIT_LEFT;
			p_wk->limit_right		= MAP_SCALE_MOVE_LIMIT_RIGHT;
			p_wk->limit_bottom	=	MAP_SCALE_MOVE_LIMIT_BOTTOM + 24;
		}
	}

	//���W����
	//�ꏊ�g��̒��ňʒu�ݒ������̂ŁA
	//���������
	{	
		GFL_POINT	pos;

    //OBJ�͋t
    pos.x	=	-(cp_center_next->x);
    pos.y	=	-(cp_center_next->y);

	/*	NAGI_Printf( "PLACE scale SX%d SY%d EX%d EY%d PX%d PY%d\n",
				cp_center_start->x, cp_center_start->y, cp_center_next->x, cp_center_next->y,
				pos.x, pos.y );
*/
		PLACE_SetWldPos( p_wk, &pos );
	}

	{	
		int i;
		//�e�ꏊ�A�g��
		//�g��͒��S���W�Œ�
		for( i = 0; i < p_wk->data_num; i++ )
		{	
			PlaceData_Scale( &p_wk->p_place[i], scale, &sc_center_pos );
			PlaceData_SetPos( &p_wk->p_place[i], &p_wk->pos );
		}
	}

	//�v���C���[�ʒu���Đݒ�
	{	
		PLACEMARK_SetPos( &p_wk->player, &p_wk->pos );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�\���A��\���ݒ�
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 *	@param	is_visible				TRUE�Ȃ�\��FALSE�Ȃ�Δ�\��
 *
 */
//-----------------------------------------------------------------------------
static void PLACE_SetVisible( PLACE_WORK *p_wk, BOOL is_visible )
{	
	int i;
	for( i = 0; i < p_wk->data_num; i++ )
	{	
		PlaceData_SetVisible( &p_wk->p_place[i], is_visible );
	}

	PLACEMARK_SetVisible( &p_wk->player, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����I�ɕ\���𑀍삷��
 *
 *	@param	PLACE_WORK *p_wk  ���[�N
 *	@param	is_visible        TRUE�Ȃ�Ε\��  FALSE�Ȃ�Δ�\��
 */
//-----------------------------------------------------------------------------
static void PLACE_SetVisibleForce( PLACE_WORK *p_wk, BOOL is_visible )
{
  if( is_visible == FALSE )
  {
    int i;
    for( i = 0; i < p_wk->data_num; i++ )
    {	
      PlaceData_SetVisibleForce( &p_wk->p_place[i], is_visible );
    }
    PLACEMARK_SetVisible( &p_wk->player, is_visible );
  }
  else
  {
    PLACE_SetVisible( p_wk, TRUE );
    PLACEMARK_SetVisible( &p_wk->player, TRUE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	���W��ݒ�
 *
 *	@param	PLACE_WORK *p_wk		���[�N
 *	@param	GFL_POINT *cp_pos		���W
 *
 */
//-----------------------------------------------------------------------------
static void PLACE_SetWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos )
{	
//	GFL_CLACTPOS	clpos;
	int i;

	p_wk->pos.x	= MATH_CLAMP( cp_pos->x, p_wk->limit_left, p_wk->limit_right );
	p_wk->pos.y	= MATH_CLAMP( cp_pos->y, p_wk->limit_top, p_wk->limit_bottom );

	for( i = 0; i < p_wk->data_num; i++ )
	{	
		PlaceData_SetPos( &p_wk->p_place[i], &p_wk->pos );
	}


	PLACEMARK_SetPos( &p_wk->player, &p_wk->pos );

//	clpos.x	= p_wk->pos.x;
//	clpos.y	= p_wk->pos.y;
	//NAGI_Printf( "place wld pos X%d Y%d\n", clpos.x, clpos.y );
//	GFL_CLACT_USERREND_SetSurfacePos( p_wk->p_rend, 0, &clpos );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	���W���擾
 *
 *	@param	const PLACE_WORK *cp_wk	���[�N
 *	@param	*p_pos									�󂯎����W
 */
//-----------------------------------------------------------------------------
static void PLACE_GetWldPos( const PLACE_WORK *cp_wk, GFL_POINT *p_pos )
{	
	*p_pos	= cp_wk->pos;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	���W�����Z
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 *	@param	GFL_POINT *cp_pos	���Z�l 
 */
//-----------------------------------------------------------------------------
static void PLACE_AddWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	p_wk->pos.x	+= cp_pos->x;
	p_wk->pos.y	+= cp_pos->y;
	PLACE_SetWldPos( p_wk, &p_wk->pos );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�J�[�\�����Փ˂�����
 *
 *	@param	const PLACE_WORK *cp_wk	���[�N
 *	@param	CURSOR_WORK *cp_wk			�J�[�\��
 *
 *	@return	NULL�Ȃ�Γ������Ă��Ȃ�	����ȊO�Ȃ�Γ��������ꏊ�̃f�[�^
 */
//-----------------------------------------------------------------------------
static const PLACE_DATA* PLACE_Hit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor )
{	
	int i;
	
	GFL_POINT pos;

	CURSOR_GetPos( cp_cursor, &pos );

#if 0
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
	{	
		NAGI_Printf( "CUR x%d y%d\n", pos.x, pos.y );
	}
#endif

	//�����蔻��
	for( i = 0; i < cp_wk->data_num; i++ )
	{
		if( PlaceData_IsHit( &cp_wk->p_place[i], &pos, &cp_wk->pos )  )
		{	
			return &cp_wk->p_place[i];
		}
	}

	return NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�J�[�\�����z�����ݔ͈͓���
 *
 *	@param	const PLACE_WORK *cp_wk	���[�N
 *	@param	CURSOR_WORK *cp_wk			�J�[�\��
 *
 *	@return	NULL�Ȃ�Γ������Ă��Ȃ�	����ȊO�Ȃ�Γ��������ꏊ�̃f�[�^
 */
//-----------------------------------------------------------------------------
static const PLACE_DATA* PLACE_PullHit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor )
{	
	int i;
	GFL_POINT pos;
	u32 best_distance;
	u32 now_distance;
	s32 best_idx;

	CURSOR_GetPos( cp_cursor, &pos );

	//�����Փ˂��Ă����Ȃ�Έ�ԋ߂��̂��̂�Ԃ�
	best_idx			=	-1;
	best_distance	= 0xFFFFFFFF;
	for( i = 0; i < cp_wk->data_num; i++ )
	{
		if( PlaceData_IsPullHit( &cp_wk->p_place[i], &pos, &cp_wk->pos, &now_distance )  )
		{
			if( best_distance > now_distance )
			{	
				best_idx	= i;
				best_distance	= now_distance;
			}
		}
	}

	if( best_idx == -1 )
	{	
		return NULL;
	}
	else
	{	
		return &cp_wk->p_place[ best_idx ];
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^	�f�[�^�擾
 *
 *	@param	const PLACE_DATA *cp_wk	���[�N
 *	@param	param										���̎��
 *
 *	@return	���
 */
//-----------------------------------------------------------------------------
static s32 PLACEDATA_GetParam( const PLACE_DATA *cp_wk, TOWNMAP_DATA_PARAM param )
{	
	if( TOWNMAP_DATA_PARAM_POS_X <= param && param <= TOWNMAP_DATA_PARAM_HIT_WIDTH )
	{	
		//�g��k�����ŕύX����f�[�^
		return cp_wk->param[ param - TOWNMAP_DATA_PARAM_POS_X ];
	}
	else if( param == TOWNMAP_DATA_PARAM_SKY_FLAG )
	{
		return cp_wk->is_sky;
	}
	else
	{	
		//�I���W�i���̂܂ܕύX���Ȃ��f�[�^
		return TOWNMAP_DATA_GetParam( cp_wk->cp_data, cp_wk->data_idx, param );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^	�ꏊ���A�N�e�B�u��
 *
 *	@param	PLACE_WORK *p_wk			���[�N
 *	@param	PLACE_DATA *cp_data		���
 *
 */
//-----------------------------------------------------------------------------
static void PLACE_Active( PLACE_WORK *p_wk, const PLACE_DATA *cp_data )
{	
	int i;
	PLACE_DATA *p_data;

	for( i = 0; i < p_wk->data_num; i++ )
	{	
		p_data	= &p_wk->p_place[ i ];
		if( p_data == cp_data )
		{	
			PlaceData_Active( p_data );
		}
		else
		{	
			PlaceData_NonActive( p_data );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^	�m���A�N�e�B�u��Ԃɂ���
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void PLACE_NonActive( PLACE_WORK *p_wk )
{	
	int i;
	PLACE_DATA *p_data;

	for( i = 0; i < p_wk->data_num; i++ )
	{	
		p_data	= &p_wk->p_place[ i ];

		PlaceData_NonActive( p_data );
	}	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�]�[��ID����PLACE_DATA���擾
 *
 *	@param	const PLACE_DATA *cp_data	���[�N
 *	@param	zoneID										�]�[��ID
 *
 *	@return	PLACE_DATA	
 */
//-----------------------------------------------------------------------------
static const PLACE_DATA *PLACE_GetDataByZoneID( const PLACE_WORK *cp_wk, u16 zoneID, u16 escapeID )
{	
	const PLACE_DATA *cp_data	= NULL;
	int i;
  u16 now_zoneID;
  u16 ret;

  now_zoneID  = TOWNMAP_UTIL_GetRootZoneID( cp_wk->p_gamedata, zoneID );

	//�]�[���Ɗ��S�Ή�����ꏊ���擾
	for( i = 0; i < cp_wk->data_num; i++ )
	{	
		cp_data	= &cp_wk->p_place[i];
		if( now_zoneID == PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID ) )
		{
			return cp_data;
		}
	}
#if 0
	//���ݒn���t�B�[���h�ł͂Ȃ��Ȃ�΁A�G�X�P�[�vID����
	if( !ZONEDATA_IsFieldMatrixID( zoneID ) )
	{	
		for( i = 0; i < cp_wk->data_num; i++ )
		{	
			cp_data	= &cp_wk->p_place[i];
			if( escapeID == PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID ) )
			{
				return cp_data;
			}
		}
	}
#endif

  //GF_ASSERT_MSG(0,"�^�E���}�b�v�f�[�^�ɏ�񂪌�����܂���ł��� zoneID%d group%d\n",zoneID, now_zoneID );

	return NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����\���\���ǂ������擾
 *
 *	@param	const PLACE_DATA *cp_wk   ���[�N
 *
 *	@return TRUE�Ŗ�����\�����Ă悢  FALSE�̓_��
 */
//-----------------------------------------------------------------------------
static BOOL PLACEDATA_IsVisiblePlaceName( const PLACE_DATA *cp_wk )
{ 
  u16 arrive_flag	= TOWNMAP_DATA_GetParam( cp_wk->cp_data, cp_wk->data_idx, TOWNMAP_DATA_PARAM_ARRIVE_FLAG );
  //�����t���O���ݒ肵�Ă���ꏊ�́A�����t���O��ON�łȂ��Ɩ�����\���ł��Ȃ�

  return ((arrive_flag != TOWNMAP_DATA_ERROR ) && cp_wk->is_arrive && !cp_wk->is_hide) || (arrive_flag == TOWNMAP_DATA_ERROR );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^�쐬
 *
 *	@param	PLACE_DATA *p_wk	���[�N
 *	@param	cp_data						�f�[�^
 *	@param	data_idx					�f�[�^�ԍ�
 *	@param	*p_clunit					UNIT
 *	@param	chr								�`����
 *	@param	cel								�Z��
 *	@param	plt								�p���b�g
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void PlaceData_Init( PLACE_DATA *p_wk, const TOWNMAP_DATA *cp_data, u32 data_idx, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, GAMEDATA *p_gamedata, HEAPID heapID, BOOL is_debug )
{	
	GF_ASSERT( data_idx < TOWNMAP_DATA_MAX );

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACE_DATA) );
	p_wk->cp_data			= cp_data;
	p_wk->data_idx		= data_idx;
	p_wk->scale				= FX32_ONE;

	//�����t���O�擾
	{	
		u16 arrive_flag	= TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_ARRIVE_FLAG );
		if( arrive_flag != TOWNMAP_DATA_ERROR )
		{	
#ifdef DEBUG_GAMESYS_NONE
			if( p_gamedata == NULL )
			{	
				p_wk->is_arrive	= TRUE;
			}
			else
			{	
				p_wk->is_arrive		= TOWNMAP_UTIL_CheckFlag( p_gamedata, arrive_flag );
			}
#else
			p_wk->is_arrive		= TOWNMAP_UTIL_CheckFlag( p_gamedata, arrive_flag );
#endif //DEBUG_GAMESYS_NONE
		}
		else
		{	
			p_wk->is_arrive		= FALSE;
		}

		if( is_debug )
		{	
			p_wk->is_arrive	= TRUE;
		}
	}	

	//�ύX����f�[�^���擾
	{	
		int i;
		for( i = 0; i < PLACE_DATA_PARAM_MAX; i++ )
		{	
			p_wk->param[i]	= TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, 
					TOWNMAP_DATA_PARAM_POS_X+i );
		}

		//TOWNMAP_DATA_PARAM_SKY_FLAG
    if( is_debug )
    {
      p_wk->is_sky  = TRUE;
    }
    else
    { 
      p_wk->is_sky	= p_wk->is_arrive && TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_SKY_FLAG );
    }
	}

  //�B���t���O���擾
  { 
    u16 hide_flag	= TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIDE_FLAG );
    if( hide_flag != TOWNMAP_DATA_ERROR )
		{	
      p_wk->is_hide		= !TOWNMAP_UTIL_CheckFlag( p_gamedata, hide_flag );
		}
		else
		{	
			p_wk->is_hide		= FALSE;
		}

    if( is_debug )
		{	
      p_wk->is_hide		= FALSE;
		}
  }

	//�^�C�v�ʐݒ�
	switch( PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) )
	{	
	case TOWNMAP_PLACETYPE_TOWN:
		p_wk->active_anm			= 2;
		p_wk->non_active_anm	= 1;
		break;
	case TOWNMAP_PLACETYPE_DUNGEON:
		p_wk->active_anm			= 3;
		p_wk->non_active_anm	= PLACEDATA_ANIME_SEQ_VANISH;
		break;
	case TOWNMAP_PLACETYPE_ROAD:
		p_wk->active_anm			= PLACEDATA_ANIME_SEQ_VANISH;
		p_wk->non_active_anm	= PLACEDATA_ANIME_SEQ_VANISH;
		break;
	case TOWNMAP_PLACETYPE_TOWN_SMALL:
		p_wk->active_anm			= 3;
		p_wk->non_active_anm	= 13;
		break;
	case TOWNMAP_PLACETYPE_HIDE:
		p_wk->active_anm			= 15;
		p_wk->non_active_anm	= 14;
		break;
	case TOWNMAP_PLACETYPE_HIDE_TOWN_S:
		p_wk->active_anm			= 18;
    if( p_wk->is_arrive )
    { 
      p_wk->non_active_anm	= 17;
    }
    else
    { 
      p_wk->non_active_anm	= 16;
    }
		break;
	default:
		GF_ASSERT(0);
	}

	//�Z���쐬
  if( PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) != TOWNMAP_PLACETYPE_ROAD )
	{
		GFL_CLWK_DATA cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_X );
		cldata.pos_y	= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_Y );
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_clunit, chr, plt, cel, &cldata, 0, heapID );

		GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk, CLSYS_AFFINETYPE_DOUBLE );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, TOWNMAP_BG_PRIORITY_BAR_M+1 );
    GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, OBJ_PRIORITY_MAPOBJ );
	}

	//�ŏ��̓m���A�N�e�B�u
	PlaceData_NonActive( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^�j��
 *
 *	@param	PLACE_DATA *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void PlaceData_Exit( PLACE_DATA *p_wk )
{	
	if( p_wk->p_clwk )
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk );
	}
	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACE_DATA) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^�\���ݒ�
 *
 *	@param	PLACE_DATA *p_wk	���[�N
 *	@param	is_visible				TRUE�ŕ\��	FALSE�Ŕ�Ђ厞
 *
 */
//-----------------------------------------------------------------------------
static void PlaceData_SetVisible( PLACE_DATA *p_wk, BOOL is_visible )
{	
	if( p_wk->p_clwk )
	{	
		if( p_wk->now_anm != PLACEDATA_ANIME_SEQ_VANISH )
		{	 
      if( p_wk->is_hide )
      { 
        //�����B�����������炢���Ȃ�Ƃ����`�悵�Ȃ�
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
      }
      else
      { 
        if( PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE ||
            PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE_TOWN_S )
        { 
          PlaceData_SetAnmSeq( p_wk, p_wk->non_active_anm );
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
        }
        else 
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
        }
      }
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����I�ɕ\��������
 *
 *	@param	PLACE_DATA *p_wk  ���[�N
 *	@param	is_visible        �\��
 */
//-----------------------------------------------------------------------------
static void PlaceData_SetVisibleForce( PLACE_DATA *p_wk, BOOL is_visible )
{
  if( p_wk->p_clwk )
  {
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ�f�[�^�Ɠ���������
 *
 *	@param	const PLACE_DATA *cp_wk	��[��
 *	@param	GFL_POINT *cp_pos				���W
 *
 *	@return	TRUE�Ȃ�ΏՓ�	FALSE�Ȃ�΂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL PlaceData_IsHit( const PLACE_DATA *cp_wk, const GFL_POINT *cp_pos, const GFL_POINT *cp_wld_pos )
{	
	GFL_COLLISION3D_CIRCLE		circle;
	GFL_COLLISION3D_CYLINDER	cylinder;
	VecFx32	v1, v2;
	fx32 w;

	v1.x	= FX32_CONST( cp_pos->x );
	v1.y	= FX32_CONST( cp_pos->y );
	v1.z	= 0;
	GFL_COLLISION3D_CIRCLE_SetData( &circle, &v1, 0 );

	v1.x	= FX32_CONST( PLACEDATA_GetParam( cp_wk,TOWNMAP_DATA_PARAM_HIT_START_X ) + cp_wld_pos->x );
	v1.y	= FX32_CONST( PLACEDATA_GetParam( cp_wk,TOWNMAP_DATA_PARAM_HIT_START_Y ) + cp_wld_pos->y );
	v1.z	= 0;
	v2.x	= FX32_CONST( PLACEDATA_GetParam( cp_wk,TOWNMAP_DATA_PARAM_HIT_END_X ) + cp_wld_pos->x );
	v2.y	= FX32_CONST( PLACEDATA_GetParam( cp_wk,TOWNMAP_DATA_PARAM_HIT_END_Y ) + cp_wld_pos->y );
	v2.z	= 0;
	w			= FX32_CONST( PLACEDATA_GetParam( cp_wk,TOWNMAP_DATA_PARAM_HIT_WIDTH ) );
	GFL_COLLISION3D_CYLINDER_SetData( &cylinder, &v1, &v2, w );
	

  if( cp_wk->is_hide )
  { 
    //�B���}�b�v�Ȃ�ΐ�΂ɂ�����Ȃ�
    return FALSE;
  }
  else
  { 
    return GFL_COLLISION3D_CYLXCIR_Check( &cylinder, &circle, NULL );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�����ݔ͈͓����ǂ���
 *
 *	@param	const PLACE_DATA *cp_wk	���[�N
 *	@param	GFL_POINT *cp_pos				���W
 *	@param	�������󂯎��
 *
 *	@return	TRUE�Ȃ�΋z�����ݔ͈�	FALSE�Ȃ�Δ͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL PlaceData_IsPullHit( const PLACE_DATA *cp_wk, const GFL_POINT *cp_pos, const GFL_POINT *cp_wld_pos, u32 *p_distance )
{	
	u32 cur_x, cur_y;
	u32	x, y;

	cur_x	= PLACEDATA_GetParam( cp_wk, TOWNMAP_DATA_PARAM_CURSOR_X ) + cp_wld_pos->x;
	cur_y	= PLACEDATA_GetParam( cp_wk, TOWNMAP_DATA_PARAM_CURSOR_Y ) + cp_wld_pos->y;

	x = (cur_x - cp_pos->x) * (cur_x - cp_pos->x);
	y = (cur_y - cp_pos->y) * (cur_y - cp_pos->y);

  if( cp_wk->is_hide )
  { 
    //�B���}�b�v�Ȃ�ΐ�΂ɂ�����Ȃ�
    return FALSE;
  }
  else
  { 
    if( x + y < ((PLACE_PULL_R * PLACE_PULL_R * MATH_IAbs(cp_wk->scale)) >> FX32_SHIFT) )
    {
      if( p_distance )
      {	
        *p_distance	= (x + y);
      }

      return TRUE;
    }

    return FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�P�[������������
 *
 *	@param	PLACE_DATA *p_wk	���[�N
 *	@param	scale							�X�P�[��
 *	@param	GFL_POINT *cp_center	�g��k�����S���W
 */
//-----------------------------------------------------------------------------
static void	PlaceData_Scale( PLACE_DATA *p_wk, fx32 scale, const GFL_POINT *cp_center_next )
{	
	//�f�[�^���X�V
	{	
		int i;
		int center;
		fx32 x;
		for( i = 0; i < PLACE_DATA_PARAM_HIT_WIDTH; i++ )
		{	
			if(i % 2 == 0)
			{	
				center	= cp_center_next->x;
			}
			else
			{	
				center	= cp_center_next->y;
			}
			x	= TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_POS_X+i );
			x -= center;
			x	*= scale;
			x >>= FX32_SHIFT;
			x	+= center;

			p_wk->param[i]	=	x; 
		}

		//���͒P���ɃX�P�[���{
		p_wk->param[PLACE_DATA_PARAM_HIT_WIDTH]	= 
			(TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_WIDTH )
			 * scale) >> FX32_SHIFT;
	}
	p_wk->scale	= scale;

	//OBJ���g��
	if( p_wk->p_clwk )
	{	
		GFL_CLSCALE		clscale;
		clscale.x	= scale;
		clscale.y	= scale;

		GFL_CLACT_WK_SetScale( p_wk->p_clwk, &clscale );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W�ݒ�
 *
 *	@param	*p_wk	���[�N
 *	@param	GFL_POINT *cp_pos		���W
 */
//-----------------------------------------------------------------------------
static void PlaceData_SetPos(PLACE_DATA *p_wk, const GFL_POINT *cp_wld )
{	
	GFL_CLACTPOS	pos;
	pos.x		= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_X ) + cp_wld->x;
	pos.y		= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_Y ) + cp_wld->y;

	if( p_wk->p_clwk )
	{	
		GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���ݒ�	PLACEDATA__ANIME_SEQ_VANISH�ɂ��Ή�
 *	
 *	@param	*p_wk		���[�N
 *	@param	seq			�A�j���V�[�P���X
 */
//-----------------------------------------------------------------------------
static void PlaceData_SetAnmSeq(PLACE_DATA *p_wk, u16 seq )
{	
	p_wk->now_anm	= seq;
	if( p_wk->now_anm != PLACEDATA_ANIME_SEQ_VANISH )
	{	
		if( p_wk->p_clwk )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
			GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anm );
		}
	}
	else
	{	
    if( p_wk->p_clwk )
		{	
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
    }
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�e�B�u�ݒ�
 *
 *	@param	PLACE_DATA *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void PlaceData_Active( PLACE_DATA *p_wk )
{	

  BOOL is_hide_visible  = FALSE;
#ifdef DEBUG_OBJ_ALLVISIBLE
  if( s_is_place_visible_debug )
  { 
    return ;
  }
#endif


  if( PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE ||
      PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE_TOWN_S )
  { 
    is_hide_visible = !p_wk->is_hide;
  }

  if( p_wk->is_arrive || is_hide_visible )
  {	
    PlaceData_SetVisible( p_wk, TRUE );
    PlaceData_SetAnmSeq( p_wk,  p_wk->active_anm );
  }
  else
  {	
    PlaceData_SetVisible( p_wk, FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�m���A�N�e�B�u�ݒ�
 *
 *	@param	PLACE_DATA *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void PlaceData_NonActive( PLACE_DATA *p_wk )
{	
  BOOL is_hide_visible  = FALSE;

#ifdef DEBUG_OBJ_ALLVISIBLE
  if( s_is_place_visible_debug )
  { 
    return ;
  }
#endif

  if( PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE ||
      PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_PLACE_TYPE ) == TOWNMAP_PLACETYPE_HIDE_TOWN_S )
  { 
    is_hide_visible = !p_wk->is_hide;
  }

  if( p_wk->is_arrive || is_hide_visible )
  {	
    if( p_wk->p_clwk )
    {	
      PlaceData_SetVisible( p_wk, TRUE );
      PlaceData_SetAnmSeq( p_wk,  p_wk->non_active_anm );
    }
  }
  else
  {	
    PlaceData_SetVisible( p_wk, FALSE );
  }
}
//=============================================================================
/**
 *		PLACE_MARK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[�}�[�L���O	������
 *
 *	@param	PLACE_MARK *p_wk		���[�N
 *	@param	PLACE_DATA *cp_data	�f�[�^
 *	@param	*p_clunit						CLWK�쐬�p���j�b�g
 *	@param	chr									�L����
 *	@param	cel									�Z��
 *	@param	plt									�p���b�g
 *	@param	heapID							�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PLACEMARK_Init( PLACE_MARK *p_wk, const PLACE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, HEAPID heapID )
{	
	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACE_MARK) );
	p_wk->cp_data	= cp_data;

	//���\�[�X�擾
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		if( cp_data )
		{	
			cldata.pos_x	= PLACEDATA_GetParam(cp_data,TOWNMAP_DATA_PARAM_POS_X );
			cldata.pos_y	= PLACEDATA_GetParam(cp_data,TOWNMAP_DATA_PARAM_POS_Y );
		}

		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_clunit, chr, plt, cel, &cldata, 0, heapID );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, TOWNMAP_BG_PRIORITY_BAR_M+1 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, OBJ_PRIORITY_MARK );
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 6 );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk, TRUE );

		if( cp_data == NULL )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C��[�}�[�L���O	�j��
 *
 *	@param	PLACE_MARK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void PLACEMARK_Exit( PLACE_MARK *p_wk )
{	
	GFL_CLACT_WK_Remove( p_wk->p_clwk );
	
	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACE_MARK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[�}�[�L���O	�ꏊ�Z�b�g
 *
 *	@param	PLACE_MARK *p_wk			���[�N
 *	@param	PLACE_DATA *cp_data		�ꏊ�f�[�^
 */
//-----------------------------------------------------------------------------
static void PLACEMARK_SetPos( PLACE_MARK *p_wk, const GFL_POINT *cp_wld )
{	
	GFL_CLACTPOS clpos;
	if( p_wk->cp_data )
	{	
		clpos.x	= PLACEDATA_GetParam(p_wk->cp_data,TOWNMAP_DATA_PARAM_POS_X ) + cp_wld->x;
		clpos.y	= PLACEDATA_GetParam(p_wk->cp_data,TOWNMAP_DATA_PARAM_POS_Y ) + cp_wld->y;
	}
	GFL_CLACT_WK_SetPos( p_wk->p_clwk, &clpos, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[�}�[�L���O	�\���ݒ�
 *
 *	@param	PLACE_MARK *p_wk	���[�N
 *	@param	is_visible		TRUE�\���@FALS�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void PLACEMARK_SetVisible( PLACE_MARK *p_wk, BOOL is_visible )
{	
	if( p_wk->cp_data )
	{	
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
	}
}
//=============================================================================
/**
 *		MAP
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	������
 *
 *	@param	MAP_WORK *p_wk	���[�N
 *	@param	u8 map_frm			�n�}�p�t���[��
 *	@param	u8 road_frm			
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MAP_Init( MAP_WORK *p_wk, u8 map_frm, u8 road_frm, HEAPID heapID )
{	
	//�����A
	GFL_STD_MemClear( p_wk, sizeof(MAP_WORK) );
	p_wk->map_frm		= map_frm;
	p_wk->road_frm	= road_frm;
	p_wk->now_scale	= FX32_ONE;

	p_wk->limit_top			= 0;
	p_wk->limit_left		= 0;
	p_wk->limit_right		= 0;
	p_wk->limit_bottom	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	�j��
 *
 *	@param	MAP_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MAP_Exit( MAP_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(MAP_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	���C������
 *
 *	@param	MAP_WORK *p_wk	���[�N
 *	@param	PLACE_WORK      �g��k���̎�OBJ�𓮍삳���邽��
 */
//-----------------------------------------------------------------------------
static void MAP_Main( MAP_WORK *p_wk, PLACE_WORK *p_place )
{	
	p_wk->is_scale_end	= FALSE;

	//�g�又��
	if( p_wk->is_scale_req )
	{	
		fx32		add;
		VecFx32	add_pos;
		
		//�g�嗦�v�Z
		add	= (p_wk->next_scale - p_wk->init_scale)/MAP_SCALE_SYNC;
		p_wk->now_scale	= p_wk->init_scale + add * p_wk->sync;

    OS_FPrintf( 3, "init %f next %f now %f \n", FX_FX32_TO_F32(p_wk->init_scale), FX_FX32_TO_F32(p_wk->next_scale), FX_FX32_TO_F32(p_wk->now_scale) );

		//���S���W�v�Z
		//���S���W�v�Z�Ƃ������A���S�͏�ɉ�ʒ��S�ŁA
		//���S������Č�����悤�ɍ��W�����炷
		add_pos.x	= ((p_wk->next_pos.x - p_wk->init_pos.x)<<FX32_SHIFT);
		add_pos.y	= ((p_wk->next_pos.y - p_wk->init_pos.y)<<FX32_SHIFT);
		//NAGI_Printf( "add X%d Y%d\n", add_pos.x>> FX32_SHIFT, add_pos.y>> FX32_SHIFT );
		p_wk->pos.x	= p_wk->init_pos.x + ((add_pos.x * p_wk->sync/MAP_SCALE_SYNC) >> FX32_SHIFT);
		p_wk->pos.y	= p_wk->init_pos.y + ((add_pos.y * p_wk->sync/MAP_SCALE_SYNC) >> FX32_SHIFT);
		OS_Printf( "now X%d Y%d sca%f\n", p_wk->pos.x, p_wk->pos.y, FX_FX32_TO_F32(p_wk->now_scale) );

#if 1
		//�����v�Z
		{	
			int add_top;
			int add_bottom;
			int add_left;
			int add_right;

			add_top			= MAP_SCALE_MOVE_LIMIT_TOP / MAP_SCALE_SYNC;
			add_bottom	= MAP_SCALE_MOVE_LIMIT_BOTTOM / MAP_SCALE_SYNC;
			add_left		= MAP_SCALE_MOVE_LIMIT_LEFT	/ MAP_SCALE_SYNC;
			add_right		= MAP_SCALE_MOVE_LIMIT_RIGHT / MAP_SCALE_SYNC;
			if( p_wk->next_scale == FX32_ONE )
			{	
				p_wk->limit_top		= MAP_SCALE_MOVE_LIMIT_TOP - (add_top * p_wk->sync) ;
				p_wk->limit_bottom= MAP_SCALE_MOVE_LIMIT_BOTTOM - (add_bottom * p_wk->sync);
				p_wk->limit_left	= MAP_SCALE_MOVE_LIMIT_LEFT - (add_left * p_wk->sync);
				p_wk->limit_right	= MAP_SCALE_MOVE_LIMIT_RIGHT - (add_right * p_wk->sync);
			}
			else
			{	
				p_wk->limit_top		= add_top * p_wk->sync;
				p_wk->limit_bottom= add_bottom * p_wk->sync;
				p_wk->limit_left	= add_left * p_wk->sync;
				p_wk->limit_right	= add_right * p_wk->sync;
			}
		}
#endif

		//�I������
		if( p_wk->sync++ >= MAP_SCALE_SYNC )
		{	
			p_wk->sync	= 0;
			p_wk->is_scale_req	= FALSE;
			p_wk->now_scale		= p_wk->next_scale;
			p_wk->pos					= p_wk->next_pos;
			//���H����
			//GFL_BG_SetVisible( p_wk->road_frm, TRUE );

			//�g�傷��Ɠ���͈͂��ύX
			{	


				if( p_wk->next_scale == FX32_ONE )
				{	
					p_wk->limit_top			= 0;
					p_wk->limit_left		= 0;
					p_wk->limit_right		= 0;
					p_wk->limit_bottom	= 0;
				}
				else
				{	
					p_wk->limit_top			= MAP_SCALE_MOVE_LIMIT_TOP;
					p_wk->limit_left		= MAP_SCALE_MOVE_LIMIT_LEFT;
					p_wk->limit_right		= MAP_SCALE_MOVE_LIMIT_RIGHT;
					p_wk->limit_bottom	=	MAP_SCALE_MOVE_LIMIT_BOTTOM;
				}
			}
			p_wk->is_scale_end	= TRUE;
		}

    //OBJ�݂̂P�t���[�������̂ŁA�P�t���[���O�̍��W��n��
    PLACE_Scale( p_place, p_wk->now_scale, &p_wk->pos );
#if 0
    { 
      MtxFx22 mtx;
      MTX_Scale22( &mtx, p_wk->now_scale, p_wk->now_scale );
      GFL_BG_SetAffine( p_wk->map_frm, &mtx, sc_center_pos.x, sc_center_pos.y );
      GFL_BG_SetAffine( p_wk->road_frm, &mtx, sc_center_pos.x, sc_center_pos.y );
    }
#endif

		//���f
    MAP_SetWldPos( p_wk, &p_wk->pos );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	�g��J�n
 *
 *	@param	MAP_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MAP_Scale( MAP_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_start, const GFL_POINT *cp_center_end )
{	
	//�g��k�����N�G�X�g
	p_wk->is_scale_req	= TRUE;
	p_wk->next_scale		= scale;
	p_wk->init_scale		= p_wk->now_scale;

	//���S���W����A���W�ɒ���
	p_wk->init_pos.x	= cp_center_start->x - 128;
	p_wk->init_pos.y	= cp_center_start->y - 96;
	p_wk->next_pos.x	= cp_center_end->x - 128;
	p_wk->next_pos.y	= cp_center_end->y - 96;
	NAGI_Printf( "Scale start sx%d sy%d ex%d ey%d\n", p_wk->init_pos.x, p_wk->init_pos.y, 
			p_wk->next_pos.x, p_wk->next_pos.y);

  OS_FPrintf( 3, "init %f next %f\n", FX_FX32_TO_F32(p_wk->init_scale), FX_FX32_TO_F32(p_wk->next_scale) );

	//���H�͏���
//	GFL_BG_SetVisible( p_wk->road_frm, FALSE );

}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	�g��k�������`�F�b�N
 *
 *	@param	MAP_WORK *p_wk	���[�N
 *
 *	@return	TRUE������	FALSE�������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MAP_IsScaleEnd( const MAP_WORK *cp_wk )
{	
	return cp_wk->is_scale_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	�g��k����
 *
 *	@param	const MAP_WORK *cp_wk		���[�N
 *
 *	@return	TRUE������	FALSE�������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MAP_IsScale( const MAP_WORK *cp_wk )
{	
	return cp_wk->is_scale_req;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	���W�ݒ�
 *
 *	@param	MAP_WORK *p_wk		���[�N
 *	@param	GFL_POINT *cp_pos ���W
 *
 */
//-----------------------------------------------------------------------------
static void MAP_SetWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	p_wk->pos.x	= MATH_CLAMP( cp_pos->x, p_wk->limit_left, p_wk->limit_right );
	p_wk->pos.y	= MATH_CLAMP( cp_pos->y, p_wk->limit_top, p_wk->limit_bottom );

	GFL_BG_SetScrollReq( p_wk->map_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x );
	GFL_BG_SetScrollReq( p_wk->map_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y );
	GFL_BG_SetRotateCenterReq( p_wk->map_frm, GFL_BG_CENTER_X_SET, sc_center_pos.x );
	GFL_BG_SetRotateCenterReq( p_wk->map_frm, GFL_BG_CENTER_Y_SET, sc_center_pos.y );
	GFL_BG_SetScaleReq( p_wk->map_frm, GFL_BG_SCALE_X_SET, p_wk->now_scale );
	GFL_BG_SetScaleReq( p_wk->map_frm, GFL_BG_SCALE_Y_SET, p_wk->now_scale );

	GFL_BG_SetScrollReq( p_wk->road_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x );
	GFL_BG_SetScrollReq( p_wk->road_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y );
	GFL_BG_SetRotateCenterReq( p_wk->road_frm, GFL_BG_CENTER_X_SET, sc_center_pos.x );
	GFL_BG_SetRotateCenterReq( p_wk->road_frm, GFL_BG_CENTER_Y_SET, sc_center_pos.y );
	GFL_BG_SetScaleReq( p_wk->road_frm, GFL_BG_SCALE_X_SET, p_wk->now_scale );
	GFL_BG_SetScaleReq( p_wk->road_frm, GFL_BG_SCALE_Y_SET, p_wk->now_scale );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	���W�擾
 *
 *	@param	const MAP_WORK *cp_wk	���[�N
 *	@param	*p_pos								���W�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void MAP_GetWldPos( const MAP_WORK *cp_wk, GFL_POINT *p_pos )
{	
	*p_pos	= cp_wk->pos;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n�}	���W���Z
 *
 *	@param	MAP_WORK *p_wk			���[�N
 *	@param	GFL_POINT *cp_pos		���W���Z
 */
//-----------------------------------------------------------------------------
static void MAP_AddWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	p_wk->pos.x	+= cp_pos->x;
	p_wk->pos.y	+= cp_pos->y;
	MAP_SetWldPos( p_wk, &p_wk->pos );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����\��
 *
 *	@param	const MAP_WORK *cp_wk ���[�N
 *	@param	is_visible            TRUE�ŕ\��  FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void MAP_SetVisibleRoadFrm( const MAP_WORK *cp_wk, BOOL is_visible )
{
  GFL_BG_SetVisible( cp_wk->road_frm, is_visible );
}
//=============================================================================
/**
 *		INFO
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��	������
 *
 *	@param	INFO_WORK *p_wk	���[�N
 *	@param	frm							�����쐬�p�t���[��
 *	@param	*p_font					�t�H���g
 *	@param	*p_place_msg		�n��GMM
 *	@param	*p_guide_msg		�K�C�hGMM
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void INFO_Init( INFO_WORK *p_wk, u8 frm, GFL_FONT *p_font, const GFL_MSGDATA *cp_place_msg, const GFL_MSGDATA *cp_guide_msg, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(INFO_WORK) );
	p_wk->p_font			= p_font;
	p_wk->cp_place_msg	= cp_place_msg;
	p_wk->cp_guide_msg	= cp_guide_msg;

	//�����ʍ쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, INFO_BMPWIN_X, INFO_BMPWIN_Y, INFO_BMPWIN_W, INFO_BMPWIN_H,
			TOWNMAP_BG_PAL_S_14, GFL_BMP_CHRAREA_GET_F );

	//�ėp�����o�b�t�@
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 255, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��	�j��
 *
 *	@param	INFO_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void INFO_Exit( INFO_WORK *p_wk )
{
	//�ėp�����o�b�t�@�j��
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );

	//�����ʍ폜
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(INFO_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��	���C������
 *
 *	@param	INFO_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void INFO_Main( INFO_WORK *p_wk )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��	���A�b�v�f�[�g
 *
 *	@param	INFO_WORK *p_wk			��[��
 *	@param	PLACE_DATA *cp_data	�\������ꏊ�̏��
 */
//-----------------------------------------------------------------------------
static void INFO_Update( INFO_WORK *p_wk, const PLACE_DATA *cp_data )
{	
	GFL_BMP_DATA	*p_bmp	= GFL_BMPWIN_GetBmp( p_wk->p_bmpwin );

	//�N���A
	GFL_BMP_Clear( p_bmp, 0 );

	if( cp_data )
	{	
		u16 msgID;
		u16 zoneID;

		//�n��
		zoneID	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID );
		GF_ASSERT( zoneID != TOWNMAP_DATA_ERROR );
		GFL_MSG_GetString( p_wk->cp_place_msg, ZONEDATA_GetPlaceNameID( zoneID ), p_wk->p_strbuf );
#ifdef INFO_STR_PLACE_CENTERING
		{	
			s16 x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
			x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_wk->p_font, 0 )/2;
			PRINTSYS_Print( p_bmp, x, INFO_STR_PLACE_Y, p_wk->p_strbuf, p_wk->p_font );	
		}
#else
			PRINTSYS_Print( p_bmp, INFO_STR_PLACE_X, INFO_STR_PLACE_Y, p_wk->p_strbuf, p_wk->p_font );	
#endif

		//GUIDE
		msgID	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_GUIDE_MSGID);
		if( msgID != TOWNMAP_DATA_ERROR )
		{	
			GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
			PRINTSYS_Print( p_bmp, INFO_STR_GUIDE_X, INFO_STR_GUIDE_Y, p_wk->p_strbuf, p_wk->p_font );
		}

    //�����\��
    if( PLACEDATA_IsVisiblePlaceName( cp_data ) )
    { 
			//PLACE1
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE1_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE1_X, INFO_STR_PLACE1_Y, p_wk->p_strbuf, p_wk->p_font );
			}

			//PLACE2
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE2_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE2_X, INFO_STR_PLACE2_Y, p_wk->p_strbuf, p_wk->p_font );
			}
			//PLACE3
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE3_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE3_X, INFO_STR_PLACE3_Y, p_wk->p_strbuf, p_wk->p_font );
			}
			//PLACE4
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE4_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE4_X, INFO_STR_PLACE4_Y, p_wk->p_strbuf, p_wk->p_font );
			}
			//PLACE5
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE5_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE5_X, INFO_STR_PLACE5_Y, p_wk->p_strbuf, p_wk->p_font );
			}
			//PLACE6
			msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE6_MSGID);
			if( msgID != TOWNMAP_DATA_ERROR )
			{	
				GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
				PRINTSYS_Print( p_bmp, INFO_STR_PLACE6_X, INFO_STR_PLACE6_Y, p_wk->p_strbuf, p_wk->p_font );
			}
    }
	}
	GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��	���N���A
 *
 *	@param	INFO_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void INFO_Clear( INFO_WORK *p_wk )
{	
	GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->p_bmpwin );
}
//=============================================================================
/**
 *		MSGWND
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W��	������
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	bgframe						BG�t���[��
 *	@param	*p_font						�t�H���g
 *	@param	*p_msg						���b�Z�[�W
 *	@param	x									���WX
 *	@param	y									���WY
 *	@param	w									��
 *	@param	h									����
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, GFL_FONT *p_font, const GFL_MSGDATA *cp_msg, u8 x, u8 y, u8 w, u8 h, HEAPID heapID )
{	
	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->p_font	= p_font;
	p_wk->cp_msg		= cp_msg;

	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, TOWNMAP_BG_PAL_M_14, GFL_BMP_CHRAREA_GET_F );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 255, heapID );

}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W��	�j��
 *
 *	@param	MSGWND_WORK* p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void MSGWND_Exit( MSGWND_WORK* p_wk )
{	
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W��	�v�����g
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	strID							������ID
 *	@param	x									���WX
 *	@param	y									���WY
 */
//-----------------------------------------------------------------------------
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID, u16 x, u16 y )
{	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//������쐬
	GFL_MSG_GetString( p_wk->cp_msg, strID, p_wk->p_strbuf );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), x, y, p_wk->p_strbuf, p_wk->p_font );

	GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W��	�v�����g
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	strID							������ID
 *	@param	x									���WX
 *	@param	y									���WY
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintCode( MSGWND_WORK* p_wk, const STRCODE *cp_code, u16 x, u16 y )
{	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//������쐬
  GFL_STR_SetStringCode( p_wk->p_strbuf, cp_code );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), x, y, p_wk->p_strbuf, p_wk->p_font );

	GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	MSGWND_WORK* p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void MSGWND_Clear( MSGWND_WORK* p_wk )
{	
	GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->p_bmpwin );
}
//=============================================================================
/**
 *	PLACEWND
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�n���E�B���h�E	������
 *
 *	@param	PLACEWND_WORK *p_wk		���[�N
 *	@param	bgframe								�t�H���g�\����
 *	@param	*p_font								�t�H���g
 *	@param	GFL_MSGDATA *cp_msg		�n�����b�Z�[�W
 *	@param	GFL_CLWK		clwk			OBJ
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PLACEWND_Init( PLACEWND_WORK *p_wk, u8 bgframe, GFL_FONT *p_font, const GFL_MSGDATA *cp_msg, GFL_CLWK *p_clwk, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACEWND_WORK) );
	p_wk->p_clwk	= p_clwk;

	//�t�H���g�ʐݒ�
	MSGWND_Init( &p_wk->msgwnd, bgframe, p_font, cp_msg, PLACEWND_BMPWIN_X, PLACEWND_BMPWIN_Y,
			PLACEWND_BMPWIN_W, PLACEWND_BMPWIN_H, heapID );

	//�Z���ݒ�
	{	
		GFL_CLACTPOS	pos;
		pos.x	= PLACEWND_WND_POS_X;
		pos.y	= PLACEWND_WND_POS_Y;
		GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, 0 );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, TOWNMAP_BG_PRIORITY_FONT_M+1 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, OBJ_PRIORITY_FRONTEND );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 0 );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk, TRUE );
		GFL_CLACT_WK_StopAnm( p_wk->p_clwk ); 
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n���E�B���h�E	�j��
 *
 *	@param	PLACEWND_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void PLACEWND_Exit( PLACEWND_WORK *p_wk )
{	
	//�t�H���g�ʉ��
	MSGWND_Exit( &p_wk->msgwnd );

	GFL_STD_MemClear( p_wk, sizeof(PLACEWND_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n���E�B���h�E	���C������
 *
 *	@param	PLACEWND_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void PLACEWND_Main( PLACEWND_WORK *p_wk )
{	
#ifdef DEBUG_CURSOR_POS
  if( s_is_print_debug )
  { 
    p_wk->is_update = TRUE;
    p_wk->is_start  = FALSE;
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
    GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk, 3 );
  }
#endif

	//�A�b�v�f�[�g�`�F�b�N
	if( p_wk->is_start )
	{	
		p_wk->is_start	= FALSE;

		//OBJ�A�j���J�n
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 0 );
		GFL_CLACT_WK_StartAnm( p_wk->p_clwk ); 
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );

		//�t�H���g�͏���
		MSGWND_Clear( &p_wk->msgwnd );

		//���C���������s
		p_wk->is_update	= TRUE;
	}

	//���C������
	if( p_wk->is_update )
	{	
		//�A�j���I�����Ƀt�H���g���o��
		if( GFL_CLACT_WK_CheckAnmActive( p_wk->p_clwk ) == FALSE )
		{	
			u16 zoneID;
			//���b�Z�[�W�Ō�擾
			zoneID	= PLACEDATA_GetParam( p_wk->cp_data, TOWNMAP_DATA_PARAM_ZONE_ID );
			GF_ASSERT( zoneID != TOWNMAP_DATA_ERROR );

#ifdef DEBUG_CURSOR_POS
      if( s_is_print_debug )
      { 
        char  str[32] = { 0 };
        STRCODE strCode[32];
        STD_TSNPrintf( str , 32-1, "�J�[�\��X[%d]Y[%d]", s_cursor_pos.x, s_cursor_pos.y );
        DEB_STR_CONV_SjisToStrcode( str, strCode , 32-1 );
        MSGWND_PrintCode( &p_wk->msgwnd, strCode, PLACEWND_STR_X, PLACEWND_STR_Y );
      }
      else
#endif 
      { 
        //�����`��
        MSGWND_Print( &p_wk->msgwnd, ZONEDATA_GetPlaceNameID( zoneID ), PLACEWND_STR_X, PLACEWND_STR_Y );
      }

      //�A�j���X�g�b�v
			GFL_CLACT_WK_StopAnm( p_wk->p_clwk ); 

			//���C�������I��
			p_wk->is_update	= FALSE;
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�n���E�B���h�E	�o���J�n
 *
 *	@param	PLACEWND_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void PLACEWND_Start( PLACEWND_WORK *p_wk, const PLACE_DATA *cp_data )
{	

	GF_ASSERT( cp_data );
	p_wk->is_start	= TRUE;
	p_wk->cp_data		= cp_data;

}
//----------------------------------------------------------------------------
/**
 *	@brief	�n���E�B���h�E	�\���ݒ�
 *	
 *	@param	PLACEWND_WORK *p_wk	���[�N
 *	@param	is_visible					TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void PLACEWND_SetVisible( PLACEWND_WORK *p_wk, BOOL is_visible )
{	
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
	if( is_visible == FALSE )
	{	
		//���C���������~�߂�
		p_wk->is_start	= FALSE;
		p_wk->is_update	= FALSE;

		//���b�Z�[�W�ʃN���A����
		MSGWND_Clear( &p_wk->msgwnd );
	}
}
//=============================================================================
/**
 *		UI
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	UI������
 *
 *	@param	UI_WORK *p_wk	���[�N
 *	@param	heapID				�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void UI_Init( UI_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI�j��
 *
 *	@param	UI_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UI_Exit( UI_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI���C������
 *
 *	@param	UI_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UI_Main( UI_WORK *p_wk )
{	
	BOOL is_sync;
	BOOL is_distance;
	BOOL is_cont	= FALSE;
	fx32 mag;
	u32 x, y;
	p_wk->is_slide	= FALSE;

	//�^�b�`�̏���
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		//�^�b�`�͈͂̒��Ȃ��
		if( ((u32)( x - CURSOR_MOVE_LIMIT_LEFT) <= (u32)(CURSOR_MOVE_LIMIT_RIGHT - CURSOR_MOVE_LIMIT_LEFT))
				&	((u32)( y - CURSOR_MOVE_LIMIT_TOP) <= (u32)(CURSOR_MOVE_LIMIT_BOTTOM - CURSOR_MOVE_LIMIT_TOP)))
		{	

			p_wk->start.x		= x << FX32_SHIFT;
			p_wk->start.y		= y << FX32_SHIFT;
			p_wk->start.z		= 0;
			p_wk->end.x		= x << FX32_SHIFT;
			p_wk->end.y		= y << FX32_SHIFT;
			p_wk->end.z		= 0;
		}
	}
	else if(GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
		//�^�b�`�͈͂̒��Ȃ��
		if( ((u32)( x - CURSOR_MOVE_LIMIT_LEFT) <= (u32)(CURSOR_MOVE_LIMIT_RIGHT - CURSOR_MOVE_LIMIT_LEFT))
				&	((u32)( y - CURSOR_MOVE_LIMIT_TOP) <= (u32)(CURSOR_MOVE_LIMIT_BOTTOM - CURSOR_MOVE_LIMIT_TOP)))
		{
			if( p_wk->is_reset )
			{	
				p_wk->start		= p_wk->end;
				p_wk->is_reset	= FALSE;
			}

			p_wk->end.x		= x << FX32_SHIFT;
			p_wk->end.y		= y << FX32_SHIFT;
			p_wk->end.z		= 0;

			VEC_Subtract( &p_wk->end, &p_wk->start, &p_wk->v );

      if( VEC_Mag( &p_wk->v ) >= UI_SLIDE_MOVE_DISTANCE_MIN )
      { 
        p_wk->is_slide	= TRUE;
      }
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X���C�h����
 *
 *	@param	const UI_WORK *cp_wk	���[�N
 *	@param	*p_vec								�x�N�g��
 *	@param	*p_sync								�V���N
 *
 *	@return	TRUE�X���C�h�����@FALSE�X���C�h���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL UI_GetSlide( const UI_WORK *cp_wk, VecFx32 *p_vec, u32 *p_sync )
{	
	if( cp_wk->is_slide )
	{	
		if( p_vec )
		{	
			*p_vec	= cp_wk->v;
		}
		if( p_sync )
		{	
			*p_sync	= cp_wk->slide_sync;
		}
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X���C�h�J�n�ʒu�����Z�b�g
 *
 *	@param	UI_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UI_ResetSlide( UI_WORK *p_wk )
{	
	p_wk->is_reset	= TRUE;
}
//=============================================================================
/**
 *		ETC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	������`��
 *
 *	@param	GFL_BMP_DATA *p_bmp	��������BMP
 *	@param	GFL_POINT *cp_start	�J�n
 *	@param	GFL_POINT *cp_end		�I��
 *
 */
//-----------------------------------------------------------------------------
static void BMP_DrawLine( GFL_BMP_DATA *p_bmp, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u8 plt_num )
{	
	VecFx32	start;
	VecFx32	end;
	VecFx32 norm;
	fx32 mag;

	start.x	= cp_start->x << FX32_SHIFT;
	start.y	= cp_start->y << FX32_SHIFT;
	start.z	= 0;

	end.x	= cp_end->x << FX32_SHIFT;
	end.y	= cp_end->y << FX32_SHIFT;
	end.z	= 0;

	VEC_Subtract( &end, &start, &norm );
	mag	= VEC_Mag( &norm );
	VEC_Normalize( &norm, &norm );

	{	
		int i;
		GFL_POINT pos;

		for( i = 0; i < (mag>>FX32_SHIFT); i++ )
		{	
			pos.x	= (start.x + (norm.x * i) ) >> FX32_SHIFT;
			pos.y	= (start.y + (norm.y * i) ) >> FX32_SHIFT;
			GFL_BMP_Fill( p_bmp, pos.x, pos.y, 1, 1, plt_num );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	����������
 *
 *	@param	GFL_BMP_DATA *p_bmp	��������BMP
 *	@param	GFL_POINT *cp_start	�J�n���W
 *	@param	GFL_POINT *cp_end		�I�����W
 *	@param	w										����
 *	@param	plt_num							�������ރp���b�g�ԍ�
 */
//-----------------------------------------------------------------------------
static void BMP_DrawBoldLine( GFL_BMP_DATA *p_bmp, const GFL_POINT *cp_start, const GFL_POINT *cp_end, int w, u8 plt_num )
{	
	VecFx32	start;
	VecFx32	end;
	VecFx32 norm;
	fx32 mag;

	VecFx32	v;	//norm�̐���

	start.x	= cp_start->x << FX32_SHIFT;
	start.y	= cp_start->y << FX32_SHIFT;
	start.z	= 0;

	end.x	= cp_end->x << FX32_SHIFT;
	end.y	= cp_end->y << FX32_SHIFT;
	end.z	= 0;

	VEC_Subtract( &end, &start, &norm );
	mag	= VEC_Mag( &norm );
	VEC_Normalize( &norm, &norm );

	VEC_Set( &v, -norm.y, norm.x, 0 );

	{	
		int i,j;
		GFL_POINT pos;
		VecFx32	add;

		for( j = 0; j < w/2; j++ )
		{	
			add.x	= v.x * j;
			add.y	= v.y * j;
			add.z	= 0;

			for( i = 0; i < (mag>>FX32_SHIFT); i++ )
			{	
				pos.x	= (start.x + (norm.x * i) + add.x ) >> FX32_SHIFT;
				pos.y	= (start.y + (norm.y * i) + add.y ) >> FX32_SHIFT;
				GFL_BMP_Fill( p_bmp, pos.x, pos.y, 1, 1, plt_num );
			}

			for( i = 0; i < (mag>>FX32_SHIFT); i++ )
			{	
				pos.x	= (start.x + (norm.x * i) - add.x ) >> FX32_SHIFT;
				pos.y	= (start.y + (norm.y * i) - add.y ) >> FX32_SHIFT;
				GFL_BMP_Fill( p_bmp, pos.x, pos.y, 1, 1, plt_num );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  VBlank�^�X�N
 *
 *	@param	GFL_TCB *p_tcb  �^�X�N
 *	@param	*p_wk_adrs      ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void TownMap_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;
#if 0
  if( p_wk->is_scale_end_req )
  {
    MAP_SetVisibleRoadFrm( &p_wk->map, TRUE );


    p_wk->is_scale_end_req  = FALSE;
  }
#endif
}

//=============================================================================
/**
 *			DEBUG
 */
//=============================================================================
#ifdef DEBUG_PRINT_USE
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	������
 *
 *	@param	frm											�t���[��
 *	@param	heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static DEBUG_PRINT_WORK * DEBUGPRINT_Init( u8 frm, DEBUGPRINT_SAVE_TYPE	save, HEAPID heapID )
{	
	DEBUG_PRINT_WORK *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_PRINT_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );
	p_wk->heapID						= heapID;
	p_wk->save							= save;
	p_wk->frm								= frm;

	//�f�o�b�O�v�����g�p�t�H���g
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );	

	//�ޔ��G���A���^�C�v�ɂ���Ď擾
	switch( p_wk->save )
	{
	case DEBUGPRINT_SAVE_TYPE_NONE:
		//�g��Ȃ��̂Ŏ擾���Ȃ�
		break;
	case DEBUGPRINT_SAVE_TYPE_NETERR:
		NetErr_GetTempArea( &p_wk->p_char_temp_area, &p_wk->p_scrn_temp_area, &p_wk->p_pltt_temp_area );
		break;
	case DEBUGPRINT_SAVE_TYPE_HEAP:
		p_wk->p_char_temp_area	= GFL_HEAP_AllocMemory( heapID, DEBUGPRINT_CHAR_TEMP_AREA );
		p_wk->p_scrn_temp_area	= GFL_HEAP_AllocMemory( heapID, DEBUGPRINT_SCRN_TEMP_AREA );
		p_wk->p_pltt_temp_area	= GFL_HEAP_AllocMemory( heapID, DEBUGPRINT_PLTT_TEMP_AREA );
		break;
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�j��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Exit( DEBUG_PRINT_WORK *p_wk )
{	
	//�J���Ă�������
	if( DEBUGPRINT_IsOpen(p_wk) )
	{	
		DEBUGPRINT_Close(p_wk);
	}

	//�ޔ��G���A���^�C�v�ɂ���ĉ��
	switch( p_wk->save )
	{
	case DEBUGPRINT_SAVE_TYPE_NONE:
		break;
	case DEBUGPRINT_SAVE_TYPE_NETERR:
		break;
	case DEBUGPRINT_SAVE_TYPE_HEAP:
		GFL_HEAP_FreeMemory( p_wk->p_pltt_temp_area );
		GFL_HEAP_FreeMemory( p_wk->p_scrn_temp_area );
		GFL_HEAP_FreeMemory( p_wk->p_char_temp_area );
		break;
	}


	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�I�[�v��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Open( DEBUG_PRINT_WORK *p_wk )
{	

	GF_ASSERT( p_wk );
	GF_ASSERT( p_wk->is_open == FALSE );

	if( p_wk->save != DEBUGPRINT_SAVE_TYPE_NONE )
	{	
		//VRAM�̃f�[�^��ޔ�
		GFL_STD_MemCopy16(GFL_DISPUT_GetCgxPtr(p_wk->frm), p_wk->p_char_temp_area, DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetScrPtr(p_wk->frm), p_wk->p_scrn_temp_area, DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetPltPtr(p_wk->frm), p_wk->p_pltt_temp_area, DEBUGPRINT_PLTT_TEMP_AREA);	
		//Font�J���[�̑ޔ�
		GFL_FONTSYS_GetColor( &p_wk->font_col_bkup[0] ,
				&p_wk->font_col_bkup[1] ,
				&p_wk->font_col_bkup[2] );

		//�������ޔ�
		p_wk->prioryty_bkup = GFL_BG_GetPriority(p_wk->frm);
		p_wk->scroll_x_bkup = GFL_BG_GetScrollX(p_wk->frm);
		p_wk->scroll_y_bkup = GFL_BG_GetScrollY(p_wk->frm);
	}

	//��őޔ����������̂̐ݒ�
	GFL_BG_SetPriority( p_wk->frm , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , 0 );

	//�f�o�b�O�v�����g�p�ݒ�
	//�X�N���[���̍쐬
  {
    u8 x,y;
		u16 buf;
    for( y = 0;y<DEBUGPRINT_HEIGHT;y++ )
    {
      for( x=0;x<DEBUGPRINT_WIDTH;x++ )
      {
        buf = x+y*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
      for( x=DEBUGPRINT_WIDTH;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
    }
		for( y = DEBUGPRINT_HEIGHT;y<24;y++ )
    {
	 		buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
 			GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
			for( x=0;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
		}
    GFL_BG_LoadScreenReq( p_wk->frm );
  }
  
  //�p���b�g�̍쐬
  GFL_BG_LoadPalette( p_wk->frm, (void*)sc_debug_print_plt, sizeof(u16)*NELEMS(sc_debug_print_plt), 0 );

	//�������ނ��߂�BMP�쐬
	p_wk->p_bmp	= GFL_BMP_CreateInVRAM( GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_WIDTH, DEBUGPRINT_HEIGHT, GFL_BMP_16_COLOR, p_wk->heapID );
	GFL_STD_MemClear16( GFL_DISPUT_GetCgxPtr(p_wk->frm) , DEBUGPRINT_CHAR_TEMP_AREA );

	p_wk->is_open	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈���J���Ă��邩
 *
 *	@return	TRUE�Ȃ�ΊJ���Ă���	FALSE�Ȃ�Ε��Ă���
 */
//-----------------------------------------------------------------------------
static BOOL DEBUGPRINT_IsOpen( DEBUG_PRINT_WORK *p_wk )
{	
	if( p_wk )
	{	
		return p_wk->is_open;
	}
	else
	{	
		return FALSE;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�I��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Close( DEBUG_PRINT_WORK *p_wk )
{	

	GF_ASSERT( p_wk );
	GF_ASSERT( p_wk->is_open == TRUE );


	p_wk->is_open	= FALSE;

	GFL_BMP_Delete( p_wk->p_bmp );
	if( p_wk->save != DEBUGPRINT_SAVE_TYPE_NONE )
	{	
		//������땜�A
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , p_wk->scroll_x_bkup );
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , p_wk->scroll_y_bkup );
		GFL_BG_SetPriority( p_wk->frm , p_wk->prioryty_bkup );
		//Font�J���[�̕��A
		GFL_FONTSYS_SetColor( p_wk->font_col_bkup[0] ,
				p_wk->font_col_bkup[1] ,
				p_wk->font_col_bkup[2] );
		GFL_STD_MemCopy16(p_wk->p_char_temp_area, GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_scrn_temp_area, GFL_DISPUT_GetScrPtr(p_wk->frm), DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_pltt_temp_area, GFL_DISPUT_GetPltPtr(p_wk->frm), DEBUGPRINT_PLTT_TEMP_AREA);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɏ�������
 *
 *	@param	u16 *cp_str							���C�h������
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Print( DEBUG_PRINT_WORK *p_wk, const u16 *cp_str, u16 x, u16 y )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;

	GF_ASSERT(p_wk);

	//STRBUF�p�ɕϊ�
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUF�ɓ]��
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, p_wk->heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	//��������
	PRINTSYS_Print( p_wk->p_bmp, x, y, p_strbuf, p_wk->p_font );

	//�j��
	GFL_STR_DeleteBuffer( p_strbuf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɐ��l��������������
 *
 *	@param	u16 *cp_str							���C�h������i%d��%f���g���Ă��������j
 *	@param	number									����
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_PrintNumber( DEBUG_PRINT_WORK *p_wk, const u16 *cp_str, int number, u16 x, u16 y )
{	
	u16	str[128];
	swprintf( str, 128, cp_str, number );
	DEBUGPRINT_Print( p_wk, str, x, y );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈���N���A�[
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Clear( DEBUG_PRINT_WORK *p_wk )
{	
	GF_ASSERT(p_wk);
	GFL_BMP_Clear( p_wk->p_bmp, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�A�ꕔ�͈̓N���A�[
 *
 *	@param	x	���WX
 *	@param	y	���WY
 *	@param	w	��
 *	@param	h	����
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_ClearRange( DEBUG_PRINT_WORK *p_wk, s16 x, s16 y, u16 w, u16 h )
{	
	GF_ASSERT(p_wk);
	GFL_BMP_Fill( p_wk->p_bmp, x, y, w, h, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g
 *
 *	@param	TOWNMAP_WORK *p_wk	���C�����[�N
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Update( DEBUG_PRINT_WORK *p_print, TOWNMAP_WORK *p_wk )
{	
	if( !DEBUGPRINT_IsOpen(p_print) )
	{	
		return;
	}

	DEBUGPRINT_Clear(p_print);

	{	
		GFL_POINT pos;
		CURSOR_GetPos( &p_wk->cursor, &pos );
		DEBUGPRINT_Print( p_print, L"�J�[�\�� ", 32, 32 );
		DEBUGPRINT_PrintNumber( p_print, L"X=[%d]", pos.x, 80, 32 );
		DEBUGPRINT_PrintNumber( p_print, L"Y=[%d]", pos.y, 120, 32 );
	}
#if 0
	//�����\���̂��̂��f�o�b�O�v�����g
	{	
		const PLACE_WORK *cp_place	= &p_wk->place;
		const PLACE_DATA* cp_data		= &cp_place->p_place[i];

		GFL_POINT pos;


		pos.x	= ;
		pos.y	= ;
		DEBUGPRINT_Print( p_print, L"�ꏊ���W ", 32, 64 );
		DEBUGPRINT_PrintNumber( p_print, L"X=[%d]", pos.x, 80, 64 );
		DEBUGPRINT_PrintNumber( p_print, L"Y=[%d]", pos.y, 120, 64 );
	}
#endif
}
#endif //DEBUG_PRINT_USE

//=============================================================================
/**
 *	DEBUG_POS_CHECK
 */
//=============================================================================
#ifdef DEBUG_POS_CHECK
//----------------------------------------------------------------------------
/**
 *	@brief	�ʒu��\��
 *
 *	@param	DEBUG_PRINT_WORK *p_print	�v�����g���[�N
 *	@param	*p_wk											���C�����[�N
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_POS_Update( DEBUG_PRINT_WORK *p_print, TOWNMAP_WORK *p_wk )
{	
	const PLACE_WORK *cp_place	= &p_wk->place;

	GF_ASSERT(p_wk);
	GF_ASSERT(cp_place);

	if( !DEBUGPRINT_IsOpen(p_print) )
	{	
		return;
	}

	DEBUGPRINT_Clear(p_print);
	{	
		int i;
		const PLACE_DATA* cp_placedata;
		GFL_POINT	start;
		GFL_POINT	end;
		int w;
		u8	plt;
		for( i = 0; i < cp_place->data_num; i++ )
		{	
			cp_placedata	= &cp_place->p_place[i];

			if( cp_placedata )
			{	
				start.x	= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_START_X )+ cp_place->pos.x;
				start.y	= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_START_Y )+ cp_place->pos.y;
				end.x		= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_END_X ) + cp_place->pos.x;
				end.y		= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_END_Y ) + cp_place->pos.y;
				w				= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_WIDTH );
			
				if(p_wk->checkpos_num_debug == i)
				{	
					plt = 3;
				}else{	
					plt	= 1;
				}
				BMP_DrawBoldLine( p_print->p_bmp, &start, &end, w, plt );
			}
		}
	}
}

#endif //DEBUG_POS_CHECK

//=============================================================================
/**
 *	DEBUG_MENU_FUNC
 */
//=============================================================================
#ifdef DEBUG_MENU_USE
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g�p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_UPDATE_Print( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
#if 0
		if( p_wk->is_print_debug )
		{	
			DEBUGPRINT_Clear(sp_dp_wk);
			DEBUGPRINT_Close(sp_dp_wk);
		}
		else
		{	
			if( DEBUGPRINT_IsOpen(sp_dp_wk) )
			{	
				DEBUGPRINT_Clear(sp_dp_wk);
				DEBUGPRINT_Close(sp_dp_wk);
			}

			DEBUGPRINT_Open(sp_dp_wk);
			DEBUGPRINT_Update(sp_dp_wk, p_wk_adrs );
		}
#endif
		s_is_print_debug	^= TRUE;
		DEBUGWIN_RefreshScreen();
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g�p�`��֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_Print( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	if( s_is_print_debug )
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "�J�[�\������[ON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "�J�[�\������[OFF]" );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����蔻���ʒu���`�F�b�N�\���p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_UPDATE_CheckPos( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
		if( p_wk->is_checkpos_debug )
		{	
			DEBUGPRINT_Clear(sp_dp_wk);
			DEBUGPRINT_Close(sp_dp_wk);
			p_wk->is_checkpos_debug	= FALSE;
		}
		else
		{
			if( DEBUGPRINT_IsOpen(sp_dp_wk) )
			{	
				DEBUGPRINT_Clear(sp_dp_wk);
				DEBUGPRINT_Close(sp_dp_wk);
			}

			DEBUGPRINT_Open(sp_dp_wk);
			DEBUGPRINT_POS_Update( sp_dp_wk, p_wk_adrs );
			p_wk->is_checkpos_debug	= TRUE;
		}
		DEBUGWIN_RefreshScreen();
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����蔻���ʒu���`�F�b�N�\���p�`��֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_CheckPos( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	if( p_wk->is_checkpos_debug )
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "������͂�Ă�[ON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "������͂�Ă�[OFF]" );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʒu�����\���p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_UPDATE_CheckPosNum( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
		p_wk->checkpos_num_debug++;
		p_wk->checkpos_num_debug	%= TOWNMAP_DATA_MAX;
		DEBUGWIN_RefreshScreen();

		if( p_wk->is_checkpos_debug == TRUE )
		{	
			DEBUGPRINT_POS_Update( sp_dp_wk, p_wk_adrs );
		}

	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ʒu�����\���p�`��֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_CheckPosNum( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;
	DEBUGWIN_ITEM_SetNameV( p_item, "���傤���傤[%d]", p_wk->checkpos_num_debug );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊOBJ�����ׂĕ\���p�����֐�
 *
 *	@param	void* p_wk_adrs		���[�N
 *	@param	p_item						�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_UPDATE_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
    s_is_place_visible_debug	^= TRUE;

    if( s_is_place_visible_debug )
    { 
      int i;
      PLACE_WORK *p_place  = &p_wk->place;
      PLACE_DATA *p_data;


      for( i = 0; i < p_place->data_num; i++ )
      { 
        p_data	= &p_place->p_place[ i ];
        if( p_data->p_clwk )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_data->p_clwk, TRUE );
          if( p_data->active_anm != PLACEDATA_ANIME_SEQ_VANISH )
          { 
            PlaceData_SetAnmSeq( p_data,  p_data->active_anm );
          }
        }
      }
    }

    DEBUGWIN_RefreshScreen();
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊOBJ�����ׂĕ\���p�`��֐�
 *
 *	@param	void* p_wk_adrs		���[�N
 *	@param	p_item						�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	if( s_is_place_visible_debug )
	{	
		DEBUGWIN_ITEM_SetName( p_item, "OBJ�Ђ傤��[ON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetName( p_item, "OBJ�Ђ傤��[OFF]" );	
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����t���O�p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//----------------------------------------------------------------------------
enum
{	
	DEBUGMENU_ARRIVE_NORMAL,
	DEBUGMENU_ARRIVE_ON,
	DEBUGMENU_ARRIVE_OFF,
	DEBUGMENU_ARRIVE_MAX
};
static void DEBUGMENU_UPDATE_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;
	PLACE_WORK		*p_place	= &p_wk->place;
	PLACE_DATA		*p_data;
	int i;
  u16 arrive_flag;
  GAMEDATA	*p_gdata;

  p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
		p_wk->is_arrive_debug	++;
		p_wk->is_arrive_debug	%= DEBUGMENU_ARRIVE_MAX;

		switch( p_wk->is_arrive_debug )
		{	
		case DEBUGMENU_ARRIVE_NORMAL:
			for( i = 0; i < p_place->data_num; i++ )
			{	
				p_data	= &p_place->p_place[ i ];
        arrive_flag	= TOWNMAP_DATA_GetParam( p_data->cp_data, i, TOWNMAP_DATA_PARAM_ARRIVE_FLAG );
        p_data->is_arrive = TOWNMAP_UTIL_CheckFlag( p_gdata, arrive_flag );
				p_data->is_sky	  = p_data->is_arrive && TOWNMAP_DATA_GetParam( p_data->cp_data, p_data->data_idx, TOWNMAP_DATA_PARAM_SKY_FLAG );
			}
			break;
		case DEBUGMENU_ARRIVE_ON:
			for( i = 0; i < p_place->data_num; i++ )
			{	
				p_data	= &p_place->p_place[ i ];
				p_data->is_sky	= TRUE;
        p_data->is_arrive = TRUE;
			}
			break;
		case DEBUGMENU_ARRIVE_OFF:
			for( i = 0; i < p_place->data_num; i++ )
			{	
				p_data	= &p_place->p_place[ i ];
				p_data->is_sky	= FALSE;
        p_data->is_arrive = FALSE;
			}
			break;
		}
		DEBUGWIN_RefreshScreen();
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�����t���O�p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	switch( p_wk->is_arrive_debug )
	{	
	case DEBUGMENU_ARRIVE_NORMAL:
		DEBUGWIN_ITEM_SetName( p_item, "�Ƃ����Ⴍ�t���O[NORMAL]" );
		break;
	case DEBUGMENU_ARRIVE_ON:
		DEBUGWIN_ITEM_SetName( p_item, "�Ƃ����Ⴍ�t���O[ON]" );	
		break;
	case DEBUGMENU_ARRIVE_OFF:
		DEBUGWIN_ITEM_SetName( p_item, "�Ƃ����Ⴍ�t���O[OFF]" );
		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����t���O�p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//----------------------------------------------------------------------------
static void DEBUGMENU_UPDATE_NotPullFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{ 
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
    s_is_notpull_debug  ^=  TRUE;
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����t���O�p�����֐�
 *
 *	@param	void* p_wk_adrs	���[�N
 *	@param	p_item					�A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGMENU_DRAW_NotPullFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item )
{ 
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	if( s_is_notpull_debug )
	{	
		DEBUGWIN_ITEM_SetName( p_item, "�������݂Ȃ���[ON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetName( p_item, "�������݂Ȃ���[OFF]" );	
	}
}

#endif //DEBUG_MENU_USE
