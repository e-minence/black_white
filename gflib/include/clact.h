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
//	GFL_CLWK�ւ̃t���b�v�ݒ�Ɏg�p���܂��B
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
//	GFL_CLWK�̍��W�ݒ�֐��Ő�΍��W��ݒ肷��Ƃ��Ɏg�p���܂��B
//=====================================
#define CLWK_SETSF_NONE	(0xffff)

//-------------------------------------
///	Vram�A�h���X�擾���s
//	GFL_CLACT_WK_GetPlttAddr
//	GFL_CLACT_WK_GetCharAddr
//	�̖߂�l
//=====================================
#define CLWK_VRAM_ADDR_NONE	( 0xffffffff )


//-------------------------------------
///	���[�U�[�g���A�g���r���[�g�擾
//	�f�[�^���Ȃ��Ƃ��̖߂�l
//	GFL_CLACT_WK_GetPlttAddr
//	GFL_CLACT_WK_GetCharAddr
//	�̖߂�l
//=====================================
#define CLWK_USERATTR_NONE	(0)	// ���[�U�[�g���A�g���r���[�g�Ȃ�



//-----------------------------------------------------------------------------
/**
 *					�O������J�V�X�e���\����
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�U�[��`�����_���[
//=====================================
typedef struct _CLSYS_REND	GFL_CLSYS_REND;

//-------------------------------------
///	�Z���A�N�^�[���j�b�g
//=====================================
typedef struct _CLUNIT		GFL_CLUNIT;

//-------------------------------------
///	�Z���A�N�^�[���[�N
//=====================================
typedef struct _CLWK		GFL_CLWK;





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
} GFL_CLSYS_INIT;


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
} GFL_REND_SURFACE_INIT;

//-------------------------------------
///	GFL_CLWK���������\�[�X�f�[�^
//	���ꂼ��̃A�j�����@�̃f�[�^��ݒ肷��֐����p�ӂ���Ă��܂��B
//	�E�Z���A�j��			GFL_CLACT_WK_SetCellResData(...)
//	�EVram�]���Z���A�j��	GFL_CLACT_WK_SetTrCellResData(...)
//	�E�}���`�Z���A�j��		GFL_CLACT_WK_SetMCellResData(...)
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
} GFL_CLWK_RES;


//-------------------------------------
///	GFL_CLWK��������{�f�[�^
//=====================================
typedef struct {
	s16	pos_x;				// �����W
	s16 pos_y;				// �����W
	u16 anmseq;				// �A�j���[�V�����V�[�P���X
	u8	softpri;			// �\�t�g�D�揇��	0>0xff
	u8	bgpri;				// BG�D�揇��
} GFL_CLWK_DATA;

//-------------------------------------
///	GFL_CLWK�������A�t�B���ϊ��f�[�^
//=====================================
typedef struct {
	GFL_CLWK_DATA clwkdata;		// ��{�f�[�^
	
	s16	affinepos_x;		// �A�t�B�������W
	s16 affinepos_y;		// �A�t�B�������W
	fx32 scale_x;			// �g�傘�l
	fx32 scale_y;			// �g�備�l
	u16	rotation;			// ��]�p�x(0�`0xffff 0xffff��360�x)
	u16	affine_type;		// �㏑���A�t�B���^�C�v�iCLSYS_AFFINETYPE�j
} GFL_CLWK_AFFINEDATA;


//-------------------------------------
///	GFL_CLWK�@���W�\����	
//=====================================
typedef struct {
	s16 x;
	s16 y;
} GFL_CLACTPOS;

//-------------------------------------
///	GFL_CLWK�@�X�P�[���\����
//=====================================
typedef struct {
	fx32 x;
	fx32 y;
} GFL_CLSCALE;





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
extern void GFL_CLACT_Init( const GFL_CLSYS_INIT* cp_data, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	�j��
 *
 *	@param	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_Exit( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@���C������
 *
 *	*�S�Z���A�N�^�[���j�b�g�̕`�悪�I�������ɌĂԕK�v������܂��B
 *	*���C�����[�v�̍Ō�ɌĂԂ悤�ɂ��Ă����Ɗm�����Ǝv���܂��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_Main( void );

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@V�u�����N����
 *
 * �����ł����A���݂܂���B�Ă�ł���������
 *
 * [�Z���A�N�^�[�V�X�e���@V�u�����N���ԓ��ł̓]�������@����]
 *	���p�r�ɍ��킹�ĂQ�̃^�C�v��I�ׂ�
 *�@�@�E�]���������@�ɂ͂Q�̃^�C�v������܂��B
 *		1.	GFL_CLACT_VBlankFunc
 *		2.	GFL_CLACT_VBlankFuncTransOnly �{�@GFL_CLACT_ClearOamBuff
 *	�@�E1�@GFL_CLACT_VBlankFunc
 *		OAM�f�[�^�]����A�o�b�t�@���N���[�����܂��B
 *	�@�@���������ȂǂŁA�P���[�v���ɂQ�񊄂荞�݂�����Ɖ����\������Ȃ�
 *	�@�@�Ȃ��Ă��܂��܂��̂ŁA���荞�ݓ��Ŏg�p���鎖���o���܂���B
 *
 *	  �E2�@GFL_CLACT_VBlankFuncTransOnly �{�@GFL_CLACT_ClearOamBuff
 *		GFL_CLACT_VBlankFuncTransOnly��OAM�o�b�t�@�̃f�[�^�̓]���̂ݍs���܂��̂ŁA
 *		���̊֐���VBlank���荞�ݓ��ŌĂ�ł��������B
 *		CLACT_UNIT�̕`��O��GFL_CLACT_ClearOamBuff���ĂԂ��Ƃ�OAM�o�b�t�@���N���A����
 *		�����o���܂��B
 *
 *	�@��GFL_CLACT_ClearOamBuff�ɂ���
 *		�E�Z���A�N�^�[�V�X�e�����ɂ�oammanclear�t���O�Ƃ������̂������Ă��܂��B
 *		�@����́AOamBuff���N���A���Ă悢���𔻕ʂ���t���O�ł��B
 *		�@�t���O�������Ă����Ԃ̂Ƃ��̂�GFL_CLACT_ClearOamBuff��
 *		�@���s���邱�Ƃ��o���܂��B
 *		�@�woammanclear�t���O�̓���x
 *			GFL_CLACT_VBlankFuncTransOnly���Ă΂��Ɨ����܂��B
 *			GFL_CLACT_ClearOamBuff���Ă΂��Ɨ����܂��B
 *		�@�����CLACT_UNIT�����R��GFL_CLACT_ClearOamBuff���Ăׂ�悤�ɂ��邽��
 *		�@�s���Ă��܂��B
 *		�EGFL_CLACT_VBlankFunc�̊֐��ł�oammanclear�t���O�������Ȃ��̂ŁA
 *		�@GFL_CLACT_ClearOamBuff�͋@�\���܂���B
 *			
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_VBlankFunc( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[OAM�o�b�t�@�̃N���[������
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_ClearOamBuff( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	V�u�����N����	�]���̂�
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_VBlankFuncTransOnly( void );

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
extern GFL_CLSYS_REND* GFL_CLACT_USERREND_Create( const GFL_REND_SURFACE_INIT* cp_data, u32 data_num, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e����j������
 *
 *	@param	p_rend			���[�U�[��`�T�[�t�F�[�X�f�[�^
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_Delete( GFL_CLSYS_REND* p_rend );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X������W��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_pos			�ݒ肷����W
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_SetSurfacePos( GFL_CLSYS_REND* p_rend, u32 idx, const GFL_CLACTPOS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X������W���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_GetSurfacePos( const GFL_CLSYS_REND* cp_rend, u32 idx, GFL_CLACTPOS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�T�C�Y��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	cp_size			�T�C�Y
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_SetSurfaceSize( GFL_CLSYS_REND* p_rend, u32 idx, const GFL_CLACTPOS* cp_size );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e������T�[�t�F�[�X�T�C�Y���擾����
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	p_size			�T�C�Y�擾��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_GetSurfaceSize( const GFL_CLSYS_REND* cp_rend, u32 idx, GFL_CLACTPOS* p_size );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���ɃT�[�t�F�[�X�^�C�v��ݒ肷��
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	idx				�T�[�t�F�[�X�C���f�b�N�X
 *	@param	type			�T�[�t�F�[�X�^�C�v
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_SetSurfaceType( GFL_CLSYS_REND* p_rend, u32 idx, CLSYS_DRAW_TYPE type );
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
extern CLSYS_DRAW_TYPE GFL_CLACT_USERREND_GetSurfaceType( const GFL_CLSYS_REND* cp_rend, u32 idx );

//-------------------------------------
///	GFL_CLUNIT�֌W
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
extern GFL_CLUNIT* GFL_CLACT_UNIT_Create( u16 wknum, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g��j��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_UNIT_Delete( GFL_CLUNIT* p_unit );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�@�`�揈��
 *
 *	@param	p_unit			�Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_UNIT_Draw( GFL_CLUNIT* p_unit );
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
extern void GFL_CLACT_UNIT_SetDrawFlag( GFL_CLUNIT* p_unit, BOOL on_off );
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
extern BOOL GFL_CLACT_UNIT_GetDrawFlag( const GFL_CLUNIT* cp_unit );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�Ƀ��[�U�[�Ǝ��̃����_���[�V�X�e����ݒ�
 *
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	p_rend		���[�U�[�Ǝ��̃����_���[
 *
 *	���̊֐��ŁA�D���Ȑݒ�����������_���[���g�p���邱�Ƃ��o����悤�ɂȂ�܂��B
 *	�����[�U�[�Ǝ��̃����_���[���쐬����
 *		GFL_CLACT_USERREND_�`�֐��ō쐬�ł��܂��B
 *
 *  �����ӎ���
 *	�@�@���W�ݒ肷��Ƃ��ɓn��setsf�̒l�́A���[�U�[�Ǝ������_���[
 *	�@�@�ɓo�^�����T�[�t�F�[�X�̃C���f�b�N�X�ԍ��ƂȂ�܂��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_UNIT_SetUserRend( GFL_CLUNIT* p_unit, GFL_CLSYS_REND* p_rend );
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
extern void GFL_CLACT_UNIT_SetDefaultRend( GFL_CLUNIT* p_unit );

//-------------------------------------
///	GFL_CLWK�֌W
//
/*	�ysetsf�̐����z
 *		GFL_CLUNIT�̎g�p���郌���_���[�V�X�e����ύX���Ă��Ȃ��Ƃ���
 *		CLSYS_DEFREND_TYPE�̒l���w�肷��
 *		�ECLSYS_DEFREND_MAIN�w�莞	pos_x/y�����C����ʍ�����W����̑��΍��W�ɂȂ�B
 *		�ECLSYS_DEFREND_SUB�w�莞	pos_x/y���T�u��ʍ�����W����̑��΍��W�ɂȂ�B
 *		
 *		�Ǝ��̃����_���[�V�X�e��(GFL_CLSYS_REND)��GFL_CLUNIT�ɐݒ肵�Ă���Ƃ��́A
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
extern void GFL_CLACT_WK_SetCellResData( GFL_CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm );
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
extern void GFL_CLACT_WK_SetTrCellResData( GFL_CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dCharacterData* cp_char );
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
extern void GFL_CLACT_WK_SetMCellResData( GFL_CLWK_RES* p_res, const NNSG2dImageProxy* cp_img, const NNSG2dImagePaletteProxy* cp_pltt, NNSG2dCellDataBank* p_cell, const NNSG2dCellAnimBankData* cp_canm, const NNSG2dMultiCellDataBank* cp_mcell, const NNSG2dMultiCellAnimBankData* cp_mcanm );

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
 *		GFL_CLUNIT�̎g�p���郌���_���[�V�X�e����ύX���Ă��Ȃ��Ƃ���
 *		CLSYS_DEFREND_TYPE�̒l���w�肷��
 *		�ECLSYS_DEFREND_MAIN�w�莞	pos_x/y�����C����ʍ�����W����̑��΍��W�ɂȂ�B
 *		�ECLSYS_DEFREND_SUB�w�莞	pos_x/y���T�u��ʍ�����W����̑��΍��W�ɂȂ�B
 *		
 *		�Ǝ��̃����_���[�V�X�e����GFL_CLUNIT�ɐݒ肵�Ă���Ƃ��́A
 *		�T�[�t�F�[�X�̗v�f�����w�肷�邱�ƂŁA
 *		�w�肳�ꂽ�T�[�t�F�[�X������W����̑��΍��W�ɂȂ�B
 *
 *		�ʏ�/�Ǝ������_���[�V�X�e�����ʂŁA
 *		CLWK_SETSF_NONE���w�肷��Ɛ�΍��W�ݒ�ɂȂ�
 */
