//============================================================================
/**
 *  @file   dowsing.c
 *  @brief  �_�E�W���O
 *  @author Koji Kawada
 *  @data   2010.02.16
 *  @note   �t�B�[���h�̃T�u���
 *  ���W���[�����FDOWSING
 */
//============================================================================
#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamesystem.h"
#include "sound/pm_sndsys.h"
#include "infowin/infowin.h"
#include "app/app_menu_common.h"
#include "ui/touchbar.h"  // �A�C�R���̍��W�╝�A�A�j���ԍ��̒萔�l�����q��
#include "field/fldmmdl.h"
#include "field/fldmmdl.h"
#include "field/fieldmap.h"
#include "field/zonedata.h"
#include "../script_hideitem.h"
#include "../../../../resource/fldmapdata/flagwork/flag_define.h"  // FLAG_HIDEITEM_AREA_START�Q�Ƃ̂���

#include "../field_subscreen.h" 
#include "dowsing.h"             // field_subscreen.h���C���N���[�h���Ă���A���̃t�@�C�����C���N���[�h���ĉ������B

// �A�[�J�C�u
#include "arc_def.h"
#include "dowsing_gra.naix"

// �T�E���h
#include "sound/wb_sound_data.sadl"

// �I�[�o�[���C


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


//=============================================================================
/**
 *  �A�C�e���T�[�`
 */
//=============================================================================
// �I�[�o�[���C
FS_EXTERN_OVERLAY(notwifi);
// �_�E�W���O���N������Ƃ��́Anotwifi�����Ƀ��[�h����Ă���B

// �A�C�e���f�[�^
typedef struct
{
  u16 index;
  u8 world_flag;
  u8 revival_flag;
  u16 zone_id;
  u16 x, z;
}
ITEM_DATA;
// ��芸����HIDE_ITEM_DATA�Ɠ������g�ɂ��Ă���

// ���[�N
typedef struct
{
  const EVENTWORK*  event_wk;
  const ITEM_DATA*  item_data;
  u16               item_data_num;
  u16               search_no;  // ����item_data[search_no]����T�[�`����
}
ITEM_SEARCH_WORK;

// ����������
static ITEM_SEARCH_WORK* ItemSearchInit( HEAPID heap_id, const EVENTWORK* event_wk )
{
  ITEM_SEARCH_WORK* work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ITEM_SEARCH_WORK) );

  work->event_wk         = event_wk;
  work->item_data        = HIDEITEM_GetTable( &(work->item_data_num) );
  work->search_no        = 0;

  return work;
}

// �I������
static void ItemSearchExit( ITEM_SEARCH_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
}

// TRUE�̂Ƃ��A�C�e�������݂���
// FALSE�̂Ƃ��A�C�e�������݂��Ȃ�
// �A�C�e�������݂��邩�m�F����
static BOOL ItemSearchExist( ITEM_SEARCH_WORK* work, u16 item_data_index )
{
  u16 iflag = FLAG_HIDEITEM_AREA_START + item_data_index;
  return ( EVENTWORK_CheckEventFlag( (EVENTWORK*)(work->event_wk), iflag ) == FALSE );
}

// �ŏ�����T�[�`������
static void ItemSearchRestart( ITEM_SEARCH_WORK* work )
{
  work->search_no = 0;
}

// TRUE�̂Ƃ��L���ȃA�C�e���f�[�^��data�ɂ���Ă���
// FALSE�̂Ƃ������A�C�e���f�[�^���Ȃ�data�͕s��
// �A�C�e���f�[�^�𓾂�
static BOOL ItemSearchGet( ITEM_SEARCH_WORK* work, const ITEM_DATA** data )
{
  BOOL find = FALSE;
  const ITEM_DATA* idata;

  do
  {
    if( work->search_no >= work->item_data_num ) break;

    idata = &( work->item_data[ work->search_no ] );
    if( ItemSearchExist( work, idata->index ) )  // �A�C�e�������݂���
    {
      *data = idata;
      find = TRUE;
    }
    work->search_no++;
  }
  while( !find );

  return find;
}


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// BG�t���[��
#define BG_FRAME_S_REAR              (GFL_BG_FRAME3_S)        // �v���C�I���e�B3
#define BG_FRAME_S_TOUCHBAR          (GFL_BG_FRAME0_S)        // �v���C�I���e�B0
// BG�t���[���D��x
#define BG_FRAME_PRIO_S_REAR         (3)
#define BG_FRAME_PRIO_S_TOUCHBAR     (0)

// BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_REAR          = 1,
  BG_PAL_NUM_S_TOUCHBAR      = APP_COMMON_BAR_PLT_NUM,    // 1
  BG_PAL_NUM_S_INFOWIN       = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_S_REAR          = 0, 
  BG_PAL_POS_S_TOUCHBAR      = 1,
  BG_PAL_POS_S_INFOWIN       = 2,
};

// OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_KIT          = 1,
  OBJ_PAL_NUM_S_TOUCHBAR     = APP_COMMON_BARICON_PLT_NUM,    // 3
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_KIT          = 0,
  OBJ_PAL_POS_S_TOUCHBAR     = 1,
};

// ���C���V�[�P���X(init, exit��1�t���[���ōς܂���̂ŃV�[�P���X�ԍ��Ȃ��ł���)
enum
{
  SEQ_START,
  SEQ_,
  SEQ_END,
};

