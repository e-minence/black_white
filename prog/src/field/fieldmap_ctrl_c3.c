/**
 * @file	fieldmap_ctrl_c3.c
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@C3�}�b�v����
 * @author	tamada
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_player_nogrid.h"

#include "field_easytp.h"

#include "fieldmap_ctrl_c3.h"

#include "field_rail.h"
#include "fld_scenearea.h"


//======================================================================
//	�V�[���G���A���
//======================================================================
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Updata( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Inside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Outside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );

//-------------------------------------
///	�G���A�p�����[�^�\����
//=====================================
typedef struct {
  u32   rot_start;
  u32   rot_end;
  fx32  dist_min;
  fx32  dist_max;
  u32   pitch;
  fx32  length;
} C3_SCENEAREA_PARAM;

#define SCENE_AREA_DIST_MIN ( 0xfc024 )
//#define SCENE_AREA_DIST_MAX ( 0x18e038 )
#define SCENE_AREA_DIST_MAX ( 0x2000000 )

static const FLD_SCENEAREA_DATA sc_SCENE[] = 
{
  // ALLAY00
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x27e8, 0x3048, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Updata,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY01
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x4f28, 0x6868, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Updata,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY02
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x7a28, 0x83d8, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Updata,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY03
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0xa638, 0xaff8, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Updata,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY04
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0xcfb8, 0xd9d8, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Updata,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },
};


//======================================================================
//	rail���
//======================================================================

// �o���ƏI�[
static const RAIL_POINT sc_POINT_C3_START;
static const RAIL_POINT sc_POINT_C3_END;

// ����n�_
static const RAIL_POINT sc_POINT_C3_JOINT00;
static const RAIL_POINT sc_POINT_C3_JOINT01;
static const RAIL_POINT sc_POINT_C3_JOINT02;
static const RAIL_POINT sc_POINT_C3_JOINT03;
static const RAIL_POINT sc_POINT_C3_JOINT04;

// �x�C�G���A
static const RAIL_POINT sc_POINT_C3_BEACH00;
static const RAIL_POINT sc_POINT_C3_BEACH01;
static const RAIL_POINT sc_POINT_C3_BEACH02;
static const RAIL_POINT sc_POINT_C3_BEACH03;
static const RAIL_POINT sc_POINT_C3_BEACH04;

// �H�n�J�����Y�[���p
static const RAIL_POINT sc_POINT_C3_CA_ALLAY00;
static const RAIL_POINT sc_POINT_C3_CA_ALLAY01;
static const RAIL_POINT sc_POINT_C3_CA_ALLAY02;
static const RAIL_POINT sc_POINT_C3_CA_ALLAY03;
static const RAIL_POINT sc_POINT_C3_CA_ALLAY04;

// ���C��
static const RAIL_LINE sc_LINE_C3_STARTtoJOINT00;
static const RAIL_LINE sc_LINE_C3_JOINT00toEND;
static const RAIL_LINE sc_LINE_C3_JOINT00toCA_ALLAY00;
static const RAIL_LINE sc_LINE_C3_JOINT00toBEACH00;
static const RAIL_LINE sc_LINE_C3_JOINT00toJOINT01;
static const RAIL_LINE sc_LINE_C3_JOINT01toCA_ALLAY01;
static const RAIL_LINE sc_LINE_C3_JOINT01toBEACH01;
static const RAIL_LINE sc_LINE_C3_JOINT01toJOINT02;
static const RAIL_LINE sc_LINE_C3_JOINT02toCA_ALLAY02;
static const RAIL_LINE sc_LINE_C3_JOINT02toBEACH02;
static const RAIL_LINE sc_LINE_C3_JOINT02toJOINT03;
static const RAIL_LINE sc_LINE_C3_JOINT03toCA_ALLAY03;
static const RAIL_LINE sc_LINE_C3_JOINT03toBEACH03;
static const RAIL_LINE sc_LINE_C3_JOINT03toJOINT04;
static const RAIL_LINE sc_LINE_C3_JOINT04toCA_ALLAY04;
static const RAIL_LINE sc_LINE_C3_JOINT04toBEACH04;


// �J����
static const RAIL_CAMERA_SET sc_CAMERA_C3_NORMAL;
static const RAIL_CAMERA_SET sc_CAMERA_C3_FIRST;
static const RAIL_CAMERA_SET sc_CAMERA_C3_END;
static const RAIL_CAMERA_SET sc_CAMERA_C3_JOINT04; 

// �ʒu
static const RAIL_LINEPOS_SET sc_LINEPOS_C3_NORMAL;


// ��{������
#define RAIL_LINE_DIV_NORMAL    (196)
#define RAIL_LINE_DIV_FIRST     (16)
#define RAIL_LINE_DIV_WALKIN    (8)
#define RAIL_LINE_DIV_PICKUP    (8)



//======================================================================
//	define
//======================================================================
#define HEIGHT	(0x2000)

#define CAMERA_DEF_PITCH  ( 0x800 )
#define CAMERA_DEF_LEN    ( 0x38D000 )

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	C3_MOVE_WORK
//--------------------------------------------------------------
typedef struct {
	u16 player_len;
	u16 pos_angle;
	s16 df_len;
	s16 df_angle;

  //�|�C���^
  FIELD_RAIL_MAN * p_railMan;

  FLD_SCENEAREA* p_sceneArea;
}C3_MOVE_WORK;




//======================================================================
//	proto
//======================================================================
static void mapCtrlC3_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlC3_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlC3_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );

static void CalcPos( VecFx32 *pos, const VecFx32 *center, u16 len, u16 dir );

static void mapCtrlC3_CameraMain( FIELDMAP_WORK *fieldWork, const VecFx32* cp_pos );


//======================================================================
//	�t�B�[���h�}�b�v�@�R���g���[���@C3
//======================================================================
//--------------------------------------------------------------
///	�}�b�v�R���g���[���@�O���b�h�ړ�
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_C3Functions =
{
	FLDMAP_CTRLTYPE_NOGRID,
	mapCtrlC3_Create,
	mapCtrlC3_Main,
	mapCtrlC3_Delete,
};

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@C3�����@������
 * @param	fieldWork	FIELDMAP_WORK
 * @param	pos	���@�����ʒu
 * @param	dir ���@��������
 */
