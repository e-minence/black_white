//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		clact.h
 *	@brief		�Z���A�N�^�[�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2006.11.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __CLACT_H__
#define __CLACT_H__

#undef GLOBAL
#ifdef	__CLACT_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	CLSYS�`��ʃ^�C�v
//	�����_���[�̃T�[�t�F�[�X�ݒ��A
//	Vram�A�h���X�w��Ɏg�p���܂��B
//=====================================
typedef enum {
	CLSYS_DRAW_MAIN,// ���C�����
	CLSYS_DRAW_SUB,	// �T�u���
	CLSYS_DRAW_MAX,	// �^�C�v�ő吔
} CLSYS_DRAW_TYPE;


//-------------------------------------
///	NNSG2dRendererAffineTypeOverwiteMode���̌^���͒����̂ŕύX
//	�Z���A�g���r���[�g�̃A�t�B���ϊ��t���O���㏑������t���O
//=====================================
typedef NNSG2dRendererAffineTypeOverwiteMode	CLSYS_AFFINETYPE;
enum{
	CLSYS_AFFINETYPE_NONE	= NNS_G2D_RND_AFFINE_OVERWRITE_NONE,  // �㏑�����Ȃ�
	CLSYS_AFFINETYPE_NORMAL = NNS_G2D_RND_AFFINE_OVERWRITE_NORMAL,// �ʏ�̃A�t�B���ϊ������ɐݒ�
	CLSYS_AFFINETYPE_DOUBLE = NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE,// �{�p�A�t�B���ϊ������ɐݒ�
	CLSYS_AFFINETYPE_NUM
};

//-------------------------------------
///	NNSG2dAnimationPlayMode	���̌^���͒����̂ŕύX
//	�Z���A�j���[�V�����̃A�j���������㏑�w�肷��t���O
//=====================================
typedef NNSG2dAnimationPlayMode		CLSYS_ANM_PLAYMODE;
enum{
	CLSYS_ANMPM_INVALID		= NNS_G2D_ANIMATIONPLAYMODE_INVALID,		// ����
	CLSYS_ANMPM_FORWARD		= NNS_G2D_ANIMATIONPLAYMODE_FORWARD,        // �����^�C���Đ�(������)
	CLSYS_ANMPM_FORWARD_L	= NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP,   // ���s�[�g�Đ�(���������[�v)
	CLSYS_ANMPM_REVERSE		= NNS_G2D_ANIMATIONPLAYMODE_REVERSE,        // �����Đ�(���o�[�X�i���{�t�����j
	CLSYS_ANMPM_REVERSE_L	= NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP,   // �����Đ����s�[�g�i���o�[�X�i���{�t�������j ���[�v�j
	CLSYS_ANMPM_MAX
};


//-------------------------------------
///	�t���b�v�^�C�v
//	CLWK�ւ̃t���b�v�ݒ�Ɏg�p���܂��B
//=====================================
typedef enum {
	CLWK_FLIP_V,		// V�t���b�v���
	CLWK_FLIP_H,		// H�t���b�v���
	CLWK_FLIP_MAX
} CLWK_FLIP_TYPE;

//-------------------------------------
///	���W�^�C�v
//	���W�̌ʐݒ���s���Ƃ��Ɏg�p���܂��B
//=====================================
typedef enum{
	CLSYS_MAT_X,			// X���W
	CLSYS_MAT_Y,			// Y���W
	CLSYS_MAT_MAX
} CLSYS_MAT_TYPE;

//-------------------------------------
///	�ݒ�T�[�t�F�[�X�@��΍��W�w��
//	CLWK�̍��W�ݒ�֐��Ő�΍��W��ݒ肷��Ƃ��Ɏg�p���܂��B
//=====================================
#define CLWK_SETSF_NONE	(0xffff)

//-------------------------------------
///	Vram�A�h���X�擾���s
//	GFL_CLACT_WkGetPlttAddr
//	GFL_CLACT_WkGetCharAddr
//	�̖߂�l
//=====================================
#define CLWK_VRAM_ADDR_NONE	( 0xffffffff )




//-----------------------------------------------------------------------------
/**
 *					�O������J�V�X�e���\����
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�U�[��`�����_���[
//=====================================
typedef struct _CLSYS_REND	CLSYS_REND;

//-------------------------------------
///	�Z���A�N�^�[���j�b�g
//=====================================
typedef struct _CLUNIT		CLUNIT;

//-------------------------------------
///	�Z���A�N�^�[���[�N
//=====================================
typedef struct _CLWK		CLWK;





//-----------------------------------------------------------------------------
/**
 *					�������f�[�^�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	CLSYS�������f�[�^
//=====================================
typedef struct {
	s16 surface_main_left;	// ���C���@�T�[�t�F�[�X�̍�����W
	s16 surface_main_top;	// ���C���@�T�[�t�F�[�X�̍�����W
	s16 surface_sub_left;	// �T�u�@�T�[�t�F�[�X�̍�����W
	s16 surface_sub_top;	// �T�u�@�T�[�t�F�[�X�̍�����W
	u8	oamst_main;			// ���C�����OAM�Ǘ��J�n�ʒu
	u8	oamnum_main;		// ���C�����OAM�Ǘ���
	u8	oamst_sub;			// �T�u���OAM�Ǘ��J�n�ʒu
	u8	oamnum_sub;			// �T�u���OAM�Ǘ���
	u8	tr_cell;			// �Z��Vram�]���Ǘ���
} CLSYS_INIT;


//-------------------------------------
///	�Ǝ������_���[�쐬�p
/// �T�[�t�F�[�X�f�[�^�\����
//=====================================
typedef struct {
	s16	lefttop_x;			// �T�[�t�F�[�X���゘���W
	s16	lefttop_y;			// �T�[�t�F�[�X���゙���W
	s16	width;				// �T�[�t�F�[�X��
	s16	height;				// �T�[�t�F�[�X����
	CLSYS_DRAW_TYPE	type;	// �T�[�t�F�[�X�^�C�v(CLSYS_DRAW_TYPE)
} REND_SURFACE_INIT;

//-------------------------------------
///	CLWK���������\�[�X�f�[�^
//	���ꂼ��̃A�j�����@�̃f�[�^��ݒ肷��֐����p�ӂ���Ă��܂��B
//	�E�Z���A�j��			GFL_CLACT_WkSetCellResData(...)
//	�EVram�]���Z���A�j��	GFL_CLACT_WkSetTrCellResData(...)
//	�E�}���`�Z���A�j��		GFL_CLACT_WkSetMCellResData(...)
//=====================================
typedef struct {
	const NNSG2dImageProxy*			cp_img;		// �C���[�W�v���N�V
	const NNSG2dImagePaletteProxy*	cp_pltt;	// �p���b�g�v���N�V
	NNSG2dCellDataBank*				p_cell;		// �Z���f�[�^
    const NNSG2dCellAnimBankData*   cp_canm;	// �Z���A�j���[�V����

	// �ȉ��͕K�v�ȂƂ������l������
	const NNSG2dMultiCellDataBank*  cp_mcell;	// �}���`�Z���f�[�^		�i�����Ƃ�NULL�j
    const NNSG2dMultiCellAnimBankData* cp_mcanm;// �}���`�Z���A�j���[�V�����i�����Ƃ�NULL�j
	const NNSG2dCharacterData*		cp_char;	// Vram�]���Z���A�j���ȊO��NULL
} CLWK_RES;


//-------------------------------------
///	CLWK��������{�f�[�^
//=====================================
typedef struct {
	s16	pos_x;				// �����W
	s16 pos_y;				// �����W
	u16 anmseq;				// �A�j���[�V�����V�[�P���X
	u8	softpri;			// �\�t�g�D�揇��	0>0xff
	u8	bgpri;				// BG�D�揇��
} CLWK_DATA;

//-------------------------------------
///	CLWK�������A�t�B���ϊ��f�[�^
//=====================================
typedef struct {
	CLWK_DATA clwkdata;		// ��{�f�[�^
	
	s16	affinepos_x;		// �A�t�B�������W
	s16 affinepos_y;		// �A�t�B�������W
	fx32 scale_x;			// �g�傘�l
	fx32 scale_y;			// �g�備�l
	u16	rotation;			// ��]�p�x(0�`0xffff 0xffff��360�x)
	u16	affine_type;		// �㏑���A�t�B���^�C�v�iCLSYS_AFFINETYPE�j
} CLWK_AFFINEDATA;


//-------------------------------------
///	CLWK�@���W�\����	
//=====================================
typedef struct {
	s16 x;
	s16 y;
} CLSYS_POS;

//-------------------------------------
///	CLWK�@�X�P�[���\����
//=====================================
typedef struct {
	fx32 x;
	fx32 y;
} CLSYS_SCALE;





//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	CLSYS�֌W
//=====================================
GLOBAL void GFL_CLACT_SysInit( const CLSYS_INIT* cp_data, HEAPID heapID );
GLOBAL void GFL_CLACT_SysExit( void );
GLOBAL void GFL_CLACT_SysMain( void );
GLOBAL void GFL_CLACT_SysVblank( void );

//-------------------------------------
///	USER��`�����_���[�֌W
// �����Ǝ��̃T�[�t�F�[�X�ݒ������
// �����_���[���g�p�������Ƃ��A���̊֐��S�ō쐬���āA
// �Z���A�N�^�[���j�b�g�Ɋ֘A�t���邱�Ƃ��o���܂��B
//=====================================
GLOBAL CLSYS_REND* GFL_CLACT_UserRendCreate( const REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID );
GLOBAL void GFL_CLACT_UserRendDelete( CLSYS_REND* p_rend );
GLOBAL void GFL_CLACT_UserRendSetSurfacePos( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_pos );
GLOBAL void GFL_CLACT_UserRendGetSurfacePos( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_pos );
GLOBAL void GFL_CLACT_UserRendSetSurfaceSize( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_size );
GLOBAL void GFL_CLACT_UserRendGetSurfaceSize( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_size );
GLOBAL void GFL_CLACT_UserRendSetSurfaceType( CLSYS_REND* p_rend, u32 idx, CLSYS_DRAW_TYPE type );
GLOBAL CLSYS_DRAW_TYPE GFL_CLACT_UserRendGetSurfaceType( const CLSYS_REND* cp_rend, u32 idx );

//-------------------------------------
///	CLUNIT�֌W
//=====================================
GLOBAL CLUNIT* GFL_CLACT_UnitCreate( u16 wknum, HEAPID heapID );
GLOBAL void GFL_CLACT_UnitDelete( CLUNIT* p_unit );
GLOBAL void GFL_CLACT_UnitDraw( CLUNIT* p_unit );
GLOBAL void GFL_CLACT_UnitSetDrawFlag( CLUNIT* p_unit, BOOL on_off );
GLOBAL BOOL GFL_CLACT_UnitGetDrawFlag( const CLUNIT* cp_unit );
GLOBAL void GFL_CLACT_UnitSetUserRend( CLUNIT* p_unit, CLSYS_REND* p_rend );
GLOBAL void GFL_CLACT_UnitSetDefaultRend( CLUNIT* p_unit );

//-------------------------------------
///	CLWK�֌W
//
/*	�ysetsf�̐����z
 *		CLUNIT�̎g�p���郌���_���[�V�X�e����ύX���Ă��Ȃ��Ƃ���
 *		CLSYS_DRAW_TYPE�̒l���w�肷��
 *		�ECLSYS_DRAW_MAIN�w�莞	pos_x/y�����C����ʍ�����W����̑��΍��W�ɂȂ�B
 *		�ECLSYS_DRAW_SUB�w�莞	pos_x/y���T�u��ʍ�����W����̑��΍��W�ɂȂ�B
 *		
 *		�Ǝ��̃����_���[�V�X�e����CLUNIT�ɐݒ肵�Ă���Ƃ��́A
 *		�T�[�t�F�[�X�̗v�f�����w�肷�邱�ƂŁA
 *		�w�肳�ꂽ�T�[�t�F�[�X������W����̑��΍��W�ɂȂ�B
 *
 *		�ʏ�/�Ǝ������_���[�V�X�e�����ʂŁA
 *		CLWK_SETSF_NONE���w�肷��Ɛ�΍��W�ݒ�ɂȂ�
 */
//=====================================
// ���������\�[�X�f�[�^�ݒ�֐�
GLOBAL void GFL_CLACT_WkSetCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm );
GLOBAL void GFL_CLACT_WkSetTrCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dCharacterData* cp_char );
GLOBAL void GFL_CLACT_WkSetMCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dMultiCellDataBank* cp_mcell, const NNSG2dMultiCellAnimBankData* cp_mcanm );

