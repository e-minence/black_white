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
#include "townmap_data_sys.h"

//mine
#include "townmap_grh.h"
#include "app/townmap.h"

//debug
#include "debug/debugwin_sys.h"

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
static GFL_POINT s_debug_pos	=
{	
	0
};
#endif //PM_DEBUG


//-------------------------------------
///	COMMON
//=====================================
#define RULE_PLACE_MSG_MAX	(6)

#define MAP_SCALE_MOVE_LIMIT_TOP		(-192/2)
#define MAP_SCALE_MOVE_LIMIT_LEFT		(-256/2)
#define MAP_SCALE_MOVE_LIMIT_BOTTOM	(MAP_SCALE_MOVE_LIMIT_TOP+192 - (APPBAR_MENUBAR_H*GFL_BG_1CHRDOTSIZ))
#define MAP_SCALE_MOVE_LIMIT_RIGHT	(MAP_SCALE_MOVE_LIMIT_LEFT+256)

//OBJ�D��x
enum
{	
	OBJ_PRIORITY_CURSOR,
	OBJ_PRIORITY_FRONTEND,
	OBJ_PRIORITY_MARK,
	OBJ_PRIORITY_EFFECT,
	OBJ_PRIORITY_MAPOBJ,
};

//���S���W
static const GFL_POINT sc_center_pos =
{	
	256/2, 192/2
};

//-------------------------------------
///	PLACE
//=====================================
#define PLACE_PULL_R				(10)
#define PLACE_PULL_STRONG		(FX32_CONST(2))	//�z�����ދ����i�J�[�\�����キ�j
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
typedef enum
{	
	APPBAR_OPTION_MASK_CLOSE		= 1<<0,	//����
	APPBAR_OPTION_MASK_RETURN		= 1<<1,	//�߂�
	APPBAR_OPTION_MASK_CUR_D		= 1<<2,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_U		= 1<<3,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_L		= 1<<4,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_R		= 1<<5,	//�J�[�\���E
	APPBAR_OPTION_MASK_CHECK		= 1<<6,	//�`�F�b�N�{�b�N�X
	APPBAR_OPTION_MASK_SCALE		= 1<<7,	//�g��k��


	APPBAR_OPTION_MASK_SKY	= APPBAR_OPTION_MASK_CLOSE|APPBAR_OPTION_MASK_RETURN,	//����ԉ�ʂ̂Ƃ�
	APPBAR_OPTION_MASK_TOWN	= APPBAR_OPTION_MASK_CUR_L|APPBAR_OPTION_MASK_CUR_R|
		APPBAR_OPTION_MASK_SCALE|
		APPBAR_OPTION_MASK_CLOSE|APPBAR_OPTION_MASK_RETURN,	//�n�}��ʂ̂Ƃ�
} APPBAR_OPTION_MASK;
//�I���������̎擾
typedef enum
{
	APPBAR_SELECT_NONE	= GFL_UI_TP_HIT_NONE,
	APPBAR_SELECT_CLOSE		= 0,
	APPBAR_SELECT_RETURN,
	APPBAR_SELECT_CUR_D,
	APPBAR_SELECT_CUR_U,
	APPBAR_SELECT_CUR_L,
	APPBAR_SELECT_CUR_R,
	APPBAR_SELECT_CHECK,
	APPBAR_SELECT_SCALE_UP,
	APPBAR_SELECT_SCALE_DOWN,
} APPBAR_SELECT;
//���\�[�X
enum
{	
	APPBAR_RES_COMMON_PLT,
	APPBAR_RES_COMMON_CHR,
	APPBAR_RES_COMMON_CEL,
	APPBAR_RES_SCALE_PLT,
	APPBAR_RES_SCALE_CHR,
	APPBAR_RES_SCALE_CEL,

	APPBAR_RES_MAX
};
//�A�C�R�����
enum
{	
	APPBAR_BARICON_CLOSE,
	APPBAR_BARICON_RETURN,
	APPBAR_BARICON_CUR_D,
	APPBAR_BARICON_CUR_U,
	APPBAR_BARICON_CUR_L,
	APPBAR_BARICON_CUR_R,
	APPBAR_BARICON_CHECK,
	APPBAR_BARICON_SCALE_UP,
	APPBAR_BARICON_SCALE_DOWN,
	APPBAR_BARICON_MAX,
};
//�ʒu
//�o�[
#define APPBAR_MENUBAR_X	(0)
#define APPBAR_MENUBAR_Y	(21)
#define APPBAR_MENUBAR_W	(32)
#define APPBAR_MENUBAR_H	(3)
//CLOSE
#define APPBAR_ICON_Y	(168)
#define APPBAR_ICON_CUR_L_X				(16)
#define APPBAR_ICON_CUR_R_X				(38)
#define APPBAR_ICON_SCALE_DOWN_X	(144)
#define APPBAR_ICON_SCALE_UP_X		(168)
#define APPBAR_ICON_CLOSE_X				(200)
#define APPBAR_ICON_RETURN_X			(232)

