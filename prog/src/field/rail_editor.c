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

#include "field/fieldmap.h"
#include "event_fieldmap_control.h"
#include "field_player_nogrid.h"

#include "debug/gf_mcs.h"

#include "rail_editor.h"
#include "rail_editor_data.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�[�P���X
//=====================================
enum {
	RAIL_EDITOR_SEQ_INIT,				// ������
	RAIL_EDITOR_SEQ_CONNECT,		// �ڑ�
	RAIL_EDITOR_SEQ_RESET,			// ���Z�b�g�N��
	RAIL_EDITOR_SEQ_MAIN,				// ���N�G�X�g�Ή�
	RAIL_EDITOR_SEQ_CLOSE,			// �ؒf
	RAIL_EDITOR_SEQ_EXIT,				// �j��

} ;

// �ėp��M�o�b�t�@
#define RE_TMP_BUFF_SIZE	( 8198 )

//-------------------------------------
///	���[���A�G���A��M�o�b�t�@�T�C�Y
//=====================================
#define RE_MCS_BUFF_RAIL_SIZE	( 8198 )
#define RE_MCS_BUFF_AREA_SIZE	( 1024 )


//-------------------------------------
///	CIRCLE�ړ�
//=====================================
#define RM_DEF_ANGLE_MOVE	(16)
#define RM_DEF_LEN_MOVE		(FX32_ONE)
#define RM_DEF_CAMERA_LEN	( 0x38D000 )
#define RM_DEF_CAMERA_PITCH	( 0x800 )

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
			u32	select_recv:1;		// �I������M
			u32	rail_req:1;				// ���[����񑗐M���N�G�X�g
			u32	area_req:1;				// �G���A��񑗐M���N�G�X�g
			u32	player_req:1;			// �v���C���[��񑗐M���N�G�X�g
			u32	camera_req:1;			// �J������񑗐M���N�G�X�g
			u32	reset_req:1;			// ���Z�b�g���N�G�X�g
			u32 end_req:1;				// �I�����N�G�X�g
			u32	pad_data:23;
		};
		u32 flag;	
	};

	// �f�[�^��
	u32	rail[RE_MCS_BUFF_RAIL_SIZE/4];
	u32 rail_size;
	u32 area[RE_MCS_BUFF_AREA_SIZE/4];
	u32 area_size;
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
} DEBUG_RAIL_EDITOR;


FS_EXTERN_OVERLAY(mcs_lib);

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT DEBUG_RailEditorEvent(
    GMEVENT * p_event, int *  p_seq, void * p_work );


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

// ��M���̔��f
static void RE_Reflect( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Area( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Select( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpArea( DEBUG_RAIL_EDITOR* p_wk );

// �I�����A�R���g���[���֐�
static void RE_InputControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLine_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLine_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLine_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputArea_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputArea_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputArea_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Angle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_Rail( DEBUG_RAIL_EDITOR* p_wk );

// ���M�֐�
static void RE_SendControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_RailData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_AreaData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_PlayerData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_CameraData( DEBUG_RAIL_EDITOR* p_wk );


// ��l���A�m�[�}���ړ�
static void RE_FreeMove_Normal( DEBUG_RAIL_EDITOR* p_wk );
// ��l���A��]�ړ�
static void RE_FreeMove_Circle( DEBUG_RAIL_EDITOR* p_wk );
// ��l���@���[������
static void RE_RailMove( DEBUG_RAIL_EDITOR* p_wk );


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

		GFL_OVERLAY_Load(FS_OVERLAY_ID(mcs_lib));

		// �o�b�t�@�̃������m��
		p_wk->p_recv = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), sizeof(RE_RECV_BUFF) );
		p_wk->p_tmp_buff = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), RE_TMP_BUFF_SIZE );

		// �J�����^�[�Q�b�g�ݒ�
