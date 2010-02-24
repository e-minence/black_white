//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dchar.c
 *	@brief		wifi�Q�c�L�����N�^�ǂݍ��݃V�X�e��
 *	@author		tomoya takahshi
 *	@data		2007.02.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "system/palanm.h"
#include "arc_def.h"
#include "wifileadingchar.naix"
#include "wifi_unionobj.naix"

#include "net_app/wifi2dmap/wifi_2dchar.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
 */
//-----------------------------------------------------------------------------

//-------------------------------------


#define WF_2DC_RESM_OBJ_NUM	(WF_2DC_CHARNUM+1)	// ���\�[�X��	+1�͉A


// ���\�[�X�^�C�v
typedef enum {
  WF_2DC_RES_NML,	// �ʏ�
  WF_2DC_RES_UNI,	// ���j�I��
} WF_2DC_RESTYPE;

// �A�[�N�n���h���^�C�v
enum{
  WF_2DC_ARCHANDLE_NML,	// �ʏ�
  WF_2DC_ARCHANDLE_UNI,	// ���j�I��
  WF_2DC_ARCHANDLE_NUM,
};


//



#define WF_2DC_RESMAN_NUM	( 4 )	// �쐬���郊�\�[�X�}�l�[�W����
#define WF_2DC_ANMRESMAN_OBJNUM	( (WF_2DC_MOVENUM*2)+WF_2DC_UNICHAR_NUM+1 )	// �Z���ƃA�j���̃��\�[�X�}�l�[�W���I�u�W�F�N�g��	+1�͉A
enum{
  WF_2DC_ANMRES_ANM_CELL,
  WF_2DC_ANMRES_ANM_ANM,
  WF_2DC_ANMRES_ANM_NOFLIP_CELL,
  WF_2DC_ANMRES_ANM_NOFLIP_ANM,
  WF_2DC_ANMRES_ANM_NUM,
};
static const u8 sc_WF_2DC_ANMRES_ANMCONTID[ WF_2DC_ANMRES_ANM_NUM ] = {
  0,0,
  1,1,
};

// �ʏ�L�����N�^�[
#define WF_2DC_ARC_CHARIDX	( NARC_wifileadingchar_hero_NCLR )	// �L�����N�^�O���t�B�b�N�̊J�n
#define WF_2DC_ARC_GETNCL(x)	(((x)*2)+WF_2DC_ARC_CHARIDX)	// �p���b�g�擾
#define WF_2DC_ARC_GETNCG(x)	(WF_2DC_ARC_GETNCL(x) + 1)		// �L�����N�^�擾
#define WF_2DC_ARC_ANMIDX	( NARC_wifileadingchar_hero_NCER )	// ���j�I���A�j���f�[�^
#define WF_2DC_ARC_GETCEL(x)	(WF_2DC_ARC_ANMIDX+((x)*2))	// �Z��dataid�Q�b�g
#define WF_2DC_ARC_GETANM(x)	(WF_2DC_ARC_GETCEL(x)+1)		// �A�j��dataid�Q�b�g

#define WF_2DC_HERO_PLTT_START   ( 8 ) // HERO�L�����N�^���g�p����p���b�g�X�^�[�g�ʒu
#define WF_2DC_HERO_PLTT_NUM		( 2 )	// HERO�L�����N�^���g�p����p���b�g�{��

#define WF_2DC_HERO_KAGE_PLTT_START (10)
#define WF_2DC_HERO_KAGE_PLTT_NUM (1)


// ���j�I���L�����N�^�[
#define WF_2DC_ARC_UNICHARIDX	( NARC_wifi_unionobj_normal00_NCGR )	// �L�����N�^�O���t�B�b�N�̊J�n boy1 ���� normal00�ɒu�������܂��� nagihashi
#define WF_2DC_ARC_GETUNINCL	(NARC_wifi_unionobj_wifi_union_obj_NCLR)	// �p���b�g�擾
#define WF_2DC_ARC_GETUNINCG(x)	(WF_2DC_ARC_UNICHARIDX + x)		// �L�����N�^�擾
#define WF_2DC_UNIPLTT_NUM		( 8 )	// ���j�I���L�����N�^���g�p����p���b�g�{��
#define WF_2DC_ARC_UNIANMIDX	( NARC_wifi_unionobj_normal00_NCER )	// ���j�I���A�j���f�[�^
#define WF_2DC_ARC_GETUNICEL(x)	(WF_2DC_ARC_UNIANMIDX+((x)*2))	// �Z��dataid�Q�b�g
#define WF_2DC_ARC_GETUNIANM(x)	(WF_2DC_ARC_GETUNICEL(x)+1)		// �A�j��dataid�Q�b�g


// ���\�[�X�Ǘ�ID
#define WF_2DC_ARC_CONTANMID	( 0x100 )	// �A�j�����\�[�X�Ǘ�ID
#define WF_2DC_ARC_CONTSHADOWID	( 0x120 )	// �A���\�[�X�Ǘ�ID
#define WF_2DC_ARC_CONTCHARID	( 0x200 )	// �L�����N�^���\�[�X�Ǘ�ID
#define WF_2DC_ARC_CONTUNIONPLTTID	( 0x402 )	// ���j�I���p���b�g���\�[�X�Ǘ�ID
// ���j�I���ŏo�Ă���L�����N�^�̊Ǘ�ID�̏ꏊ�Ȃ̂ő��v
#define WF_2DC_ARC_CONTUNIONANMID	(0x180)	// ���j�I���A�j�����\�[�X�Ǘ�ID