//--------------------------------------------------------------
static void mapCtrlC3_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir )
{
	HEAPID heapID;
	C3_MOVE_WORK *work;
	FIELD_PLAYER *fld_player;
  FIELD_CAMERA * camera = FIELDMAP_GetFieldCamera(fieldWork);
	static const C3_MOVE_WORK init = {
		 0x1f0,
		 0,
		 1,
		 16
	};
	
	heapID = FIELDMAP_GetHeapID( fieldWork );
	work = GFL_HEAP_AllocClearMemory( heapID, sizeof(C3_MOVE_WORK) );
	*work = init;
	FIELDMAP_SetMapCtrlWork( fieldWork, work );

  // ���[���N��
  work->p_railMan = FIELD_RAIL_MAN_Create( FIELDMAP_GetHeapID(fieldWork), camera );
  FIELD_RAIL_MAN_Load(work->p_railMan, &sc_POINT_C3_START);
  FIELD_RAIL_MAN_GetPos(work->p_railMan, pos );

  // �V�[���G���A
  work->p_sceneArea = FLD_SCENEAREA_Create( FIELDMAP_GetHeapID(fieldWork), camera );
  FLD_SCENEAREA_Load( work->p_sceneArea, sc_SCENE, NELEMS(sc_SCENE) );
	
	{	//�r���{�[�h�ݒ�
		VecFx32 scale = {
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
		};
		GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fieldWork );
		GFL_BBD_SetScale( GFL_BBDACT_GetBBDSystem(bbdActSys), &scale );
	}
	fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	FIELD_PLAYER_SetPos( fld_player, pos);
	FIELD_PLAYER_SetDir( fld_player, dir );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@C3�����@�폜
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void mapCtrlC3_Delete( FIELDMAP_WORK *fieldWork )
{
	C3_MOVE_WORK *work = FIELDMAP_GetMapCtrlWork( fieldWork );
  FIELD_RAIL_MAN_Delete(work->p_railMan);
  FLD_SCENEAREA_Delete( work->p_sceneArea );
	GFL_HEAP_FreeMemory( work );

}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@C3�����@���C��
 * @param	fieldWork	FIELDMAP_WORK
 * @param	pos
 * @retval nothing
 */
