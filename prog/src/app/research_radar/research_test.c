/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������� ( ���j���[��� )
 * @file   research_test.c
 * @author obata
 * @date   2010.02.03
 *
 * ��������
 * �@�A���`�G�C���A�XON
 * �@�G�b�W�}�[�L���OON
 * �@�~�O���t�̊e��`�͈قȂ�z�l�ŕ`�悷��i�G�b�W�}�[�L���O�̕`��Ƀf�B�v�X�l���֌W���Ă��邽�߁j
 * �@�~�O���t�̎�O�Ƀ��C����`�悷��
 *
 * �@���S�_�ɂ́A�|���S���̐��������_�����݂��Ă��邽�߁A
 * �@1���C�����̃|���S�����������Ȃ�B
 * �@�ˏ������ׂ��傫���B
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <math.h>
#include <gflib.h>
#include "research_test.h"

#include "arc/arc_def.h"  // for ARCID_SEASON_DISPLAY
#include "arc/season_display.naix"  // for datid


//===============================================================================
// ���萔
//===============================================================================
// �����V�[�P���X
typedef enum{
  RESEARCH_TEST_SEQ_SETUP,    // ����
  RESEARCH_TEST_SEQ_MAIN,     // ���C��
  RESEARCH_TEST_SEQ_CLEAN_UP, // ��Еt��
  RESEARCH_TEST_SEQ_FINISH,   // �I��
} RESEARCH_TEST_SEQ;

// BG �V�X�e�� �w�b�_
static const GFL_BG_SYS_HEADER BGSysHeader = 
{
	GX_DISPMODE_GRAPHICS,   // �\�����[�h�w��
  GX_BGMODE_0,            // �a�f���[�h�w��(���C���X�N���[��)
  GX_BGMODE_0,            // �a�f���[�h�w��(�T�u�X�N���[��)
  GX_BG0_AS_3D            // �a�f�O�̂Q�c�A�R�c���[�h�I��
};

// BG �R���g���[�� �w�b�_
static const GFL_BG_BGCNT_HEADER BGCnt3M = 
{
  0, 0,					          // �����\���ʒu
  0x800,						      // �X�N���[���o�b�t�@�T�C�Y
  0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,			// �J���[���[�h
  GX_BG_SCRBASE_0x1000,		// �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x10000,	// �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
  1,							        // �\���v���C�I���e�B�[
  0,							        // �G���A�I�[�o�[�t���O
  0,							        // DUMMY
  FALSE,						      // ���U�C�N�ݒ�
}; 


static GXRgb EdgeColor[] = 
{ 
  GX_RGB(0, 0, 0),
  GX_RGB(31, 0, 0),
  GX_RGB(0, 31, 0),
  GX_RGB(0, 0, 31), 
  GX_RGB(31, 31, 0),
  GX_RGB(31, 0, 31),
  GX_RGB(0, 31, 31),
  GX_RGB(31, 31, 31),
};

#define CIRCLE_DIV_COUNT (100)
static VecFx16 circlePoints[ CIRCLE_DIV_COUNT ];

static int pos = 0;
static BOOL antiAlias = FALSE;
static BOOL drawLine = FALSE;
static BOOL edgeMarking = FALSE;
static GXRgb lineColor = GX_RGB(0, 0, 0);


//=============================================================================== 
// ���e�X�g��� ���[�N
//=============================================================================== 
struct _RESEARCH_TEST_WORK
{
  HEAPID heapID; 
  RESEARCH_TEST_SEQ seq;  // �����V�[�P���X
};


//===============================================================================
// ������J�֐�
//===============================================================================
static void SetupBG( RESEARCH_TEST_WORK* work );
static void CleanUpBG( RESEARCH_TEST_WORK* work );

static void Setup3D( RESEARCH_TEST_WORK* work );
static void DrawTriangle( int polygonID );

static void SetupCirclePoints();
static void DrawCircle( int pos, GXRgb color1, GXRgb color2, int polygonID, float z );

static void SetupMatrix();
static void CleanUpMatrix();

static void DrawLines();



//-------------------------------------------------------------------------------
/**
 * @brief �e�X�g��ʃ��[�N�̐���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
RESEARCH_TEST_WORK* CreateResearchTestWork( HEAPID heapID )
{
  RESEARCH_TEST_WORK* work;

  work         = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_TEST_WORK) );
  work->heapID = heapID;
  work->seq    = RESEARCH_TEST_SEQ_SETUP;
  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief �e�X�g��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
void DeleteResearchTestWork( RESEARCH_TEST_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  } 
  GFL_HEAP_FreeMemory( work );
} 

//-------------------------------------------------------------------------------
/**
 * @brief �e�X�g��� ���C���֐�
 *
 * @param work �v���Z�X���[�N
 */
//-------------------------------------------------------------------------------
RESEARCH_TEST_RESULT ResearchTestMain( RESEARCH_TEST_WORK* work )
{ 
  switch( work->seq )
  {
  // ������
  case RESEARCH_TEST_SEQ_SETUP:
    SetupBG( work );
    Setup3D( work );
    SetupMatrix();
    SetupCirclePoints();
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);
    work->seq = RESEARCH_TEST_SEQ_MAIN;
    break;

  // ���C��
  case RESEARCH_TEST_SEQ_MAIN:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
    {
      work->seq = RESEARCH_TEST_SEQ_CLEAN_UP;
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      edgeMarking = !edgeMarking;
      G3X_EdgeMarking( edgeMarking );
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      antiAlias = !antiAlias;
      G3X_AntiAlias( antiAlias );
      OS_TFPrintf( 3, "antiAlias = %d\n", antiAlias );
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      if( lineColor == 0 )
      {
        lineColor = GX_RGB(31, 31, 31);
      }
      else
      {
        lineColor = GX_RGB(0, 0, 0);
      }
      {
        int i;
        for( i=0; i<NELEMS(EdgeColor); i++ )
        {
          EdgeColor[i] = lineColor;
        }
        G3X_SetEdgeColorTable( EdgeColor );
      }
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      drawLine = !drawLine;
    }

    if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
    {
      pos--;
      if( pos < 0 ){ pos = 0; }
    }
    if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
    {
      pos++;
      if( CIRCLE_DIV_COUNT <= pos ){ pos = CIRCLE_DIV_COUNT - 1; }
    }
    if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
    {
    }
    if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
    {
    }

    // �`��
    {
      int i;
      for( i=0; i<pos; i++ )
      {
        if( i < 40 )
        {
          DrawCircle( i, GX_RGB(31, 0, 0), GX_RGB(31, 25, 25), (1<<3) + 1, 0.1f );
        }
        else if( i < 70 )
        {
          DrawCircle( i, GX_RGB(0, 31, 0), GX_RGB(25, 31, 25), (2<<3) + 2, 0.3f );
        }
        else if( i < 90 )
        {
          DrawCircle( i, GX_RGB(0, 0, 31), GX_RGB(25, 25, 31), (3<<3) + 3, 0.6f );
        }
        else
        {
          DrawCircle( i, GX_RGB(31, 31, 0), GX_RGB(31, 31, 25), (4<<3) + 4, 0.9f );
        }
      }
    }
    DrawTriangle( 1 );
    if( drawLine ){ DrawLines( 0 ); }
    G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );

    break;

  // ��Еt��
  case RESEARCH_TEST_SEQ_CLEAN_UP:
    CleanUpMatrix();
    CleanUpBG( work );
    work->seq = RESEARCH_TEST_SEQ_FINISH;
    break;

  // �I��
  case RESEARCH_TEST_SEQ_FINISH:
    return RESEARCH_TEST_RESULT_END;
  } 
  return RESEARCH_TEST_RESULT_CONTINUE;
}