#define APPBAR_ICON_W	(24)
#define APPBAR_ICON_H	(24)
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
#define INFO_STR_GUIDE_X	(32)
#define INFO_STR_GUIDE_Y	(80)
#define INFO_STR_PLACE1_X	(32)
#define INFO_STR_PLACE1_Y	(128)
#define INFO_STR_PLACE2_X	(32)
#define INFO_STR_PLACE2_Y	(144)
#define INFO_STR_PLACE3_X	(32)
#define INFO_STR_PLACE3_Y	(160)
#define INFO_STR_PLACE4_X	(144)
#define INFO_STR_PLACE4_Y	(128)
#define INFO_STR_PLACE5_X	(144)
#define INFO_STR_PLACE5_Y	(144)
#define INFO_STR_PLACE6_X	(144)
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
	GFL_CLWK	*p_clwk[APPBAR_BARICON_MAX];
	u32				res[APPBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
	s32				trg;
	s32				cont;
	u32				mode;
} APPBAR_WORK;
#define APPBAR_BG_CHARAAREA_SIZE	(8*8*GFL_BG_1CHRDATASIZ)
//-------------------------------------
///	�J�[�\������
//=====================================
typedef struct 
{
	GFL_CLWK	*p_clwk[CURSOR_CLWK_MAX];
	GFL_CLACTPOS	pos;
	BOOL					is_trg;
	BOOL					is_effect;
} CURSOR_WORK;
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
	u16		param[PLACE_DATA_PARAM_MAX];
	u8		non_active_anm;
	u8		active_anm;
	u16		now_anm;

	fx32	scale;
	BOOL	is_arrive;	//���B�t���O
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
	GFL_CLUNIT			*p_clunit;	//�ꏊ��UNIT��ς���̂ō쐬����
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

	MtxFx22	mtx;
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
	
	//BG�����1�V���N�x�点�邽�߂̃V�[�P���X
	u16	move_seq;
	u16 dummy;


	BOOL	is_place_visible_debug;
	BOOL	is_arrive_debug;
	BOOL	is_checkpos_debug;

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
static void APPBAR_Init( APPBAR_WORK *p_wk, APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, HEAPID heapID );
static void APPBAR_Exit( APPBAR_WORK *p_wk );
static void APPBAR_Main( APPBAR_WORK *p_wk );
static APPBAR_SELECT APPBAR_GetTrg( const APPBAR_WORK *cp_wk );
static APPBAR_SELECT APPBAR_GetCont( const APPBAR_WORK *cp_wk );
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );
//-------------------------------------
///	CURSOR
//=====================================
static void CURSOR_Init( CURSOR_WORK *p_wk, GFL_CLWK *p_cursor, GFL_CLWK *p_ring, HEAPID heapID );
static void CURSOR_Exit( CURSOR_WORK *p_wk );
static void CURSOR_Main( CURSOR_WORK *p_wk, const PLACE_WORK *cp_place );
static void CURSOR_GetPos( const CURSOR_WORK *cp_wk, GFL_POINT *p_pos );
static BOOL CURSOR_GetTrg( const CURSOR_WORK *cp_wk );
static BOOL CURSOR_GetPointTrg( const CURSOR_WORK *cp_wk, u32 *p_x, u32 *p_y );
static void CURSOR_SetEffect( CURSOR_WORK *p_wk, BOOL is_visible );
//-------------------------------------
///	PLACE
//=====================================
static void PLACE_Init( PLACE_WORK *p_wk, u16 now_zone_ID, const TOWNMAP_DATA *cp_data, const TOWNMAP_GRAPHIC_SYS *cp_grh, HEAPID heapID);
static void PLACE_Exit( PLACE_WORK *p_wk );
static void PLACE_Main( PLACE_WORK *p_wk );
static void PLACE_Scale( PLACE_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_start, const GFL_POINT *cp_center_next );
static void PLACE_SetVisible( PLACE_WORK *p_wk, BOOL is_visible );
static void PLACE_SetWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos );
static void PLACE_GetWldPos( const PLACE_WORK *cp_wk, GFL_POINT *p_pos );
static void PLACE_AddWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos );
static const PLACE_DATA* PLACE_Hit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor );
static const PLACE_DATA* PLACE_PullHit( const PLACE_WORK *cp_wk, const CURSOR_WORK *cp_cursor );
static u16 PLACEDATA_GetParam( const PLACE_DATA *cp_wk, TOWNMAP_DATA_PARAM param );
static void PLACE_Active( PLACE_WORK *p_wk, const PLACE_DATA *cp_data );
static void PLACE_NonActive( PLACE_WORK *p_wk );
static void PlaceData_Init( PLACE_DATA *p_wk, const TOWNMAP_DATA *cp_data, u32 data_idx, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, HEAPID heapID );
static void PlaceData_Exit( PLACE_DATA *p_wk );
static void PlaceData_SetVisible( PLACE_DATA *p_wk, BOOL is_visible );
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
static void MAP_Main( MAP_WORK *p_wk );
static void MAP_Scale( MAP_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_start, const GFL_POINT *cp_center_end );
static BOOL MAP_IsScaleEnd( const MAP_WORK *cp_wk );
static BOOL MAP_IsScale( const MAP_WORK *cp_wk );
static void MAP_SetWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos );
static void MAP_GetWldPos( const MAP_WORK *cp_wk, GFL_POINT *p_pos );
static void MAP_AddWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos );
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
///	ETC
//=====================================
static void BMP_DrawLine( GFL_BMP_DATA *p_bmp, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u8 plt_num );

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

#define DEBUGPRINT_CHAR_TEMP_AREA (0x4000)
#define DEBUGPRINT_SCRN_TEMP_AREA (0x800)
#define DEBUGPRINT_PLTT_TEMP_AREA (0x20)
#define DEBUGPRINT_WIDTH  (32)
#define DEBUGPRINT_HEIGHT (18)

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
static void DEBUGMENU_UPDATE_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_VisiblePlace( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_UPDATE_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
static void DEBUGMENU_DRAW_ArriveFlag( void* p_wk_adrs, DEBUGWIN_ITEM* p_item );
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
	TOWNMAP_WORK	*p_wk;
	TOWNMAP_PARAM	*p_param	= p_param_adrs;
	u16	data_len;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TOWNMAP, 0x60000 );

	//���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(TOWNMAP_WORK), HEAPID_TOWNMAP );
	GFL_STD_MemClear( p_wk, sizeof(TOWNMAP_WORK) );
	p_wk->p_param	= p_param;

	//���ʃV�X�e���쐬---------------------
	//�t�H���g
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
    NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_TOWNMAP );

	//�n�����b�Z�[�W�f�[�^
	p_wk->p_place_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, HEAPID_TOWNMAP );
	//�K�C�h���b�Z�[�W�f�[�^
	p_wk->p_guide_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, HEAPID_TOWNMAP );

	//���W���[���쐬----------------------
	//�O���t�B�b�N�쐬
	p_wk->p_grh	= TOWNMAP_GRAPHIC_Init( HEAPID_TOWNMAP );

	//�V�[�P���X�쐬
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );

	//�A�v���P�[�V�����o�[�쐬
	{	
		u32 appbar_mode;

		if( p_wk->p_param->mode == TOWNMAP_MODE_SKY )
		{	
			appbar_mode	= APPBAR_OPTION_MASK_SKY;

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
		else
		{	
			appbar_mode	= APPBAR_OPTION_MASK_TOWN;
		}

		APPBAR_Init( &p_wk->appbar,appbar_mode,
				TOWNMAP_GRAPHIC_GetUnit( p_wk->p_grh, TOWNMAP_OBJ_CLUNIT_DEFAULT ),
				TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_BAR_M),
				TOWNMAP_BG_PAL_M_15, TOWNMAP_OBJ_PAL_M_12, HEAPID_TOWNMAP );
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


	//�J�[�\���쐬
	CURSOR_Init( &p_wk->cursor, TOWNMAP_GRAPHIC_GetClwk( p_wk->p_grh, TOWNMAP_OBJ_CLWK_CURSOR ),
			TOWNMAP_GRAPHIC_GetClwk( p_wk->p_grh, TOWNMAP_OBJ_CLWK_RING_CUR ), HEAPID_TOWNMAP );

	//�}�b�v�f�[�^�쐬
	p_wk->p_data	= TOWNMAP_DATA_Alloc( HEAPID_TOWNMAP );

	//�ꏊ�쐬
	PLACE_Init( &p_wk->place, 0, p_wk->p_data, p_wk->p_grh, HEAPID_TOWNMAP );

	//�n�}�쐬
	MAP_Init( &p_wk->map,
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_MAP_M ),
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_ROAD_M ),
			HEAPID_TOWNMAP );

	//���ʏ��쐬
	INFO_Init( &p_wk->info, TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_S ),
			p_wk->p_font, p_wk->p_place_msg, p_wk->p_guide_msg, HEAPID_TOWNMAP );