//--------------------------------------------------------------
static void mapCtrlC3_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos )
{
	int key_cont = FIELDMAP_GetKeyCont( fieldWork );
  int key_trg = FIELDMAP_GetKeyTrg(fieldWork);
	C3_MOVE_WORK *mwk = FIELDMAP_GetMapCtrlWork( fieldWork );
  BOOL rail_flag = FIELD_RAIL_MAN_GetActiveFlag(mwk->p_railMan);
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );;

  //�f�o�b�O�̂��߁A���[��������ON/OFF
  if (key_trg & PAD_BUTTON_L)
  {
    FIELD_RAIL_MAN_SetActiveFlag(mwk->p_railMan, !rail_flag);
  }

  if (rail_flag)
  {
    // ���[������
    FIELD_RAIL_MAN_Update(mwk->p_railMan, FIELDMAP_GetKeyCont(fieldWork) );
    FIELD_RAIL_MAN_GetPos(mwk->p_railMan, pos );
    pos->y = HEIGHT;
    FIELD_PLAYER_SetPos( fld_player, pos );
    FIELD_RAIL_MAN_UpdateCamera(mwk->p_railMan);

    // �V�[���G���A�����ŃJ�����㏑��
    FLD_SCENEAREA_Updata( mwk->p_sceneArea, pos );

		FIELD_PLAYER_C3_Move( fld_player, key_cont, 0 );
  }	
  else
  {
		s16 df_angle;
		s16 df_len;

		if( key_cont & PAD_BUTTON_B ){
			df_angle	= mwk->df_angle*4;
			df_len		= mwk->df_len*4;
		}else{
			df_angle	= mwk->df_angle;
			df_len		= mwk->df_len;
		}
		
		if (key_cont & PAD_KEY_LEFT) {
			mwk->pos_angle -= df_angle;
		}
		if (key_cont & PAD_KEY_RIGHT) {
			mwk->pos_angle += df_angle;
		}
		if (key_cont & PAD_KEY_UP) {
			mwk->player_len -= df_len;
		}
		if (key_cont & PAD_KEY_DOWN) {
			mwk->player_len += df_len;
		}


    {
      VecFx32 cam, player_pos;
      FIELD_CAMERA *camera_control;
      camera_control = FIELDMAP_GetFieldCamera( fieldWork );
      FIELD_CAMERA_GetTargetPos( camera_control, &cam);
      CalcPos( &player_pos, &cam, mwk->player_len, mwk->pos_angle );
      //SetPlayerActTrans( fieldWork->field_player, &player_pos );
      FIELD_PLAYER_SetPos( fld_player, &player_pos);
      *pos = player_pos;
      // �v���C���[�ʒu����A�J�������W������
      mapCtrlC3_CameraMain( fieldWork, pos );
      FIELD_CAMERA_SetAngleYaw( camera_control, mwk->pos_angle);
      FIELD_PLAYER_C3_Move( fld_player, key_cont, mwk->pos_angle );

      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
        VecFx32 normal_vec = {0,0,-FX32_ONE};
        VecFx32 npos;
        u16 rotate;
        fx32 dist;
        VecFx32 normal;
        
        OS_TPrintf( "player pos x=0x%x y=0x%x z=0x%x\n", player_pos.x, player_pos.y, player_pos.z );
        OS_TPrintf( "0x%x, 0x%x, 0x%x\n", player_pos.x, player_pos.y, player_pos.z );

        VEC_Subtract( pos, &cam, &npos );
        npos.y = 0;
        dist = VEC_Mag( &npos );
        VEC_Normalize( &npos, &npos );
        rotate  = FX_AcosIdx( VEC_DotProduct( &npos, &normal_vec ) );
        VEC_CrossProduct( &npos, &normal_vec, &normal );

        if( normal.y < 0 ){
          rotate = 0x10000 - rotate;
        }
        
        OS_TPrintf( "rotate\n0x%x dist=0x%x normal=0x%x\n", rotate, dist, normal.y );
      }
    }
	}

}

//======================================================================
//	C3�@�T�u
//======================================================================
//--------------------------------------------------------------
/**
 * ���W�v�Z
 * @param pos ���W
 * @param center ���S�ʒu
 * @param len ����
 * @param dir ����
 * @retval nothing
 */