// �^�b�`�o�[�̏��
typedef enum
{
  TB_STATE_WAIT,
  TB_STATE_ANIM,
  TB_STATE_TRIG,
  TB_STATE_END,
}
TB_STATE;

// �\������OBJ
enum
{
  KIT_ROD_L    = 0,
  KIT_ROD_R    = 1,  // KIT_ROD_R��KIT_ROD_L�̃��\�[�X���؂�Ă���̂ŁA���̏��Ԃ͕ύX�s�B
  KIT_ARROW    = 2,
  KIT_ITEM     = 3,
  KIT_MAX,
};

static const struct 
{
  u8         cgr;
  u8         cell;
  u8         anim;
}
kit_data[KIT_MAX]	=
{
  {
	  NARC_dowsing_gra_dowsing_obj01_NCGR,
	  NARC_dowsing_gra_dowsing_obj01_NCER,
	  NARC_dowsing_gra_dowsing_obj01_NANR,
  },
  {
	  NARC_dowsing_gra_dowsing_obj01_NCGR,
	  NARC_dowsing_gra_dowsing_obj01_NCER,
	  NARC_dowsing_gra_dowsing_obj01_NANR,
  },
  {
	  NARC_dowsing_gra_dowsing_obj02_NCGR,
	  NARC_dowsing_gra_dowsing_obj02_NCER,
	  NARC_dowsing_gra_dowsing_obj02_NANR,
  },
  {
	  NARC_dowsing_gra_dowsing_obj03_NCGR,
	  NARC_dowsing_gra_dowsing_obj03_NCER,
	  NARC_dowsing_gra_dowsing_obj03_NANR,
  },
};

// ���b�h�̏��
#include "../../../../resource/dowsing_gra/dowsing_rod_enum.h"  // ROD��ROD_MAX����`����Ă���w�b�_�[�t�@�C��

static const struct
{
  u32            rod_l_anmseq :3;
  u32            rod_r_anmseq :3;
  u32            arrow_anmseq :3;
  u32            arrow_pos_x  :8;
  u32            arrow_pos_y  :8;
  u32            pan          :7;  // ( ( pan -8 ) * 16 )���g��( 0(-128) ... 8(0) ... 16(127) )
}
kid_info[ROD_MAX] =
{
  // rod_l_anmseq, rod_r_anmseq, arrow_anmseq, arrow_pos_x, arrow_pos_y,   pan 
  {             0,            0,            0,         128,          88,     8 }, 
  {             1,            1,            1,         128,         104,     8 }, 
  {             3,            3,            3,         128,          64,     0 }, 
  {             2,            2,            2,         128,          64,    16 }, 
  {             5,            5,            5,         128,          88,     4 }, 
  {             7,            7,            7,         128,         104,     4 }, 
  {             4,            4,            4,         128,          88,    12 }, 
  {             6,            6,            6,         128,         104,    12 }, 
  {             4,            5,            0,         128,         152,     0 },  // �A�C�e���\��
  {             5,            4,            0,         128,          88,     0 },  // ����\��
};

// �A�C�e�������t���Ă��Ȃ��Ƃ�
#define  ITEM_NONE    (0xFFFF)

// ���
typedef enum
{
  STATE_NEED_SEARCH,        // �A�C�e����T���K�v����
  STATE_SEARCH_IF_CHANGE,   // �v���C���[���ړ�������A�����Ă����A�C�e�����Ȃ��Ȃ����肵����A�A�C�e����T��
}
STATE;

// �^��ɂ���Ƃ���SE��炷�l
#define ABOVE_SE_PITCH (384)
#define ABOVE_SE_WAIT  (6)