#ifdef DEBUG_MENU_USE

	//�f�o�b�O���j���[
	DEBUGWIN_InitProc( TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_DEBUG_S ),
			p_wk->p_font );
	DEBUGWIN_ChangeLetterColor( 0, 31, 0 );
	DEBUGWIN_ChangeSelectColor( 31, 0, 0 );
	DEBUGWIN_AddGroupToTop( 0 , "�^�E���}�b�v" , HEAPID_TOWNMAP );
#endif //DEBUG_MENU_USE

	//�f�o�b�O�\��
#ifdef DEBUG_PRINT_USE
	sp_dp_wk	= DEBUGPRINT_Init( TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_FONT_M ), 
			DEBUGPRINT_SAVE_TYPE_HEAP, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_Print, DEBUGMENU_DRAW_Print, p_wk, 0, HEAPID_TOWNMAP );
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_ArriveFlag, DEBUGMENU_DRAW_ArriveFlag, p_wk, 0, HEAPID_TOWNMAP );

#ifdef DEBUG_POS_CHECK
	DEBUGWIN_AddItemToGroupEx( DEBUGMENU_UPDATE_CheckPos, DEBUGMENU_DRAW_CheckPos, p_wk, 0, HEAPID_TOWNMAP );
#endif //DEBUG_POS_CHECK

#endif //DEBUG_PRINT_USE

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

	//���W���[���j��-------------

	//�f�o�b�O�j��
#ifdef DEBUG_PRINT_USE
	DEBUGPRINT_Exit(sp_dp_wk);
#endif //DEBUG_PRINT_USE
#ifdef DEBUG_MENU_USE
	DEBUGWIN_RemoveGroup( 0 );
	DEBUGWIN_ExitProc();
#endif //DEBUG_MENU_USE

	//���ʏ��j��
	INFO_Exit( &p_wk->info );

	//�n�}�j��
	MAP_Exit( &p_wk->map );

	//�ꏊ�j��
	PLACE_Exit( &p_wk->place );

	//�}�b�v�f�[�^�j��
	TOWNMAP_DATA_Free( p_wk->p_data );

	//�J�[�\���j��
	CURSOR_Exit( &p_wk->cursor );

	//�n���E�B���h�E�j��
	PLACEWND_Exit( &p_wk->placewnd );

	//����ԃ��[�h�Ȃ�΃��b�Z�[�W�ʔj��
	if( p_wk->p_param->mode == TOWNMAP_MODE_SKY )
	{	
		MSGWND_Exit( &p_wk->msgwnd );
	}
	//�A�v���P�[�V�����o�[�j��
	APPBAR_Exit( &p_wk->appbar );

	//�V�[�P���X�j��
	SEQ_Exit( &p_wk->seq );	

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

	//�I��
	if( SEQ_IsEnd( &p_wk->seq ) )
	{	
		return GFL_PROC_RES_FINISH;
	}
	else
	{	
		return GFL_PROC_RES_CONTINUE;
	}