//--------------------------------------------------------------
static void CalcPos(VecFx32 * pos, const VecFx32 * center, u16 len, u16 dir)
{
	pos->x = center->x + len * FX_SinIdx(dir);
	pos->y = center->y + HEIGHT;
	pos->z = center->z + len * FX_CosIdx(dir);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����̌v�Z
 *
 *	@param	fieldWork
 *	@param	cp_pos 
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3_CameraMain( FIELDMAP_WORK *fieldWork, const VecFx32* cp_pos )
{
  VecFx32 pos, target, n0, camera_pos;
  FIELD_CAMERA* p_camera;
  fx32 xz_dist;
  fx32 target_y;

  p_camera = FIELDMAP_GetFieldCamera( fieldWork );
	FIELD_CAMERA_GetTargetPos( p_camera, &target);
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  // �����x�N�g������A�J����angle�����߂�
  camera_pos.y = FX_Mul( FX_SinIdx( CAMERA_DEF_PITCH ), CAMERA_DEF_LEN );
  xz_dist      = FX_Mul( FX_CosIdx( CAMERA_DEF_PITCH ), CAMERA_DEF_LEN );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target_y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}




//======================================================================
//	SCENEAREA���
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A���  �G���A����
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      ���@�ʒu
 *
 *	@retval TRUE    �G���A��
 *	@retval FALSE   �G���A�O
 */
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 normal_vec = {0,0,-FX32_ONE};
  VecFx32 npos;
  fx32 dist;
  u32 rotate;
  VecFx32 target, normal;
  FIELD_CAMERA* p_camera;
  const C3_SCENEAREA_PARAM* cp_param = (C3_SCENEAREA_PARAM*)cp_data->area;

  //�J�����擾
  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );
  FIELD_CAMERA_GetTargetPos( p_camera, &target );
  target.y = 0;
  

  VEC_Subtract( cp_pos, &target, &npos );
  npos.y  = 0;
  dist    = VEC_Mag( &npos );
  VEC_Normalize( &npos, &npos );
  rotate  = FX_AcosIdx( VEC_DotProduct( &npos, &normal_vec ) );
  VEC_CrossProduct( &npos, &normal_vec, &normal );
  if( normal.y < 0 ){
    rotate = 0x10000 - rotate;
  }

  // �G���A������
  if( (cp_param->rot_start <= rotate) && (cp_param->rot_end > rotate) ){
    if( (cp_param->dist_min <= dist) && (cp_param->dist_max > dist) ){
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�V����
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_Updata( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 pos, target, n0, camera_pos;
  FIELD_CAMERA* p_camera;
  fx32 xz_dist;
  fx32 target_y;
  const C3_SCENEAREA_PARAM* cp_param = (const C3_SCENEAREA_PARAM*)cp_data->area;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );
	FIELD_CAMERA_GetTargetPos( p_camera, &target);
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  // �����x�N�g������A�J����angle�����߂�
  camera_pos.y = FX_Mul( FX_SinIdx( cp_param->pitch ), cp_param->length );
  xz_dist      = FX_Mul( FX_CosIdx( cp_param->pitch ), cp_param->length );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target_y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �͈͂ɓ������u�Ԃ̏���
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_Inside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �͈͂���o���u�Ԃ̏���
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_Outside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
}




//======================================================================
//	RAIL���
//======================================================================
// �o���ƏI�[
static const RAIL_POINT sc_POINT_C3_START = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_STARTtoJOINT00, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_LEFT, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x461fa2, 0x6000, 0x20189e
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_FIRST,
  //const char * name;
  "sc_POINT_C3_START",
};

static const RAIL_POINT sc_POINT_C3_END = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT00toEND, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_LEFT, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x461fa2, 0x6000, 0x20189e
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_END,
  //const char * name;
  "sc_POINT_C3_END",
};