//-------------------------------------------------------------------------------
/**
 * @brief BG������������
 */
//-------------------------------------------------------------------------------
static void SetupBG( RESEARCH_TEST_WORK* work )
{
  GFL_BG_SetBGMode( &BGSysHeader );
  GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &BGCnt3M, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl3D( 0 ); 
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON ); 

  // �ǂݍ���
  {
    ARCHANDLE* handle;
    ARCDATID datid_pltt, datid_char, datid_scrn;

    handle     = GFL_ARC_OpenDataHandle( ARCID_SEASON_DISPLAY, work->heapID ); 
    datid_pltt = NARC_season_display_spring_nclr;
    datid_char = NARC_season_display_spring_ncgr;
    datid_scrn = NARC_season_display_spring_nscr;

    { // �p���b�g�f�[�^
      void* src;
      NNSG2dPaletteData* pltt_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_pltt, work->heapID );
      NNS_G2dGetUnpackedPaletteData( src, &pltt_data );
      GFL_BG_LoadPalette( GFL_BG_FRAME3_M, pltt_data->pRawData, 0x20, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    { // �L�����N�^�f�[�^
      void* src;
      NNSG2dCharacterData* char_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_char, work->heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &char_data );
      GFL_BG_LoadCharacter( GFL_BG_FRAME3_M, char_data->pRawData, char_data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    { // �X�N���[���f�[�^
      void* src;
      NNSG2dScreenData* scrn_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_scrn, work->heapID );
      NNS_G2dGetUnpackedScreenData( src, &scrn_data );
      GFL_BG_LoadScreen( GFL_BG_FRAME3_M, scrn_data->rawData, scrn_data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }

    GFL_ARC_CloseDataHandle( handle );
  } 
}

//-------------------------------------------------------------------------------
/**
 * @brief BG��Еt��
 */
//-------------------------------------------------------------------------------
static void CleanUpBG( RESEARCH_TEST_WORK* work )
{
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF ); 
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
}

//-------------------------------------------------------------------------------
/**
 * @brief 3D�����ݒ�
 */
