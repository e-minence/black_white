//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldeff_bubble.c
 *	@brief  �[�C�@���A�G�t�F�N�g
 *	@author	tomoya takahashi
 *	@date		2010.02.23
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "system/gfl_use.h"

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_bubble.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	Resource�C���f�b�N�X
//=====================================
enum {
  // Resource
  BUBBLE_RES_MDL_BUBBLE = 0,
  BUBBLE_RES_ANM_ITA,
  BUBBLE_RES_ANM_IMA,
  BUBBLE_RES_MAX,

  // �����_���[
  BUBBLE_RND_BUBBLE = 0,
  BUBBLE_RND_MAX,

  // �A�j��
  BUBBLE_ANM_ITA = 0,
  BUBBLE_ANM_IMA,
  BUBBLE_ANM_MAX,
} ;

// �ǂݍ��݃��\�[�X
static const u32 sc_RES_INDEX[ BUBBLE_RES_MAX ] = {
  NARC_fldeff_awa_nsbmd,
  NARC_fldeff_awa_nsbta,
  NARC_fldeff_awa_nsbma,
};

// �����_���[��`
static const u32 sc_RND_RES_INDEX[ BUBBLE_RND_MAX ] = {
  BUBBLE_RES_MDL_BUBBLE,
};

// �A�j���[�V������`
static const u32 sc_ANM_RES_INDEX[ BUBBLE_ANM_MAX ] = {
  BUBBLE_RES_ANM_ITA,
  BUBBLE_RES_ANM_IMA,
};


//-------------------------------------
///	�A���[�N�ő吔
//=====================================
#define BUBBLE_WK_MAX   (6)


//-------------------------------------
///	�A�o�́@�^�C�~���O
//=====================================
#define BUBBLE_POP_TIMING ( 120 )
#define BUBBLE_POP_ACTION_TIMING ( 5 )

//-------------------------------------
///	�o�����W����
//=====================================
#define BUBBLE_POP_POS_Y  ( 7*FX32_ONE )
#define BUBBLE_POP_POS_FRONT_UP  ( 14*FX32_ONE )  //�A�����@�̌���������o��悤��
#define BUBBLE_POP_POS_FRONT  ( 4*FX32_ONE )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//--------------------------------------------------------------
///  FLDEFF_BUBBLE�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_BUBBLE FLDEFF_BUBBLE;

//--------------------------------------------------------------
///  FLDEFF_BUBBLE�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_BUBBLE
{
  FLDEFF_CTRL*  p_fectrl;
  
  GFL_G3D_RES*  p_res[BUBBLE_RES_MAX];
  GFL_G3D_RND*  p_rnd[BUBBLE_RND_MAX];
  GFL_G3D_ANM*  p_anm[BUBBLE_ANM_MAX];
  GFL_G3D_OBJ*  p_obj[BUBBLE_RND_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_BUBBLE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_BUBBLE* p_fldeff_bubble;
  MMDL* p_mmdl;
}TASKHEADER_BUBBLE;

//-------------------------------------
///	�A���[�N
//=====================================
typedef struct {
  BOOL flag;
  int frame;
  VecFx32 pos;
} BUBBLE_WK;


//--------------------------------------------------------------
/// TASKWORK_FOOTMARK
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_BUBBLE head;
  u8 last_action;
  s8 action_count;
  s8 frame;
  BUBBLE_WK obj[BUBBLE_WK_MAX];
}TASKWORK_BUBBLE;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void bubble_InitResource( FLDEFF_BUBBLE* p_wk );
static void bubble_ExitResource( FLDEFF_BUBBLE* p_wk );


static void bubbleTask_Init( FLDEFF_TASK* p_task, void* p_work );
static void bubbleTask_Exit( FLDEFF_TASK* p_task, void* p_work );
static void bubbleTask_Update( FLDEFF_TASK* p_task, void* p_work );
static void bubbleTask_Draw( FLDEFF_TASK* p_task, void* p_work );



// �A�Ǘ�
static BUBBLE_WK* bubbleTask_GetClearWk( TASKWORK_BUBBLE* p_wk );
static void bubbleTask_SetUpObj( BUBBLE_WK* p_wk, const MMDL* cp_mmdl );
static void bubbleTask_ClearObj( BUBBLE_WK* p_wk );
static void bubbleTask_UpdateObj( BUBBLE_WK* p_wk, int frame_max );
static void bubbleTask_DrawObj( BUBBLE_WK* p_wk, GFL_G3D_OBJ* p_mdl );

// �ڂ̑O�̕������擾
static u16 bubbleTask_GetFrontWay( const MMDL* cp_mmdl, VecFx16* p_way );

// �w�b�_�[
static const FLDEFF_TASK_HEADER data_bubbleTaskHeader;

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���ɐ��A�@�G�t�F�N�g�^�X�N���֘A�t����
 *
 *	@param	p_mmdl      ���f��
 *	@param	p_fectrl    �G�t�F�N�g���[�N
 */
//-----------------------------------------------------------------------------
void FLDEFF_BUBBLE_SetMMdl( MMDL* p_mmdl, FLDEFF_CTRL* p_fectrl )
{
  FLDEFF_BUBBLE* p_bubble;
  TASKHEADER_BUBBLE head;
  VecFx32 pos;

  p_bubble = FLDEFF_CTRL_GetEffectWork( p_fectrl, FLDEFF_PROCID_BUBBLE );
  head.p_fldeff_bubble  = p_bubble;
  head.p_mmdl           = p_mmdl;

  // �ʒu
  MMDL_GetVectorPos( p_mmdl, &pos );

  FLDEFF_CTRL_AddTask(
      p_fectrl, &data_bubbleTaskHeader, &pos, 0, &head, 0 );
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���A�G�t�F�N�g��������
 *
 *	@param	p_fectrl  �G�t�F�N�g���[�N
 *	@param	heapID    �q�[�vID
 *
 *	@return �G�t�F�N�g�g�p���[�N
 */
//-----------------------------------------------------------------------------
void * FLDEFF_BUBBLE_Init( FLDEFF_CTRL* p_fectrl, HEAPID heapID )
{
  FLDEFF_BUBBLE* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_BUBBLE) );

  p_wk->p_fectrl = p_fectrl;
  bubble_InitResource( p_wk );
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A�G�t�F�N�g�̔j��
 *
 *	@param	Pfectrl �G�t�F�N�g���[�N
 *	@param	p_work  ���[�N
 */
//-----------------------------------------------------------------------------
void FLDEFF_BUBBLE_Delete( FLDEFF_CTRL* p_fectrl, void* p_work )
{
  FLDEFF_BUBBLE* p_wk = p_work;
  bubble_ExitResource( p_wk );
  GFL_HEAP_FreeMemory( p_wk );
}





//-----------------------------------------------------------------------------
/**
 *      private 
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X������
 */
//-----------------------------------------------------------------------------
static void bubble_InitResource( FLDEFF_BUBBLE* p_wk )
{
  int i, j;
  ARCHANDLE* p_handle = FLDEFF_CTRL_GetArcHandleEffect( p_wk->p_fectrl );
  BOOL result;

  // ���\�[�X�ǂݍ���
  for( i=0; i<BUBBLE_RES_MAX; i++ ){
    p_wk->p_res[i] = GFL_G3D_CreateResourceHandle( p_handle, sc_RES_INDEX[i] );
    if( GFL_G3D_CheckResourceType( p_wk->p_res[i], GFL_G3D_RES_CHKTYPE_TEX ) ){
      // �e�N�X�`���]��
      result = GFL_G3D_TransVramTexture( p_wk->p_res[i] );
      GF_ASSERT( result );
    }
  }

  // �����_���[����
  for( i=0; i<BUBBLE_RND_MAX; i++ ){
    p_wk->p_rnd[i] = GFL_G3D_RENDER_Create( p_wk->p_res[ sc_RND_RES_INDEX[i] ], 
        0, p_wk->p_res[ sc_RND_RES_INDEX[i] ] );
  }

  // �A�j���[�V�����I�u�W�F
  for( i=0; i<BUBBLE_ANM_MAX; i++ ){
    p_wk->p_anm[i] = GFL_G3D_ANIME_Create( p_wk->p_rnd[BUBBLE_RND_BUBBLE], 
        p_wk->p_res[ sc_ANM_RES_INDEX[i] ], 0 );
  }

  // OBJ����
  for( i=0; i<BUBBLE_RND_MAX; i++ ){
    p_wk->p_obj[i] = GFL_G3D_OBJECT_Create( p_wk->p_rnd[i], p_wk->p_anm, BUBBLE_ANM_MAX );

    for( j=0; j<BUBBLE_ANM_MAX; j++ ){
      GFL_G3D_OBJECT_EnableAnime( p_wk->p_obj[i], j );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�j��
 */
//-----------------------------------------------------------------------------
static void bubble_ExitResource( FLDEFF_BUBBLE* p_wk )
{
  int i;
  BOOL result;
  
  for( i=0; i<BUBBLE_RND_MAX; i++ ){
    GFL_G3D_OBJECT_Delete( p_wk->p_obj[i] );
  }

  for( i=0; i<BUBBLE_ANM_MAX; i++ ){
    GFL_G3D_ANIME_Delete( p_wk->p_anm[i] );
  }

  for( i=0; i<BUBBLE_RND_MAX; i++ ){
    GFL_G3D_RENDER_Delete( p_wk->p_rnd[i] );
  }

  for( i=0; i<BUBBLE_RES_MAX; i++ ){
    if( GFL_G3D_CheckResourceType( p_wk->p_res[i], GFL_G3D_RES_CHKTYPE_TEX ) ){
      // �e�N�X�`���j��
      result = GFL_G3D_FreeVramTexture( p_wk->p_res[i] );
      GF_ASSERT( result );
    }
    GFL_G3D_DeleteResource( p_wk->p_res[i] );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���A�@�^�X�N�@������
 */
//-----------------------------------------------------------------------------
static void bubbleTask_Init( FLDEFF_TASK* p_task, void* p_work )
{
  TASKWORK_BUBBLE* p_wk = p_work;
  const TASKHEADER_BUBBLE * cp_head;
  cp_head = FLDEFF_TASK_GetAddPointer( p_task );
  p_wk->head = *cp_head;
  p_wk->frame = BUBBLE_POP_TIMING;
  p_wk->last_action = MMDL_CheckPossibleAcmd( p_wk->head.p_mmdl );
  p_wk->action_count = BUBBLE_POP_ACTION_TIMING;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A�@�^�X�N�@�j��
 */
//-----------------------------------------------------------------------------
static void bubbleTask_Exit( FLDEFF_TASK* p_task, void* p_work )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A�@�^�X�N�@�X�V
 */
//-----------------------------------------------------------------------------
static void bubbleTask_Update( FLDEFF_TASK* p_task, void* p_work )
{
  TASKWORK_BUBBLE* p_wk = p_work;
  int i;
  int frame_max;
  BUBBLE_WK* p_obj;

  
  // �����Ǘ�
  p_wk->frame ++;
  if( p_wk->frame >= BUBBLE_POP_TIMING ){
    p_obj = bubbleTask_GetClearWk( p_wk );
    if( p_obj ){
      bubbleTask_SetUpObj( p_obj, p_wk->head.p_mmdl );
    }
    p_wk->frame = 0;
  }

  // �����͂��߁A�Ƃ܂����Ƃ��A
  // �͂��߁[���Ƃ܂��BUBBLE_POP_ACTION_TIMING��J��Ԃ�����o��
  if( p_wk->last_action != MMDL_CheckPossibleAcmd( p_wk->head.p_mmdl ) ){

    p_wk->action_count ++;
    
    if( p_wk->action_count >= BUBBLE_POP_ACTION_TIMING ){
      p_obj = bubbleTask_GetClearWk( p_wk );
      if( p_obj ){
        bubbleTask_SetUpObj( p_obj, p_wk->head.p_mmdl );
      }
      p_wk->action_count = 0;
    }
    
    p_wk->last_action = MMDL_CheckPossibleAcmd( p_wk->head.p_mmdl );
  }

  // ���[�N���C��
  GFL_G3D_OBJECT_GetAnimeFrameMax( p_wk->head.p_fldeff_bubble->p_obj[ BUBBLE_RND_BUBBLE ], BUBBLE_ANM_ITA, &frame_max );

  for( i=0; i<BUBBLE_WK_MAX; i++ ){
    bubbleTask_UpdateObj( &p_wk->obj[i], frame_max );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A�@�^�X�N�@�`��
 */
//-----------------------------------------------------------------------------
static void bubbleTask_Draw( FLDEFF_TASK* p_task, void* p_work )
{
  TASKWORK_BUBBLE* p_wk = p_work;
  int i;

  for( i=0; i<BUBBLE_WK_MAX; i++ ){
    bubbleTask_DrawObj( &p_wk->obj[i], p_wk->head.p_fldeff_bubble->p_obj[ BUBBLE_RND_BUBBLE ] );
  }
}



// �A�Ǘ�
//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă��郏�[�N���擾
 *
 *	@param	p_wk    ���[�N
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
static BUBBLE_WK* bubbleTask_GetClearWk( TASKWORK_BUBBLE* p_wk )
{
  int i;
  for( i=0; i<BUBBLE_WK_MAX; i++ ){
    if( p_wk->obj[i].flag == FALSE ){
      return &p_wk->obj[i];
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�u�W�F�̃Z�b�g�A�b�v
 *
 *	@param	p_wk    ���[�N
 *	@param	cp_pos  �ʒu
 */
//-----------------------------------------------------------------------------
static void bubbleTask_SetUpObj( BUBBLE_WK* p_wk, const MMDL* cp_mmdl )
{
  VecFx16 way;
  u16 dir;
  fx32 mul;
  
  p_wk->flag  = TRUE;
  p_wk->frame = 0;
  MMDL_GetVectorPos( cp_mmdl, &p_wk->pos );
  dir = bubbleTask_GetFrontWay( cp_mmdl, &way );
  if( dir == DIR_UP ){
    mul = BUBBLE_POP_POS_FRONT_UP;
  }else{
    mul = BUBBLE_POP_POS_FRONT;
  }

  p_wk->pos.y += BUBBLE_POP_POS_Y;
  p_wk->pos.x += FX_Mul( way.x, mul );
  p_wk->pos.z += FX_Mul( way.z, mul );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�u�W�F���̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void bubbleTask_ClearObj( BUBBLE_WK* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(BUBBLE_WK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�u�W�F���̍X�V
 */
//-----------------------------------------------------------------------------
static void bubbleTask_UpdateObj( BUBBLE_WK* p_wk, int frame_max )
{
  if( p_wk->flag == FALSE ){
    return;
  }
  
  p_wk->frame += FX32_ONE;
  if( p_wk->frame > frame_max ){
    bubbleTask_ClearObj( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�u�W�F���̕`��
 */
//-----------------------------------------------------------------------------
static void bubbleTask_DrawObj( BUBBLE_WK* p_wk, GFL_G3D_OBJ* p_mdl )
{
  static GFL_G3D_OBJSTATUS status = {
    {0},
    {FX32_ONE,FX32_ONE,FX32_ONE},
    {
      FX32_ONE,0,0,
      0,FX32_ONE,0,
      0,0,FX32_ONE,
    },
  };
  int i;

  if( p_wk->flag == FALSE ){
    return;
  }

  // �ʒu�ƃt���[���ŕ`��
  status.trans = p_wk->pos;

  for( i=0; i<BUBBLE_ANM_MAX; i++ ){
    GFL_G3D_OBJECT_SetAnimeFrame( p_mdl,
        i, &p_wk->frame );
  }
  GFL_G3D_DRAW_DrawObjectCullingON( p_mdl, &status );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ڂ̑O�̕�����Ԃ�
 */
//-----------------------------------------------------------------------------
static u16 bubbleTask_GetFrontWay( const MMDL* cp_mmdl, VecFx16* p_way )
{
  u16 dir = MMDL_GetDirDisp( cp_mmdl );
  
  if( !MMDL_CheckStatusBit( cp_mmdl, MMDL_STABIT_RAIL_MOVE ) ){
    p_way->y = 0;
    p_way->x = MMDL_TOOL_GetDirAddValueGridX( dir );
    p_way->z = MMDL_TOOL_GetDirAddValueGridZ( dir );

    p_way->x = GRID_TO_FX32( p_way->x );
    p_way->z = GRID_TO_FX32( p_way->z );
    VEC_Fx16Normalize( p_way, p_way );
  }else{
    MMDL_Rail_GetDirLineWay( cp_mmdl, dir, p_way );
    p_way->y = 0;
    VEC_Fx16Normalize( p_way, p_way );
  }

  return dir;
}



//--------------------------------------------------------------
//  ���A�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_bubbleTaskHeader =
{
  sizeof(TASKWORK_BUBBLE),
  bubbleTask_Init,
  bubbleTask_Exit,
  bubbleTask_Update,
  bubbleTask_Draw,
};