// �o�^�j���֌W
GLOBAL CLWK* GFL_CLACT_WkAdd( CLUNIT* p_unit, const CLWK_DATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
GLOBAL CLWK* GFL_CLACT_WkAddAffine( CLUNIT* p_unit, const CLWK_AFFINEDATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
GLOBAL void GFL_CLACT_WkDel( CLWK* p_wk );

// �`��t���O
GLOBAL void GFL_CLACT_WkSetDrawFlag( CLWK* p_wk, BOOL on_off );
GLOBAL BOOL GFL_CLACT_WkGetDrawFlag( const CLWK* cp_wk );
// �T�[�t�F�[�X�����΍��W
GLOBAL void GFL_CLACT_WkSetPos( CLWK* p_wk, const CLSYS_POS* cp_pos, u16 setsf );
GLOBAL void GFL_CLACT_WkGetPos( const CLWK* cp_wk, CLSYS_POS* p_pos, u16 setsf );
GLOBAL void GFL_CLACT_WkSetTypePos( CLWK* p_wk, s16 pos, u16 setsf, CLSYS_MAT_TYPE type );
GLOBAL s16 GFL_CLACT_WkGetTypePos( const CLWK* cp_wk, u16 setsf, CLSYS_MAT_TYPE type );
// ��΍��W
GLOBAL void GFL_CLACT_WkSetWldPos( CLWK* p_wk, const CLSYS_POS* cp_pos );
GLOBAL void GFL_CLACT_WkGetWldPos( const CLWK* cp_wk, CLSYS_POS* p_pos );
GLOBAL void GFL_CLACT_WkSetWldTypePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
GLOBAL s16 GFL_CLACT_WkGetWldTypePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
// �A�t�B���p�����[�^
GLOBAL void GFL_CLACT_WkSetAffineParam( CLWK* p_wk, CLSYS_AFFINETYPE affine );
GLOBAL CLSYS_AFFINETYPE GFL_CLACTWkGetAffineParam( const CLWK* cp_wk );
// �A�t�B�����W
GLOBAL void GFL_CLACT_WkSetAffinePos( CLWK* p_wk, const CLSYS_POS* cp_pos );
GLOBAL void GFL_CLACT_WkGetAffinePos( const CLWK* cp_wk, CLSYS_POS* p_pos );
GLOBAL void GFL_CLACT_WkSetTypeAffinePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
GLOBAL s16 GFL_CLACT_WkGetTypeAffinePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
// �g��k��
GLOBAL void GFL_CLACT_WkSetScale( CLWK* p_wk, const CLSYS_SCALE* cp_sca );
GLOBAL void GFL_CLACT_WkGetScale( const CLWK* cp_wk, CLSYS_SCALE* p_sca );
GLOBAL void GFL_CLACT_WkSetTypeScale( CLWK* p_wk, fx32 scale, CLSYS_MAT_TYPE type );
GLOBAL fx32 GFL_CLACT_WkGetTypeScale( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
// ��]
GLOBAL void GFL_CLACT_WkSetRotation( CLWK* p_wk, u16 rotation );
GLOBAL u16  GFL_CLACT_WkGetRotation( const CLWK* cp_wk );
// �t���b�v
GLOBAL void GFL_CLACT_WkSetFlip( CLWK* p_wk, CLWK_FLIP_TYPE flip_type, BOOL on_off );
GLOBAL BOOL GFL_CLACT_WkGetFlip( const CLWK* cp_wk, CLWK_FLIP_TYPE flip_type );
// �I�u�W�F�N�g���[�h
GLOBAL void GFL_CLACT_WkSetObjMode( CLWK* p_wk, GXOamMode mode );
GLOBAL GXOamMode GFL_CLACT_WkGetObjMode( const CLWK* cp_wk );
// ���U�C�N
GLOBAL void GFL_CLACT_WkSetMosaic( CLWK* p_wk, BOOL on_off );
GLOBAL BOOL GFL_CLACT_WkGetMosaic( const CLWK* cp_wk );
// �p���b�g
GLOBAL void GFL_CLACT_WkSetPlttOffs( CLWK* p_wk, u8 pal_offs );
GLOBAL u8 GFL_CLACT_WkGetPlttOffs( const CLWK* cp_wk );
GLOBAL u32 GFL_CLACT_WkGetPlttAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type );
GLOBAL void GFL_CLACT_WkSetPlttProxy( CLWK* p_wk, const NNSG2dImagePaletteProxy* cp_pltt );
// �L�����N�^
GLOBAL u32 GFL_CLACT_WkGetCharAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type );
GLOBAL void GFL_CLACT_WkSetImgProxy( CLWK* p_wk, const NNSG2dImageProxy* cp_img );
// �\�t�g�D�揇��
GLOBAL void GFL_CLACT_WkSetSoftPri( CLWK* p_wk, u8 pri );
GLOBAL u8 GFL_CLACT_WkGetSoftPri( const CLWK* cp_wk );
// BG�D�揇��
GLOBAL void GFL_CLACT_WkSetBgPri( CLWK* p_wk, u8 pri );
GLOBAL u8 GFL_CLACT_WkGetBgPri( const CLWK* cp_wk );
// �A�j���[�V�����V�[�P���X
GLOBAL void GFL_CLACT_WkSetAnmSeq( CLWK* p_wk, u16 anmseq );
GLOBAL u16 GFL_CLACT_WkGetAnmSeq( const CLWK* cp_wk );
GLOBAL void GFL_CLACT_WkSetAnmSeqDiff( CLWK* p_wk, u16 anmseq );
// �A�j���[�V�����t���[��
GLOBAL void GFL_CLACT_WkSetAnmFrame( CLWK* p_wk, u16 idx );
GLOBAL void GFL_CLACT_WkAddAnmFrame( CLWK* p_wk, fx32 speed );
GLOBAL u16 GFL_CLACT_WkGetAnmFrame( const CLWK* cp_wk );
// �I�[�g�A�j���[�V����
GLOBAL void GFL_CLACT_WkSetAutoAnmFlag( CLWK* p_wk, BOOL on_off );
GLOBAL BOOL GFL_CLACT_WkGetAutoAnmFlag( const CLWK* cp_wk );
GLOBAL void GFL_CLACT_WkSetAutoAnmSpeed( CLWK* p_wk, fx32 speed );
GLOBAL fx32	GFL_CLACT_WkGetAutoAnmSpeed( const CLWK* cp_wk );
// �A�j���[�V�����Đ���~
GLOBAL void GFL_CLACT_WkStartAnm( CLWK* p_wk );
GLOBAL void GFL_CLACT_WkStopAnm( CLWK* p_wk );
GLOBAL BOOL GFL_CLACT_WkCheckAnmActive( const CLWK* cp_wk );
// �A�j���[�V������ԏ�����
GLOBAL void GFL_CLACT_WkResetAnm( CLWK* p_wk );
// �A�j���[�V�����Đ����@�̕ύX
GLOBAL void GFL_CLACT_WkSetAnmMode( CLWK* p_wk, CLSYS_ANM_PLAYMODE playmode );
GLOBAL CLSYS_ANM_PLAYMODE GFL_CLACT_WkGetAnmMode( const CLWK* cp_wk );

#undef	GLOBAL
#endif		// __CLACT_H__