// ����n�_
static const RAIL_POINT sc_POINT_C3_JOINT00 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT00toEND, &sc_LINE_C3_JOINT00toJOINT01, &sc_LINE_C3_JOINT00toCA_ALLAY00, &sc_LINE_C3_JOINT00toBEACH00,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_RIGHT, RAIL_KEY_LEFT, RAIL_KEY_UP, RAIL_KEY_DOWN,
  },
  //VecFx32 pos;
  {
    0x476e5e, 0x6000, 0x222466
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_JOINT00",
};
static const RAIL_POINT sc_POINT_C3_JOINT01 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT00toJOINT01, &sc_LINE_C3_JOINT01toJOINT02, &sc_LINE_C3_JOINT01toCA_ALLAY01, &sc_LINE_C3_JOINT01toBEACH01,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_RIGHT, RAIL_KEY_LEFT, RAIL_KEY_UP, RAIL_KEY_DOWN,
  },
  //VecFx32 pos;
  {
    0x47e292, 0x6000, 0x3d31c2
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_JOINT01",
};
static const RAIL_POINT sc_POINT_C3_JOINT02 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT01toJOINT02, &sc_LINE_C3_JOINT02toJOINT03, &sc_LINE_C3_JOINT02toCA_ALLAY02, &sc_LINE_C3_JOINT02toBEACH02,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_RIGHT, RAIL_KEY_LEFT, RAIL_KEY_UP, RAIL_KEY_DOWN,
  },
  //VecFx32 pos;
  {
    0x303d9a, 0x6000, 0x4bd08a
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_JOINT02",
};
static const RAIL_POINT sc_POINT_C3_JOINT03 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT02toJOINT03, &sc_LINE_C3_JOINT03toJOINT04, &sc_LINE_C3_JOINT03toCA_ALLAY03, &sc_LINE_C3_JOINT03toBEACH03,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_RIGHT, RAIL_KEY_LEFT, RAIL_KEY_UP, RAIL_KEY_DOWN,
  },
  //VecFx32 pos;
  {
    0x18cc86, 0x6000, 0x3d5d06
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_JOINT03",
};
static const RAIL_POINT sc_POINT_C3_JOINT04 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT03toJOINT04, &sc_LINE_C3_JOINT04toCA_ALLAY04, &sc_LINE_C3_JOINT04toBEACH04, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_RIGHT, RAIL_KEY_UP, RAIL_KEY_DOWN, RAIL_KEY_NULL, 
  },
  //VecFx32 pos;
  {
    0x196733, 0x6000, 0x22dee4
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
//  &sc_CAMERA_C3_JOINT04,
  //const char * name;
  "sc_POINT_C3_JOINT04",
};

// �x�C�G���A
static const RAIL_POINT sc_POINT_C3_BEACH00 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT00toBEACH00, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_UP, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x51ae62, 0x6000, 0x22beda
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_BEACH00",
};
static const RAIL_POINT sc_POINT_C3_BEACH01 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT01toBEACH01, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_UP, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x4db4de, 0x6000, 0x44e95a
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_BEACH01",
};
static const RAIL_POINT sc_POINT_C3_BEACH02 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT02toBEACH02, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_UP, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x300db2, 0x6000, 0x54d1b6
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_BEACH02",
};
static const RAIL_POINT sc_POINT_C3_BEACH03 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT03toBEACH03, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_UP, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x159152, 0x6000, 0x4a2fea
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_BEACH03",
};
static const RAIL_POINT sc_POINT_C3_BEACH04 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT04toBEACH04, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_UP, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0xc9f34, 0x6000, 0x206329
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_BEACH04",
};

// �H�n�J�����Y�[���p
static const RAIL_POINT sc_POINT_C3_CA_ALLAY00 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT00toCA_ALLAY00, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_DOWN, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x45b43b, 0x6000, 0x23473f
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_CA_ALLAY00",
};
static const RAIL_POINT sc_POINT_C3_CA_ALLAY01 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT01toCA_ALLAY01, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_DOWN, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x454772, 0x6000, 0x3c1a3b
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_CA_ALLAY01",
};
static const RAIL_POINT sc_POINT_C3_CA_ALLAY02 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT02toCA_ALLAY02, NULL, NULL, NULL, 
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_DOWN, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x3015fb, 0x6000, 0x490273
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_CA_ALLAY02",
};
static const RAIL_POINT sc_POINT_C3_CA_ALLAY03 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT03toCA_ALLAY03, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_DOWN, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL,
  },
  //VecFx32 pos;
  {
    0x3001b8, 0x6000, 0x490273
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_CA_ALLAY03",
};
static const RAIL_POINT sc_POINT_C3_CA_ALLAY04 = 
{
  //const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  {
    &sc_LINE_C3_JOINT04toCA_ALLAY04, NULL, NULL, NULL,
  },
  //RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];
  {
    RAIL_KEY_DOWN, RAIL_KEY_NULL, RAIL_KEY_NULL, RAIL_KEY_NULL, 
  },
  //VecFx32 pos;
  {
    0x19fc49, 0x6000, 0x235a8c
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_POINT_C3_CA_ALLAY04",
};

