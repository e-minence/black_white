//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_editor.c
 *	@brief	���[���G�f�B�^�[	DS������
 *	@author	tomoya takahashi
 *	@date		2009.07.13
 *
 *	���W���[�����FRIAL_EDITOR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// �f�o�b�N�p�@�\
#ifdef PM_DEBUG

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "arc/arc_def.h"
#include "arc/debug/rail_editor.naix"

#include "field/fieldmap.h"
#include "event_fieldmap_control.h"
#include "field_player_nogrid.h"

#include "fieldmap_func.h"


#include "debug/gf_mcs.h"

#include "rail_editor.h"
#include "rail_editor_data.h"
#include "field_rail_loader_func.h"
#include "field_rail_func.h"
#include "fld_scenearea_loader_func.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[���G�f�B�^�J�e�S��ID
//=====================================
#define GFL_MCS_RAIL_EDITOR_CATEGORY_ID ( 20625 )


//-------------------------------------
///	�V�[�P���X
//=====================================
enum {
	RAIL_EDITOR_SEQ_INIT,				// ������
	RAIL_EDITOR_SEQ_CONNECT,		// �ڑ�
	RAIL_EDITOR_SEQ_LINK,		    // �����N
	RAIL_EDITOR_SEQ_RESET,			// ���Z�b�g�N��
	RAIL_EDITOR_SEQ_MAIN,				// ���N�G�X�g�Ή�
	RAIL_EDITOR_SEQ_CLOSE,			// �ؒf
	RAIL_EDITOR_SEQ_EXIT,				// �j��

} ;

// �ėp��M�o�b�t�@
#define RE_TMP_BUFF_SIZE	( 0xC000 )

//-------------------------------------
///	���[���A�G���A��M�o�b�t�@�T�C�Y
//=====================================
#define RE_MCS_BUFF_RAIL_SIZE	( 0xC000 )
#define RE_MCS_BUFF_AREA_SIZE	( 0x400 )
#define RE_MCS_BUFF_ATTR_SIZE	( 0xC000 )


//-------------------------------------
///	CIRCLE�ړ�
//=====================================
#define RM_DEF_ANGLE_MOVE	(0x100)
#define RM_DEF_LEN_MOVE		(8*FX32_ONE)
#define RM_DEF_CAMERA_LEN	( 0x10D000 )
#define RM_DEF_CAMERA_PITCH	( 0x800 )


//-------------------------------------
///	���[���`�惊�\�[�X
//=====================================
#define RM_DRAW_OBJ_RES_MAX	( FIELD_RAIL_TYPE_MAX )
#define RM_DRAW_OBJ_MAX	( 312 )

// position���\�[�X
enum
{
	// ���\�[�X
	RM_DRAW_OBJ_RES_POSITION_MDL = 0,
	RM_DRAW_OBJ_RES_POSITION_ICA,
	RM_DRAW_OBJ_RES_POSITION_IMA,
	RM_DRAW_OBJ_RES_POSITION_NUM,

	// �A�j��
	RM_DRAW_OBJ_ANM_POSITION_ICA = 0,
	RM_DRAW_OBJ_ANM_POSITION_IMA,
	RM_DRAW_OBJ_ANM_POSITION_NUM,
};
#define POSITION_ICA_ANMSPEED_MOVE	( FX32_CONST(5) )
#define POSITION_ICA_ANMSPEED	( FX32_HALF )
#define POSITION_IMA_ADDSPEED	( FX32_CONST(10) )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	��M���o�b�t�@
//=====================================
typedef struct {
	// �t���O��
	union
	{
		struct
		{
			u32	rail_recv:1;			// ���[������M
			u32	area_recv:1;			// �G���A����M
			u32	attr_recv:1;			// �A�g���r���[�g����M
			u32	select_recv:1;		// �I������M
			u32	rail_req:1;				// ���[����񑗐M���N�G�X�g
			u32	area_req:1;				// �G���A��񑗐M���N�G�X�g
			u32	player_req:1;			// �v���C���[��񑗐M���N�G�X�g
			u32	camera_req:1;			// �J������񑗐M���N�G�X�g
			u32	reset_req:1;			// ���Z�b�g���N�G�X�g
			u32 end_req:1;				// �I�����N�G�X�g
			u32 raillocation_req:1;	// ���[�����P�[�V�������M���N�G�X�g
			u32 attr_req:1;	      // �A�g���r���[�g���M���N�G�X�g
			u32	pad_data:20;
		};
		u32 flag;	
	};

	// �f�[�^��
	u32	rail[RE_MCS_BUFF_RAIL_SIZE/4];
	u32 rail_size;
	u32 area[RE_MCS_BUFF_AREA_SIZE/4];
	u32 area_size;
	u32 attr[RE_MCS_BUFF_ATTR_SIZE/4];
	u32 attr_size;
	RE_MCS_SELECT_DATA select;
} RE_RECV_BUFF;


//-------------------------------------
///	���[���G�f�B�^�[�C�x���g���[�N
//=====================================
typedef struct {
	FIELDMAP_WORK* p_fieldmap;		// �t�B�[���h�}�b�v
	RE_RECV_BUFF*	p_recv;						// ��M�o�b�t�@
	u32*					p_tmp_buff;// �ėp��M�o�b�t�@
	VecFx32				camera_target;
	const VecFx32* cp_field_default_target;
	VecFx32				default_target_offset;

	u32	last_control_type;
	RE_MCS_SELECT_DATA select_last;


	// ���[���`�惏�[�N
	FLDMAPFUNC_WORK* p_rail_draw;


  // FIELD_PLAYER_NOGRID_WORK
  FIELD_PLAYER_NOGRID* p_railplayer;

  const FIELD_RAIL_WORK* cp_default_railwork;
} DEBUG_RAIL_EDITOR;



//-------------------------------------
///	���\�[�X
//  60byte
//=====================================
typedef struct {
	u8  use;
	u8  rail_type;		// �|�C���g���A���C����
	u16 rail_index;		// �e�[�u���C���f�b�N�X
	GFL_G3D_OBJSTATUS trans;
} RE_RAIL_DRAW_OBJ;

//-------------------------------------
///	���[�����`�揈�����[�N
//=====================================
typedef struct {
	// ���\�[�X��
	GFL_G3D_RES* p_mdl[ RM_DRAW_OBJ_RES_MAX ];
	GFL_G3D_RND* p_rnd[ RM_DRAW_OBJ_RES_MAX ];
	GFL_G3D_OBJ* p_obj[ RM_DRAW_OBJ_RES_MAX ];

	// position
	GFL_G3D_RES* p_position[ RM_DRAW_OBJ_RES_POSITION_NUM ];
	GFL_G3D_ANM* p_positionanm[ RM_DRAW_OBJ_ANM_POSITION_NUM ];
	GFL_G3D_RND* p_positionrnd;
	GFL_G3D_OBJ* p_positionobj;
	VecFx32			position_last_pos;
	int					color_frame;

	// �^�[�Q�b�g�̕`��
	BOOL	draw_target;

  // ���[���\����ON�@OFF
  BOOL rail_draw_flag;
	
	// �`��I�u�W�F��
	RE_RAIL_DRAW_OBJ draw_obj[ RM_DRAW_OBJ_MAX ];
	
	// ������
	const DEBUG_RAIL_EDITOR* cp_parent;
} RE_RAIL_DRAW_WORK;


//FS_EXTERN_OVERLAY(mcs_lib);

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT DEBUG_RailEditorEvent(
    GMEVENT * p_event, int *  p_seq, void * p_work );

// ���[���`�揈��
static void RE_DRAW_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Draw(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Create(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Delete(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);

static void RE_DRAW_SetDrawTarget( RE_RAIL_DRAW_WORK* p_wk, BOOL flag );
static RE_RAIL_DRAW_OBJ* RE_DRAW_GetClearWork( RE_RAIL_DRAW_WORK* p_wk );
static void RE_DRAW_SetUpData( RE_RAIL_DRAW_WORK* p_wk, const FLDNOGRID_MAPPER* cp_nogridMapper );
static void RE_DRAW_ClearAllDrawObj( RE_RAIL_DRAW_WORK* p_wk );
static void RE_DRAW_ClearLineDrawObj( RE_RAIL_DRAW_WORK* p_wk, u32 line_idx );
static void RE_DRAW_SetUpLine( RE_RAIL_DRAW_WORK* p_wk, const FLDNOGRID_MAPPER* cp_nogridMapper, u32 line_idx );


static void RE_DRAW_DRAWOBJ_Clear( RE_RAIL_DRAW_OBJ* p_drawobj );
static BOOL RE_DRAW_DRAWOBJ_IsUse( const RE_RAIL_DRAW_OBJ* cp_drawobj );
static void RE_DRAW_DRAWOBJ_Set( RE_RAIL_DRAW_OBJ* p_drawobj, u32 rail_type, u32 rail_index, const VecFx32* cp_trans, const VecFx32* cp_distance, u32 width );
static void RE_DRAW_DRAWOBJ_Draw( RE_RAIL_DRAW_WORK* p_wk, RE_RAIL_DRAW_OBJ* p_drawobj );
static BOOL RE_DRAW_DRAWOBJ_IsLineIdx( const RE_RAIL_DRAW_OBJ* cp_drawobj, u32 index );

static const FLDMAPFUNC_DATA sc_RE_DRAW_FUNCDATA = 
{
	0, sizeof(RE_RAIL_DRAW_WORK),
	RE_DRAW_Create, 
	RE_DRAW_Delete, 
	RE_DRAW_Update, 
	RE_DRAW_Draw, 
};


// ��M�f�[�^�A�Ή��֐�
static void RE_Recv_ClearFlag( RE_RECV_BUFF* p_buff );
static BOOL RE_Recv_IsEnd( const RE_RECV_BUFF* cp_buff );
static BOOL RE_Recv_IsReset( const RE_RECV_BUFF* cp_buff );
static void RE_RecvControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Recv_Rail( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_Area( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_SelectData( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_RailReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_AreaReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_PlayerDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_CameraDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_ResetReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_EndReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_RailLocationReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_Attr( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_AttrReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );

// ��M���̔��f
static void RE_Reflect( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Area( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Attr( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Select( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpArea( DEBUG_RAIL_EDITOR* p_wk );

// �I�����A�R���g���[���֐�
static void RE_InputControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk );

static void RE_InitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk );

static void RE_ExitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk );

// ���M�֐�
static void RE_SendControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_RailData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_AreaData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_PlayerData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_CameraData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_RailLocationData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_AttrData( DEBUG_RAIL_EDITOR* p_wk );



//----------------------------------------------------------------------------
/**
 *	@brief	���[���G�f�B�^�[�C�x���g�J�n
 *
 *	@param	gsys				�V�X�e��
 *	@param	fieldmap		�t�B�[���h�}�b�v
 *
 *	@return	�C�x���g���[�N
 */
//-----------------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_RailEditor( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT* p_event;
  DEBUG_RAIL_EDITOR* p_wk;

  p_event = GMEVENT_Create(
      gsys, NULL, DEBUG_RailEditorEvent, sizeof(DEBUG_RAIL_EDITOR) );

	p_wk = GMEVENT_GetEventWork( p_event );
	p_wk->p_fieldmap = fieldmap;

	return p_event;
}


//-----------------------------------------------------------------------------
/**
 *			private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	���[���G�f�B�^�[�C�x���g
 *
 *	@retval GMEVENT_RES_CONTINUE = 0,
 *	@retval	GMEVENT_RES_FINISH,
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT DEBUG_RailEditorEvent( GMEVENT * p_event, int *  p_seq, void * p_work )
{
  DEBUG_RAIL_EDITOR* p_wk = p_work;

	switch( *p_seq )
	{
	// ������
	case RAIL_EDITOR_SEQ_INIT:

//		GFL_OVERLAY_Load(FS_OVERLAY_ID(mcs_lib));

		// �o�b�t�@�̃������m��
		p_wk->p_recv = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), sizeof(RE_RECV_BUFF) );
		p_wk->p_tmp_buff = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), RE_TMP_BUFF_SIZE );

		// �J�����^�[�Q�b�g�ݒ�
//		FIELD_CAMERA_BindTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->camera_target );


		// �f�t�H���g�^�[�Q�b�g�̕ύX
		p_wk->cp_field_default_target = FIELD_CAMERA_DEBUG_GetDefaultTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ) );

		// �f�t�H���g�^�[�Q�b�g�I�t�Z�b�g�擾
		FIELD_CAMERA_GetTargetOffset( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->default_target_offset );

		FIELD_CAMERA_DEBUG_SetDefaultTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->camera_target );

    // ���[�����샏�[�N�̎擾
    {
      FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
      
      p_wk->p_railplayer = FIELD_PLAYER_NOGRID_Create( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), FIELDMAP_GetHeapID(p_wk->p_fieldmap) );
      
      p_wk->cp_default_railwork = FIELD_RAIL_MAN_DEBUG_GetBindWork( FLDNOGRID_MAPPER_GetRailMan( p_mapper ) );
      FLDNOGRID_MAPPER_BindCameraWork( p_mapper, FIELD_PLAYER_NOGRID_GetRailWork( p_wk->p_railplayer ) );

      // ���W��ݒ�
      FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &p_wk->camera_target );

      // �`��^�X�N��o�^
      p_wk->p_rail_draw = FLDMAPFUNC_Create( FIELDMAP_GetFldmapFuncSys(p_wk->p_fieldmap), &sc_RE_DRAW_FUNCDATA );
      {
        RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	
        
        p_drawwk->cp_parent = p_wk;

        RE_DRAW_SetUpData( p_drawwk, p_mapper );

        p_drawwk->rail_draw_flag = TRUE;
      }
    }
		
		(*p_seq) = RAIL_EDITOR_SEQ_CONNECT;
		break;

	// �ڑ�
	case RAIL_EDITOR_SEQ_CONNECT:	
		if( GFL_MCS_Open() )
		{

			(*p_seq) = RAIL_EDITOR_SEQ_LINK;
		}
		break;

  // �����N
  case RAIL_EDITOR_SEQ_LINK:
		if( GFL_MCS_CheckLink( GFL_MCS_RAIL_EDITOR_CATEGORY_ID ) )
		{
			(*p_seq) = RAIL_EDITOR_SEQ_MAIN;
    }
    break;

	// ���Z�b�g�N��
	case RAIL_EDITOR_SEQ_RESET:
		(*p_seq) = RAIL_EDITOR_SEQ_MAIN;
		break;


	// ���N�G�X�g�Ή�
	case RAIL_EDITOR_SEQ_MAIN:

		// ��M���t���O���j��
		RE_Recv_ClearFlag( p_wk->p_recv );

		// ��M���Ή�
		RE_RecvControl( p_wk );

		// �I����M
		if( RE_Recv_IsEnd( p_wk->p_recv ) )
		{
			(*p_seq)++;
			break;
		}

		// ���Z�b�g��M
		if( RE_Recv_IsReset( p_wk->p_recv ) ){
			break;
		}
		
		// ��M��񔽉f
		RE_Reflect(p_wk);
		
		// �I�����A����Ή�
		RE_InputControl( p_wk );

		// ��񑗐M�Ǘ�
		RE_SendControl( p_wk );

		break;

	// �ؒf
	case RAIL_EDITOR_SEQ_CLOSE:			
		(*p_seq) ++;
		break;

	// �j��
	case RAIL_EDITOR_SEQ_EXIT:				


		GFL_MCS_Close();

//		GFL_OVERLAY_Unload( FS_OVERLAY_ID( mcs_lib ) );

		// �o�b�t�@�̃������j��
		GFL_HEAP_FreeMemory( p_wk->p_recv );
		GFL_HEAP_FreeMemory( p_wk->p_tmp_buff );

		// �j��
		FLDMAPFUNC_Delete( p_wk->p_rail_draw );


		// �f�t�H���g�^�[�Q�b�g�����Ƃɂ��ǂ�
		FIELD_CAMERA_DEBUG_SetDefaultTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), p_wk->cp_field_default_target );

    {
      FLDNOGRID_MAPPER* p_mapper;
     
      p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
      
      FIELD_PLAYER_NOGRID_Delete( p_wk->p_railplayer );
      if(p_wk->cp_default_railwork != NULL)
      {
        FLDNOGRID_MAPPER_BindCameraWork( p_mapper, p_wk->cp_default_railwork );
      }
    }
		
		return GMEVENT_RES_FINISH;

	default:
		GF_ASSERT( 0 );
		break;
	}

  return GMEVENT_RES_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	���[�����`�揈��	�쐬
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Create(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	static const u32 sc_res_id[ RM_DRAW_OBJ_RES_MAX ] = 
	{
		NARC_rail_editor_rail_editor_point_nsbmd,
		NARC_rail_editor_rail_editor_line_nsbmd,
	};
	static const u32 sc_positionres_id[ RM_DRAW_OBJ_RES_POSITION_NUM ] = 
	{
		NARC_rail_editor_rail_editor_position_nsbmd,
		NARC_rail_editor_rail_editor_position_nsbca,
		NARC_rail_editor_rail_editor_position_nsbma,
	};
	RE_RAIL_DRAW_WORK* p_wk = p_work;

	// �ŏ��̓^�[�Q�b�g�`�悵�Ȃ�
	p_wk->draw_target = FALSE;
	
	// �`�惊�\�[�X�ǂݍ���
	for( i=0; i<RM_DRAW_OBJ_RES_MAX; i++ )
	{
		
		p_wk->p_mdl[i] = GFL_G3D_CreateResourceArc( ARCID_RAIL_EDITOR, sc_res_id[i] );
		p_wk->p_rnd[i] = GFL_G3D_RENDER_Create( p_wk->p_mdl[i], 0, NULL );
		p_wk->p_obj[i] = GFL_G3D_OBJECT_Create( p_wk->p_rnd[i], NULL, 0 );
	}

	// �`�惊�\�[�X�ǂݍ���
	for( i=0; i<RM_DRAW_OBJ_RES_POSITION_NUM; i++ )
	{
		p_wk->p_position[i] = GFL_G3D_CreateResourceArc( ARCID_RAIL_EDITOR, sc_positionres_id[i] );
	}
	// �����_���[�쐬
	p_wk->p_positionrnd = GFL_G3D_RENDER_Create( p_wk->p_position[RM_DRAW_OBJ_RES_POSITION_MDL], 0, NULL );
	// �A�j������
	for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
	{
		p_wk->p_positionanm[i] = GFL_G3D_ANIME_Create( p_wk->p_positionrnd, p_wk->p_position[ RM_DRAW_OBJ_RES_POSITION_ICA+i ], 0 );
	}
	// �I�u�W�F�쐬
	p_wk->p_positionobj = GFL_G3D_OBJECT_Create( p_wk->p_positionrnd, p_wk->p_positionanm, RM_DRAW_OBJ_ANM_POSITION_NUM );
	for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
	{
		GFL_G3D_OBJECT_EnableAnime( p_wk->p_positionobj, i );
	}
	{
		FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
		FIELD_PLAYER_GetPos( p_player, &p_wk->position_last_pos );
	}


	// ���[�N�̃N���A
  RE_DRAW_ClearAllDrawObj( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����`�揈��	�j��
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Delete(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	RE_RAIL_DRAW_WORK* p_wk = p_work;

	// position���\�[�X�j��
	{
		// �I�u�W�F�쐬
		GFL_G3D_OBJECT_Delete( p_wk->p_positionobj );
		// �A�j������
		for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
		{
			GFL_G3D_ANIME_Delete( p_wk->p_positionanm[i] );
		}
		// �����_���[�쐬
		GFL_G3D_RENDER_Delete( p_wk->p_positionrnd );
		// �`�惊�\�[�X�ǂݍ���
		for( i=0; i<RM_DRAW_OBJ_RES_POSITION_NUM; i++ )
		{
			GFL_G3D_DeleteResource( p_wk->p_position[i] );
		}
	}
	
	// �`�惊�\�[�X�j��
	for( i=0; i<RM_DRAW_OBJ_RES_MAX; i++ )
	{
		GFL_G3D_OBJECT_Delete( p_wk->p_obj[i] );
		GFL_G3D_RENDER_Delete( p_wk->p_rnd[i] );
		GFL_G3D_DeleteResource( p_wk->p_mdl[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����`�揈��	�A�b�v�f�[�g
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	RE_RAIL_DRAW_WORK* p_wk = p_work;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    if( p_wk->rail_draw_flag )
    {
      p_wk->rail_draw_flag = FALSE;
    }
    else
    {
      p_wk->rail_draw_flag = TRUE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����`�揈��	�`��
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Draw(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	RE_RAIL_DRAW_WORK* p_wk = p_work;

  if( p_wk->rail_draw_flag == FALSE )
  {
  
    // ���[�N�̕`��
    for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
    {
      RE_DRAW_DRAWOBJ_Draw( p_wk, &p_wk->draw_obj[i] );
    }

  }
	
	// ��l���ʒu�ɒu��
	{
		FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
		GFL_G3D_OBJSTATUS trans = 
		{
			{0,0,0},
			{FX32_ONE, FX32_ONE, FX32_ONE},
			{FX32_ONE,0,0, 0,FX32_ONE,0, 0,0,FX32_ONE },
		};

		// ���W�̎擾
		FIELD_PLAYER_GetPos( p_player, &trans.trans );

		
		// �ړ����́A�A�j��,�ړ����ĂȂ��Ȃ�I��
		{
			fx32 dist;

			dist = VEC_Distance( &trans.trans, &p_wk->position_last_pos );
			p_wk->position_last_pos = trans.trans;
			if( dist )
			{
				GFL_G3D_OBJECT_LoopAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_ICA, POSITION_ICA_ANMSPEED_MOVE );

				if( (p_wk->color_frame+POSITION_IMA_ADDSPEED) < FX32_CONST(60) )
				{
					p_wk->color_frame += POSITION_IMA_ADDSPEED;
				}
				else
				{
					p_wk->color_frame = FX32_CONST(60);
				}
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_IMA, &p_wk->color_frame );
			}
			else
			{
				GFL_G3D_OBJECT_LoopAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_ICA, POSITION_ICA_ANMSPEED );
				if( (p_wk->color_frame-POSITION_IMA_ADDSPEED) > 0 )
				{
					p_wk->color_frame -= POSITION_IMA_ADDSPEED;
				}
				else
				{
					p_wk->color_frame = 0;
				}
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_IMA, &p_wk->color_frame );
			}
		}

		GFL_G3D_DRAW_DrawObject( p_wk->p_positionobj, &trans );
	}

	// �^�[�Q�b�g�ʒu�ɒu��
	if( p_wk->draw_target )
	{
		FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
    const GFL_G3D_CAMERA * cp_core_camera = FIELD_CAMERA_GetCameraPtr( p_camera );
		GFL_G3D_OBJSTATUS trans =   
		{
			{0,0,0},
			{FX32_ONE, FX32_ONE, FX32_ONE},
			{FX32_ONE,0,0, 0,FX32_ONE,0, 0,0,FX32_ONE },
		};
		GFL_G3D_CAMERA_GetTarget( cp_core_camera, &trans.trans );
		GFL_G3D_DRAW_DrawObject( p_wk->p_positionobj, &trans );
	}
	
}



//----------------------------------------------------------------------------
/**
 *	@brief	�^�[�Q�b�g�ʒu��`�悷��̂��ݒ�
 *	
 *	@param	p_wk		���[�N
 *	@param	flag		�t���O
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_SetDrawTarget( RE_RAIL_DRAW_WORK* p_wk, BOOL flag )
{
	p_wk->draw_target = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�󂢂Ă��郏�[�N���擾����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static RE_RAIL_DRAW_OBJ* RE_DRAW_GetClearWork( RE_RAIL_DRAW_WORK* p_wk )
{
	int i;

	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		if( RE_DRAW_DRAWOBJ_IsUse( &p_wk->draw_obj[i] ) == FALSE )
		{
			return &p_wk->draw_obj[i];
		}
	}

  OS_TPrintf( "RailEditor DrawOBJ Over!!!" );
			
	return &p_wk->draw_obj[0];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�Z�b�g�A�b�v
 *
 *	@param	p_wk				      ���[�N
 *	@param	cp_nogridMapper	  ���[�����
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_SetUpData( RE_RAIL_DRAW_WORK* p_wk, const FLDNOGRID_MAPPER* cp_nogridMapper )
{
	int  i;
  const FIELD_RAIL_LOADER* cp_loader = FLDNOGRID_MAPPER_GetRailLoader( cp_nogridMapper );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_nogridMapper );
  const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( cp_loader );
	RE_RAIL_DRAW_OBJ* p_drawobj;
	VecFx32 distance = {0,0,0};

	// �S���N���[��
  RE_DRAW_ClearAllDrawObj( p_wk );

	// �|�C���g
	for( i=0; i<cp_setting->point_count; i++ )
	{
		p_drawobj = RE_DRAW_GetClearWork( p_wk );
		
		RE_DRAW_DRAWOBJ_Set( p_drawobj, FIELD_RAIL_TYPE_POINT, i, &cp_setting->point_table[i].pos, &distance, 0 );
	}

	// ���C��
	for( i=0; i<cp_setting->line_count; i++ )
	{
    RE_DRAW_SetUpLine( p_wk, cp_nogridMapper, i );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  �S�`�您�Ԃ����̔j��
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_ClearAllDrawObj( RE_RAIL_DRAW_WORK* p_wk )
{
  int i;

	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		RE_DRAW_DRAWOBJ_Clear( &p_wk->draw_obj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̃��C���̏����N���A
 *  
 *	@param	p_wk        ���[�N
 *	@param	line_idx    ���C���C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_ClearLineDrawObj( RE_RAIL_DRAW_WORK* p_wk, u32 line_idx )
{
  int i;

	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
    if( RE_DRAW_DRAWOBJ_IsLineIdx( &p_wk->draw_obj[i], line_idx ) )
    {
  		RE_DRAW_DRAWOBJ_Clear( &p_wk->draw_obj[i] );
    }
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̃��C���̃Z�b�g�A�b�v
 *  
 *	@param	p_wk              ���[�N
 *	@param	cp_nogridMapper   �}�b�p�[
 *	@param	line_idx          ���C���C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_SetUpLine( RE_RAIL_DRAW_WORK* p_wk, const FLDNOGRID_MAPPER* cp_nogridMapper, u32 line_idx )
{
	int  i;
  const FIELD_RAIL_LOADER* cp_loader = FLDNOGRID_MAPPER_GetRailLoader( cp_nogridMapper );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_nogridMapper );
  const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( cp_loader );
	RE_RAIL_DRAW_OBJ* p_drawobj;
	const RAIL_POINT* cp_point_s;
	const RAIL_POINT* cp_point_e;
  const RAIL_LINEPOS_SET* cp_linepos;
	VecFx32 point_s, point_e;
	VecFx32 distance = {0,0,0};
  u32 width;

  // line_idx�̃��[�N���������炷�ׂď���
  RE_DRAW_ClearLineDrawObj( p_wk, line_idx );


  cp_linepos =  &cp_setting->linepos_table[ cp_setting->line_table[line_idx].line_pos_set ];
  
  // ����
  if( cp_linepos->func_index == FIELD_RAIL_LOADER_LINEPOS_FUNC_STRAIT )
  {
    p_drawobj = RE_DRAW_GetClearWork( p_wk );

    cp_point_s = &cp_setting->point_table[ cp_setting->line_table[line_idx].point_s ];
    point_s = cp_point_s->pos;
    cp_point_e = &cp_setting->point_table[ cp_setting->line_table[line_idx].point_e ];
    point_e = cp_point_e->pos;
    
    VEC_Subtract( &point_e, &point_s, &distance );

    // ���C��SEARCH
    width = 0;
    for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
    {
      if( cp_point_s->lines[i] == line_idx )
      {
        width = cp_point_s->width_ofs_max[i];
        break;
      }
    }
    
    RE_DRAW_DRAWOBJ_Set( p_drawobj, FIELD_RAIL_TYPE_LINE, line_idx, &point_s, &distance, width );
  }
  else
  {
    u32 line_ofs_max;
    VecFx32 pos1, pos2;
    RAIL_LOCATION location;


    //  �J�[�u
    // �O���b�h�T�C�Y�����[�v
    location.rail_index   = line_idx;
    location.type         = FIELD_RAIL_TYPE_LINE;
    location.key          = RAIL_KEY_UP;
    location.width_grid   = 0;
    location.line_grid    = 0;
    line_ofs_max          = FIELD_RAIL_MAN_GetLocationLineOfsMaxGrid( cp_railman, &location );
    line_ofs_max += 1;
    FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &location, &pos1 );
    VEC_Set( &pos2, pos1.x, pos1.y, pos1.z );

    for( i=1; i<line_ofs_max; i+=3 )
    {
      location.line_grid = i;
      FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &location, &pos2 );
      width = FIELD_RAIL_MAN_GetLocationWidthGrid( cp_railman, &location );

      p_drawobj = RE_DRAW_GetClearWork( p_wk );

      VEC_Subtract( &pos2, &pos1, &distance );

      RE_DRAW_DRAWOBJ_Set( p_drawobj, FIELD_RAIL_TYPE_LINE, line_idx, &pos1, &distance, RAIL_GRID_TO_OFS(width) );

      VEC_Set( &pos1, pos2.x, pos2.y, pos2.z );
    }
  }
}








//----------------------------------------------------------------------------
/**
 *	@brief	�`��I�u�W�F�@�N���A
 *
 *	@param	p_drawobj		�`��I�u�W�F
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Clear( RE_RAIL_DRAW_OBJ* p_drawobj )
{
	p_drawobj->use = FALSE;
	p_drawobj->rail_type = 0;
	p_drawobj->rail_index = 0;

}

//----------------------------------------------------------------------------
/**
 *	@brief	USE	�t���O�̃`�F�b�N
 *
 *	@param	cp_drawobj 
 */
//-----------------------------------------------------------------------------
static BOOL RE_DRAW_DRAWOBJ_IsUse( const RE_RAIL_DRAW_OBJ* cp_drawobj )
{
	return cp_drawobj->use;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`�您�Ԃ���	���ݒ�
 *
 *	@param	p_drawobj			�`�您�Ԃ���
 *	@param	rail_type			���[���^�C�v
 *	@param	rail_index		�e�[�u���C���f�b�N�X
 *	@param	cp_trans			�]�u���i���C���͎n�_�j
 *	@param	cp_way				�����Ƌ���
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Set( RE_RAIL_DRAW_OBJ* p_drawobj, u32 rail_type, u32 rail_index, const VecFx32* cp_trans, const VecFx32* cp_distance, u32 width )
{
	VecFx32 vec_n;
	
	GF_ASSERT( rail_type < RM_DRAW_OBJ_RES_MAX );
	p_drawobj->rail_type	= rail_type;
	p_drawobj->rail_index = rail_index;

	MTX_Identity33( &p_drawobj->trans.rotate );
	VEC_Set( &p_drawobj->trans.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	p_drawobj->trans.trans = *cp_trans;

	if( rail_type == FIELD_RAIL_TYPE_LINE )
	{
		GF_ASSERT( cp_distance );
		VEC_Set( &p_drawobj->trans.scale, FX32_CONST(width), FX32_ONE, VEC_Mag(cp_distance) );
		VEC_Normalize( cp_distance, &vec_n );
		GFL_CALC3D_MTX_GetVecToRotMtxXZ( &vec_n, &p_drawobj->trans.rotate );
	}

	p_drawobj->use = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`�您�Ԃ����@�`�揈��
 *
 *	@param	p_wk				���[���`��V�X�e��
 *	@param	p_drawobj		�I�u�W�F���[�N
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Draw( RE_RAIL_DRAW_WORK* p_wk, RE_RAIL_DRAW_OBJ* p_drawobj )
{
	if( p_drawobj->use )
	{
		GFL_G3D_DRAW_DrawObject( p_wk->p_obj[p_drawobj->rail_type], &p_drawobj->trans );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C���C���f�b�N�X�`�F�b�N
 *
 *	@param	cp_wk     ���[�N
 *	@param	index     �C���f�b�N�X
 *
 *	@retval TRUE  �͂�
 *	@retval FALSE ������
 */
//-----------------------------------------------------------------------------
static BOOL RE_DRAW_DRAWOBJ_IsLineIdx( const RE_RAIL_DRAW_OBJ* cp_drawobj, u32 index )
{
  if( cp_drawobj->use )
  {
    if( cp_drawobj->rail_type == FIELD_RAIL_TYPE_LINE )
    {
      if( cp_drawobj->rail_index == index )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}







// ��M�f�[�^�A�Ή��֐�
//----------------------------------------------------------------------------
/**
 *	@brief	��M�o�b�t�@�t���O���N���A
 *
 *	@param	p_buff �o�b�t�@
 */
//-----------------------------------------------------------------------------
static void RE_Recv_ClearFlag( RE_RECV_BUFF* p_buff )
{
	GF_ASSERT( p_buff );
	p_buff->flag = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��M�o�b�t�@�@�I���t���O�`�F�b�N
 *	
 *	@param	cp_buff		�o�b�t�@
 *
 *	@retval	TRUE	�I��
 *	@retval	FALSE	�r��
 */
//-----------------------------------------------------------------------------
static BOOL RE_Recv_IsEnd( const RE_RECV_BUFF* cp_buff )
{
	GF_ASSERT( cp_buff );
	return cp_buff->end_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��M�o�b�t�@	���Z�b�g�t���O�`�F�b�N
 *
 *	@param	cp_buff		�o�b�t�@
 *
 *	@retval	TRUE	���Z�b�g
 *	@retval	FALSE	���Z�b�g����Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL RE_Recv_IsReset( const RE_RECV_BUFF* cp_buff )
{
	GF_ASSERT( cp_buff );
	return cp_buff->reset_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��M���Ǘ�
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_RecvControl( DEBUG_RAIL_EDITOR* p_wk )
{
	const RE_MCS_HEADER* cp_header;
	BOOL result;
  int size;
	static void (* const cpRecv[RE_MCS_DATA_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size ) = 
	{
		NULL,
		NULL,
		RE_Recv_Rail,
		RE_Recv_Area,
    RE_Recv_Attr,
		NULL,
		NULL,
		RE_Recv_SelectData,
		RE_Recv_RailReq,
		RE_Recv_AreaReq,
    RE_Recv_AttrReq,
		RE_Recv_PlayerDataReq,
		RE_Recv_CameraDataReq,
		RE_Recv_ResetReq,
		RE_Recv_EndReq,
		NULL,
		RE_Recv_RailLocationReq
	};

	// ��M�f�[�^�����邩�`�F�b�N
  size = GFL_MCS_CheckReadable( GFL_MCS_RAIL_EDITOR_CATEGORY_ID );
  result = GFL_MCS_Read( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_wk->p_tmp_buff, RE_TMP_BUFF_SIZE );
  if( result )
  {
    cp_header = (const RE_MCS_HEADER*)p_wk->p_tmp_buff;
      
    // ���m�̃f�[�^������
    GF_ASSERT( cp_header->data_type < RE_MCS_DATA_TYPE_MAX );

    if( cpRecv[ cp_header->data_type ] )
    {
      cpRecv[ cp_header->data_type ]( p_wk, p_wk->p_tmp_buff, size );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����̎�M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_Rail( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_RAIL_DATA* cp_rail = cp_data;

	GF_ASSERT( cp_rail->header.data_type == RE_MCS_DATA_RAIL );

	p_wk->p_recv->rail_recv = TRUE;
	p_wk->p_recv->reset_req = cp_rail->reset;

	GF_ASSERT( size < RE_MCS_BUFF_RAIL_SIZE );
	GFL_STD_MemCopy( cp_rail, p_wk->p_recv->rail, size );

	TOMOYA_Printf( "rail_recv size = %d\n", size );
	p_wk->p_recv->rail_size = size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�G���A����M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_Area( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_AREA_DATA* cp_area = cp_data;

	GF_ASSERT( cp_area->header.data_type == RE_MCS_DATA_AREA );

	p_wk->p_recv->area_recv = TRUE;
	p_wk->p_recv->reset_req = cp_area->reset;

	GF_ASSERT( size < RE_MCS_BUFF_AREA_SIZE );
	GFL_STD_MemCopy( cp_area, p_wk->p_recv->area, size );
	p_wk->p_recv->area_size = size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�����̎�M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_SelectData( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_SELECT_DATA* cp_select = cp_data;

	GF_ASSERT( cp_select->header.data_type == RE_MCS_DATA_SELECTDATA );

	p_wk->p_recv->select_recv = TRUE;

	GF_ASSERT( size == sizeof(RE_MCS_SELECT_DATA) );
	GFL_STD_MemCopy( cp_select, &p_wk->p_recv->select, size );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[����񑗐M���N�G�X�g
 */
//-----------------------------------------------------------------------------
static void RE_Recv_RailReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RAIL_REQ );
	p_wk->p_recv->rail_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�G���A��񑗐M���N�G�X�g
 */
//-----------------------------------------------------------------------------
static void RE_Recv_AreaReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_AREA_REQ );
	p_wk->p_recv->area_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[��񑗐M���N�G�X�g
 */
//-----------------------------------------------------------------------------
static void RE_Recv_PlayerDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_PLAYER_REQ );
	p_wk->p_recv->player_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J������񑗐M���N�G�X�g
 */
//-----------------------------------------------------------------------------
static void RE_Recv_CameraDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_CAMERA_REQ );
	p_wk->p_recv->camera_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�@���N�G�X�g
 */
//-----------------------------------------------------------------------------
static void RE_Recv_ResetReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RESET_REQ );
	p_wk->p_recv->reset_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I���@���N�G�X�g��M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_EndReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_END_REQ );
	p_wk->p_recv->end_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����P�[�V�������M�@���N�G�X�g��M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_RailLocationReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RAILLOCATION_REQ );
	p_wk->p_recv->raillocation_req = TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g���̎�M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_Attr( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_ATTR_DATA* cp_attr = cp_data;

	GF_ASSERT( cp_attr->header.data_type == RE_MCS_DATA_ATTR );

	p_wk->p_recv->attr_recv = TRUE;

	GF_ASSERT( size < RE_MCS_BUFF_ATTR_SIZE );
	GFL_STD_MemCopy( cp_attr, p_wk->p_recv->attr, size );
	p_wk->p_recv->attr_size = size;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g���̑��M�@���N�G�X�g��M
 */
//-----------------------------------------------------------------------------
static void RE_Recv_AttrReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_ATTR_REQ );
	p_wk->p_recv->attr_req = TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	��M�����@���f	
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Reflect( DEBUG_RAIL_EDITOR* p_wk )
{
	if( p_wk->p_recv->rail_recv )
	{
		RE_Reflect_Rail( p_wk );
	}
	if( p_wk->p_recv->area_recv )
	{
		RE_Reflect_Area( p_wk );
	}
	if( p_wk->p_recv->select_recv )
	{
		RE_Reflect_Select( p_wk );
	}
  if( p_wk->p_recv->attr_recv )
  {
    RE_Reflect_Attr( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[������M���f
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_nogridMapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	RE_MCS_RAIL_DATA* p_data = (RE_MCS_RAIL_DATA*)p_wk->p_recv->rail;	
	void* p_setdata;

	// ���P�[�V�����擾
	FIELD_PLAYER_NOGRID_GetLocation( p_wk->p_railplayer, &location );

	// �m�[�O���b�h�}�b�v�Ƀf�[�^�ݒ�
	p_setdata = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID( p_wk->p_fieldmap ), (p_wk->p_recv->rail_size  - 8) );
	GFL_STD_MemCopy( p_data->rail, p_setdata, (p_wk->p_recv->rail_size  - 8) );
  FLDNOGRID_MAPPER_DEBUG_LoadRailBynary( p_nogridMapper, p_setdata, (p_wk->p_recv->rail_size  - 8) );

	// ���P�[�V�����ݒ�
  {
    const FIELD_RAIL_LOADER* cp_loader = FLDNOGRID_MAPPER_GetRailLoader( p_nogridMapper );
    const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( cp_loader );
    
    // ���C���C���f�b�N�X���͈͓��H
    if( cp_setting->line_count <= location.rail_index )
    {
      location.rail_index = 0;
    }
    // �O���b�h���O�ɖ߂�
    location.width_grid = 0;
    location.line_grid = 0;
  }
	FIELD_PLAYER_NOGRID_SetLocation( p_wk->p_railplayer, &location );

	// �`�攽�f
	{
		RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	
		
		RE_DRAW_SetUpData( p_drawwk, p_nogridMapper );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�G���A����M���f
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Area( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	RE_MCS_AREA_DATA* p_data = (RE_MCS_AREA_DATA*)p_wk->p_recv->area;	
	void* p_setdata;

	// area���[�_�[�ɐݒ�
	p_setdata = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID( p_wk->p_fieldmap ), (p_wk->p_recv->area_size - 8) );
	GFL_STD_MemCopy( p_data->area, p_setdata, (p_wk->p_recv->area_size - 8) );

  // �ݒ�
  FLDNOGRID_MAPPER_DEBUG_LoadAreaBynary( p_mapper, p_setdata, (p_wk->p_recv->area_size - 8) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g���̔��f
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Attr( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	RE_MCS_ATTR_DATA* p_data = (RE_MCS_ATTR_DATA*)p_wk->p_recv->attr;	
	void* p_setdata;

	// area���[�_�[�ɐݒ�
	p_setdata = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID( p_wk->p_fieldmap ), (p_wk->p_recv->attr_size - 4) );
	GFL_STD_MemCopy( p_data->attr, p_setdata, (p_wk->p_recv->attr_size - 4) );

  // �ݒ�
  FLDNOGRID_MAPPER_DEBUG_LoadAttrBynary( p_mapper, p_setdata, (p_wk->p_recv->attr_size - 4), FIELDMAP_GetHeapID(p_wk->p_fieldmap) );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�I�����̔��f
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Select( DEBUG_RAIL_EDITOR* p_wk )
{
	// �e�[�u�����C���f�b�N�X�̕ύX
	if( (p_wk->p_recv->select.select_data != p_wk->select_last.select_data) ||
			(p_wk->p_recv->select.select_index != p_wk->select_last.select_index) ||
			(p_wk->p_recv->select.select_seq == p_wk->select_last.select_seq) )
	{
		static void (*const cpJump[RE_SELECT_DATA_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
		{
			NULL,
			RE_JumpPoint,
			RE_JumpLine,
			RE_JumpArea,
		};

		GF_ASSERT( p_wk->p_recv->select.select_data < RE_SELECT_DATA_MAX );
		if( cpJump[p_wk->p_recv->select.select_data] )
		{
			cpJump[p_wk->p_recv->select.select_data]( p_wk );
		}
	}

	p_wk->select_last = p_wk->p_recv->select;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g�̏ꏊ�ɃW�����v
 */
//-----------------------------------------------------------------------------
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk )
{
	RAIL_LOCATION location;
	VecFx32 pos;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap ); 
	const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( FLDNOGRID_MAPPER_GetRailLoader( p_mapper ) );

	if( cp_setting->point_count > p_wk->p_recv->select.select_index )
	{

		location.type					= FIELD_RAIL_TYPE_POINT;
		location.rail_index		= p_wk->p_recv->select.select_index;
		location.line_grid			= 0;
		location.width_grid		= 0;
		location.key					= 0;
		FIELD_PLAYER_NOGRID_SetLocation( p_wk->p_railplayer, &location );

		// �ʒu��l���ɐݒ�
		FIELD_PLAYER_NOGRID_GetPos( p_wk->p_railplayer, &pos );
		// 
		FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���̊J�n�ʒu�ꏊ�ɃW�����v
 */
//-----------------------------------------------------------------------------
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk )
{
	RAIL_LOCATION location;
	VecFx32 pos;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap ); 
	const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( FLDNOGRID_MAPPER_GetRailLoader( p_mapper ) );

	if( cp_setting->line_count > p_wk->p_recv->select.select_index )
	{
		location.type					= FIELD_RAIL_TYPE_LINE;
		location.rail_index		= p_wk->p_recv->select.select_index;
		location.line_grid			= 0;
		location.width_grid		= 0;
		location.key					= 0;
		FIELD_PLAYER_NOGRID_SetLocation( p_wk->p_railplayer, &location );

		// �ʒu��l���ɐݒ�
		FIELD_PLAYER_NOGRID_GetPos( p_wk->p_railplayer, &pos );
		// 
		FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	area�ɋ߂��|�C���g�ɃW�����v
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_JumpArea( DEBUG_RAIL_EDITOR* p_wk )
{
	// �|�C���g�f�[�^���Ȃ߂āAarea�ʒu�ɋ߂��ꏊ�ɔ��
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	const FLD_SCENEAREA_LOADER* cp_arealoader = FLDNOGRID_MAPPER_GetSceneAreaLoader( p_mapper );
	const FLD_SCENEAREA_DATA* cp_data = FLD_SCENEAREA_LOADER_GetData( cp_arealoader );
	u32 data_num = FLD_SCENEAREA_LOADER_GetDataNum( cp_arealoader );

	// ���̃R���g���[�����[�h���A��]�ړ��Ȃ�΁A��]�p�x���n�_�̈ʒu�Ɏ����Ă����B���������R���g���[������B
	if( data_num > p_wk->p_recv->select.select_index )
	{
		//  ���̏�񂩂�A�W�����v
		if( cp_data[ p_wk->p_recv->select.select_index ].checkArea_func == FLD_SCENEAREA_AREACHECK_CIRCLE )
		{
			if( p_wk->p_recv->select.select_seq == RE_SELECT_DATA_SEQ_FREE_CIRCLE )
			{
				const FLD_SCENEAREA_CIRCLE_PARAM* cp_wk = (const FLD_SCENEAREA_CIRCLE_PARAM*)cp_data[ p_wk->p_recv->select.select_index ].area;
				VecFx32 target;
				VecFx32 pl_pos;
				u16 _rotate;

				_rotate = (u16)((u32)cp_wk->rot_start);

				FIELD_PLAYER_GetPos( p_player, &pl_pos );
				FIELD_CAMERA_GetTargetPos( p_camera, &target );
				FIELD_CAMERA_SetAngleYaw( p_camera, _rotate );
				pl_pos.x = target.x + FX_Mul( FX_SinIdx( _rotate ), cp_wk->rot_start );
				pl_pos.z = target.z + FX_Mul( FX_CosIdx( _rotate ), cp_wk->dist_max );
				FIELD_PLAYER_SetPos( p_player, &pl_pos );
			}
		}
	}
//	TOMOYA_Printf( "RE_JumpArea ������\n" );
}



//----------------------------------------------------------------------------
/**
 *	@brief	�I�����ɂ��A���̓R���g���[��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_InputControl( DEBUG_RAIL_EDITOR* p_wk )
{
	static void (*const cpInitInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_InitInputPoint_FreeNormal,
		RE_InitInputPoint_FreeCircle,
		RE_InitInputPoint_FreeGrid,
		RE_InitInputPoint_Rail,
		RE_InitInputCamera_Pos,
		RE_InitInputCamera_Target,
		RE_InitInputLinePos_CenterPos,
	};

	static void (*const cpExitInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_ExitInputPoint_FreeNormal,
		RE_ExitInputPoint_FreeCircle,
		RE_ExitInputPoint_FreeGrid,
		RE_ExitInputPoint_Rail,
		RE_ExitInputCamera_Pos,
		RE_ExitInputCamera_Target,
		RE_ExitInputLinePos_CenterPos,
	};
	
	static void (*const cpInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_InputPoint_FreeNormal,
		RE_InputPoint_FreeCircle,
		RE_InputPoint_FreeGrid,
		RE_InputPoint_Rail,
		RE_InputCamera_Pos,
		RE_InputCamera_Target,
		RE_InputLinePos_CenterPos,
	};

	// �ύX�`�F�b�N
	if( p_wk->p_recv->select.select_seq != p_wk->last_control_type )
	{
		if(cpExitInput[ p_wk->last_control_type ])
		{
			cpExitInput[ p_wk->last_control_type ]( p_wk );
		}

		if(cpInitInput[ p_wk->p_recv->select.select_seq ])
		{
			cpInitInput[ p_wk->p_recv->select.select_seq ]( p_wk );
		}
	}
	
	// �I����Ԃɂ��A����
	GF_ASSERT( p_wk->p_recv->select.select_seq < RE_SELECT_DATA_SEQ_TYPE_MAX);	
	if( cpInput[p_wk->p_recv->select.select_seq] )
	{
		cpInput[p_wk->p_recv->select.select_seq]( p_wk );
	}

	p_wk->last_control_type = p_wk->p_recv->select.select_seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g	���R�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	u16 yaw;

	// ���O���b�h�T�C�Y�ł̈ړ��ɂ���
	FIELD_PLAYER_NOGRID_Free_Move( p_player, GFL_UI_KEY_GetCont(), 0x4000 );

	// �J�����𓮂���
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
	{
		yaw = FIELD_CAMERA_GetAngleYaw( p_camera );

		yaw -= 182;

		FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
	}
	else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
	{
		yaw = FIELD_CAMERA_GetAngleYaw( p_camera );

		yaw += 182;

		FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
	}

	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
}

static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	VecFx32 target, pl_pos, sub;
	u16 yaw;
	fx32 len;
	fx32 camera_len;
	u32 key_repeat = GFL_UI_KEY_GetRepeat();
	u32 key_cont = GFL_UI_KEY_GetCont();
	s16 df_angle;
	s16 df_len;
	BOOL change = FALSE;
	fx32 pos_y;

	// ���[����񂩂�A�␳���Ă�����

	// �J�����^�[�Q�b�g�𒆐S�ɉ�]�ړ�
	FIELD_CAMERA_GetTargetPos( p_camera, &target );
	FIELD_PLAYER_GetPos( p_player, &pl_pos );
	pos_y = pl_pos.y;

	target.y	= pl_pos.y;
	len				= VEC_Distance( &target, &pl_pos );
	yaw = FIELD_CAMERA_GetAngleYaw( p_camera );

	camera_len = FIELD_CAMERA_GetAngleLen( p_camera );

	if( key_cont & PAD_BUTTON_B )
	{
		df_len		= 4*RM_DEF_LEN_MOVE;
		df_angle	= 4*RM_DEF_ANGLE_MOVE;
	}
	else
	{
		df_len		= RM_DEF_LEN_MOVE;
		df_angle	= RM_DEF_ANGLE_MOVE;
	}

 
	if( (key_cont & PAD_BUTTON_A) == 0 )
	{
		if( key_repeat & PAD_KEY_LEFT )
		{
			yaw -= df_angle;
			change = TRUE;
		}
		else if( key_repeat & PAD_KEY_RIGHT )
		{
			yaw += df_angle;
			change = TRUE;
		}
		else if( key_repeat & PAD_KEY_UP )
		{
			len += df_len;
			change = TRUE;
		}
		else if( key_repeat & PAD_KEY_DOWN )
		{
			len -= df_len;
			change = TRUE;
		}
		else if( key_repeat & PAD_BUTTON_Y )
		{
			pos_y -= 2*FX32_ONE;
			change = TRUE;
		}
		else if( key_repeat & PAD_BUTTON_X )
		{
			pos_y += 2*FX32_ONE;
			change = TRUE;
		}
	}
	else
	{
		// yaw ���������@�����@�Ł@�^�[�Q�b�g����
		if( key_repeat & PAD_KEY_UP )
		{
			change = TRUE;
			target.z += FX_Mul( FX_CosIdx( yaw ), -4*FX32_ONE );
      target.x += FX_Mul( FX_SinIdx( yaw ), -4*FX32_ONE );
		}
		else if( key_repeat & PAD_KEY_DOWN )
		{ 
			change = TRUE;
			target.z += FX_Mul( FX_CosIdx( yaw ), 4*FX32_ONE );
      target.x += FX_Mul( FX_SinIdx( yaw ), 4*FX32_ONE );
		}
		else if( key_repeat & PAD_KEY_LEFT )
		{ 
			change = TRUE;
			target.z += FX_Mul( FX_CosIdx( yaw + 0x4000 ), -4*FX32_ONE );
      target.x += FX_Mul( FX_SinIdx( yaw + 0x4000 ), -4*FX32_ONE );
		}
		else if( key_repeat & PAD_KEY_RIGHT )
		{ 
			change = TRUE;
			target.z += FX_Mul( FX_CosIdx( yaw + 0x4000 ), 4*FX32_ONE );
      target.x += FX_Mul( FX_SinIdx( yaw + 0x4000 ), 4*FX32_ONE );
		}
		if( change )
		{
			FIELD_CAMERA_SetTargetPos( p_camera, &target );
		}
	}

	if( key_repeat & PAD_BUTTON_L )
	{
		camera_len += df_len;
		change = TRUE;
	}
	else if( key_repeat & PAD_BUTTON_R )
	{
		camera_len -= df_len;
		change = TRUE;
	}

	if( change )
	{
		// �p�x����A���W�����߂�
		pl_pos.x = target.x + FX_Mul( FX_SinIdx( yaw ), len );
		pl_pos.z = target.z + FX_Mul( FX_CosIdx( yaw ), len );
		pl_pos.y = pos_y;
		FIELD_PLAYER_SetPos( p_player, &pl_pos );

		FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
		FIELD_CAMERA_SetAngleLen( p_camera, camera_len );

    // ���[�������X�V
    {
      FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
      const FIELD_RAIL_LOADER * cp_rail_loader = FLDNOGRID_MAPPER_GetRailLoader( p_mapper );
      const RAIL_SETTING* cp_rail_setting = FIELD_RAIL_LOADER_GetData( cp_rail_loader );
      FIELD_RAIL_MAN * p_railman = FLDNOGRID_MAPPER_DEBUG_GetRailMan( p_mapper );
      RAIL_LOCATION location;
      u32 linepos_set;

      
      FIELD_PLAYER_NOGRID_GetLocation( p_wk->p_railplayer, &location	);
      if( location.type == FIELD_RAIL_TYPE_LINE )
      {
        linepos_set = cp_rail_setting->line_table[ location.rail_index ].line_pos_set;
        if( cp_rail_setting->linepos_table[ linepos_set ].func_index == FIELD_RAIL_LOADER_LINEPOS_FUNC_CURVE )
        {
          RAIL_POSFUNC_CURVE_WORK* p_curve_wk = (RAIL_POSFUNC_CURVE_WORK*)cp_rail_setting->linepos_table[ linepos_set ].work;
          p_curve_wk->x = target.x;
          p_curve_wk->y = target.y;
          p_curve_wk->z = target.z;

          OS_TPrintf( "Curve Center x[%d] y[%d] z[%d]\n", p_curve_wk->x, p_curve_wk->y, p_curve_wk->z );
          
          FIELD_RAIL_MAN_DEBUG_ChangeData( p_railman, cp_rail_setting );	//���[�����ύX
          // ���C���`����ύX
          RE_DRAW_SetUpLine( FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw ), p_mapper, location.rail_index );
        }
      }
    }
	}

	

	// ��������ɃJ������z�u
	//FIELD_CAMERA_SetAnglePitch( p_camera, RM_DEF_CAMERA_PITCH );
	//FIELD_CAMERA_SetAngleLen( p_camera, RM_DEF_CAMERA_LEN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g	���[���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );


	// ���[���V�X�e�����C��
  FLDNOGRID_MAPPER_Main( p_mapper );

  // �v���C���[���상�C��
  FIELD_PLAYER_NOGRID_Move( p_wk->p_railplayer, GFL_UI_KEY_GetTrg(), GFL_UI_KEY_GetCont() );

	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
//	FIELD_CAMERA_BindTarget( p_camera, &p_wk->camera_target );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�������W����
 */
//-----------------------------------------------------------------------------
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;

	// �J��������͉���ʂōs��
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);

  FIELD_SUBSCREEN_Main(subscreen);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�[�Q�b�g���W�̑���
 */
//-----------------------------------------------------------------------------
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;

	// �J��������͉���ʂōs��
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
  FIELD_SUBSCREEN_Main(subscreen);
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�����쏈���A	���S���W
 */
//-----------------------------------------------------------------------------
static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
	// ���S���W�𒲂ׂ₷���悤��
	RE_InputPoint_FreeNormal( p_wk );
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���R�ړ��@�O���b�h�P��
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );
  s16 gx, gy, gz;
  int key = GFL_UI_KEY_GetRepeat();
  VecFx32 pos;
  int dir;


  gx = MMDL_GetGridPosX( p_mmdl );
  gy = MMDL_GetGridPosY( p_mmdl );
  gz = MMDL_GetGridPosZ( p_mmdl );
  
  dir = MMDL_GetDirDisp( p_mmdl );

	// �O���b�h�T�C�Y�œ���
  if( key & PAD_KEY_UP )
  {
    gz --;
  }
  if( key & PAD_KEY_DOWN )
  {
    gz ++;
  }
  if( key & PAD_KEY_LEFT )
  {
    gx --;
  }
  if( key & PAD_KEY_RIGHT )
  {
    gx ++;
  }
  if( key & PAD_BUTTON_Y )
  {
    gy --;
  }
  if( key & PAD_BUTTON_X )
  {
    gy ++;
  }
  MMDL_InitGridPosition( p_mmdl, gx, gy, gz, dir );
  MMDL_GetVectorPos( p_mmdl, &pos );

  FIELD_PLAYER_SetPos( p_player, &pos );
  p_wk->camera_target = pos;
}



// ������
//----------------------------------------------------------------------------
/**
 *	@brief	���R�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	VecFx32 pos;

  OS_TPrintf( "::::::���R���� ������@::::::\n" );
  OS_TPrintf( "�㉺���E �ړ�\n" );
  OS_TPrintf( "YorX�@�㉺�ړ�\n" );
  OS_TPrintf( "B  �����ړ��i�A�g���r���[�g�Ȃǂ݂Ȃ��j\n" );
  OS_TPrintf( "LorR   �J����������]\n" );

	FIELD_CAMERA_BindDefaultTarget( p_camera );

	FLDNOGRID_MAPPER_SetActive(p_mapper, FALSE);
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, FALSE);

	FIELD_CAMERA_ChangeMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	// �v���C���[���W���n�[�t�O���b�h�P�ʂɂ���
	FIELD_PLAYER_GetPos( p_player, &pos );
	pos.x -= pos.x % GRID_HALF_FX32;
	pos.y -= pos.y % GRID_HALF_FX32;
	pos.z -= pos.z % GRID_HALF_FX32;
	FIELD_PLAYER_SetPos( p_player, &pos );

	// ��{�A���O���̐ݒ�
/*	FIELD_CAMERA_SetAnglePitch( p_camera, 45*182 );
	FIELD_CAMERA_SetAngleYaw( p_camera, 0 );
	FIELD_CAMERA_SetAngleLen( p_camera, 0x90000 );*/

}
//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�N���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	const FIELD_RAIL_LOADER * cp_rail_loader = FLDNOGRID_MAPPER_GetRailLoader( p_mapper );
	const RAIL_SETTING* cp_rail_setting = FIELD_RAIL_LOADER_GetData( cp_rail_loader );
	RAIL_LOCATION location;
	VecFx32 target, pl_pos, sub;
	u16 yaw;
	fx32 len;
	u32 linepos_set;

  OS_TPrintf( "::::::���R��]���� ������@::::::\n" );
  OS_TPrintf( "���E ��]�ړ�\n" );
  OS_TPrintf( "�㉺�@���S����̋�����ύX\n" );
  OS_TPrintf( "LorR   �J�����Y�[������\n" );
  OS_TPrintf( "A�{�㉺���E�@��]���S���W��ύX\n" );

	FLDNOGRID_MAPPER_SetActive(p_mapper, FALSE);
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, FALSE);

	FIELD_CAMERA_FreeTarget( p_camera );
	
	// ���[�����̃��C���̒��S�f�[�^���^�[�Q�b�g�ɐݒ�
	FIELD_CAMERA_GetTargetPos( p_camera, &target );	// ���̃^�[�Q�b�g�ŏ�����
	FIELD_PLAYER_NOGRID_GetLocation( p_wk->p_railplayer, &location	);
	if( location.type == FIELD_RAIL_TYPE_LINE )
	{
		linepos_set = cp_rail_setting->line_table[ location.rail_index ].line_pos_set;
		if( cp_rail_setting->linepos_table[ linepos_set ].func_index == FIELD_RAIL_LOADER_LINEPOS_FUNC_CURVE )
		{
			const RAIL_POSFUNC_CURVE_WORK* cp_curve_wk = (const RAIL_POSFUNC_CURVE_WORK*)cp_rail_setting->linepos_table[ linepos_set ].work;
			target.x = cp_curve_wk->x;
			target.y = cp_curve_wk->y;
			target.z = cp_curve_wk->z;
			FIELD_CAMERA_SetTargetPos( p_camera, &target );	// ���̃^�[�Q�b�g�ŏ�����
		}
	}

	// �J�����^�[�Q�b�g�𒆐S�ɉ�]�ړ�
	FIELD_PLAYER_GetPos( p_player, &pl_pos );

	target.y	= pl_pos.y;
	len				= VEC_Distance( &target, &pl_pos );

	FIELD_CAMERA_ChangeMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	VEC_Subtract( &pl_pos, &target, &sub );
	yaw				= FX_Atan2Idx( sub.x, sub.z );

	// �p�x����A���W�����߂�
	pl_pos.x = target.x + FX_Mul( FX_SinIdx( yaw ), len );
	pl_pos.z = target.z + FX_Mul( FX_CosIdx( yaw ), len );
	FIELD_PLAYER_SetPos( p_player, &pl_pos );

	// ��������ɃJ������z�u
//	FIELD_CAMERA_SetAnglePitch( p_camera, RM_DEF_CAMERA_PITCH );
	FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
//	FIELD_CAMERA_SetAngleLen( p_camera, len + RM_DEF_CAMERA_LEN );

	// �^�[�Q�b�g�̕`��ON
	{
		RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	

		RE_DRAW_SetDrawTarget( p_drawwk, TRUE );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���[���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );

	// ���[���ړ�
  FLDNOGRID_MAPPER_SetActive( p_mapper, TRUE );
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, TRUE);

	FIELD_CAMERA_FreeTarget( p_camera );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J��������ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	
	// ���[���ړ�
  FLDNOGRID_MAPPER_SetActive( p_mapper, FALSE );
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, FALSE);

	// �J��������͉���ʂōs��
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
	FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
	{ 
		void * inner_work;
		FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
		inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
		FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS);

		// ���샂�[�h�ݒ�
		FIELD_CAMERA_ChangeMode( cam, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�[�Q�b�g����
 */
//-----------------------------------------------------------------------------
static void RE_InitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	

	// ���[���ړ�
  FLDNOGRID_MAPPER_SetActive( p_mapper, FALSE );
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, FALSE);

	// �J��������͉���ʂōs��
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
	FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
	{ 
		void * inner_work;
		FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
		inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
		FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_TARGET_POS);

		// ���샂�[�h�ݒ�
		FIELD_CAMERA_ChangeMode( cam, FIELD_CAMERA_MODE_CALC_TARGET_POS );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief		�ړ����S���W�ݒ�
 */
//-----------------------------------------------------------------------------
static void RE_InitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );

	// ���R�ړ����[�h
	RE_InitInputPoint_FreeNormal( p_wk );

	// ��{�A���O���̐ݒ�
	// ���Ȃ�ォ��
	FIELD_CAMERA_SetAnglePitch( p_camera, 90*182 );
	FIELD_CAMERA_SetAngleYaw( p_camera, 0 );
	FIELD_CAMERA_SetAngleLen( p_camera, 0x150000 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[�O���b�h�ړ��̏�����
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	VecFx32 pos;
  int dir;


  OS_TPrintf( "::::::���R�O���b�h���� ������@::::::\n" );
  OS_TPrintf( "�㉺���E �ړ�\n" );
  OS_TPrintf( "Y X�@�����ړ�\n" );

	FIELD_CAMERA_BindDefaultTarget( p_camera );

	FLDNOGRID_MAPPER_SetActive(p_mapper, FALSE);
  FLDNOGRID_MAPPER_SetRailCameraActive(p_mapper, FALSE);

	FIELD_CAMERA_ChangeMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	// �v���C���[���W�O���b�h�ɍ��킹��
	FIELD_PLAYER_GetPos( p_player, &pos );
	pos.x = SIZE_GRID_FX32(pos.x);
	pos.y = SIZE_GRID_FX32(pos.y);
	pos.z = SIZE_GRID_FX32(pos.z);
  dir = MMDL_GetDirDisp( p_mmdl );
	MMDL_InitGridPosition( p_mmdl, pos.x, pos.y, pos.z, dir );
  MMDL_GetVectorPos( p_mmdl, &pos );

  FIELD_PLAYER_SetPos( p_player, &pos );
}

// �j��
//----------------------------------------------------------------------------
/**
 *	@brief	���R�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
}
//----------------------------------------------------------------------------
/**
 *	@brief	�~�`�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	// �^�[�Q�b�g�̕`��ON
	{
		RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	

		RE_DRAW_SetDrawTarget( p_drawwk, TRUE );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���[���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J��������
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
	FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

	{
		FIELD_SUBSCREEN_WORK * subscreen;
		

		// �J��������͉���ʂōs��
		subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
		FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_NORMAL);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�[�Q�b�g����
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);

	FIELD_CAMERA_DEBUG_GetBindSubScreenTarget( cam, &p_wk->camera_target );

	FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

	// �I�t�Z�b�g�ɕύX�l�������Ă��܂��̂ŁA�^�[�Q�b�g�̎����W�ɖ߂�
	FIELD_CAMERA_SetTargetOffset( cam, &p_wk->default_target_offset );

	{
		FIELD_SUBSCREEN_WORK * subscreen;
		

		// �J��������͉���ʂōs��
		subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
		FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_NORMAL);
	}

	// �J�����A���O�����[�h�ɖ߂�
	FIELD_CAMERA_ChangeMode( cam, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
	FIELD_CAMERA_SetTargetPos( cam, &p_wk->camera_target );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�u�ړ����S���W����
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h����I��
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_FreeGrid( DEBUG_RAIL_EDITOR* p_wk )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief	��񑗐M�Ǘ�
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_SendControl( DEBUG_RAIL_EDITOR* p_wk )
{
	if( p_wk->p_recv->rail_req )
	{
		// ���[�����̑��M
		RE_Send_RailData( p_wk );
	}
	else if( p_wk->p_recv->area_req )
	{
		// area���̑��M
		RE_Send_AreaData( p_wk );
	}
	else if( p_wk->p_recv->player_req )
	{
		// player���̑��M
		RE_Send_PlayerData( p_wk );
	}
	else if( p_wk->p_recv->camera_req )
	{
		// camera���̑��M
		RE_Send_CameraData( p_wk );
	}
	else if( p_wk->p_recv->raillocation_req )
	{
		// ���[�����P�[�V�������M
		RE_Send_RailLocationData( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����𑗐M
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Send_RailData( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	const FIELD_RAIL_LOADER* cp_loader = FLDNOGRID_MAPPER_GetRailLoader( p_mapper );
	const void* cp_data = FIELD_RAIL_LOADER_DEBUG_GetData( cp_loader );	
	u32 datasize = FIELD_RAIL_LOADER_DEBUG_GetDataSize( cp_loader );	
	RE_MCS_RAIL_DATA* p_senddata = (RE_MCS_RAIL_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_RAIL;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->rail, datasize );
	
  TOMOYA_Printf( "send raildata data size = 0x%x\n", datasize+4+sizeof(RE_MCS_HEADER) );
	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	area���𑗐M
 *	
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Send_AreaData( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	const FLD_SCENEAREA_LOADER* cp_loader = FLDNOGRID_MAPPER_GetSceneAreaLoader( p_mapper );
	const void* cp_data = FLD_SCENEAREA_LOADER_DEBUG_GetData( cp_loader );
	u32 datasize				= FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( cp_loader );
	RE_MCS_AREA_DATA* p_senddata = (RE_MCS_AREA_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_AREA;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->area, datasize );
	
  TOMOYA_Printf( "send areadata data size = 0x%x\n", datasize+4+sizeof(RE_MCS_HEADER) );
	
	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[���𑗐M
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Send_PlayerData( DEBUG_RAIL_EDITOR* p_wk )
{
	const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	const FIELD_CAMERA* cp_camera	= FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	RE_MCS_PLAYER_DATA* p_senddata = (RE_MCS_PLAYER_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos, target;
	BOOL result;
	
	p_senddata->header.data_type = RE_MCS_DATA_PLAYERDATA;

	FIELD_CAMERA_GetTargetPos( cp_camera, &target );
	FIELD_PLAYER_GetPos( cp_player, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;
	target.y = pos.y;	
	p_senddata->target_length = VEC_Distance( &target, &pos );

	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, sizeof(RE_MCS_PLAYER_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�������𑗐M
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void RE_Send_CameraData( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	const GFL_G3D_CAMERA * cp_core_camera = FIELD_CAMERA_GetCameraPtr( p_camera );
	RE_MCS_CAMERA_DATA* p_senddata = (RE_MCS_CAMERA_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos;
	VecFx32 target;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_CAMERADATA;

	FIELD_CAMERA_Main( p_camera, 0 );

	GFL_G3D_CAMERA_GetTarget( cp_core_camera, &target );
	VEC_Subtract( &target, &p_wk->default_target_offset, &target );	// �^�[�Q�b�g�I�t�Z�b�g���͌��炷
	GFL_G3D_CAMERA_GetPos( cp_core_camera, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;
	p_senddata->target_x = target.x;
	p_senddata->target_y = target.y;
	p_senddata->target_z = target.z;
	
	if( FIELD_CAMERA_GetMode( p_camera ) == FIELD_CAMERA_MODE_CALC_CAMERA_POS )
	{
		p_senddata->pitch = FIELD_CAMERA_GetAnglePitch( p_camera );
		p_senddata->yaw		= FIELD_CAMERA_GetAngleYaw( p_camera );
		p_senddata->len		= FIELD_CAMERA_GetAngleLen( p_camera );
	}
	else
	{
		VecFx32 camera_way;
		fx32 xz_dist;

		VEC_Subtract( &pos, &target, &camera_way );
		pos.y = 0;
		target.y = 0;
		xz_dist = VEC_Distance( &pos, &target );

		p_senddata->len		= VEC_Mag( &camera_way );
		p_senddata->pitch = FX_Atan2( -camera_way.y, xz_dist );
		p_senddata->yaw		= FX_Atan2( camera_way.x, camera_way.z );
	}

	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, sizeof(RE_MCS_CAMERA_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����P�[�V�����̑��M
 *
 *	@param	p_wk	��[��
 */
//-----------------------------------------------------------------------------
static void RE_Send_RailLocationData( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_MCS_RAILLOCATION_DATA* p_senddata = (RE_MCS_RAILLOCATION_DATA*)p_wk->p_tmp_buff;
	RAIL_LOCATION location;
	BOOL result;

	FIELD_PLAYER_NOGRID_GetLocation( p_wk->p_railplayer, &location );

	p_senddata->header.data_type = RE_MCS_DATA_RAILLOCATIONDATA;
	p_senddata->rail_type = location.type;
	p_senddata->index			= location.rail_index;


	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, sizeof(RE_MCS_RAILLOCATION_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g���̑��M
 */
//-----------------------------------------------------------------------------
static void RE_Send_AttrData( DEBUG_RAIL_EDITOR* p_wk )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_wk->p_fieldmap );
	const RAIL_ATTR_DATA* cp_attr = FLDNOGRID_MAPPER_GetRailAttrData( p_mapper );
	const void* cp_data = RAIL_ATTR_DEBUG_GetData( cp_attr );
	u32 datasize				= RAIL_ATTR_DEBUG_GetDataSize( cp_attr );
	RE_MCS_ATTR_DATA* p_senddata = (RE_MCS_ATTR_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_ATTR;
	GFL_STD_MemCopy( cp_data, p_senddata->attr, datasize );
	
  TOMOYA_Printf( "send attrdata data size = 0x%x\n", datasize+sizeof(RE_MCS_HEADER) );
	
	// ���M
	result = GFL_MCS_Write( GFL_MCS_RAIL_EDITOR_CATEGORY_ID, p_senddata, datasize+sizeof(RE_MCS_HEADER) );
	GF_ASSERT( result );	
}



#endif // PM_DEBUG

