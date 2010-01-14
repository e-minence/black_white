//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fog_light_test.c
 *	@brief  FOG���g�p����LIGHT�e�X�g
 *	@author	tomoya takahashi
 *	@date		2010.01.14
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fog_light_test.h"
#include "fieldmap_func.h"

#include "arc/arc_def.h"
#include "arc/debug/rail_editor.naix"
#include "arc/field_light.naix"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���f���萔
//=====================================
enum {
  MODEL_NORMAL = 0,
  MODEL_SHADOW,
  MODEL_SHADOW_MSK,

  MODEL_MAX,
  
} ;
//-------------------------------------
///	�`�惂�[�h
//=====================================
enum {
  DRAWMODE_NORMAL,
  DRAWMODE_SHADOW,

  DRAWMODE_MAX,
} ;


static const u32 sc_MODEL_IDX[MODEL_MAX] = 
{
  NARC_rail_editor_fog_light_nsbmd,
  NARC_rail_editor_fog_light_shdow_nsbmd,
  NARC_rail_editor_fog_light_shdow_msk_nsbmd,
};

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	FOG_LIGHT_TEST
//=====================================
typedef struct {

  u32 draw_mode;

  PLAYER_WORK* p_player;
  FIELD_FOG_WORK* p_fog;

  GFL_G3D_RES* p_res[MODEL_MAX];
  GFL_G3D_RND* p_rnd[MODEL_MAX];
  GFL_G3D_OBJ* p_obj[MODEL_MAX];
  
    
  FLDMAPFUNC_WORK * p_taskwk; // ���ŗp

  u16 rotate_x;
  
} FOG_LIGHT_TEST_WK;


