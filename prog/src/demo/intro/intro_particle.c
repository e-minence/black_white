//=============================================================================
/**
 *
 *	@file		intro_particle.c
 *	@brief  �C���g�� �p�[�e�B�N������
 *	@author	hosaka genya
 *	@data		2009.12.21
 *
 */
//=============================================================================

#include <gflib.h>

//�A�[�J�C�u
#include "arc_def.h" // for ARCID_XXX

#include "particle/wazaeffect/spa.naix" // for NARC_Spa_be_m_ball_00_open1_spa

#include "intro_graphic.h"

#include "intro_particle.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//--------------------------------------------------------------
///	�C���g���p�p�[�e�B�N��
//==============================================================
struct _INTRO_PARTICLE_WORK {
  // [IN]
  HEAPID               heap_id;
  INTRO_GRAPHIC_WORK*  graphic;
  // [PRIVATE]
  GFL_PTC_PTR     ptc;
  u8              spa_work[ PARTICLE_LIB_HEAP_SIZE ];
};

//-----------------------------------------------------------------------------
/**
 *	@brief  �p�[�e�B�N�� ����
 *
 *	@param	INTRO_GRAPHIC_WORK* graphic
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static VecFx32 sc_camera_eye = { 0, 0, FX32_CONST(70), };
static VecFx32 sc_camera_up = { 0, FX32_ONE, 0 };
static VecFx32 sc_camera_at = { 0, 0, -FX32_ONE };

INTRO_PARTICLE_WORK* INTRO_PARTICLE_Create( INTRO_GRAPHIC_WORK* graphic, HEAPID heap_id )
{
  INTRO_PARTICLE_WORK* wk;

  GFL_PTC_Init( heap_id );
  
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INTRO_PARTICLE_WORK) );
  wk->ptc     = GFL_PTC_Create( wk->spa_work, sizeof(wk->spa_work), TRUE, heap_id );
  wk->graphic = graphic;
  wk->heap_id = heap_id;
	
  /* �p�[�e�B�N���̃J�����𐳎ˉe�ɐݒ� */	
	{
		GFL_G3D_PROJECTION projection; 
		projection.type = GFL_G3D_PRJORTH;
		projection.param1 = FX32_CONST(4);  //(GX_LCD_SIZE_Y-1)*48;
		projection.param2 = -FX32_CONST(4); //0;
		projection.param3 = -FX32_CONST(3); //0;
		projection.param4 = FX32_CONST(3);  //(GX_LCD_SIZE_X-1)*48;
		projection.near = FX32_ONE * 1;
		projection.far  = FX32_CONST( 1024 );	// ���ˉe�Ȃ̂Ŋ֌W�Ȃ����A�O�̂��߃N���b�v��far��ݒ�
		projection.scaleW = FX32_ONE;//0
		GFL_PTC_PersonalCameraDelete( wk->ptc );
		GFL_PTC_PersonalCameraCreate( wk->ptc, &projection, DEFAULT_PERSP_WAY, &sc_camera_eye, &sc_camera_up, &sc_camera_at, wk->heap_id );
	}

  //@TODO �L���E�`�Ń{�[�����\�[�X�����[�h
  {
    void *res;
    res = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_be_ball_normal_1_spa, wk->heap_id );
    GFL_PTC_SetResource( wk->ptc, res, TRUE, NULL );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �p�[�e�B�N�� �㏈��
 *
 *	@param	INTRO_PARTICLE_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_PARTICLE_Exit( INTRO_PARTICLE_WORK* wk )
{
  GFL_PTC_Exit();
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �p�[�e�B�N�� �又��
 *
 *	@param	INTRO_PARTICLE_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_PARTICLE_Main( INTRO_PARTICLE_WORK* wk )
{
  GFL_PTC_Main();
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �����X�^�[�{�[���̃G�~�b�^�𐶐�
 *
 *	@param	INTRO_PARTICLE_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_PARTICLE_CreateEmitterMonsterBall( INTRO_PARTICLE_WORK* wk, fx32 px, fx32 py, fx32 pz )
{
  VecFx32 pos = { 
    0,0,0,	
  };

  pos.x = px;
  pos.y = py;
  pos.z = pz;

//  pos.x = -FX32_CONST(2.0);//GFL_STD_MtRand(256) - 128;
//  pos.y = -FX32_CONST(2.0);//GFL_STD_MtRand(192) - 96;
//  pos.z = 0;//GFL_STD_MtRand(64);	

//  if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    GFL_PTC_CreateEmitter( wk->ptc, 0, &pos );
    GFL_PTC_CreateEmitter( wk->ptc, 1, &pos );
    GFL_PTC_CreateEmitter( wk->ptc, 2, &pos );
  }
}