//-------------------------------------------------------------------------------
static void Setup3D( RESEARCH_TEST_WORK* work )
{
  G3X_Init();
  G3X_InitMtxStack();
  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );

  G3_MtxMode( GX_MTXMODE_PROJECTION );
  G3_Identity();
  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  G3_Identity(); 
  G3X_EdgeMarking( TRUE );
  G3X_SetEdgeColorTable( EdgeColor );
}

//-------------------------------------------------------------------------------
/**
 * @brief �O�p�`�`��
 */
//-------------------------------------------------------------------------------
static void DrawTriangle( int polygonID )
{
  G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
      GX_POLYGONMODE_MODULATE,       // modulation mode
      GX_CULL_NONE,       // cull none
      polygonID,          // polygon ID(0 - 63)
      31,                 // alpha(0 - 31)
      0                   // OR of GXPolygonAttrMisc's value
      );

  G3_Begin(GX_BEGIN_TRIANGLES);
  {
    G3_Color( GX_RGB(31, 16, 16) );
    G3_Vtx( 0, 0, -FX16_ONE );

    G3_Color( GX_RGB(16, 31, 16) );
    G3_Vtx( -FX16_ONE, -FX16_ONE, -FX16_ONE );

    G3_Color( GX_RGB(16, 16, 31) );
    G3_Vtx( FX16_ONE, -FX16_ONE, -FX16_ONE ); 
  }
  G3_End();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void SetupCirclePoints()
{
  int i;
  float radius = 0.7f;

  for( i=0; i<CIRCLE_DIV_COUNT; i++ )
  {
    float rad;
    float x, y, z;

    rad = 2 * 3.141592 * i / (float)CIRCLE_DIV_COUNT;
    x   = radius * cos(rad);
    y   = radius * sin(rad);
    z   = 0;//0.01f * i - 1.0f;

    VEC_Fx16Set( &(circlePoints[i]), FX_F32_TO_FX16(x), 
        FX_F32_TO_FX16(y), FX_F32_TO_FX16(z)  );
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void DrawCircle( int pos, GXRgb color1, GXRgb color2, int polygonID, float z )
{
  int pointIdx;
  int nextPointIdx;

  pointIdx     = pos % (CIRCLE_DIV_COUNT - 1);
  nextPointIdx = (pos + 1) % (CIRCLE_DIV_COUNT - 1);

  G3_PolygonAttr( GX_LIGHTMASK_NONE,        // no lights
                  GX_POLYGONMODE_MODULATE,  // modulation mode
                  GX_CULL_NONE,             // cull none
                  polygonID,                // polygon ID(0 - 63)
                  31,                       // alpha(0 - 31)
                  0                         // OR of GXPolygonAttrMisc's value
                  );

  G3_Begin( GX_BEGIN_TRIANGLES );

  G3_Color( color1 );
  G3_Vtx( circlePoints[ pointIdx ].x, circlePoints[ pointIdx ].y, FX_F32_TO_FX16(z) );

  G3_Color( color2 );
  G3_Vtx( 0, 0, FX_F32_TO_FX16(z) );

  G3_Color( color1 );
  G3_Vtx( circlePoints[ nextPointIdx ].x, circlePoints[ nextPointIdx ].y, FX_F32_TO_FX16(z) );

  G3_End();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void SetupMatrix()
{
  VecFx32 camPos, camUp, target;

  VEC_Set( &camPos, 0, 0, FX32_ONE );
  VEC_Set( &camUp, 0, FX32_ONE, 0 );
  VEC_Set( &target, 0, 0, 0 );

  G3_LookAt( &camPos, &camUp, &target, NULL );
  //G3_PushMtx();

  G3_Ortho( FX_F32_TO_FX16(1.0f),
            FX_F32_TO_FX16(-1.0f),
            FX_F32_TO_FX16(-1.333f),
            FX_F32_TO_FX16(1.333f),
            FX_F32_TO_FX16(0.1f),
            FX_F32_TO_FX16(5.0f),
            NULL ); 
  //G3_PushMtx();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void CleanUpMatrix()
{
  G3_MtxMode( GX_MTXMODE_PROJECTION );
  //G3_PopMtx(1); 

  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  //G3_PopMtx(1); 
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void DrawLines( int polygonID )
{
  int i;
  int idx;
  int pointIdx[] = {0, 40, 70, 90};
  fx16 z = FX_F32_TO_FX16( 0.9f );

  G3_PolygonAttr( GX_LIGHTMASK_NONE,        // no lights
                  GX_POLYGONMODE_MODULATE,  // modulation mode
                  GX_CULL_NONE,             // cull none
                  polygonID,                // polygon ID(0 - 63)
                  31,                       // alpha(0 - 31)
                  0                         // OR of GXPolygonAttrMisc's value
                  );

  for( i=0; i<NELEMS(pointIdx); i++ )
  {
    idx = pointIdx[i];

    G3_Color( lineColor );

    G3_Vtx( 0, 0, z ); 
    G3_Vtx( circlePoints[ idx ].x, circlePoints[ idx ].y, z );
    G3_Vtx( circlePoints[ idx ].x, circlePoints[ idx ].y, z );
  }
}
