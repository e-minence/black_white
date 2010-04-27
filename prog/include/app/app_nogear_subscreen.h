//============================================================================
/**
 *  @file   app_nogear_subscreen.h
 *  @brief  C�M�A����O�̃T�u�X�N���[��
 *  @author Koji Kawada
 *  @data   2010.03.04
 *  @note   �C�����C���֐��ł��B
 *
 *  ���W���[�����FAPP_NOGEAR_SUBSCREEN, APP_NOGEAR_SS
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "pm_version.h"
#include "arc_tool.h"

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
#define APP_NOGEAR_SS_BG_FRAME_S_NOGEAR      (GFL_BG_FRAME0_S)

// �T�uBG�t���[���̃v���C�I���e�B
#define APP_NOGEAR_SS_BG_FRAME_PRI_S_NOGEAR  (0)

// �T�uBG�p���b�g
// �{��
enum
{
  APP_NOGEAR_SS_BG_PAL_NUM_S_NOGEAR       = 0,  // �S�]��
};
// �ʒu
enum
{
  APP_NOGEAR_SS_BG_PAL_POS_S_NOGEAR       = 0,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  APP_NOGEAR_SS_OBJ_PAL_NUM_S_            = 0,
};
// �ʒu
enum
{
  APP_NOGEAR_SS_OBJ_PAL_POS_S_            = 0,
};

// �a�f�ݒ�
// BG�O���t�B�b�N���[�h�ݒ�
static const GXBGMode APP_NOGEAR_SS_bgModeSub = GX_BGMODE_0; //�T�uBG�ʐݒ�
// BG�ʐݒ�
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
	BOOL								is_visible;
}	APP_NOGEAR_SS_sc_bgsetup[]	=
{	
	//SUB---------------------------
	{	
		APP_NOGEAR_SS_BG_FRAME_S_NOGEAR,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x8000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, APP_NOGEAR_SS_BG_FRAME_PRI_S_NOGEAR, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
};

// C�M�A����O�̃T�u�X�N���[���̃f�[�^
typedef enum
{
  APP_NOGEAR_SS_TYPE_MALE,
  APP_NOGEAR_SS_TYPE_FEMALE,
  APP_NOGEAR_SS_TYPE_MAX,
}
APP_NOGEAR_SS_TYPE;
enum
{
  APP_NOGEAR_SS_DATA_NCL,
  APP_NOGEAR_SS_DATA_NCG,
  APP_NOGEAR_SS_DATA_NSC,
  APP_NOGEAR_SS_DATA_MAX,
};

static ARCDATID APP_NOGEAR_SS_arcdatid[APP_NOGEAR_SS_TYPE_MAX][APP_NOGEAR_SS_DATA_MAX] =
{
  {
    NARC_c_gear_c_gear_m_NCLR,
    NARC_c_gear_c_gear_m1_NCGR,
    NARC_c_gear_c_gear01_n_NSCR,
  },
  {
    NARC_c_gear_c_gear_f_NCLR,
    NARC_c_gear_c_gear_f1_NCGR,
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
*  �C�����C���֐���`
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
static inline void APP_NOGEAR_SUBSCREEN_Init(void)
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
    GXS_SetGraphicsMode( APP_NOGEAR_SS_bgModeSub );
    GFL_DISP_GXS_InitVisibleControlBG();
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < NELEMS(APP_NOGEAR_SS_sc_bgsetup); i++ )
		{	
			GFL_BG_SetBGControl( APP_NOGEAR_SS_sc_bgsetup[i].frame, &APP_NOGEAR_SS_sc_bgsetup[i].bgcnt_header, APP_NOGEAR_SS_sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( APP_NOGEAR_SS_sc_bgsetup[i].frame );
			GFL_BG_SetVisible( APP_NOGEAR_SS_sc_bgsetup[i].frame, APP_NOGEAR_SS_sc_bgsetup[i].is_visible );
		}
	}

  // OBJ
  {
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
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
static inline void APP_NOGEAR_SUBSCREEN_Exit(void)
{
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < NELEMS(APP_NOGEAR_SS_sc_bgsetup); i++ )
		{	
			GFL_BG_FreeBGControl( APP_NOGEAR_SS_sc_bgsetup[i].frame );
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
static inline void APP_NOGEAR_SUBSCREEN_Trans(
                        HEAPID   heap_id,     // �ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
                        u8       sex          // PM_MALE or PM_FEMALE  // include/pm_version.h
                   )
{
  ARCHANDLE*               handle;
  APP_NOGEAR_SS_TYPE       type     = ( sex == PM_FEMALE ) ? ( APP_NOGEAR_SS_TYPE_FEMALE ) : ( APP_NOGEAR_SS_TYPE_MALE );

  // C�M�A����O�̃T�u�X�N���[���̐���
  handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, heap_id );

  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      APP_NOGEAR_SS_arcdatid[type][APP_NOGEAR_SS_DATA_NCL],
      PALTYPE_SUB_BG,
      APP_NOGEAR_SS_BG_PAL_POS_S_NOGEAR * 0x20,
      APP_NOGEAR_SS_BG_PAL_NUM_S_NOGEAR * 0x20,
      heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      APP_NOGEAR_SS_arcdatid[type][APP_NOGEAR_SS_DATA_NCG],
      APP_NOGEAR_SS_BG_FRAME_S_NOGEAR,
			0,
      0,  // �S�]��
      FALSE,
      heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      APP_NOGEAR_SS_arcdatid[type][APP_NOGEAR_SS_DATA_NSC],
      APP_NOGEAR_SS_BG_FRAME_S_NOGEAR,
      0,
      0,  // �S�]��
      FALSE,
      heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( APP_NOGEAR_SS_BG_FRAME_S_NOGEAR );
}