// ���C��
static const RAIL_LINE sc_LINE_C3_STARTtoJOINT00 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_START, &sc_POINT_C3_JOINT00,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_FIRST,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_FIRST,
  //const char * name;
  "sc_LINE_C3_STARTtoJOINT00",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toEND =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT00, &sc_POINT_C3_END,
  //RAIL_KEY key;
  RAIL_KEY_RIGHT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_FIRST,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_END,
  //const char * name;
  "sc_LINE_C3_JOINT00toEND",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toCA_ALLAY00 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT00, &sc_POINT_C3_CA_ALLAY00,
  //RAIL_KEY key;
  RAIL_KEY_UP,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_PICKUP,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT00toCA_ALLAY00",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toBEACH00 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT00, &sc_POINT_C3_BEACH00,
  //RAIL_KEY key;
  RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_WALKIN,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT00toBEACH00",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toJOINT01 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT00, &sc_POINT_C3_JOINT01,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT00toJOINT01",
};
static const RAIL_LINE sc_LINE_C3_JOINT01toCA_ALLAY01 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT01, &sc_POINT_C3_CA_ALLAY01,
  //RAIL_KEY key;
  RAIL_KEY_UP,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_PICKUP,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT01toCA_ALLAY01",
};
static const RAIL_LINE sc_LINE_C3_JOINT01toBEACH01 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT01, &sc_POINT_C3_BEACH01,
  //RAIL_KEY key;
  RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_WALKIN,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT01toBEACH01",
};
static const RAIL_LINE sc_LINE_C3_JOINT01toJOINT02 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT01, &sc_POINT_C3_JOINT02,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT01toJOINT02",
};
static const RAIL_LINE sc_LINE_C3_JOINT02toCA_ALLAY02 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT02, &sc_POINT_C3_CA_ALLAY02,
  //RAIL_KEY key;
  RAIL_KEY_UP,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_PICKUP,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT02toCA_ALLAY02",
};
static const RAIL_LINE sc_LINE_C3_JOINT02toBEACH02 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT02, &sc_POINT_C3_BEACH02,
  //RAIL_KEY key;
  RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_WALKIN,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT02toBEACH02",
};
static const RAIL_LINE sc_LINE_C3_JOINT02toJOINT03 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT02, &sc_POINT_C3_JOINT03,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT02toJOINT03",
};
static const RAIL_LINE sc_LINE_C3_JOINT03toCA_ALLAY03 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT03, &sc_POINT_C3_CA_ALLAY03,
  //RAIL_KEY key;
  RAIL_KEY_UP,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_PICKUP,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT03toCA_ALLAY03",
};
static const RAIL_LINE sc_LINE_C3_JOINT03toBEACH03 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT03, &sc_POINT_C3_BEACH03,
  //RAIL_KEY key;
  RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_WALKIN,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT03toBEACH03",
};
static const RAIL_LINE sc_LINE_C3_JOINT03toJOINT04 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT03, &sc_POINT_C3_JOINT04,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT03toJOINT04",
};
static const RAIL_LINE sc_LINE_C3_JOINT04toCA_ALLAY04 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT04, &sc_POINT_C3_CA_ALLAY04,
  //RAIL_KEY key;
  RAIL_KEY_UP,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_PICKUP,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT04toCA_ALLAY04",
};
static const RAIL_LINE sc_LINE_C3_JOINT04toBEACH04 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_JOINT04, &sc_POINT_C3_BEACH04,
  //RAIL_KEY key;
  RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
  //u32 line_divider;
  RAIL_LINE_DIV_WALKIN,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT04toBEACH04",
};


// �J����
static const RAIL_CAMERA_SET sc_CAMERA_C3_NORMAL = 
{
  FIELD_RAIL_POSFUNC_CircleCamera,
  0x800,
  0x38D000,
};
static const RAIL_CAMERA_SET sc_CAMERA_C3_FIRST = 
{
  FIELD_RAIL_CAMERAFUNC_FixAngleCamera,
  0xA00,
  0x55C0,
  0x3FD000,
};

static const RAIL_CAMERA_SET sc_CAMERA_C3_END = 
{
  FIELD_RAIL_CAMERAFUNC_FixAngleCamera,
  0x600,
  0x55C0,
  0x26D000,
};

static const RAIL_CAMERA_SET sc_CAMERA_C3_JOINT04 = 
{
  FIELD_RAIL_CAMERAFUNC_FixAngleCamera,
  0x800,
  0xAA40,
  0x38D000,
};

// �ʒu
static const RAIL_LINEPOS_SET sc_LINEPOS_C3_NORMAL = 
{
  FIELD_RAIL_POSFUNC_CircleLine,
	0x2f6f36,
  0x4000,
  0x301402
};