#ifdef PM_DEBUG
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
#endif //PM_DEBUG
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

	//�J�[�\���̏ꏊ�����蔻��
	{	
		const PLACE_DATA *cp_data;
		cp_data	= PLACE_Hit( &p_wk->place, &p_wk->cursor );
		if( cp_data != p_wk->cp_select && cp_data != NULL )
		{	
			INFO_Update( &p_wk->info, cp_data );
			PLACEWND_Start( &p_wk->placewnd, cp_data );
			PLACE_Active( &p_wk->place, cp_data );
			p_wk->cp_select	= cp_data;

			if( PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE_TYPE) == TOWNMAP_PLACETYPE_ROAD )
			{	
				CURSOR_SetEffect( &p_wk->cursor, TRUE );
			}
			else
			{	
				CURSOR_SetEffect( &p_wk->cursor, FALSE );
			}
		}
		else if( cp_data == NULL && p_wk->cp_select )
		{	
			INFO_Clear( &p_wk->info );
			PLACEWND_SetVisible( &p_wk->placewnd, FALSE );
			PLACE_NonActive( &p_wk->place );
			CURSOR_SetEffect( &p_wk->cursor, FALSE );
			p_wk->cp_select	= NULL;
		}
		//�I������
		if( p_wk->p_param->mode == TOWNMAP_MODE_SKY )
		{	
			if( cp_data != NULL && CURSOR_GetTrg( &p_wk->cursor ) )
			{	
				p_wk->p_param->select	= TOWNMAP_SELECT_SKY;
				p_wk->p_param->zoneID	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID );
				p_wk->p_param->grid.x	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_WARP_X );
				p_wk->p_param->grid.y	= PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_WARP_Y );
				SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
			}
		}
	}

	//�J�[�\���̒n�}�ړ�
	if( p_wk->is_scale )
	{	
		BOOL	is_move	= FALSE;
		GFL_POINT	pos;
		GFL_POINT	add	= {0,0};
		CURSOR_GetPos( &p_wk->cursor, &pos );
		if( pos.y < CURSOR_MAP_MOVE_RANGE_TOP )
		{	
			add.y -= 4;
			is_move	= TRUE;
		}
		if( CURSOR_MAP_MOVE_RANGE_BOTTOM_APPBAR >= pos.y && pos.y > CURSOR_MAP_MOVE_RANGE_BOTTOM )
		{	
			add.y += 4;
			is_move	= TRUE;
		}
		if( pos.x < CURSOR_MAP_MOVE_RANGE_LEFT )
		{	
			add.x -= 4;
			is_move	= TRUE;
		}
		if( pos.x > CURSOR_MAP_MOVE_RANGE_RIGHT )
		{	
			add.x += 4;
			is_move	= TRUE;
		}

		if( is_move )
		{	
			GFL_POINT	pos;
			PLACE_GetWldPos( &p_wk->place, &pos );
			NAGI_Printf( "POS X%d Y%d\n", pos.x, pos.y );
			NAGI_Printf( "LIM L%d T%d R%d B%d\n", 
					p_wk->place.limit_left, p_wk->place.limit_top, 
					p_wk->place.limit_right, p_wk->place.limit_bottom );


			//BG�̈ړ���x�点�鏈��
			switch( p_wk->move_seq )
			{	
			default:
				MAP_AddWldPos( &p_wk->map, &add );
				//OBJ��BG�͕������t
				add.x	*= -1;
				add.y *= -1;
				PLACE_AddWldPos( &p_wk->place, &add );
			}
		}
	}

	///���W���[�����C������
	APPBAR_Main( &p_wk->appbar );
	CURSOR_Main( &p_wk->cursor, &p_wk->place );
	PLACE_Main( &p_wk->place );
	MAP_Main( &p_wk->map );
	PLACEWND_Main( &p_wk->placewnd );

	//�A�v���P�[�V�����o�[�I��
	switch( APPBAR_GetTrg( &p_wk->appbar ) )
	{	
	case APPBAR_SELECT_CLOSE:
		p_wk->p_param->select	= TOWNMAP_SELECT_CLOSE;
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
		break;
	case APPBAR_SELECT_RETURN:
		p_wk->p_param->select	= TOWNMAP_SELECT_RETURN;
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
		break;
	case APPBAR_SELECT_SCALE_UP:
		if(p_wk->is_scale == FALSE && !MAP_IsScale( &p_wk->map ))
		{	
			GFL_POINT next_pos;
			CURSOR_GetPos( &p_wk->cursor, &next_pos );
			PLACE_Scale( &p_wk->place, FX32_CONST(2), &sc_center_pos, &next_pos );
			MAP_Scale( &p_wk->map, FX32_CONST(0.5), &sc_center_pos, &next_pos );

			PLACE_SetVisible( &p_wk->place, FALSE );

			p_wk->is_scale	= TRUE;
		}
		break;
	case APPBAR_SELECT_SCALE_DOWN:
		if(p_wk->is_scale == TRUE && !MAP_IsScale( &p_wk->map ) )
		{	
			GFL_POINT src_pos;
			MAP_GetWldPos( &p_wk->map, &src_pos );
			//NAGI_Printf( "WLD X%d Y%d\n", src_pos.x, src_pos.y );
			src_pos.x	= src_pos.x + 256/2;
			src_pos.y	= src_pos.y + 192/2;
			PLACE_Scale( &p_wk->place, FX32_CONST(1), &src_pos, &sc_center_pos );
			MAP_Scale( &p_wk->map, FX32_CONST(1), &src_pos, &sc_center_pos );
			
			PLACE_SetVisible( &p_wk->place, FALSE );

			p_wk->is_scale	= FALSE;
		}
		break;
	}

	//�X�P�[���I����̏ꏊ�\������
	if( MAP_IsScaleEnd( &p_wk->map ) )
	{	
		PLACE_SetVisible( &p_wk->place, TRUE );
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
 *	@param	mask							���[�h�}�X�N
 *	@param	p_unit						APPBAR��OBJ�����pp_unit
 *	@param	bar_frm						�g�p����BG�ʁi�����Ƀ��C�����T�u���𔻒肷��̂ɂ��g�p�j
 *	@param	bg_plt						�g�p����BG�p���b�g�ԍ�
 *	@param	obj_plt						�g�p����OBJ�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Init( APPBAR_WORK *p_wk, APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, HEAPID heapID )
{	
	CLSYS_DRAW_TYPE	clsys_draw_type;
	CLSYS_DEFREND_TYPE	clsys_def_type;
	PALTYPE							bgplttype;

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
	p_wk->bg_frm	= bar_frm;
	p_wk->trg			= APPBAR_SELECT_NONE;
	p_wk->cont		= APPBAR_SELECT_NONE;
	p_wk->mode		= mask;

	//OBJ�ǂݍ��ޏꏊ���`�F�b�N
	{	
		if( bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}
	}
	
	//���ʃ��\�[�X
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

		//BG
		//�̈�̊m��
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
				bgplttype, bg_plt*0x20, APP_COMMON_BAR_PLT_NUM*0x20, heapID );

		p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, APPBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
		GF_ASSERT( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL );
		//�X�N���[���̓�������ɒu���āA����32*3������������
		ARCHDL_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), 0, 21, 32, 24, 
				APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, APPBAR_MENUBAR_W, APPBAR_MENUBAR_H,
				bg_plt, FALSE, heapID );

		//NAGI_Printf( "pos %d size %d\n", GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos) );

		//OBJ
		//���ʃA�C�R�����\�[�X
		if( (p_wk->mode & APPBAR_OPTION_MASK_CLOSE)
				|| (p_wk->mode & APPBAR_OPTION_MASK_RETURN)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_D)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_U)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_L)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_R)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CHECK)
				)
		{	
			p_wk->res[APPBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, APP_COMMON_BARICON_PLT_NUM, heapID );	
			p_wk->res[APPBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
					APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

			p_wk->res[APPBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
					APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
				 	APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), heapID );
		}
		GFL_ARC_CloseDataHandle( p_handle );
	}

	//�A�v�����Ƃ̃��\�[�X
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );
		if( (p_wk->mode & APPBAR_OPTION_MASK_SCALE) )
		{	
			p_wk->res[APPBAR_RES_SCALE_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				NARC_townmap_gra_tmap_bg_d_NCLR, clsys_draw_type, (obj_plt+2)*0x20,
				2, 1, heapID );	

			p_wk->res[APPBAR_RES_SCALE_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_townmap_gra_tmap_obj_d_NCGR, FALSE, clsys_draw_type, heapID );

			p_wk->res[APPBAR_RES_SCALE_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_townmap_gra_tmap_obj_d_NCER,
				 	NARC_townmap_gra_tmap_obj_d_NANR, heapID );
		}

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�̍쐬
	{
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		//�I���{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CLOSE )
 		{	
			cldata.pos_x	= APPBAR_ICON_CLOSE_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CLOSE;
			p_wk->p_clwk[APPBAR_BARICON_CLOSE]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//�߂�{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_RETURN )
 		{	
			cldata.pos_x	= APPBAR_ICON_RETURN_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_RETURN;
			p_wk->p_clwk[APPBAR_BARICON_RETURN]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//���{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CUR_L )
 		{	
			cldata.pos_x	= APPBAR_ICON_CUR_L_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CURSOR_LEFT;
			p_wk->p_clwk[APPBAR_BARICON_CUR_L]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//�E�{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CUR_R )
 		{	
			cldata.pos_x	= APPBAR_ICON_CUR_R_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CURSOR_RIGHT;
			p_wk->p_clwk[APPBAR_BARICON_CUR_R]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//���ꃊ�\�[�X�j��
		if( p_wk->mode & APPBAR_OPTION_MASK_SCALE )
		{	
			cldata.pos_x	= APPBAR_ICON_SCALE_UP_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= 7;
			p_wk->p_clwk[APPBAR_BARICON_SCALE_UP]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_SCALE_CHR],
					p_wk->res[APPBAR_RES_SCALE_PLT],
					p_wk->res[APPBAR_RES_SCALE_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);

			cldata.pos_x	= APPBAR_ICON_SCALE_DOWN_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= 8;
			p_wk->p_clwk[APPBAR_BARICON_SCALE_DOWN]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_SCALE_CHR],
					p_wk->res[APPBAR_RES_SCALE_PLT],
					p_wk->res[APPBAR_RES_SCALE_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
			GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[APPBAR_BARICON_SCALE_DOWN], obj_plt+3, CLWK_PLTTOFFS_MODE_PLTT_TOP );
			GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[APPBAR_BARICON_SCALE_UP], obj_plt+3, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		}
	}

	//�ݒ�
	{	
		int i;
		for( i = 0; i < APPBAR_BARICON_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[i], TOWNMAP_BG_PRIORITY_BAR_M );
				GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[i], OBJ_PRIORITY_FRONTEND );
			}
		}
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
	//CLWK
	{	
		int i;
		for( i = 0; i < APPBAR_BARICON_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//OBJ
	{	
		//���ꃊ�\�[�X�j��
		if( p_wk->mode & APPBAR_OPTION_MASK_SCALE )
		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_SCALE_CEL] );
			GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_SCALE_CHR] );
			GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_SCALE_PLT] );
		}

		//���ʃ��\�[�X�j��
		if( (p_wk->mode & APPBAR_OPTION_MASK_CLOSE)
				|| (p_wk->mode & APPBAR_OPTION_MASK_RETURN)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_D)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_U)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_L)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_R)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CHECK)
				)
 		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_COMMON_CEL] );
			GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_COMMON_CHR] );
			GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_COMMON_PLT] );
		}
	}

	//BG
	{	
		GFL_BG_FreeCharacterArea(p_wk->bg_frm,
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));
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
static void APPBAR_Main( APPBAR_WORK *p_wk )
{	
	static const GFL_UI_TP_HITTBL	sc_hit_tbl[APPBAR_BARICON_MAX]	=
	{	
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CLOSE_X, APPBAR_ICON_CLOSE_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_RETURN_X, APPBAR_ICON_RETURN_X + APPBAR_ICON_W
		},
		{	
			0
		},
		{	
			0
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CUR_L_X, APPBAR_ICON_CUR_L_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CUR_R_X, APPBAR_ICON_CUR_R_X + APPBAR_ICON_W
		},
		{	
			0
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_SCALE_UP_X, APPBAR_ICON_SCALE_UP_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_SCALE_DOWN_X, APPBAR_ICON_SCALE_DOWN_X + APPBAR_ICON_W
		},
		
	};
	static const u32 sc_mask_tbl[APPBAR_BARICON_MAX]	=
	{	
		APPBAR_OPTION_MASK_CLOSE,
		APPBAR_OPTION_MASK_RETURN,
		APPBAR_OPTION_MASK_CUR_D,
		APPBAR_OPTION_MASK_CUR_U,
		APPBAR_OPTION_MASK_CUR_L,
		APPBAR_OPTION_MASK_CUR_R,
		APPBAR_OPTION_MASK_CHECK,
		APPBAR_OPTION_MASK_SCALE,
		APPBAR_OPTION_MASK_SCALE,
	};
	
	//�^�b�`�̓���
	int i;
	u32 x, y;

	p_wk->trg		= APPBAR_SELECT_NONE;
	p_wk->cont	= APPBAR_SELECT_NONE;
	for( i = 0; i < NELEMS(sc_hit_tbl); i++ )
	{	
		if( p_wk->mode & sc_mask_tbl[i] )
		{	
#if 0
			if( CURSOR_GetPointTrg( cp_cursor, &x, &y ) )
#else
			if( GFL_UI_TP_GetPointTrg( &x, &y ) )
#endif 
			{	
				if( ((u32)( x - sc_hit_tbl[i].rect.left) <= 
							(u32)(sc_hit_tbl[i].rect.right - sc_hit_tbl[i].rect.left))
						&	((u32)( y - sc_hit_tbl[i].rect.top) <=
							(u32)(sc_hit_tbl[i].rect.bottom - sc_hit_tbl[i].rect.top)))
				{
					p_wk->trg	= i;
				}
			}
#if 0
			if( GFL_UI_TP_GetPointCont( &x, &y ) )
			{	
				if( ((u32)( x - sc_hit_tbl[i].rect.left) <= 
							(u32)(sc_hit_tbl[i].rect.right - sc_hit_tbl[i].rect.left))
						&	((u32)( y - sc_hit_tbl[i].rect.top) <=
							(u32)(sc_hit_tbl[i].rect.bottom - sc_hit_tbl[i].rect.top)))
				{
					p_wk->cont	= i;
				}
			}
#endif
		}
	}

	//�L�[�̓���
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{	
		p_wk->trg	= APPBAR_SELECT_RETURN;
	}
	else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
	{	
		p_wk->trg	= APPBAR_SELECT_CLOSE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{	
		p_wk->cont	= APPBAR_SELECT_CUR_L;
	}
	else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{	
		p_wk->cont	= APPBAR_SELECT_CUR_R;
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
static APPBAR_SELECT APPBAR_GetTrg( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->trg;
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
static APPBAR_SELECT APPBAR_GetCont( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->cont;
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
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
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[CURSOR_CLWK_RING],	TOWNMAP_BG_PRIORITY_TOP_M );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[CURSOR_CLWK_RING], OBJ_PRIORITY_EFFECT );

		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[CURSOR_CLWK_RING], TRUE );
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
		norm.y	-= FX32_ONE;
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
	{	
		norm.y	+= FX32_ONE;
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{	
		norm.x	-= FX32_ONE;
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
		is_move	= TRUE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{	
		norm.x	+= FX32_ONE;
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], TRUE );
		is_move	= TRUE;
	}


	//�z�����܂�鏈��
	//�z�����ݔ͈͓��Ȃ�΋z�����܂��
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

		p_wk->pos.x	+= add.x >> FX32_SHIFT; 
		p_wk->pos.y	+= add.y >> FX32_SHIFT;
		p_wk->pos.x	= MATH_CLAMP( p_wk->pos.x, CURSOR_MOVE_LIMIT_LEFT, CURSOR_MOVE_LIMIT_RIGHT );
		p_wk->pos.y	= MATH_CLAMP( p_wk->pos.y, CURSOR_MOVE_LIMIT_TOP, CURSOR_MOVE_LIMIT_BOTTOM );
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_CURSOR], &p_wk->pos, 0 );
	}

	//����
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
	{	
		p_wk->is_trg	= TRUE;
	}

	//�^�b�`����
	{	
		u32 x, y;
		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_CURSOR], FALSE );
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

	//�J�[�\���G�t�F�N�g�̐ݒ�
	if(p_wk->is_effect )
	{	
		GFL_CLACT_WK_SetPos( p_wk->p_clwk[CURSOR_CLWK_RING], &p_wk->pos, 0 );
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
 *	@brief	���oOBJ���o��
 *
 *	@param	CURSOR_WORK *p_wk	���[�N
 *	@param	is_visible				TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void CURSOR_SetEffect( CURSOR_WORK *p_wk, BOOL is_visible )
{	
	p_wk->is_effect	= is_visible;
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CURSOR_CLWK_RING], is_visible );
	if( is_visible )
	{	
		GFL_CLACT_WK_ResetAnm( p_wk->p_clwk[CURSOR_CLWK_RING] );
	}
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
static void PLACE_Init( PLACE_WORK *p_wk, u16 now_zone_ID, const TOWNMAP_DATA *cp_data, const TOWNMAP_GRAPHIC_SYS *cp_grh, HEAPID heapID)
{	
	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(PLACE_WORK) );

	//���j�b�g�쐬
	//���[�U�[�����_���[���j�b�g
	{	
		static const GFL_REND_SURFACE_INIT sc_rend_surface_init =
		{	
			0,0,256,192,
			CLSYS_DRAW_MAIN
		};
		p_wk->p_clunit	= GFL_CLACT_UNIT_Create( 128, 0, heapID );
		p_wk->p_rend		= GFL_CLACT_USERREND_Create( &sc_rend_surface_init, 1, heapID );
		GFL_CLACT_UNIT_SetUserRend( p_wk->p_clunit, p_wk->p_rend );
	}

	//�쐬����CLWK�̐����v�Z
	{	
		int i;
		u16 type;
#if 0
		p_wk->data_num	= 0;	//�O�ɂ��Ă���J�E���g
		for( i = 0; i < TOWNMAP_DATA_MAX; i++ )
		{	
			type	= TOWNMAP_DATA_GetParam( cp_data, i, TOWNMAP_DATA_PARAM_PLACE_TYPE );
			//���ƃ_���W���������Z�������
			if( 1	)
			{	
				p_wk->data_num++;
			}
		}
#else
		p_wk->data_num	= TOWNMAP_DATA_MAX;
#endif
	}
	
	//�ݒ肳��Ă��鐔��CLWK���o�����߁A�o�b�t�@�쐬
	p_wk->p_place	= GFL_HEAP_AllocMemory( heapID, sizeof(PLACE_DATA) * p_wk->data_num );
	GFL_STD_MemClear( p_wk->p_place, sizeof(PLACE_DATA) * p_wk->data_num );

	//CLWK�쐬
	{	
		int i;
		int place_cnt;
		u32 chr, cel, plt;
		u16 type;
		//���\�[�X�擾
		TOWNMAP_GRAPHIC_GetObjResource( cp_grh, &chr, &cel, &plt );
		//������
		place_cnt	= 0;
		for( i = 0; i < TOWNMAP_DATA_MAX; i++ )
		{	
			type	= TOWNMAP_DATA_GetParam( cp_data, i, TOWNMAP_DATA_PARAM_PLACE_TYPE );
			
			if( 1	)
			{	
				PlaceData_Init( &p_wk->p_place[place_cnt], cp_data, i, p_wk->p_clunit, chr, cel, plt, heapID );
				place_cnt++;
			}
		}
	}

	//Player�̍쐬
	{	
		u32 chr, cel, plt;
		const PLACE_DATA *cp_data	= NULL;
		int i;
		//�]�[���ɑΉ�����ꏊ���擾
		for( i = 0; i < p_wk->data_num; i++ )
		{	
			cp_data	= &p_wk->p_place[i];
			if( now_zone_ID == PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_ZONE_ID ) )
			{	
				break;
			}
		}
		//���\�[�X�擾
		TOWNMAP_GRAPHIC_GetObjResource( cp_grh, &chr, &cel, &plt );
		//�쐬
		PLACEMARK_Init( &p_wk->player, cp_data, p_wk->p_clunit, chr, cel, plt, heapID );
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

	//�����_���j��
	{	
		GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );
		GFL_CLACT_USERREND_Delete( p_wk->p_rend );
	}

	//�E�j�b�g�j��
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );

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
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꏊ	�X�P�[���A�b�v�J�n
 *
 *	@param	PLACE_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void PLACE_Scale( PLACE_WORK *p_wk, fx32 scale, const GFL_POINT *cp_center_start, const GFL_POINT *cp_center_next )
{	

	//�g�傷��Ɠ���͈͂��ύX
	{	
		if( scale == FX32_ONE )
		{	
			p_wk->limit_top			= 0;
			p_wk->limit_left		= 0;
			p_wk->limit_right		= 0;
			p_wk->limit_bottom	= 0;
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

		if( scale == FX32_ONE )
		{
			pos.x	= 0;
			pos.y	= 0;
		}
		else
		{	
			//OBJ�͋t
			pos.x	=	-(cp_center_next->x-128);
			pos.y	=	-(cp_center_next->y-96);
		}

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

	//�J�[�\���̍��W���ꏊ���W�n�ɒ���
	CURSOR_GetPos( cp_cursor, &pos );
	pos.x	+= cp_wk->limit_left;
	pos.y	+= cp_wk->limit_top;

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

	//�J�[�\���̍��W���ꏊ���W�n�ɒ���
	CURSOR_GetPos( cp_cursor, &pos );
	pos.x	+= cp_wk->limit_left;
	pos.y	+= cp_wk->limit_top;

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
static u16 PLACEDATA_GetParam( const PLACE_DATA *cp_wk, TOWNMAP_DATA_PARAM param )
{	
	if( TOWNMAP_DATA_PARAM_POS_X <= param && param <= TOWNMAP_DATA_PARAM_HIT_END_Y )
	{	
		//�g��k�����ŕύX����f�[�^
		return cp_wk->param[ param - TOWNMAP_DATA_PARAM_POS_X ];
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
static void PlaceData_Init( PLACE_DATA *p_wk, const TOWNMAP_DATA *cp_data, u32 data_idx, GFL_CLUNIT *p_clunit, u32 chr, u32 cel, u32 plt, HEAPID heapID )
{	

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(PLACE_DATA) );
	p_wk->cp_data			= cp_data;
	p_wk->data_idx		= data_idx;
	p_wk->scale				= FX32_ONE;
	p_wk->is_arrive		= TRUE;	//�Ƃ肠��������ON

	//�ύX����f�[�^���擾
	{	
		int i;
		for( i = 0; i < PLACE_DATA_PARAM_MAX; i++ )
		{	
			p_wk->param[i]	= TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, 
					TOWNMAP_DATA_PARAM_POS_X+i );
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
	default:
		GF_ASSERT(0);
	}

	//�Z���쐬
	{
		GFL_CLWK_DATA cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_X );
		cldata.pos_y	= PLACEDATA_GetParam( p_wk,TOWNMAP_DATA_PARAM_POS_Y );
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_clunit, chr, plt, cel, &cldata, 0, heapID );

		GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk, CLSYS_AFFINETYPE_DOUBLE );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, TOWNMAP_BG_PRIORITY_BAR_M+1 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, OBJ_PRIORITY_MAPOBJ );
	GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk, CLSYS_AFFINETYPE_DOUBLE );
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
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
		}
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
	
	return GFL_COLLISION3D_CYLXCIR_Check( &cylinder, &circle, NULL );
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
		//���W�͍������X�P�[���{
		p_wk->param[PLACE_DATA_PARAM_POS_X]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_POS_X ) - 
			 cp_center_next->x )* scale) >> FX32_SHIFT) + cp_center_next->x;

		p_wk->param[PLACE_DATA_PARAM_POS_Y]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_POS_Y ) - 
				cp_center_next->y )* scale) >> FX32_SHIFT) + cp_center_next->y;

		p_wk->param[PLACE_DATA_PARAM_CURSOR_X]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_CURSOR_X ) - 
				cp_center_next->x )* scale) >> FX32_SHIFT) + cp_center_next->x;

		p_wk->param[PLACE_DATA_PARAM_CURSOR_Y]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_CURSOR_Y ) - 
				cp_center_next->y) * scale) >> FX32_SHIFT) + cp_center_next->y;

		p_wk->param[PLACE_DATA_PARAM_HIT_START_X]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_START_X ) - 
				cp_center_next->x) * scale) >> FX32_SHIFT) + cp_center_next->x;

		p_wk->param[PLACE_DATA_PARAM_HIT_START_Y]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_START_Y ) - 
				cp_center_next->y) * scale) >> FX32_SHIFT) + cp_center_next->y;

		p_wk->param[PLACE_DATA_PARAM_HIT_END_X]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_END_X ) - 
				cp_center_next->x) * scale) >> FX32_SHIFT) + cp_center_next->x;

		p_wk->param[PLACE_DATA_PARAM_HIT_END_Y]	= 
			(((TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_END_Y ) - 
				cp_center_next->y) * scale) >> FX32_SHIFT) + cp_center_next->y;

		//���͒P���ɃX�P�[���{
		p_wk->param[PLACE_DATA_PARAM_HIT_WIDTH]	= 
			(TOWNMAP_DATA_GetParam( p_wk->cp_data, p_wk->data_idx, TOWNMAP_DATA_PARAM_HIT_WIDTH )
			 * scale) >> FX32_SHIFT;
	
		p_wk->scale	= scale;
	}

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
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
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
	if( p_wk->is_arrive )
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
	if( p_wk->is_arrive )
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
		cldata.pos_x	= PLACEDATA_GetParam(cp_data,TOWNMAP_DATA_PARAM_POS_X );
		cldata.pos_y	= PLACEDATA_GetParam(cp_data,TOWNMAP_DATA_PARAM_POS_Y );
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_clunit, chr, plt, cel, &cldata, 0, heapID );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, TOWNMAP_BG_PRIORITY_BAR_M+1 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, OBJ_PRIORITY_MARK );
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 6 );
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
	clpos.x	= PLACEDATA_GetParam(p_wk->cp_data,TOWNMAP_DATA_PARAM_POS_X ) + cp_wld->x;
	clpos.y	= PLACEDATA_GetParam(p_wk->cp_data,TOWNMAP_DATA_PARAM_POS_Y ) + cp_wld->y;
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
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
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
 */
