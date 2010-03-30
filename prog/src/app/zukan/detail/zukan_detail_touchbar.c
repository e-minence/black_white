//============================================================================
/**
 *  @file   zukan_detail_touchbar.c
 *  @brief  �}�ӏڍ׉�ʋ��ʂ̃^�b�`�o�[
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_TOUCHBAR
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "zukan_detail_def.h"
#include "zknd_tbar.h"
#include "zukan_detail_touchbar.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "zukan_gra.naix"
#include "townmap_gra.naix"

// �T�E���h
#include "sound/wb_sound_data.sadl"

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_CUSTOM             = 3,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_CUSTOM             = ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR + ZKND_TBAR_OBJ_PLT_NUM,
};

// GENERAL�̃p���b�g�A�j���p�Ƀp���b�g��ύX����
#define OBJ_PAL_OFFSET_GENERAL_ANIME_NONE  (0)
#define OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC  (2)
#define RES_PAL_POS_GENERAL_ANIME_START    (1)  // ���\�[�X�̃p���b�g�̗�ԍ�
#define RES_PAL_POS_GENERAL_ANIME_END      (3)  // ���\�[�X�̃p���b�g�̗�ԍ�
#define GENERAL_ANIME_ADD              (0x400)  // FX_CosIdx���g�p����̂�0<= <0x10000

// �Z���A�N�^�[���j�b�g�Ŏg���郏�[�N��
#define CLUNIT_WORK_NUM    (16)

// �X�N���[��
#define SCROLL_Y_DISAPPEAR     (-24)
#define SCROLL_Y_APPEAR        (0)
#define SCROLL_VALUE_OUTSIDE   (1)  // �ړ���
#define SCROLL_VALUE_INSIDE    (2)
#define SCROLL_WAIT_OUTSIDE    (0)  // 0=���t���[���ړ�; 1=�ړ��������1�t���[���҂�; 2=�ړ��������2�t���[���҂�;
#define SCROLL_WAIT_INSIDE     (0)
static const int scroll_value[ZUKAN_DETAIL_TOUCHBAR_SPEED_MAX] =
{
  SCROLL_VALUE_OUTSIDE,
  SCROLL_VALUE_INSIDE,
};
static const u8 scroll_wait[ZUKAN_DETAIL_TOUCHBAR_SPEED_MAX] =
{
  SCROLL_WAIT_OUTSIDE,
  SCROLL_WAIT_INSIDE,
};

// �J�X�^���{�^��
// GENERAL
enum
{
  GENERAL_CUSTOM_INFO    = ZKND_TBAR_ICON_CUSTOM,
  GENERAL_CUSTOM_MAP,
  GENERAL_CUSTOM_VOICE,
  GENERAL_CUSTOM_FORM,
};
// MAP
enum
{
  MAP_CUSTOM_DUMMY       = ZKND_TBAR_ICON_CUSTOM,
};
// FORM
enum
{
  FORM_CUSTOM_EXCHANGE    = ZKND_TBAR_ICON_CUSTOM,
};

// �{�^����
// GENERAL
enum
{
  GENERAL_ICON_RETURN,
  GENERAL_ICON_CLOSE,
  GENERAL_ICON_CHECK,
  GENERAL_ICON_CUSTOM_FORM,
  GENERAL_ICON_CUSTOM_VOICE,
  GENERAL_ICON_CUSTOM_MAP,
  GENERAL_ICON_CUSTOM_INFO,
  GENERAL_ICON_CUR_D,
  GENERAL_ICON_CUR_U,
  GENERAL_ICON_MAX,
};
// MAP
enum
{
  MAP_ICON_RETURN,
  MAP_ICON_MAX,
};
// FORM 
enum
{
  FORM_ICON_RETURN,
  FORM_ICON_CUR_R,
  FORM_ICON_CUR_L,
  FORM_ICON_CUSTOM_EXCHANGE,
  FORM_ICON_CUR_D,
  FORM_ICON_CUR_U,
  FORM_ICON_MAX,
};
#define ICON_MAX_MAX (GENERAL_ICON_MAX)  // GENERAL_ICON_MAX, MAP_ICON_MAX, FORM_ICON_MAX�̒��ōő�̂���

typedef struct
{
  int            id;
  GFL_CLACTPOS   pos;
  u16            width;
  ZKNDTL_COMMAND cmd;
}
ICON_CONST_SET;

// GENERAL
static const ICON_CONST_SET general_icon_const_set[GENERAL_ICON_MAX] =
{
  { ZKND_TBAR_ICON_RETURN,    { 232, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_RETURN,        },
  { ZKND_TBAR_ICON_CLOSE,     { 208, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_CLOSE,         },
  { ZKND_TBAR_ICON_CHECK,     { 184, ZKND_TBAR_ICON_Y_CHECK },    24,  ZKNDTL_CMD_CHECK,         },
  { GENERAL_CUSTOM_FORM,      { 144, ZKND_TBAR_ICON_Y       },    32,  ZKNDTL_CMD_FORM,          },
  { GENERAL_CUSTOM_VOICE,     { 112, ZKND_TBAR_ICON_Y       },    32,  ZKNDTL_CMD_VOICE,         }, 
  { GENERAL_CUSTOM_MAP,       {  80, ZKND_TBAR_ICON_Y       },    32,  ZKNDTL_CMD_MAP,           },
  { GENERAL_CUSTOM_INFO,      {  48, ZKND_TBAR_ICON_Y       },    32,  ZKNDTL_CMD_INFO,          },
  { ZKND_TBAR_ICON_CUR_D,     {  24, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_CUR_D,         },
  { ZKND_TBAR_ICON_CUR_U,     {   0, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_CUR_U,         },
};
// MAP
static const ICON_CONST_SET map_icon_const_set[MAP_ICON_MAX] =
{
  { ZKND_TBAR_ICON_RETURN,    { 232, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_MAP_RETURN,    },
};
// FORM
static const ICON_CONST_SET form_icon_const_set[FORM_ICON_MAX] =
{
  { ZKND_TBAR_ICON_RETURN,    { 232, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_FORM_RETURN,   },
  { ZKND_TBAR_ICON_CUR_R,     { 192, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_FORM_CUR_R,    },
  { ZKND_TBAR_ICON_CUR_L,     { 168, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_FORM_CUR_L,    },
  { FORM_CUSTOM_EXCHANGE,     { 104, ZKND_TBAR_ICON_Y       },    48,  ZKNDTL_CMD_FORM_EXCHANGE, },
  { ZKND_TBAR_ICON_CUR_D,     {  24, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_FORM_CUR_D,    },
  { ZKND_TBAR_ICON_CUR_U,     {   0, ZKND_TBAR_ICON_Y       },    24,  ZKNDTL_CMD_FORM_CUR_U,    },
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
typedef struct
{
  const ICON_CONST_SET*  cset;
  GFL_CLWK*              clwk;
}
ICON_SET;

//-------------------------------------
/// ���[�N
//=====================================
struct _ZUKAN_DETAIL_TOUCHBAR_WORK
{
  // ���̂Ƃ��납��ؗp�������
  HEAPID                        heap_id;

  // ����
  BOOL                          form_version;  // TRUE�̂Ƃ��t�H��������o�[�W����

  // �����ō쐬�������
  // ���
  ZUKAN_DETAIL_TOUCHBAR_STATE   state;
  ZUKAN_DETAIL_TOUCHBAR_TYPE    prev_type;
  ZUKAN_DETAIL_TOUCHBAR_TYPE    type;
  ZUKAN_DETAIL_TOUCHBAR_DISP    prev_disp;
  ZUKAN_DETAIL_TOUCHBAR_DISP    disp;
  ZUKAN_DETAIL_TOUCHBAR_SPEED   speed;
  u8                            scroll_wait_count;

  // �Z���A�N�^�[���j�b�g
  GFL_CLUNIT*                   clunit;

  // VBlank��TCB
  GFL_TCB*                      vblank_tcb;

  // �^�b�`�o�[
  ZKND_TBAR_WORK*               tbwk;

  // �J�X�^���{�^��
  // GENERAL
  u32    general_ncl;
  u32    general_ncg;
  u32    general_nce;
  // MAP
  u32    map_ncl;
  u32    map_ncg;
  u32    map_nce;
  // FORM
  u32    form_ncl;
  u32    form_ncg;
  u32    form_nce;

  // ICON_SET
  u8            icon_set_num;
  ICON_SET      icon_set[ICON_MAX_MAX];
  s16           icon_ofs_pos_y;

  // GENERAL�̃p���b�g�A�j��
  u16           pal_anime_general_start[0x10];
  u16           pal_anime_general_end[0x10];
  u16           pal_anime_general_now[0x10];
  int           pal_anime_general_count;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk );

// �^�b�`�o�[
static void Zukan_Detail_Touchbar_Create( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_Delete( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_SetIconOfsPosY( ZUKAN_DETAIL_TOUCHBAR_WORK* work, s16 ofs_pos_y );

// GENERAL
static void Zukan_Detail_Touchbar_CreateGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_DeleteGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_MainGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
// GENERAL�̃p���b�g�A�j��
static void Zukan_Detail_Touchbar_AnimeBaseInitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_AnimeBaseExitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_AnimeInitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_AnimeExitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_AnimeMainGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work );

// MAP
static void Zukan_Detail_Touchbar_CreateMap( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_DeleteMap( ZUKAN_DETAIL_TOUCHBAR_WORK* work );

// FORM
static void Zukan_Detail_Touchbar_CreateForm( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
static void Zukan_Detail_Touchbar_DeleteForm( ZUKAN_DETAIL_TOUCHBAR_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
ZUKAN_DETAIL_TOUCHBAR_WORK* ZUKAN_DETAIL_TOUCHBAR_Init( HEAPID heap_id, BOOL form_version )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*   work;

  // ���[�N����
  work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZUKAN_DETAIL_TOUCHBAR_WORK) );

  // ������
  work->heap_id           = heap_id;
  work->form_version      = form_version;
  
  // ���
  work->state = ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR;
  work->prev_type = ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL;
  work->type      = ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL;
  work->prev_disp = ZUKAN_DETAIL_TOUCHBAR_DISP_INFO;
  work->disp      = ZUKAN_DETAIL_TOUCHBAR_DISP_INFO;
  work->speed = ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE;

  // �����X�N���[���ʒu
  GFL_BG_SetScroll( ZKNDTL_BG_FRAME_M_TOUCHBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_DISAPPEAR );

  // �v���C�I���e�B
  GFL_BG_SetPriority( ZKNDTL_BG_FRAME_M_TOUCHBAR, ZKNDTL_BG_FRAME_PRI_M_TOUCHBAR );

  // �Z���A�N�^�[���j�b�g
	work->clunit	= GFL_CLACT_UNIT_Create( CLUNIT_WORK_NUM, 0, work->heap_id );
	GFL_CLACT_UNIT_SetDefaultRend( work->clunit );

  // NULL������
  work->tbwk = NULL;

  // �^�b�`�o�[
  Zukan_Detail_Touchbar_Create( work );
  ZKND_TBAR_SetActiveWhole( work->tbwk, FALSE );
  work->icon_ofs_pos_y = -SCROLL_Y_DISAPPEAR;
  Zukan_Detail_Touchbar_SetIconOfsPosY( work, work->icon_ofs_pos_y );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Touchbar_VBlankFunc, work, 1 );

  return work;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_Exit( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );
  
  // �^�b�`�o�[
  Zukan_Detail_Touchbar_Delete( work );

  // �Z���A�N�^�[���j�b�g
	GFL_CLACT_UNIT_Delete( work->clunit );

  // ���[�N�j��
  GFL_HEAP_FreeMemory( work );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_Main( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  switch( work->state )
  {
  case ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR:
    {
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR_TO_APPEAR:
    {
      if( work->scroll_wait_count == 0 )
      {
        if( GFL_BG_GetScrollY( ZKNDTL_BG_FRAME_M_TOUCHBAR ) >= SCROLL_Y_APPEAR )
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_M_TOUCHBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_APPEAR );
          work->icon_ofs_pos_y = SCROLL_Y_APPEAR;
          Zukan_Detail_Touchbar_SetIconOfsPosY( work, work->icon_ofs_pos_y );
          work->state = ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR;
          ZKND_TBAR_SetActiveWhole( work->tbwk, TRUE );
        }
        else
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_M_TOUCHBAR, GFL_BG_SCROLL_Y_INC, scroll_value[work->speed] );
          work->icon_ofs_pos_y -= scroll_value[work->speed];
          Zukan_Detail_Touchbar_SetIconOfsPosY( work, work->icon_ofs_pos_y );
          work->scroll_wait_count = scroll_wait[work->speed];
        }
      }
      else
      {
        work->scroll_wait_count--;
      }
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR:
    {
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR_TO_DISAPPEAR:
    {
      if( work->scroll_wait_count == 0 )
      {
        if( GFL_BG_GetScrollY( ZKNDTL_BG_FRAME_M_TOUCHBAR ) <= SCROLL_Y_DISAPPEAR )
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_M_TOUCHBAR, GFL_BG_SCROLL_Y_SET, SCROLL_Y_DISAPPEAR );
          work->icon_ofs_pos_y = -SCROLL_Y_DISAPPEAR;
          Zukan_Detail_Touchbar_SetIconOfsPosY( work, work->icon_ofs_pos_y );
          work->state = ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR;
        }
        else
        {
          GFL_BG_SetScrollReq( ZKNDTL_BG_FRAME_M_TOUCHBAR, GFL_BG_SCROLL_Y_DEC, scroll_value[work->speed] );
          work->icon_ofs_pos_y += scroll_value[work->speed];
          Zukan_Detail_Touchbar_SetIconOfsPosY( work, work->icon_ofs_pos_y );
          work->scroll_wait_count = scroll_wait[work->speed];
        }
      }
      else
      {
        work->scroll_wait_count--;
      }
    }
    break;
  }

  ZKND_TBAR_Main( work->tbwk );

  // GENERAL�̃p���b�g�A�j��
  if( work->type == ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL )
  {
    Zukan_Detail_Touchbar_MainGeneral( work );
    Zukan_Detail_Touchbar_AnimeMainGeneral( work );
  }
}


//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_SetType(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   ZUKAN_DETAIL_TOUCHBAR_TYPE  type, 
                   ZUKAN_DETAIL_TOUCHBAR_DISP  disp )
{
  work->prev_type = work->type;
  work->type = type;
  work->prev_disp = work->disp;
  work->disp = disp;

  Zukan_Detail_Touchbar_Delete( work );
  Zukan_Detail_Touchbar_Create( work );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
ZUKAN_DETAIL_TOUCHBAR_STATE ZUKAN_DETAIL_TOUCHBAR_GetState( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  return work->state;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_Appear( ZUKAN_DETAIL_TOUCHBAR_WORK* work, ZUKAN_DETAIL_TOUCHBAR_SPEED speed )
{
  work->speed = speed;

  if( work->state != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
  {
    work->state = ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR_TO_APPEAR;
    work->scroll_wait_count = 0;
  }
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_Disappear( ZUKAN_DETAIL_TOUCHBAR_WORK* work, ZUKAN_DETAIL_TOUCHBAR_SPEED speed )
{
  work->speed = speed;

  if( work->state != ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR )
  {
    work->state = ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR_TO_DISAPPEAR;
    work->scroll_wait_count = 0;
    
    ZKND_TBAR_SetActiveWhole( work->tbwk, FALSE );
  }
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
ZKNDTL_COMMAND ZUKAN_DETAIL_TOUCHBAR_GetTrg( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  int icon_id = ZKND_TBAR_GetTrg( work->tbwk );
  ZKNDTL_COMMAND cmd = ZKNDTL_CMD_NONE;
  u8 i;

  if( icon_id != ZKND_TBAR_SELECT_NONE )
  {
    switch( work->type )
    {
    case ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL:
    case ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP:
    case ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM:
      {
        for( i=0; i<work->icon_set_num; i++ )
        {
          if( icon_id == work->icon_set[i].cset->id )
          {
            cmd = work->icon_set[i].cset->cmd;;
            break;
          }
        }
      }
      break;
    }
  }

  return cmd;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_Unlock( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  ZKND_TBAR_UnlockWhole( work->tbwk );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   ZUKAN_DETAIL_TOUCHBAR_DISP  disp )
{
  if( work->type == ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL )
  {
    work->prev_disp = work->disp;
    work->disp = disp;

    // GENERAL�̃p���b�g�A�j��
    Zukan_Detail_Touchbar_AnimeExitGeneral( work );
    Zukan_Detail_Touchbar_AnimeInitGeneral( work );
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK* work = (ZUKAN_DETAIL_TOUCHBAR_WORK*)wk;
}

//-------------------------------------
/// �^�b�`�o�[
//=====================================
static void Zukan_Detail_Touchbar_Create( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  Zukan_Detail_Touchbar_Delete( work );

  switch( work->type )
  {
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL:
    {
      Zukan_Detail_Touchbar_CreateGeneral( work );
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP:
    {
      Zukan_Detail_Touchbar_CreateMap( work );
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM:
    {
      Zukan_Detail_Touchbar_CreateForm( work );
    }
    break;
  }
}
static void Zukan_Detail_Touchbar_Delete( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  switch( work->prev_type )
  {
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL:
    {
      Zukan_Detail_Touchbar_DeleteGeneral( work );
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP:
    {
      Zukan_Detail_Touchbar_DeleteMap( work );
    }
    break;
  case ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM:
    {
      Zukan_Detail_Touchbar_DeleteForm( work );
    }
    break;
  }
}

static void Zukan_Detail_Touchbar_SetIconOfsPosY( ZUKAN_DETAIL_TOUCHBAR_WORK* work, s16 ofs_pos_y )
{
  u8 i;
  for( i=0; i<work->icon_set_num; i++ )
  {
    GFL_CLACTPOS pos = work->icon_set[i].cset->pos;
    pos.y += ofs_pos_y;
    GFL_CLACT_WK_SetWldPos( work->icon_set[i].clwk, &pos );
  }
}

//-------------------------------------
/// GENERAL
//=====================================
static void Zukan_Detail_Touchbar_CreateGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  CLSYS_DRAW_TYPE draw_type;
  
  // ���C�� or �T�u
  draw_type = (ZKNDTL_BG_FRAME_M_TOUCHBAR < GFL_BG_FRAME0_S) ? (CLSYS_DRAW_MAIN) : (CLSYS_DRAW_SUB);
 
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    work->general_ncl = GFL_CLGRP_PLTT_RegisterEx(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCLR,
                            draw_type,
                            OBJ_PAL_POS_M_CUSTOM * 0x20, 0, OBJ_PAL_NUM_M_CUSTOM,
                            work->heap_id );	
    work->general_ncg = GFL_CLGRP_CGR_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCGR,
                            FALSE,
                            draw_type,
                            work->heap_id );
    work->general_nce = GFL_CLGRP_CELLANIM_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCER,
                            NARC_zukan_gra_info_info_obj_NANR,
                            work->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // ICON_SET��ICON_CONST_SET��ݒ肷��
  work->icon_set_num = GENERAL_ICON_MAX;
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].cset = &general_icon_const_set[i];
    } 
  }

  // �^�b�`�o�[�̐ݒ�
  {
    ZKND_TBAR_SETUP setup;
    
    // �A�C�R���̐ݒ�
    // �������
    ZKND_TBAR_ITEM_ICON icon_tbl[GENERAL_ICON_MAX];
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      icon_tbl[i].icon    = work->icon_set[i].cset->id;
      icon_tbl[i].pos     = work->icon_set[i].cset->pos;
      icon_tbl[i].width   = work->icon_set[i].cset->width;
    }
    // �ȉ��J�X�^���{�^���Ȃ�Γ���Ȃ��Ă͂����Ȃ����
    // �J�X�^���{�^��
    {
      u8 i;

      i=GENERAL_ICON_CUSTOM_FORM;
      {
        icon_tbl[i].cg_idx            = work->general_ncg;         // �L�������\�[�X
        icon_tbl[i].plt_idx           = work->general_ncl;         // �p���b�g���\�[�X
        icon_tbl[i].cell_idx          = work->general_nce;         // �Z�����\�[�X
        icon_tbl[i].active_anmseq     = 3;                         // �A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].noactive_anmseq   = 3;                        // �m���A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].push_anmseq       = 7;                        // �������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
        icon_tbl[i].key               = 0;                         // �L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
        icon_tbl[i].se                = SEQ_SE_DECIDE1;            // �������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�
      }
      i=GENERAL_ICON_CUSTOM_VOICE;
      {
        icon_tbl[i].cg_idx            = work->general_ncg;         // �L�������\�[�X
        icon_tbl[i].plt_idx           = work->general_ncl;         // �p���b�g���\�[�X
        icon_tbl[i].cell_idx          = work->general_nce;         // �Z�����\�[�X
        icon_tbl[i].active_anmseq     = 2;                         // �A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].noactive_anmseq   = 2;                        // �m���A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].push_anmseq       = 6;                        // �������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
        icon_tbl[i].key               = 0;                         // �L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
        icon_tbl[i].se                = SEQ_SE_DECIDE1;            // �������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�
      }
      i=GENERAL_ICON_CUSTOM_MAP;
      {
        icon_tbl[i].cg_idx            = work->general_ncg;         // �L�������\�[�X
        icon_tbl[i].plt_idx           = work->general_ncl;         // �p���b�g���\�[�X
        icon_tbl[i].cell_idx          = work->general_nce;         // �Z�����\�[�X
        icon_tbl[i].active_anmseq     = 1;                         // �A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].noactive_anmseq   = 1;                        // �m���A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].push_anmseq       = 5;                        // �������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
        icon_tbl[i].key               = 0;                         // �L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
        icon_tbl[i].se                = SEQ_SE_DECIDE1;            // �������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�
      }
      i=GENERAL_ICON_CUSTOM_INFO;
      {
        icon_tbl[i].cg_idx            = work->general_ncg;         // �L�������\�[�X
        icon_tbl[i].plt_idx           = work->general_ncl;         // �p���b�g���\�[�X
        icon_tbl[i].cell_idx          = work->general_nce;         // �Z�����\�[�X
        icon_tbl[i].active_anmseq     = 0;                         // �A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].noactive_anmseq   = 0;                        // �m���A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].push_anmseq       = 4;                        // �������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
        icon_tbl[i].key               = 0;                         // �L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
        icon_tbl[i].se                = SEQ_SE_DECIDE1;            // �������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�
      }
    }

    // �ݒ�\����
    // �����قǂ̑����{���\�[�X���������
    GFL_STD_MemClear( &setup, sizeof(ZKND_TBAR_SETUP) );

    setup.p_item		= icon_tbl;				                             // ��̑����
    setup.item_num	= NELEMS(icon_tbl);                            // �����������邩
    setup.p_unit		= work->clunit;										             // OBJ�ǂݍ��݂̂��߂�CLUNIT
    setup.bar_frm 	= ZKNDTL_BG_FRAME_M_TOUCHBAR;		               // BG�ǂݍ��݂̂��߂�BG��
    setup.bg_plt		= ZKNDTL_BG_PAL_POS_M_TOUCHBAR;                // BG�p���b�g
    setup.obj_plt  	= ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR;	           	 // OBJ�p���b�g
    setup.mapping	  = ZKNDTL_OBJ_MAPPING_M;   	                   // �}�b�s���O���[�h

    work->tbwk = ZKND_TBAR_Init( &setup, work->heap_id );
  }

  // ICON_SET��clwk��ݒ肷��
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].clwk = ZKND_TBAR_GetClwk( work->tbwk, work->icon_set[i].cset->id );
    } 
  }

  // �t�H��������o�[�W�������t�H�����Ȃ��o�[�W������
  {
    if( work->form_version )
    {
      // ���̂܂܂ł���
    }
    else
    {
      ZKND_TBAR_SetVisible( work->tbwk, work->icon_set[GENERAL_ICON_CUSTOM_FORM].cset->id, FALSE );
    }
  }

  // GENERAL�̃p���b�g�A�j��
  Zukan_Detail_Touchbar_AnimeBaseInitGeneral( work );
  Zukan_Detail_Touchbar_AnimeInitGeneral( work );
}
static void Zukan_Detail_Touchbar_DeleteGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  if( work->tbwk )
  {
    // GENERAL�̃p���b�g�A�j��
    Zukan_Detail_Touchbar_AnimeExitGeneral( work );
    Zukan_Detail_Touchbar_AnimeBaseExitGeneral( work );

    // �^�b�`�o�[�j��
    ZKND_TBAR_Exit( work->tbwk );

    // ���\�[�X�j��
    {	
      GFL_CLGRP_PLTT_Release( work->general_ncl );
      GFL_CLGRP_CGR_Release( work->general_ncg );
      GFL_CLGRP_CELLANIM_Release( work->general_nce );
    }
  }

  // NULL������
  work->tbwk = NULL;
}
static void Zukan_Detail_Touchbar_MainGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  u8 icon = GENERAL_ICON_MAX;
  int trg = GFL_UI_KEY_GetTrg();
  if( trg & PAD_KEY_LEFT )
  {
    switch( work->disp )
    {
    case ZUKAN_DETAIL_TOUCHBAR_DISP_INFO:
      {
        if( ZKND_TBAR_GetVisible( work->tbwk, work->icon_set[GENERAL_ICON_CUSTOM_FORM].cset->id ) )
          icon = GENERAL_ICON_CUSTOM_FORM;
        else
          icon = GENERAL_ICON_CUSTOM_MAP;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_MAP:
      {
        icon = GENERAL_ICON_CUSTOM_INFO;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE:
      {
        icon = GENERAL_ICON_CUSTOM_MAP;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_FORM:
      {
        icon = GENERAL_ICON_CUSTOM_VOICE;
      }
      break;
    }
  }
  else if( trg & PAD_KEY_RIGHT )
  {
    switch( work->disp )
    {
    case ZUKAN_DETAIL_TOUCHBAR_DISP_INFO:
      {
        icon = GENERAL_ICON_CUSTOM_MAP;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_MAP:
      {
        icon = GENERAL_ICON_CUSTOM_VOICE;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE:
      {
        if( ZKND_TBAR_GetVisible( work->tbwk, work->icon_set[GENERAL_ICON_CUSTOM_FORM].cset->id ) )
          icon = GENERAL_ICON_CUSTOM_FORM;
        else
          icon = GENERAL_ICON_CUSTOM_INFO;
      }
      break;
    case ZUKAN_DETAIL_TOUCHBAR_DISP_FORM:
      {
        icon = GENERAL_ICON_CUSTOM_INFO;
      }
      break;
    }
  }
  // �A�C�R�����L�[�ŉ��������Ƃɂ���
  if( icon != GENERAL_ICON_MAX )
  {
    ZKND_TBAR_PushByKey( work->tbwk, work->icon_set[icon].cset->id );
  } 
}

// GENERAL�̃p���b�g�A�j��
static void Zukan_Detail_Touchbar_AnimeBaseInitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  NNSG2dPaletteData* pal_data;
  void* buf = GFL_ARC_UTIL_LoadPalette( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_info_obj_NCLR, &pal_data, work->heap_id );
  u16* raw_data = pal_data->pRawData;
  GFL_STD_MemCopy( &raw_data[RES_PAL_POS_GENERAL_ANIME_START*0x10], work->pal_anime_general_start, 0x20 );
  GFL_STD_MemCopy( &raw_data[RES_PAL_POS_GENERAL_ANIME_END*0x10], work->pal_anime_general_end, 0x20 );
  GFL_HEAP_FreeMemory( buf );
  work->pal_anime_general_count = 0;
}
static void Zukan_Detail_Touchbar_AnimeBaseExitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  // �������Ȃ�
}
static void Zukan_Detail_Touchbar_AnimeInitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  u8 icon;
  GFL_CLWK* clwk;
  switch( work->disp )
  {
  case ZUKAN_DETAIL_TOUCHBAR_DISP_INFO:   icon = GENERAL_ICON_CUSTOM_INFO;    break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_MAP:    icon = GENERAL_ICON_CUSTOM_MAP;     break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE:  icon = GENERAL_ICON_CUSTOM_VOICE;   break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_FORM:   icon = GENERAL_ICON_CUSTOM_FORM;    break;
  }
  clwk = ZKND_TBAR_GetClwk( work->tbwk, work->icon_set[icon].cset->id );
  //GFL_CLACT_WK_SetPlttOffs( clwk, OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC, CLWK_PLTTOFFS_MODE_PLTT_TOP );  // ����ł������悤�Ƀp���b�g��ς�����̂����A���Â�
  GFL_CLACT_WK_SetPlttOffs( clwk, OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC, CLWK_PLTTOFFS_MODE_OAM_COLOR );
}
static void Zukan_Detail_Touchbar_AnimeExitGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  u8 icon;
  GFL_CLWK* clwk;
  switch( work->prev_disp )
  {
  case ZUKAN_DETAIL_TOUCHBAR_DISP_INFO:   icon = GENERAL_ICON_CUSTOM_INFO;    break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_MAP:    icon = GENERAL_ICON_CUSTOM_MAP;     break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE:  icon = GENERAL_ICON_CUSTOM_VOICE;   break;
  case ZUKAN_DETAIL_TOUCHBAR_DISP_FORM:   icon = GENERAL_ICON_CUSTOM_FORM;    break;
  }
  clwk = ZKND_TBAR_GetClwk( work->tbwk, work->icon_set[icon].cset->id );
  //GFL_CLACT_WK_SetPlttOffs( clwk, OBJ_PAL_OFFSET_GENERAL_ANIME_NONE, CLWK_PLTTOFFS_MODE_PLTT_TOP );  // �Â������ɖ߂�����f�U�C�i�[����̃p���b�g�A�j�����������Ȃ��Ȃ��Ă��܂����B
  GFL_CLACT_WK_SetPlttOffs( clwk, OBJ_PAL_OFFSET_GENERAL_ANIME_NONE, CLWK_PLTTOFFS_MODE_OAM_COLOR );
}
static void Zukan_Detail_Touchbar_AnimeMainGeneral( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  u8 i;
  fx16 cos;

  if( work->pal_anime_general_count + GENERAL_ANIME_ADD >= 0x10000 )
  {
    work->pal_anime_general_count = work->pal_anime_general_count + GENERAL_ANIME_ADD - 0x10000;
  }
  else
  {
    work->pal_anime_general_count = work->pal_anime_general_count + GENERAL_ANIME_ADD;
  }
  cos = ( FX_CosIdx( work->pal_anime_general_count ) + FX16_ONE ) / 2;  // 0<= <=1�ɂ��Ă���

  for( i=0; i<0x10; i++ )
  {
    u8 s_r = ( work->pal_anime_general_start[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 s_g = ( work->pal_anime_general_start[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 s_b = ( work->pal_anime_general_start[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    u8 e_r = ( work->pal_anime_general_end[i]   & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 e_g = ( work->pal_anime_general_end[i]   & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 e_b = ( work->pal_anime_general_end[i]   & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    u8 r = s_r + (((e_r-s_r)*cos)>>FX16_SHIFT);
    u8 g = s_g + (((e_g-s_g)*cos)>>FX16_SHIFT);
    u8 b = s_b + (((e_b-s_b)*cos)>>FX16_SHIFT);

    work->pal_anime_general_now[i] = GX_RGB( r, g, b );
  }

  if( ZKNDTL_BG_FRAME_M_TOUCHBAR < GFL_BG_FRAME0_S )
  {
    NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_OBJ_PLTT_MAIN,
        ( OBJ_PAL_POS_M_CUSTOM + OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC ) * 0x20,
        work->pal_anime_general_now,
        0x20 );
  }
  else
  {
    NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_BG_PLTT_SUB ,
        ( OBJ_PAL_POS_M_CUSTOM + OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC ) * 0x20,
        work->pal_anime_general_now,
        0x20 );
  }
}

//-------------------------------------
/// MAP
//=====================================
static void Zukan_Detail_Touchbar_CreateMap( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  CLSYS_DRAW_TYPE draw_type;
  
  // ���C�� or �T�u
  draw_type = (ZKNDTL_BG_FRAME_M_TOUCHBAR < GFL_BG_FRAME0_S) ? (CLSYS_DRAW_MAIN) : (CLSYS_DRAW_SUB);
 
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, work->heap_id );
    work->map_ncl = GFL_CLGRP_PLTT_RegisterEx(
                            handle,
                            NARC_townmap_gra_tmap_obj_NCLR,
                            draw_type,
                            OBJ_PAL_POS_M_CUSTOM * 0x20, 2, OBJ_PAL_NUM_M_CUSTOM,
                            work->heap_id );	
    work->map_ncg = GFL_CLGRP_CGR_Register(
                            handle,
                            NARC_townmap_gra_tmap_obj_d_NCGR,
                            FALSE,
                            draw_type,
                            work->heap_id );
    work->map_nce = GFL_CLGRP_CELLANIM_Register(
                            handle,
                            NARC_townmap_gra_tmap_obj_d_NCER,
                            NARC_townmap_gra_tmap_obj_d_NANR,
                            work->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // ICON_SET��ICON_CONST_SET��ݒ肷��
  work->icon_set_num = MAP_ICON_MAX;
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].cset = &map_icon_const_set[i];
    } 
  }

  // �^�b�`�o�[�̐ݒ�
  {
    ZKND_TBAR_SETUP setup;
 
    // �A�C�R���̐ݒ�
    // �������
    ZKND_TBAR_ITEM_ICON icon_tbl[MAP_ICON_MAX];
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      icon_tbl[i].icon    = work->icon_set[i].cset->id;
      icon_tbl[i].pos     = work->icon_set[i].cset->pos;
      icon_tbl[i].width   = work->icon_set[i].cset->width;
    }
  
    // �ݒ�\����
    // �����قǂ̑����{���\�[�X���������
    GFL_STD_MemClear( &setup, sizeof(ZKND_TBAR_SETUP) );

    setup.p_item		= icon_tbl;				                             // ��̑����
    setup.item_num	= NELEMS(icon_tbl);                            // �����������邩
    setup.p_unit		= work->clunit;										             // OBJ�ǂݍ��݂̂��߂�CLUNIT
    setup.bar_frm 	= ZKNDTL_BG_FRAME_M_TOUCHBAR;		               // BG�ǂݍ��݂̂��߂�BG��
    setup.bg_plt		= ZKNDTL_BG_PAL_POS_M_TOUCHBAR;                // BG�p���b�g
    setup.obj_plt  	= ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR;	           	 // OBJ�p���b�g
    setup.mapping	  = ZKNDTL_OBJ_MAPPING_M;   	                   // �}�b�s���O���[�h

    work->tbwk = ZKND_TBAR_Init( &setup, work->heap_id );
  }

  // ICON_SET��clwk��ݒ肷��
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].clwk = ZKND_TBAR_GetClwk( work->tbwk, work->icon_set[i].cset->id );
    } 
  }
}
static void Zukan_Detail_Touchbar_DeleteMap( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  if( work->tbwk )
  {
    // �^�b�`�o�[�j��
    ZKND_TBAR_Exit( work->tbwk );

    // ���\�[�X�j��
    {	
      GFL_CLGRP_PLTT_Release( work->map_ncl );
      GFL_CLGRP_CGR_Release( work->map_ncg );
      GFL_CLGRP_CELLANIM_Release( work->map_nce );
    }
  }

  // NULL������
  work->tbwk = NULL;
}

//-------------------------------------
/// FORM
//=====================================
static void Zukan_Detail_Touchbar_CreateForm( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  CLSYS_DRAW_TYPE draw_type;
  
  // ���C�� or �T�u
  draw_type = (ZKNDTL_BG_FRAME_M_TOUCHBAR < GFL_BG_FRAME0_S) ? (CLSYS_DRAW_MAIN) : (CLSYS_DRAW_SUB);
 
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    work->form_ncl = GFL_CLGRP_PLTT_RegisterEx(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCLR,
                            draw_type,
                            OBJ_PAL_POS_M_CUSTOM * 0x20, 0, OBJ_PAL_NUM_M_CUSTOM,
                            work->heap_id );	
    work->form_ncg = GFL_CLGRP_CGR_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCGR,
                            FALSE,
                            draw_type,
                            work->heap_id );
    work->form_nce = GFL_CLGRP_CELLANIM_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCER,
                            NARC_zukan_gra_info_info_obj_NANR,
                            work->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // ICON_SET��ICON_CONST_SET��ݒ肷��
  work->icon_set_num = FORM_ICON_MAX;
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].cset = &form_icon_const_set[i];
    } 
  }

  // �^�b�`�o�[�̐ݒ�
  {
    ZKND_TBAR_SETUP setup;
    
    // �A�C�R���̐ݒ�
    // �������
    ZKND_TBAR_ITEM_ICON icon_tbl[FORM_ICON_MAX];
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      icon_tbl[i].icon    = work->icon_set[i].cset->id;
      icon_tbl[i].pos     = work->icon_set[i].cset->pos;
      icon_tbl[i].width   = work->icon_set[i].cset->width;
    }
    // �ȉ��J�X�^���{�^���Ȃ�Γ���Ȃ��Ă͂����Ȃ����
    // �J�X�^���{�^��
    {
      u8 i;
      for( i=FORM_ICON_CUSTOM_EXCHANGE; i<=FORM_ICON_CUSTOM_EXCHANGE; i++ )
      {
        icon_tbl[i].cg_idx            = work->form_ncg;         // �L�������\�[�X
        icon_tbl[i].plt_idx           = work->form_ncl;         // �p���b�g���\�[�X
        icon_tbl[i].cell_idx          = work->form_nce;         // �Z�����\�[�X
        icon_tbl[i].active_anmseq     = 14;                         // �A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].noactive_anmseq   = 14;                        // �m���A�N�e�B�u�̂Ƃ��̃A�j��
        icon_tbl[i].push_anmseq       = 20;                        // �������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
        icon_tbl[i].key               = PAD_BUTTON_A;              // �L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
        icon_tbl[i].se                = SEQ_SE_DECIDE1;            // �������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�
      }
    }

    // �ݒ�\����
    // �����قǂ̑����{���\�[�X���������
    GFL_STD_MemClear( &setup, sizeof(ZKND_TBAR_SETUP) );

    setup.p_item		= icon_tbl;				                             // ��̑����
    setup.item_num	= NELEMS(icon_tbl);                            // �����������邩
    setup.p_unit		= work->clunit;										             // OBJ�ǂݍ��݂̂��߂�CLUNIT
    setup.bar_frm 	= ZKNDTL_BG_FRAME_M_TOUCHBAR;		               // BG�ǂݍ��݂̂��߂�BG��
    setup.bg_plt		= ZKNDTL_BG_PAL_POS_M_TOUCHBAR;                // BG�p���b�g
    setup.obj_plt  	= ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR;	           	 // OBJ�p���b�g
    setup.mapping	  = ZKNDTL_OBJ_MAPPING_M;   	                   // �}�b�s���O���[�h

    work->tbwk = ZKND_TBAR_Init( &setup, work->heap_id );
  }

  // ICON_SET��clwk��ݒ肷��
  {
    u8 i;
    for( i=0; i<work->icon_set_num; i++ )
    {
      work->icon_set[i].clwk = ZKND_TBAR_GetClwk( work->tbwk, work->icon_set[i].cset->id );
    } 
  }

  /*
  {
    // �p���b�g��0�Ԃ���ǂݍ���ł���΁A
    // ���̃p���b�g���ǂ��ɔz�u���Ă��悤��
    // �p���b�g�̃I�t�Z�b�g�����Ă��K�v�͂Ȃ��B

    // 1�Ԃ̃p���b�g���g���Ă���OBJ�B
    // ���̃p���b�g��1�Ԃ�8�Ԃɓǂݍ��ށB
    u32 ncl, ncg, nce;
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heap_id );
    ncl = GFL_CLGRP_PLTT_RegisterEx(
                    handle,
                    NARC_townmap_gra_tmap_obj_NCLR,
                    draw_type,
                    8 * 0x20, 1, 1,
                    heap_id );	
    ncg = GFL_CLGRP_CGR_Register(
                    handle,
                    NARC_townmap_gra_tmap_obj_d_NCGR,
                    FALSE,
                    draw_type,
                    heap_id );
    nce = GFL_CLGRP_CELLANIM_Register(
                    handle,
                    NARC_townmap_gra_tmap_obj_d_NCER,
                    NARC_townmap_gra_tmap_obj_d_NANR,
                    heap_id );
    GFL_ARC_CloseDataHandle( handle );

    {
      static GFL_CLWK* clwk;
		  GFL_CLWK_DATA cldata;
		  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		  cldata.pos_x	= 64;
	  	cldata.pos_y	= 64;
      cldata.anmseq = 6;
      clwk = GFL_CLACT_WK_Create( work->clunit, ncg, ncl, nce, &cldata, CLSYS_DEFREND_MAIN, heap_id );
		  GFL_CLACT_WK_SetAutoAnmFlag( clwk, TRUE );
      // �p���b�g�̃I�t�Z�b�g�����Ă����Ȃ��ƁA9�Ԃ̃p���b�g���g���ĕ`�悳���B
      GFL_CLACT_WK_SetPlttOffs( clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );  // 8�Ԃ̃p���b�g���g����B
      //GFL_CLACT_WK_SetPlttOffs( clwk, 0, CLWK_PLTTOFFS_MODE_OAM_COLOR );  // 9�Ԃ̃p���b�g���g����B
      //GFL_CLACT_WK_SetPlttOffs( clwk, 8, CLWK_PLTTOFFS_MODE_OAM_COLOR );  // �ǂ����g���邩�悭������Ȃ��B
      //GFL_CLACT_WK_SetPlttOffs( clwk, 1, CLWK_PLTTOFFS_MODE_OAM_COLOR );  // 10�Ԃ̃p���b�g���g����B
      // �^��
      // �Ⴆ��1�Ԃ�2�Ԃ̃p���b�g��2�{�g���悤��OBJ���������ꍇ�A
      // GFL_CLACT_WK_SetPlttOffs( clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );�Ƃ�����A������8�Ԃ�9�Ԃ��g�����낤���H
      // ���܂��������A8�Ԃ����g��Ȃ��A�Ȃ�Ă��Ƃɂ͂Ȃ�Ȃ����낤���H
    }
  }
  */
}
static void Zukan_Detail_Touchbar_DeleteForm( ZUKAN_DETAIL_TOUCHBAR_WORK* work )
{
  if( work->tbwk )
  {
    // �^�b�`�o�[�j��
    ZKND_TBAR_Exit( work->tbwk );

    // ���\�[�X�j��
    {	
      GFL_CLGRP_PLTT_Release( work->form_ncl );
      GFL_CLGRP_CGR_Release( work->form_ncg );
      GFL_CLGRP_CELLANIM_Release( work->form_nce );
    }
  }

  // NULL������
  work->tbwk = NULL;
}