//-----------------------------------------------------------------------------
extern GFL_CLWK* GFL_CLACT_WK_Add( GFL_CLUNIT* p_unit, const GFL_CLWK_DATA* cp_data, const GFL_CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
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
extern GFL_CLWK* GFL_CLACT_WK_AddAffine( GFL_CLUNIT* p_unit, const GFL_CLWK_AFFINEDATA* cp_data, const GFL_CLWK_RES* cp_res, u16 setsf, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�j������
 *
 *	@param	p_wk	�j������Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_Remove( GFL_CLWK* p_wk );

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
extern void GFL_CLACT_WK_SetDrawFlag( GFL_CLWK* p_wk, BOOL on_off );
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
extern BOOL GFL_CLACT_WK_GetDrawFlag( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pos			���W�\����
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetPos( GFL_CLWK* p_wk, const GFL_CLACTPOS* cp_pos, u16 setsf );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetPos( const GFL_CLWK* cp_wk, GFL_CLACTPOS* p_pos, u16 setsf );
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
extern void GFL_CLACT_WK_SetTypePos( GFL_CLWK* p_wk, s16 pos, u16 setsf, CLSYS_MAT_TYPE type );
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
extern s16 GFL_CLACT_WK_GetTypePos( const GFL_CLWK* cp_wk, u16 setsf, CLSYS_MAT_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N	
 *	@param	cp_pos			���W
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetWldPos( GFL_CLWK* p_wk, const GFL_CLACTPOS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pos			���W�i�[��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetWldPos( const GFL_CLWK* cp_wk, GFL_CLACTPOS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	��΍��W�ʐݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pos				�ݒ�l
 *	@param	type			���W�^�C�v
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetWldTypePos( GFL_CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
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
extern s16 GFL_CLACT_WK_GetWldTypePos( const GFL_CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
extern void GFL_CLACT_WK_SetAffineParam( GFL_CLWK* p_wk, CLSYS_AFFINETYPE affine );
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
extern CLSYS_AFFINETYPE GFL_CLACTWkGetAffineParam( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	cp_pos		�A�t�B���ϊ����΍��W
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAffinePos( GFL_CLWK* p_wk, const GFL_CLACTPOS* cp_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ꊇ�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	p_pos		�A�t�B���ϊ����΍��W�擾��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetAffinePos( const GFL_CLWK* cp_wk, GFL_CLACTPOS* p_pos );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�t�B���ϊ����΍��W�ʐݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pos			�ݒ�l
 *	@param	type		���W�^�C�v
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetTypeAffinePos( GFL_CLWK* p_wk, s16 pos, CLSYS_MAT_TYPE type );
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
extern s16 GFL_CLACT_WK_GetTypeAffinePos( const GFL_CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
extern void GFL_CLACT_WK_SetScale( GFL_CLWK* p_wk, const GFL_CLSCALE* cp_sca );
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���l�ꊇ�擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_sca			�g��k���l�擾��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetScale( const GFL_CLWK* cp_wk, GFL_CLSCALE* p_sca );
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
extern void GFL_CLACT_WK_SetTypeScale( GFL_CLWK* p_wk, fx32 scale, CLSYS_MAT_TYPE type );
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
extern fx32 GFL_CLACT_WK_GetTypeScale( const GFL_CLWK* cp_wk, CLSYS_MAT_TYPE type );
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
extern void GFL_CLACT_WK_SetRotation( GFL_CLWK* p_wk, u16 rotation );
//----------------------------------------------------------------------------
/**
 *	@brief	��]�p�x���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	��]�p�x(0�`0xffff 0xffff��360�x)
 */
//-----------------------------------------------------------------------------
extern u16  GFL_CLACT_WK_GetRotation( const GFL_CLWK* cp_wk );
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
extern void GFL_CLACT_WK_SetFlip( GFL_CLWK* p_wk, CLWK_FLIP_TYPE flip_type, BOOL on_off );
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
extern BOOL GFL_CLACT_WK_GetFlip( const GFL_CLWK* cp_wk, CLWK_FLIP_TYPE flip_type );
//----------------------------------------------------------------------------
/**
 *	@brief		�I�u�W�F���[�h�ݒ�
 *	
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	mode		�I�u�W�F���[�h
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetObjMode( GFL_CLWK* p_wk, GXOamMode mode );
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
extern GXOamMode GFL_CLACT_WK_GetObjMode( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	���U�C�N�t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		���U�C�NON�|OFF	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetMosaic( GFL_CLWK* p_wk, BOOL on_off );
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
extern BOOL GFL_CLACT_WK_GetMosaic( const GFL_CLWK* cp_wk );
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
extern void GFL_CLACT_WK_SetPlttOffs( GFL_CLWK* p_wk, u8 pal_offs );
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	pal_offs	�p���b�g�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
extern u8 GFL_CLACT_WK_GetPlttOffs( const GFL_CLWK* cp_wk );
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
extern u32 GFL_CLACT_WK_GetPlttAddr( const GFL_CLWK* cp_wk, CLSYS_DRAW_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	�V�����p���b�g�v���N�V��ݒ肷��
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pltt			�ݒ肷��p���b�g�v���N�V
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetPlttProxy( GFL_CLWK* p_wk, const NNSG2dImagePaletteProxy* cp_pltt );
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�v���N�V�f�[�^�̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_pltt			�p���b�g�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetPlttProxy( const GFL_CLWK* cp_wk,  NNSG2dImagePaletteProxy* p_pltt );
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
extern u32 GFL_CLACT_WK_GetCharAddr( const GFL_CLWK* cp_wk, CLSYS_DRAW_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V��ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_img			�ݒ�C���[�W�v���N�V
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetImgProxy( GFL_CLWK* p_wk, const NNSG2dImageProxy* cp_img );
//----------------------------------------------------------------------------
/**
 *	@brief	�C���[�W�v���N�V���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *	@param	p_img			�C���[�W�v���N�V�i�[��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetImgProxy( const GFL_CLWK* cp_wk,  NNSG2dImageProxy* p_img );
//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̐ݒ�
 *	
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				�\�t�g�D�揇��	0>0xff
 *	
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetSoftPri( GFL_CLWK* p_wk, u8 pri );
//----------------------------------------------------------------------------
/**
 *	@brief	�\�t�g�D�揇�ʂ̎擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@return	�\�t�g�D�揇��
 */
//-----------------------------------------------------------------------------
extern u8 GFL_CLACT_WK_GetSoftPri( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ̐ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	pri				BG�D�揇��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetBgPri( GFL_CLWK* p_wk, u8 pri );
//----------------------------------------------------------------------------
/**
 *	@brief	BG�D�揇�ʂ��擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	BG�D�揇��
 */
//-----------------------------------------------------------------------------
extern u8 GFL_CLACT_WK_GetBgPri( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmSeq( GFL_CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetAnmSeq( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X�̐����擾
 *	
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����V�[�P���X�̐�
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetAnmSeqNum( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X���ς���Ă�����ύX����
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmSeqDiff( GFL_CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X��ύX����@
 *			���̍ۃt���[�����ԂȂǂ̃��Z�b�g���s���܂���B�@MultiCell��Ή�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	anmseq		�A�j���[�V�����V�[�P���X
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmSeqNoReset( GFL_CLWK* p_wk, u16 anmseq );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	idx			�t���[����
 *	NitroCharacter�̃A�j���[�V�����V�[�P���X���̃R�}�ԍ����w�肵�Ă�������
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmFrame( GFL_CLWK* p_wk, u16 idx );
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
extern void GFL_CLACT_WK_AddAnmFrame( GFL_CLWK* p_wk, fx32 speed );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[�������擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetAnmFrame( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O�ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	on_off		�I�[�g�A�j���[�V����On-Off	TRUE:On	FALSE:Off
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAutoAnmFlag( GFL_CLWK* p_wk, BOOL on_off );
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
extern BOOL GFL_CLACT_WK_GetAutoAnmFlag( const GFL_CLWK* cp_wk );
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
extern void GFL_CLACT_WK_SetAutoAnmSpeed( GFL_CLWK* p_wk, fx32 speed );
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����X�s�[�h���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�I�[�g�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
extern fx32 GFL_CLACT_WK_GetAutoAnmSpeed( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����J�n
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	GFL_CLACT_WK_StopAnm�łƂ߂��A�j���[�V�������J�n�����܂��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_StartAnm( GFL_CLWK* p_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V������~
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_StopAnm( GFL_CLWK* p_wk );
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
extern BOOL GFL_CLACT_WK_CheckAnmActive( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������X�^�[�g
 *
 *	@param	p_wk	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_ResetAnm( GFL_CLWK* p_wk );
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
extern void GFL_CLACT_WK_SetAnmMode( GFL_CLWK* p_wk, CLSYS_ANM_PLAYMODE playmode );
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
extern CLSYS_ANM_PLAYMODE GFL_CLACT_WK_GetAnmMode( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����V�[�P���X�̃��[�U�[�g���A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	seq			�V�[�P���X�i���o�[
 *
 *	@retval	CLWK_USERATTR_NONE	�g���A�g���r���[�g�f�[�^�Ȃ�
 *	@retval	����ȊO			�g���A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
extern u32 GFL_CLACT_WK_GetUserAttrAnmSeq( const GFL_CLWK* cp_wk, u32 seq );
//----------------------------------------------------------------------------
/**
 *	@brief	���̃A�j���[�V�����V�[�P���X�̃��[�U�[�g���A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	CLWK_USERATTR_NONE	�g���A�g���r���[�g�f�[�^�Ȃ�
 *	@retval	����ȊO			�g���A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
extern u32 GFL_CLACT_WK_GetUserAttrAnmSeqNow( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[���̃��[�U�[�g���A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	seq			�V�[�P���X�i���o�[
 *	@param	frame		�t���[���i���o�[
 *
 *	@retval	CLWK_USERATTR_NONE	�g���A�g���r���[�g�f�[�^�Ȃ�
 *	@retval	����ȊO			�g���A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
extern u32 GFL_CLACT_WK_GetUserAttrAnmFrame( const GFL_CLWK* cp_wk, u32 seq, u32 frame );
//----------------------------------------------------------------------------
/**
 *	@brief	���̃A�j���[�V�����t���[���̃��[�U�[�g���A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@retval	CLWK_USERATTR_NONE	�g���A�g���r���[�g�f�[�^�Ȃ�
 *	@retval	����ȊO			�g���A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
extern u32 GFL_CLACT_WK_GetUserAttrAnmFrameNow( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���̃��[�U�[�g���A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	cellidx		�Z���C���f�b�N�X
 *
 *	@retval	CLWK_USERATTR_NONE	�g���A�g���r���[�g�f�[�^�Ȃ�
 *	@retval	����ȊO			�g���A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
extern u32 GFL_CLACT_WK_GetUserAttrCell( const GFL_CLWK* cp_wk, u32 cellidx );

#endif		// __CLACT_H__

