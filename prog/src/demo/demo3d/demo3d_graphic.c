//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		demo3d_graphic.c
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	genya hosaka
 *	@data		2011.11.27
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "print/gf_font.h"
 
#include "demo3d_data.h" // for Demo3D_DATA_XXX

//�O�����J
#include "demo3d_graphic.h"
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
///	�o���N�ݒ�
//=====================================
static const GFL_DISP_VRAM sc_vramSetTable[2] =
{
  // �ʏ펞�̐ݒ�
  {
    GX_VRAM_BG_16_F,						// ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_16_G,					// ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_128_D,	      // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_01_AB,						// �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0123_E,				// �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_128K,// ���C�����OBJ�}�b�s���O���[�h		
    GX_OBJVRAMMODE_CHAR_1D_32K,// �T�u���OBJ�}�b�s���O���[�h
  },
  // 2���3D���[�h�̐ݒ�
  // �L���v�`����VRAM-C,D���g�����Ƃ��l�����ĊO���Ă���B
  {
    GX_VRAM_BG_16_F,						// ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_32_H,				// �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_16_G,					// ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,	      // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_01_AB,					// �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0123_E,		// �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_128K,// ���C�����OBJ�}�b�s���O���[�h		
    GX_OBJVRAMMODE_CHAR_1D_32K,// �T�u���OBJ�}�b�s���O���[�h
  },
};


//=============================================================================
/**
 *					�a�f�ݒ�
 *						�EBG�̐ݒ�i�����ݒ�`BG�ʐݒ�j
 *						�EBMPWIN�̏�����
*/
//=============================================================================
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
	{	
		GFL_BG_FRAME1_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x8000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
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
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ256_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
};

//=============================================================================
/**
 *					�n�a�i�ݒ�
*/
//=============================================================================
//-------------------------------------
///	CLSYS_INIT�f�[�^
//=====================================
static const GFL_CLSYS_INIT sc_clsys_init	=
#if 1	//�㉺��ʂȂ��Ȃ��ꍇ
{	
	0, 0,		//���C���T�[�t�F�[�X�̍���X,Y���W
	0, 512,	//�T�u�T�[�t�F�[�X�̍���X,Y���W
	4, 124,	//���C��OAM�Ǘ��J�n,��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	4, 124-12,	//�T�uOAM�Ǘ��J�n,��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă�������) (2��ʕ\���̃L���v�`���[�Ɍ�납��12��OAM���g���Ă��܂��B)
	0,			//�Z��Vram�]���Ǘ���
	32,32,32,32,	//��ׁA��گāA�ٱ�ҁA����ق̓o�^�ł��郊�\�[�X�Ǘ��ő吔
  16, 16,				//���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
};
#else	//�㉺��ʂȂ������ꍇ
{	
	0, 0,		//���C���T�[�t�F�[�X�̍���X,Y���W
	0, 192,	//�T�u�T�[�t�F�[�X�̍���X,Y���W
	4, 124,	//���C��OAM�Ǘ��J�n,��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	4, 124-12,	//�T�uOAM�Ǘ��J�n,��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j (2��ʕ\���̃L���v�`���[�Ɍ�납��12��OAM���g���Ă��܂��B)
	0,			//�Z��Vram�]���Ǘ���
	32,32,32,32,	//��ׁA��گāA�ٱ�ҁA����ق̓o�^�ł��郊�\�[�X�Ǘ��ő吔
  16, 16,				//���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
};
#endif
//-------------------------------------
///	���[�N�쐬�ő吔
//=====================================
#define GRAPHIC_OBJ_CLWK_CREATE_MAX	(32)


//=============================================================================
/**
 *					�R�c�ݒ�
*/
//=============================================================================
//-------------------------------------
///	�e�N�X�`���A��گĂ�VRAM����
//=====================================
#define GRAPHIC_G3D_TEXSIZE	(GFL_G3D_TEX256K)	//�o���N�̃e�N�X�`���C���[�W�X���b�g���ނƂ��킹�Ă�������
#define GRAPHIC_G3D_PLTSIZE	(GFL_G3D_PLT64K)	//�o���N����گăC���[�W�X���b�g���ނƂ��킹�Ă�������

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
  GFL_G3D_LIGHTSET  *p_lightset;
} GRAPHIC_G3D_WORK;

//-------------------------------------
///	�O���t�B�b�N���C�����[�N
//=====================================
struct _DEMO3D_GRAPHIC_WORK
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GRAPHIC_G3D_WORK		g3d;
	GFL_TCB						*p_vblank_task;
  BOOL is_double;
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
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLUNIT* GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );

//-------------------------------------
///	3D
//=====================================
static void GRAPHIC_G3D_Init( GRAPHIC_G3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_G3D_Exit( GRAPHIC_G3D_WORK *p_wk );
static void GRAPHIC_G3D_StartDraw( GRAPHIC_G3D_WORK *p_wk );
static void GRAPHIC_G3D_EndDraw( GRAPHIC_G3D_WORK *p_wk );
static void Graphic_3d_SetUp( void );

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
DEMO3D_GRAPHIC_WORK * DEMO3D_GRAPHIC_Init( int display_select, DEMO3D_ID demo_id, HEAPID heapID )
{	
	DEMO3D_GRAPHIC_WORK * p_wk;
	p_wk	= GFL_HEAP_AllocMemory(heapID, sizeof(DEMO3D_GRAPHIC_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(DEMO3D_GRAPHIC_WORK) );

  p_wk->is_double = Demo3D_DATA_GetDoubleFlag( demo_id );

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	//VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable[ p_wk->is_double ] );

	//�f�B�X�v���CON
	GFL_DISP_SetDispSelect( display_select );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();

	//�t�H���g������
	GFL_FONTSYS_Init();

	//���W���[��������
  
  // �_�u�����[�h��BG/OBJ���g��Ȃ�
  if( p_wk->is_double == FALSE )
  {
    GRAPHIC_BG_Init( &p_wk->bg, heapID );
    GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable[0], heapID );

    //VBlankTask�o�^
    p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
  }

	GRAPHIC_G3D_Init( &p_wk->g3d, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�j��
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEMO3D_GRAPHIC_Exit( DEMO3D_GRAPHIC_WORK *p_wk )
{	
  if( p_wk->is_double == FALSE )
  {
    //VBLANKTask����
    GFL_TCB_DeleteTask( p_wk->p_vblank_task );
  }

	//���W���[���j��
	GRAPHIC_G3D_Exit( &p_wk->g3d );

  // �_�u�����[�h��BG/OBJ���g��Ȃ�
  if( p_wk->is_double == FALSE )
  {
    GRAPHIC_OBJ_Exit( &p_wk->obj );
    GRAPHIC_BG_Exit( &p_wk->bg );
  }

	//�f�t�H���g�F�֖߂�
	GFL_FONTSYS_SetDefaultColor();

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(DEMO3D_GRAPHIC_WORK) );
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N2D�`�揈��
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEMO3D_GRAPHIC_2D_Draw( DEMO3D_GRAPHIC_WORK *p_wk )
{	
  // �_�u�����[�h��BG/OBJ���g��Ȃ�
  if( p_wk->is_double == FALSE )
  {
    GRAPHIC_OBJ_Main( &p_wk->obj );
    GRAPHIC_BG_Main( &p_wk->bg );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N3D�`��J�n����
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEMO3D_GRAPHIC_3D_StartDraw( DEMO3D_GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_G3D_StartDraw( &p_wk->g3d );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N3D�`��J�n�I������
 *
 *	@param	GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEMO3D_GRAPHIC_3D_EndDraw( DEMO3D_GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_G3D_EndDraw( &p_wk->g3d );
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
GFL_CLUNIT * DEMO3D_GRAPHIC_GetClunit( const DEMO3D_GRAPHIC_WORK *cp_wk )
{	
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->is_double == FALSE );

	return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f���V�[����3D�p�����[�^�ݒ�
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 */
//-----------------------------------------------------------------------------
void DEMO3D_GRAPHIC_Scene3DParamSet( DEMO3D_GRAPHIC_WORK *p_wk, const FIELD_LIGHT_STATUS* f_light, DEMO3D_3DSCENE_PARAM* prm )
{
  GRAPHIC_G3D_WORK* g3d = &p_wk->g3d;

  //���C�g�Đݒ�
	GFL_G3D_LIGHT_SetColor( g3d->p_lightset, 0, (u16*)&f_light->light);
	GFL_G3D_LIGHT_SetColor( g3d->p_lightset, 1, (u16*)&f_light->light1);
	GFL_G3D_LIGHT_SetColor( g3d->p_lightset, 2, (u16*)&f_light->light2);
	GFL_G3D_LIGHT_SetColor( g3d->p_lightset, 3, (u16*)&f_light->light3);
  GFL_G3D_LIGHT_Switching( g3d->p_lightset );

#if 0
	// �t�H�O�Z�b�g�A�b�v
  G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );
  G3X_SetFogColor(FIELD_DEFAULT_FOG_COLOR, 0);
  G3X_SetFogTable(fldmapdata_fogColorTable);
	
	// �N���A�J���[�̐ݒ�
	//color,alpha,depth,polygonID,fog
	G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,0,FALSE);

	G3X_SetEdgeColorTable( fldmapdata_edgeColorTable ); 
	G3X_EdgeMarking( FALSE );
#endif
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
	DEMO3D_GRAPHIC_WORK *p_wk	= p_work;

  if( p_wk->is_double == FALSE )
  {
    GRAPHIC_BG_VBlankFunction( &p_wk->bg );
    GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
  }
}
//=============================================================================
/**
 *						BG
 */
//=============================================================================
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

//=============================================================================
/**
 *				OBJ
 */
//=============================================================================
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

//=============================================================================
/**
 *					GRAPHIC_G3D
 */
//=============================================================================
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

  // ���C�g�쐬
  {
    //���C�g�����ݒ�f�[�^
    static const GFL_G3D_LIGHT_DATA light_data[] = {
      { 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
      { 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
      { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
      { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
    };

    static const GFL_G3D_LIGHTSET_SETUP light_setup = { light_data, NELEMS(light_data) };

    p_wk->p_lightset = GFL_G3D_LIGHT_Create( &light_setup, heapID );
    GFL_G3D_LIGHT_Switching( p_wk->p_lightset );
  }

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
  // ���C�g�j��
  GFL_G3D_LIGHT_Delete( p_wk->p_lightset );

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