#define WF_2DC_ARC_TURNANMCHARSIZ			(0x800)	// �U������̂ݎ��̃L�����N�^�T�C�Y
#define WF_2DC_ARC_TURNANMCHARSIZ_NOFLIP	(0x800)	// �U������̂ݎ��̃L�����N�^�T�C�Y
#define WF_2DC_ARC_NORMALANMCHARSIZ			(0x1400)// �����{�U������̂ݎ��̃L�����N�^�T�C�Y
#define WF_2DC_ARC_NORMALANMCHARSIZ_NOTFLIP	(0x1800)// �����{�U������̂ݎ��̃L�����N�^�T�C�Y	�t���b�v�Ȃ��̃L�����̂Ƃ�


// �e�A�j���̃t���[����
#define WF_2DC_ANM_WAY_FRAME	(1)		// �����ς��A�j��
#define WF_2DC_ANM_WALK_FRAME	(WF_COMMON_WALK_FRAME)		// �����A�j��
#define WF_2DC_ANM_TURN_FRAME	(WF_COMMON_TURN_FRAME)		// �U������A�j��
#define WF_2DC_ANM_RUN_FRAME	(WF_COMMON_RUN_FRAME)		// ����A�j��
#define WF_2DC_ANM_WALLWALK_FRAME	(WF_COMMON_WALLWALK_FRAME)	// �Ǖ����A�j��
#define WF_2DC_ANM_SLOWWALK_FRAME	(WF_COMMON_SLOWWALK_FRAME)	// �Ǖ����A�j��
#define WF_2DC_ANM_HIGHWALK2_FRAME	(WF_COMMON_HIGHWALK2_FRAME)	// ���������A�j��
#define WF_2DC_ANM_HIGHWALK4_FRAME	(WF_COMMON_HIGHWALK4_FRAME)	// ���������A�j��
#define WF_2DC_ANM_ROTA_FRAME	(0xff)	// ��]�A�j��

// �A�j���̃V�[�P���X��
#define WF_2DC_ANM_ROTA			(4)
#define WF_2DC_ANM_WALK			(5)		// �����A�j���̃V�[�P���X��
#define WF_2DC_ANM_RUN			(9)		// ����A�j���̃V�[�P���X��
#define WF_2DC_ANM_FRAME		(2*FX32_ONE)	// 16 fure
#define WF_2DC_ANM_SLOWFRAME (FX32_ONE)
#define WF_2DC_ANM_HIGH2 (8*FX32_ONE)			//
#define WF_2DC_ANM_HIGH4 (6*FX32_ONE)


// ����
#define WF_2DC_SHADOW_MAT_OFS_X		( 8 )
#define WF_2DC_SHADOW_MAT_OFS_Y		( 14 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	�L�����N�^���\�[�X�f�[�^
//=====================================
typedef struct {
  WF_2DC_RESTYPE type;

  u32 resid[2];	//CG PL���\�[�X�I�u�W�F

  u32					drawtype;	// �]����
  WF_2DC_MOVETYPE		movetype;	// ����^�C�v
} WF_2DCRES;


//-------------------------------------
///	�A�j�����\�[�X�f�[�^
//=====================================
typedef struct {
  void* resobj[WF_2DC_ANMRES_ANM_NUM];	// CE AN���\�[�X�I�u�W�F
  u32 resid;
} WF_2DCANMRES;


//-------------------------------------
/// 2D�L�����N�^���[�N
//=====================================
struct _WF_2DCWK {
  const WF_2DCRES* cp_res;	// ���\�[�X�f�[�^
  GFL_CLWK*	p_clwk;		// �l��
  GFL_CLWK*	p_shadow;	// �A
  WF_2DC_ANMTYPE	patanmtype;
  WF_COMMON_WAY	patanmway;
  s16				patanmframe;
  s16				pad_0;
};

//-------------------------------------
///	�A���\�[�X
//=====================================
typedef struct {
  u32 resid[4];	//�e���\�[�X
} WF_2DCSH_RES;


//-------------------------------------
///	2D�L�����N�^�Ǘ��V�X�e��
//=====================================
struct _WF_2DCSYS {
  GFL_CLUNIT* p_unit;
  PALETTE_FADE_PTR p_pfd;
  WF_2DCWK*		p_wk;
  u32 objnum;
  WF_2DCRES		chrres[ WF_2DC_CHARNUM ];
  WF_2DCANMRES	anmres[ WF_2DC_CHARNUM ];
  WF_2DCSH_RES	shadowres;
  u32				shadow_pri;		// �A�̃\�t�g�D�揇�ʊJ�n�ʒu
  u32 unionplttid;
  ARCHANDLE*	p_handle[WF_2DC_ARCHANDLE_NUM];
  u32 CELLANIMIndex[WF_2DC_MOVENUM][WF_2DC_ANMRES_ANM_NUM];
  u32 hero_no;
} ;


//-------------------------------------
///	�O���t�B�b�N�t�@�C���ƃL�����N�^�R�[�h�������N
//����f�[�^�̍\����
//=====================================
typedef struct {
  u16 code;
  u16 wf2dc_char;
} WF_2DC_CHARDATA;


//-----------------------------------------------------------------------------
/**
 *					�f�[�^��
 */
//-----------------------------------------------------------------------------


