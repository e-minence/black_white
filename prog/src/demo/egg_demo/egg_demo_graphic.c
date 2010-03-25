//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		graphic.c
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Toru=Nagihashi
 *	@data		2010.01.25
 *	@version {1.0}
 *	@note 
 *	{	
 *	���T�v
 *	ui_template_graphic�̓t�@�C�����ƃR�s�y���Ďg�����Ƃ��ł���A�O���t�B�b�N�ݒ胂�W���[���ł��B
 *	���\�[�X�ǂݍ��݁ACLWK�쐬���͍s�킸�A�O���t�B�b�N�̐ݒ�̂ݍs���܂��B
 *	��{�I�ɁA.c�㕔�̃f�[�^�̏������������ŁABG,OBJ,G3D�̐ݒ��ύX�ł��A
 *	���񏑂��悤�ȏ������y������ړI�ŏ�����Ă���܂��B
 *
 *	�Ⴆ�΁AGFL_BG_BGCNT_HEADER�̐ݒ��ς��Ă����΁A
 *	����ɐݒ肪�ǂݍ��܂��A�ƌ����������ł��B
 *	
 *	�����g
 *	ui_template_graphic.c�̒��g�́A�f�[�^���ƁA�֐����ɂ킩��Ă���܂��B
 *	�E�ȉ��A�f�[�^�c�Ə����ꂽ�Ƃ��납��A�ȏオ�f�[�^�Ə����ꂽ�Ƃ���܂ł�
 *	�f�[�^���ł��B�i�����_��22�s�`319�s�j
 * 	���̒���ς��邾���ŁA�ȒP�ɕ`��ݒ肪�I���A�Ƃ����̂�_���Ă���܂��B
 *
 *	�E�\���̐錾���牺���֐����ł��B
 *	��{�ύX����K�v�͂���܂��񂪁A�A�v�����Ŏ����D�݂ɕς������Ƃ��́A
 *	���ύX���������B
 *
 *	���g�p���@
 *	�P�D�g�p�������t�H���_�փR�s�[�����l�[�� ui_template, UI_TEMPLATE -> config CONFIG�Ȃ�
 *	�Q�D���ʐݒ�ɂāA�g�p�O���t�B�b�N�̃}�N����ݒ�A�o���N�ݒ�
 *	�R�DBG,OBJ,3D�ȂǁA�g�p����O���t�B�b�N��ݒ�
 *	�S�D�O�����J�֐����g���A�g�p�������\�[�X�Ɛڑ�
 * 	}
 *
 * 	@noto   ui_template_graphic.c���R�s�y���Ė��O��ύX���A����̉�ʂɍ����悤�ɏ����ύX���������ł��B
 *
 * 	���W���[�����FEGG_DEMO_GRAPHIC
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "print/gf_font.h"
#include "system/gfl_use.h"

//�O�����J
#include "egg_demo_graphic.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//
//	�ȉ��A�f�[�^	���̉ӏ���ύX����΁AOK�ł�
//		�������ŃW�����v
//		���ʐݒ�
//		�a�f�ݒ�
//		�n�a�i�ݒ�
//		�R�c�ݒ�
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//=============================================================================
/**
 *					���ʐݒ�
*/
//=============================================================================
//-------------------------------------
///	�g�p�O���t�B�b�N�}�N���ݒ�
//=====================================
#define GRAPHIC_BG_USE	//OFF�ɂ����BG�g�p���܂���
#define GRAPHIC_OBJ_USE	//OFF�ɂ����OBJ�g�p���܂���
#define GRAPHIC_G3D_USE	//OFF�ɂ����3D�g�p���܂���
//-------------------------------------
///	�o���N�ݒ�
//=====================================
static const GFL_DISP_VRAM sc_vramSetTable =
{
	GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_64_E,					// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_16_I,	      // �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_01_BD,						// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_01_FG,				// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_64K,// ���C�����OBJ�}�b�s���O���[�h		
	GX_OBJVRAMMODE_CHAR_1D_32K,// �T�u���OBJ�}�b�s���O���[�h
};

//=============================================================================
/**
 *					�a�f�ݒ�
 *						�EBG�̐ݒ�i�����ݒ�`BG�ʐݒ�j
 *						�EBMPWIN�̏�����
*/
//=============================================================================
#ifdef GRAPHIC_BG_USE
//-------------------------------------
///	BG�O���t�B�b�N���[�h�ݒ�
//=====================================
static const GFL_BG_SYS_HEADER sc_bgsys_header	=
{	
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D	//�O���t�B�b�N���[�h�A���C��BG�ʐݒ�A�T�uBG�ʐݒ�ABG0�ʐݒ�
};
//-------------------------------------
///	BG�ʐݒ�
//=====================================
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
	BOOL								is_visible;
}	sc_bgsetup[]	=
{	
	//MAIN------------------------
#if 0	//3D�ݒ�̂��߂���Ȃ�
	{	
		GFL_BG_FRAME0_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x128,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,	//BG�̎��
		TRUE,	//�����\��
	},
#endif
	{	
		GFL_BG_FRAME1_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME2_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x0800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME3_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},

	//SUB---------------------------
	{	
		GFL_BG_FRAME0_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME1_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME2_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
#if 0	//�g��Ȃ��ꍇ�́A���̃e�[�u����������Ă��������B(#if#endif����Ȃ��Ă����v�ł�)
	{	
		GFL_BG_FRAME3_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
#endif
};

#endif //GRAPHIC_BG_USE
//=============================================================================
/**
 *					�n�a�i�ݒ�
*/
//=============================================================================
#ifdef GRAPHIC_OBJ_USE
//-------------------------------------
///	CLSYS_INIT�f�[�^
//=====================================
static const GFL_CLSYS_INIT sc_clsys_init	=
#if 1	//�㉺��ʂȂ��Ȃ��ꍇ
{	
	0, 0,		//���C���T�[�t�F�[�X�̍���X,Y���W
	0, 512,	//�T�u�T�[�t�F�[�X�̍���X,Y���W
	4, 124,	//���C��OAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	4, 124,	//��bOAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	0,			//�Z��Vram�]���Ǘ���
	32,32,32,32,	//��ׁA��گāA�ٱ�ҁA����ق̓o�^�ł��郊�\�[�X�Ǘ��ő吔
  16, 16,				//���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
};
#else	//�㉺��ʂȂ������ꍇ
{	
	0, 0,		//���C���T�[�t�F�[�X�̍���X,Y���W
	0, 192,	//�T�u�T�[�t�F�[�X�̍���X,Y���W
	4, 124,	//���C��OAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	4, 124,	//��bOAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	0,			//�Z��Vram�]���Ǘ���
	32,32,32,32,	//��ׁA��گāA�ٱ�ҁA����ق̓o�^�ł��郊�\�[�X�Ǘ��ő吔
  16, 16,				//���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
};
#endif
//-------------------------------------
///	���[�N�쐬�ő吔
//=====================================
#define GRAPHIC_OBJ_CLWK_CREATE_MAX	(128)

#endif //GRAPHIC_OBJ_USE
//=============================================================================
/**
 *					�R�c�ݒ�
*/
//=============================================================================
#ifdef GRAPHIC_G3D_USE
//-------------------------------------
///	�e�N�X�`���A��گĂ�VRAM����
//=====================================
#define GRAPHIC_G3D_TEXSIZE	(GFL_G3D_TEX256K)	//�o���N�̃e�N�X�`���C���[�W�X���b�g���ނƂ��킹�Ă�������
#define GRAPHIC_G3D_PLTSIZE	(GFL_G3D_PLT32K)	//�o���N����گăC���[�W�X���b�g���ނƂ��킹�Ă�������

//-------------------------------------
///	�J�����ʒu
//=====================================
//static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,FX32_CONST( 70 ) };	//�ʒu
//static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };					//�����
//static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };	//�^�[�Q�b�g

static const VecFx32 sc_CAMERA_PER_POS    	= { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 30.0f ), FX_F32_TO_FX32( 100.0f ) };//70.0f ) };//3000.0f ) };
static const VecFx32 sc_CAMERA_PER_UP       = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(  1.0f ), FX_F32_TO_FX32(    0.0f ) };
static const VecFx32 sc_CAMERA_PER_TARGET		= { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 30.0f ), FX_F32_TO_FX32(    0.0f ) };

//-------------------------------------
///	�v���W�F�N�V����
//		�v���W�F�N�V�����ׂ̍��������͊e���ł����Ȃ��Ă�������
//=====================================
//���ˉe�J�����B�ˉe�J����
static inline GFL_G3D_CAMERA* GRAPHIC_G3D_CAMERA_Create
		( const VecFx32* cp_pos, const VecFx32* cp_up, const VecFx32* cp_target, HEAPID heapID )
{
#if 1	//�ˉe
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									defaultCameraAspect, 0,
									defaultCameraNear, defaultCameraFar, 0,
									cp_pos, cp_up, cp_target, heapID );
#else	//���ˉe	�N���b�v�ʂ͓K���ł�
	return GFL_G3D_CAMERA_CreateOrtho( 
		// const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			FX32_CONST(96), -FX32_CONST(96), -FX32_CONST(128), FX32_CONST(128),
									defaultCameraNear, defaultCameraFar, 0,
									cp_pos, cp_up, cp_target, heapID );
#endif
}

//-------------------------------------
///	3D�ݒ�R�[���o�b�N�֐�
//=====================================
static void Graphic_3d_SetUp( void )
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	G2_SetBG0Priority(0);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	//	�t�H�O�A�G�b�W�}�[�L���O�A�g�D�[���V�F�[�h�̏ڍאݒ��
	//	�e�[�u�����쐬���Đݒ肵�Ă�������
	G3X_SetShading( GX_SHADING_HIGHLIGHT );	//�V�F�[�f�B���O
	G3X_AntiAlias( FALSE );									//	�A���`�G�C���A�X
	G3X_AlphaTest( FALSE, 0 );							//	�A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );									//	�A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );								//	�G�b�W�}�[�L���O
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );	//�t�H�O

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = 
		{
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
}

#endif //GRAPHIC_G3D_USE
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//
//	�ȏオ�f�[�^�B	�ȉ��͊�{�I�ɏ���������K�v������܂���
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG���[�N
//=====================================
typedef struct 
{
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJ���[�N
//=====================================
typedef struct 
{
	GFL_CLUNIT			*p_clunit;
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	3D�`���
//=====================================
typedef struct 
{
	GFL_G3D_CAMERA		*p_camera;
} GRAPHIC_G3D_WORK;

//-------------------------------------
///	�O���t�B�b�N���C�����[�N
//=====================================
struct _EGG_DEMO_GRAPHIC_WORK
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GRAPHIC_G3D_WORK		g3d;
	GFL_TCB						*p_vblank_task;
};

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	����
//=====================================
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//-------------------------------------
///	BG
//=====================================
#ifdef GRAPHIC_BG_USE
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
#endif //GRAPHIC_BG_USE
//-------------------------------------
///	OBJ
//=====================================
#ifdef GRAPHIC_OBJ_USE
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLUNIT* GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );
#endif //GRAPHIC_OBJ_USE
//-------------------------------------
///	3D
//=====================================
#ifdef GRAPHIC_G3D_USE
static void GRAPHIC_G3D_Init( GRAPHIC_G3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_G3D_Exit( GRAPHIC_G3D_WORK *p_wk );
static void GRAPHIC_G3D_StartDraw( GRAPHIC_G3D_WORK *p_wk );
static void GRAPHIC_G3D_EndDraw( GRAPHIC_G3D_WORK *p_wk );
static GFL_G3D_CAMERA * GRAPHIC_G3D_GetCamera( const GRAPHIC_G3D_WORK *cp_wk );
static void Graphic_3d_SetUp( void );
#endif //GRAPHIC_G3D_USE

//=============================================================================
/**
 *					�O���[�o��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�ݒ�
 *
 *	@param	�f�B�X�v���C�I��	GX_DISP_SELECT_MAIN_SUB or GX_DISP_SELECT_SUB_MAIN
 *	@param	HEAPID heapID �q�[�v
 */
//-----------------------------------------------------------------------------
EGG_DEMO_GRAPHIC_WORK * EGG_DEMO_GRAPHIC_Init( int display_select, HEAPID heapID )
{	
	EGG_DEMO_GRAPHIC_WORK * p_wk;
	p_wk	= GFL_HEAP_AllocMemory(heapID, sizeof(EGG_DEMO_GRAPHIC_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(EGG_DEMO_GRAPHIC_WORK) );

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	//VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	//�f�B�X�v���CON
	GFL_DISP_SetDispSelect( display_select );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();

	//�t�H���g������
	GFL_FONTSYS_Init();

	//���W���[��������
#ifdef GRAPHIC_BG_USE
	GRAPHIC_BG_Init( &p_wk->bg, heapID );
#endif //GRAPHIC_BG_USE
#ifdef GRAPHIC_OBJ_USE
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );
#endif //GRAPHIC_OBJ_USE
#ifdef GRAPHIC_G3D_USE
	GRAPHIC_G3D_Init( &p_wk->g3d, heapID );
#endif //GRAPHIC_G3D_USE

	//VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�j��
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_GRAPHIC_Exit( EGG_DEMO_GRAPHIC_WORK *p_wk )
{	
	//VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���W���[���j��
#ifdef GRAPHIC_G3D_USE
	GRAPHIC_G3D_Exit( &p_wk->g3d );
#endif //GRAPHIC_G3D_USE
#ifdef GRAPHIC_OBJ_USE
	GRAPHIC_OBJ_Exit( &p_wk->obj );
#endif //GRAPHIC_OBJ_USE
#ifdef GRAPHIC_BG_USE
	GRAPHIC_BG_Exit( &p_wk->bg );
#endif //GRAPHIC_BG_USE

	//�f�t�H���g�F�֖߂�
	GFL_FONTSYS_SetDefaultColor();

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(EGG_DEMO_GRAPHIC_WORK) );
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N2D�`�揈��
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_GRAPHIC_2D_Draw( EGG_DEMO_GRAPHIC_WORK *p_wk )
{	
#ifdef GRAPHIC_OBJ_USE
	GRAPHIC_OBJ_Main( &p_wk->obj );
#endif //GRAPHIC_OBJ_USE
#ifdef GRAPHIC_BG_USE
	GRAPHIC_BG_Main( &p_wk->bg );
#endif //GRAPHIC_BG_USE
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N3D�`��J�n����
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_GRAPHIC_3D_StartDraw( EGG_DEMO_GRAPHIC_WORK *p_wk )
{	
#ifdef GRAPHIC_G3D_USE
	GRAPHIC_G3D_StartDraw( &p_wk->g3d );
#endif //GRAPHIC_G3D_USE
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N3D�`��J�n�I������
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_GRAPHIC_3D_EndDraw( EGG_DEMO_GRAPHIC_WORK *p_wk )
{	
#ifdef GRAPHIC_G3D_USE
	GRAPHIC_G3D_EndDraw( &p_wk->g3d );
#endif //GRAPHIC_G3D_USE
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJCLUNIT�̎擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 *
 *	@return	CLUNIT�擾
 */
//-----------------------------------------------------------------------------
GFL_CLUNIT * EGG_DEMO_GRAPHIC_GetClunit( const EGG_DEMO_GRAPHIC_WORK *cp_wk )
{	
#ifdef GRAPHIC_OBJ_USE
	return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
#else
	return NULL;
#endif //GRAPHIC_OBJ_USE
}
//----------------------------------------------------------------------------
/**
 *	@brief	G3D_CAMERA�̎擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 *
 *	@return	CAMERA�擾
 */
//-----------------------------------------------------------------------------
GFL_G3D_CAMERA * EGG_DEMO_GRAPHIC_GetCamera( const EGG_DEMO_GRAPHIC_WORK *cp_wk )
{	
#ifdef GRAPHIC_G3D_USE
	return GRAPHIC_G3D_GetCamera( &cp_wk->g3d );
#else
	return NULL;
#endif //GRAPHIC_G3D_USE
}
//=============================================================================
/**
 *						����
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�NVBLANK�֐�
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{
	EGG_DEMO_GRAPHIC_WORK *p_wk	= p_work;
#ifdef GRAPHIC_BG_USE
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
#endif //GRAPHIC_BG_USE
#ifdef GRAPHIC_OBJ_USE
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
#endif //GRAPHIC_OBJ_USE
}
//=============================================================================
/**
 *						BG
 */
//=============================================================================
#ifdef GRAPHIC_BG_USE
//----------------------------------------------------------------------------
/**
 *	@brief	BG	������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	���[�N
 *	@param	heapID								�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );

	//������
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�O���t�B�b�N���[�h�ݒ�
	{	
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, sc_bgsetup[i].is_visible );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	�j��
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk )
{	
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//�I��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	���C������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk )
{	
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	VBlank�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}
#endif //GRAPHIC_BG_USE
//=============================================================================
/**
 *				OBJ
 */
//=============================================================================
#ifdef GRAPHIC_OBJ_USE
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk			���[�N
 *	@param	GFL_DISP_VRAM* cp_vram_bank	�o���N�e�[�u��
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//�V�X�e���쐬
	GFL_CLACT_SYS_Create( &sc_clsys_init, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( GRAPHIC_OBJ_CLWK_CREATE_MAX, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//�\��
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	�j��
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk )
{	
	//�V�X�e���j��
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
	GFL_CLACT_SYS_Delete();
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	���C������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_Main();
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	V�u�����N����
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��CLUNIT�擾
 *
 *	@param	const GRAPHIC_OBJ_WORK *cp_wk	���[�N
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT* GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}
#endif// GRAPHIC_OBJ_USE
//=============================================================================
/**
 *					GRAPHIC_G3D
 */
//=============================================================================
#ifdef GRAPHIC_G3D_USE
//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̏�����
 *
 *	@param	p_wk			���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_G3D_Init( GRAPHIC_G3D_WORK *p_wk, HEAPID heapID )
{
	GFL_G3D_Init( GFL_G3D_VMANLNK, GRAPHIC_G3D_TEXSIZE,
			GFL_G3D_VMANLNK, GRAPHIC_G3D_PLTSIZE, 0, heapID, Graphic_3d_SetUp );

	p_wk->p_camera = GRAPHIC_G3D_CAMERA_Create( &sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_G3D_Exit( GRAPHIC_G3D_WORK *p_wk )
{
	GFL_G3D_CAMERA_Delete( p_wk->p_camera );
	GFL_G3D_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��J�n
 *	
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_G3D_StartDraw( GRAPHIC_G3D_WORK *p_wk )
{	

	GFL_G3D_DRAW_Start();
	GFL_G3D_CAMERA_Switching( p_wk->p_camera );
	GFL_G3D_DRAW_SetLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��I��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_G3D_EndDraw( GRAPHIC_G3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_End();
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�����擾
 *
 *	@param	const GRAPHIC_G3D_WORK *cp_wk		���[�N
 *
 *	@return	�J����
 */
//-----------------------------------------------------------------------------
static GFL_G3D_CAMERA * GRAPHIC_G3D_GetCamera( const GRAPHIC_G3D_WORK *cp_wk )
{	
	return cp_wk->p_camera;
}
#endif// GRAPHIC_G3D_USE
