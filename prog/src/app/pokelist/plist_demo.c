//======================================================================
/**
 * @file	plist_demo.c
 * @brief	�|�P�������X�g�F�t�H�����`�F���W���o
 * @author	ariizumi
 * @data	10/02/22
 *
 * ���W���[�����FPLIST_DEMO
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "pokelist_gra.naix"

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_demo.h"

#include "pokelist_particle_lst.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define PLIST_DEMO_EFFECT_WORK_SIZE (0x5000)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PLIST_DEMO_InitEffect( PLIST_WORK *work );
static void PLIST_DEMO_TermEffect( PLIST_WORK *work );


//--------------------------------------------------------------
//	�f��������
//--------------------------------------------------------------
void PLIST_DEMO_DemoInit( PLIST_WORK *work )
{
  GF_ASSERT_MSG(work->demoType!=PDT_NONE,"Plz set demotype!\n");
  
  //BG��3D�p�ɐ؂�ւ���
  PLIST_TermBG0_2DMenu( work );
  PLIST_InitBG0_3DParticle( work );

  PLIST_DEMO_InitEffect( work );
	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, 0x3f, 31, 0);
  
  work->mainSeq = PSMS_FORM_CHANGE_MAIN;
}

//--------------------------------------------------------------
//	�f���J��
//--------------------------------------------------------------
void PLIST_DEMO_DemoTerm( PLIST_WORK *work )
{
  PLIST_DEMO_TermEffect( work );
  //3D��BG�p�ɐ؂�ւ���
  PLIST_TermBG0_3DParticle( work );
  PLIST_InitBG0_2DMenu( work );
  work->mainSeq = PSMS_FADEOUT;
}

//--------------------------------------------------------------
//	�f���X�V
//--------------------------------------------------------------
void PLIST_DEMO_DemoMain( PLIST_WORK *work )
{
  GFL_PTC_CalcAll();
  
  //3D�`��  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    GFL_PTC_DrawAll();
  }
  GFL_G3D_DRAW_End();

  //GFL_PTC_Main();

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    work->mainSeq = PSMS_FORM_CHANGE_TERM;
  }
}
  

//--------------------------------------------------------------
//	�G�t�F�N�g�t�@�C���̏�����
//--------------------------------------------------------------
static void PLIST_DEMO_InitEffect( PLIST_WORK *work )
{
  u8 i;
  u8 emmitNum = 0;
  void* effRes;
  u32 fileIdx = NARC_pokelist_gra_demo_sheimi_spa;
  //���\�[�X�ǂݍ���
  if( work->demoType == PDT_GIRATHINA_TO_ORIGIN ||
      work->demoType == PDT_GIRATHINA_TO_NORMAL )
  {
    fileIdx = NARC_pokelist_gra_demo_girathina_spa;
    emmitNum = DEMO_GIRATHINA_SPAMAX;
  }
  else
	if( work->demoType == PDT_SHEIMI_TO_SKY )
  {
    fileIdx = NARC_pokelist_gra_demo_sheimi_spa;
    emmitNum = DEMO_SHEIMI_SPAMAX;
  }
  effRes = GFL_PTC_LoadArcResource( ARCID_POKELIST , fileIdx , work->heapId );
  
  
  //�܂��}�l�[�W���̍쐬
  work->effTempWork = GFL_NET_Align32Alloc( work->heapId , PLIST_DEMO_EFFECT_WORK_SIZE );
  work->ptcWork = GFL_PTC_Create( work->effTempWork,PLIST_DEMO_EFFECT_WORK_SIZE,FALSE,work->heapId);
  
	//���\�[�X�ƃ}�l�[�W���̊֘A�t��
	GFL_PTC_SetResource( work->ptcWork , effRes , FALSE , GFUser_VIntr_GetTCBSYS() );
	
  //�G�~�b�^�[�̍쐬
  {
    GFL_CLACTPOS pos2d;
    VecFx32 pos;
    PLIST_PLATE_GetPlatePosition( work , work->plateWork[work->pokeCursor] , &pos2d );
    pos2d.x = pos2d.x-( (PLIST_PLATE_WIDTH /2)*8 ) + PLIST_PLATE_POKE_POS_X;
    pos2d.y = pos2d.y-( (PLIST_PLATE_HEIGHT/2)*8 ) + PLIST_PLATE_POKE_POS_Y;
    pos.x = FX32_CONST(pos2d.x)/PLIST_DEMO_SCALE;
    pos.y = FX32_CONST(pos2d.y)/PLIST_DEMO_SCALE;
    pos.z = FX32_CONST(64);
    for( i=0;i<emmitNum;i++ )
    {
      GFL_PTC_CreateEmitter( work->ptcWork , i , &pos );
    }
  }
}

//--------------------------------------------------------------
//	�G�t�F�N�g�t�@�C���̊J��
//--------------------------------------------------------------
static void PLIST_DEMO_TermEffect( PLIST_WORK *work )
{
  u8 i;

  GFL_PTC_Delete( work->ptcWork );
  GFL_NET_Align32Free( work->effTempWork );
}
