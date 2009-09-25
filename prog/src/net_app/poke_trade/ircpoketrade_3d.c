//=============================================================================
/**
 * @file	  ircpoketrade_3d.c
 * @bfief	  �|�P���������R�c����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/18
 */
//=============================================================================


#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/monsno_def.h"

#include "trade.naix"

#include "ircpokemontrade.h"
#include "ircpokemontrade_local.h"
#include "msg/msg_poke_trade.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "font/font.naix"    // NARC_font_default_nclr

#include "system/bmp_winframe.h"
#include "savedata/box_savedata.h"
#include "pokeicon/pokeicon.h"

#include "tradedemo.naix"



#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear	    ( 1 << FX32_SHIFT )
#define cameraFar       ( 400 << FX32_SHIFT )


//============================================================================================
/**
 * @brief BG�f�[�^
 */
//============================================================================================
// �D�揇��
enum
{
  PRIORITY_MAIN_BG1,
  PRIORITY_MAIN_BG0,
};




//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light_data[] = {
  { 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light_setup = { light_data, NELEMS(light_data) };
static GFL_G3D_LIGHTSET* g_lightSet;

typedef void (MODEL3D_CAMERA_FUNC)(IRC_POKEMON_TRADE* pWork);
typedef void (MODEL3D_MOVE_FUNC)(IRC_POKEMON_TRADE* pWork,GFL_G3D_OBJSTATUS* pStatus);


typedef struct 
{
  MODEL3D_CAMERA_FUNC* setCamera;
  MODEL3D_MOVE_FUNC* setMove;
} MODEL3D_SET_FUNCS;


//-------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//-------------------------------------------------------------------------------------------
static void _demoInit( HEAPID heap_id )
{
  //VRAM�ݒ�
  //	GFL_DISP_SetBank(&sc_VRAM_param );

  // BGL������
  //	GFL_BG_Init( heap_id );	// �V�X�e��������
  //	GFL_BG_SetBGMode( &sc_BG_mode );	  // BG���[�h�ݒ�
  //	GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &sc_BGCNT1_M, GFL_BG_MODE_TEXT );	// MAIN_BG1�̐ݒ�
  //	GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );						// MAIN_BG0��\��ON
  //	GFL_BG_SetBGControl3D( PRIORITY_MAIN_BG0 );		              // 3D�ʂ̕\���D�揇�ʂ�ݒ�

  // 3D�V�X�e����������
  // GFL_G3D_Init(
  //   GFL_G3D_VMANLNK, GFL_G3D_TEX128K,
  //    GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0x1000, heap_id, NULL );

  // ���C�g�쐬
  g_lightSet = GFL_G3D_LIGHT_Create( &light_setup, heap_id );
  GFL_G3D_LIGHT_Switching( g_lightSet );

  // �J���������ݒ�
  {
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 };
    proj.param1 = FX_SinIdx( cameraPerspway );
    proj.param2 = FX_CosIdx( cameraPerspway );
    GFL_G3D_SetSystemProjection( &proj );
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �I������
 */
//-------------------------------------------------------------------------------------------
static void _demoExit()
{
  // ���C�g�Z�b�g�j��
  GFL_G3D_LIGHT_Delete( g_lightSet );

  // 3D�V�X�e���I��
  GFL_G3D_Exit();

  // BGL�j��
  GFL_BG_Exit();
}


//============================================================================================
/**
 * @brief 3D�f�[�^
 */
//============================================================================================

static const GFL_G3D_UTIL_RES res_table_reel[] =
{
  { ARCID_POKETRADE,
    NARC_trade_box_001_nsbmd,
    GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_trade1[] =
{
  { ARCID_POKETRADEDEMO,
    NARC_tradedemo_trade_nsbmd,
    GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_ANM anm_table_ball[] =
{
  { 1, 0 },
  { 2, 0 },
};
static const GFL_G3D_UTIL_OBJ obj_table_ball[] =
{
  {
    0,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                         // �e�N�X�`�����\�[�XID
    NULL,
    0,
    //   anm_table_ball,           // �A�j���e�[�u��(�����w��̂���)
    //   NELEMS(anm_table_ball),   // �A�j�����\�[�X��
  },
};

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_reel, NELEMS(res_table_reel), obj_table_ball, NELEMS(obj_table_ball) },
  { res_table_trade1, NELEMS(res_table_trade1), obj_table_ball, NELEMS(obj_table_ball) },
};


static void _cameraSetReel(IRC_POKEMON_TRADE* pWork)
{
  VecFx32 campos;
  VecFx32 tarpos;

  campos.x = 0*FX32_ONE;
  campos.y = FX32_ONE*43;
  campos.z = 241*FX32_ONE;
  tarpos.x = 0;
  tarpos.y = FX32_ONE*43;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
  GFL_G3D_CAMERA_SetFar( pWork->camera, &campos.z );
  tarpos.x = 0;
  tarpos.y = FX32_ONE;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetCamUp( pWork->camera, &tarpos );
}

static void _cameraSetTrade01(IRC_POKEMON_TRADE* pWork)
{
  VecFx32 campos;
  VecFx32 tarpos;

  campos.x = 0*FX32_ONE;
  campos.y = 0;
  campos.z = 241*FX32_ONE;
  tarpos.x = 0;
  tarpos.y = 0;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
  GFL_G3D_CAMERA_SetFar( pWork->camera, &campos.z );
  tarpos.x = 0;
  tarpos.y = FX32_ONE;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetCamUp( pWork->camera, &tarpos );
}


static void _moveSetReel(IRC_POKEMON_TRADE* pWork,GFL_G3D_OBJSTATUS* pStatus)
{
  int a;

  VEC_Set( &pStatus->trans, 0, 0, 0 );
  VEC_Set( &pStatus->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &pStatus->rotate );

  a = -pWork->FriendBoxScrollNum * 65535 / 2976;

  MTX_RotY33(	&pStatus->rotate,FX_SinIdx((u16)a),FX_CosIdx((u16)a));
  // �J�����X�V
  //  ICA_ANIME_SetCameraStatus( work->icaAnime, work->camera );
  GFL_G3D_CAMERA_Switching(pWork->camera );



  {
    // �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
    G3X_SetShading( GX_SHADING_TOON); //GX_SHADING_HIGHLIGHT );
    G3X_AntiAlias( FALSE );
    G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
    G3X_AlphaBlend( FALSE );		// �A���t�@�u�����h�@�I��
    G3X_EdgeMarking( FALSE );
    G3X_SetFog( TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

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
  }
}


static const MODEL3D_SET_FUNCS modelset[] =
{
  { _cameraSetReel,_moveSetReel },
  { _cameraSetTrade01,_moveSetReel },
};





//============================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//============================================================================================
static void Initialize( IRC_POKEMON_TRADE* work,int no );
static void Finalize( IRC_POKEMON_TRADE* work );
static void Draw( IRC_POKEMON_TRADE* work );


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
void IRC_POKETRADEDEMO_Init( IRC_POKEMON_TRADE* pWork )
{
  VecFx32 campos;
  VecFx32 tarpos;

  // ����������
  _demoInit( pWork->heapID );
  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  pWork->g3dUtil = GFL_G3D_UTIL_Create( 10, 10, pWork->heapID );

  Finalize(pWork);

  Initialize( pWork, REEL_PANEL_OBJECT);
  pWork->modelno = REEL_PANEL_OBJECT;

  modelset[pWork->modelno].setCamera(pWork);

}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
void IRC_POKETRADEDEMO_Main( IRC_POKEMON_TRADE* pWork )
{
  IRC_POKEMON_TRADE* work = pWork;
  BOOL end = FALSE;
  VecFx32 campos;
  VecFx32 tarpos;



  Draw( work );
}


//============================================================================================
/**
 * @brief �I���֐�
 */
//============================================================================================
void IRC_POKETRADEDEMO_End( IRC_POKEMON_TRADE* pWork )
{
  IRC_POKEMON_TRADE* work = pWork;

  // �I������
  //  ICA_ANIME_Delete( work->icaAnime );
  Finalize( work );
  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( work->g3dUtil );

  _demoExit();

}

//============================================================================================
/**
 * @brief ���f�����Z�b�g���Ȃ����֐�
 */
//============================================================================================
void IRC_POKETRADEDEMO_SetModel( IRC_POKEMON_TRADE* pWork, int modelno)
{

  Finalize(pWork);
  Initialize( pWork, REEL_PANEL_OBJECT);

  pWork->modelno = modelno;

  modelset[pWork->modelno].setCamera(pWork);

}

//============================================================================================
/**
 * @brief ���f���������֐�
 */
//============================================================================================
void IRC_POKETRADEDEMO_RemoveModel( IRC_POKEMON_TRADE* pWork)
{
  Finalize(pWork);
}



//============================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @breif ������
 */
//--------------------------------------------------------------------------------------------
static void Initialize( IRC_POKEMON_TRADE* work, int modelno )
{

  // ���j�b�g�ǉ�
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      if(work->unitIndex[i]==NULL)
      {
        work->unitIndex[i] = GFL_G3D_UTIL_AddUnit( work->g3dUtil, &setup[modelno] );
        break;
      }
    }
  }

  // �A�j���[�V�����L����
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // ica�f�[�^�����[�h
  //  work->icaAnime = ICA_ANIME_CreateStreamingAlloc(
  //      work->heapID, ARCID_POKETRADEDEMO, NARC_debug_obata_ica_test_data2_bin, 10 );

  // �J�����쐬
  if(work->camera==NULL)
  {
    VecFx32    pos = { 0, 0, 0 };
    VecFx32 target = { 0, 0, 0 };
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, work->heapID );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �I��
 */
//--------------------------------------------------------------------------------------------
static void Finalize( IRC_POKEMON_TRADE* work )
{
  // �J�����j��
  if(work->camera)
  {
    GFL_G3D_CAMERA_Delete( work->camera );
  }

  // ���j�b�g�j��
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      if(work->unitIndex[i])
      {
        GFL_G3D_UTIL_DelUnit( work->g3dUtil, work->unitIndex[i] );
        work->unitIndex[i] = NULL;
      }
    }
  }

}


//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//--------------------------------------------------------------------------------------------
static void Draw( IRC_POKEMON_TRADE* work )
{
  static fx32 frame = 0;
  static fx32 anime_speed = FX32_ONE;
  GFL_G3D_OBJSTATUS status;


  modelset[work->modelno].setMove(work, &status);

  // TEMP: �J�����ݒ�
  //  {
  //    fx32 far = FX32_ONE * 0;
  //    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  //  }

  // �A�j���[�V�����X�V
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
  }

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
  GFL_G3D_DRAW_End();

  frame += anime_speed;
  //  ICA_ANIME_IncAnimeFrame( work->icaAnime, anime_speed );
}
