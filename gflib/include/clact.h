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
///	�f�t�H���g�ݒ背���_���[���T�[�t�F�[�X�w��萔
//=====================================
typedef enum{
	CLSYS_DEFREND_MAIN,
	CLSYS_DEFREND_SUB,
	CLSYS_DEFREND_NUM
} CLSYS_DEFREND_TYPE;


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
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@������
 *
 *	@param	cp_data		�������f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysInit( const CLSYS_INIT* cp_data, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	�j��
 *
 *	@param	none
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysExit( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@���C������
 *
 *	*�S�Z���A�N�^�[���j�b�g�̕`�悪�I�������ɌĂԕK�v������܂��B
 *	*���C�����[�v�̍Ō�ɌĂԂ悤�ɂ��Ă����Ɗm�����Ǝv���܂��B
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysMain( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@V�u�����N����
 *
 *	��OAM�f�[�^�]����o�b�t�@���N���[�����܂��B
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysVblank( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[OAM�o�b�t�@�̃N���[������
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysOamBuffClean( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	V�u�����N����	�]���̂�
 *
 *	��OAM�f�[�^�̓]���̂ݍs���܂��B
 *	OAM�o�b�t�@�̏������́A�e���̃^�C�~���O�ōs���Ă��������B
 *	���̍ۂɂ́uGFL_CLACT_SysOamBuffClean�v�֐����g�p���Ă��������B
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_SysVblankTransOnly( void );

//-------------------------------------
///	USER��`�����_���[�֌W
// �����Ǝ��̃T�[�t�F�[�X�ݒ������
// �����_���[���g�p�������Ƃ��A���̊֐��S�ō쐬���āA
// �Z���A�N�^�[���j�b�g�Ɋ֘A�t���邱�Ƃ��o���܂��B
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
GLOBAL CLSYS_REND* GFL_CLACT_UserRendCreate( const REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e����j������
 *
 *	@param	p_rend			���[�U�[��`�T�[�t�F�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendDelete( CLSYS_REND* p_rend );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X������W��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_pos			�ݒ肷����W
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendSetSurfacePos( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X������W���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendGetSurfacePos( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�T�C�Y��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_size			�T�C�Y
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendSetSurfaceSize( CLSYS_REND* p_rend, u32 idx, const CLSYS_POS* cp_size );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X�T�C�Y���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_size			�T�C�Y�擾��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendGetSurfaceSize( const CLSYS_REND* cp_rend, u32 idx, CLSYS_POS* p_size );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�^�C�v��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	type			�T�[�t�F�[�X�^�C�v
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UserRendSetSurfaceType( CLSYS_REND* p_rend, u32 idx, CLSYS_DRAW_TYPE type );
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
GLOBAL CLSYS_DRAW_TYPE GFL_CLACT_UserRendGetSurfaceType( const CLSYS_REND* cp_rend, u32 idx );

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
GLOBAL CLUNIT* GFL_CLACT_UnitCreate( u16 wknum, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g��j��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UnitDelete( CLUNIT* p_unit );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�@�`�揈��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_UnitDraw( CLUNIT* p_unit );
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
GLOBAL void GFL_CLACT_UnitSetDrawFlag( CLUNIT* p_unit, BOOL on_off );
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
GLOBAL BOOL GFL_CLACT_UnitGetDrawFlag( const CLUNIT* cp_unit );
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
GLOBAL void GFL_CLACT_UnitSetUserRend( CLUNIT* p_unit, CLSYS_REND* p_rend );
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
GLOBAL void GFL_CLACT_UnitSetDefaultRend( CLUNIT* p_unit );

//-------------------------------------
///	CLWK�֌W
//
/*	�ysetsf�̐����z
 *		CLUNIT�̎g�p���郌���_���[�V�X�e����ύX���Ă��Ȃ��Ƃ���
 *		CLSYS_DEFREND_TYPE�̒l���w�肷��
 *		�ECLSYS_DEFREND_MAIN�w�莞	pos_x/y�����C����ʍ�����W����̑��΍��W�ɂȂ�B
 *		�ECLSYS_DEFREND_SUB�w�莞	pos_x/y���T�u��ʍ�����W����̑��΍��W�ɂȂ�B
 *		
 *		�Ǝ��̃����_���[�V�X�e��(CLSYS_REND)��CLUNIT�ɐݒ肵�Ă���Ƃ��́A
 *		�T�[�t�F�[�X�̗v�f�����w�肷�邱�ƂŁA
 *		�w�肳�ꂽ�T�[�t�F�[�X������W����̑��΍��W�ɂȂ�B
 *
 *		�ʏ�/�Ǝ������_���[�V�X�e�����ʂŁA
 *		CLWK_SETSF_NONE���w�肷��Ɛ�΍��W�ݒ�ɂȂ�
 */
//=====================================
// ���������\�[�X�f�[�^�ݒ�֐�
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
GLOBAL void GFL_CLACT_WkSetCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm );
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
GLOBAL void GFL_CLACT_WkSetTrCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dCharacterData* cp_char );
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
GLOBAL void GFL_CLACT_WkSetMCellResData( CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dMultiCellDataBank* cp_mcell, const NNSG2dMultiCellAnimBankData* cp_mcanm );

// �o�^�j���֌W
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
GLOBAL CLWK* GFL_CLACT_WkAdd( CLUNIT* p_unit, const CLWK_DATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
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
GLOBAL CLWK* GFL_CLACT_WkAddAffine( CLUNIT* p_unit, const CLWK_AFFINEDATA* cp_data, const CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�j������
 *
 *	@param	p_wk	�j������Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkDel( CLWK* p_wk );

// ���[�N�ɑ΂��鑀��
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
GLOBAL void GFL_CLACT_WkSetDrawFlag( CLWK* p_wk, BOOL on_off );
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
GLOBAL BOOL GFL_CLACT_WkGetDrawFlag( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pos			���W�\����
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetPos( CLWK* p_wk, const CLSYS_POS* cp_pos, u16 setsf );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetPos( const CLWK* cp_wk, CLSYS_POS* p_pos, u16 setsf );
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
GLOBAL void GFL_CLACT_WkSetTypePos( CLWK* p_wk, s16 pos, u16 setsf, CLSYS_MAT_TYPE type );
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
GLOBAL s16 GFL_CLACT_WkGetTypePos( const CLWK* cp_wk, u16 setsf, CLSYS_MAT_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N	
 *	@param	cp_pos			���W
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetWldPos( CLWK* p_wk, const CLSYS_POS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetWldPos( const CLWK* cp_wk, CLSYS_POS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ʐݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pos				�ݒ�l
 *	@param	type			���W�^�C�v
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetWldTypePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
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
GLOBAL s16 GFL_CLACT_WkGetWldTypePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
GLOBAL void GFL_CLACT_WkSetAffineParam( CLWK* p_wk, CLSYS_AFFINETYPE affine );
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
GLOBAL CLSYS_AFFINETYPE GFL_CLACTWkGetAffineParam( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	cp_pos		�A�t�B���ϊ����΍��W
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAffinePos( CLWK* p_wk, const CLSYS_POS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	p_pos		�A�t�B���ϊ����΍��W�擾��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetAffinePos( const CLWK* cp_wk, CLSYS_POS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ʐݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pos			�ݒ�l
 *	@param	type		���W�^�C�v
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetTypeAffinePos( CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
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
GLOBAL s16 GFL_CLACT_WkGetTypeAffinePos( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
GLOBAL void GFL_CLACT_WkSetScale( CLWK* p_wk, const CLSYS_SCALE* cp_sca );
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ꊇ�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_sca			�g��k���l�擾��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetScale( const CLWK* cp_wk, CLSYS_SCALE* p_sca );
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
GLOBAL void GFL_CLACT_WkSetTypeScale( CLWK* p_wk, fx32 scale, CLSYS_MAT_TYPE type );
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
GLOBAL fx32 GFL_CLACT_WkGetTypeScale( const CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
GLOBAL void GFL_CLACT_WkSetRotation( CLWK* p_wk, u16 rotation );
//----------------------------------------------------------------------------
/**
 *	@brief	��]�p�x���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	��]�p�x(0�`0xffff 0xffff��360�x)
 */
//-----------------------------------------------------------------------------
GLOBAL u16  GFL_CLACT_WkGetRotation( const CLWK* cp_wk );
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
GLOBAL void GFL_CLACT_WkSetFlip( CLWK* p_wk, CLWK_FLIP_TYPE flip_type, BOOL on_off );
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
GLOBAL BOOL GFL_CLACT_WkGetFlip( const CLWK* cp_wk, CLWK_FLIP_TYPE flip_type );
//----------------------------------------------------------------------------
/**
 *	@brief		�I�u�W�F���[�h�ݒ�
 *	
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	mode		�I�u�W�F���[�h
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetObjMode( CLWK* p_wk, GXOamMode mode );
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
GLOBAL GXOamMode GFL_CLACT_WkGetObjMode( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	���U�C�N�t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		���U�C�NON�|OFF	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetMosaic( CLWK* p_wk, BOOL on_off );
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
GLOBAL BOOL GFL_CLACT_WkGetMosaic( const CLWK* cp_wk );
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
GLOBAL void GFL_CLACT_WkSetPlttOffs( CLWK* p_wk, u8 pal_offs );
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	pal_offs	�p���b�g�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
GLOBAL u8 GFL_CLACT_WkGetPlttOffs( const CLWK* cp_wk );
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
GLOBAL u32 GFL_CLACT_WkGetPlttAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	�V�����p���b�g�v���N�V��ݒ肷��
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pltt			�ݒ肷��p���b�g�v���N�V
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetPlttProxy( CLWK* p_wk, const NNSG2dImagePaletteProxy* cp_pltt );
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�v���N�V�f�[�^�̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pltt			�p���b�g�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetPlttProxy( const CLWK* cp_wk,  NNSG2dImagePaletteProxy* p_pltt );
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
GLOBAL u32 GFL_CLACT_WkGetCharAddr( const CLWK* cp_wk, CLSYS_DRAW_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V��ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_img			�ݒ�C���[�W�v���N�V
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetImgProxy( CLWK* p_wk, const NNSG2dImageProxy* cp_img );
//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_img			�C���[�W�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkGetImgProxy( const CLWK* cp_wk,  NNSG2dImageProxy* p_img );
//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̐ݒ�
 *	
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				�\�t�g�D�揇��	0>0xff
 *	
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetSoftPri( CLWK* p_wk, u8 pri );
//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	�\�t�g�D�揇��
 */
//-----------------------------------------------------------------------------
GLOBAL u8 GFL_CLACT_WkGetSoftPri( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ̐ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				BG�D�揇��
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetBgPri( CLWK* p_wk, u8 pri );
//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ��擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	BG�D�揇��
 */
//-----------------------------------------------------------------------------
GLOBAL u8 GFL_CLACT_WkGetBgPri( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAnmSeq( CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
GLOBAL u16 GFL_CLACT_WkGetAnmSeq( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���ς���Ă�����ύX����
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAnmSeqDiff( CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX����@
 *			���̍ۃt���[�����ԂȂǂ̃��Z�b�g���s���܂���B�@MultiCell��Ή�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAnmSeqNoReset( CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	idx			�t���[����
 *	NitroCharacter�̃A�j���[�V�����V�[�P���X���̃R�}�ԍ����w�肵�Ă�������
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAnmFrame( CLWK* p_wk, u16 idx );
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
GLOBAL void GFL_CLACT_WkAddAnmFrame( CLWK* p_wk, fx32 speed );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[�������擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
GLOBAL u16 GFL_CLACT_WkGetAnmFrame( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		�I�[�g�A�j���[�V����On-Off	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkSetAutoAnmFlag( CLWK* p_wk, BOOL on_off );
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
GLOBAL BOOL GFL_CLACT_WkGetAutoAnmFlag( const CLWK* cp_wk );
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
GLOBAL void GFL_CLACT_WkSetAutoAnmSpeed( CLWK* p_wk, fx32 speed );
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����X�s�[�h���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�I�[�g�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
GLOBAL fx32 GFL_CLACT_WkGetAutoAnmSpeed( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����J�n
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	GFL_CLACT_WkStopAnm�łƂ߂��A�j���[�V�������J�n�����܂��B
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkStartAnm( CLWK* p_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V������~
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkStopAnm( CLWK* p_wk );
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
GLOBAL BOOL GFL_CLACT_WkCheckAnmActive( const CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������X�^�[�g
 *
 *	@param	p_wk	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
GLOBAL void GFL_CLACT_WkResetAnm( CLWK* p_wk );
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
GLOBAL void GFL_CLACT_WkSetAnmMode( CLWK* p_wk, CLSYS_ANM_PLAYMODE playmode );
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
GLOBAL CLSYS_ANM_PLAYMODE GFL_CLACT_WkGetAnmMode( const CLWK* cp_wk );

#undef	GLOBAL
#endif		// __CLACT_H__

