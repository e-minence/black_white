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
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Inside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Outside( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_InsideFxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_OutsideFxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );

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

  // �Œ�J�����ɕK�v�ȃp�����[�^
  fx32  target_x;
  fx32  target_y;
  fx32  target_z;
  fx32  camera_x;
  fx32  camera_y;
  fx32  camera_z;
} C3_SCENEAREA_PARAM;

#define SCENE_AREA_DIST_MIN ( 0xfc024 )
#define SCENE_AREA_DIST_MAX ( 0x1a8fea )
//#define SCENE_AREA_DIST_MAX ( 0x2000000 )

static const FLD_SCENEAREA_DATA sc_SCENE[] = 
{
  // ALLAY00
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x2598, 0x3048, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Update,
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
    C3_SCENEAREA_Update,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY02
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x7a28, 0x9298, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Update,
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
    C3_SCENEAREA_Update,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // ALLAY04
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0xcfb8, 0xe148, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // pitch len
      0x600, 0x26D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Update,
    C3_SCENEAREA_Inside,
    C3_SCENEAREA_Outside,
  },

  // �s�J�`���[�̃r���̌Œ�J����
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
//      0xc268, 0xcfb8, SCENE_AREA_DIST_MIN, 0x1a5001,
      0xc268, 0xcfb8, SCENE_AREA_DIST_MIN, SCENE_AREA_DIST_MAX,
      // �J����
      // dummy, dummy,
      0, 0,
      // target x,y,z
      0x3e2504,0xfff1ee7f,0x10d83a,
      // camera x,y,z
      0xb573b,0x76e53,0x30e81c,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_InsideFxCamera,
    C3_SCENEAREA_InsideFxCamera,
    C3_SCENEAREA_OutsideFxCamera,
  },

  // �O���G���A
  {
    { 
      // �G���A
      // rot_start, rot_end, dist_min, dist_max
      // (���{0,0,-FX32_ONE}�Ƃ����Ƃ��̊p�x)
      0x1ca8, 0xe398, 0x1c8f6b, 0x300000,
      // �J����
      // pitch len
      0x800, 0x38D000,
    },
    C3_SCENEAREA_CheckArea,
    C3_SCENEAREA_Update,
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
static const RAIL_LINEPOS_SET sc_LINEPOS_C3_Strait;

// ���[�����
static const RAIL_POINT* scp_RAIL_POINT[] = {
  &sc_POINT_C3_START,
  &sc_POINT_C3_END,
                          
  &sc_POINT_C3_JOINT00,
  &sc_POINT_C3_JOINT01,
  &sc_POINT_C3_JOINT02,
  &sc_POINT_C3_JOINT03,
  &sc_POINT_C3_JOINT04,
                          
                          
  &sc_POINT_C3_BEACH00,
  &sc_POINT_C3_BEACH01,
  &sc_POINT_C3_BEACH02,
  &sc_POINT_C3_BEACH03,
  &sc_POINT_C3_BEACH04,
                          
                          
  &sc_POINT_C3_CA_ALLAY00,
  &sc_POINT_C3_CA_ALLAY01,
  &sc_POINT_C3_CA_ALLAY02,
  &sc_POINT_C3_CA_ALLAY03,
  &sc_POINT_C3_CA_ALLAY04,
};
static const RAIL_LINE* scp_RAIL_LINE[] = {
  &sc_LINE_C3_STARTtoJOINT00,
  &sc_LINE_C3_JOINT00toEND,
  &sc_LINE_C3_JOINT00toCA_ALLAY00,
  &sc_LINE_C3_JOINT00toBEACH00,
  &sc_LINE_C3_JOINT00toJOINT01,
  &sc_LINE_C3_JOINT01toCA_ALLAY01,
  &sc_LINE_C3_JOINT01toBEACH01,
  &sc_LINE_C3_JOINT01toJOINT02,
  &sc_LINE_C3_JOINT02toCA_ALLAY02,
  &sc_LINE_C3_JOINT02toBEACH02,
  &sc_LINE_C3_JOINT02toJOINT03,
  &sc_LINE_C3_JOINT03toCA_ALLAY03,
  &sc_LINE_C3_JOINT03toBEACH03,
  &sc_LINE_C3_JOINT03toJOINT04,
  &sc_LINE_C3_JOINT04toCA_ALLAY04,
  &sc_LINE_C3_JOINT04toBEACH04,
};
static const RAIL_SETTING sc_RAIL_SETTING = {
  NELEMS( scp_RAIL_POINT ),
  NELEMS( scp_RAIL_LINE ),
  scp_RAIL_POINT,
  scp_RAIL_LINE,
  16,
  2*FX32_ONE
};

// ���[���ʒu���
static const RAIL_LOCATION sc_RAIL_START_LOCATION = {
  FIELD_RAIL_TYPE_POINT,
  0,
};


//======================================================================
//	define
//======================================================================
#define HEIGHT	(0x3000)

#define CAMERA_DEF_PITCH  ( 0x800 )
#define CAMERA_DEF_LEN    ( 0x38D000 )
#define CAMERA_DEF_TARGET_X ( 0x2f6f36 )
#define CAMERA_DEF_TARGET_Y ( 0x4000 )
#define CAMERA_DEF_TARGET_Z ( 0x301402 )

// C3�J�������͈�
static const VecFx32 sc_RAIL_CAMERA_MOVE_AREA_START = { 0x464cf6, 0, 0x202210 };
static const VecFx32 sc_RAIL_CAMERA_MOVE_AREA_END = { 0x177042, 0, 0x21e54a };

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
  fx32  camera_len;
  u16   camera_pitch;

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

static void mapCtrlC3_CameraMain( FIELDMAP_WORK *fieldWork, const VecFx32* cp_pos, u16 pitch, fx32 len );
static void cameraMain( FIELD_CAMERA* p_camera, const VecFx32* cp_pos, u16 pitch, fx32 len );

static void cameraRailAreaControl( FIELD_CAMERA * p_camera );


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
  FIELD_RAIL_MAN* railMan = FIELDMAP_GetFieldRailMan(fieldWork);
	static const C3_MOVE_WORK init = {
		 0x1f0,
		 0,
		 1,
		 16,
     CAMERA_DEF_LEN,
     CAMERA_DEF_PITCH,
	};
	
	heapID = FIELDMAP_GetHeapID( fieldWork );
	work = GFL_HEAP_AllocClearMemory( heapID, sizeof(C3_MOVE_WORK) );
	*work = init;
	FIELDMAP_SetMapCtrlWork( fieldWork, work );

  // ���[���N��
  FIELD_RAIL_MAN_Load(railMan, &sc_RAIL_SETTING);
  FIELD_RAIL_MAN_SetLocation( railMan, &sc_RAIL_START_LOCATION );
  FIELD_RAIL_MAN_GetPos(railMan, pos );

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
	int key_cont = GFL_UI_KEY_GetCont(  );
  int key_trg = GFL_UI_KEY_GetTrg();
	C3_MOVE_WORK *mwk = FIELDMAP_GetMapCtrlWork( fieldWork );
  FIELD_RAIL_MAN* railMan = FIELDMAP_GetFieldRailMan(fieldWork);
  BOOL rail_flag = FIELD_RAIL_MAN_GetActiveFlag(railMan);
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );;

  //�f�o�b�O�̂��߁A���[��������ON/OFF
  if (key_trg & PAD_BUTTON_L)
  {
    FIELD_RAIL_MAN_SetActiveFlag(railMan, !rail_flag);
  }

  if (rail_flag)
  {
    // ���[������
    FIELD_RAIL_MAN_GetPos(railMan, pos );
    pos->y = HEIGHT;
    FIELD_PLAYER_SetPos( fld_player, pos );
    
    // �V�[���G���A�����ŃJ�����㏑��
    if( FLD_SCENEAREA_Update( mwk->p_sceneArea, pos ) == FLD_SCENEAREA_UPDATE_NONE ){
      // �J����������E�Ǘ�
      cameraRailAreaControl( FIELDMAP_GetFieldCamera( fieldWork ) );
    }

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
		
    if( (key_cont & PAD_BUTTON_R) ){
      if (key_cont & PAD_KEY_LEFT) {
        mwk->camera_len -= FX32_ONE*2;
      }
      if (key_cont & PAD_KEY_RIGHT) {
        mwk->camera_len += FX32_ONE*2;
      }

      if (key_cont & PAD_KEY_UP) {
        mwk->camera_pitch += 0x200;
      }
      if (key_cont & PAD_KEY_DOWN) {
        mwk->camera_pitch -= 0x200;
      }
    }else{
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
      // ��v���C���[����
      if (key_cont & PAD_BUTTON_SELECT) {
        mwk->player_len = 0x01B8;
      }
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
      mapCtrlC3_CameraMain( fieldWork, pos, mwk->camera_pitch, mwk->camera_len );
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
static void mapCtrlC3_CameraMain( FIELDMAP_WORK *fieldWork, const VecFx32* cp_pos, u16 pitch, fx32 len )
{
  FIELD_CAMERA* p_camera;
  
  p_camera = FIELDMAP_GetFieldCamera( fieldWork );

  cameraMain( p_camera, cp_pos, pitch, len );
}
static void cameraMain( FIELD_CAMERA* p_camera, const VecFx32* cp_pos, u16 pitch, fx32 len )
{
  VecFx32 pos, target, n0, camera_pos;
  fx32 xz_dist;
  fx32 target_y;

	FIELD_CAMERA_GetTargetPos( p_camera, &target);
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  // �����x�N�g������A�J����angle�����߂�
  camera_pos.y = FX_Mul( FX_SinIdx( pitch ), len );
  xz_dist      = FX_Mul( FX_CosIdx( pitch ), len );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target_y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����쎞�̃J�������͈͂̊Ǘ�
 *  
 *	@param	fieldWork   �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
static void cameraRailAreaControl( FIELD_CAMERA * p_camera )
{
  VecFx32 target;
  VecFx32 camera_pos, camera_way;
  VecFx32 area_start, area_sn;
  VecFx32 area_end, area_en;
  u16 rot_start, rot_end;

  FIELD_CAMERA_GetTargetPos( p_camera, &target );
  FIELD_CAMERA_GetCameraPos( p_camera, &camera_pos );


  // �^�[�Q�b�g�𒆐S�Ƃ��������ɕύX
  VEC_Subtract( &camera_pos, &target, &camera_pos ); 
  VEC_Subtract( &sc_RAIL_CAMERA_MOVE_AREA_START, &target, &area_start ); 
  VEC_Subtract( &sc_RAIL_CAMERA_MOVE_AREA_END, &target, &area_end );
  camera_way = camera_pos;  // ���̌�̍��W�����Ɏg�p���邽�ߕۑ�
  camera_pos.y  = 0;
  area_start.y  = 0;
  area_end.y    = 0;
  
  VEC_Normalize( &camera_pos, &camera_pos );
  VEC_Normalize( &area_start, &area_start );
  VEC_Normalize( &area_end, &area_end );

  // start end�Ƃ̊O�ς����߂�
  VEC_CrossProduct( &area_start, &camera_pos, &area_sn );
  VEC_CrossProduct( &area_end, &camera_pos, &area_en );
  rot_start   = FX_AcosIdx( VEC_DotProduct( &area_start, &camera_pos ) );
  rot_end     = FX_AcosIdx( VEC_DotProduct( &area_end, &camera_pos ) );

/*
  OS_TPrintf( "start = 0x%x end = 0x%x\n", area_sn.y, area_en.y );
  OS_TPrintf( "rot_start = %d rot_end = %d\n", rot_start/182, rot_end/182 );
//*/
  
  // �͈͊O�Ȃ̂ŃJ�������W��ύX
  if( (area_sn.y > 0) && (area_en.y < 0) ){
    fx32 dist;

    // �����͂��̂܂�
    camera_pos.y = camera_way.y;
    camera_way.y = 0;

    // ���ʂ̕����𒲐�����
    dist = VEC_Mag( &camera_way );
    if( rot_start < rot_end ){
      camera_pos.x = FX_Mul( area_start.x, dist );
      camera_pos.z = FX_Mul( area_start.z, dist );
    }else{
      camera_pos.x = FX_Mul( area_end.x, dist );
      camera_pos.z = FX_Mul( area_end.z, dist );
    }

    camera_pos.x += target.x;
    camera_pos.z += target.z;

	  FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
  }
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
  const C3_SCENEAREA_PARAM* cp_param = (C3_SCENEAREA_PARAM*)cp_data->area;

  VEC_Set( &target, CAMERA_DEF_TARGET_X, 0, CAMERA_DEF_TARGET_Z );
  

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
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
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

  // �J����������E�Ǘ�
  cameraRailAreaControl( p_camera );
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
  C3_SCENEAREA_Update( cp_sys, cp_data, cp_pos );
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

//----------------------------------------------------------------------------
/**
 *	@brief  �Œ�J�����́@�ݒ菈��
 *  
 *	@param	cp_sys    �V�X�e�����[�N
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_InsideFxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  const C3_SCENEAREA_PARAM* cp_param = (const C3_SCENEAREA_PARAM*)cp_data->area;
  VecFx32 target, camera_pos;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

  // �^�[�Q�b�g�ʒu�ݒ�
  VEC_Set( &target, cp_param->target_x, cp_param->target_y, cp_param->target_z );
  VEC_Set( &camera_pos, cp_param->camera_x, cp_param->camera_y, cp_param->camera_z );
  FIELD_CAMERA_SetTargetPos( p_camera, &target );
  FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Œ�J�����́@�������
 *  
 *	@param	cp_sys    �V�X�e�����[�N
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_OutsideFxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  VecFx32 target, camera_pos;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

  // �^�[�Q�b�g�ʒu�ݒ�
  VEC_Set( &target, CAMERA_DEF_TARGET_X, CAMERA_DEF_TARGET_Y, CAMERA_DEF_TARGET_Z );
  FIELD_CAMERA_SetTargetPos( p_camera, &target );

  // �J�����[�p�����[�^�ݒ�
  cameraMain( p_camera, cp_pos, 0x600, 0x26D000 );
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
    0x46992b, 0x7000, 0x203cfa
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
    0x447656, 0x6000, 0x1f24d2
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
    0x47aa2e, 0x6000, 0x231482
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
    0x475286, 0x6000, 0x3db512
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
    0x2fa47e, 0x6000, 0x4b9402
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
    0x17b1b6, 0x6000, 0x3df662
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
    0x17ef96, 0x6000, 0x21cc3a
  },
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
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
    0x4d1dd9, 0x6000, 0x2027d2
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
    0x4d3d74, 0x6000, 0x411568
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
    0x2fb0b9, 0x6000, 0x51e402
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
    0x13a5c6, 0x6000, 0x40543e
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
    0x1261b6, 0x6000, 0x1e6c22
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
    0x43b326, 0x6000, 0x253502
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
    0x427d90, 0x6000, 0x3af354
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
    0x2f9a52, 0x6000, 0x465402
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
    0x1ca876, 0x6000, 0x3b0f32
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
    0x1d1016, 0x6000, 0x24e9da
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
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_FIRST,
  //const char * name;
  "sc_LINE_C3_STARTtoJOINT00",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toEND =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_END, &sc_POINT_C3_JOINT00,
  //RAIL_KEY key;
  RAIL_KEY_LEFT,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_NORMAL,
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
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT00toCA_ALLAY00",
};
static const RAIL_LINE sc_LINE_C3_JOINT00toBEACH00 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_BEACH00,&sc_POINT_C3_JOINT00, 
  //RAIL_KEY key;
  RAIL_KEY_UP,//RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
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
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT01toCA_ALLAY01",
};
static const RAIL_LINE sc_LINE_C3_JOINT01toBEACH01 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_BEACH01,&sc_POINT_C3_JOINT01, 
  //RAIL_KEY key;
  RAIL_KEY_UP,//RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
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
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT02toCA_ALLAY02",
};
static const RAIL_LINE sc_LINE_C3_JOINT02toBEACH02 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_BEACH02,&sc_POINT_C3_JOINT02, 
  //RAIL_KEY key;
  RAIL_KEY_UP,//RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
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
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT03toCA_ALLAY03",
};
static const RAIL_LINE sc_LINE_C3_JOINT03toBEACH03 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
  &sc_POINT_C3_BEACH03,&sc_POINT_C3_JOINT03, 
  //RAIL_KEY key;
  RAIL_KEY_UP,//RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
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
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT04toCA_ALLAY04",
};
static const RAIL_LINE sc_LINE_C3_JOINT04toBEACH04 =
{
  //const RAIL_POINT * point_s
  //const RAIL_POINT * point_e
   &sc_POINT_C3_BEACH04,&sc_POINT_C3_JOINT04,
  //RAIL_KEY key;
  RAIL_KEY_UP,//RAIL_KEY_DOWN,
  //const RAIL_LINEPOS_SET * line_pos_set;
  &sc_LINEPOS_C3_Strait,//&sc_LINEPOS_C3_NORMAL,
  //const RAIL_CAMERA_SET * camera;
  &sc_CAMERA_C3_NORMAL,
  //const char * name;
  "sc_LINE_C3_JOINT04toBEACH04",
};


// �J����
static const RAIL_CAMERA_SET sc_CAMERA_C3_NORMAL = 
{
  FIELD_RAIL_POSFUNC_CircleCamera,
  0x700,
  0x2B1000,
  0x2f6f36,
  HEIGHT,
  0x301402,
};
static const RAIL_CAMERA_SET sc_CAMERA_C3_FIRST = 
{
  FIELD_RAIL_CAMERAFUNC_FixAllCamera,
  0x5e6375, 0x2a357, 0x1f4628,
  0x2fcf14, 0xa8c3c, 0x2d6876,
};

static const RAIL_CAMERA_SET sc_CAMERA_C3_END = 
{
  FIELD_RAIL_POSFUNC_FixLenCircleCamera,
  0x600,
  0x26D000,
  0x2f6f36,
  HEIGHT,
  0x301402,
};

// �ʒu
static const RAIL_LINEPOS_SET sc_LINEPOS_C3_NORMAL = 
{
  FIELD_RAIL_POSFUNC_CurveLine,
  0x2f6f36,
  HEIGHT,
  0x301402,
};
static const RAIL_LINEPOS_SET sc_LINEPOS_C3_Strait =
{
  FIELD_RAIL_POSFUNC_StraitLine,
  0,
  0,
  0,
};