//=============================================================================
/**
 *					�a�f�ݒ�
*/
//=============================================================================
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
	//SUB---------------------------
	{	
		BG_FRAME_S_TOUCHBAR,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_128x128,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, BG_FRAME_PRIO_S_TOUCHBAR, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,	//BG�̎��
		TRUE,	//�����\��
	},
	{	
		BG_FRAME_S_REAR,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, BG_FRAME_PRIO_S_REAR, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
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
///	���[�N�쐬�ő吔
//=====================================
#define GRAPHIC_OBJ_CLWK_CREATE_MAX	(8)


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// OBJ�Ŏg������
//=====================================
typedef struct
{
  u32         ncg;
  u32         nce;
  GFL_CLWK*   clwk;
}
KIT_SET;

//-------------------------------------
/// ���[�N
//=====================================
struct _DOWSING_WORK
{
  // ������ؗp
  HEAPID                      heap_id;
  HEAPID                      tempHeapId;  // �ꎞ�m�ۗp�q�[�v  // �֐����Ŋm�ۂ��ĉ������悤�ȂƂ��͂�����g�p����
  FIELD_SUBSCREEN_WORK*       subscreen_wk;
  FIELDMAP_WORK*              fieldmap_wk;

  // �����ō쐬
  GFL_CLUNIT*                 clunit;

  GFL_ARCUTIL_TRANSINFO       rear_tinfo;

  GFL_ARCUTIL_TRANSINFO       touchbar_tinfo;
  u32                         touchbar_ncl;
  u32                         touchbar_ncg;
  u32                         touchbar_nce;
  GFL_CLWK*                   touchbar_clwk;
  TB_STATE                    tb_state;

  u32                         kit_ncl;
  KIT_SET                     kit_set[KIT_MAX];

  u8*                         item_rod_table;

  ROD                         rod_prev;
  ROD                         rod_curr;
  ROD                         rod_draw_prev;
  u16                         item_prev;
  u16                         item_curr;
  s32                         player_grid_pos_x_prev;
  s32                         player_grid_pos_y_prev;
  s32                         player_grid_pos_z_prev;

  STATE                       state;

  SEPLAYER_ID                 above_seplayer_id;
  BOOL                        above_se_flag;      // �Đ�����TRUE
  u16                         above_se_wait;      // �҂t���[����

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �A�C�e���T�[�`
  ITEM_SEARCH_WORK*           item_search_wk;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Dowsing_VBlankFunc( GFL_TCB* tcb, void* wk );

// �O���t�B�b�N
static void Dowsing_GraphicInit( DOWSING_WORK* work );
static void Dowsing_GraphicExit( DOWSING_WORK* work );

// BG
static void Dowsing_BgInit( DOWSING_WORK* work );
static void Dowsing_BgExit( DOWSING_WORK* work );

// OBJ
static void Dowsing_ObjInit( DOWSING_WORK* work );
static void Dowsing_ObjExit( DOWSING_WORK* work );

// �^�b�`�o�[
static void Dowsing_TouchbarInit( DOWSING_WORK* work );
static void Dowsing_TouchbarExit( DOWSING_WORK* work );
static void Dowsing_TouchbarMain( DOWSING_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           init
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
DOWSING_WORK*    DOWSING_Init(
                            HEAPID                  heap_id,
                            HEAPID                  tempHeapId,
                            FIELD_SUBSCREEN_WORK*   subscreen_wk,
                            FIELDMAP_WORK*          fieldmap_wk    )
{
  DOWSING_WORK* work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "DOWSING_Init\n" );
  }
#endif

  // ���[�N
  work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(DOWSING_WORK) );

  // ����
  {
    work->heap_id            = heap_id;
    work->tempHeapId         = tempHeapId;
    work->subscreen_wk       = subscreen_wk;
    work->fieldmap_wk        = fieldmap_wk;
  }

  // �O���t�B�b�N
  Dowsing_GraphicInit( work );
  // BG
  Dowsing_BgInit( work );
  // OBJ
  Dowsing_ObjInit( work );
  // �^�b�`�o�[
  Dowsing_TouchbarInit( work );

  // �A�C�e���̈ʒu�ƃ��b�h�̏�Ԃ̑Ή��e�[�u��
  {
    u32 size;
    work->item_rod_table = GFL_ARC_UTIL_LoadEx(
             ARCID_DOWSING_GRA, NARC_dowsing_gra_dowsing_area_dat,
             FALSE, work->heap_id, &size );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Dowsing_VBlankFunc, work, 1 );

  // ������Ԃɂ���
  work->rod_prev       = ROD_MAX;    // ������Ԃŕ`�悪�X�V�����悤�ɁArod_curr�Ƃ͈قȂ�l�ɂ��Ă���
  work->rod_curr       = ROD_NONE;
  work->rod_draw_prev  = ROD_MAX;    // ������Ԃŕ`�悪�X�V�����悤�ɁArod_curr�Ƃ͈قȂ�l�ɂ��Ă���
  work->item_prev      = ITEM_NONE;
  work->item_curr      = ITEM_NONE;

  work->player_grid_pos_x_prev    = 0;
  work->player_grid_pos_y_prev    = 0;
  work->player_grid_pos_z_prev    = 0;

  work->state        = STATE_NEED_SEARCH;

  // ������Ԃŕ`����X�V���Ă���
  DOWSING_Draw( work, TRUE );

  // �A�C�e���T�[�`
  {
    GAMESYS_WORK*   gs_wk;
    GAMEDATA*       gamedata;
    EVENTWORK*      event_wk;

    gs_wk     = FIELDMAP_GetGameSysWork( work->fieldmap_wk );
    gamedata  = GAMESYSTEM_GetGameData( gs_wk );
    event_wk  = GAMEDATA_GetEventWork( gamedata );

    work->item_search_wk   = ItemSearchInit( work->heap_id, event_wk );
  }

  // OBJ���A���t�@�u�����h����
  {
    int obj_alpha = 8;
    G2S_SetBlendAlpha(
        GX_BLEND_PLANEMASK_OBJ,
        //GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
        // ���̐ݒ肾�ƁA�������ݒ肵�Ă��Ȃ�OBJ(�^�b�`�o�[��X�A�C�R����ʐM�A�C�R��)�܂ŁA���̂��������ɂȂ��Ă��܂����̂ŁA���L�̂悤�ɂ����B 
        GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG3,
        obj_alpha,
        16 - obj_alpha );
  }

  return work;
}


//------------------------------------------------------------------
/**
 *  @brief           exit
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void DOWSING_Exit( DOWSING_WORK* work )
{
#ifdef DEBUG_KAWADA
  {
    OS_Printf( "DOWSING_Exit\n" );
  }
#endif

  // OBJ���A���t�@�u�����h���Ă����̂����ɖ߂�
  {
    G2S_BlendNone();
  }

  // �A�C�e���T�[�`
  ItemSearchExit( work->item_search_wk );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �A�C�e���̈ʒu�ƃ��b�h�̏�Ԃ̑Ή��e�[�u��
  {
    GFL_HEAP_FreeMemory( work->item_rod_table );
  }

  // �^�b�`�o�[
  Dowsing_TouchbarExit( work );
  // OBJ
  Dowsing_ObjExit( work );
  // BG
  Dowsing_BgExit( work );
  // �O���t�B�b�N
  Dowsing_GraphicExit( work );

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}


//------------------------------------------------------------------
/**
 *  @brief           update
 *
 *  @param[in,out]   
 *  @param[in]       active    TRUE:����ʃA�N�e�B�u���
 *                             FALSE:��A�N�e�B�u(���̃C�x���g���F������󂯕t���Ă͂����Ȃ�)
 *
 *  @retval          
 */
//------------------------------------------------------------------
void DOWSING_Update( DOWSING_WORK* work, BOOL active )
{
  GAMESYS_WORK*   gs_wk;
  PLAYER_WORK*    player_wk;
  const VecFx32*  player_pos;
  s32             player_grid_pos_x;
  s32             player_grid_pos_y;
  s32             player_grid_pos_z;
  u32             zone_id;

  BOOL            search;

  if( !active ) return;

  // �O��̏�Ԃ��o���Ă���
  work->rod_prev = work->rod_curr;
  work->item_prev = work->item_curr;

  // ����
  gs_wk       = FIELDMAP_GetGameSysWork( work->fieldmap_wk );
  player_wk   = GAMESYSTEM_GetMyPlayerWork( gs_wk );
  player_pos  = PLAYERWORK_getPosition( player_wk );

  player_grid_pos_x = SIZE_GRID_FX32( player_pos->x );
  player_grid_pos_y = SIZE_GRID_FX32( player_pos->y );
  player_grid_pos_z = SIZE_GRID_FX32( player_pos->z );

  {
    GAMEDATA*     gamedata    = GAMESYSTEM_GetGameData( gs_wk );
    MAP_MATRIX*   map_mat     = GAMEDATA_GetMapMatrix( gamedata );

    zone_id = MAP_MATRIX_ZONE_ID_NON;

    if( MAP_MATRIX_CheckVectorPosRange( map_mat, player_pos->x, player_pos->z ) == TRUE )
    {
      zone_id = MAP_MATRIX_GetVectorPosZoneID( map_mat, player_pos->x, player_pos->z );
    }
  }
  
  search = FALSE;

  if( work->state == STATE_NEED_SEARCH )
  {
    work->state = STATE_SEARCH_IF_CHANGE;
    search = TRUE;
  }
  else
  {
    if(    player_grid_pos_x != work->player_grid_pos_x_prev
        || player_grid_pos_y != work->player_grid_pos_y_prev
        || player_grid_pos_z != work->player_grid_pos_z_prev )  // �v���C���[���ړ�������
    {
      search = TRUE;
    }
    else if( !ItemSearchExist( work->item_search_wk, work->item_prev ) )  // �O�񌩂��Ă����A�C�e�����Ȃ��Ȃ��Ă�����
    {
      work->item_prev = ITEM_NONE;
      search = TRUE;
    }
  }

  // �A�C�e���T�[�`
  if( search ) 
  {
    u32 distance_sq_min = 0xFFFFFFFF;
    const ITEM_DATA* distance_min_item_data = NULL;  // �ŏ������t�����Ƃ�NULL�łȂ��Ȃ�
    u16 distance_min_table_idx;
    
    s32 rect_x_min = player_grid_pos_x - AREA_ORIGIN_X;  // rect_x_min<= <rect_x_max
    s32 rect_x_max = player_grid_pos_x + AREA_WIDTH - AREA_ORIGIN_X;
    s32 rect_z_min = player_grid_pos_z - AREA_ORIGIN_Y;  // rect_z_min<= <rect_z_max
    s32 rect_z_max = player_grid_pos_z + AREA_HEIGHT - AREA_ORIGIN_Y;

    const ITEM_DATA* item_data;
    ItemSearchRestart( work->item_search_wk );
    
    while( ItemSearchGet( work->item_search_wk, &item_data ) )
    {
      u16 table_idx;  // item_rod_table�̃C���f�b�N�X

      // �����̂���]�[��ID�ƃA�C�e���̃]�[��ID��������
      if( zone_id == MAP_MATRIX_ZONE_ID_NON )
      {
        continue;
      }
      else
      {
         if( zone_id != (u32)(item_data->zone_id) )
         {
           if(    (item_data->world_flag)
               && ZONEDATA_IsFieldMatrixID( (u16)zone_id ) )
           {
             // �]�[��ID�͈Ⴄ���A�t�B�[���h�}�b�v�Ȃ̂œ������W�n
           }
           else
           {
             continue;
           }
         }
      }

      // �������͂ރT�[�`�͈͂̋�`�̒��ɂ��邩
      if( !(    rect_x_min<=item_data->x && item_data->x<rect_x_max
             && rect_z_min<=item_data->z && item_data->z<rect_z_max ) )
      {
        continue;
      }

      // �������͂ރT�[�`�͈͂̋�`�̒��ɂ���̂ŁAitem_rod_table�̃C���f�b�N�X�����߂���
      table_idx =   ( item_data->z - player_grid_pos_z + AREA_ORIGIN_Y ) * AREA_WIDTH \
                  + ( item_data->x - player_grid_pos_x + AREA_ORIGIN_X );

      // ���̏ꏊ��ROD_NONE(NN)�ł͂Ȃ���
      if( work->item_rod_table[ table_idx ] == ROD_NONE )
      {
        continue;
      }

      // �ł��߂���
      {
        u32 distance_sq =   ( item_data->x - player_grid_pos_x ) * ( item_data->x - player_grid_pos_x ) \
                          + ( item_data->z - player_grid_pos_z ) * ( item_data->z - player_grid_pos_z );
        if( distance_sq < distance_sq_min )
        {
          distance_sq_min = distance_sq;
          distance_min_item_data = item_data;
          distance_min_table_idx = table_idx;
        }
      }
    }

    // ����̏��
    if( distance_min_item_data )
    {
      work->rod_curr = work->item_rod_table[ distance_min_table_idx ];
      work->item_curr = distance_min_item_data->index;
    }
    else
    {
      work->rod_curr = ROD_NONE;
      work->item_curr = ITEM_NONE;
    }
  }

  // ����̏�ԂɕK�v�ȉ��̏������s��
  {
    BOOL can_play_se = TRUE;

    // �^��ɂ���Ƃ�����炷
    if( can_play_se )
    {
      if( work->rod_curr == ROD_ABOVE )
      {
        BOOL play = FALSE;
        if( !(work->above_se_flag) )
        {
          // �^��ɂ���Ƃ��������߂Ė炷
          play = TRUE;
          work->above_se_flag = TRUE;
        }
        else
        {
          // �^��ɂ���Ƃ�����炵��
          if( work->above_se_wait == ABOVE_SE_WAIT )
          {
            if( !PMSND_CheckPlaySE_byPlayerID( work->above_seplayer_id ) )
            {
              work->above_se_wait--;
            }
          }
          else if( work->above_se_wait == 0 )
          {
            play = TRUE;
          }
          else
          {
            work->above_se_wait--;
          }
        }
        if( play )
        {
          int pitch = ABOVE_SE_PITCH;
          work->above_seplayer_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_FLD_120 );
          PMSND_PlaySE_byPlayerID( SEQ_SE_FLD_120, work->above_seplayer_id );
          PMSND_SetStatusSE_byPlayerID( work->above_seplayer_id, PMSND_NOEFFECT, pitch, PMSND_NOEFFECT );
              // �l�͈̔͂́A-32768�`32767�ł��Bpitch�͐��̒l�ō������ցA���̒l�ŒႢ���֕ω����܂��B�}64�ł��傤�ǔ����ω����܂��B
          
          work->above_se_wait = ABOVE_SE_WAIT;
        }
        can_play_se = FALSE;
      }
      else
      {
        // �^��ɂ���Ƃ����͂����炳�Ȃ�
        work->above_se_flag = FALSE;
      }
    }

    // ���񏉂߂Č��t�����Ƃ��A�������́A�Ⴄ�A�C�e�������t�����Ƃ�
    // ���t��������炷
    if( can_play_se )
    {
      if( work->rod_curr != ROD_ABOVE )
      {
        if( work->item_curr != ITEM_NONE )
        {
          if( work->item_prev != work->item_curr )
          {
            SEPLAYER_ID seplayer_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_FLD_120 );
            int pan = ( (int)( kid_info[work->rod_curr].pan ) -8 ) * 16;
            if( pan < -127 )      pan = -127;  // �{����-128�܂�OK�����A�E�ƍ��킹��-127�܂łɂ��Ă���
            else if( pan >  127 ) pan =  127;
            PMSND_PlaySE_byPlayerID( SEQ_SE_FLD_120, seplayer_id );
            PMSND_SetStatusSE_byPlayerID( seplayer_id, PMSND_NOEFFECT, PMSND_NOEFFECT, pan );
                // �l�͈̔͂́A-128�`127�ł��B���̒l�ŉE�̕��ցA���̒l�ō��̕��ֈړ����܂��B
            can_play_se = FALSE;
          }
        }
      }
    }

    // ���b�h������������炷
    if( can_play_se )
    {
      if( work->rod_prev != work->rod_curr )
      {
        PMSND_PlaySE( SEQ_SE_FLD_122 );
        can_play_se = FALSE;
      }
    }
  }

  // ����̏�Ԃ����̑O��̏�ԂƂ��Ċo���Ă���
  work->player_grid_pos_x_prev    = player_grid_pos_x;
  work->player_grid_pos_y_prev    = player_grid_pos_y;
  work->player_grid_pos_z_prev    = player_grid_pos_z;

  // �^�b�`�o�[
  Dowsing_TouchbarMain( work );
}


