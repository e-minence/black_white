//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		clact.c
 *	@brief		�Z���A�N�^�[�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2006.11.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "include/gflib.h"

#define __CLACT_H_GLOBAL
#include "include/clact.h"

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
///	CLSYS_REND�֌W
//=====================================
#define CLSYS_DEFFREND_ZOFFS	(1)		// �f�t�H���g�Őݒ肷��3D�p��Z�I�t�Z�b�g�ݒ�


//-------------------------------------
///	CLWK_ANMDATA�֌W
//=====================================
typedef enum{
	CLWK_ANM_CELL,		// �Z���A�j��
	CLWK_ANM_TRCELL,	// �Z���]���A�j��
	CLWK_ANM_MULTICEL,	// �}���`�Z���A�j��
	CLWK_ANM_MAX,		// �A�j���[�V�����^�C�v�ő吔
} CLWK_ANMTYPE;


//-------------------------------------
///	CLUNIT	�����_���[�^�C�v
//=====================================
#define CLUNIT_RENDTYPE_DEFF	(0)	// �f�t�H���g�ݒ背���_���[
#define CLUNIT_RENDTYPE_USER	(1)	// ���[�U�[��`�����_���[


//-------------------------------------
///	�t���b�v�}�X�N
//=====================================
#define CLSYS_FLIPNONE	(0)	// �t���b�v�Ȃ�
#define CLSYS_VFLIP		(1)	// V�t���b�v
#define CLSYS_HFLIP		(2)	// H�t���b�v
#define CLSYS_VHFLIP	(CLSYS_VFLIP|CLSYS_HFLIP)	// VH�t���b�v

//-------------------------------------
///	�I�[�g�A�j���f�t�H���g�X�s�[�h
//=====================================
#define CLWK_AUTOANM_DEFF_SPEED	( FX32_ONE )


//-----------------------------------------------------------------------------
/**
 *					�I�u�W�F�N�g���̍\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	OAM�}�l�[�W���I�u�W�F�N�g
//====================================
typedef struct {
	NNSG2dOamManagerInstance man;
	BOOL init;	// �������������̃t���O
} OAMMAN_DATA;

//-------------------------------------
///	OAMMAN OAM�}�l�[�W���Ǘ��\����
//=====================================
typedef struct {
	OAMMAN_DATA man[ CLSYS_DRAW_MAX ];	// ���C����ʗpOAM�}�l�[�W��
} CLSYS_OAMMAN;

//-------------------------------------
///	REND�@�����_���[�Ǘ��\����
//=====================================
typedef struct _CLSYS_REND{
	NNSG2dRendererInstance rend;
	NNSG2dRenderSurface*   p_surface;
	u32	surface_num;
} CLSYS_REND;

//-------------------------------------
///	TRMAN�@�]���f�[�^�i�[�̈�
//=====================================
typedef struct {
	void*	p_src;			// �]���f�[�^		�f�[�^���ݒ肳��Ă��邩�̃`�F�b�N�ɂ��g�p
	u32		addr;			// �]����A�h���X
	u32		size;			// �T�C�Y
	CLSYS_DRAW_TYPE	type;	// �]����^�C�v	
} TRMAN_DATA;

//-------------------------------------
///	TRMAN	�Z��Vram�]���}�l�[�W���֌W
//=====================================
typedef struct {
	NNSG2dCellTransferState* p_cell;	// �Z���]���Ǘ��o�b�t�@
	TRMAN_DATA* p_trdata;				// �]���L�����N�^�Ǘ��o�b�t�@
	u32 data_num;						// ���o�b�t�@�̗v�f��
	BOOL init;							// �������L��
} CLSYS_TRMAN;



//-------------------------------------
///	CLSYS�\����
//=====================================
typedef struct {
	CLSYS_OAMMAN	oamman;
	CLSYS_REND		rend;
	CLSYS_TRMAN		trman;
	u16 pltt_no[CLSYS_DRAW_MAX];	// �㏑������p���b�g�i���o�[
} CLSYS;


//-------------------------------------
///	�A�j���[�V�����f�[�^�\����
//=====================================
typedef struct {
	CLWK_ANMTYPE	type;	// �i�[����Ă���A�j���^�C�v
	union{
		struct{
			const NNSG2dCellDataBank*       cp_cell;	// �Z���f�[�^
			const NNSG2dCellAnimBankData*   cp_canm;	// �Z���A�j���[�V����
			NNSG2dCellAnimation				anmctrl;	// �A�j���[�V�����R���g���[��
		} cell;
		struct{
			NNSG2dCellDataBank*				p_cell;		// �Z���f�[�^
			const NNSG2dCellAnimBankData*   cp_canm;	// �Z���A�j���[�V����
			NNSG2dCellAnimation				anmctrl;	// �A�j���[�V�����R���g���[��
			u32								trhandle;	// �Z���A�j���[�V������Vram�]�����Ǘ�����
														// �Z���]����ԊǗ��I�u�W�F�N�g�̃n���h��
		} trcell;
		struct{
			const NNSG2dCellDataBank*       cp_cell;	// �Z���f�[�^
			const NNSG2dCellAnimBankData*	cp_canm;	// �Z���A�j���[�V����
			NNSG2dMultiCellAnimation		anmctrl;	// �A�j���[�V�����R���g���[��
			const NNSG2dMultiCellDataBank*        cp_mcell;    // �}���`�Z���f�[�^
			const NNSG2dMultiCellAnimBankData*    cp_mcanm;   // �}���`�Z���A�j���[�V����
			void* p_wk;									// �A�j���[�V�������[�N
		} multicell;
	} data;
} CLWK_ANMDATA;


//-------------------------------------
///	CLWK�\����
//=====================================
typedef struct _CLWK{
	CLWK*					p_next;			// ���̃f�[�^
	CLWK*					p_last;			// �O�̃f�[�^
	CLUNIT*					p_unit;			// �����̐e�Z���A�N�^�[���j�b�g
	CLSYS_POS				pos;			// ���W
	CLSYS_POS				affinepos;		// �A�t�B�����W
	CLSYS_SCALE				scale;			// �g��
	NNSG2dImageProxy		img_proxy;		// �L�����N�^/�e�N�X�`���v���L�V
	NNSG2dImagePaletteProxy pltt_proxy;		// �p���b�g�v���L�V
	fx32					auto_anm_speed;	// �I�[�g�A�j���X�s�[�h
	u32						setsf;	// �T�[�t�F�[�X�C���f�b�N�X
	u16						rotation;		// ��]
	u16						anmseq;			// �A�j���[�V�����V�[�P���X

	u32						soft_pri:8;		// �\�t�g�D�揇��(u8)
	u32						over_write:4;	// �I�[�o�[���C�g�t���O(0�`8)
	u32						pal_offs:4;		// �p���b�g�I�t�Z�b�g(0�`15)
	u32						bg_pri:2;		// BG�D�揇��(0�`3)
	u32						affine:2;		// �A�t�B���ϊ��^�C�v(0�`2)
	u32						vhflip:2;		// �t���b�v�ϊ��}�X�N(0�`3)
	u32						objmode:2;		// �I�u�W�F�N�g���[�h(0�`3)
	u32						mosaic:1;		// ���U�C�N�t���O(0�`1)
	u32						auto_anm:1;		// �I�[�g�A�j���[�V�����t���O(0:1)
	u32						draw_flag:1;	// �`��On-Off
	u32						playmode:4;		// �A�j���[�V�����v���C���[�h
	u32						dummy:1;		// ���܂�bit

	CLWK_ANMDATA			anmdata;		// �Z���A�j���f�[�^
} CLWK;


//-------------------------------------
///	CLUNT�\����
//=====================================
typedef struct _CLUNIT{
	CLWK*	p_wk;		// �Z�����[�N
	CLWK*	p_drawlist;	// �`�惊�X�g
	CLSYS_REND* p_rend;	// �֘A�t�����Ă��郌���_���[
	u16		wk_num;		// �Z�����[�N��
	u8		rend_type;// �����_���[�^�C�v	�i���[�U�[/�f�t�H���g�j
	u8		draw_flag;// �`��ON_OFF�t���O
} CLUNIT;



//-----------------------------------------------------------------------------
/**
 *			�Z���A�N�^�[�������f�[�^�\���̐錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�T�[�t�F�[�X�R�[���o�b�N�\����
//=====================================
typedef struct {
    NNSG2dOamRegisterFunction         pFuncOamRegister;         // Oam �o�^�֐�
    NNSG2dAffineRegisterFunction      pFuncOamAffineRegister;   // Affine �o�^�֐�
    NNSG2dRndCellCullingFunction      pFuncVisibilityCulling;   // ���J�����O�֐�
} REND_SURFACE_CALLBACK;



//-----------------------------------------------------------------------------
/**
 *			�Z���A�N�^�[�V�X�e������
 */
//-----------------------------------------------------------------------------
static CLSYS* pClsys = NULL;



//-----------------------------------------------------------------------------
/**
 *			�f�[�^��
 */
//-----------------------------------------------------------------------------
static const u16 CLSYS_DRAW_TYPEtoNNS_G2D_VRAM_TYPE[ CLSYS_DRAW_MAX ] = {
	NNS_G2D_VRAM_TYPE_2DMAIN,
	NNS_G2D_VRAM_TYPE_2DSUB,
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	CLSYS�֌W
//=====================================
static void CLSYS_DefaultRendInit( CLSYS_REND* p_rend, const CLSYS_INIT* cp_data, HEAPID heapID );
static void CLSYS_SysSetPaletteProxy( const NNSG2dImagePaletteProxy* cp_pltt, u8 pal_offs );


//-------------------------------------
///	CLSYS_OAMMAN�֌W
//=====================================
static void OAMMAN_SysInit( CLSYS_OAMMAN* p_oamman, u8 oamst_main, u8 oamnum_main, u8 oamst_sub, u8 oamnum_sub );
static void OAMMAN_SysExit( CLSYS_OAMMAN* p_oamman );
static void OAMMAN_SysTrans( CLSYS_OAMMAN* p_oamman );
static void OAMMAN_SysClean( CLSYS_OAMMAN* p_oamman );
static void OAMMAN_ObjCreate( OAMMAN_DATA* p_obj, u8 oamst, u8 oamnum, NNSG2dOamType oam_type );
static BOOL OAMMAN_ObjEntryOamAttr( OAMMAN_DATA* p_obj, const GXOamAttr* pOam, u16 affineIndex );
static u16 OAMMAN_ObjEntryAffine( OAMMAN_DATA* p_obj, const MtxFx22* mtx );
static void OAMMAN_ObjTrans( OAMMAN_DATA* p_obj );
static void OAMMAN_ObjClearBuff( OAMMAN_DATA* p_obj );

//-------------------------------------
///	CLSYS_REND�֌W
//=====================================
static void REND_SysInit( CLSYS_REND* p_rend, const REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID );
static void REND_SysExit( CLSYS_REND* p_rend );
static void REND_SysBeginDraw( CLSYS_REND* p_rend, const NNSG2dImageProxy* pImgProxy, const NNSG2dImagePaletteProxy* pPltProxy );
static void REND_SysEndDraw( CLSYS_REND* p_rend );
static void REND_SysSetAffine( CLSYS_REND* p_rend, CLSYS_AFFINETYPE mode );
static void REND_SysSetFlip( CLSYS_REND* p_rend, BOOL vflip, BOOL hflip );
static void REND_SysSetOverwrite( CLSYS_REND* p_rend, u8 over_write );
static void REND_SysSetMosaicFlag( CLSYS_REND* p_rend, BOOL on_off );
static void REND_SysSetOBJMode( CLSYS_REND* p_rend, GXOamMode objmode );
static void REND_SysSetBGPriority( CLSYS_REND* p_rend, u8 pri );
static void REND_SurfaceObjCreate( NNSG2dRenderSurface* p_surface, const REND_SURFACE_INIT* cp_data );
static void REND_SurfaceObjSetPos( NNSG2dRenderSurface* p_surface, s16 x, s16 y );
static void REND_SurfaceObjSetSize( NNSG2dRenderSurface* p_surface, s16 width, s16 height );
static void REND_SurfaceObjSetType( NNSG2dRenderSurface* p_surface, CLSYS_DRAW_TYPE type );
static void REND_SurfaceObjSetCallBack( NNSG2dRenderSurface* p_surface, const REND_SURFACE_CALLBACK* cp_callback );
static void REND_SurfaceObjGetPos( const NNSG2dRenderSurface* cp_surface, s16* p_x, s16* p_y );
static void REND_SurfaceObjGetSize( const NNSG2dRenderSurface* cp_surface, s16* p_width, s16* p_height );
static CLSYS_DRAW_TYPE REND_SurfaceObjGetType( const NNSG2dRenderSurface* cp_surface );
// �����_���[Default�R�[���o�b�N
static BOOL REND_CallBackAddOam( OAMMAN_DATA* p_obj, const GXOamAttr* pOam, u16 affineIndex, u32 pltt_no );
static BOOL REND_CallBackMainAddOam( const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine );
static BOOL REND_CallBackSubAddOam( const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine );
static u16 REND_CallBackMainAddAffine( const MtxFx22* mtx );
static u16 REND_CallBackSubAddAffine( const MtxFx22* mtx );
static BOOL REND_CallBackCulling( const NNSG2dCellData* pCell, const MtxFx32* pMtx, const NNSG2dViewRect* pViewRect );


//-------------------------------------
///	TRMAN	�֌W
//=====================================
static void TRMAN_SysInit( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID );
static void TRMAN_SysExit( CLSYS_TRMAN* p_trman );
static void TRMAN_SysMain( CLSYS_TRMAN* p_trman );
static void TRMAN_SysVBlank( CLSYS_TRMAN* p_trman );
static u8 TRMAN_SysGetBuffNum( const CLSYS_TRMAN* cp_trman );
static void TRMAN_CellTransManCreate( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID );
static void TRMAN_CellTransManDelete( CLSYS_TRMAN* p_trman );
static u32 TRMAN_CellTransManAddHandle( CLSYS_TRMAN* p_trman );
static void TRMAN_CellTransManDelHandle( CLSYS_TRMAN* p_trman, u32 handle );
static void TRMAN_TrSysCreate( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID );
static void TRMAN_TrSysDelete( CLSYS_TRMAN* p_trman );
static void TRMAN_TrSysUpdata( CLSYS_TRMAN* p_trman );
static void TRMAN_TrSysTrans( CLSYS_TRMAN* p_trman );
static TRMAN_DATA* TRMAN_TrSysGetCleanBuff( CLSYS_TRMAN* p_trman );
static BOOL TRMAN_TrSysCallBackAddTransData( NNS_GFD_DST_TYPE type, u32 dstAddr, void* pSrc, u32 szByte );
static void TRMAN_TrDataClean( TRMAN_DATA* p_data );
static void TRMAN_TrDataAddData( TRMAN_DATA* p_data, CLSYS_DRAW_TYPE type, u32 dstAddr, void* pSrc, u32 szByte );
static void TRMAN_TrDataTrans( TRMAN_DATA* p_data );
static BOOL TRMAN_TrDataCheckClean( const TRMAN_DATA* cp_data );


//-------------------------------------
///	CLUNIT	�֌W
//=====================================
static CLWK* CLUNIT_SysGetCleanWk( CLUNIT* p_unit );
static void CLUNIT_DrawListAdd( CLUNIT* p_unit, CLWK* p_wk );
static void CLUNIT_DrawListDel( CLUNIT* p_unit, CLWK* p_wk );
static CLWK* CLUNIT_DrawListSarchTop( CLWK* p_top, u8 pri );
static CLWK* CLUNIT_DrawListSarchBottom( CLWK* p_bottom, u8 pri );


//-------------------------------------
///	CLWK	�֌W
//=====================================
static void CLWK_SysClean( CLWK* p_wk );
static void CLWK_SysDraw( CLWK* p_wk, CLSYS_REND* p_rend );
static void CLWK_SysDrawSetRend( CLWK* p_wk, CLSYS_REND* p_rend );
static void CLWK_SysDrawCell( CLWK* p_wk );
static void CLWK_SysAutoAnm( CLWK* p_wk );
static void CLWK_SysSetClwkData( CLWK* p_wk, const CLWK_DATA* cp_data, u16 setsf );
static void CLWK_SysSetClwkRes( CLWK* p_wk, const CLWK_RES* cp_res );
static void CLWK_SysGetSetSfPos( const CLWK* cp_wk, u16 setsf, CLSYS_POS* p_pos );

//-------------------------------------
///	CLWK_ANMDATA	�֌W
//=====================================
static void CLWK_AnmDataInit( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID );
static void CLWK_AnmDataExit( CLWK_ANMDATA* p_anmdata );
static CLWK_ANMTYPE CLWK_AnmDataGetType( const CLWK_RES* cp_res );
static void CLWK_AnmDataCreateCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID );
static void CLWK_AnmDataCreateTRCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID );
static void CLWK_AnmDataCreateMCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID );
static void CLWK_AnmDataDeleteCellData( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataDeleteTRCellData( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataDeleteMCellData( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataSetAnmSeq( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqNoReset( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqNoResetCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqNoResetTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmSeqNoResetMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmFrame( CLWK_ANMDATA* p_anmdata, u16 idx );
static void CLWK_AnmDataSetAnmFrameCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmFrameTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataSetAnmFrameMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq );
static void CLWK_AnmDataAddAnmFrame( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataAddAnmFrameCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataAddAnmFrameTRCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataAddAnmFrameMCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrl( CLWK_ANMDATA* p_anmdata );
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlCell( CLWK_ANMDATA* p_anmdata );
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlTRCell( CLWK_ANMDATA* p_anmdata );
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlMCell( CLWK_ANMDATA* p_anmdata );
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrl( const CLWK_ANMDATA* cp_anmdata );
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlCell( const CLWK_ANMDATA* cp_anmdata );
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlTRCell( const CLWK_ANMDATA* cp_anmdata );
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlMCell( const CLWK_ANMDATA* cp_anmdata );
static u16 CLWK_AnmDataGetAnmFrame( const CLWK_ANMDATA* cp_anmdata );
static void CLWK_AnmDataSetAnmSpeed( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataSetAnmSpeedCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataSetAnmSpeedTRCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static void CLWK_AnmDataSetAnmSpeedMCell( CLWK_ANMDATA* p_anmdata, fx32 speed );
static BOOL CLWK_AnmDataCheckAnmActive( const CLWK_ANMDATA* cp_anmdata );
static void CLWK_AnmDataResetAnm( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataResetAnmCell( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataResetAnmMCell( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataSetAnmMode( CLWK_ANMDATA* p_anmdata, CLSYS_ANM_PLAYMODE playmode );
static void CLWK_AnmDataStartAnm( CLWK_ANMDATA* p_anmdata );
static void CLWK_AnmDataStopAnm( CLWK_ANMDATA* p_anmdata );







//-----------------------------------------------------------------------------
/**
 *				�O�����J�֐��S
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	CLSYS�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@������
 *
 *	@param	cp_data		�������f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysInit( const CLSYS_INIT* cp_data, HEAPID heapID )
{
	int i;

	GF_ASSERT( pClsys == NULL );

	//OS_Printf( "[%d]\n", __LINE__ );

	pClsys = GFL_HEAP_AllocMemory( heapID, sizeof(CLSYS) );
	GFL_STD_MemFill( pClsys, 0, sizeof(CLSYS) );
	
	// OAM�}�l�W���[������
	OAMMAN_SysInit( &pClsys->oamman, 
			cp_data->oamst_main, cp_data->oamnum_main,
			cp_data->oamst_sub, cp_data->oamnum_sub
			);

	// �����_���[������
	CLSYS_DefaultRendInit( &pClsys->rend, cp_data, heapID );

	// �Z���]���A�j���V�X�e��������
	TRMAN_SysInit( &pClsys->trman, cp_data->tr_cell, heapID );

	// �p���b�g�i���o�[
	for( i=0; i<CLSYS_DRAW_MAX; i++ ){
		pClsys->pltt_no[i] = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	�j��
 *
 *	@param	none
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysExit( void )
{
	GF_ASSERT( pClsys );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	// �e�V�X�e���j��
	OAMMAN_SysExit( &pClsys->oamman );
	REND_SysExit( &pClsys->rend );
	TRMAN_SysExit( &pClsys->trman );

	GFL_HEAP_FreeMemory( pClsys );
	pClsys = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@���C������
 *
 *	*�S�Z���A�N�^�[���j�b�g�̕`�悪�I�������ɌĂԕK�v������܂��B
 *	*���C�����[�v�̍Ō�ɌĂԂ悤�ɂ��Ă����Ɗm�����Ǝv���܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysMain( void )
{
	//OS_Printf( "[%d]\n", __LINE__ );
	if( pClsys ){
		TRMAN_SysMain( &pClsys->trman );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@V�u�����N����
 *
 *	��OAM�f�[�^�]����o�b�t�@���N���[�����܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysVblank( void )
{
	//OS_Printf( "[%d]\n", __LINE__ );
	if( pClsys ){
		OAMMAN_SysTrans( &pClsys->oamman );
		OAMMAN_SysClean( &pClsys->oamman );
		TRMAN_SysVBlank( &pClsys->trman );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[OAM�o�b�t�@�̃N���[������
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysOamBuffClean( void )
{
	if( pClsys ){
		OAMMAN_SysClean( &pClsys->oamman );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	V�u�����N����	�]���̂�
 *
 *	��OAM�f�[�^�̓]���̂ݍs���܂��B
 *	OAM�o�b�t�@�̏������́A�e���̃^�C�~���O�ōs���Ă��������B
 *	���̍ۂɂ́uGFL_CLACT_SysOamBuffClean�v�֐����g�p���Ă��������B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_SysVblankTransOnly( void )
{
	if( pClsys ){
		OAMMAN_SysTrans( &pClsys->oamman );
		TRMAN_SysVBlank( &pClsys->trman );
	}
}



//-------------------------------------
///	USER��`�����_���[�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e�����쐬����
 *
 *	@param	cp_data			���[�U�[��`�T�[�t�F�[�X�f�[�^
 *	@param	data_num		�f�[�^��
 *	@param	heapID			�q�[�vID
 *
 *	@return	�쐬���������_���[�V�X�e��
 */
//-----------------------------------------------------------------------------
CLSYS_REND* GFL_CLACT_UserRendCreate( const REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID )
{
	CLSYS_REND* p_rend;
	//OS_Printf( "[%d]\n", __LINE__ );
	GF_ASSERT( cp_data );
	
	p_rend = GFL_HEAP_AllocMemory( heapID, sizeof(CLSYS_REND) );
	REND_SysInit( p_rend, cp_data, data_num, heapID );
	return p_rend;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e����j������
 *
 *	@param	p_rend			���[�U�[��`�T�[�t�F�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendDelete( CLSYS_REND* p_rend )
{
	GF_ASSERT( p_rend );
	//OS_Printf( "[%d]\n", __LINE__ );

	REND_SysExit( p_rend );
	GFL_HEAP_FreeMemory( p_rend );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X������W��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_pos			�ݒ肷����W
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendSetSurfacePos( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_pos )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( p_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	REND_SurfaceObjSetPos( &p_rend->p_surface[ idx ], cp_pos->x, cp_pos->y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X������W���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendGetSurfacePos( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_pos )
{
	GF_ASSERT( cp_rend );
	GF_ASSERT( cp_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	REND_SurfaceObjGetPos( &cp_rend->p_surface[ idx ], &p_pos->x, &p_pos->y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�T�C�Y��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_size			�T�C�Y
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendSetSurfaceSize( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_size )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( p_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	REND_SurfaceObjSetSize( &p_rend->p_surface[ idx ], cp_size->x, cp_size->y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X�T�C�Y���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_size			�T�C�Y�擾��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendGetSurfaceSize( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_size )
{
	GF_ASSERT( cp_rend );
	GF_ASSERT( cp_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	REND_SurfaceObjGetSize( &cp_rend->p_surface[ idx ], &p_size->x, &p_size->y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�^�C�v��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	type			�T�[�t�F�[�X�^�C�v
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UserRendSetSurfaceType( CLSYS_REND* p_rend, u32 idx, CLSYS_DRAW_TYPE type )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( p_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	REND_SurfaceObjSetType( &p_rend->p_surface[ idx ], type );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X�^�C�v���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *
 *	@return	�T�[�t�F�[�X�^�C�v	�iCLSYS_DRAW_TYPE�j
 */
//-----------------------------------------------------------------------------
CLSYS_DRAW_TYPE GFL_CLACT_UserRendGetSurfaceType( const CLSYS_REND* cp_rend, u32 idx )
{
	GF_ASSERT( cp_rend );
	GF_ASSERT( cp_rend->surface_num > idx );
	//OS_Printf( "[%d]\n", __LINE__ );
	return REND_SurfaceObjGetType( &cp_rend->p_surface[ idx ] );
}


//-------------------------------------
///	CLUNIT�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�𐶐�
 *
 *	@param	wknum		���[�N��
 *	@param	heapID		�q�[�vID
 *
 *	@return	���j�b�g�|�C���^
 */
//-----------------------------------------------------------------------------
CLUNIT* GFL_CLACT_UnitCreate( u16 wknum, HEAPID heapID )
{
	CLUNIT* p_unit;
	int i;
	//OS_Printf( "[%d]\n", __LINE__ );

	// ���̂𐶐�
	p_unit = GFL_HEAP_AllocMemory( heapID, sizeof(CLUNIT) );
	GFL_STD_MemFill( p_unit, 0, sizeof(CLUNIT) );

	// ���[�N�쐬
	p_unit->p_wk = GFL_HEAP_AllocMemory( heapID, sizeof(CLWK)*wknum );
	p_unit->wk_num = wknum;
	for( i=0; i<wknum; i++ ){
		CLWK_SysClean( &p_unit->p_wk[i] );
	}

	// �f�t�H���g�����_���[�ݒ�
	GFL_CLACT_UnitSetDefaultRend( p_unit );
	
	// �\��ON
	GFL_CLACT_UnitSetDrawFlag( p_unit, TRUE );

	return p_unit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g��j��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UnitDelete( CLUNIT* p_unit )
{
	int i;
	
	GF_ASSERT( p_unit );
	//OS_Printf( "[%d]\n", __LINE__ );

	// �S���[�N�j��
	for( i=0; i<p_unit->wk_num; i++ ){
		if( p_unit->p_wk[i].p_next != NULL ){
			GFL_CLACT_WkDel( &p_unit->p_wk[i] );
		}
	}
	// ���[�N�j��
	GFL_HEAP_FreeMemory( p_unit->p_wk );
	// ���̂�j��
	GFL_HEAP_FreeMemory( p_unit );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�@�`�揈��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UnitDraw( CLUNIT* p_unit )
{
	CLWK* p_wk;

	GF_ASSERT( p_unit );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	// �\���t���O�������Ă��邩
	if( p_unit->draw_flag == FALSE ){
		return;
	}
	// �\�����郏�[�N�����邩�`�F�b�N
	if( p_unit->p_drawlist == NULL ){
		return;
	}

	// �擪�̃f�[�^��ݒ�
	p_wk = p_unit->p_drawlist;

	do{
		// �\��
		CLWK_SysDraw( p_wk, p_unit->p_rend );

		// �I�[�g�A�j������
		CLWK_SysAutoAnm( p_wk );

		// ����
		p_wk = p_wk->p_next;
	}while( p_wk != p_unit->p_drawlist );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�ɕ`��t���O��ݒ�
 *	
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	on_off		�`��ON-OFF
 *
 *	TRUE	�\��
 *	FALSE	��\��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UnitSetDrawFlag( CLUNIT* p_unit, BOOL on_off )
{
	GF_ASSERT( p_unit );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_unit->draw_flag = on_off;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`��t���O���擾
 *
 *	@param	cp_unit		�Z���A�N�^�[���j�b�g
 *
 *	@retval	TRUE	�\��
 *	@retval	FALSE	��\��
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_UnitGetDrawFlag( const CLUNIT* cp_unit )
{
	GF_ASSERT( cp_unit );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_unit->draw_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�Ƀ��[�U�[�Ǝ��̃����_���[�V�X�e����ݒ�
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	p_rend		���[�U�[�Ǝ��̃����_���[
 *
 *	���̊֐��ŁA�D���Ȑݒ�����������_���[���g�p���邱�Ƃ��o����悤�ɂȂ�܂��B
 *	�����[�U�[�Ǝ��̃����_���[���쐬����
 *		GFL_CLACT_UserRend�`�֐��ō쐬�ł��܂��B
 *
 *  �����ӎ���
 *	�@�@���W�ݒ肷��Ƃ��ɓn��setsf�̒l�́A���[�U�[�Ǝ������_���[
 *	�@�@�ɓo�^�����T�[�t�F�[�X�̃C���f�b�N�X�ԍ��ƂȂ�܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UnitSetUserRend( CLUNIT* p_unit, CLSYS_REND* p_rend )
{
	GF_ASSERT( p_unit );
	GF_ASSERT( p_rend );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_unit->p_rend = p_rend;
	p_unit->rend_type = CLUNIT_RENDTYPE_USER;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�Ƀf�t�H���g�����_���[�V�X�e����ݒ�
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *
 *	���̊֐��ŁA�Ǝ������_���[�V�X�e������f�t�H���g�����_���[�V�X�e����
 *	�g�p����悤�ɖ߂����Ƃ��o���܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_UnitSetDefaultRend( CLUNIT* p_unit )
{
	GF_ASSERT( pClsys );
	GF_ASSERT( p_unit );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_unit->p_rend = &pClsys->rend;
	p_unit->rend_type = CLUNIT_RENDTYPE_DEFF;
}


//-------------------------------------
///	CLWK�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���p���\�[�X�f�[�^�쐬
 *
 *	@param	p_res			���\�[�X�f�[�^�i�[��
 *	@param	cp_img			�C���[�W�v���N�V
 *	@param	cp_pltt			�p���b�g�v���N�V
 *	@param	p_cell			�Z���f�[�^�o���N
 *	@param	cp_canm			�Z���A�j���[�V�����f�[�^�o���N
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm )
{
	p_res->cp_img	= cp_img;
	p_res->cp_pltt	= cp_pltt;
	p_res->p_cell	= p_cell;
	p_res->cp_canm	= cp_canm;
	p_res->cp_mcell = NULL;
	p_res->cp_mcanm = NULL;
	p_res->cp_char	= NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����p���\�[�X�f�[�^�쐬
 *
 *	@param	p_res		���\�[�X�f�[�^�i�[��
 *	@param	cp_img		�C���[�W�v���N�V
 *	@param	cp_pltt     �p���b�g�v���N�V
 *	@param	p_cell      �Z���f�[�^�o���N
 *	@param	cp_canm     �Z���A�j���[�V�����f�[�^�o���N
 *	@param	cp_char     �L�����N�^�f�[�^
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetTrCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dCharacterData* cp_char )
{
	GFL_CLACT_WkSetCellResData( p_res, cp_img, cp_pltt, p_cell, cp_canm );
	p_res->cp_char = cp_char;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����@���\�[�X�f�[�^�쐬
 *
 *	@param	p_res			���\�[�X�f�[�^�i�[��
 *	@param	cp_img			�C���[�W�v���N�V
 *	@param	cp_pltt         �p���b�g�v���N�V
 *	@param	p_cell          �Z���f�[�^�o���N
 *	@param	cp_canm         �Z���A�j���[�V�����f�[�^�o���N
 *	@param	cp_mcell		�}���`�Z���f�[�^�o���N
 *	@param	cp_mcanm		�}���`�Z���A�j���[�V�����f�[�^�o���N
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetMCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dMultiCellDataBank* cp_mcell, const NNSG2dMultiCellAnimBankData* cp_mcanm )
{
	GFL_CLACT_WkSetCellResData( p_res, cp_img, cp_pltt, p_cell, cp_canm );
	p_res->cp_mcell = cp_mcell;
	p_res->cp_mcanm = cp_mcanm;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�̓o�^
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 *	@param	cp_data			�Z���A�N�^�[�f�[�^
 *	@param	cp_res			�Z���A�N�^�[���\�[�X
 *	@param	setsf			�ݒ�T�[�t�F�[�X
 *	@param	heapID			�q�[�vID
 *
 *	@return	�o�^�����Z���A�N�^�[���[�N
 *
 *	�ysetsf�̐����z
 *		CLUNIT�̎g�p���郌���_���[�V�X�e����ύX���Ă��Ȃ��Ƃ���
 *		CLSYS_DEFREND_TYPE�̒l���w�肷��
 *		�ECLSYS_DEFREND_MAIN�w�莞	pos_x/y�����C����ʍ�����W����̑��΍��W�ɂȂ�B
 *		�ECLSYS_DEFREND_SUB�w�莞	pos_x/y���T�u��ʍ�����W����̑��΍��W�ɂȂ�B
 *		
 *		�Ǝ��̃����_���[�V�X�e����CLUNIT�ɐݒ肵�Ă���Ƃ��́A
 *		�T�[�t�F�[�X�̗v�f�����w�肷�邱�ƂŁA
 *		�w�肳�ꂽ�T�[�t�F�[�X������W����̑��΍��W�ɂȂ�B
 *
 *		�ʏ�/�Ǝ������_���[�V�X�e�����ʂŁA
 *		CLWK_SETSF_NONE���w�肷��Ɛ�΍��W�ݒ�ɂȂ�
 */
//-----------------------------------------------------------------------------
CLWK* GFL_CLACT_WkAdd( CLUNIT* p_unit, const CLWK_DATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID )
{
	CLWK* p_wk;
	//OS_Printf( "[%d]\n", __LINE__ );
	
	// �󂢂Ă��郏�[�N���擾
	p_wk = CLUNIT_SysGetCleanWk( p_unit );

	// ���̂������ԑ厖�ł��I
	
	// �e�Z���A�N�^�[���j�b�g��ݒ�
	p_wk->p_unit = p_unit;

	// �Z���A�j���[�V�����f�[�^������
	CLWK_AnmDataInit( &p_wk->anmdata, cp_res, heapID );

	// ���[�N�Ƀp�����[�^�ݒ�
	CLWK_SysSetClwkData( p_wk, cp_data, setsf );
	CLWK_SysSetClwkRes( p_wk, cp_res );

	// �`�惊�X�g�ɓo�^
	CLUNIT_DrawListAdd( p_unit, p_wk );
	GFL_CLACT_WkSetDrawFlag( p_wk, TRUE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�̓o�^	�A�t�B���ϊ��o�[�W����
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 *	@param	cp_data			�Z���A�N�^�[�f�[�^�@�A�t�B���o�[�W����
 *	@param	cp_res			�Z���A�N�^�[���\�[�X
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 *	@param	heapID			�q�[�vID
 *
 *	@return	�o�^�����Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
CLWK* GFL_CLACT_WkAddAffine( CLUNIT* p_unit, const CLWK_AFFINEDATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID )
{
	CLWK* p_wk;
	GF_ASSERT( cp_data );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	// �o�^
	p_wk = GFL_CLACT_WkAdd( p_unit, &cp_data->clwkdata, cp_res, setsf, heapID );

	// �A�t�B���ݒ�
	GFL_CLACT_WkSetTypeAffinePos( p_wk, cp_data->affinepos_x, CLSYS_MAT_X );
	GFL_CLACT_WkSetTypeAffinePos( p_wk, cp_data->affinepos_x, CLSYS_MAT_Y );
	GFL_CLACT_WkSetTypeScale( p_wk, cp_data->scale_x, CLSYS_MAT_X );
	GFL_CLACT_WkSetTypeScale( p_wk, cp_data->scale_y, CLSYS_MAT_Y );
	GFL_CLACT_WkSetRotation( p_wk, cp_data->rotation );
	GFL_CLACT_WkSetAffineParam( p_wk, cp_data->affine_type );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j������
 *
 *	@param	p_wk	�j������Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkDel( CLWK* p_wk )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	// �`�惊�X�g����͂���
	CLUNIT_DrawListDel( p_wk->p_unit, p_wk );

	// �A�j���f�[�^�j��
	CLWK_AnmDataExit( &p_wk->anmdata );

	// ���[�N���N���A
	CLWK_SysClean( p_wk );
}



//----------------------------------------------------------------------------
/**
 *	@brief	�\���t���O�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	on_off			�\��ON-OFF
 *	TRUE	�\��
 *	FALSE	��\��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetDrawFlag( CLWK* p_wk, BOOL on_off )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->draw_flag = on_off;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\���t���O�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@retval	TRUE	�\��
 *	@retval	FALSE	��\��
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_WkGetDrawFlag( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->draw_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pos			���W�\����
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetPos( CLWK* p_wk, const CLSYS_POS* cp_pos, u16 setsf )
{
	CLSYS_POS sf_pos;
	CLSYS_POS wk_pos;

	GF_ASSERT( cp_pos );
	//OS_Printf( "[%d]\n", __LINE__ );

	// �ݒ�T�[�t�F�[�X�̍��W���擾����
	CLWK_SysGetSetSfPos( p_wk, setsf, &sf_pos );

	// ��΍��W�ɂ��Đݒ�
	wk_pos.x = cp_pos->x + sf_pos.x;
	wk_pos.y = cp_pos->y + sf_pos.y;
	GFL_CLACT_WkSetWldPos( p_wk, &wk_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetPos( const CLWK* cp_wk, CLSYS_POS* p_pos, u16 setsf )
{
	CLSYS_POS sf_pos;

	GF_ASSERT( p_pos );
	//OS_Printf( "[%d]\n", __LINE__ );

	// �ݒ�T�[�t�F�[�X�̍��W���擾����
	CLWK_SysGetSetSfPos( cp_wk, setsf, &sf_pos );

	// �����̍��W���擾����
	GFL_CLACT_WkGetWldPos( cp_wk, p_pos );

	// ���΍��W�ɕύX����
	p_pos->x -= sf_pos.x;
	p_pos->y -= sf_pos.y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ʐݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pos			�ݒ�l
 *	@param	setsf		�ݒ�T�[�t�F�[�X
 *	@param	type		���W�^�C�v
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetTypePos( CLWK* p_wk, s16 pos, u16 setsf, CLSYS_MAT_TYPE type )
{
	CLSYS_POS sf_pos;
	//OS_Printf( "[%d]\n", __LINE__ );
	// �ݒ�T�[�t�F�[�X�̍��W���擾����
	CLWK_SysGetSetSfPos( p_wk, setsf, &sf_pos );
	// ��΍��W�ɂ��Đݒ�
	if( type == CLSYS_MAT_X ){
		pos += sf_pos.x;
	}else{
		pos += sf_pos.y;
	}
	GFL_CLACT_WkSetWldTypePos( p_wk, pos, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ʎ擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	setsf		�ݒ�T�[�t�F�[�X
 *	@param	type		���W�^�C�v
 *
 *	@return	���W�^�C�v�̃T�[�t�F�[�X�����΍��W
 */
//-----------------------------------------------------------------------------
s16 GFL_CLACT_WkGetTypePos( const CLWK* cp_wk, u16 setsf, CLSYS_MAT_TYPE type )
{
	CLSYS_POS sf_pos;
	s16 pos;

	//OS_Printf( "[%d]\n", __LINE__ );
	// �ݒ�T�[�t�F�[�X�̍��W���擾����
	CLWK_SysGetSetSfPos( cp_wk, setsf, &sf_pos );

	// �����̍��W���擾����
	pos = GFL_CLACT_WkGetWldTypePos( cp_wk, type );

	// ���΍��W�ɕύX����
	if( type == CLSYS_MAT_X ){
		pos -= sf_pos.x;
	}else{
		pos -= sf_pos.y;
	}
	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N	
 *	@param	cp_pos			���W
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetWldPos( CLWK* p_wk, const CLSYS_POS* cp_pos )
{
	GF_ASSERT( cp_pos );
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->pos = *cp_pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetWldPos( const CLWK* cp_wk, CLSYS_POS* p_pos )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_pos );
	//OS_Printf( "[%d]\n", __LINE__ );
	*p_pos = cp_wk->pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ʐݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pos				�ݒ�l
 *	@param	type			���W�^�C�v
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetWldTypePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );
	if( type == CLSYS_MAT_X ){
		p_wk->pos.x = pos;
	}else{
		p_wk->pos.y = pos;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ʎ擾
 *	
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	type			���W�^�C�v
 *
 *	@return	���W�^�C�v�̐�΍��W
 */
//-----------------------------------------------------------------------------
s16 GFL_CLACT_WkGetWldTypePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type )
{
	s16 pos;
	GF_ASSERT( cp_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );
	if( type == CLSYS_MAT_X ){
		pos = cp_wk->pos.x;
	}else{
		pos = cp_wk->pos.y;
	}
	return pos;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���p�����[�^�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	affine		�A�t�B���p�����[�^
 *	affine
 *		CLSYS_AFFINETYPE_NONE	�㏑�����Ȃ�
 *		CLSYS_AFFINETYPE_NORMAL	�ʏ�̃A�t�B���ϊ������ɐݒ�
 *		CLSYS_AFFINETYPE_DOUBLE	�{�p�A�t�B���ϊ������ɐݒ�
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAffineParam( CLWK* p_wk, CLSYS_AFFINETYPE affine )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( CLSYS_AFFINETYPE_NUM > affine );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->affine = affine;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���p�����[�^�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	CLSYS_AFFINETYPE_NONE	�㏑�����Ȃ�
 *	@retval	CLSYS_AFFINETYPE_NORMAL	�ʏ�̃A�t�B���ϊ������ɐݒ�
 *	@retval	CLSYS_AFFINETYPE_DOUBLE	�{�p�A�t�B���ϊ������ɐݒ�
 */
//-----------------------------------------------------------------------------
CLSYS_AFFINETYPE GFL_CLACTWkGetAffineParam( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->affine;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	cp_pos		�A�t�B���ϊ����΍��W
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAffinePos( CLWK* p_wk, const CLSYS_POS* cp_pos )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_pos );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->affinepos = *cp_pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	p_pos		�A�t�B���ϊ����΍��W�擾��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetAffinePos( const CLWK* cp_wk, CLSYS_POS* p_pos )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_pos );
	//OS_Printf( "[%d]\n", __LINE__ );
	*p_pos = cp_wk->affinepos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ʐݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pos			�ݒ�l
 *	@param	type		���W�^�C�v
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetTypeAffinePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );
	if( type == CLSYS_MAT_X ){
		p_wk->affinepos.x = pos;
	}else{
		p_wk->affinepos.y = pos;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ʎ擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	type		���W�^�C�v
 *
 *	@return	���W�^�C�v�̃A�t�B���ϊ����΍��W
 */
//-----------------------------------------------------------------------------
s16 GFL_CLACT_WkGetTypeAffinePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type )
{
	s16 pos;

	GF_ASSERT( cp_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );

	if( type == CLSYS_MAT_X ){
		pos = cp_wk->affinepos.x;
	}else{
		pos = cp_wk->affinepos.y;
	}
	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ꊇ�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_sca			�g��k���l
 *
 *	*AffineParam��
 *		CLSYS_AFFINETYPE_NORMAL		�ʏ�
 *		CLSYS_AFFINETYPE_DOUBLE		�{�p
 *	���ݒ肳��Ă��Ȃ��ƕ`��ɔ��f����܂���B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetScale( CLWK* p_wk, const CLSYS_SCALE* cp_sca )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_sca );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->scale = *cp_sca;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ꊇ�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_sca			�g��k���l�擾��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetScale( const CLWK* cp_wk, CLSYS_SCALE* p_sca )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_sca );
	//OS_Printf( "[%d]\n", __LINE__ );
	*p_sca = cp_wk->scale;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ʐݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	scale			�ݒ�l
 *	@param	type			���W�^�C�v
 *
 *	*AffineParam��
 *		CLSYS_AFFINETYPE_NORMAL		�ʏ�
 *		CLSYS_AFFINETYPE_DOUBLE		�{�p
 *	���ݒ肳��Ă��Ȃ��ƕ`��ɔ��f����܂���B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetTypeScale( CLWK* p_wk, fx32 scale, CLSYS_MAT_TYPE type )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );
	if( type == CLSYS_MAT_X ){
		p_wk->scale.x = scale;
	}else{
		p_wk->scale.y = scale;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ʎ擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	type			���W�^�C�v
 *
 *	@return	���W�^�C�v�̊g��k���l
 */
//-----------------------------------------------------------------------------
fx32 GFL_CLACT_WkGetTypeScale( const CLWK* cp_wk, CLSYS_MAT_TYPE type )
{
	fx32 scale;

	GF_ASSERT( cp_wk );
	GF_ASSERT( CLSYS_MAT_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	if( type == CLSYS_MAT_X ){
		scale = cp_wk->scale.x;
	}else{
		scale = cp_wk->scale.y;
	}
	return scale;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]�p�x�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	rotation		��]�p�x(0�`0xffff 0xffff��360�x)
 *
 *	*AffineParam��
 *		CLSYS_AFFINETYPE_NORMAL		�ʏ�
 *		CLSYS_AFFINETYPE_DOUBLE		�{�p
 *	���ݒ肳��Ă��Ȃ��ƕ`��ɔ��f����܂���B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetRotation( CLWK* p_wk, u16 rotation )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->rotation = rotation;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]�p�x���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	��]�p�x(0�`0xffff 0xffff��360�x)
 */
//-----------------------------------------------------------------------------
u16  GFL_CLACT_WkGetRotation( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->rotation;
}

//----------------------------------------------------------------------------
/**
 *	@brief		�t���b�v�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	flip_type		�ݒ�t���b�v�^�C�v
 *	@param	on_off			ONOFF�t���O	TRUE:On	FALSE:Off
 *
 *	*AffineParam��NNS_G2D_RND_AFFINE_OVERWRITE_NONE�ɂȂ��Ă��Ȃ��Ɣ��f����܂���
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetFlip( CLWK* p_wk, CLWK_FLIP_TYPE flip_type, BOOL on_off )
{
	u8 msk;

	GF_ASSERT( p_wk );
	GF_ASSERT( CLWK_FLIP_MAX > flip_type );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	if( flip_type == CLWK_FLIP_V ){
		msk = CLSYS_VFLIP;
	}else{
		msk = CLSYS_HFLIP;
	}
	if( on_off == FALSE ){
		p_wk->vhflip &= ~msk;
	}else{
		p_wk->vhflip |= msk;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���b�v�ݒ���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	flip_type		�t���b�v�^�C�v
 *
 *	@retval	TRUE	�t���b�v�ݒ�
 *	@retval	FALSE	�t���b�v���ݒ�
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_WkGetFlip( const CLWK* cp_wk, CLWK_FLIP_TYPE flip_type )
{
	BOOL ret = FALSE;

	//OS_Printf( "[%d]\n", __LINE__ );
	GF_ASSERT( cp_wk );
	GF_ASSERT( CLWK_FLIP_MAX > flip_type );
	
	if( flip_type == CLWK_FLIP_V ){
		if( cp_wk->vhflip & CLSYS_VFLIP ){
			ret = TRUE;
		}
	}else{
		if( cp_wk->vhflip & CLSYS_HFLIP ){
			ret = TRUE;
		}
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief		�I�u�W�F���[�h�ݒ�
 *	
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	mode		�I�u�W�F���[�h
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetObjMode( CLWK* p_wk, GXOamMode mode )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( GX_OAM_MODE_BITMAPOBJ >= mode );

	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->objmode = mode;
	
	if(mode == GX_OAM_MODE_NORMAL){
		p_wk->over_write ^= NNS_G2D_RND_OVERWRITE_OBJMODE;
	}else{
		p_wk->over_write |= NNS_G2D_RND_OVERWRITE_OBJMODE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�N�g���[�h�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	GX_OAM_MODE_NORMAL		�m�[�}��OBJ
 *	@retval	GX_OAM_MODE_XLU			������OBJ 
 *	@retval	GX_OAM_MODE_OBJWND		OBJ�E�B���h�E
 *	@retval	GX_OAM_MODE_BITMAPOBJ	�r�b�g�}�b�vOBJ
 */
//-----------------------------------------------------------------------------
GXOamMode GFL_CLACT_WkGetObjMode( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->objmode;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���U�C�N�t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		���U�C�NON�|OFF	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetMosaic( CLWK* p_wk, BOOL on_off )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );

	p_wk->mosaic = on_off;
	if( on_off == TRUE ){
		p_wk->over_write |= NNS_G2D_RND_OVERWRITE_MOSAIC;
	}else{
		p_wk->over_write ^= NNS_G2D_RND_OVERWRITE_MOSAIC;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���U�C�N�t���O�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	TRUE	���U�C�N�ݒ�
 *	@retval	FALSE	���U�C�N���ݒ�
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_WkGetMosaic( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->mosaic;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pal_offs	�p���b�g�I�t�Z�b�g
 *
 *	�p���b�g�v���N�V�A�h���X+pal_offs+OamAttr.c_param��
 *	�p���b�g�ԍ����g�p����悤�ɂȂ�܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetPlttOffs( CLWK* p_wk, u8 pal_offs )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( 16 > pal_offs );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->pal_offs = pal_offs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	pal_offs	�p���b�g�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
u8 GFL_CLACT_WkGetPlttOffs( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->pal_offs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�v���N�V�̃A�h���X�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	type		�`��^�C�v
 *
 *	@retval	CLWK_VRAM_ADDR_NONE�ȊO	�p���b�gVram�A�h���X
 *	@retval	CLWK_VRAM_ADDR_NONE		�ǂݍ��܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 GFL_CLACT_WkGetPlttAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type )
{
	BOOL result;

	//OS_Printf( "[%d]\n", __LINE__ );
	GF_ASSERT( cp_wk );
	GF_ASSERT( CLSYS_DRAW_MAX > type );

	// �`��^�C�v��VRAM�Ƀp���b�g���ǂݍ��܂�Ă��邩�`�F�b�N
	result = NNS_G2dIsImagePaletteReadyToUse( &cp_wk->pltt_proxy, CLSYS_DRAW_TYPEtoNNS_G2D_VRAM_TYPE[ type ] );
	if( result == FALSE ){
		return CLWK_VRAM_ADDR_NONE;
	}
	return NNS_G2dGetImagePaletteLocation( &cp_wk->pltt_proxy, CLSYS_DRAW_TYPEtoNNS_G2D_VRAM_TYPE[ type ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�����p���b�g�v���N�V��ݒ肷��
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pltt			�ݒ肷��p���b�g�v���N�V
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetPlttProxy( CLWK* p_wk, const NNSG2dImagePaletteProxy* cp_pltt )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_pltt );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->pltt_proxy = *cp_pltt;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�v���N�V�f�[�^�̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pltt			�p���b�g�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetPlttProxy( const CLWK* cp_wk,  NNSG2dImagePaletteProxy* p_pltt )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_pltt );
	*p_pltt = cp_wk->pltt_proxy;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�f�[�^Vram�A�h���X���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	type		�`��^�C�v
 *
 *	@retval	CLWK_VRAM_ADDR_NONE�ȊO	�L�����N�^Vram�A�h���X
 *	@retval	CLWK_VRAM_ADDR_NONE		�ǂݍ��܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 GFL_CLACT_WkGetCharAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type )
{
	BOOL result;

	GF_ASSERT( cp_wk );
	GF_ASSERT( CLSYS_DRAW_MAX > type );
	//OS_Printf( "[%d]\n", __LINE__ );

	// �`��^�C�v��VRAM�ɃL�����N�^���ǂݍ��܂�Ă��邩�`�F�b�N
	result = NNS_G2dIsImageReadyToUse( &cp_wk->img_proxy, CLSYS_DRAW_TYPEtoNNS_G2D_VRAM_TYPE[ type ] );
	if( result == FALSE ){
		return CLWK_VRAM_ADDR_NONE;
	}
	return NNS_G2dGetImageLocation( &cp_wk->img_proxy, CLSYS_DRAW_TYPEtoNNS_G2D_VRAM_TYPE[ type ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V��ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_img			�ݒ�C���[�W�v���N�V
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetImgProxy( CLWK* p_wk, const NNSG2dImageProxy* cp_img )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_img );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->img_proxy = *cp_img;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_img			�C���[�W�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkGetImgProxy( const CLWK* cp_wk,  NNSG2dImageProxy* p_img )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_img );
	*p_img = cp_wk->img_proxy;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̐ݒ�
 *	
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				�\�t�g�D�揇��	0>0xff
 *	
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetSoftPri( CLWK* p_wk, u8 pri )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );

	p_wk->soft_pri = pri;
	CLUNIT_DrawListDel( p_wk->p_unit, p_wk );
	CLUNIT_DrawListAdd( p_wk->p_unit, p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	�\�t�g�D�揇��
 */
//-----------------------------------------------------------------------------
u8 GFL_CLACT_WkGetSoftPri( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	
	return cp_wk->soft_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ̐ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				BG�D�揇��
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetBgPri( CLWK* p_wk, u8 pri )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( 4 > pri );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->bg_pri = pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ��擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	BG�D�揇��
 */
//-----------------------------------------------------------------------------
u8 GFL_CLACT_WkGetBgPri( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->bg_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAnmSeq( CLWK* p_wk, u16 anmseq )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->anmseq = anmseq;
	CLWK_AnmDataSetAnmSeq( &p_wk->anmdata, anmseq );

	// �A�j���[�V�����J�n
	GFL_CLACT_WkStartAnm( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
u16 GFL_CLACT_WkGetAnmSeq( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->anmseq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���ς���Ă�����ύX����
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAnmSeqDiff( CLWK* p_wk, u16 anmseq )
{
	//OS_Printf( "[%d]\n", __LINE__ );
	GF_ASSERT( p_wk );
	if( anmseq != p_wk->anmseq ){
		GFL_CLACT_WkSetAnmSeq( p_wk, anmseq );
		// �A�j���[�V�����J�n
		GFL_CLACT_WkStartAnm( p_wk );
	}
}

#if 0	// �g�p�p�r���킩��Ȃ��̂Ŕr��
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX����@
 *			���̍ۃt���[�����ԂȂǂ̃��Z�b�g���s���܂���B�@MultiCell��Ή�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAnmSeqNoReset( CLWK* p_wk, u16 anmseq )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->anmseq = anmseq;
	CLWK_AnmDataSetAnmSeqNoReset( &p_wk->anmdata, anmseq );
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	idx			�t���[����
 *	NitroCharacter�̃A�j���[�V�����V�[�P���X���̃R�}�ԍ����w�肵�Ă�������
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAnmFrame( CLWK* p_wk, u16 idx )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	CLWK_AnmDataSetAnmFrame( &p_wk->anmdata, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����i�߂�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	speed		�i�߂�A�j���[�V�����X�s�[�h
 *
 *	*�t�Đ����\�ł�
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkAddAnmFrame( CLWK* p_wk, fx32 speed )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	CLWK_AnmDataSetAnmSpeed( &p_wk->anmdata, speed );
	CLWK_AnmDataAddAnmFrame( &p_wk->anmdata, FX32_ONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[�������擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
u16 GFL_CLACT_WkGetAnmFrame( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return CLWK_AnmDataGetAnmFrame( &cp_wk->anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		�I�[�g�A�j���[�V����On-Off	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAutoAnmFlag( CLWK* p_wk, BOOL on_off )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->auto_anm = on_off;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	TRUE	�I�[�g�A�j��ON
 *	@retval	FALSE	�I�[�g�A�j��OFF
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_WkGetAutoAnmFlag( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->auto_anm;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����X�s�[�h�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	speed		�A�j���[�V�����X�s�[�h		
 *
 *	*�t�Đ����\�ł�
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAutoAnmSpeed( CLWK* p_wk, fx32 speed )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->auto_anm_speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����X�s�[�h���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�I�[�g�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
fx32 GFL_CLACT_WkGetAutoAnmSpeed( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return cp_wk->auto_anm_speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����J�n
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	GFL_CLACT_WkStopAnm�łƂ߂��A�j���[�V�������J�n�����܂��B
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkStartAnm( CLWK* p_wk )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	CLWK_AnmDataStartAnm( &p_wk->anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V������~
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkStopAnm( CLWK* p_wk )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	CLWK_AnmDataStopAnm( &p_wk->anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N	�A�j���[�V��������`�F�b�N
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	TRUE	�Đ���
 *	@retval	FALSE	��~��
 */
//-----------------------------------------------------------------------------
BOOL GFL_CLACT_WkCheckAnmActive( const CLWK* cp_wk )
{
	GF_ASSERT( cp_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	return CLWK_AnmDataCheckAnmActive( &cp_wk->anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������X�^�[�g
 *
 *	@param	p_wk	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkResetAnm( CLWK* p_wk )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	CLWK_AnmDataResetAnm( &p_wk->anmdata );
	CLWK_AnmDataStartAnm( &p_wk->anmdata );	// �A�N�e�B�u�t���O�����������Ȃ��̂ŁA�蓮�ōĊJ������
}

//----------------------------------------------------------------------------
/**
 *	@brief	�㏑���A�j���[�V�������샂�[�h�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	playmode	���샂�[�h
 *
 *	playmode
 *	CLSYS_ANMPM_INVALID		�����i�����ݒ肷���NitroCharacter�Őݒ肵���̃A�j�������ɂȂ�܂��j
 *	CLSYS_ANMPM_FORWARD		�����^�C���Đ�(������)
 *	CLSYS_ANMPM_FORWARD_L	���s�[�g�Đ�(���������[�v)
 *	CLSYS_ANMPM_REVERSE		�����Đ�(���o�[�X�i���{�t�����j
 *	CLSYS_ANMPM_REVERSE_L	�����Đ����s�[�g�i���o�[�X�i���{�t�������j ���[�v�j
 *		
 */
//-----------------------------------------------------------------------------
void GFL_CLACT_WkSetAnmMode( CLWK* p_wk, CLSYS_ANM_PLAYMODE playmode )
{
	GF_ASSERT( p_wk );
	//OS_Printf( "[%d]\n", __LINE__ );
	p_wk->playmode = playmode;
	CLWK_AnmDataSetAnmMode( &p_wk->anmdata, playmode );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�㏑���A�j���[�V�����������擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	CLSYS_ANMPM_INVALID		����(NitroCharacter�Őݒ肵���A�j�������ŃA�j�����Ă��܂�)
 *	@retval	CLSYS_ANMPM_FORWARD		�����^�C���Đ�(������)
 *	@retval	CLSYS_ANMPM_FORWARD_L	���s�[�g�Đ�(���������[�v)
 *	@retval	CLSYS_ANMPM_REVERSE		�����Đ�(���o�[�X�i���{�t�����j
 *	@retval	CLSYS_ANMPM_REVERSE_L	�����Đ����s�[�g�i���o�[�X�i���{�t�������j ���[�v�j
 */
//-----------------------------------------------------------------------------
CLSYS_ANM_PLAYMODE GFL_CLACT_WkGetAnmMode( const CLWK* cp_wk )
{
	//OS_Printf( "[%d]\n", __LINE__ );
	GF_ASSERT( cp_wk );
	return cp_wk->playmode;
}




//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐��S
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	CLSYS�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�t�H���g�ݒ背���_���[�쐬
 *
 *	@param	p_rend		�����_���[�V�X�e��
 *	@param	cp_data		�������f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void CLSYS_DefaultRendInit( CLSYS_REND* p_rend, const CLSYS_INIT* cp_data, HEAPID heapID )
{
	// �f�t�H���g�����_���[�@�T�[�t�F�[�X�ݒ�
	static REND_SURFACE_INIT c_defsurface[ CLSYS_DRAW_MAX ] = {
		{
			0, 0,
			256, 192,
			CLSYS_DRAW_MAIN
		},
		{
			0, 0,
			256, 192,
			CLSYS_DRAW_SUB
		}
	};

	// ������W�����ݒ�
	c_defsurface[ CLSYS_DEFREND_MAIN ].lefttop_x = cp_data->surface_main_left;
	c_defsurface[ CLSYS_DEFREND_MAIN ].lefttop_y = cp_data->surface_main_top;
	c_defsurface[ CLSYS_DEFREND_SUB ].lefttop_x = cp_data->surface_sub_left;
	c_defsurface[ CLSYS_DEFREND_SUB ].lefttop_y = cp_data->surface_sub_top;
	
	REND_SysInit( p_rend, c_defsurface, CLSYS_DRAW_MAX, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���Ƀp���b�g�v���N�V��ݒ�
 *	
 *	@param	cp_pltt		�p���b�g�v���N�V
 *	@param	pal_offs	�p���b�g�I�t�Z�b�g
 */	
//-----------------------------------------------------------------------------
static void CLSYS_SysSetPaletteProxy( const NNSG2dImagePaletteProxy* cp_pltt, u8 pal_offs )
{
	GF_ASSERT( pClsys );
	GF_ASSERT( cp_pltt );
	GF_ASSERT( 16 > pal_offs );
	
	// �p���b�g�v���N�V����A�h���X���擾����
	pClsys->pltt_no[ CLSYS_DRAW_MAIN ] = NNS_G2dGetImagePaletteLocation( cp_pltt, NNS_G2D_VRAM_TYPE_2DMAIN ) / 32;
	pClsys->pltt_no[ CLSYS_DRAW_SUB ] = NNS_G2dGetImagePaletteLocation( cp_pltt, NNS_G2D_VRAM_TYPE_2DSUB ) / 32;

	pClsys->pltt_no[ CLSYS_DRAW_MAIN ] += pal_offs;
	pClsys->pltt_no[ CLSYS_DRAW_SUB ] += pal_offs;
}



//-------------------------------------
///	CLSYS_OamMan�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���V�X�e���̏�����
 *
 *	@param	p_oamman		OAM�}�l�[�W���V�X�e��
 *	@param	oamst_main		���C�����OAM�Ǘ��J�n�ʒu
 *	@param	oamnum_main		���C�����OAM�Ǘ���
 *	@param	oamst_sub		�T�u���OAM�Ǘ��J�n�ʒu
 *	@param	oamnum_sub		�T�u���OAM�Ǘ���
 */
//-----------------------------------------------------------------------------
static void OAMMAN_SysInit( CLSYS_OAMMAN* p_oamman, u8 oamst_main, u8 oamnum_main, u8 oamst_sub, u8 oamnum_sub )
{
	GF_ASSERT( p_oamman );
	
	// NitroSystem OamManagerSystem�̏�����
	NNS_G2dInitOamManagerModule();

	// ���C���ƃT�u��OAM�}�l�[�W���C���X�^���X���쐬
	OAMMAN_ObjCreate( &p_oamman->man[NNS_G2D_OAMTYPE_MAIN], oamst_main, oamnum_main, NNS_G2D_OAMTYPE_MAIN );
	OAMMAN_ObjCreate( &p_oamman->man[NNS_G2D_OAMTYPE_SUB], oamst_sub, oamnum_sub, NNS_G2D_OAMTYPE_SUB );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���V�X�e���̔j��
 *
 *	@param	p_oamman		OAM�}�l�[�W���V�X�e��
 */
//-----------------------------------------------------------------------------
static void OAMMAN_SysExit( CLSYS_OAMMAN* p_oamman )
{
	GF_ASSERT( p_oamman );	// OAMMAN�������Ă��Ȃ��Ȃ炱�̏������s�����Ƃ��o���Ȃ��悤�ɂ��邽��
	// NitroSystem OamManagerSystem�̏�����
	NNS_G2dInitOamManagerModule();
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���V�X�e��	�]������
 *
 *	@param	p_oamman		OAM�}�l�[�W���V�X�e��
 */
//-----------------------------------------------------------------------------
static void OAMMAN_SysTrans( CLSYS_OAMMAN* p_oamman )
{
	int i;
	GF_ASSERT( p_oamman );
	for( i=0; i<CLSYS_DRAW_MAX; i++ ){
		OAMMAN_ObjTrans( &p_oamman->man[ i ] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���V�X�e���@�o�b�t�@�N���[������
 *
 *	@param	p_oamman		OAM�}�l�[�W���V�X�e��
 */
//-----------------------------------------------------------------------------
static void OAMMAN_SysClean( CLSYS_OAMMAN* p_oamman )
{
	int i;
	GF_ASSERT( p_oamman );
	for( i=0; i<CLSYS_DRAW_MAX; i++ ){
		OAMMAN_ObjClearBuff( &p_oamman->man[ i ] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���I�u�W�F�N�g�̏�����
 *
 *	@param	p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 *	@param	oamst			OAM�Ǘ��J�n�ʒu
 *	@param	oamnum			OAM�Ǘ���
 *	@param	oam_type		OAM�^�C�v
 */
//-----------------------------------------------------------------------------
static void OAMMAN_ObjCreate( OAMMAN_DATA* p_obj, u8 oamst, u8 oamnum, NNSG2dOamType oam_type )
{
	BOOL result;

	GF_ASSERT( p_obj );

	if( oamnum == 0 ){
		p_obj->init = FALSE;
	}else{
		
		// �����]��Ӱ�ނ�OAM�}�l�[�W��������������
		result = NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(
				&p_obj->man,
				oamst,
				oamnum,
				oam_type
				);
		GF_ASSERT( result == TRUE );

		p_obj->init = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���I�u�W�F�N�g��OAMAttr��o�^
 *
 *	@param	p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 *	@param	pOam			OAMAttr
 *	@param	affineIndex		�A�t�B��Index
 *
 *	@retval	TRUE	�o�^����
 *	@retval	FALSE	�o�^���s
 */
//-----------------------------------------------------------------------------
static BOOL OAMMAN_ObjEntryOamAttr( OAMMAN_DATA* p_obj, const GXOamAttr* pOam, u16 affineIndex )
{
	GF_ASSERT( p_obj );
	GF_ASSERT( p_obj->init );
	GF_ASSERT( pOam );
	
	return NNS_G2dEntryOamManagerOamWithAffineIdx(
			&p_obj->man,
			pOam,
			affineIndex
			);
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���I�u�W�F�N�g�ɃA�t�B���s���o�^
 *
 *	@param	p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 *	@param	mtx				�A�t�B���}�g���N�X
 *
 *	@return	�o�^�����A�t�B��Index
 */
//-----------------------------------------------------------------------------
static u16 OAMMAN_ObjEntryAffine( OAMMAN_DATA* p_obj, const MtxFx22* mtx )
{
	GF_ASSERT( p_obj );
	GF_ASSERT( p_obj->init );
	GF_ASSERT( mtx );
	return NNS_G2dEntryOamManagerAffine(
			&p_obj->man,
			mtx
			);
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���I�u�W�F�N�g�@RAM�]������
 *
 *	@param	 p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------
static void OAMMAN_ObjTrans( OAMMAN_DATA* p_obj )
{
	GF_ASSERT( p_obj );
	if( p_obj->init ){
		NNS_G2dApplyOamManagerToHW( &p_obj->man );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���I�u�W�F�N�g�@�o�b�t�@�N���A
 *
 *	@param	p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------
static void OAMMAN_ObjClearBuff( OAMMAN_DATA* p_obj )
{
	GF_ASSERT( p_obj );
	if( p_obj->init ){
		NNS_G2dResetOamManagerBuffer( &p_obj->man );
	}
}


//-------------------------------------
///	CLSYS_REND�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���̏�����
 *
 *	@param	p_rend		�����_���[�V�X�e��
 *	@param	cp_data		�o�^�T�[�t�F�[�X�f�[�^
 *	@param	data_num	�f�[�^��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void REND_SysInit( CLSYS_REND* p_rend, const REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID )
{
	int i;

	GF_ASSERT( p_rend );
	GF_ASSERT( cp_data );
	
	// �����_���[������
	NNS_G2dInitRenderer( &p_rend->rend );
	NNS_G2dSetRendererSpriteZoffset( &p_rend->rend, CLSYS_DEFFREND_ZOFFS );

	
	// �T�[�t�F�[�X�I�u�W�F�N�g�������m��
	p_rend->p_surface = GFL_HEAP_AllocMemory( heapID, sizeof(NNSG2dRenderSurface)*data_num );
	p_rend->surface_num = data_num;

	// �T�[�t�F�[�X�I�u�W�F�N�g������
	// �����_���[�ɓo�^
	for( i=0; i<data_num; i++ ){
		REND_SurfaceObjCreate( &p_rend->p_surface[i], &cp_data[i] );
		NNS_G2dAddRendererTargetSurface( &p_rend->rend, &p_rend->p_surface[i] );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���̔j��
 *
 *	@param	p_rend		�����_���[�V�X�e��
 */
//-----------------------------------------------------------------------------
static void REND_SysExit( CLSYS_REND* p_rend )
{
	GF_ASSERT( p_rend );
	// �T�[�t�F�[�X�I�u�W�F�N�g�������j��
	GFL_HEAP_FreeMemory( p_rend->p_surface );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���@�`��O����
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	pImgProxy		�C���[�W�v���N�V
 *	@param	pPltProxy		�p���b�g�v���N�V
 */
//-----------------------------------------------------------------------------
static void REND_SysBeginDraw( CLSYS_REND* p_rend, const NNSG2dImageProxy* pImgProxy, const NNSG2dImagePaletteProxy* pPltProxy )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( pImgProxy );
	GF_ASSERT( pPltProxy );
	NNS_G2dSetRendererImageProxy( &p_rend->rend, pImgProxy, pPltProxy );
	NNS_G2dBeginRendering( &p_rend->rend );

}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e��	�`��㏈��
 *
 *	@param	p_rend			�����_���[�V�X�e��
 */
//-----------------------------------------------------------------------------
static void REND_SysEndDraw( CLSYS_REND* p_rend )
{
	GF_ASSERT( p_rend );
	NNS_G2dEndRendering();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�ɃA�t�B���ϊ��㏑���ݒ���s��
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	mode			�A�t�B���ϊ����[�h
 */
//-----------------------------------------------------------------------------
static void REND_SysSetAffine( CLSYS_REND* p_rend, CLSYS_AFFINETYPE mode )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( CLSYS_AFFINETYPE_NUM > mode );
	NNS_G2dSetRndCoreAffineOverwriteMode(
			&(p_rend->rend.rendererCore),
			mode );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���@�t���b�v�ݒ���s��
 *
 *	@param	p_rend		�����_���[�V�X�e��
 *	@param	vflip		V�t���b�v�L��
 *	@param	hflip		H�t���b�v�L��
 */
//-----------------------------------------------------------------------------
static void REND_SysSetFlip( CLSYS_REND* p_rend, BOOL vflip, BOOL hflip )
{
	GF_ASSERT( p_rend );
	
	NNS_G2dSetRndCoreFlipMode(
			&(p_rend->rend.rendererCore),
			vflip,
			hflip );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���ɃI�[�o�[���C�h�t���O��ݒ�
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	over_write		�I�[�o�[���C�h�t���O
 */
//-----------------------------------------------------------------------------
static void REND_SysSetOverwrite( CLSYS_REND* p_rend, u8 over_write )
{
	GF_ASSERT( p_rend );
	// �I�[�o�[���C�g�L���t���O�ݒ�
	NNS_G2dSetRendererOverwriteEnable( &p_rend->rend, over_write );
	// �I�[�o�[���C�g�����t���O�ݒ�
	NNS_G2dSetRendererOverwriteDisable( &p_rend->rend, ~over_write );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���Ƀ��U�C�N�̗L����ݒ�
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	on_off			���U�C�N�̗L��
 */
//-----------------------------------------------------------------------------
static void REND_SysSetMosaicFlag( CLSYS_REND* p_rend, BOOL on_off )
{
	GF_ASSERT( p_rend );
	// �I�[�o�[���C�g�Ƀ��U�C�N�̒l��ݒ�
	NNS_G2dSetRendererOverwriteMosaicFlag( &p_rend->rend, on_off );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���ɏ㏑������I�u�W�F���[�h��ݒ�
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	objmode			�I�u�W�F���[�h
 */
//-----------------------------------------------------------------------------
static void REND_SysSetOBJMode( CLSYS_REND* p_rend, GXOamMode objmode )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( GX_OAM_MODE_BITMAPOBJ > objmode );
	// �I�[�o�[���C�g��OBJ���[�h�̒l��ݒ�
	NNS_G2dSetRendererOverwriteOBJMode( &p_rend->rend, objmode );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�V�X�e���ɏ㏑������BG�D�揇�ʂ�ݒ�
 *
 *	@param	p_rend			�����_���[�V�X�e��
 *	@param	pri				BG�D�揇��
 */
//-----------------------------------------------------------------------------
static void REND_SysSetBGPriority( CLSYS_REND* p_rend, u8 pri )
{
	GF_ASSERT( p_rend );
	GF_ASSERT( 4 > pri );
	// BG�D�揇�ʂ�ݒ�
	NNS_G2dSetRendererOverwritePriority( &p_rend->rend, pri );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g������
 *
 *	@param	p_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	cp_data			�T�[�t�F�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjCreate( NNSG2dRenderSurface* p_surface, const REND_SURFACE_INIT* cp_data )
{
	// �R�[���o�b�N�f�[�^
	static const REND_SURFACE_CALLBACK callback[ CLSYS_DRAW_MAX ] = {
		{
			REND_CallBackMainAddOam,
			REND_CallBackMainAddAffine,
			REND_CallBackCulling
		},
		{
			REND_CallBackSubAddOam,
			REND_CallBackSubAddAffine,
			REND_CallBackCulling
		},
	};

	GF_ASSERT( cp_data );
	GF_ASSERT( CLSYS_DRAW_MAX > cp_data->type );

	NNS_G2dInitRenderSurface(p_surface);
	
	// �T�[�t�F�[�X�f�[�^�ݒ�
	REND_SurfaceObjSetPos( p_surface, cp_data->lefttop_x, cp_data->lefttop_y );
	REND_SurfaceObjSetSize( p_surface, cp_data->width, cp_data->height );
	REND_SurfaceObjSetType( p_surface, cp_data->type );
	REND_SurfaceObjSetCallBack( p_surface, &callback[ cp_data->type ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�ɍ�����W��ݒ肷��
 *
 *	@param	p_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	x				���゘���W
 *	@param	y				���゙���W
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjSetPos( NNSG2dRenderSurface* p_surface, s16 x, s16 y )
{
	GF_ASSERT( p_surface );
	p_surface->viewRect.posTopLeft.x = x << FX32_SHIFT;
	p_surface->viewRect.posTopLeft.y = y << FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�̑傫����ݒ�
 *
 *	@param	p_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	width			��
 *	@param	height			����
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjSetSize( NNSG2dRenderSurface* p_surface, s16 width, s16 height )
{
	GF_ASSERT( p_surface );
	p_surface->viewRect.sizeView.x = width << FX32_SHIFT;
	p_surface->viewRect.sizeView.y = height << FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�̃^�C�v��ݒ�
 *
 *	@param	p_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	type			�T�[�t�F�[�X�^�C�v
 */	
//-----------------------------------------------------------------------------
static void REND_SurfaceObjSetType( NNSG2dRenderSurface* p_surface, CLSYS_DRAW_TYPE type )
{
	static const u16 type_dict[ CLSYS_DRAW_MAX ] = {
		NNS_G2D_SURFACETYPE_MAIN2D,
		NNS_G2D_SURFACETYPE_SUB2D,
	};

	GF_ASSERT( p_surface );
	GF_ASSERT( CLSYS_DRAW_MAX > type );

	p_surface->type = type_dict[ type ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�Ƀf�t�H���g�̃R�[���o�b�N�֐���ݒ�
 *
 *	@param	p_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	cp_callback		�R�[���o�b�N�f�[�^
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjSetCallBack( NNSG2dRenderSurface* p_surface, const REND_SURFACE_CALLBACK* cp_callback )
{
	GF_ASSERT( p_surface );
	GF_ASSERT( cp_callback );
	p_surface->pFuncOamRegister			= cp_callback->pFuncOamRegister;
	p_surface->pFuncOamAffineRegister	= cp_callback->pFuncOamAffineRegister;
	p_surface->pFuncVisibilityCulling	= cp_callback->pFuncVisibilityCulling;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g������W���擾
 *	
 *	@param	cp_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	p_x				���゘���W�i�[��
 *	@param	p_y				���゙���W�i�[��
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjGetPos( const NNSG2dRenderSurface* cp_surface, s16* p_x, s16* p_y )
{
	GF_ASSERT( cp_surface );
	GF_ASSERT( p_x );
	GF_ASSERT( p_y );
	*p_x = cp_surface->viewRect.posTopLeft.x >> FX32_SHIFT;
	*p_y = cp_surface->viewRect.posTopLeft.y >> FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�̑傫�����擾
 *
 *	@param	cp_surface		�T�[�t�F�[�X�I�u�W�F�N�g
 *	@param	p_width			���i�[��
 *	@param	p_height		�����i�[��
 */
//-----------------------------------------------------------------------------
static void REND_SurfaceObjGetSize( const NNSG2dRenderSurface* cp_surface, s16* p_width, s16* p_height )
{
	GF_ASSERT( cp_surface );
	GF_ASSERT( p_width );
	GF_ASSERT( p_height );
	*p_width	= cp_surface->viewRect.sizeView.x >> FX32_SHIFT;
	*p_height	= cp_surface->viewRect.sizeView.y >> FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�I�u�W�F�N�g�^�C�v�擾
 *
 *	@param	cp_surface				�T�[�t�F�[�X�I�u�W�F�N�g
 *
 *	@retval	CLSYS_DRAW_MAIN		���C����ʗp�T�[�t�F�[�X
 *	@retval	CLSYS_DRAW_SUB		�T�u��ʗp�T�[�t�F�[�X
 */
//-----------------------------------------------------------------------------
static CLSYS_DRAW_TYPE REND_SurfaceObjGetType( const NNSG2dRenderSurface* cp_surface )
{
	GF_ASSERT( cp_surface );
	if( cp_surface->type == NNS_G2D_SURFACETYPE_MAIN2D ){
		return CLSYS_DRAW_MAIN;
	}else if( cp_surface->type == NNS_G2D_SURFACETYPE_SUB2D ){
		return CLSYS_DRAW_SUB;
	}
	// �s���ȃT�[�t�F�[�X�^�C�v
	GF_ASSERT( 0 );
	return CLSYS_DRAW_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W����OAMAttr��o�^���鏈��
 *
 *	@param	p_obj			OAM�}�l�[�W���I�u�W�F�N�g
 *	@param	pOam			�o�^����OamAttr	
 *	@param	affineIndex		�A�t�B��Index
 *	@param	pltt_no			�㏑���p���b�g�i���o�[
 *
 *	@retval	TRUE	�o�^����
 *	@retval	FALSE	�o�^���s
 */
//-----------------------------------------------------------------------------
static BOOL REND_CallBackAddOam( OAMMAN_DATA* p_obj, const GXOamAttr* pOam, u16 affineIndex, u32 pltt_no )
{
	BOOL result;
	GXOamAttr oamattr;

	GF_ASSERT( pOam );

	oamattr = *pOam;

	// �p���b�g�i���o�[��OamAttr�ɏ㏑������
	if( (oamattr.cParam + pltt_no) <= 0xf ){
		oamattr.cParam += pltt_no;
	}else{
		// �\���p���b�gNo��15�ȏ�ɂȂ�܂����B
		// ���̃p���b�g�ʒu���ƐF���o�Ȃ��ł��B
		GF_ASSERT( 0 );
	}
	
	// OAM�}�l�[�W���ɓo�^
	result = OAMMAN_ObjEntryOamAttr( p_obj, &oamattr, affineIndex );

	// OAM�o�^���s�@
	// OAM�o�^�\����葽���`�悵�悤�Ƃ��܂����B
	GF_ASSERT( result );
	return result;

}
// ���C����ʗp
static BOOL REND_CallBackMainAddOam( const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine )
{
	GF_ASSERT( pClsys );

	return REND_CallBackAddOam( &pClsys->oamman.man[ CLSYS_DRAW_MAIN ], pOam, 
			affineIndex, 
			pClsys->pltt_no[ CLSYS_DRAW_MAIN ] );
}
// �T�u��ʗp
static BOOL REND_CallBackSubAddOam( const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine )
{
	GF_ASSERT( pClsys );
	return REND_CallBackAddOam( &pClsys->oamman.man[ CLSYS_DRAW_SUB ], pOam, 
			affineIndex,
			pClsys->pltt_no[ CLSYS_DRAW_SUB ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�}�l�[�W���ɃA�t�B���s���ݒ肷��
 *
 *	@param	mtx		�A�t�B���s��
 *
 *	@return	�A�t�B��Index
 */
//-----------------------------------------------------------------------------
// ���C����ʗp
static u16 REND_CallBackMainAddAffine( const MtxFx22* mtx )
{
	u16 result;
	GF_ASSERT( pClsys );
	result = OAMMAN_ObjEntryAffine( &pClsys->oamman.man[ CLSYS_DRAW_MAIN ], mtx );
	// �A�t�B���C���f�b�N�X�o�^���s
	GF_ASSERT( result != NNS_G2D_OAM_AFFINE_IDX_NONE );
	return result;
}
// �T�u��ʗp
static u16 REND_CallBackSubAddAffine( const MtxFx22* mtx )
{
	u16 result;
	GF_ASSERT( pClsys );
	result = OAMMAN_ObjEntryAffine( &pClsys->oamman.man[ CLSYS_DRAW_SUB ], mtx );
	// �A�t�B���C���f�b�N�X�o�^���s
	GF_ASSERT( result != NNS_G2D_OAM_AFFINE_IDX_NONE );
	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�̈���ɃZ���f�[�^�����邩���`�F�b�N���鏈��
 *
 *	@param	pCell			�\������Z���f�[�^
 *	@param	pMtx			�\���ʒu�s��
 *	@param	pViewRect		�T�[�t�F�[�X�̈�
 *
 *	@retval	TRUE	�\������
 *	@retval	FALSE	�\�����Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL REND_CallBackCulling( const NNSG2dCellData* pCell, const MtxFx32* pMtx, const NNSG2dViewRect* pViewRect )
{
	// �R���o�[�^�ɂ��v�Z���ꂽ���E�����a���擾����
	const NNSG2dCellBoundingRectS16* Rect = NNS_G2dGetCellBoundingRect( pCell );
    const fx32  R = NNS_G2dGetCellBoundingSphereR( pCell );
    // �Z���̕\���ʒu���_�����߂�
    const fx32  px = pMtx->_20 - pViewRect->posTopLeft.x;
    const fx32  py = pMtx->_21 - pViewRect->posTopLeft.y;
	// �Z����`
	fx32  minY; 
	fx32  maxY; 
	fx32  minX; 
	fx32  maxX; 
	fx32  work;

	// �Z���̋��E���������`�����߂�
	// �Z����`�ʒu�𒆐S�ʒu�ɂ����Ƃ�
	if( NNS_G2dCellHasBR( pCell ) == TRUE ){
		minY = Rect->minY << FX32_SHIFT;
		maxY = Rect->maxY << FX32_SHIFT;
		minX = Rect->minX << FX32_SHIFT;
		maxX = Rect->maxX << FX32_SHIFT;
	}else{
		minY = -R << FX32_SHIFT;
		maxY = R << FX32_SHIFT;
		minX = -R << FX32_SHIFT;
		maxX = R << FX32_SHIFT;
	}
	
	// ���W�v�Z�ōs��v�Z��̍��W�ɕϊ�
	minY = FX_Mul(minY, pMtx->_01) + FX_Mul(minY, pMtx->_11) + py;
	maxY = FX_Mul(maxY, pMtx->_01) + FX_Mul(maxY, pMtx->_11) + py;
	minX = FX_Mul(minX, pMtx->_00) + FX_Mul(minX, pMtx->_10) + px;
	maxX = FX_Mul(maxX, pMtx->_00) + FX_Mul(maxX, pMtx->_10) + px;

	// ���W���t�]���Ă���\��������̂Ń`�F�b�N
	if( maxY < minY ){
		work = maxY;
		maxY = minY;
		minY = work;
	}

	if( maxX < minX ){
		work = maxX;
		maxX = minX;
		minX = work;
	}

    if( (maxY > 0) && (minY < pViewRect->sizeView.y) ){
        if( (maxX > 0) && (minX < pViewRect->sizeView.x) ){
			return TRUE;
        }
    }
	
	return FALSE;
}


//-------------------------------------
///	TRMAN	�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���V�X�e���@������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *	@param	tr_cell		�Z���]���o�b�t�@��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void TRMAN_SysInit( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID )
{
	GF_ASSERT( p_trman );

	if( tr_cell == 0 ){
		p_trman->init = FALSE;
	}else{
		p_trman->data_num = tr_cell;
		TRMAN_CellTransManCreate( p_trman, tr_cell, heapID );
		TRMAN_TrSysCreate( p_trman, tr_cell, heapID );
		p_trman->init = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���V�X�e���@�j��
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_SysExit( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );
	if( p_trman->init ){
		TRMAN_CellTransManDelete( p_trman );
		TRMAN_TrSysDelete( p_trman );
		p_trman->init = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���V�X�e���@���C������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_SysMain( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );
	if( p_trman->init ){
		TRMAN_TrSysUpdata( p_trman );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���V�X�e���@V�u�����N����
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_SysVBlank( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );
	if( p_trman->init ){
		TRMAN_TrSysTrans( p_trman );
	}	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���V�X�e���@�o�b�t�@�����擾
 *
 *	@param	cp_trman	�Z���]���A�j���V�X�e��
 *
 *	@return	�o�b�t�@��
 */
//-----------------------------------------------------------------------------
static u8 TRMAN_SysGetBuffNum( const CLSYS_TRMAN* cp_trman )
{
	GF_ASSERT( cp_trman );
	return cp_trman->data_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���}�l�[�W������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *	@param	tr_cell		�o�b�t�@�쐬��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void TRMAN_CellTransManCreate( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID )
{
	GF_ASSERT( p_trman );

	p_trman->p_cell = GFL_HEAP_AllocMemory(heapID, sizeof(NNSG2dCellTransferState)*tr_cell);
	
	NNS_G2dInitCellTransferStateManager(
				p_trman->p_cell,
				tr_cell,
				TRMAN_TrSysCallBackAddTransData
			);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]���A�j���}�l�[�W���j��
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_CellTransManDelete( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );
	GFL_HEAP_FreeMemory( p_trman->p_cell );
	p_trman->p_cell = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���]����ԃI�u�W�F�N�g�n���h�����擾����
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *
 *	@return	�Z���]����ԃI�u�W�F�N�g�̃n���h��
 */
//-----------------------------------------------------------------------------
static u32 TRMAN_CellTransManAddHandle( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );	// CLSYS_TRMAN�������Ă���V�X�e�������Ă�ł͂����Ȃ�
	GF_ASSERT( p_trman->init );// CLSYS_TRMAN���쐬���Ă��Ȃ�	
	return NNS_G2dGetNewCellTransferStateHandle();
}

//----------------------------------------------------------------------------
/**
 	@brief	�Z���]����ԃI�u�W�F�N�g�n���h����j������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *	@param	handle		�n���h��
 */
//-----------------------------------------------------------------------------
static void TRMAN_CellTransManDelHandle( CLSYS_TRMAN* p_trman, u32 handle )
{
	GF_ASSERT( p_trman );	// CLSYS_TRMAN�������Ă���V�X�e�������Ă�ł͂����Ȃ�
	GF_ASSERT( p_trman->init );// CLSYS_TRMAN���쐬���Ă��Ȃ�
	NNS_G2dFreeCellTransferStateHandle( handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���V�X�e������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *	@param	tr_cell		�쐬����o�b�t�@��
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrSysCreate( CLSYS_TRMAN* p_trman, u8 tr_cell, HEAPID heapID )
{
	int i;

	GF_ASSERT( p_trman );
	p_trman->p_trdata = GFL_HEAP_AllocMemory( heapID, sizeof(TRMAN_DATA)*tr_cell );
	for( i=0; i<p_trman->data_num; i++ ){
		TRMAN_TrDataClean( &p_trman->p_trdata[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���V�X�e���j��
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrSysDelete( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );
	GFL_HEAP_FreeMemory( p_trman->p_trdata );
	p_trman->p_trdata = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���V�X�e��	�Z���A�j���X�V����
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrSysUpdata( CLSYS_TRMAN* p_trman )
{
	GF_ASSERT( p_trman );	// CLSYS_TRMAN�������Ă���V�X�e�������Ă�ł͂����Ȃ�
	GF_ASSERT( p_trman->init );// CLSYS_TRMAN���쐬���Ă��Ȃ�
	NNS_G2dUpdateCellTransferStateManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���V�X�e���@�]������
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrSysTrans( CLSYS_TRMAN* p_trman )
{
	int i;
	
	GF_ASSERT( p_trman );

	for( i=0; i<p_trman->data_num; i++ ){
		// �f�[�^�ݒ肳��Ă��邩�`�F�b�N
		if( TRMAN_TrDataCheckClean( &p_trman->p_trdata[i] ) == FALSE ){
			// �]�����s
			TRMAN_TrDataTrans( &p_trman->p_trdata[i] );
			TRMAN_TrDataClean( &p_trman->p_trdata[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���V�X�e���@�󂢂Ă���o�b�t�@���擾
 *
 *	@param	p_trman		�Z���]���A�j���V�X�e��
 *
 *	@return	�󂢂Ă���o�b�t�@�|�C���^
 */
//-----------------------------------------------------------------------------
static TRMAN_DATA* TRMAN_TrSysGetCleanBuff( CLSYS_TRMAN* p_trman )
{
	int i;

	GF_ASSERT( p_trman );

	for( i=0; i<p_trman->data_num; i++ ){
		// �f�[�^�ݒ肳��Ă��邩�`�F�b�N
		if( TRMAN_TrDataCheckClean( &p_trman->p_trdata[i] ) ){
			return &p_trman->p_trdata[i];
		}
	}
	// �����󂫂��Ȃ�
	// GF_CLACT_SysVBlank�֐����Ă�ł��Ȃ��\��������܂��B
	GF_ASSERT( 0 );
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���f�[�^���o�b�t�@�ɐݒ�
 *
 *	@param	type		�]���^�C�v
 *	@param	dstAddr		�]����A�h���X
 *	@param	pSrc		�]���f�[�^
 *	@param	szByte		�]���f�[�^�T�C�Y
 *
 *	@retval	TRUE	�]���f�[�^�ݒ萬��
 */
//-----------------------------------------------------------------------------
static BOOL TRMAN_TrSysCallBackAddTransData( NNS_GFD_DST_TYPE type, u32 dstAddr, void* pSrc, u32 szByte )
{
	TRMAN_DATA* p_data;
	CLSYS_DRAW_TYPE tr_type;
	
	GF_ASSERT( pClsys );

	// �󂢂Ă���o�b�t�@�擾
	p_data = TRMAN_TrSysGetCleanBuff( &pClsys->trman );

	if( type == NNS_GFD_DST_2D_OBJ_CHAR_MAIN ){
		tr_type = CLSYS_DRAW_MAIN;
	}else if( type == NNS_GFD_DST_2D_OBJ_CHAR_SUB ){
		tr_type = CLSYS_DRAW_SUB;
	}else{
		// �����ɗ��邱�Ƃ͂��肦�Ȃ�
		GF_ASSERT(0);
	}

	GF_ASSERT( pSrc );
	GF_ASSERT( szByte != 0 );
	
	TRMAN_TrDataAddData( p_data, tr_type, dstAddr, pSrc, szByte );
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���f�[�^�o�b�t�@��������
 *
 *	@param	p_data	���[�N
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrDataClean( TRMAN_DATA* p_data )
{
	GF_ASSERT( p_data );
	GFL_STD_MemFill( p_data, 0, sizeof(TRMAN_DATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���f�[�^��ݒ�
 *
 *	@param	p_data		�]���f�[�^�i�[��
 *	@param	type		�]���^�C�v
 *	@param	dstAddr		Vram�A�h���X
 *	@param	pSrc		�]���L�����N�^�f�[�^
 *	@param	szByte		�]���f�[�^�T�C�Y
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrDataAddData( TRMAN_DATA* p_data, CLSYS_DRAW_TYPE type, u32 dstAddr, void* pSrc, u32 szByte )
{
	GF_ASSERT( p_data );
	p_data->type	= type;
	p_data->addr	= dstAddr;
	p_data->p_src	= pSrc;
	p_data->size	= szByte;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^��Vram�ɓ]��
 *
 *	@param	p_data	�]���f�[�^
 */
//-----------------------------------------------------------------------------
static void TRMAN_TrDataTrans( TRMAN_DATA* p_data )
{
	static void (* const load_func[ CLSYS_DRAW_MAX ])(const void*, u32, u32) = {
		GX_LoadOBJ,
		GXS_LoadOBJ,
	};
	GF_ASSERT( p_data );
	DC_FlushRange( p_data->p_src, p_data->size );
	load_func[ p_data->type ]( p_data->p_src, p_data->addr, p_data->size );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���f�[�^�o�b�t�@�Ƀf�[�^�������Ă��Ȃ����`�F�b�N
 *
 *	@param	cp_data		�`�F�b�N����o�b�t�@
 *
 *	@retval	TRUE	�󂢂Ă���
 *	@retval	FALSE	�󂢂Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TRMAN_TrDataCheckClean( const TRMAN_DATA* cp_data )
{
	GF_ASSERT( cp_data );
	if( cp_data->p_src == NULL ){
		return TRUE;
	}
	return FALSE;
}


//-------------------------------------
///	CLUNIT	�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g������CLWK���擾����
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	
 *	@return	���CLWK�I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------
static CLWK* CLUNIT_SysGetCleanWk( CLUNIT* p_unit )
{
	int i;

	GF_ASSERT( p_unit );

	for( i=0; i<p_unit->wk_num; i++ ){
		if( p_unit->p_wk[i].p_next == NULL ){
			return &p_unit->p_wk[i];
		}
	}
	GF_ASSERT( 0 );	// �o�^���I�[�o�[
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`�惊�X�g�ɐV�K�o�^
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	p_wk		�o�^����Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
static void CLUNIT_DrawListAdd( CLUNIT* p_unit, CLWK* p_wk )
{
	u8 top_pri;		// �擪�̗D�揇��
	u8 bottom_pri;	// �ŏI�̗D�揇��
	u8 ave_pri;		// �D�揇�ʂ̕���
	u8 wk_pri;		// �o�^����D�揇��
	CLWK* p_last;	// �o�^����O���[�N

	GF_ASSERT( p_unit );
	GF_ASSERT( p_wk );
	
	// �擪�`�F�b�N
	if( p_unit->p_drawlist == NULL ){
		p_unit->p_drawlist = p_wk;
		p_unit->p_drawlist->p_next = p_wk;
		p_unit->p_drawlist->p_last = p_wk;
		return ;
	}

	// �o�^����D�揇�ʂ��擾
	wk_pri = GFL_CLACT_WkGetSoftPri( p_wk );

	// �擪�ɓo�^���邩�`�F�b�N
	top_pri = GFL_CLACT_WkGetSoftPri( p_unit->p_drawlist );
	if( wk_pri < top_pri ){
		// �擪�ɓo�^
		p_last = p_unit->p_drawlist->p_last;
		p_unit->p_drawlist = p_wk;

	}else{

		// �擪�ƍŏI�v�f�̗D�揇�ʂ��擾
		bottom_pri = GFL_CLACT_WkGetSoftPri( p_unit->p_drawlist->p_last );
		ave_pri = (top_pri + bottom_pri)/2;	// ���ς����߂�


		if( ave_pri >= wk_pri ){
			// �擪���猟��
			p_last = CLUNIT_DrawListSarchTop( p_unit->p_drawlist, wk_pri );
		}else{
			// ������猟��
			p_last = CLUNIT_DrawListSarchBottom( p_unit->p_drawlist->p_last, wk_pri );
		}
	}

	// p_last�̎��ɓo�^
	p_wk->p_last			= p_last;
	p_wk->p_next			= p_last->p_next;
	p_last->p_next->p_last	= p_wk;
	p_last->p_next			= p_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`�惊�X�g���烏�[�N���͂���
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	p_wk		�͂������[�N
 */
//-----------------------------------------------------------------------------
static void CLUNIT_DrawListDel( CLUNIT* p_unit, CLWK* p_wk )
{
	GF_ASSERT( p_unit );
	GF_ASSERT( p_wk );

	// �������擪���`�F�b�N
	if( p_unit->p_drawlist == p_wk ){
		// �����̎���������������I���
		if( p_wk->p_next == p_wk ){
			p_unit->p_drawlist = NULL;
		}else{
			// �擪�������̎��̂�ɏ���
			p_unit->p_drawlist = p_wk->p_next;
		}
	}
	// �͂���
	p_wk->p_next->p_last = p_wk->p_last;
	p_wk->p_last->p_next = p_wk->p_next;
	// �����̃��X�g�|�C���^���N���A
	p_wk->p_next = NULL;
	p_wk->p_last = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`�惊�X�g�擪����o�^����O���[�N���擾
 *
 *	@param	p_top		�`�惊�X�g�擪�f�[�^
 *	@param	pri			�D�揇��
 *
 *	@return	�o�^����O���[�N
 */
//-----------------------------------------------------------------------------
static CLWK* CLUNIT_DrawListSarchTop( CLWK* p_top, u8 pri )
{
	CLWK* p_wk = p_top;
	u8 ck_pri;

	GF_ASSERT( p_top );
	
	do{
		ck_pri = GFL_CLACT_WkGetSoftPri( p_wk );
		if( ck_pri > pri ){
			return p_wk->p_last;	// �D�揇�ʂ��傫�����[�N�̑O�̃��[�N�̎��ɒǉ�����
		}
		// ����
		p_wk = p_wk->p_next;
	} while( p_wk != p_top );

	// �Ō���ɓo�^���Ă��炤
	return p_top->p_last;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`�惊�X�g��납��o�^����O���[�N���擾
 *
 *	@param	p_top		�`�惊�X�g�Ō���f�[�^
 *	@param	pri			�D�揇��
 *
 *	@return	�o�^����O���[�N
 */
//-----------------------------------------------------------------------------
static CLWK* CLUNIT_DrawListSarchBottom( CLWK* p_bottom, u8 pri )
{
	CLWK* p_wk = p_bottom;
	u8 ck_pri;

	GF_ASSERT( p_bottom );

	do{
		ck_pri = GFL_CLACT_WkGetSoftPri( p_wk );
		if( ck_pri <= pri ){
			return p_wk;	// �D�揇�ʂ������������������[�N�̎��ɒǉ�����
		}
		// �O��
		p_wk = p_wk->p_last;
	} while( p_wk != p_bottom );

	// �擪�ɓo�^���Ă��炤
	return p_bottom->p_next;
}


//-------------------------------------
///	CLWK	�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�̃N���A
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
static void CLWK_SysClean( CLWK* p_wk )
{
	GF_ASSERT( p_wk );
	GFL_STD_MemFill( p_wk, 0, sizeof(CLWK) );
	p_wk->auto_anm_speed = CLWK_AUTOANM_DEFF_SPEED;
	NNS_G2dInitImageProxy( &p_wk->img_proxy );
	NNS_G2dInitImagePaletteProxy( &p_wk->pltt_proxy );
	p_wk->over_write |= NNS_G2D_RND_OVERWRITE_PRIORITY;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�`�揈��
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	p_rend		�����_���[�V�X�e��
 */
//-----------------------------------------------------------------------------
static void CLWK_SysDraw( CLWK* p_wk, CLSYS_REND* p_rend )
{
	GF_ASSERT( p_wk );

	// ��\��
	if( p_wk->draw_flag == 0 ){
		return;
	}
	
	// �����_���[�`��O�ݒ�
	REND_SysBeginDraw( p_rend, &p_wk->img_proxy, &p_wk->pltt_proxy );

	// �����_���[�ɕ`��p�����[�^��ݒ肷��
	CLWK_SysDrawSetRend( p_wk, p_rend );

	// �p���b�g�i���o�[�ݒ�
	CLSYS_SysSetPaletteProxy( &p_wk->pltt_proxy, p_wk->pal_offs );
	
	// �`�揈��
	NNS_G2dPushMtx();
	{
		CLWK_SysDrawCell( p_wk );
	}
	NNS_G2dPopMtx();

	// �����_���[�`��I��
	REND_SysEndDraw( p_rend );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���[�ɃZ���A�N�^�[���[�N
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	p_rend		�����_���[�V�X�e��
 */
//-----------------------------------------------------------------------------
static void CLWK_SysDrawSetRend( CLWK* p_wk, CLSYS_REND* p_rend )
{
	GF_ASSERT( p_wk );
	// �A�t�B���ݒ�
	REND_SysSetAffine( p_rend, p_wk->affine );
	if( p_wk->affine == NNS_G2D_RND_AFFINE_OVERWRITE_NONE ){
		BOOL vflip, hflip;
		vflip = GFL_CLACT_WkGetFlip( p_wk, CLWK_FLIP_V );
		hflip = GFL_CLACT_WkGetFlip( p_wk, CLWK_FLIP_H );
		REND_SysSetFlip( p_rend, vflip, hflip );
	}
	// �I�o�[���C�g�t���O�ݒ�
	REND_SysSetOverwrite( p_rend, p_wk->over_write );
	// �I�o�[���C�g�p�����[�^�ݒ�
	REND_SysSetMosaicFlag( p_rend, p_wk->mosaic );
	REND_SysSetOBJMode( p_rend, p_wk->objmode );
	REND_SysSetBGPriority( p_rend, p_wk->bg_pri );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���f�[�^�̕`��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void CLWK_SysDrawCell( CLWK* p_wk )
{
	GF_ASSERT( p_wk );

	// ���W�ړ�
	NNS_G2dTranslate( FX32_CONST(p_wk->pos.x), FX32_CONST(p_wk->pos.y), 0 );
	
	if(p_wk->affine != NNS_G2D_RND_AFFINE_OVERWRITE_NONE){
		NNS_G2dTranslate( FX32_CONST(p_wk->affinepos.x), FX32_CONST(p_wk->affinepos.y), 0 );
		NNS_G2dScale( p_wk->scale.x, p_wk->scale.y, FX32_ONE );
		NNS_G2dRotZ( FX_SinIdx(p_wk->rotation), FX_CosIdx(p_wk->rotation) );
		NNS_G2dTranslate( -FX32_CONST(p_wk->affinepos.x), -FX32_CONST(p_wk->affinepos.y), 0 );
	}

	// ���ꂼ��̕��@�ŕ`��
	switch( p_wk->anmdata.type ){
	case CLWK_ANM_CELL:
		NNS_G2dDrawCellAnimation( &p_wk->anmdata.data.cell.anmctrl );
		break;
	case CLWK_ANM_TRCELL:
		NNS_G2dDrawCellAnimation( &p_wk->anmdata.data.trcell.anmctrl );
		break;
	case CLWK_ANM_MULTICEL:
		NNS_G2dDrawMultiCellAnimation( &p_wk->anmdata.data.multicell.anmctrl );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�I�[�g�A�j���[�V�������s��
 *
 *	@param	p_wk	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
static void CLWK_SysAutoAnm( CLWK* p_wk )
{
	if( p_wk->auto_anm == TRUE ){
		GFL_CLACT_WkAddAnmFrame( p_wk, p_wk->auto_anm_speed );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�ɏ������f�[�^��ݒ肷��
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	cp_data		�������f�[�^
 *	@param	setsf		�ݒ�T�[�t�F�[�X
 */
//-----------------------------------------------------------------------------
static void CLWK_SysSetClwkData( CLWK* p_wk, const CLWK_DATA* cp_data, u16 setsf )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_data );
	
	GFL_CLACT_WkSetTypePos( p_wk, cp_data->pos_x, setsf, CLSYS_MAT_X );
	GFL_CLACT_WkSetTypePos( p_wk, cp_data->pos_y, setsf, CLSYS_MAT_Y );
	p_wk->soft_pri = cp_data->softpri;
	GFL_CLACT_WkSetBgPri( p_wk, cp_data->bgpri );
	GFL_CLACT_WkSetAnmSeq( p_wk, cp_data->anmseq );
	GFL_CLACT_WkStartAnm( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�f�[�^�����[�N�ɓo�^
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	cp_res		���\�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_SysSetClwkRes( CLWK* p_wk, const CLWK_RES* cp_res )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_res );
	p_wk->img_proxy = *cp_res->cp_img;
	p_wk->pltt_proxy = *cp_res->cp_pltt;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ݒ�T�[�t�F�[�X�̍��W���擾����
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	setsf		�ݒ�T�[�t�F�[�X
 *	@param	p_pos		�ݒ�T�[�t�F�[�X���W�i�[��
 */
//-----------------------------------------------------------------------------
static void CLWK_SysGetSetSfPos( const CLWK* cp_wk, u16 setsf, CLSYS_POS* p_pos )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( p_pos );
	// �T�[�t�F�[�X���W���擾
	if( setsf != CLWK_SETSF_NONE ){
		GFL_CLACT_UserRendGetSurfacePos( cp_wk->p_unit->p_rend, setsf, p_pos );
	}else{
		p_pos->x = 0;
		p_pos->y = 0;
	}
}


//-------------------------------------
///	CLWK_ANMDATA	�֌W
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief		�A�j���[�V�����f�[�^������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	cp_res			���\�[�X���[�N
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataInit( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID )
{
	static void (* const p_create[ CLWK_ANM_MAX ])( CLWK_ANMDATA* , const CLWK_RES* ,  HEAPID ) = {
		CLWK_AnmDataCreateCellData,
		CLWK_AnmDataCreateTRCellData,
		CLWK_AnmDataCreateMCellData
	};

	GF_ASSERT( p_anmdata );
	
	// �A�j���[�V�����^�C�v���擾
	p_anmdata->type = CLWK_AnmDataGetType( cp_res );

	// ���ꂼ��̏�������
	p_create[ p_anmdata->type ]( p_anmdata, cp_res, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�̔j������
 *
 *	@param	p_anmdata	�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataExit( CLWK_ANMDATA* p_anmdata )
{
	static void (* const p_delete[ CLWK_ANM_MAX ])( CLWK_ANMDATA*  ) = {
		CLWK_AnmDataDeleteCellData,
		CLWK_AnmDataDeleteTRCellData,
		CLWK_AnmDataDeleteMCellData,
	};

	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );
	
	p_delete[ p_anmdata->type ]( p_anmdata );
	GFL_STD_MemFill( p_anmdata, 0, sizeof(CLWK_ANMDATA) );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�f�[�^����A�j���[�V�����^�C�v���擾����
 *
 *	@param	cp_res	���\�[�X�f�[�^
 *
 *	@return	�A�j���[�V�����^�C�v
 */
//-----------------------------------------------------------------------------
static CLWK_ANMTYPE CLWK_AnmDataGetType( const CLWK_RES* cp_res )
{
	GF_ASSERT( cp_res );

	// Vram�]���Z���A�j��
	if( cp_res->cp_char != NULL ){
		return CLWK_ANM_TRCELL;
	}
	// �}���`�Z��
	if( cp_res->cp_mcell != NULL ){
		return CLWK_ANM_MULTICEL;
	}
	return CLWK_ANM_CELL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����f�[�^�@������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	cp_res			���\�[�X�f�[�^
 *	@param	heapID
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataCreateCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID )
{
	GF_ASSERT( p_anmdata );
	GF_ASSERT( cp_res );

	p_anmdata->data.cell.cp_cell	= cp_res->p_cell;
	p_anmdata->data.cell.cp_canm	= cp_res->cp_canm;

	// �A�j���[�V�����R���g���[��������
	NNS_G2dInitCellAnimation( 
			&p_anmdata->data.cell.anmctrl,
			NNS_G2dGetAnimSequenceByIdx(p_anmdata->data.cell.cp_canm, 0),
			p_anmdata->data.cell.cp_cell
			);
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	cp_res			���\�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataCreateTRCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID )
{
	GF_ASSERT( pClsys );
	GF_ASSERT( p_anmdata );
	GF_ASSERT( cp_res );

	p_anmdata->data.trcell.p_cell	= cp_res->p_cell;
	p_anmdata->data.trcell.cp_canm	= cp_res->cp_canm;
	
	// Vram�]���Z���Ǘ��n���h��
	p_anmdata->data.trcell.trhandle = TRMAN_CellTransManAddHandle( &pClsys->trman ); 

	// �Z��Vram�]���A�j���̏�����
	NNS_G2dInitCellAnimationVramTransfered(
                &p_anmdata->data.trcell.anmctrl,
                NNS_G2dGetAnimSequenceByIdx(p_anmdata->data.trcell.cp_canm, 0),
                p_anmdata->data.trcell.p_cell,

                p_anmdata->data.trcell.trhandle,
                NNS_G2D_VRAM_ADDR_NONE ,
                NNS_G2dGetImageLocation(cp_res->cp_img, NNS_G2D_VRAM_TYPE_2DMAIN),
                NNS_G2dGetImageLocation(cp_res->cp_img, NNS_G2D_VRAM_TYPE_2DSUB),
                cp_res->cp_char->pRawData, 
                NULL,
                cp_res->cp_char->szByte 
            );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V������������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	cp_res			���\�[�X�f�[�^
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataCreateMCellData( CLWK_ANMDATA* p_anmdata, const CLWK_RES* cp_res, HEAPID heapID )
{
	u32 wk_size;

	GF_ASSERT( p_anmdata );
	GF_ASSERT( cp_res );
	
	p_anmdata->data.multicell.cp_cell	= cp_res->p_cell;
	p_anmdata->data.multicell.cp_canm	= cp_res->cp_canm;
	p_anmdata->data.multicell.cp_mcell	= cp_res->cp_mcell;
	p_anmdata->data.multicell.cp_mcanm	= cp_res->cp_mcanm;
	
	// ���[�N�쐬
	wk_size =  NNS_G2dGetMCWorkAreaSize( p_anmdata->data.multicell.cp_mcell, NNS_G2D_MCTYPE_SHARE_CELLANIM );
	p_anmdata->data.multicell.p_wk = GFL_HEAP_AllocMemory( heapID, wk_size );

	// �}���`�Z���A�j���[�V����������
	NNS_G2dInitMCAnimationInstance( 
		&p_anmdata->data.multicell.anmctrl,  
		p_anmdata->data.multicell.p_wk, 
		p_anmdata->data.multicell.cp_canm,  
		p_anmdata->data.multicell.cp_cell,
		p_anmdata->data.multicell.cp_mcell,
		NNS_G2D_MCTYPE_SHARE_CELLANIM
	);

	// �A�j���[�V�����V�[�P���X�ݒ�
	NNS_G2dSetAnimSequenceToMCAnimation( &p_anmdata->data.multicell.anmctrl, 
			NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.multicell.cp_mcanm, 0 ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����@�j������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataDeleteCellData( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );
	// �������Ȃ��ėǂ�
	GFL_STD_MemFill( p_anmdata, 0, sizeof(CLWK_ANMDATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@�j������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataDeleteTRCellData( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( pClsys );
	GF_ASSERT( p_anmdata );

	// Vram�]���Ǘ��n���h�����
	TRMAN_CellTransManDelHandle( &pClsys->trman, p_anmdata->data.trcell.trhandle );
	GFL_STD_MemFill( p_anmdata, 0, sizeof(CLWK_ANMDATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����@�j������
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataDeleteMCellData( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );

	// ���[�N�j��
	GFL_HEAP_FreeMemory( p_anmdata->data.multicell.p_wk );
	GFL_STD_MemFill( p_anmdata, 0, sizeof(CLWK_ANMDATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�̃A�j���[�V�����V�[�P���X��ύX
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	anmseq			�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeq( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	static void (* const p_SetAnmSeq[ CLWK_ANM_MAX ])( CLWK_ANMDATA*, u16 ) = {
		CLWK_AnmDataSetAnmSeqCell,
		CLWK_AnmDataSetAnmSeqTRCell,
		CLWK_AnmDataSetAnmSeqMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );
	p_SetAnmSeq[ p_anmdata->type ]( p_anmdata, anmseq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����@�A�j���[�V�����V�[�P���X�ύX
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	anmseq			�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	const NNSG2dAnimSequence* p_seq;

	GF_ASSERT( p_anmdata );
	
	// �V�[�P���X���擾
	p_seq = NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.cell.cp_canm, anmseq );

	// �V�[�P���X��K�p
	NNS_G2dSetCellAnimationSequence( &p_anmdata->data.cell.anmctrl, p_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@�A�j���[�V�����V�[�P���X�ύX
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	const NNSG2dAnimSequence* p_seq;

	GF_ASSERT( p_anmdata );
	
	// �V�[�P���X���擾
	p_seq = NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.trcell.cp_canm, anmseq );

	// �V�[�P���X��K�p
	NNS_G2dSetCellAnimationSequence( &p_anmdata->data.trcell.anmctrl, p_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����@�A�j���[�V�����V�[�P���X�ύX
 *	
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	const NNSG2dMultiCellAnimSequence* p_seq;

	GF_ASSERT( p_anmdata );

	p_seq = NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.multicell.cp_mcanm, anmseq );
	NNS_G2dSetAnimSequenceToMCAnimation( 
			&p_anmdata->data.multicell.anmctrl, 
			p_seq );
}

#if 0		// �g�p�p�r���킩��Ȃ��̂Ŕr��
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX����@
 *			���̍ۃt���[�����ԂȂǂ̃��Z�b�g���s���܂���B�@MultiCell��Ή�
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	anmseq			�A�j���[�V�����V�[�P���X
 */	
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqNoReset( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	static void (* const p_SetAnmSeq[ CLWK_ANM_MAX ])( CLWK_ANMDATA*, u16 ) = {
		CLWK_AnmDataSetAnmSeqNoResetCell,
		CLWK_AnmDataSetAnmSeqNoResetTRCell,
		CLWK_AnmDataSetAnmSeqNoResetMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );

	p_SetAnmSeq[ p_anmdata->type ]( p_anmdata, anmseq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����@���Z�b�g���s��Ȃ��V�[�P���X�ύX����
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 *	@param	anmseq			�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqNoResetCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	const NNSG2dAnimSequence* cp_seq;

	GF_ASSERT( p_anmdata );
	
	// �V�[�P���X���擾
	cp_seq = NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.cell.cp_canm, anmseq );

	// �V�[�P���X��K�p
	NNS_G2dSetCellAnimationSequenceNoReset( &p_anmdata->data.cell.anmctrl, cp_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@���Z�b�g���s��Ȃ��V�[�P���X�ύX����
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqNoResetTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	const NNSG2dAnimSequence* cp_seq;

	GF_ASSERT( p_anmdata );
	
	// �V�[�P���X���擾
	cp_seq = NNS_G2dGetAnimSequenceByIdx( p_anmdata->data.trcell.cp_canm, anmseq );

	// �V�[�P���X��K�p
	NNS_G2dSetCellAnimationSequenceNoReset( &p_anmdata->data.trcell.anmctrl, cp_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V����	��Ή�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSeqNoResetMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	GF_ASSERT( p_anmdata );
	// �}���`�Z���A�j���[�V�����ɂ͔�Ή��ł�
	GF_ASSERT( 0 );	
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�̃A�j���[�V�����t���[������ݒ�
 *	
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	idx					�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmFrame( CLWK_ANMDATA* p_anmdata, u16 idx )
{
	static void (* const p_SetFrame[ CLWK_ANM_MAX ])( CLWK_ANMDATA*, u16 ) = {
		CLWK_AnmDataSetAnmFrameCell,
		CLWK_AnmDataSetAnmFrameTRCell,
		CLWK_AnmDataSetAnmFrameMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );

	p_SetFrame[ p_anmdata->type ]( p_anmdata, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����@�A�j���[�V�����t���[�����ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmFrameCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetCellAnimationCurrentFrame( &p_anmdata->data.cell.anmctrl, anmseq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@�A�j���[�V�����t���[�����ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmFrameTRCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetCellAnimationCurrentFrame( &p_anmdata->data.trcell.anmctrl, anmseq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����@�A�j���[�V�����t���[�����ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	anmseq				�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmFrameMCell( CLWK_ANMDATA* p_anmdata, u16 anmseq )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetMCAnimationCurrentFrame( &p_anmdata->data.multicell.anmctrl, anmseq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�@�A�j���[�V�����t���[����i�߂�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataAddAnmFrame( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	static void (* const p_add[ CLWK_ANM_MAX ])( CLWK_ANMDATA*, fx32 ) = {
		CLWK_AnmDataAddAnmFrameCell,
		CLWK_AnmDataAddAnmFrameTRCell,
		CLWK_AnmDataAddAnmFrameMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );

	p_add[ p_anmdata->type ]( p_anmdata, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����@�A�j���[�V������i�߂�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataAddAnmFrameCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dTickCellAnimation( &p_anmdata->data.cell.anmctrl, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����@�A�j���[�V������i�߂�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataAddAnmFrameTRCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dTickCellAnimation( &p_anmdata->data.trcell.anmctrl, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����@�A�j���[�V������i�߂�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataAddAnmFrameMCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dTickMCAnimation( &p_anmdata->data.multicell.anmctrl, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^����A�j���[�V�����R���g���[�����擾����
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrl( CLWK_ANMDATA* p_anmdata )
{
	static NNSG2dAnimController* (* const p_get[ CLWK_ANM_MAX ])( CLWK_ANMDATA* ) = {
		CLWK_AnmDataGetAnmCtrlCell,
		CLWK_AnmDataGetAnmCtrlTRCell,
		CLWK_AnmDataGetAnmCtrlMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );
		
	return p_get[ p_anmdata->type ]( p_anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlCell( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );
	return NNS_G2dGetCellAnimationAnimCtrl( &p_anmdata->data.cell.anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlTRCell( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );
	return NNS_G2dGetCellAnimationAnimCtrl( &p_anmdata->data.trcell.anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static NNSG2dAnimController* CLWK_AnmDataGetAnmCtrlMCell( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );
	return NNS_G2dGetMCAnimAnimCtrl( &p_anmdata->data.multicell.anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^����A�j���[�V�����R���g���[�����擾����
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrl( const CLWK_ANMDATA* cp_anmdata )
{
	static const NNSG2dAnimController* (* const p_get[ CLWK_ANM_MAX ])( const CLWK_ANMDATA* ) = {
		CLWK_AnmDataGetCAnmCtrlCell,
		CLWK_AnmDataGetCAnmCtrlTRCell,
		CLWK_AnmDataGetCAnmCtrlMCell,
	};
	GF_ASSERT( cp_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > cp_anmdata->type );

	return p_get[ cp_anmdata->type ]( cp_anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	cp_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlCell( const CLWK_ANMDATA* cp_anmdata )
{
	GF_ASSERT( cp_anmdata );
	return &cp_anmdata->data.cell.anmctrl.animCtrl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	cp_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlTRCell( const CLWK_ANMDATA* cp_anmdata )
{
	GF_ASSERT( cp_anmdata );
	return &cp_anmdata->data.trcell.anmctrl.animCtrl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V��������A�j���[�V�����R���g���[�����擾
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����R���g���[��
 */
//-----------------------------------------------------------------------------
static const NNSG2dAnimController* CLWK_AnmDataGetCAnmCtrlMCell( const CLWK_ANMDATA* cp_anmdata )
{
	GF_ASSERT( cp_anmdata );
	return &cp_anmdata->data.multicell.anmctrl.animCtrl;
}
		

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�̃A�j���[�V�����t���[�������擾
 *
 *	@param	cp_anmdata			�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
static u16 CLWK_AnmDataGetAnmFrame( const CLWK_ANMDATA* cp_anmdata )
{
	const NNSG2dAnimController* cp_anmctrl = CLWK_AnmDataGetCAnmCtrl( cp_anmdata );
	GF_ASSERT( cp_anmdata );
	return NNS_G2dGetAnimCtrlCurrentFrame( cp_anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�ɃI�[�g�A�j���[�V�����X�s�[�h��ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSpeed( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	static void (* p_set[ CLWK_ANM_MAX ])( CLWK_ANMDATA* , fx32 )={
		CLWK_AnmDataSetAnmSpeedCell,
		CLWK_AnmDataSetAnmSpeedTRCell,
		CLWK_AnmDataSetAnmSpeedMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );

	p_set[ p_anmdata->type ]( p_anmdata, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V�����f�[�^�ɃI�[�g�A�j���[�V�����X�s�[�h�ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSpeedCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetCellAnimationSpeed( &p_anmdata->data.cell.anmctrl, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vram�]���Z���A�j���[�V�����f�[�^�ɃI�[�g�A�j���[�V�����X�s�[�h�ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSpeedTRCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetCellAnimationSpeed( &p_anmdata->data.trcell.anmctrl, speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V�����f�[�^�ɃI�[�g�A�j���[�V�����X�s�[�h�ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	speed				�X�s�[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmSpeedMCell( CLWK_ANMDATA* p_anmdata, fx32 speed )
{
	GF_ASSERT( p_anmdata );
	NNS_G2dSetMCAnimationSpeed( &p_anmdata->data.multicell.anmctrl, speed );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����������Ă��邩�`�F�b�N����
 *
 *	@param	cp_anmdata			�A�j���[�V�����f�[�^
 *
 *	@retval	TRUE	�Đ���
 *	@retval	FALSE	��~��
 */
//-----------------------------------------------------------------------------
static BOOL CLWK_AnmDataCheckAnmActive( const CLWK_ANMDATA* cp_anmdata )
{
	const NNSG2dAnimController* cp_anmctrl = CLWK_AnmDataGetCAnmCtrl( cp_anmdata );
	GF_ASSERT( cp_anmdata );
	return NNS_G2dIsAnimCtrlActive( cp_anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^	�A�j���[�V�������Z�b�g
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataResetAnm( CLWK_ANMDATA* p_anmdata )
{
	static void (* p_reset[ CLWK_ANM_MAX ])( CLWK_ANMDATA* )={
		CLWK_AnmDataResetAnmCell,
		CLWK_AnmDataResetAnmCell,
		CLWK_AnmDataResetAnmMCell,
	};
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLWK_ANM_MAX > p_anmdata->type );

	p_reset[p_anmdata->type]( p_anmdata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���[�V����		�A�j���[�V�������Z�b�g
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataResetAnmCell( CLWK_ANMDATA* p_anmdata )
{
	NNSG2dAnimController* p_anmctrl = CLWK_AnmDataGetAnmCtrl( p_anmdata );
	GF_ASSERT( p_anmdata );
	NNS_G2dResetAnimCtrlState( p_anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�Z���A�j���[�V����	�A�j���[�V�������Z�b�g
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataResetAnmMCell( CLWK_ANMDATA* p_anmdata )
{
	GF_ASSERT( p_anmdata );
	CLWK_AnmDataResetAnmCell( p_anmdata );
	NNS_G2dResetMCCellAnimationAll( &p_anmdata->data.multicell.anmctrl.multiCellInstance );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^�ɃA�j���[�V�����v���C���[�h��ݒ�
 *
 *	@param	p_anmdata			�A�j���[�V�����f�[�^
 *	@param	playmode			�A�j���[�V�����v���C���[�h
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataSetAnmMode( CLWK_ANMDATA* p_anmdata, CLSYS_ANM_PLAYMODE playmode )
{
	NNSG2dAnimController* p_anmctrl = CLWK_AnmDataGetAnmCtrl( p_anmdata );
	
	GF_ASSERT( p_anmdata );
	GF_ASSERT( CLSYS_ANMPM_MAX > playmode );
	
	if( playmode == CLSYS_ANMPM_INVALID ){
		// �����f�[�^�̎����ɖ߂�
		p_anmctrl->bReverse = FALSE;		// ���̊֐��̒��Ń��o�[�X�t���O�����������Ȃ��̂ŁA�����ŏ���������
		NNS_G2dResetAnimCtrlPlayModeOverridden( p_anmctrl );
	}else{	
		NNS_G2dSetAnimCtrlPlayModeOverridden( p_anmctrl, playmode );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^	�A�j���[�V�����̍Đ�
 *	
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataStartAnm( CLWK_ANMDATA* p_anmdata )
{
	NNSG2dAnimController* p_anmctrl = CLWK_AnmDataGetAnmCtrl( p_anmdata );
	NNS_G2dStartAnimCtrl( p_anmctrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����f�[�^	�A�j���[�V�����̒�~
 *
 *	@param	p_anmdata		�A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------
static void CLWK_AnmDataStopAnm( CLWK_ANMDATA* p_anmdata )
{
	NNSG2dAnimController* p_anmctrl = CLWK_AnmDataGetAnmCtrl( p_anmdata );
	NNS_G2dStopAnimCtrl( p_anmctrl );
}