//-----------------------------------------------------------------------------
static void MAP_Main( MAP_WORK *p_wk )
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

		//���S���W�v�Z
		//���S���W�v�Z�Ƃ������A���S�͏�ɉ�ʒ��S�ŁA
		//���S������Č�����悤�ɍ��W�����炷
		add_pos.x	= (FX32_CONST(p_wk->next_pos.x - p_wk->init_pos.x))/MAP_SCALE_SYNC;
		add_pos.y	= (FX32_CONST(p_wk->next_pos.y - p_wk->init_pos.y))/MAP_SCALE_SYNC;
		//NAGI_Printf( "add X%d Y%d\n", add_pos.x>> FX32_SHIFT, add_pos.y>> FX32_SHIFT );
		p_wk->pos.x	= p_wk->init_pos.x + ((add_pos.x * p_wk->sync) >> FX32_SHIFT);
		p_wk->pos.y	= p_wk->init_pos.y + ((add_pos.y * p_wk->sync) >> FX32_SHIFT);

		//�I������
		if( p_wk->sync++ > MAP_SCALE_SYNC )
		{	
			p_wk->sync	= 0;
			p_wk->is_scale_req	= FALSE;
			p_wk->now_scale		= p_wk->next_scale;
			p_wk->pos					= p_wk->next_pos;

			//���H����
			GFL_BG_SetVisible( p_wk->road_frm, TRUE );

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
		
		//���f
		MTX_Scale22( &p_wk->mtx, p_wk->now_scale, p_wk->now_scale );
		GFL_BG_SetAffine( p_wk->map_frm, &p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
		GFL_BG_SetAffine( p_wk->road_frm, &p_wk->mtx, sc_center_pos.x, sc_center_pos.y );

		GFL_BG_SetAffineScroll( p_wk->map_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x,
				&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
		GFL_BG_SetAffineScroll( p_wk->map_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y,
				&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
		GFL_BG_SetAffineScroll( p_wk->road_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x,
				&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
		GFL_BG_SetAffineScroll( p_wk->road_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y,
				&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
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
	//NAGI_Printf( "Scale start sx%d sy%d ex%d ey%d", p_wk->init_pos.x, p_wk->init_pos.y, 
		//	p_wk->next_pos.x, p_wk->next_pos.y);


	//���H�͏���
	GFL_BG_SetVisible( p_wk->road_frm, FALSE );

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

	GFL_BG_SetAffineScroll( p_wk->map_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x,
			&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
	GFL_BG_SetAffineScroll( p_wk->map_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y,
			&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
	GFL_BG_SetAffineScroll( p_wk->road_frm, GFL_BG_SCROLL_X_SET, p_wk->pos.x,
			&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
	GFL_BG_SetAffineScroll( p_wk->road_frm, GFL_BG_SCROLL_Y_SET, p_wk->pos.y,
			&p_wk->mtx, sc_center_pos.x, sc_center_pos.y );
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
		msgID = PLACEDATA_GetParam( cp_data, TOWNMAP_DATA_PARAM_PLACE5_MSGID);
		if( msgID != TOWNMAP_DATA_ERROR )
		{	
			GFL_MSG_GetString( p_wk->cp_guide_msg, msgID, p_wk->p_strbuf );
			PRINTSYS_Print( p_bmp, INFO_STR_PLACE6_X, INFO_STR_PLACE6_Y, p_wk->p_strbuf, p_wk->p_font );
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
	//�A�b�v�f�[�g�`�F�b�N
	if( p_wk->is_start )
	{	
		p_wk->is_start	= FALSE;

		//OBJ�A�j���J�n
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
		GFL_CLACT_WK_ResetAnm( p_wk->p_clwk );
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

			//�����`��
			MSGWND_Print( &p_wk->msgwnd, ZONEDATA_GetPlaceNameID( zoneID ), PLACEWND_STR_X, PLACEWND_STR_Y );

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
		MSGWND_Clear( &p_wk->msgwnd );
	}
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

	VEC_Subtract( &start, &end, &norm );
	mag	= VEC_Mag( &norm );
	VEC_Normalize( &norm, &norm );

	{	
		int i;
		GFL_POINT pos;

		for( i = 0; i < (mag>>FX32_SHIFT); i++ )
		{	
			pos.x	= (start.x + (norm.x * i) ) >> FX32_SHIFT;
			pos.y	= (start.y + (norm.y * i) ) >> FX32_SHIFT;
			GFL_BMP_Fill( p_bmp, pos.x, pos.y, 128, 128, plt_num );
			NAGI_Printf( "%d %d\n", pos.x, pos.y );
		}
		NAGI_Printf( "SX%d SY%d EX%d EY%d m%d\n", cp_start->x, cp_start->y, cp_end->x, cp_end->y, mag>>FX32_SHIFT );
	}
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
				NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );	

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
		DEBUGPRINT_Print( p_print, L"�J�[�\�����W ", 32, 32 );
		DEBUGPRINT_PrintNumber( p_print, L"X=[%d]", pos.x, 80, 32 );
		DEBUGPRINT_PrintNumber( p_print, L"Y=[%d]", pos.y, 120, 32 );
	}
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

	//InVram��Print�����Ή����Ă��Ȃ�Fill�͂܂�

	{	
		int i;
		const PLACE_DATA* cp_placedata;
		GFL_POINT	start;
		GFL_POINT	end;
		for( i = 0; i < cp_place->data_num; i++ )
		{	
			cp_placedata	= &cp_place->p_place[i];

			if( cp_placedata )
			{	
				start.x		= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_START_Y );
				start.y		= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_START_X );
				end.x			= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_END_Y );
				end.y			= PLACEDATA_GetParam( cp_placedata, TOWNMAP_DATA_PARAM_HIT_END_X );
			
				BMP_DrawLine( p_print->p_bmp, &start, &end, 2 );
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
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
		if( DEBUGPRINT_IsOpen(sp_dp_wk) )
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
	if( DEBUGPRINT_IsOpen(sp_dp_wk) )
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "�f�o�b�O�\��[���݁FON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "�f�o�b�O�\��[���݁FOFF]" );
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
		DEBUGWIN_ITEM_SetNameV( p_item, "�ʒu�\��[���݁FON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetNameV( p_item, "�ʒu�\��[���݁FOFF]" );
	}
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
		p_wk->is_place_visible_debug	^= TRUE;
		PLACE_SetVisible( &p_wk->place, p_wk->is_place_visible_debug );
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

	if( p_wk->is_place_visible_debug )
	{	
		DEBUGWIN_ITEM_SetName( p_item, "�ꏊOBJ�\��[���݁FON]" );
	}
	else
	{	
		DEBUGWIN_ITEM_SetName( p_item, "�ꏊOBJ�\��[���݁FOFF]" );	
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

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {	
		p_wk->is_arrive_debug	++;
		p_wk->is_arrive_debug	%= DEBUGMENU_ARRIVE_MAX;

		switch( p_wk->is_arrive_debug )
		{	
		case DEBUGMENU_ARRIVE_NORMAL:
			break;
		case DEBUGMENU_ARRIVE_ON:
			for( i = 0; i < p_place->data_num; i++ )
			{	
				p_data	= &p_place->p_place[ i ];
				p_data->is_arrive	= TRUE;
			}
			break;
		case DEBUGMENU_ARRIVE_OFF:
			for( i = 0; i < p_place->data_num; i++ )
			{	
				p_data	= &p_place->p_place[ i ];
				p_data->is_arrive	= FALSE;
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
		DEBUGWIN_ITEM_SetName( p_item, "�����t���O����[���݁F�ʏ�]" );
		break;
	case DEBUGMENU_ARRIVE_ON:
		DEBUGWIN_ITEM_SetName( p_item, "�����t���O����[���݁F�S��ON]" );	
		break;
	case DEBUGMENU_ARRIVE_OFF:
		DEBUGWIN_ITEM_SetName( p_item, "�����t���O����[���݁F�S��OFF]" );
		break;
	}
}

#endif //DEBUG_MENU_USE