//------------------------------------------------------------------
/**
 *  @brief           draw
 *
 *  @param[in,out]   
 *  @param[in]       active    TRUE:����ʃA�N�e�B�u���
 *                             FALSE:��A�N�e�B�u(���̃C�x���g���F������󂯕t���Ă͂����Ȃ�)
 *
 *  @retval          
 */
//------------------------------------------------------------------
void DOWSING_Draw( DOWSING_WORK* work, BOOL active )
{
  if( !active ) return;

  // �O���OBJ�̕\���ƍ����OBJ�̕\�����قȂ�ꍇ�A�`����X�V����
  if( work->rod_draw_prev != work->rod_curr )
  {
    GFL_CLACTPOS  pos;

    // �\��/��\��
    GFL_CLACT_WK_SetDrawEnable( work->kit_set[KIT_ARROW].clwk, (work->rod_curr!=ROD_NONE)  );
    GFL_CLACT_WK_SetDrawEnable( work->kit_set[KIT_ITEM].clwk,  (work->rod_curr==ROD_ABOVE) );
   
    // �ʒu
    pos.x = kid_info[work->rod_curr].arrow_pos_x;
    pos.y = kid_info[work->rod_curr].arrow_pos_y;
    GFL_CLACT_WK_SetPos( work->kit_set[KIT_ARROW].clwk, &pos, CLSYS_DEFREND_SUB );
   
    // �A�j��
    GFL_CLACT_WK_SetAnmSeq( work->kit_set[KIT_ROD_L].clwk, kid_info[work->rod_curr].rod_l_anmseq );
    GFL_CLACT_WK_SetAnmSeq( work->kit_set[KIT_ROD_R].clwk, kid_info[work->rod_curr].rod_r_anmseq );
    GFL_CLACT_WK_SetAnmSeq( work->kit_set[KIT_ARROW].clwk, kid_info[work->rod_curr].arrow_anmseq );

    // �����OBJ�̕\�������̑O���OBJ�̕\���Ƃ��Ċo���Ă���
    work->rod_draw_prev = work->rod_curr;
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
static void Dowsing_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  DOWSING_WORK* work = (DOWSING_WORK*)wk;
}

// �O���t�B�b�N
static void Dowsing_GraphicInit( DOWSING_WORK* work )
{
  // pokemon_wb/prog/src/field/fieldmap.c
  // fldmapdata_dispVram

  // BG
	//BG�ʐݒ�
	{	
		int i;
    for( i=GFL_BG_FRAME0_S; i<=GFL_BG_FRAME3_S; i++ )
    {
			GFL_BG_SetVisible( i, VISIBLE_OFF );
    }
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, sc_bgsetup[i].is_visible );

      GFL_BG_SetScroll( sc_bgsetup[i].frame, GFL_BG_SCROLL_X_SET, 0 );
      GFL_BG_SetScroll( sc_bgsetup[i].frame, GFL_BG_SCROLL_Y_SET, 0 );
		}
	}

  // OBJ
	//�V�X�e���쐬
	work->clunit	= GFL_CLACT_UNIT_Create( GRAPHIC_OBJ_CLWK_CREATE_MAX, 0, work->heap_id );
	GFL_CLACT_UNIT_SetDefaultRend( work->clunit );

	//�\��
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}
static void Dowsing_GraphicExit( DOWSING_WORK* work )
{
  // OBJ
	//�V�X�e���j��
	GFL_CLACT_UNIT_Delete( work->clunit );

  // BG
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}
}

