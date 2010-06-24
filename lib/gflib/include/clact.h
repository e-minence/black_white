//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		clact.h
 *	@brief		�Z���A�N�^�[�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2006.11.28
 *	@data		2008.12.24		5�_�ύX
 *								�@1.��{�̃V�X�e���̊֐���Sys������悤�ɕύX
 *								�@2.Init Exit��Create Delete�ɕύX
 *								�@3.DrawFlag��DrawEnable�ɕύX
 *								�@4.CLUNIT�̕`��֐���p�~���AGFL_CLACT_MainSys�֐��Ȃ��ŁA���������ɕ`�悷��悤�ɕύX
 *								�@5.VBlank�ł̓]���֐����P�ɂ܂Ƃ�B
 *								�@**4.5.�́AVBlank���Ԃł̏������܂Ƃ߂邽�߂̕ύX*
 *
 *	@data		2009.01.07		obj_graphic_man.h�̓��e�𓝍�
 *	@data		2009.08.31		�EGFL_CLACT_WK_SetAnmFrame�̈Ӗ��𖼑O�ʂ�A
 *	                      �@�A�j���[�V�����t���[������ݒ肷��֐��ɕύX
 *	                        ��SetAnmFrame�̓}���`�Z���g�p�s�\�ł��B
 *	                      �E�A�j���[�V�����̃C���f�b�N�X��ݒ肷��֐�
 *	                        GFL_CLACT_WK_SetAnmIndex��ǉ�
 *	@data   2009.09.08    Element(�A�j���[�V�������ʏ��)���擾����C���^�[�t�F�[�X��ǉ�
 *	@data   2009.09.30    �L�����N�^�A�p���b�g�ē]���֐����쐬
 *	@data   2010.1.27    �A�j���[�V�����R�[���o�b�N��ݒ�
 *	@data   2010.2.5    OAMAttr�P�ʂ̃J�����O�쐬�@GFL_CLACT_USERREND_SetOAMAttrCulling�@
 *	@data   2010.4.2    �A�t�B�����l�����Ȃ��J�����O���쐬�@���[�U�[�����_���[�̏��������Ɏw��ł���悤�ɁB
 *	@data   2010.4.12    �J�����O�Ȃ��@�ݒ���쐬
 *	@data   2010.5.10    �A�j���[�V����SRT�A�j���[�V�������f��ONOFF�쐬
 *	@data   2010.5.13    �_�~�[�̃L�����N�^�f�[�^�o�^�֐��ǉ�
 *	                      
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __CLACT_H__
#define __CLACT_H__

#include "arc_tool.h"
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�A�T�[�g�`�F�b�N�@���x���ύX
// 1�ɂ��邱�Ƃ�NULL�`�F�b�N���s���Ă���ASSERT���J�b�g���܂��B
// �`�F�b�N���Â��Ȃ�܂����A�������������邱�Ƃ��o���܂��B
//=====================================
#define GFL_CLACT_ASSERT_LEVEL_LAW (1)

//-------------------------------------
///	CLSYS�`��ʃ^�C�v
//	�����_���[�̃T�[�t�F�[�X�ݒ��A
//	Vram�A�h���X�w��Ɏg�p���܂��B
//=====================================
typedef enum {
	CLSYS_DRAW_MAIN,// ���C�����
	CLSYS_DRAW_SUB,	// �T�u���

	CLSYS_DRAW_MAX,	// �����
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
///	�����_���[�J�����O�^�C�v
//=====================================
typedef enum{
  CLSYS_REND_CULLING_TYPE_NORMAL,       // �ʏ�
  CLSYS_REND_CULLING_TYPE_NOT_AFFINE,   // �A�t�B�����l�����Ȃ�     ���ׁ@��
  CLSYS_REND_CULLING_TYPE_NONE,        // �J�����O���s��Ȃ��@�J�����O���ׁ@�Œ�
  CLSYS_REND_CULLING_TYPE_MAX,    // �V�X�e�����Ŏg�p
} CLSYS_REND_CULLING_TYPE;

//-------------------------------------
///	�p���b�g�I�t�Z�b�g�ݒ胂�[�h
//=====================================
typedef enum{
	CLWK_PLTTOFFS_MODE_OAM_COLOR,		// OamAttr.c_param����̃I�t�Z�b�g�ݒ�
	CLWK_PLTTOFFS_MODE_PLTT_TOP,		// �p���b�g�]����̐擪����̃I�t�Z�b�g�ݒ�

	CLWK_PLTTOFFS_MODE_NUM
} CLWK_PLTTOFFS_MODE;

//-------------------------------------
///	�A�j���[�V�����R�[���o�b�N�^�C�v
//=====================================
typedef enum{
  CLWK_ANM_CALLBACK_TYPE_LAST_FRM,    // �A�j���[�V�����V�[�P���X�̍ŏI�t���[���I�����ɂ�т���
  CLWK_ANM_CALLBACK_TYPE_SPEC_FRM,    // �w��t���[���̍Đ����ɌĂт���
  CLWK_ANM_CALLBACK_TYPE_EVER_FRM,    // ���t���[���Ăяo��

  CLWK_ANM_CALLBACK_TYPE_MAX,    // �V�X�e�����Ŏg�p
} CLWK_ANM_CALLBACK_TYPE;

//-------------------------------------
///	�A�j���[�V�����R�[���o�b�N�֐�
//=====================================
typedef void (*CLWK_ANM_CALLBACK_FUNC)( u32 param, fx32 currentFrame );


//-------------------------------------
///	�T�[�t�F�[�X�C���f�b�N�X�@��΍��W�w��
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


//-------------------------------------
///	�O���t�B�b�N�Ǘ��萔
//=====================================
#define GFL_CLGRP_REGISTER_FAILED	(0xffffffff)	// �o�^���s
#define GFL_CLGRP_EXPLTT_OFFSET		(0x200)			// �g���p���b�g�w��


//-------------------------------------
///	OAM�Ǘ����@���[��
//
//	OAM�o�b�t�@���ꊇ�]�����邽�߂ɁA�ȉ��Q�̃��[�������K�v������܂��B
//	�P�D�Ǘ��J�n�ʒu��4�̔{���ɂ���
//	�Q�D�Ǘ�����4�̔{���ɂ���	
//=====================================
#define GFL_CLSYS_OAMMAN_INTERVAL	( 4 )


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
	u8	oamst_main;			// ���C�����OAM�Ǘ��J�n�ʒu 4�̔{��
	u8	oamnum_main;		// ���C�����OAM�Ǘ���		 4�̔{��
	u8	oamst_sub;			// �T�u���OAM�Ǘ��J�n�ʒu	 4�̔{��
	u8	oamnum_sub;			// �T�u���OAM�Ǘ���		 4�̔{��
	u32	tr_cell;			// �Z��Vram�]���Ǘ���

	u16  CGR_RegisterMax;			///< �o�^�ł���L�����f�[�^��
	u16  PLTT_RegisterMax;			///< �o�^�ł���p���b�g�f�[�^��
	u16  CELL_RegisterMax;			///< �o�^�ł���Z���A�j���p�^�[����
	u16  MULTICELL_RegisterMax;		///< �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

  u16 CGRVMan_AreaOffsMain; ///< ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
  u16 CGRVMan_AreaOffsSub;  ///< �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
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
  CLSYS_REND_CULLING_TYPE culling;  // �J�����O�^�C�v
} GFL_REND_SURFACE_INIT;


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


//-------------------------------------
///	�A�j���[�V�����v�f�\����
//=====================================
typedef struct {
  u16   cell_index;       // �Z���C���f�b�N�X
  u16   rotZ;             // ��]Z
  fx32  scale_x;          // �X�P�[��X
  fx32  scale_y;          // �X�P�[��Y
  s16   trans_x;          // �ړ�X
  s16   trans_y;          // �ړ�Y
} GFL_CLWK_ANMELEMENT;


//-------------------------------------
///	�A�j���[�V�����v�f�\����
//=====================================
typedef struct {
  u16 callback_type;  // CLWK_ANM_CALLBACK_TYPE
  u16 frame_idx;      // CLWK_ANM_CALLBACK_TYPE_SPEC_FRM�p�t���[���C���f�b�N�X
  u32 param;          // �R�[���o�b�N���[�N
  CLWK_ANM_CALLBACK_FUNC  p_func; // �R�[���o�b�N�֐�
} GFL_CLWK_ANM_CALLBACK;
   


//-----------------------------------------------------------------------------
/**
 *				��{�I��CLSYS_INIT�f�[�^
 *				�Ƃ肠�����o���Ă݂����Ƃ��ȂǂɎg�p���Ă�������
 */
//-----------------------------------------------------------------------------
/*
 *	�㉺��ʂ��Ȃ��Ȃ��ꍇ
 *	const GFL_CLSYS_INIT GFL_CLSYSINIT_DEF_DIVSCREEN = {
 *		0, 0,
 *		0, 512,
 *		4, 124,
 *		4, 124,
 *		0,
 *		32,32,32,32,
 *    16, 16,
 *	};
 */
extern const GFL_CLSYS_INIT GFL_CLSYSINIT_DEF_DIVSCREEN;
/*
 *	�㉺��ʂ��Ȃ���ꍇ
 *	const GFL_CLSYS_INIT GFL_CLSYSINIT_DEF_CONSCREEN = {
 *		0, 0,
 *		0, 192,
 *		4, 124,
 *		4, 124,
 *		0,
 *		32,32,32,32,
 *    16, 16,
 *	};
 */
extern const GFL_CLSYS_INIT GFL_CLSYSINIT_DEF_CONSCREEN;

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
 *	@param	cp_vrambank	VRAM�o���N�ݒ�
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_SYS_Create( const GFL_CLSYS_INIT* cp_data, const GFL_DISP_VRAM* cp_vramBank, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	�j��
 *
 *	@param	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_SYS_Delete( void );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@���C������
 *
 *	���j�b�g�`�揈��
 *	VRAM�]���A�j���̏ꍇ�̓L�����N�^����]���^�X�N�ւ̓o�^���܂��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_SYS_Main( void );

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e���@V�u�����N����
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_SYS_VBlankFunc( void );

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�V�X�e��	OAM�o�b�t�@�̃N���A�֐�
 *
 *	���SCLUNIT��OAM�������܂��̂Œ��ӂ��K�v�ł��B
 *	�@���f�����͎̂���VBlank���Ԃł��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_SYS_ClearAllOamBuffer( void );


//-------------------------------------
///	GFL_CLGRP�֌W
//	���\�[�X�Ǘ��֌W�ł��B
//=====================================
//-----------------------------------------------------------------------------------------------------------
/**
 *	CGR�f�[�^�֘A����
 *
 *	�E�o�^
 *		GFL_CLGRP_CGR_Register					VRAM�풓�^�f�[�^�p
 *		GFL_CLGRP_CGR_Register_VramTransfer		VRAM�]���^�f�[�^�p
 *		GFL_CLGRP_CGR_CreateAlies_VramTransfer	�o�^�ς�VRAM�]���^�f�[�^�̃G�C���A�X�𐶐�
 *		GFL_CLGRP_CGR_RegisterDummy			VRAM�풓�^�_�~�[�f�[�^�p
 *
 *	�E���
 *		GFL_CLGRP_CGR_Release
 *
 *	�EVRAM�]�����f�[�^�̍����ւ�
 *		GFL_CLGRP_CGR_ReplaceSrc_VramTransfer
 *
 *	�EVRAM�]�����f�[�^�̃|�C���^�擾
 *		GFL_CLGRP_CGR_GetSrcPointer_VramTransfer
 *
 *	�E�v���L�V�擾
 *		GFL_CLGRP_CGR_GetProxy
 *
 */
//-----------------------------------------------------------------------------------------------------------
//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�풓�^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h����VRAM�]�����s���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��͔̂j������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   cgrDataID		[in] �A�[�J�C�u����CGR�f�[�^ID
 * @param   compressedFlag	[in] �f�[�^�����k����Ă��邩
 * @param   targetVram		[in] �]����VRAM�w��
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_CLGRP_CGR_Register( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, CLSYS_DRAW_TYPE targetVram, HEAPID heapID );


//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�]���^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��̂��ێ���������B
 * CGR�f�[�^�Ɋ֘A�t����ꂽ�Z���A�j���f�[�^����ɓo�^����Ă���K�v������B
 *
 * @param   arcHandle		�A�[�J�C�u�n���h��
 * @param   cgrDataID		CGR�f�[�^�̃A�[�J�C�u��ID
 * @param   targetVram		�]����VRAM�w��
 * @param   cellIndex		�֘A�t����ꂽ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   heapID			�f�[�^���[�h�p�q�[�v
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_CLGRP_CGR_Register_VramTransfer( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, CLSYS_DRAW_TYPE targetVram, u32 cellIndex, HEAPID heapID );

//==============================================================================================
/**
 * �o�^����Ă���VRAM�]���^CGR�̃G�C���A�X�𐶐����A�V���ɓo�^����
 *
 * ���łɓo�^����Ă���CGR�f�[�^�Ɠ���̂��Q�Ƃ��A�v���L�V���쐬�E�ێ�����B
 * �o�^����Ă���CGR�f�[�^�Ɠ���̂��̂��Q�Ƃ��邽�߁A�q�[�v�g�p�ʂ�}���邱�Ƃ��ł���B
 * CGR�f�[�^�Ɋ֘A�t����ꂽ�Z���A�j���f�[�^����ɓo�^����Ă���K�v������B
 *
 * ���ŏ��ɓo�^����CGR�f�[�^�ƃG�C���A�X�i���������j�͂ǂ̏��Ԃŉ�����Ă��ǂ��B
 *
 * @param   srcCgrIdx		[in] ���łɓo�^����Ă���CGR�f�[�^�C���f�b�N�X�iVRAM�]���^�j
 * @param   cellAnimIdx		[in] �֘A�Â���ꂽ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   targetVram		[in] �]����VRAM
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_CLGRP_CGR_CreateAliesVramTransfer( u32 srcCgrIdx, u32 cellAnimIdx, CLSYS_DRAW_TYPE targetVram );

//==============================================================================================
/**
 * ��CGR�f�[�^�̓o�^�iVRAM�풓�^OBJ�p�j
 *
 * �_�~�[�f�[�^��VRAM�]�����s���A�v���L�V���쐬���ĕێ�����B
 *
 * @param   size        [in] �_�~�[�L�����N�^�T�C�Y
 * @param   targetVram		[in] �]����VRAM�w��
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_CLGRP_CGR_RegisterDummy( u32 size, CLSYS_DRAW_TYPE targetVram, HEAPID heapID );


//==============================================================================================
/**
 * �o�^���ꂽCGR�f�[�^�̉��
 *
 * @param   index		�o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_CLGRP_CGR_Release( u32 index );

//==============================================================================================
/**
 * �o�^�ς�CGR�̓]���A�h���X�ɕʂ̃f�[�^���Ĕz�u����(VRAM�]���^�͕s�\)
 *
 * @param   index			CGR�f�[�^�̓o�^�C���f�b�N�X
 * @param	src				�ǂݍ��ݍς݃L�����N�^���\�[�X
 *
 */
//==============================================================================================
extern void GFL_CLGRP_CGR_Replace( u32 index, const NNSG2dCharacterData* src );

//==============================================================================================
/**
 * �o�^�ς�CGR�̓]���A�h���X�ɕʂ̃f�[�^���Ĕz�u����@�ׂ����w���(VRAM�]���^�͕s�\)
 *
 * @param   index			CGR�f�[�^�̓o�^�C���f�b�N�X
 * @param	  cp_buff   �L�����N�^���f�[�^
 * @param   size      �]���T�C�Y  byte�P��
 * @param   offs      �]���I�t�Z�b�g�i�o�^�ς�CGR���]������Ă���A�h���X����̃I�t�Z�b�g byte�P�ʁj
 * @param   vramType  �]����VramType  
 *
 * vramType��CLSYS_DRAW_MAX��n���ƁA�o�^�ς�CGR�œ]������Ă���`��ʂ��ׂĂɃ��\�[�X��]�����܂��B
 *
 */
//==============================================================================================
extern void GFL_CLGRP_CGR_ReplaceEx( u32 index, const void* cp_buff, u32 size, u32 offs, CLSYS_DRAW_TYPE vramType );


//==============================================================================================
/**
 * �o�^�ς�VRAM�]���^CGR�̓]�����f�[�^����ʂ̃f�[�^�ɍ����ւ���
 *
 * @param   index			CGR�f�[�^�iVRAM�]���^�j�̓o�^�C���f�b�N�X
 * @param   arc				�����ւ����CGR�f�[�^�����A�[�J�C�u�̃n���h��
 * @param   cgrDatIdx		�����ւ����CGR�f�[�^�̃A�[�J�C�u���C���f�b�N�X
 * @param   compressedFlag	�����ւ����CGR�f�[�^�����k����Ă��邩
 * @param	heapID			��Ɨp�q�[�vID
 *
 */
//==============================================================================================
extern void GFL_CLGRP_CGR_ReplaceSrc_VramTransfer( u32 index, ARCHANDLE* arc, u32 cgrDatIdx, BOOL compressedFlag, HEAPID heapID );

//==============================================================================================
/**
 * �o�^�ς�VRAM�]���^CGR�̓]�����f�[�^�|�C���^���擾�i��舵���͐T�d�ɁI�j
 *
 * @param   index			CGR�f�[�^�iVRAM�]���^�j�̓o�^�C���f�b�N�X
 *
 * @retval  void*		�f�[�^�|�C���^
 */
//==============================================================================================
extern void* GFL_CLGRP_CGR_GetSrcPointer_VramTransfer( u32 index );

//==============================================================================================
/**
 * CGR�v���L�V�擾
 *
 * @param   index		[in]  �o�^�C���f�b�N�X
 * @param   proxy		[out] �v���L�V�f�[�^�擾�̂��߂̍\���̃A�h���X
 *
 */
//==============================================================================================
extern void GFL_CLGRP_CGR_GetProxy( u32 index, NNSG2dImageProxy* proxy );

//--------------------------------------------------------------
/**
 * @brief   �o�^���ꂽCGR�f�[�^����]����A�h���X���擾����
 * @param   index		�o�^�C���f�b�N�X
 * @param   vramType	�]����VRAM�w��
 * @retval  �]����I�t�Z�b�g�A�h���X
 */
//--------------------------------------------------------------
extern u32 GFL_CLGRP_CGR_GetAddr( u32 index, CLSYS_DRAW_TYPE vramType );



//-----------------------------------------------------------------------------------------------------------
/**
 *	�p���b�g�f�[�^�֘A����
 *
 *�E�o�^
 *
 *		GFL_CLGRP_PLTT_Register		�i�񈳏k�p���b�g�f�[�^��p�j
 *			�]����I�t�Z�b�g���w��\�B�p���b�gVRAM�e�ʂ𒴂��Ȃ��͈͂őS�]�����܂��B
 *
 *		GFL_CLGRP_PLTT_RegisterEx	�i�񈳏k�p���b�g�f�[�^��p�j
 *			�]����I�t�Z�b�g�ɉ����A�]�����f�[�^�̓ǂݍ��݊J�n�ʒu�A�]���{�����w��ł��܂��B
 *
 *		GFL_CLGRP_PLTT_RegisterComp	�i���k�p���b�g�f�[�^��p�j
 *			NitroCharacter�ŕҏW�����ʂ�̈ʒu�ɓ]�����܂��B
 *			�]����I�t�Z�b�g���w�肵�Ă��炷���Ƃ��o���܂��B
 *
 *�E���
 *		GFL_CLGRP_PLTT_Release
 *
 *�E�v���L�V�擾
 *		GFL_CLGRP_PLTT_GetProxy
 *
 */
//-----------------------------------------------------------------------------------------------------------
//==============================================================================================
/**
 * �񈳏k�p���b�g�f�[�^�̓o�^�����VRAM�ւ̓]��
 * �]����I�t�Z�b�g��0�ȊO�̒l���w�肷��ƁAVRAM�T�C�Y�𒴂��Ȃ��悤�ɓ]���T�C�Y���K�X�A���������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * ��OBJ�g���p���b�g���w�肵�����ꍇ�A byteOffs�̒l�� GFL_CLGRP_EXPLTT_OFFSET + �I�t�Z�b�g�o�C�g�����w�肷��
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 *
 */
//==============================================================================================
extern u32 GFL_CLGRP_PLTT_Register( ARCHANDLE* arcHandle, u32 plttDataID, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID );

//==============================================================================================
/**
 * �񈳏k�p���b�g�f�[�^�̓o�^�����VRAM�ւ̓]���i�g���Łj
 * �]����I�t�Z�b�g�w��̑��ɁA�f�[�^�ǂݏo���J�n�ʒu�Ɠ]���{�����w��ł���B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   srcStartLine	[in] �f�[�^�ǂݏo���J�n�ʒu�i�p���b�g���{�ڂ���]�����邩�H 1�{=16�F�j
 * @param   numTransLines	[in] �]���{���i���{���]�����邩�H 1�{=16�F�^0���ƑS�ē]���j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * ��OBJ�g���p���b�g���w�肵�����ꍇ�A byteOffs�̒l�� GFL_CLGRP_EXPLTT_OFFSET �����Z���Ďw�肷��B
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 *
 */
//==============================================================================================
extern u32 GFL_CLGRP_PLTT_RegisterEx( ARCHANDLE* arcHandle, u32 plttDataID, CLSYS_DRAW_TYPE vramType, u16 byteOffs, u16 srcStartLine, u16 numTransLines, HEAPID heapID );

//=============================================================================================
/**
 * ���k�p���b�g�f�[�^�i-pcm�I�v�V�������w�肵�Đ��������f�[�^�j�̓o�^�����VRAM�ւ̓]��
 * NitroCharacter�ŕҏW�����ʂ��VRAM�z�u�����B
 *�i�ꉞ�A�I�t�Z�b�g���w�肷�邱�ƂŃY�������Ƃ��\�ɂ��Ă���j
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_CLGRP_REGISTER_FAILED�j
 */
//=============================================================================================
extern u32 GFL_CLGRP_PLTT_RegisterComp( ARCHANDLE* arcHandle, u32 plttDataID, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID );

//==============================================================================================
/**
 * �o�^���ꂽ�p���b�g�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_CLGRP_PLTT_Release( u32 index );

//==============================================================================================
/**
 * �o�^�ςݔ񈳏k�^�p���b�g�̓]���A�h���X�ɕʂ̃f�[�^���Ĕz�u����
 *
 * @param   index			PLTT�f�[�^�̓o�^�C���f�b�N�X
 * @param	src				�ǂݍ��ݍς݃p���b�g���\�[�X
 * @param	numTransLines	�]���{���i���{���]�����邩�H 1�{=16�F�^0���ƑS�ē]���j 
 *
 */
//==============================================================================================
extern void GFL_CLGRP_PLTT_Replace( u32 index, const NNSG2dPaletteData* src, u16 numTransLines );

//==============================================================================================
/**
 * �o�^���ꂽ�p���b�g�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_CLGRP_PLTT_GetProxy( u32 index, NNSG2dImagePaletteProxy* proxy );

//--------------------------------------------------------------
/**
 * @brief   �o�^���ꂽ�p���b�g�f�[�^����]����A�h���X���擾����
 * @param   index		�o�^�C���f�b�N�X
 * @param   vramType	�]����VRAM�w��
 * @retval  �]����I�t�Z�b�g�A�h���X
 */
//--------------------------------------------------------------
extern u32 GFL_CLGRP_PLTT_GetAddr( u32 index, CLSYS_DRAW_TYPE vramType );


//-----------------------------------------------------------------------------------------------------------
/**
 *	�Z���A�j���f�[�^�֘A����
 *
 *	�E�o�^
 *		GFL_CLGRP_CELLANIM_Register
 *
 *	�E���
 *		GFL_CLGRP_CELLANIM_Release
 *
 *	�EVRAM�]���^���ǂ�������
 *		GFL_CLGRP_CELLANIM_IsVramTransfer
 *
 */
//-----------------------------------------------------------------------------------------------------------
//==============================================================================================
/**
 * �Z���A�j���f�[�^�o�^
 *
 * @param   arcHandle		�A�[�J�C�u�n���h��
 * @param   cellDataID		�Z���f�[�^ID
 * @param   animDataID		�A�j���f�[�^ID
 * @param   heapID			�f�[�^�ێ��p�q�[�vID
 *
 * @retval  �o�^�C���f�b�N�X
 *
 */
//==============================================================================================
extern u32 GFL_CLGRP_CELLANIM_Register( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, HEAPID heapID );
extern u32 GFL_CLGRP_CELLANIM_RegisterEx( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, BOOL cellCompressedFlag, BOOL animCompressedFlag, HEAPID heapID );

//==============================================================================================
/**
 * �o�^���ꂽ�Z���A�j���f�[�^�̉��
 *
 * @param   index		�o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_CLGRP_CELLANIM_Release( u32 index );

//==============================================================================================
/**
 * �w�肳�ꂽ�Z���f�[�^���AVRAM�]���^���ǂ������`�F�b�N
 *
 * @param   index		�o�^�C���f�b�N�X
 *
 * @retval  BOOL		TRUE�Ȃ�VRAM�]���^�f�[�^
 */
//==============================================================================================
extern BOOL GFL_CLGRP_CELLANIM_IsVramTransfer( u32 index );



//-------------------------------------
///	GFL_CLUNIT�֌W
//
//	GFL_CLWK���܂Ƃ߂�P�̃O���[�v�ł��B
//�@�Q�[���ɂ���ăO���[�v�̕������͕ς��Ǝv���܂����A
//	��Ƃ��ẮA
//	�@�t�B�[����
//	�@�@�@�V�C�O���[�v
//	�@�@�@���j���[���X�g�O���[�v
//	�@�Ƃ킯�A���ꂼ���GFL_CLUNIT���쐬���A�\�����̊Ǘ����s�����Ƃ��ł��܂��B
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�𐶐�
 *
 *	@param	wknum		���[�N��
 *	@param	unit_pri	���j�b�g�`��D�揇�ʁ@�D��� 0,1,2,3,4,5....�D�揬
 *	@param	heapID		�q�[�vID
 *
 *	@return	���j�b�g�|�C���^
 */
//-----------------------------------------------------------------------------
extern GFL_CLUNIT* GFL_CLACT_UNIT_Create( u16 wknum, u8 unit_pri, HEAPID heapID );
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
 *	@brief	�Z���A�N�^�[���j�b�g�ɕ`��t���O��ݒ�
 *	
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	on_off		�`��ON-OFF
 *
 *	TRUE	�\��
 *	FALSE	��\��
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_UNIT_SetDrawEnable( GFL_CLUNIT* p_unit, BOOL on_off );
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
extern BOOL GFL_CLACT_UNIT_GetDrawEnable( const GFL_CLUNIT* cp_unit );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`��D�揇�ʂ�ύX
 *	
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	uni_pri		���j�b�g�`��D�揇�ʁ@�D��� 0,1,2,3,4,5....�D�揬
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_UNIT_SetPri( GFL_CLUNIT* p_unit, u8 uni_pri );
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���j�b�g�̕`��D�揇�ʂ��擾
 *
 *	@param	cp_unit		�Z���A�N�^�[���j�b�g
 *
 *	@retval	�`��D�揇�ʁ@�D��� 0,1,2,3,4,5....�D�揬
 */
//-----------------------------------------------------------------------------
extern u8 GFL_CLACT_UNIT_GetPri( const GFL_CLUNIT* cp_unit );
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

//	CLGRP�̃��\�[�X���g�p�����o�^
extern GFL_CLWK* GFL_CLACT_WK_Create( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_DATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_CLACT_WK_CreateVT( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_DATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_CLACT_WK_CreateAffine( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex,
	const GFL_CLWK_AFFINEDATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_CLACT_WK_CreateVTAffine( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_AFFINEDATA* param, u16 setSerface, HEAPID heapID );


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
extern void GFL_CLACT_WK_SetDrawEnable( GFL_CLWK* p_wk, BOOL on_off );
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
extern BOOL GFL_CLACT_WK_GetDrawEnable( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ݒ�
 *
 *	@param	p_wk			�Z���A�N�^�[���[�N
 *	@param	cp_pos			���W�\����
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 *	                  ( �f�t�H���g�����_���[��ԂȂ� CLSYS_DEFREND_MAIN/CLSYS_DEFREND_SUB )
 *	                  ( �Ǝ������_���[��ԂȂ� �T�[�t�F�[�X�e�[�u���̃C���f�b�N�X )
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
 *	                  ( �f�t�H���g�����_���[��ԂȂ� CLSYS_DEFREND_MAIN/CLSYS_DEFREND_SUB )
 *	                  ( �Ǝ������_���[��ԂȂ� �T�[�t�F�[�X�e�[�u���̃C���f�b�N�X )
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetPos( const GFL_CLWK* cp_wk, GFL_CLACTPOS* p_pos, u16 setsf );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ʐݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	pos			�ݒ�l
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 *	                  ( �f�t�H���g�����_���[��ԂȂ� CLSYS_DEFREND_MAIN/CLSYS_DEFREND_SUB )
 *	                  ( �Ǝ������_���[��ԂȂ� �T�[�t�F�[�X�e�[�u���̃C���f�b�N�X )
 *	@param	type		���W�^�C�v
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetTypePos( GFL_CLWK* p_wk, s16 pos, u16 setsf, CLSYS_MAT_TYPE type );
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�t�F�[�X�����΍��W�ʎ擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	setsf			�T�[�t�F�[�X�C���f�b�N�X
 *	                  ( �f�t�H���g�����_���[��ԂȂ� CLSYS_DEFREND_MAIN/CLSYS_DEFREND_SUB )
 *	                  ( �Ǝ������_���[��ԂȂ� �T�[�t�F�[�X�e�[�u���̃C���f�b�N�X )
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
 *	@param	mode		�I�t�Z�b�g�ݒ胂�[�h
 *
 *	CLWK_PLTTOFFS_MODE_PLTT_TOP
 *		�p���b�g�v���N�V�A�h���X+pal_offs��
 *		�p���b�g�ԍ����g�p����悤�ɂȂ�܂��B
 * 
 *	CLWK_PLTTOFFS_MODE_OAM_COLOR
 *		�p���b�g�v���N�V�A�h���X+pal_offs+OamAttr.c_param��
 *		�p���b�g�ԍ����g�p����悤�ɂȂ�܂��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetPlttOffs( GFL_CLWK* p_wk, u8 pal_offs, CLWK_PLTTOFFS_MODE mode );
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g�ݒ胂�[�h�̎擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�p���b�g�I�t�Z�b�g�ݒ胂�[�h
 *
 *	CLWK_PLTTOFFS_MODE_PLTT_TOP
 *		�p���b�g�v���N�V�A�h���X+pal_offs��
 *		�p���b�g�ԍ����g�p����悤�ɂȂ�܂��B
 * 
 *	CLWK_PLTTOFFS_MODE_OAM_COLOR
 *		�p���b�g�v���N�V�A�h���X+pal_offs+OamAttr.c_param��
 *		�p���b�g�ԍ����g�p����悤�ɂȂ�܂��B
 */
//-----------------------------------------------------------------------------
extern CLWK_PLTTOFFS_MODE GFL_CLACT_WK_GetPlttOffsMode( const GFL_CLWK* cp_wk );
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
 *	@brief	�p���b�g�v���N�V��VRAM�I�t�Z�b�g�A�h���X�擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	type		�`��^�C�v
 *
 *	@retval	CLWK_VRAM_ADDR_NONE�ȊO	�p���b�gVRAM�I�t�Z�b�g�A�h���X
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
 *	@brief	�L�����N�^�f�[�^VRAM�I�t�Z�b�g�A�h���X���擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *	@param	type		�`��^�C�v
 *
 *	@retval	CLWK_VRAM_ADDR_NONE�ȊO	�L�����N�^��VRAM�I�t�Z�b�g�A�h���X
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
 *	@brief	CLGRP CGR�C���f�b�N�X���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@retval CLGRP CGR�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetCgrIndex( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	CLGRP PLTT�C���f�b�N�X���擾
 *
 *	@param	cp_wk			�Z���A�N�^�[���[�N
 *
 *	@retval CLGRP PLTT�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetPlttIndex( const GFL_CLWK* cp_wk );
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
 *	@brief	�A�j���[�V�����C���f�b�N�X��ݒ�
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	idx		  �C���f�b�N�X��
 *	NitroCharacter�̃A�j���[�V�����V�[�P���X���̃R�}�ԍ����w�肵�Ă�������
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmIndex( GFL_CLWK* p_wk, u16 idx );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����C���f�b�N�X�����擾
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����C���f�b�N�X��
 */
//-----------------------------------------------------------------------------
extern u16 GFL_CLACT_WK_GetAnmIndex( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[�������擾
 *	        ���̃t���[���́AAnmIndex���ł̃t���[���ł��B
 *
 *	@param	cp_wk		�Z���A�N�^�[���[�N
 *
 *	@return	�A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------
extern fx32 GFL_CLACT_WK_GetAnmFrame( const GFL_CLWK* cp_wk );
//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����ݒ�
 *	        ���̃t���[���́AAnmIndex���ł̃t���[���ł��B
 *
 *	@param	p_wk		�Z���A�N�^�[���[�N
 *	@param	frame		�t���[����
 *
 *	�}���`�Z���ł́A�g�p�s�\�ł��B
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetAnmFrame( GFL_CLWK* p_wk, fx32 frame );
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


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����́A�\���v�f���擾����B
 *	  ��Multi Cell�@��Ή�
 *
 *	@param	cp_wk         ���[�N
 *	@param	p_element     �G�������g
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_GetAnmElementNow( const GFL_CLWK* cp_wk, GFL_CLWK_ANMELEMENT* p_element );

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����R�[���o�b�N��ݒ�
 *
 *	@param	p_wk         ���[�N
 *	@param  cp_data       �R�[���o�b�N���
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_StartAnmCallBack( GFL_CLWK* p_wk, const GFL_CLWK_ANM_CALLBACK* cp_data );


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����R�[���o�b�N��OFF
 *
 *	@param	p_wk         ���[�N
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_StopAnmCallBack( GFL_CLWK* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V����SRT�A�j���[�V�������f��ONOFF
 *
 *	@param	p_wk         ���[�N
 *	@param  flag         �t���O TRUE�F���f����@FALSE�F���f���Ȃ�
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_WK_SetSRTAnimeFlag( GFL_CLWK* p_wk, BOOL flag );

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V����SRT�A�j���[�V�������f�t���O�̎擾
 *
 *	@param	p_wk         ���[�N
 *	@retval  TRUE�F���f����@FALSE�F���f���Ȃ�
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_CLACT_WK_GetSRTAnimeFlag( const GFL_CLWK* cp_wk );




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
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���@OAMAttr�P�ʂ̃J�����O�ݒ�
 *	        �`�敉�ׂɒ��ӁI
 *
 *	@param	p_rend			���[�U�[��`�����_���[�V�X�e��
 *	@param	flag        TRUE:ON FALSE�FOFF
 */
//-----------------------------------------------------------------------------
extern void GFL_CLACT_USERREND_SetOAMAttrCulling( GFL_CLSYS_REND* p_rend, BOOL flag );
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[��`�����_���[�V�X�e���@OAMAttr�P�ʂ̃J�����O�ݒ�擾
 *
 *	@param	cp_rend			���[�U�[��`�����_���[�V�X�e��
 *
 *	@retval TRUE    ON	
 *	@retval FALSE   OFF
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_CLACT_USERREND_GetOAMAttrCulling( const GFL_CLSYS_REND* cp_rend );


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif		// __CLACT_H__