//		FIELD_CAMERA_BindTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->camera_target );

		// ���W��ݒ�
		FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &p_wk->camera_target );
		
		(*p_seq) = RAIL_EDITOR_SEQ_CONNECT;
		break;

	// �ڑ�
	case RAIL_EDITOR_SEQ_CONNECT:	
		if( !MCS_Init( FIELDMAP_GetHeapID(p_wk->p_fieldmap) ) )
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

		// MCS���C��
		MCS_Main();

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
		MCS_Exit();

		GFL_OVERLAY_Unload( FS_OVERLAY_ID( mcs_lib ) );

		// �o�b�t�@�̃������j��
		GFL_HEAP_FreeMemory( p_wk->p_recv );
		GFL_HEAP_FreeMemory( p_wk->p_tmp_buff );
		return GMEVENT_RES_FINISH;

	default:
		GF_ASSERT( 0 );
		break;
	}

  return GMEVENT_RES_CONTINUE;
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
	u32 size;
	static void (* const cpRecv[RE_MCS_DATA_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size ) = 
	{
		NULL,
		NULL,
		RE_Recv_Rail,
		RE_Recv_Area,
		NULL,
		NULL,
		RE_Recv_SelectData,
		RE_Recv_RailReq,
		RE_Recv_AreaReq,
		RE_Recv_PlayerDataReq,
		RE_Recv_CameraDataReq,
		RE_Recv_ResetReq,
		RE_Recv_EndReq,
	};

	// ��M�f�[�^�����邩�`�F�b�N
	if( MCS_CheckRead() != 0 )
	{
		size = MCS_Read( p_wk->p_tmp_buff, RE_TMP_BUFF_SIZE );
		if( size != 0 )
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
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FIELD_RAIL_LOADER* p_railload = FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	RE_MCS_RAIL_DATA* p_data = (RE_MCS_RAIL_DATA*)p_wk->p_recv->rail;	

	// ���P�[�V�����擾
	FIELD_RAIL_MAN_GetLocation( p_rail, &location );

	// ���[����񃍁[�_�[�ɐݒ�
	FIELD_RAIL_LOADER_Clear( p_railload );
	FIELD_RAIL_LOADER_DEBUG_LoadBinary( p_railload, p_data->rail, p_wk->p_recv->rail_size  - 8 );

	// ���[���}�l�[�W���ɓo�^
	FIELD_RAIL_MAN_Load( p_rail,  FIELD_RAIL_LOADER_GetData( p_railload ) );

	// ���P�[�V�����ݒ�
	FIELD_RAIL_MAN_SetLocation( p_rail, &location );
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
	FLD_SCENEAREA* p_area = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	FLD_SCENEAREA_LOADER* p_areaload = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );
	RE_MCS_AREA_DATA* p_data = (RE_MCS_AREA_DATA*)p_wk->p_recv->area;	

	// area���[�_�[�ɐݒ�
	FLD_SCENEAREA_LOADER_Clear( p_areaload );
	FLD_SCENEAREA_LOADER_LoadBinary( p_areaload, p_data->area, p_wk->p_recv->area_size - 8 );

	// area�}�l�[�W���ɓo�^
	FLD_SCENEAREA_Release( p_area );
	FLD_SCENEAREA_Load( p_area, 
			FLD_SCENEAREA_LOADER_GetData( p_areaload ),
			FLD_SCENEAREA_LOADER_GetDataNum( p_areaload ),
			FLD_SCENEAREA_LOADER_GetFunc( p_areaload ) );
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
	static void (*const cpJump[RE_SELECT_DATA_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
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

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g�̏ꏊ�ɃW�����v
 */
//-----------------------------------------------------------------------------
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	VecFx32 pos;

	location.type					= FIELD_RAIL_TYPE_POINT;
	location.rail_index		= p_wk->p_recv->select.select_index;
	location.line_ofs			= 0;
	location.width_ofs		= 0;
	location.key					= 0;
	FIELD_RAIL_MAN_SetLocation( p_rail, &location );

	// �ʒu��l���ɐݒ�
	FIELD_RAIL_MAN_GetPos( p_rail, &pos );
	// 
	FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���̊J�n�ʒu�ꏊ�ɃW�����v
 */
//-----------------------------------------------------------------------------
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	VecFx32 pos;

	location.type					= FIELD_RAIL_TYPE_LINE;
	location.rail_index		= p_wk->p_recv->select.select_index;
	location.line_ofs			= 0;
	location.width_ofs		= 0;
	location.key					= 0;
	FIELD_RAIL_MAN_SetLocation( p_rail, &location );

	// �ʒu��l���ɐݒ�
	FIELD_RAIL_MAN_GetPos( p_rail, &pos );
	// 
	FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
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
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA_LOADER* p_arealoader = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );

	TOMOYA_Printf( "RE_JumpArea ������\n" );
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
	static void (*const cpInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_InputPoint_FreeNormal,
		RE_InputPoint_FreeCircle,
		RE_InputPoint_Rail,
		RE_InputLine_FreeNormal,
		RE_InputLine_FreeCircle,
		RE_InputLine_Rail,
		RE_InputArea_FreeNormal,
		RE_InputArea_FreeCircle,
		RE_InputArea_Rail,
		RE_InputCamera_FreeNormal,
		RE_InputCamera_FreeCircle,
		RE_InputCamera_Pos,
		RE_InputCamera_Angle,
		RE_InputCamera_Target,
		RE_InputCamera_Rail,
		RE_InputLinePos_FreeNormal,
		RE_InputLinePos_FreeCircle,
		RE_InputLinePos_CenterPos,
		RE_InputLinePos_Rail,
	};
	
	// �I����Ԃɂ��A����
	GF_ASSERT( p_wk->p_recv->select.select_seq < RE_SELECT_DATA_SEQ_TYPE_MAX);	
	if( cpInput[p_wk->p_recv->select.select_seq] )
	{
		cpInput[p_wk->p_recv->select.select_seq]( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g	���R�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	// �t���[���@����
	RE_FreeMove_Normal( p_wk );
}
static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	// �t���[���@����
	RE_FreeMove_Circle( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�C���g	���[���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_RailMove( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���m�[�}���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputLine_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Normal( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C���T�[�N���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputLine_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Circle( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�����[���ړ�
 */
//-----------------------------------------------------------------------------
static void RE_InputLine_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_RailMove( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�G���A���́@�t���[����
 */
//-----------------------------------------------------------------------------
static void RE_InputArea_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Normal( p_wk );
}
static void RE_InputArea_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Circle( p_wk );
}
static void RE_InputArea_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_RailMove( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J��������
 */
//-----------------------------------------------------------------------------
static void RE_InputCamera_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Normal( p_wk );
}
static void RE_InputCamera_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Circle( p_wk );
}
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
}
static void RE_InputCamera_Angle( DEBUG_RAIL_EDITOR* p_wk )
{
}
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
}
static void RE_InputCamera_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_RailMove( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���ʒu����
 */
//-----------------------------------------------------------------------------
static void RE_InputLinePos_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Normal( p_wk );
}

static void RE_InputLinePos_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_FreeMove_Circle( p_wk );
}

static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
}

static void RE_InputLinePos_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	RE_RailMove( p_wk );
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
	const FIELD_RAIL_LOADER* cp_loader = FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap );
	const void* cp_data = FIELD_RAIL_LOADER_DEBUG_GetData( cp_loader );	
	u32 datasize = FIELD_RAIL_LOADER_DEBUG_GetDataSize( cp_loader );	
	RE_MCS_RAIL_DATA* p_senddata = (RE_MCS_RAIL_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_RAIL;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->rail, datasize );
	
	// ���M
	result = MCS_Write( MCS_CHANNEL0, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
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
	const FLD_SCENEAREA_LOADER* cp_loader = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );
	const void* cp_data = FLD_SCENEAREA_LOADER_DEBUG_GetData( cp_loader );
	u32 datasize				= FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( cp_loader );
	RE_MCS_AREA_DATA* p_senddata = (RE_MCS_AREA_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_AREA;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->area, datasize );
	
	
	// ���M
	result = MCS_Write( MCS_CHANNEL0, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
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
	RE_MCS_PLAYER_DATA* p_senddata = (RE_MCS_PLAYER_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos;
	BOOL result;
	
	p_senddata->header.data_type = RE_MCS_DATA_PLAYERDATA;

	FIELD_PLAYER_GetPos( cp_player, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;

	// ���M
	result = MCS_Write( MCS_CHANNEL0, p_senddata, sizeof(RE_MCS_PLAYER_DATA) );
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
	const FIELD_CAMERA* cp_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	RE_MCS_CAMERA_DATA* p_senddata = (RE_MCS_CAMERA_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos;
	VecFx32 target;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_CAMERADATA;

	FIELD_CAMERA_GetTargetPos( cp_camera, &target );
	FIELD_CAMERA_GetCameraPos( cp_camera, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;
	p_senddata->target_x = target.x;
	p_senddata->target_y = target.y;
	p_senddata->target_z = target.z;
	p_senddata->pitch = FIELD_CAMERA_GetAnglePitch( cp_camera );
	p_senddata->yaw		= FIELD_CAMERA_GetAngleYaw( cp_camera );
	p_senddata->len		= FIELD_CAMERA_GetAngleLen( cp_camera );

	// ���M
	result = MCS_Write( MCS_CHANNEL0, p_senddata, sizeof(RE_MCS_CAMERA_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	��l���@�ʏ�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_FreeMove_Normal( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	FIELD_RAIL_MAN_SetActiveFlag(p_rail, FALSE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, FALSE);
	
	FIELD_PLAYER_NOGRID_Move( p_player, GFL_UI_KEY_GetCont() );

	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
	FIELD_CAMERA_BindTarget( p_camera, &p_wk->camera_target );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��l���@�~�ړ�
 */
//-----------------------------------------------------------------------------
static void RE_FreeMove_Circle( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	VecFx32 target, pl_pos;
	u16 yaw;
	fx32 len;
	u32 key_cont = GFL_UI_KEY_GetCont();
	s16 df_angle;
	s16 df_len;

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	FIELD_RAIL_MAN_SetActiveFlag(p_rail, FALSE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, FALSE);

	FIELD_CAMERA_FreeTarget( p_camera );

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

	// �J�����^�[�Q�b�g�𒆐S�ɉ�]�ړ�
	FIELD_CAMERA_GetTargetPos( p_camera, &target );
	FIELD_PLAYER_GetPos( p_player, &pl_pos );
	target.y	= pl_pos.y;
	len				= VEC_Distance( &target, &pl_pos );
	yaw				= FX_Atan2Idx( target.x, target.z );

	if( key_cont & PAD_KEY_LEFT )
	{
		yaw += df_angle;
	}
	else if( key_cont & PAD_KEY_RIGHT )
	{
		yaw -= df_angle;
	}
	else if( key_cont & PAD_KEY_UP )
	{
		len -= df_len;
	}
	else if( key_cont & PAD_KEY_DOWN )
	{
		len += df_len;
	}

	// �p�x����A���W�����߂�
	pl_pos.x = FX_Mul( FX_SinIdx( yaw ), len );
	pl_pos.z = FX_Mul( FX_CosIdx( yaw ), len );
	FIELD_PLAYER_SetPos( p_player, &pl_pos );
	
	// ��������ɃJ������z�u
	FIELD_CAMERA_SetAnglePitch( p_camera, RM_DEF_CAMERA_PITCH );
	FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
	FIELD_CAMERA_SetAngleLen( p_camera, RM_DEF_CAMERA_LEN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[������
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_RailMove( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	VecFx32 pos;

	// ���[���ړ�
	FIELD_RAIL_MAN_SetActiveFlag(p_rail, FALSE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, FALSE);
	
  FIELD_RAIL_MAN_GetPos(p_rail, &pos );
  FIELD_PLAYER_SetPos( p_player, &pos );

	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
//	FIELD_CAMERA_BindTarget( p_camera, &p_wk->camera_target );
}


#endif // PM_DEBUG