// BG
static void Dowsing_BgInit( DOWSING_WORK* work )
{
  HEAPID  tempHeapId   = work->tempHeapId;
  
  GFL_ARCUTIL_TRANSINFO  tinfo;
  ARCHANDLE*  handle;

  // �ǂݍ���œ]��
  handle = GFL_ARC_OpenDataHandle( ARCID_DOWSING_GRA, tempHeapId );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
             handle,
             NARC_dowsing_gra_dowsing_bg01_NCLR,
             PALTYPE_SUB_BG,
             0                  * 0x20,
             BG_PAL_POS_S_REAR  * 0x20,
             BG_PAL_NUM_S_REAR  * 0x20,
             tempHeapId );

  tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                            handle,
                            NARC_dowsing_gra_dowsing_bg01_NCGR,
                            BG_FRAME_S_REAR,
                            0,
                            FALSE,
                            tempHeapId );
  GF_ASSERT_MSG( tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "DOWSING : BG�L�����̈悪����܂���ł����B\n" );

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
        handle,
        NARC_dowsing_gra_dowsing_bg01_NSCR,
        BG_FRAME_S_REAR,
        0,
        GFL_ARCUTIL_TRANSINFO_GetPos( tinfo ),
        0,
        FALSE,
        tempHeapId );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenV_Req( BG_FRAME_S_REAR );

  work->rear_tinfo = tinfo;
}
static void Dowsing_BgExit( DOWSING_WORK* work )
{
  GFL_BG_FreeCharacterArea(
      BG_FRAME_S_REAR,
      GFL_ARCUTIL_TRANSINFO_GetPos( work->rear_tinfo ),
      GFL_ARCUTIL_TRANSINFO_GetSize( work->rear_tinfo ) );
}