static u8 s_FOG_TBL[ 32 ] = {
  0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,

  0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,

  0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,

  0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void FOG_LIGHT_TEST_Create( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work );
static void FOG_LIGHT_TEST_Delete( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work );
static void FOG_LIGHT_TEST_Update( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work );
static void FOG_LIGHT_TEST_Write( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work );


static void FOG_LIGHT_TEST_LoadMdl( FOG_LIGHT_TEST_WK* p_wk, HEAPID heapID );
static void FOG_LIGHT_TEST_ReleaseMdl( FOG_LIGHT_TEST_WK* p_wk );

//-------------------------------------
///	�^�X�N�f�[�^
//=====================================
static const FLDMAPFUNC_DATA sc_FLDMAPFUNC_DATA = 
{
  0,
  sizeof(FOG_LIGHT_TEST_WK),
  FOG_LIGHT_TEST_Create,
  FOG_LIGHT_TEST_Delete,
  FOG_LIGHT_TEST_Update,
  FOG_LIGHT_TEST_Write,
};



//----------------------------------------------------------------------------
/**
 *	@brief  FOG�@LIGHT�@�e�X�g  ������
 */
//-----------------------------------------------------------------------------
void FOG_LIGHT_TEST_Init( FIELDMAP_WORK* fieldWork )
{
  FOG_LIGHT_TEST_WK* p_wk;
  FLDMAPFUNC_WORK * p_taskwk;
  FLDMAPFUNC_SYS* p_tasksys = FIELDMAP_GetFldmapFuncSys( fieldWork );
  
  // �^�X�N����
  p_taskwk = FLDMAPFUNC_Create( p_tasksys, &sc_FLDMAPFUNC_DATA );

  // ���[�N�擾
  p_wk = FLDMAPFUNC_GetFreeWork( p_taskwk );

  p_wk->p_taskwk = p_taskwk;
}







//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  FOG��LIGHT�V�X�e��������
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_Create( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work )
{
  FOG_LIGHT_TEST_WK* p_wk = p_work;
  FIELD_FOG_WORK * p_fog = FIELDMAP_GetFieldFog( p_fieldmap );
  FIELD_LIGHT * p_light = FIELDMAP_GetFieldLight( p_fieldmap );
  GAMESYS_WORK * p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA * p_gdata = GAMESYSTEM_GetGameData( p_gsys );
  
  // �t�H�O�e�[�u����ݒ�
  p_wk->p_fog = p_fog;
  FIELD_FOG_SetTblAll( p_fog, s_FOG_TBL );
  FIELD_FOG_SetFlag( p_fog, TRUE );
  FIELD_FOG_SetSlope( p_fog, FIELD_FOG_SLOPE_0x8000 );
  FIELD_FOG_SetOffset( p_fog, 0 );

  // LIGHT�J���[�ύX
  FIELD_LIGHT_Change( p_light, NARC_field_light_test_light00_00_dat );

  // ���f���ǂݍ���
  FOG_LIGHT_TEST_LoadMdl( p_wk, FIELDMAP_GetHeapID( p_fieldmap ) ); 

  //�v���C���[�擾
  p_wk->p_player = GAMEDATA_GetMyPlayerWork( p_gdata );

  p_wk->rotate_x = 0;

  p_wk->draw_mode = DRAWMODE_NORMAL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e���j��
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_Delete( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work )
{
  FOG_LIGHT_TEST_WK* p_wk = p_work;
  // ���f���j��
  FOG_LIGHT_TEST_ReleaseMdl( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�V����
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_Update( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work )
{
  FOG_LIGHT_TEST_WK* p_wk = p_work;
  int i;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    FLDMAPFUNC_Delete( p_taskwk );
    return ;
  }


  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
  {
    for( i=0; i<32; i++ )
    {
      s_FOG_TBL[i] += 0x8;
    }
    OS_TPrintf( "FOG TBL %d\n", s_FOG_TBL[0] );
    FIELD_FOG_SetTblAll( p_wk->p_fog, s_FOG_TBL );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
  {
    for( i=0; i<32; i++ )
    {
      s_FOG_TBL[i] -= 0x8;
    }
    OS_TPrintf( "FOG TBL %d\n", s_FOG_TBL[0] );
    FIELD_FOG_SetTblAll( p_wk->p_fog, s_FOG_TBL );
  }

  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    p_wk->rotate_x += 182;
    OS_TPrintf( "rotate x 0x%x\n", p_wk->rotate_x );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y )
  {
    p_wk->rotate_x -= 182;
    OS_TPrintf( "rotate x 0x%x\n", p_wk->rotate_x );
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    // Shadow�{�����[�����g�p
    p_wk->draw_mode = (p_wk->draw_mode + 1) % DRAWMODE_MAX;
    switch( p_wk->draw_mode )
    {
    case DRAWMODE_NORMAL:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
      OS_TPrintf( "DRAWMODE Normal\n" );
      break;
    case DRAWMODE_SHADOW:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
      OS_TPrintf( "DRAWMODE Shadow  need manual�\�[�g\n" );
      break;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�揈��
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_Write( FLDMAPFUNC_WORK * p_taskwk, FIELDMAP_WORK * p_fieldmap, void * p_work )
{
  FOG_LIGHT_TEST_WK* p_wk = p_work;
  GFL_G3D_OBJSTATUS objstatus = {
    {0,0,0},
    {FX32_ONE,FX32_ONE,FX32_ONE},
    { FX32_ONE,0,0, 0,FX32_ONE,0, 0,0,FX32_ONE, },
  };
  MtxFx33 rot_x;
  u16 rotate;
  
  // ���@�ʒu�A��������������
  objstatus.trans = *(PLAYERWORK_getPosition( p_wk->p_player ));
  rotate = PLAYERWORK_getDirection( p_wk->p_player ); 
  MTX_RotY33( &objstatus.rotate, FX_SinIdx( rotate ), FX_CosIdx( rotate ) );
  MTX_RotX33( &rot_x, FX_SinIdx( p_wk->rotate_x ), FX_CosIdx( p_wk->rotate_x ) );
  MTX_Concat33( &rot_x, &objstatus.rotate, &objstatus.rotate );

  // ���f���̕`��
  if( p_wk->draw_mode == DRAWMODE_NORMAL )
  {
    GFL_G3D_DRAW_DrawObject( p_wk->p_obj[MODEL_NORMAL], &objstatus );
  }
  else
  {
    GFL_G3D_DRAW_DrawObject( p_wk->p_obj[MODEL_SHADOW_MSK], &objstatus );
    GFL_G3D_DRAW_DrawObject( p_wk->p_obj[MODEL_SHADOW], &objstatus );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���f���̓ǂݍ���
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_LoadMdl( FOG_LIGHT_TEST_WK* p_wk, HEAPID heapID )
{
  int i;
  
  for( i=0; i<MODEL_MAX; i++ )
  {
    p_wk->p_res[i] = GFL_G3D_CreateResourceArc( ARCID_RAIL_EDITOR, sc_MODEL_IDX[i] );
    p_wk->p_rnd[i] = GFL_G3D_RENDER_Create( p_wk->p_res[i], 0, NULL );
    p_wk->p_obj[i] = GFL_G3D_OBJECT_Create( p_wk->p_rnd[i], NULL, 0 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���̔j��
 */
//-----------------------------------------------------------------------------
static void FOG_LIGHT_TEST_ReleaseMdl( FOG_LIGHT_TEST_WK* p_wk )
{
  int i;

  for( i=0; i<MODEL_MAX; i++ )
  {
    GFL_G3D_OBJECT_Delete(p_wk->p_obj[i]);
    GFL_G3D_RENDER_Delete( p_wk->p_rnd[i] );
    GFL_G3D_DeleteResource( p_wk->p_res[i] );
  }
}

