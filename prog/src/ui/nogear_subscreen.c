//============================================================================
/**
 *  @file   nogear_subscreen.c
 *  @brief  C�M�A����O�̃T�u�X�N���[���̐����A�j��
 *  @author Koji Kawada
 *  @data   2010.03.04
 *  @note   
 *  ���W���[�����FNOGEAR_SUBSCREEN
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "pm_version.h"
#include "arc_tool.h"

#include "ui/nogear_subscreen.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "c_gear.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// �T�uBG�t���[��
#define BG_FRAME_S_NOGEAR      (GFL_BG_FRAME0_S)

// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_NOGEAR  (0)

// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_NOGEAR        = 0,  // �S�]��
};
// �ʒu
enum
{
  BG_PAL_POS_S_NOGEAR       = 0,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_            = 0,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_            = 0,
};

// �a�f�ݒ�
// BG�O���t�B�b�N���[�h�ݒ�
static const GXBGMode bgModeSub = GX_BGMODE_0; //�T�uBG�ʐݒ�
// BG�ʐݒ�
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
	BOOL								is_visible;
}	sc_bgsetup[]	=
{	
	//SUB---------------------------
	{	
		BG_FRAME_S_NOGEAR,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x8000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, BG_FRAME_PRI_S_NOGEAR, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
};

// C�M�A����O�̃T�u�X�N���[���̃f�[�^
typedef enum
{
  TYPE_MALE,
  TYPE_FEMALE,
  TYPE_MAX,
}
TYPE;
enum
{
  DATA_NCL,
  DATA_NCG,
  DATA_NSC,
  DATA_MAX,
};

static ARCDATID arcdatid[TYPE_MAX][DATA_MAX] =
{
  {
    NARC_c_gear_c_gear_NCLR,
    NARC_c_gear_c_gear_NCGR,
    NARC_c_gear_c_gear01_n_NSCR,
  },
  {
    NARC_c_gear_c_gear2_NCLR,
    NARC_c_gear_c_gear2_NCGR,
    NARC_c_gear_c_gear01_n_NSCR,
  },
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �T�u�X�N���[������������
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void NOGEAR_SUBSCREEN_Init(void)
{
	//���W�X�^������
	G2S_BlendNone();	
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//�f�B�X�v���CON
  GXS_DispOn();
	
  //�\��
	GFL_DISP_GXS_InitVisibleControl();

	//�O���t�B�b�N���[�h�ݒ�
	{	
    GXS_SetGraphicsMode( bgModeSub );
    GFL_DISP_GXS_InitVisibleControlBG();
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

//------------------------------------------------------------------
/**
 *  @brief           �T�u�X�N���[���I������
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void NOGEAR_SUBSCREEN_Exit(void)
{
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//���W�X�^������
	G2S_BlendNone();	
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );
}

//------------------------------------------------------------------
/**
 *  @brief           �T�u�X�N���[���ǂݍ���œ]��
 *
 *  @param[in]       heap_id       �ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
 *  @param[in]       sex           PM_MALE or PM_FEMALE  // include/pm_version.h
 *
 *  @retval          
 */
//------------------------------------------------------------------
void NOGEAR_SUBSCREEN_Trans(
               HEAPID   heap_id,     // �ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
               u8       sex          // PM_MALE or PM_FEMALE  // include/pm_version.h
            )
{
  ARCHANDLE* handle;
  TYPE       type     = ( sex == PM_FEMALE ) ? ( TYPE_FEMALE ) : ( TYPE_MALE );

  // C�M�A����O�̃T�u�X�N���[���̐���
  handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, heap_id );

  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      arcdatid[type][DATA_NCL],
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_NOGEAR * 0x20,
      BG_PAL_NUM_S_NOGEAR * 0x20,
      heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      arcdatid[type][DATA_NCG],
      BG_FRAME_S_NOGEAR,
			0,
      0,  // �S�]��
      FALSE,
      heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      arcdatid[type][DATA_NSC],
      BG_FRAME_S_NOGEAR,
      0,
      0,  // �S�]��
      FALSE,
      heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_S_NOGEAR );
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// 
//=====================================