// OBJ
static void Dowsing_ObjInit( DOWSING_WORK* work )
{
  u8  i;

  ARCHANDLE*  handle;

  handle = GFL_ARC_OpenDataHandle( ARCID_DOWSING_GRA, work->tempHeapId );

  work->kit_ncl = GFL_CLGRP_PLTT_RegisterEx(
            handle,
            NARC_dowsing_gra_dowsing_obj01_NCLR,
            CLSYS_DRAW_SUB,
            OBJ_PAL_POS_S_KIT  * 0x20,
            0,
            OBJ_PAL_NUM_S_KIT,
            work->heap_id );

  for( i=0; i<KIT_MAX; i++ )
  {
    GFL_CLWK_DATA  clwk_data = { 0 };  // 0�ŏ�����

    if( i == KIT_ROD_R )  // KIT_ROD_L �̂��؂��
    {
      work->kit_set[i].ncg = work->kit_set[KIT_ROD_L].ncg;
      work->kit_set[i].nce = work->kit_set[KIT_ROD_L].nce;
    }
    else
    {
      work->kit_set[i].ncg = GFL_CLGRP_CGR_Register(
                 handle,
	               kit_data[i].cgr,
                 FALSE,
                 CLSYS_DRAW_SUB,
                 work->heap_id );

      work->kit_set[i].nce = GFL_CLGRP_CELLANIM_Register(
                 handle,
	               kit_data[i].cell,
	               kit_data[i].anim, 
                 work->heap_id );
    }

    clwk_data.softpri = i;  // OBJ�̃A���t�@�u�����h�͂ł��Ȃ��̂ŁA�ǂꂪ��ɕ\������邩�A�D��x�����߂Ă����B
                            // TWL_ProgramingManual.pdf    6.7 �J���[�������    OBJ ���m�̃��u�����f�B���O�͂ł��܂���B

    work->kit_set[i].clwk = GFL_CLACT_WK_Create(
                            work->clunit,
                            work->kit_set[i].ncg,
                            work->kit_ncl,
                            work->kit_set[i].nce,
                            &clwk_data,
                            CLSYS_DEFREND_SUB,
                            work->heap_id );
    
    GFL_CLACT_WK_SetAutoAnmFlag( work->kit_set[i].clwk, TRUE );
    GFL_CLACT_WK_SetObjMode( work->kit_set[i].clwk, GX_OAM_MODE_XLU );  // ������OBJ
  }
  
  GFL_ARC_CloseDataHandle( handle );

  // �ʒu���Œ�̂��̂́A�����Őݒ肵�Ă���
  {
    GFL_CLACTPOS  pos;

    pos.x =  80;  pos.y =  96;
    GFL_CLACT_WK_SetPos( work->kit_set[KIT_ROD_L].clwk, &pos, CLSYS_DEFREND_SUB );
    pos.x = 176;  pos.y =  96;
    GFL_CLACT_WK_SetPos( work->kit_set[KIT_ROD_R].clwk, &pos, CLSYS_DEFREND_SUB );
    pos.x = 128;  pos.y =  96;
    GFL_CLACT_WK_SetPos( work->kit_set[KIT_ITEM].clwk,  &pos, CLSYS_DEFREND_SUB );
  }
}
static void Dowsing_ObjExit( DOWSING_WORK* work )
{
  u8 i;
  for( i=0; i<KIT_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( work->kit_set[i].clwk );
   
    if( i == KIT_ROD_R )  // KIT_ROD_L �̂��؂�Ă��邾���Ȃ̂ŁA�������Ȃ�
    {
    }
    else
    {
      GFL_CLGRP_CELLANIM_Release( work->kit_set[i].nce );
      GFL_CLGRP_CGR_Release( work->kit_set[i].ncg );
    }
  }

  GFL_CLGRP_PLTT_Release( work->kit_ncl );
}