/// �e�A�j���̏I���t���[����
static const u8 WF_2DC_AnmFrame[WF_2DC_ANMNUM] = {
  WF_2DC_ANM_WAY_FRAME,
  WF_2DC_ANM_ROTA_FRAME,
  WF_2DC_ANM_WALK_FRAME,
  WF_2DC_ANM_TURN_FRAME,
  WF_2DC_ANM_RUN_FRAME,
  WF_2DC_ANM_WALLWALK_FRAME,
  WF_2DC_ANM_SLOWWALK_FRAME,
  WF_2DC_ANM_HIGHWALK2_FRAME,
  WF_2DC_ANM_HIGHWALK4_FRAME,
  0,
};

//----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
 */
//-----------------------------------------------------------------------------

//-------------------------------------
/// �A�j�����\�[�X�֘A
//=====================================
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype );

static u32 WF_2DC_CharNoGet( u32 view_type );
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype );
static u32 WF_2DC_AnmResContIdGet( WF_2DC_MOVETYPE movetype, u32 res_type, BOOL flip );

//-------------------------------------
/// �L�����E�p���b�g���\�[�X�֘A
//=====================================
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype );
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no );
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no );
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res );
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res );


//-------------------------------------
///	�A���\�[�X�֘A
//=====================================
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap );
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys );
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres );



//-------------------------------------
/// ���j�I�����\�[�X�֘A
//=====================================
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type,int palstartpos, u32 heap );
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys );
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap );
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys );

//-------------------------------------
/// �I�u�W�F���[�N����֘A
//=====================================
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys );
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk );
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk );


//-------------------------------------
/// �A�j���Ǘ��֘A
//=====================================
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );

static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�@2D�`��V�X�e���쐬
 *
 *	@param	p_clset		�g�p����A�N�^�[�Z�b�g
 *	@param	pfd			�p���b�g�t�F�[�h�|�C���^
 *	@param	objnum		�I�u�W�F�N�g��
 *	@param	heap		�q�[�v
 *
 *	@return	�쐬�����V�X�e���|�C���^
 */
//-----------------------------------------------------------------------------
WF_2DCSYS* WF_2DC_SysInit( GFL_CLUNIT* p_unit, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 heap )
{
  WF_2DCSYS* p_sys;
  int i;

  // �������m��
  p_sys = GFL_HEAP_AllocClearMemory( heap, sizeof(WF_2DCSYS) );

  // �A�N�^�[�Z�b�g�͏ォ����炤
  p_sys->p_unit = p_unit;

  // �p���b�g�t�F�[�h�V�X�e��
  p_sys->p_pfd = p_pfd;

  // �A�[�J�C�u�n���h���I�[�v��
  p_sys->p_handle[WF_2DC_ARCHANDLE_NML] = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, heap );
  p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] = GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, heap );


  //    NNS_G2dInitImagePaletteProxy( p_sys->pltProxy[screen] );
  //  GFL_ARC_UTIL_TransVramPaletteMakeProxy( arcfile , arcpal ,
  //                                        hwscreen , 0 , wk->heapid , &wk->clact.pltProxy[screen] );


  // ���\�[�X�}�l�[�W��������
  //	for( i=0; i<2; i++ ){	// CG�APL�́A�L�����N�^��
  //		p_sys->p_res_man[i] = CLACT_U_ResManagerInit( WF_2DC_RESM_OBJ_NUM, i, heap );
  //	}
  //	for( i=0; i<2; i++ ){	// CE,AN�̓A�j���^�C�v��
  //		p_sys->p_res_man[i+2] = CLACT_U_ResManagerInit( WF_2DC_ANMRESMAN_OBJNUM, i+2, heap );
  //	}

  //���\�[�XID�̏�����
  for( i=0; i<WF_2DC_CHARNUM; i++ ){
    p_sys->chrres[i].resid[0] = GFL_CLGRP_REGISTER_FAILED;
  }
  p_sys->shadowres.resid[0] = GFL_CLGRP_REGISTER_FAILED;
  p_sys->unionplttid = GFL_CLGRP_REGISTER_FAILED;

  // �A�j���[�V�������\�[�X�ǂݍ���
  //	for( i=0; i<WF_2DC_MOVENUM; i++ ){
  WF_2DC_AnmResLoad( p_sys, 0, heap );
  //	}

  // �A�N�^�[���[�N�쐬
  p_sys->objnum = objnum;
  p_sys->p_wk = GFL_HEAP_AllocClearMemory( heap, sizeof(WF_2DCWK)*p_sys->objnum );

  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�@2D�`��V�X�e���j��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WF_2DC_SysExit( WF_2DCSYS* p_sys )
{
  int i;

  // ���[�N�j��
  for( i=0; i<p_sys->objnum; i++ ){
    if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == FALSE ){
      WF_2DC_WkDel( &p_sys->p_wk[i] );
    }
  }

  // �A�j���[�V�������\�[�X�j��
  //	for( i=0; i<WF_2DC_MOVENUM; i++ ){
  WF_2DC_AnmResDel( p_sys, 0 );
  //	}

  // �L�����N�^���\�[�X�j��
  WF_2DC_AllResDel( p_sys );

  // �A�[�J�C�u�n���h���N���[�Y
  //	GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_NML] );
  //	GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] );

  GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_NML] );
  GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] );

  // ���\�[�X�}�l�[�W���j��
  //	for( i=0; i<WF_2DC_RESMAN_NUM; i++ ){
  //		CLACT_U_ResManagerDelete( p_sys->p_res_man[i] );
  //	}

  // ���[�N�j��
  GFL_HEAP_FreeMemory( p_sys->p_wk );
  GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�o�^
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	view_type		�����ځiFIELDOBJCODE�j
 *	@param	draw_type		�]����
 *	@param	movetype		����^�C�v
 *	@param	heap			�q�[�v
 *
 *	draw_type
	main�ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAIN
	sub�ɓo�^ �FNNS_G2D_VRAM_TYPE_2DSUB
	�����ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResSet( WF_2DCSYS* p_sys, u32 view_type, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  u32 char_no;
  BOOL result;

  // �ǂݍ��݉\���`�F�b�N
  result = WF_2DC_AnmModeLinkCheck( view_type, movetype );
  GF_ASSERT( result );

  // �����ڂƐ��ʂ���L�����N�^�[�ԍ��擾
  char_no = WF_2DC_CharNoGet( view_type );

  // �ǂݍ���
  WF_2DC_CharResLoad( p_sys, char_no, draw_type, movetype, heap, WF_2DC_RES_NML );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X�j��
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	view_type	������FIELDOBJCODE
 *
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResDel( WF_2DCSYS* p_sys, u32 view_type )
{
  u32 char_no;

  char_no = WF_2DC_CharNoGet( view_type );

  // ���\�[�X�j��
  WF_2DC_CharResDel( p_sys, char_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ǂݍ���ł��邩�`�F�b�N
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *	@param	view_type	�����ځiFIELDOBJCODE�j
 *
 *	@retval	TRUE	�ǂݍ���ł���
 *	@retval	FALSE	�ǂݍ���łȂ�
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_ResCheck( const WF_2DCSYS* cp_sys, u32 view_type )
{
  u32 char_no;

  // �����ڂƐ��ʂ���L�����N�^�[�ԍ��擾
  char_no = WF_2DC_CharNoGet( view_type );
  return WF_2DC_CharResCheck( cp_sys, char_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�S���\�[�X�̔j��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WF_2DC_AllResDel( WF_2DCSYS* p_sys )
{
  int i;

  // ���j�I�����\�[�X�̔j��
  //	if( p_sys->p_unionpltt != NULL ){
  if( p_sys->unionplttid !=  GFL_CLGRP_REGISTER_FAILED){
    WF_2DC_UnionResDel( p_sys );
  }

  // ���̑��̃��\�[�X�͂�
  for( i=0; i<WF_2DC_CHARNUM; i++ ){
    if( WF_2DC_CharResCheck( p_sys, i ) == TRUE ){
      // ���\�[�X�j��
      WF_2DC_CharResDel( p_sys, i );
    }
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�̓o�^
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	draw_type		�]����
 *	@param	movetype		����^�C�v
 *  @param  palstartpos   �p���b�g�J�n�ʒu
 *	@param	heap			�q�[�v
 *
 *	draw_type
	main�ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAIN
	sub�ɓo�^ �FNNS_G2D_VRAM_TYPE_2DSUB
	�����ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResSet( WF_2DCSYS* p_sys, u32 draw_type, WF_2DC_MOVETYPE movetype,int palstartpos, u32 heap )
{
  int i;

  // ����\���`�F�b�N
  // ��l���ȊO�́A����ȊO�󂯕t���Ă��Ȃ�
  GF_ASSERT( movetype != WF_2DC_MOVERUN );


  // ���j�I���A�j�����\�[�X�Ǎ���
  WF_2DC_UniCharPlttResLoad( p_sys, draw_type, palstartpos, heap );

  // �A�j�����\�[�X�o�^
  WF_2DC_UniCharAnmResLoad( p_sys, heap );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    // �ǂݍ���
    WF_2DC_CharResLoad( p_sys, i, draw_type, movetype, heap, WF_2DC_RES_UNI );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�̃��\�[�X�͂�
 *
 *	@param	p_sys			�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResDel( WF_2DCSYS* p_sys )
{
  int i;

  // ���j�I���p���b�g�j��
  WF_2DC_UniCharPlttResDel( p_sys );

  // �A�j�����\�[�X�j��
  WF_2DC_UniCharAnmResDel( p_sys );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    // �L�����N�^�͂�
    WF_2DC_CharResDel( p_sys, i );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�A�̃��\�[�X��ǂݍ���
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	draw_type		�]����
 *	@param	shadow_pri		�A�̕\���D�揇��
 *	@param	heap			�q�[�vID
 *
 *	draw_type
	main�ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAIN
	sub�ɓo�^ �FNNS_G2D_VRAM_TYPE_2DSUB
	�����ɓo�^�FNNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResSet( WF_2DCSYS* p_sys, u32 draw_type, u32 shadow_pri, u32 heap )
{
  WF_2DC_ShResLoad( p_sys, draw_type, heap );
  p_sys->shadow_pri = shadow_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�̃��\�[�X��j������
 *
 *	@param	p_sys		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResDel( WF_2DCSYS* p_sys )
{
  WF_2DC_ShResDel( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�A�N�^�[�o�^
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	cp_data		�o�^�f�[�^
 *	@param	view_type	FIELDOBJCODE
 *	@param	movetype	����^�C�v
 *	@param	heap		�q�[�v
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
WF_2DCWK* WF_2DC_WkAdd( WF_2DCSYS* p_sys, const WF_2DC_WKDATA* cp_data, u32 view_type, u32 heap )
{
  WF_2DCWK* p_wk;
  //	GFL_CLWK_DATA add;
  GFL_CLWK_DATA	cellInitData;
  u32 char_no;

  // ��̃��[�N�擾
  p_wk = WF_2DC_WkCleanGet( p_sys );

  // �L�����N�^�i���o�[�擾
  char_no = WF_2DC_CharNoGet( view_type );

  // �ǂݍ��ݍς݃`�F�b�N
  GF_ASSERT( WF_2DC_CharResCheck( p_sys, char_no ) == TRUE );


  cellInitData.pos_x = cp_data->x;
  cellInitData.pos_y = cp_data->y;
  cellInitData.anmseq = 0;
  cellInitData.softpri = cp_data->pri;
  cellInitData.bgpri = cp_data->bgpri;



  // �\���G���A
  if( (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) ||
      (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
    //		add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
  }else{
    //		add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
  }

  // �A�N�^�[�o�^
  p_wk->p_clwk = GFL_CLACT_WK_Create(p_sys->p_unit,
                                     p_sys->chrres[ char_no ].resid[ 0 ],
                                     p_sys->chrres[ char_no ].resid[ 1 ],
                                     p_sys->anmres[char_no].resid,
                                     &cellInitData, CLSYS_DEFREND_MAIN, heap );

  //	CLACT_BGPriorityChg( p_wk->p_clwk, cp_data->bgpri );
  GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, cp_data->bgpri );

  // ���샂�[�h�ɂ�菉���A�j����ύX
  if( p_sys->chrres[ char_no ].movetype == WF_2DC_MOVENORMAL ){
    // ��������
    GFL_CLACT_WK_SetAnmIndex(p_wk->p_clwk, WF_2DC_ANM_WALK+WF_COMMON_BOTTOM);
  }else{
    // ������
    GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk, WF_COMMON_BOTTOM );
  }

  // �A�j���f�[�^������
  p_wk->patanmtype = WF_2DC_ANMNONE;
  p_wk->patanmframe = 0;
  p_wk->patanmway = WF_COMMON_BOTTOM;

  // ���\�[�X�ۑ�
  p_wk->cp_res = &p_sys->chrres[ char_no ];

  // �A���[�N�̍쐬
  if( WF_2DC_ShResCheck( &p_sys->shadowres ) ){


    cellInitData.pos_x = cp_data->x;
    cellInitData.pos_y = cp_data->y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = p_sys->shadow_pri;
    cellInitData.bgpri = 0;


    // �\���G���A
    if( (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) ||
        (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
      //			add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
    }else{
      //			add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
    }


    p_wk->p_shadow = GFL_CLACT_WK_Create(p_sys->p_unit,
                                         p_sys->shadowres.resid[0],
                                         p_sys->shadowres.resid[1],
                                         p_sys->shadowres.resid[2],
                                         &cellInitData,
                                         CLSYS_DEFREND_MAIN,heap );

    GFL_CLACT_WK_SetBgPri( p_wk->p_shadow, cp_data->bgpri );
  }else{
    p_wk->p_shadow = NULL;
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�^�[�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDel( WF_2DCWK* p_wk )
{

  if( p_wk->p_shadow != NULL ){
    GFL_CLACT_WK_Remove( p_wk->p_shadow );
  }
  GFL_CLACT_WK_Remove( p_wk->p_clwk );
  GFL_STD_MemClear( p_wk, sizeof(WF_2DCWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�|�C���^�̎擾
 *
 *	@param	p_wk	���[�N
 *
 *	@return	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
GFL_CLWK* WF_2DC_WkClWkGet( WF_2DCWK* p_wk )
{
  return p_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�|�C���^�̎擾
 *
 *	@param	p_wk	���[�N
 *
 *	@return	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
const GFL_CLWK* WF_2DC_WkConstClWkGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W�̐ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	x		�����W
 *	@param	y		�����W
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
  GFL_CLACTPOS	mat;

  mat.x = x;
  mat.y = y;

  GFL_CLACT_WK_SetWldPos( p_wk->p_clwk, &mat );


  if( p_wk->p_shadow ){

    mat.x += WF_2DC_SHADOW_MAT_OFS_X;
    mat.y += WF_2DC_SHADOW_MAT_OFS_Y;
    GFL_CLACT_WK_SetWldPos( p_wk->p_shadow, &mat );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W�擾
 *
 *	@param	p_wk	���[�N
 *	@param	x_y		�t���O
 *
 *	x_y
	WF_2DC_GET_X,
	WF_2DC_GET_Y
 *
 *
 *	@return	�t���O�̍��W�̒l
 */
//-----------------------------------------------------------------------------
s16 WF_2DC_WkMatrixGet( WF_2DCWK* p_wk, WF_2DC_MAT x_y )
{
  GFL_CLACTPOS p_pos;

  GFL_CLACT_WK_GetWldPos(p_wk->p_clwk, &p_pos);

  if( x_y == WF_2DC_GET_X ){
    return p_pos.x;
  }
  else{
    return p_pos.y;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\���D�揇�ʂ�ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	pri		�\���D�揇��
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawPriSet( WF_2DCWK* p_wk, u16 pri )
{
  GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, pri );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\���D�揇�ʂ��擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�\���D�揇��
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkDrawPriGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetSoftPri( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V������i�߂�
 *
 *	@param	p_wk	���[�N
 *	@param	frame	�t���[��
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmAddFrame( WF_2DCWK* p_wk, fx32 frame )
{
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brie	�\����\���ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	flag	�t���O
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawFlagSet( WF_2DCWK* p_wk, BOOL flag )
{
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, flag );
  if( p_wk->p_shadow ){
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_shadow, flag );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brie	�e�̂ݕ\����\���ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	flag	�t���O
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkShadowDrawFlagSet( WF_2DCWK* p_wk, BOOL flag )
{
  if( p_wk->p_shadow ){
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_shadow, flag );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\����\���擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�\��
 *	@retval	FALSE	��\��
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkDrawFlagGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetDrawEnable( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	����^�C�v���擾����
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	����^�C�v
 */
//-----------------------------------------------------------------------------
WF_2DC_MOVETYPE WF_2DC_WkMoveTypeGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����^�C�v���擾����
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�A�j���[�V�����^�C�v
 */
//-----------------------------------------------------------------------------
WF_2DC_ANMTYPE WF_2DC_WkAnmTypeGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->patanmtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[���ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	frame		�t���[��
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmFrameSet( WF_2DCWK* p_wk, u16 frame )
{
  GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[���擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�t���[���C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkAnmFrameGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetAnmIndex( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�̈ʒu�𑀍삷��
 *
 *	@param	p_wk	���[�N
 *	@param	x		�����W
 *	@param	y		�����W
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkShadowMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
  /*	VecFx32	mat;
	if( p_wk->p_shadow != NULL ){

		mat.x = x+WF_2DC_SHADOW_MAT_OFS_X<<FX32_SHIFT;
		mat.y = y+WF_2DC_SHADOW_MAT_OFS_Y<<FX32_SHIFT;
		mat.z = 0;
		CLACT_SetMatrix( p_wk->p_shadow, &mat );
	}
   */
  GFL_CLACTPOS pos;

  if( p_wk->p_shadow != NULL ){
    pos.x = x+WF_2DC_SHADOW_MAT_OFS_X;
    pos.y = y+WF_2DC_SHADOW_MAT_OFS_Y;
    GFL_CLACT_WK_SetWldPos(p_wk->p_shadow,&pos );
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief	Pattern�A�j���J�n
 *
 *	@param	p_wk		���[�N
 *	@param	anmtype		�A�j���^�C�v
 *	@param	anmway		�A�j������
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmStart( WF_2DCWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway )
{
  static void (* const pFunc[])( WF_2DCWK*, WF_COMMON_WAY ) = {
    WF_2DC_WkPatAnmWayInit,
    WF_2DC_WkPatAnmRotaInit,
    WF_2DC_WkPatAnmWalkInit,
    WF_2DC_WkPatAnmTurnInit,
    WF_2DC_WkPatAnmRunInit,
    WF_2DC_WkPatAnmWallWalkInit,
    WF_2DC_WkPatAnmSlowWalkInit,
    WF_2DC_WkPatAnmHighWalk2Init,
    WF_2DC_WkPatAnmHighWalk4Init,
    NULL
    };

  GF_ASSERT( anmtype < WF_2DC_ANMNUM );
  GF_ASSERT( anmway < WF_COMMON_WAYNUM );

  p_wk->patanmtype = anmtype;
  p_wk->patanmframe = 0;

  if( pFunc[ anmtype ] != NULL ){
    pFunc[ anmtype ]( p_wk, anmway );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	Pattern�A�j���I���`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�I��
 *	@retval	FALSE	�r��
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkPatAnmEndCheck( const WF_2DCWK* cp_wk )
{
  if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] == 0xff ){	// ���[�v�ݒ�
    return FALSE;
  }

  //�I���`�F�b�N
  if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] <= cp_wk->patanmframe ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Pattern�A�j����i�߂�
 *
 *	@param	p_wk		���[�N
 *	@param	speed		�X�s�[�h
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmAddFrame( WF_2DCWK* p_wk )
{
  BOOL updata = FALSE;

  if( WF_2DC_AnmFrame[ p_wk->patanmtype ] == 0xff ){	// ���[�v�ݒ�
    updata = TRUE;
  }else{

    if( WF_2DC_AnmFrame[ p_wk->patanmtype ] >= p_wk->patanmframe + 1 ){
      updata = TRUE;
    }
  }

  if( updata == TRUE ){
    WF_2DC_WkPatAnmUpdate( p_wk );	// �A�j���X�V
    p_wk->patanmframe ++;
  }
}


//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������\�[�X�ǂݍ���
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	movetype		����^�C�v
 *	@param	heap			�q�[�v
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap )
{
  int i;


  for(i = WF_2DC_UNICHAR_NUM; i < WF_2DC_CHARNUM; i++ ){
    p_sys->anmres[i].resid = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                         WF_2DC_ARC_GETCEL(i-WB_HERO),
                                                         WF_2DC_ARC_GETANM(i-WB_HERO),
                                                         heap);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������\�[�X�j��
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	movetype		����^�C�v
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype )
{
  int i;

  for(i = WF_2DC_UNICHAR_NUM; i < WF_2DC_CHARNUM; i++ ){
    GFL_CLGRP_CELLANIM_Release(p_sys->anmres[i].resid);
    p_sys->anmres[i].resid = 0;
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ڂ���L�����N�^NO���擾����
 *
 *	@param	view_type	�����ځiFIELDOBJCODE�j
 *
 *	@return	�L�����N�^NO
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharNoGet( u32 view_type )
{
  return view_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�w�肳�ꂽ�l���ɓ���^�C�v���w��ł��邩�`�F�b�N
 *
 *	@param	view_type		������
 *	@param	movetype		����^�C�v
 *
 *	@retval	TRUE	�w��ł���
 *	@retval	FALSE	�w��ł��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype )
{
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X�ǂݍ���
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 *	@param	draw_type	�]����
 *	@param	movetype	����^�C�v
 *	@param	heap		�q�[�v
 *	@param	restype		���\�[�X�^�C�v
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype )
{
  // �ǂݍ��ݍς݂łȂ����Ƃ��`�F�b�N
  //	GF_ASSERT( p_sys->res[ char_no ].resobj[0] == NULL );

  if( restype == WF_2DC_RES_NML ){
    WF_2DC_CharResLoadNml( p_sys, char_no, draw_type, movetype, heap );
  }else{
    WF_2DC_CharResLoadUni( p_sys, char_no, draw_type, movetype, heap );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�[���\�[�X�ǂݍ��ݒʏ펞
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 *	@param	draw_type	�]����
 *	@param	movetype	����^�C�v
 *	@param	heap		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  NNSG2dCharacterData* p_char;
  BOOL result;
  BOOL flip;	// �t���b�v���g�p����L�����N�^��
  u32 cell_contid, anm_contid;


  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                               WF_2DC_ARC_GETNCG( char_no-WB_HERO ),
                                                               FALSE , draw_type, heap);

  p_sys->chrres[ char_no ].resid[ 1 ] = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                                  WF_2DC_ARC_GETNCL( char_no-WB_HERO ),
                                                                  draw_type, WF_2DC_HERO_PLTT_START*0x20, 0, WF_2DC_HERO_PLTT_NUM, heap);



  flip = TRUE;

  // �Ǘ��p�����[�^�ݒ�
  p_sys->chrres[ char_no ].drawtype = draw_type;
  p_sys->chrres[ char_no ].movetype = movetype;
  p_sys->chrres[ char_no ].type = WF_2DC_RES_NML;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�[���\�[�X�ǂݍ���	���j�I����
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 *	@param	draw_type	�]����
 *	@param	movetype	����^�C�v
 *	@param	heap		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  NNSG2dCharacterData* p_char;
  BOOL result;
  u32 union_idx;


  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                               WF_2DC_ARC_GETUNINCG( char_no ),
                                                               FALSE , draw_type, heap);


  p_sys->chrres[ char_no ].resid[ 1 ] = p_sys->unionplttid;


  // �Ǘ��p�����[�^�ݒ�
  p_sys->chrres[ char_no ].drawtype = draw_type;
  p_sys->chrres[ char_no ].movetype = movetype;
  p_sys->chrres[ char_no ].type = WF_2DC_RES_UNI;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X�j��
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no )
{

  if( p_sys->chrres[ char_no ].type == WF_2DC_RES_NML ){
    WF_2DC_CharResDelNml( p_sys, char_no );
  }else{
    WF_2DC_CharResDelUni( p_sys, char_no );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X�j��	�ʏ펞
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no )
{

  GFL_CLGRP_CGR_Release(p_sys->chrres[ char_no ].resid[0]);
  GFL_CLGRP_PLTT_Release(p_sys->chrres[ char_no ].resid[1]);
  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;
  p_sys->chrres[ char_no ].resid[ 1 ] = GFL_CLGRP_REGISTER_FAILED;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X�j��	���j�I����
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no )
{
  GFL_CLGRP_CGR_Release(p_sys->chrres[ char_no ].resid[0]);
  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^���\�[�X���ǂݍ��܂�Ă��邩�`�F�b�N
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *	@param	char_no		�L�����N�^�ԍ�
 *
 *	@retval	TRUE	�ǂݍ��݊���
 *	@retval	FALSE	�ǂݍ���ł��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no )
{
  if( cp_sys->chrres[ char_no ].resid[ 0 ] == GFL_CLGRP_REGISTER_FAILED ){
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��^�C�v�̎擾
 *
 *	@param	cp_res	���\�[�X���[�N
 *
 *	@return	�`��^�C�v�擾
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res )
{
  return cp_res->drawtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����^�C�v�擾
 *
 *	@param	cp_res	���\�[�X���[�N
 *
 *	@return	����^�C�v
 */
//-----------------------------------------------------------------------------
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res )
{
  return cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���\�[�X��ǂݍ���
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	draw_type	�\�����
 *	@param	heap		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap )
{
  int i;
  BOOL result;
  BOOL hero_load;
  //	u32 hero_no;
  //	GF_ASSERT( p_sys->shadowres.resobj[0] == NULL );

  // ��l�����ǂݍ��܂�Ă��邩�`�F�b�N����

  hero_load = FALSE;
  if( WF_2DC_CharResCheck( p_sys, WB_HERO ) == TRUE ){
    p_sys->hero_no		= WB_HERO;
    hero_load	= TRUE;
  }
  else if( WF_2DC_CharResCheck( p_sys, WB_HEROINE ) == TRUE ){
    p_sys->hero_no		= WB_HEROINE;
    hero_load	= TRUE;
  }

  GF_ASSERT_MSG( hero_load == TRUE, "shodow load miss  hero or heroine not load" );


  p_sys->shadowres.resid[2] = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                          NARC_wifileadingchar_hero_ine_kage_NCER,
                                                          NARC_wifileadingchar_hero_ine_kage_NANR,
                                                          heap);

  p_sys->shadowres.resid[0] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                     NARC_wifileadingchar_hero_ine_kage_NCGR,
                                                     FALSE , draw_type, heap);

  p_sys->shadowres.resid[1] = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                        NARC_wifileadingchar_hero_ine_kage_NCLR,
                                                        draw_type, WF_2DC_HERO_KAGE_PLTT_START*0x20, 0, WF_2DC_HERO_KAGE_PLTT_NUM,heap);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���\�[�X�̔j��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys )
{
  int i;

  if( WF_2DC_ShResCheck( &p_sys->shadowres ) == TRUE ){

    // �L�����N�^���\�[�X�j��
    GFL_CLGRP_CGR_Release(p_sys->shadowres.resid[0]);
    p_sys->shadowres.resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;
    GFL_CLGRP_PLTT_Release(p_sys->shadowres.resid[1]);
    p_sys->shadowres.resid[ 1 ] = GFL_CLGRP_REGISTER_FAILED;
    GFL_CLGRP_CELLANIM_Release(p_sys->shadowres.resid[2]);
    p_sys->shadowres.resid[ 2 ] = GFL_CLGRP_REGISTER_FAILED;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���\�[�X���ǂݍ��܂�Ă��邩�`�F�b�N
 *
 *	@param	cp_shadowres	���[�N
 *
 *	@retval	TRUE	���\�[�X�ǂݍ��ݍς�
 *	@retval	FALSE	���\�[�X���ǂݍ��܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres )
{

  //	if( cp_shadowres->resobj[0] != NULL ){
  if( cp_shadowres->resid[0] != GFL_CLGRP_REGISTER_FAILED ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�p�p���b�g�Ǎ���
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	draw_type	�`���
 *	@param	heap
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type, int offset, u32 heap )
{
  BOOL result;

  p_sys->unionplttid = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                 WF_2DC_ARC_GETUNINCL,
                                                 draw_type, offset, 0, WF_2DC_UNIPLTT_NUM,heap);

}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�p�p���b�g�j��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys )
{
  //	GF_ASSERT( p_sys->p_unionpltt != NULL );

  GFL_CLGRP_PLTT_Release(p_sys->unionplttid);
  p_sys->unionplttid = GFL_CLGRP_REGISTER_FAILED;


  // ���j�I���p���b�g�j��
  //	CLACT_U_PlttManagerDelete( p_sys->p_unionpltt );
  //	CLACT_U_ResManagerResDelete( p_sys->p_res_man[1], p_sys->p_unionpltt );
  //	p_sys->p_unionpltt = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�p�A�j�����\�[�X�Ǎ���
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap )
{
  int i;

  GF_ASSERT( p_sys->anmres[ 0 ].resobj[0] == NULL );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){

    p_sys->anmres[i].resid = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                         WF_2DC_ARC_GETUNICEL(i),
                                                         WF_2DC_ARC_GETUNIANM(i),
                                                         heap);


    /*
		p_sys->unionres[i].resobj[ 0 ] =
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[2], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNICEL(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELL_RES, heap );

		p_sys->unionres[i].resobj[ 1 ] =
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[3], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNIANM(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELLANM_RES, heap );
     */
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j�I���L�����N�^�p�A�j�����\�[�X�j��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys )
{
  int i;

  //	GF_ASSERT( p_sys->unionres[ 0 ].resobj[0] != NULL );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    GFL_CLGRP_CELLANIM_Release(p_sys->anmres[i].resid);
    p_sys->anmres[i].resid = GFL_CLGRP_REGISTER_FAILED;


    //		CLACT_U_ResManagerResDelete( p_sys->p_res_man[2],
    //					p_sys->unionres[i].resobj[ 0 ] );

    //		CLACT_U_ResManagerResDelete( p_sys->p_res_man[3],
    //					p_sys->unionres[i].resobj[ 1 ] );

    //		p_sys->unionres[ i ].resobj[0] = NULL;
    //		p_sys->unionres[ i ].resobj[1] = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	��̃��[�N���擾
 *
 *	@param	p_sys	���[�N
 *
 *	@return	��̃��[�N
 */
//-----------------------------------------------------------------------------
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys )
{
  int i;

  for( i=0; i<p_sys->objnum; i++ ){
    if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == TRUE ){
      return &p_sys->p_wk[i];
    }
  }
  GF_ASSERT( 0 );
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Pattern�A�j���X�V
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk )
{
  static void (* const pFunc[])( WF_2DCWK*  ) = {
    NULL,
    WF_2DC_WkPatAnmRotaMain,
    WF_2DC_WkPatAnmWalkMain,
    WF_2DC_WkPatAnmTurnMain,
    WF_2DC_WkPatAnmRunMain,
    WF_2DC_WkPatAnmWallWalkMain,
    WF_2DC_WkPatAnmSlowWalkMain,
    WF_2DC_WkPatAnmHighWalk2Main,
    WF_2DC_WkPatAnmHighWalk4Main,
    NULL
    };
  if( pFunc[p_wk->patanmtype] != NULL ){
    pFunc[p_wk->patanmtype]( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ς��A�j��������
 *
 *	@param	p_wk	���[�N
 *	@param	way		����
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]�A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_ROTA );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_ROTA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�U������A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );

  // ��Őݒ肷�邱�ƂłQ�t���[���ڂŕ������ς��
  p_wk->patanmway = way;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_RUN+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_RUN+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Ǖ��������A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����������A�j��������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����A�j���Q�t���[��
 *
 *	@param	p_wk		���[�N
 *	@param	way			����
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����A�j���S�t���[��
 *
 *	@param	p_wk		���[�N
 *	@param	way			����
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]�A�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�U������A�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk )
{
  if( p_wk->patanmframe == 0 ){
    //	CLACT_AnmFrameSet( p_wk->p_clwk, 1 );
    GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, 1 );
  }else{
    //	CLACT_AnmChg( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
    GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	����A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Ǖ��������A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����������A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����Q�t���A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH2 );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_HIGH2 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����S�t���A�j�����C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH4 );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_HIGH4 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���[�N���g���Ă��邩�`�F�b�N
 *
 *	@param cp_wk	���[�N
 *
 *	@retval		TRUE�@		���g�p
 *	@retval		FALSE		�g�p��
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk )
{
  if( cp_wk->p_clwk == NULL ){
    return TRUE;
  }
  return FALSE;
}