// �^�b�`�o�[
static void Dowsing_TouchbarInit( DOWSING_WORK* work )
{
  HEAPID  tempHeapId   = work->tempHeapId;
  
  // INFOWIN
  {
    GAMESYS_WORK*      gs_wk       = FIELDMAP_GetGameSysWork( work->fieldmap_wk );
    GAME_COMM_SYS_PTR  gcs_ptr     = GAMESYSTEM_GetGameCommSysPtr( gs_wk );

    INFOWIN_Init( BG_FRAME_S_TOUCHBAR, BG_PAL_POS_S_INFOWIN, gcs_ptr, work->heap_id );  // GFL_BG_AllocCharacterArea �ŃL�����̗̈���m�ۂ��Ă���
  }

  // �^�b�`�o�[
  {
    u16 pal_offset   = BG_PAL_POS_S_TOUCHBAR << 12;

    void*             buf;
    NNSG2dScreenData* screen;

    GFL_CLWK_DATA  clwk_data = { 0 };  // 0�ŏ�����

    // �n���h���I�[�v��
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), tempHeapId );
    
    // BG
    // �p���b�g
    GFL_ARCHDL_UTIL_TransVramPalette(
        handle,
        APP_COMMON_GetBarPltArcIdx(), 
        PALTYPE_SUB_BG, BG_PAL_POS_S_TOUCHBAR * 0x20, 
        BG_PAL_NUM_S_TOUCHBAR * 0x20,
        tempHeapId );
    // �L����
    work->touchbar_tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                                handle,
                                APP_COMMON_GetBarCharArcIdx(), 
                                BG_FRAME_S_TOUCHBAR,
                                0, FALSE, tempHeapId );  // 4 * GFL_BG_1CHRDATASIZ �����Ȃ�
    GF_ASSERT_MSG( work->touchbar_tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "DOWSING : BG�L�����̈悪����܂���ł����B\n" );

    // �X�N���[���ǂݍ���
    buf = GFL_ARCHDL_UTIL_LoadScreen(
              handle, APP_COMMON_GetBarScrnArcIdx(),
              FALSE, &screen, tempHeapId );
    // �X�N���[�����H
    {
      u16  i;
      u16* scr = (u16*)(screen->rawData);
      u16  chara_offset = GFL_ARCUTIL_TRANSINFO_GetPos( work->touchbar_tinfo );

      for( i=0; i<32*24; i++ )
      {
        scr[i] &= 0x0FFF;
        scr[i] = ( scr[i] + chara_offset ) | pal_offset;
      }
    }
    // �X�N���[���������݁��]��
    GFL_BG_WriteScreenExpand( BG_FRAME_S_TOUCHBAR,
                              0, 21, 32,  3,
                              screen->rawData,
                              0, 21, 32, 24 );
    GFL_BG_LoadScreenV_Req( BG_FRAME_S_TOUCHBAR );

    GFL_HEAP_FreeMemory( buf );


    // OBJ
    work->touchbar_ncl = GFL_CLGRP_PLTT_RegisterEx(
                             handle,
                             APP_COMMON_GetBarIconPltArcIdx(), 
                             CLSYS_DRAW_SUB,
                             OBJ_PAL_POS_S_TOUCHBAR * 0x20,
                             0,
                             OBJ_PAL_NUM_S_TOUCHBAR,
                             work->heap_id );
  
    work->touchbar_ncg = GFL_CLGRP_CGR_Register(
                             handle, 
                             APP_COMMON_GetBarIconCharArcIdx(),
                             FALSE, CLSYS_DRAW_SUB, work->heap_id );
    
    work->touchbar_nce = GFL_CLGRP_CELLANIM_Register( handle, 
                             APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K),
                             APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K),
                             work->heap_id );

    clwk_data.pos_x = TOUCHBAR_ICON_X_07;
    clwk_data.pos_y = TOUCHBAR_ICON_Y;
    work->touchbar_clwk = GFL_CLACT_WK_Create(
                              work->clunit,
                              work->touchbar_ncg,
                              work->touchbar_ncl,
                              work->touchbar_nce,
                              &clwk_data,
                              CLSYS_DEFREND_SUB,
                              work->heap_id );
    GFL_CLACT_WK_SetAutoAnmFlag( work->touchbar_clwk, TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->touchbar_clwk, APP_COMMON_BARICON_EXIT );  // active

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );

    // �^�b�`�o�[�̏��
    work->tb_state = TB_STATE_WAIT;
  }
}
static void Dowsing_TouchbarExit( DOWSING_WORK* work )
{
  // �^�b�`�o�[
  {
    // OBJ
    GFL_CLACT_WK_Remove( work->touchbar_clwk );
    GFL_CLGRP_CELLANIM_Release( work->touchbar_nce );
    GFL_CLGRP_CGR_Release( work->touchbar_ncg );
    GFL_CLGRP_PLTT_Release( work->touchbar_ncl );

    // BG
    GFL_BG_FreeCharacterArea(
        BG_FRAME_S_TOUCHBAR,
        GFL_ARCUTIL_TRANSINFO_GetPos( work->touchbar_tinfo ),
        GFL_ARCUTIL_TRANSINFO_GetSize( work->touchbar_tinfo ) );
  }

  // INFOWIN
  {
    INFOWIN_Exit();
  }
}
static void Dowsing_TouchbarMain( DOWSING_WORK* work )
{
  // INFOWIN
  {
    INFOWIN_Update();
  }

  // �^�b�`�o�[
  {
    switch( work->tb_state )
    {
    case TB_STATE_WAIT:
      {
        BOOL input_finish = FALSE;
        
        // �^�b�`����
        if( !input_finish )
        {
          u32 x, y;
          if( GFL_UI_TP_GetPointTrg( &x, &y ) )
          {
            if(    TOUCHBAR_ICON_X_07<=x && x<TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH
                && TOUCHBAR_ICON_Y   <=y && y<TOUCHBAR_ICON_Y   +TOUCHBAR_ICON_HEIGHT )
            {
    				  GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
              input_finish = TRUE;
            }
          }
        }
        // �L�[����
        if( !input_finish ) 
        {
          int trg = GFL_UI_KEY_GetTrg();
          if( trg & PAD_BUTTON_X )
          {
            GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
            input_finish = TRUE;
          }
        }

        // ���͂��������Ƃ�
        if( input_finish )
        {
          work->tb_state = TB_STATE_ANIM;
          GFL_CLACT_WK_SetAnmSeq( work->touchbar_clwk, APP_COMMON_BARICON_EXIT_ON );  // push
          PMSND_PlaySE( TOUCHBAR_SE_CLOSE );
        }
      }
      break;
    case TB_STATE_ANIM:
      {
        if( !GFL_CLACT_WK_CheckAnmActive( work->touchbar_clwk ) )
        {
          work->tb_state = TB_STATE_TRIG;
          //GFL_CLACT_WK_SetAnmSeq( work->touchbar_clwk, APP_COMMON_BARICON_EXIT_OFF );  // noactive
          GFL_CLACT_WK_SetAnmSeq( work->touchbar_clwk, APP_COMMON_BARICON_EXIT );  // active
        }
      }
      break;
    case TB_STATE_TRIG:
      {
        // 1�t���[������
			  FIELD_SUBSCREEN_Change( work->subscreen_wk, FIELD_SUBSCREEN_NORMAL );
        work->tb_state = TB_STATE_END;
      }
      break;
    case TB_STATE_END:
      {
        // �������Ȃ��ŏI����҂�
      }
      break;
    }
  }
}

