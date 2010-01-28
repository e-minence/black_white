//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_crowd_people.c
 *	@brief  �Q�W�Ǘ�����
 *	@author	tomoya takahshi
 *	@date		2010.01.26
 *
 *	���W���[�����FFIELD_CROWD_PEOPLE
 *
 *	C03�̘H�n�p�@�Q�W�����@�@��R�̓��샂�f�����g�p���āA�\��
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/script_message.naix"

#include "field/field_const.h"
#include "field/field_msgbg.h"

#include "fieldmap.h"
#include "field_crowd_people.h"


#include "fieldmap/field_crowd_people.naix"

#include "msg/script/msg_crowd_people_scr.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// ����V�[�P���X
enum
{
  // �ʏ�
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT=0,// ���슮���҂�
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK,    // ���f�{�w��
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_END,     // �I��

  // �Ӓn��
  FIELD_CROWD_PEOPLE_SEQ_NASTY_WALKWAIT=0,// ���슮���҂�
  FIELD_CROWD_PEOPLE_SEQ_NASTY_WALK,    // ���f�{�w��
  FIELD_CROWD_PEOPLE_SEQ_NASTY_END,     // �I��
};

// ����^�C�v
enum
{
  FIELD_CROWD_PEOPLE_TYPE_NORMAL,   // ���ʂ̐l
  FIELD_CROWD_PEOPLE_TYPE_NASTY,    // �������@�Ȑl

  FIELD_CROWD_PEOPLE_TYPE_MAX,    //
};

// �l���ő吔
#define FIELD_CROWD_PEOPLE_MAX  (32)

// �Q�WOBJ����ID
#define FIELD_CROWD_PEOPLE_ID (48) // �d�|���悤OBJID���g�p�����Ă��炤

// �l����{�l�w�b�_�[
static MMDL_HEADER s_MMDL_HEADER = 
{
  FIELD_CROWD_PEOPLE_ID,  ///<����ID
  BOY1,                      ///<�\������OBJ�R�[�h
  MV_DMY,                 ///<����R�[�h
  0,                      ///<�C�x���g�^�C�v
  0,                      ///<�C�x���g�t���O
  0,                      ///<�C�x���gID
  0,                      ///<�w�����
  0,                      ///<�w��p�����^ 0
  0,                      ///<�w��p�����^ 1
  0,                      ///<�w��p�����^ 2
	0,			                ///<X�����ړ�����
	0,			                ///<Z�����ړ�����
	MMDL_HEADER_POSTYPE_GRID,///<�|�W�V�����^�C�v
};

// �I�u�Z�b�g�𓮂�������
#define FIELD_CROWD_PEOPLE_OFFSET_MOVE_MIN  ( -4 )
#define FIELD_CROWD_PEOPLE_OFFSET_MOVE_RND  ( 8 )

//-------------------------------------
///	�ʏ퓮��@�萔
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_GRID (2) // �����O�܂Ń`�F�b�N����̂��H


//-------------------------------------
///	�Ӓn������@�萔
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID (1) // �����O�܂Ń`�F�b�N����̂��H

#define FIELD_CROWD_PEOPLE_PUT_PAR (1) // FIELD_CROWD_PEOPLE_PUT_PAR��Ɉ��Ӓn�����ł�



//-------------------------------------
///	�N�����
//=====================================
#define FIELD_CROWD_PEOPLE_BOOT_POINT_MAX (2)
#define FIELD_CROWD_PEOPLE_OBJ_CODE_MAX (4)   // �\��OBJ_CODE�̍ő吔

#define FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME (6) // �����̃t���[����


#define FIELD_CROWD_PEOPLE_SCRIPT_MAX   (4) // OBJCODE�̃X�N���v�g�̐�


//-------------------------------------
///	�N����񃊃X�g�̃f�[�^
//=====================================
typedef u32 BOOT_ZONEID_TYPE;


//-------------------------------------
///	�����@�萔
//=====================================
#define NOISE_TIME_MIN  ( 16)
#define NOISE_TIME_WAIT_NUM  ( 1 )
#define NOISE_MSG_MAX  ( 20 )
#define NOISE_SIZX  ( 8 )
#define NOISE_SIZY  ( 2 )
#define NOISE_WK_WAIT  ( 48)

#define NOISE_WK_MAX  ( 16 )

static const u8 sc_NOISE_X[ NOISE_WK_MAX ] = 
{
  1,
  22,
  2,
  20,
  4,
  21,
  3,
  19,
  1,
  19,
  2,
  22,
  3,
  20,
  4,
  21,
};
static const u8 sc_NOISE_Y[ NOISE_WK_MAX ] = 
{
  6,
  2,
  17,
  14,
  17,
  10,
  14,
  2,
  10,
  17,
  10,
  6,
  2,
  6,
  14,
  6,
};


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	objcode �X�N���v�g�e�[�u��
//=====================================
typedef struct 
{
  u32 objcode;
  u16 script[FIELD_CROWD_PEOPLE_SCRIPT_MAX];
} FIELD_CROWD_PEOPLE_OBJSCRIPT;


//-------------------------------------
///	�N���_���
//=====================================
typedef struct 
{
  // �N���O���b�h�͈�
  u16 top;
  u16 bottom;
  u16 left;
  u16 right;

  // �ړ�����
  u16 move_dir;

  // �ړ�����
  u16 move_grid;

} FIELD_CROWD_PEOPLE_BOOT_POINT;

//-------------------------------------
///	�N�����
//=====================================
typedef struct 
{
  s16 wait;       // �N���E�G�C�g 0�ŒZ 
  u8 point_num;  // �N���|�C���g�̐�
  u8 obj_code_num;// OBJCODE�̐�
  u16 obj_code_tbl[FIELD_CROWD_PEOPLE_OBJ_CODE_MAX]; // OBJCODE�̃e�[�u��
  
  FIELD_CROWD_PEOPLE_BOOT_POINT point[FIELD_CROWD_PEOPLE_BOOT_POINT_MAX];
} FIELD_CROWD_PEOPLE_BOOT_DATA;


//-------------------------------------
///	�N���R���g���[���V�X�e��
//=====================================
typedef struct 
{
  FIELD_CROWD_PEOPLE_BOOT_DATA boot_data;
  s16 wait[ FIELD_CROWD_PEOPLE_BOOT_POINT_MAX ];


  FIELD_CROWD_PEOPLE_OBJSCRIPT* p_objcode_script_tbl;
  u32 objcode_num;
} FIELD_CROWD_PEOPLE_BOOL_CONTROL;


//-------------------------------------
///	�������[�N
//=====================================
typedef struct {
  u16 flag;
  u8 x;
  u8 y;
  u16 id;
  s16 wait;
} FIELD_CROWD_PEOPLE_NOISE_WK;



//-------------------------------------
///	�T�u�E�B���h�E�@�����V�X�e��
//=====================================
typedef struct 
{
  s16 wait;
  s16 wait_max;
  u32 add_idx;
  GFL_MSGDATA * p_msgdata;
  FLDMSGBG* p_fmb;

  FIELD_CROWD_PEOPLE_NOISE_WK wk[NOISE_WK_MAX];
} FIELD_CROWD_PEOPLE_NOISE;


//-------------------------------------
///	�Ǘ��P�I�u�W�F
//=====================================
typedef struct 
{
  u8 flag;
  u8 seq;
  u8 type;
  u8 move_dir;
  u32 move_grid;
  MMDL* p_mmdl;
} FIELD_CROWD_PEOPLE_WK;


//-------------------------------------
///	�Ǘ����[�N
//=====================================
typedef struct 
{
  FIELD_PLAYER* p_player;
  MMDLSYS* p_fos;
  FIELD_CROWD_PEOPLE_WK people_wk[FIELD_CROWD_PEOPLE_MAX];

  FIELD_CROWD_PEOPLE_BOOL_CONTROL boot_control;

  FIELD_CROWD_PEOPLE_NOISE noise;
} FIELD_CROWD_PEOPLE;




//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------





static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Delete( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Update( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );



// 
static FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_GetClearWk( FIELD_CROWD_PEOPLE* p_sys );
static const FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_Search( const FIELD_CROWD_PEOPLE* cp_sys, u16 gx, u16 gz, u16 move_dir, u16 type );

//-------------------------------------
///	���[�N�Ǘ�
//=====================================
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_data, u32 idx );
static void FIELD_CROWD_PEOPLE_WK_Exit( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_Main( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid );
static void FIELD_CROWD_PEOPLE_WK_ClearMove( FIELD_CROWD_PEOPLE_WK* p_wk );


// �N���Ǘ�
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int zone_id, HEAPID heapID );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, FIELD_CROWD_PEOPLE* p_sysdata );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( const FIELD_CROWD_PEOPLE* cp_sysdata, const FIELD_CROWD_PEOPLE_BOOT_DATA* cp_data, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point, FIELD_CROWD_PEOPLE_WK* p_people, u16 add_grid );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk, FIELD_CROWD_PEOPLE* p_sysdata );

// �^�C�v���Ƃ̓���
static void FIELD_CROWD_PEOPLE_WK_SetUpNormal( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_SetUpNasty( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_MainNormal( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_MainNasty( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );


// �������O�Ɏ��@�����Ȃ����`�F�b�N
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( const FIELD_CROWD_PEOPLE_WK* cp_wk, const FIELD_PLAYER* cp_player, u16 grid );
static BOOL FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( s16 mygx, s16 mygz, s16 pl_gx, s16 pl_gz, u16 grid, u16 move_dir );



// NOISE�Ǘ�
static void FIELD_CROWD_PEOPLE_NOISE_Init( FIELD_CROWD_PEOPLE_NOISE* p_wk, FLDMSGBG* p_fmb, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, HEAPID heapID );
static void FIELD_CROWD_PEOPLE_NOISE_Exit( FIELD_CROWD_PEOPLE_NOISE* p_wk );
static void FIELD_CROWD_PEOPLE_NOISE_Main( FIELD_CROWD_PEOPLE_NOISE* p_wk, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, const FIELDMAP_WORK* cp_fieldmap );

static void FIELD_CROWD_PEOPLE_NOISEWK_Init( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, u16 id );
static void FIELD_CROWD_PEOPLE_NOISEWK_Start( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb, GFL_MSGDATA* p_msgdata );
static void FIELD_CROWD_PEOPLE_NOISEWK_Main( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb );
static void FIELD_CROWD_PEOPLE_NOISEWK_Clear( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_NOISEWK_Off( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb );



static const FLDMAPFUNC_DATA sc_FLDMAPFUNC_DATA = 
{
  0,
  sizeof(FIELD_CROWD_PEOPLE),
  FIELD_CROWD_PEOPLE_TASK_Create,
  FIELD_CROWD_PEOPLE_TASK_Delete,
  FIELD_CROWD_PEOPLE_TASK_Update,
  NULL,
};



//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W�Ǘ��^�X�N����
 *
 *	@param	sys   �V�X�e��
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FLDMAPFUNC_WORK * FIELD_CROWD_PEOPLE_Create( FLDMAPFUNC_SYS * sys )
{
  return FLDMAPFUNC_Create( sys, &sc_FLDMAPFUNC_DATA );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W���[�N����
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work ) {
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;
  int zone_id;
  HEAPID heapID;
  FLDMSGBG* p_fmb = FIELDMAP_GetFldMsgBG( p_fieldmap );

  // �Ǘ��V�X�e�����[�N������
  p_wk->p_player  = FIELDMAP_GetFieldPlayer( p_fieldmap );
  p_wk->p_fos     = FIELDMAP_GetMMdlSys( p_fieldmap );
  zone_id = FIELDMAP_GetZoneID( p_fieldmap );
  heapID = FIELDMAP_GetHeapID( p_fieldmap );

  // �]�[������N������ǂݍ���
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( &p_wk->boot_control, zone_id, heapID );

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Init( &p_wk->people_wk[i], p_wk->p_fos, zone_id, &p_wk->boot_control, i ); 
  }

  // �S����z�u
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( &p_wk->boot_control, p_wk );


  // �����V�X�e��������
  FIELD_CROWD_PEOPLE_NOISE_Init( &p_wk->noise, p_fmb, &p_wk->boot_control, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W���[�N�j��
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Delete( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;
  
  // �Ǘ��V�X�e�����[�N�j��
  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Exit( &p_wk->people_wk[i] ); 
  }

  // �]�[������N������j��
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( &p_wk->boot_control );

  // �����V�X�e���j��
  FIELD_CROWD_PEOPLE_NOISE_Exit( &p_wk->noise );

  p_wk->p_player  = NULL;
  p_wk->p_fos     = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W�@�X�V
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Update( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;

  // �N���`�F�b�N
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( &p_wk->boot_control, p_wk ); 

  
  // �Ǘ��V�X�e�����[�N�X�V
  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Main( &p_wk->people_wk[i], p_wk->p_fos, p_wk->p_player ); 
  }

  // �����V�X�e���X�V
  FIELD_CROWD_PEOPLE_NOISE_Main( &p_wk->noise, &p_wk->boot_control, p_fieldmap );
}






//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă��郏�[�N���擾
 *
 *	@param	p_sys 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_GetClearWk( FIELD_CROWD_PEOPLE* p_sys )
{
  int i;

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    if( p_sys->people_wk[i].flag == FALSE )
    {
      return &p_sys->people_wk[i];
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  gx gz����move_dir�̕����Ł@type���삵�Ă���I�u�W�F������
 *
 *	@param	cp_sys       �V�X�e�����[�N
 *	@param	gx          �O���b�hX
 *	@param	gz          �O���b�hZ
 *	@param	move_dir    �������
 *	@param	type        �^�C�v 
 *
 *	@return �l�����[�N
 */
//-----------------------------------------------------------------------------
static const FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_Search( const FIELD_CROWD_PEOPLE* cp_sys, u16 gx, u16 gz, u16 move_dir, u16 type )
{
  int i;
  s16 move_x;
  s16 move_z;
  s16 dif_x;
  s16 dif_z;
  s16 mdl_gx, mdl_gz;



  switch( move_dir )
  {
  case DIR_UP:
    move_x = 0;
    move_z = -1;
    break;
  case DIR_DOWN:
    move_x = 0;
    move_z = 1;
    break;
  case DIR_RIGHT:
    move_x = 1;
    move_z = 0;
    break;
  case DIR_LEFT:
    move_x = -1;
    move_z = 0;
    break;
  }
  

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    if( cp_sys->people_wk[i].flag )
    {
      if(cp_sys->people_wk[i].type == FIELD_CROWD_PEOPLE_TYPE_NASTY)
      {
        // �O���b�h�����ɂ��邩�H
        mdl_gx = MMDL_GetGridPosX( cp_sys->people_wk[i].p_mmdl );
        mdl_gz = MMDL_GetGridPosZ( cp_sys->people_wk[i].p_mmdl );

        dif_x = mdl_gx - gx;
        dif_z = mdl_gz - gz;

        if( dif_x != 0 ){
          dif_x /= MATH_ABS( dif_x );
        }
        if( dif_z != 0 ){
          dif_z /= MATH_ABS( dif_z );
        }

        // �ړ���������v������AOK
        if( (dif_x == move_x) || (dif_z == move_z) )
        {
          return &cp_sys->people_wk[i];
        }
      }
    }
  }

  return NULL;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N  ������
 *
 *	@param	p_wk      ���[�N
 *	@param	p_fos     ���샂�f���V�X�e��
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_data, u32 idx )
{
  u16 obj_code;
  u32 rand = GFUser_GetPublicRand(0);
  int i;


  // �����ڌ���
  // �e�[�u�����烉���_���ŁB
  obj_code = cp_data->boot_data.obj_code_tbl[ rand % cp_data->boot_data.obj_code_num ];

  s_MMDL_HEADER.obj_code = obj_code;
  s_MMDL_HEADER.id = FIELD_CROWD_PEOPLE_ID + idx;

  for( i=0; i<cp_data->objcode_num; i++ )
  {
    if( cp_data->p_objcode_script_tbl[i].objcode == obj_code )
    {
      s_MMDL_HEADER.event_id = cp_data->p_objcode_script_tbl[i].script[ rand % FIELD_CROWD_PEOPLE_SCRIPT_MAX ];
      break;
    }
  }
  // OBJCODE�̃X�N���v�g��ݒ肵�����`�F�b�N
  GF_ASSERT( i < cp_data->objcode_num );
  
  p_wk->p_mmdl    = MMDLSYS_AddMMdl( p_fos, &s_MMDL_HEADER, zone_id ); 
  p_wk->flag      = 0;
  p_wk->seq       = 0;
  p_wk->type      = 0;
  p_wk->move_dir  = 0;

  // Vanish
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, TRUE );

  // �`��I�t�Z�b�g�ݒ�
  {
    VecFx32 offset = {0,0,0};

    // �n���O���b�h�@�{�@�|
    offset.x = (FIELD_CROWD_PEOPLE_OFFSET_MOVE_MIN<<FX32_SHIFT) + GFUser_GetPublicRand(FIELD_CROWD_PEOPLE_OFFSET_MOVE_RND<<FX32_SHIFT);
    MMDL_SetVectorDrawOffsetPos( p_wk->p_mmdl, &offset );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�@�j��
 *
 *	@param	p_wk      ���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Exit( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  // �I�u�W�F��j��
  MMDL_Delete( p_wk->p_mmdl );
  p_wk->p_mmdl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�@���C��
 *
 *	@param	p_wk        ���[�N
 *	@param  p_fos       ����V�X�e�����[�N
 *	@param	cp_player   �v���C���[�V�X�e��
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Main( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  static void (* const cpFunc[FIELD_CROWD_PEOPLE_TYPE_MAX])( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player ) = 
  {
    FIELD_CROWD_PEOPLE_WK_MainNormal,
    FIELD_CROWD_PEOPLE_WK_MainNasty,
  };
  
  if( p_wk->flag == FALSE ){
    return ;
  }
  
  GF_ASSERT( p_wk->type < FIELD_CROWD_PEOPLE_TYPE_MAX );
  
  // �^�C�v���Ƃ̓���
  cpFunc[ p_wk->type ]( p_wk, p_fos, cp_player );

  // ���W��\��
#ifdef PM_DEBUG
  /*
  {
    VecFx32 pos;
    MMDL_GetVectorPos( p_wk->p_mmdl, &pos );
    TOMOYA_Printf( "mdl pos x[%d] y[%d] z[%d]\n", FX_Whole(pos.x), FX_Whole(pos.y), FX_Whole(pos.z) );
  }
  */
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�@����Z�b�g�A�b�v
 *
 *	@param	p_wk        ���[�N
 *	@param	p_fos       ���샂�f���V�X�e��
 *	@param	cp_player   ��l�����[�N
 *	@param  type        ����^�C�v
 *	@param	gx          �J�n�O���b�h�ʒu
 *	@param	gz          �J�n�O���b�h�ʒu
 *	@param	move_dir    �������
 *	@param  move_grid   ����O���b�h����
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid )
{
  static void (* const cpFunc[FIELD_CROWD_PEOPLE_TYPE_MAX])( FIELD_CROWD_PEOPLE_WK* p_wk ) = 
  {
    FIELD_CROWD_PEOPLE_WK_SetUpNormal,
    FIELD_CROWD_PEOPLE_WK_SetUpNasty,
  };

  GF_ASSERT( p_wk->flag == FALSE );

  // ����J�n
  p_wk->flag = TRUE; 

  // �\��ON
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, FALSE );

  // �l���̏���傫���ύX
  {
    MMDL_HEADER header = s_MMDL_HEADER;
    header.obj_code = 10;
    header.dir      = move_dir;
    MMDLHEADER_SetGridPos( &header, gx, gz, 0 );
    MMDL_ChangeMoveParam( p_wk->p_mmdl, &header );
  }
  p_wk->type = type;
  p_wk->move_dir = move_dir;
  p_wk->seq = 0;
  p_wk->move_grid = move_grid;

  // �������[�N���̐ݒ�
  cpFunc[ type ]( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brieif ������̃N���A
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_ClearMove( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  p_wk->flag = FALSE;
  p_wk->type = 0;
  p_wk->move_dir = 0;
  p_wk->seq = 0;

  // �\��OFF
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, TRUE );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���@�N���@�Ǘ������@������
 *
 *	@param	p_wk        ���[�N
 *	@param	zone_id     �]�[��ID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int zone_id, HEAPID heapID )
{
  int i;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_FIELD_CROWD_PEOPLE, GFL_HEAP_LOWID(heapID) );

  {
    BOOT_ZONEID_TYPE* p_list;
    u32 list_num;
    u32 list_idx;
    u32 size;

    // �N��IDX�����߂�
    p_list = GFL_ARC_LoadDataAllocByHandle( p_handle, NARC_field_crowd_people_list_data_dat, GFL_HEAP_LOWID(heapID) );
    size = GFL_ARC_GetDataSizeByHandle( p_handle, NARC_field_crowd_people_list_data_dat );
    list_num = size / sizeof(BOOT_ZONEID_TYPE);

    for( i=0; i<list_num; i++ )
    {
      if( p_list[i] == zone_id )
      {
        break;
      }
    }
    GF_ASSERT_MSG( i < list_num, "zone�ɌQ�W��񂪂���܂���B\n" );
    list_idx = i;

    TOMOYA_Printf( "crowd people list_idx %d\n", list_idx );
    
    // �N���|�C���g����ǂݍ���
    GFL_ARC_LoadDataOfsByHandle( p_handle, list_idx, 0, sizeof(FIELD_CROWD_PEOPLE_BOOT_DATA), &p_wk->boot_data );

    GFL_HEAP_FreeMemory( p_list );
  }

  // �X�N���v�g���ǂݍ���
  {
    u32 size;
    size = GFL_ARC_GetDataSizeByHandle( p_handle, NARC_field_crowd_people_script_dat );


    p_wk->p_objcode_script_tbl = GFL_ARC_LoadDataAllocByHandle( p_handle, NARC_field_crowd_people_script_dat, heapID );
    p_wk->objcode_num = size / sizeof(FIELD_CROWD_PEOPLE_OBJSCRIPT);
  }

  GFL_ARC_CloseDataHandle( p_handle );

  for( i=0; i<FIELD_CROWD_PEOPLE_BOOT_POINT_MAX; i++ )
  {
    p_wk->wait[i] = GFUser_GetPublicRand(p_wk->boot_data.wait); // �����E�G�C�g�l
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���@�N���@�Ǘ������@�j��
 *
 *	@param	p_wk        ���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk->p_objcode_script_tbl );
  p_wk->p_objcode_script_tbl = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���@�N���@���C������
 *
 *	@param	p_wk        ���[�N
 *	@param	p_sysdata   �V�X�e���f�[�^
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, FIELD_CROWD_PEOPLE* p_sysdata )
{
  int i;
  
  for( i=0; i<p_wk->boot_data.point_num; i++ )
  {
    // �E�G�C�g���Ԃ���������A�l���o��
    if( p_wk->wait[i] >= p_wk->boot_data.wait )
    {
      FIELD_CROWD_PEOPLE_WK* p_people = FIELD_CROWD_PEOPLE_GetClearWk( p_sysdata );
      if( p_people )
      {
        // ���̃|�C���g����l���o���B
        FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( p_sysdata, &p_wk->boot_data, &p_wk->boot_data.point[i], p_people, 0 ); 
        p_wk->wait[i] = 0;
      }
      else
      {
        TOMOYA_Printf( "CROWD people people none\n" );
      }
    }
    else
    {
      p_wk->wait[i] ++;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �N���Ǘ������@�l�����̃Z�b�g�A�b�v
 *
 *	@param	cp_data       �N���f�[�^
 *	@param	cp_point      �N���|�C���g�f�[�^
 *	@param	p_people      �Z�b�g�A�b�v�l��
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( const FIELD_CROWD_PEOPLE* cp_sysdata, const FIELD_CROWD_PEOPLE_BOOT_DATA* cp_data, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point, FIELD_CROWD_PEOPLE_WK* p_people, u16 add_grid )
{
  u16 gx, gz;
  u16 type = FIELD_CROWD_PEOPLE_TYPE_NORMAL;
  u32 rand = GFUser_GetPublicRand( 0 );
  s32 move_grid;
  
  // �N���O���b�h����
  if( (cp_point->move_dir == DIR_UP) || (cp_point->move_dir == DIR_DOWN) )
  {
    gx = cp_point->left + (rand % ((cp_point->right - cp_point->left)+1));
    if(cp_point->move_dir == DIR_UP){
      gz = cp_point->top - add_grid;
    }else{
      gz = cp_point->top + add_grid;
    }
  }
  else
  {
    if(cp_point->move_dir == DIR_LEFT){
      gx = cp_point->left - add_grid;
    }else{
      gx = cp_point->left + add_grid;
    }
    gz = cp_point->top + (rand % ((cp_point->bottom - cp_point->top)+1));
  }

  // ����^�C�v����
  // ���̃O���b�h��ɘb����l�����Ȃ���΁A�b����Ӓn���Ȑl���o���B
  if( FIELD_CROWD_PEOPLE_Search( cp_sysdata, gx, gz, cp_point->move_dir, FIELD_CROWD_PEOPLE_TYPE_NASTY ) == NULL )
  {
    if( (rand % FIELD_CROWD_PEOPLE_PUT_PAR) == 0 )
    {
      type = FIELD_CROWD_PEOPLE_TYPE_NASTY;
    }
  }

  move_grid = cp_point->move_grid - add_grid;
  if( move_grid <= 0 ){
    move_grid = 1;
  }

  // �ݒ�
  FIELD_CROWD_PEOPLE_WK_SetUp( p_people, cp_sysdata->p_fos, cp_sysdata->p_player, type, gx, gz, cp_point->move_dir, cp_point->move_grid - add_grid );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�n�l���z�u
 *
 *	@param	p_wk        ���[�N
 *	@param	p_sysdata   �V�X�e���f�[�^
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk, FIELD_CROWD_PEOPLE* p_sysdata )
{
  int i, j;
  int total_time;
  int total_people_num;
  int time;

  for( i=0; i<cp_wk->boot_data.point_num; i++ )
  {
    // �S���̈ʒu��������߂ɕK�v�ȑS�̃^�C��
    total_time = cp_wk->boot_data.point[i].move_grid * FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME;

    // wait�Ŋ���ƁA���l�����`�F�b�N
    total_people_num = total_time / cp_wk->boot_data.wait;

    // ���̐l���Z�b�g�A�b�v
    time = 0;
    for( j=0; j<total_people_num; j++ )
    {
      FIELD_CROWD_PEOPLE_WK* p_people = FIELD_CROWD_PEOPLE_GetClearWk( p_sysdata );

      if( p_people ){
        FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( p_sysdata, &cp_wk->boot_data, &cp_wk->boot_data.point[i], p_people, time / FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME ); 
        time += cp_wk->boot_data.wait;
      }
    }
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ퓮��̃Z�b�g�A�b�v
 *
 *	@param	p_wk  ���[�N
 */ 
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUpNormal( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  MMDL_SetStatusBitHeightGetOFF( p_wk->p_mmdl, TRUE );  // �����擾OFF
  MMDL_SetStatusBitAttrGetOFF( p_wk->p_mmdl, TRUE );    // �A�g���r���[�g�擾OFF
  MMDL_SetStatusBitFellowHit( p_wk->p_mmdl, FALSE );    // ���̃I�u�W�F�Ƃ̔��� OFF
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Ӓn������̃Z�b�g�A�b�v
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUpNasty( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  MMDL_SetStatusBitHeightGetOFF( p_wk->p_mmdl, TRUE );  // �����擾OFF
  MMDL_SetStatusBitAttrGetOFF( p_wk->p_mmdl, TRUE );    // �A�g���r���[�g�擾OFF
  MMDL_SetStatusBitFellowHit( p_wk->p_mmdl, TRUE );    // ���̃I�u�W�F�Ƃ̔��� ON
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ�I�u�W�F����
 *
 *	@param	p_wk        ���[�N
 *	@param	p_fos       ����I�u�W�F�V�X�e��
 *	@param	cp_player   ��l�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_MainNormal( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  switch( p_wk->seq )
  {
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT:// ���슮���҂�
    if( MMDL_CheckEndAcmd( p_wk->p_mmdl ) )
    {
      p_wk->seq ++;
    }
    else
    {
      break;
    }
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK:    // ���f�{�w��

    // �I������
    if( p_wk->move_grid <= 0 )
    {
      // �I��
      p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_END;
      break;
    }

    // ��l���ƂԂ��邩�`�F�b�N
    if( FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( p_wk, cp_player, FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_GRID ) )
    {
      u16 player_dir = FIELD_PLAYER_GetDir( cp_player );
      u16 move_dir;
   
      // �悯��
      // �悯�����������
      if( (player_dir == p_wk->move_dir) || (MMDL_TOOL_FlipDir(player_dir) == p_wk->move_dir) )
      {
        // �����_���łX�O�x��̂ǂ��炩�̕����ɂ���
        if( GFUser_GetPublicRand(2) == 0 )
        {
          move_dir = player_dir + 2;
        }
        else
        {
          move_dir = MMDL_TOOL_FlipDir(player_dir + 2);
        }
      }
      else
      {
        // �v���C���[�̔��Ε����ɂ悯��
        move_dir = MMDL_TOOL_FlipDir(player_dir);
      }

      // �悯�܁[���B
      MMDL_SetAcmd( p_wk->p_mmdl, AC_WALK_U_6F + move_dir );
    }
    else
    {
      // ���̂܂ܐi��
      MMDL_SetAcmd( p_wk->p_mmdl, AC_WALK_U_6F + p_wk->move_dir );
      p_wk->move_grid --; // �ړ��������Z
    }
    p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT;
    break;

  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_END:     // �I��
    // ���̃N���A
    FIELD_CROWD_PEOPLE_WK_ClearMove( p_wk );
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Ӓn���I�u�W�F����  ���i����݂̂ł��B
 *
 *	@param	p_wk        ���[�N
 *	@param	p_fos       ����I�u�W�F�V�X�e��
 *	@param	cp_player   ��l�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_MainNasty( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  switch( p_wk->seq )
  {
  case FIELD_CROWD_PEOPLE_SEQ_NASTY_WALKWAIT:// ���슮���҂�
    if( MMDL_CheckEndAcmd( p_wk->p_mmdl ) )
    {
      p_wk->seq ++;
    }
    else
    {
      break;
    }
  case FIELD_CROWD_PEOPLE_SEQ_NASTY_WALK:    // ���f�{�w��

    // �I������
    if( p_wk->move_grid <= 0 )
    {
      // �I��
      p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NASTY_END;
      break;
    }

    // ��l���ƂԂ��邩�`�F�b�N
    if( FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( p_wk, cp_player, FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID ) )
    {
      // �Ԃ���Ȃ牽�����Ȃ�
      break;
    }
    else
    {
      // ���̂܂ܐi��
      MMDL_SetAcmd( p_wk->p_mmdl, AC_WALK_U_6F + p_wk->move_dir );
      p_wk->move_grid --; // �ړ��������Z
    }
    p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NASTY_WALKWAIT;
    break;

  case FIELD_CROWD_PEOPLE_SEQ_NASTY_END:     // �I��
    // ���̃N���A
    FIELD_CROWD_PEOPLE_WK_ClearMove( p_wk );
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  grid���O�܂łɎ��@�����Ȃ����`�F�b�N
 *
 *	@param	cp_wk         ���[�N
 *	@param	cp_player     �v���C���[
 *	@param  grid          �O���b�h
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( const FIELD_CROWD_PEOPLE_WK* cp_wk, const FIELD_PLAYER* cp_player, u16 grid )
{
  s16 player_gx, player_gz;
  s16 my_gx, my_gz;
  const MMDL* cp_playermmdl = FIELD_PLAYER_GetMMdl( cp_player );
  
  player_gx = MMDL_GetGridPosX( cp_playermmdl );
  player_gz = MMDL_GetGridPosZ( cp_playermmdl );
  my_gx = MMDL_GetGridPosX( cp_wk->p_mmdl );
  my_gz = MMDL_GetGridPosZ( cp_wk->p_mmdl );
  
  if( FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( my_gx, my_gz, player_gx, player_gz, grid, cp_wk->move_dir ) )
  {
    return TRUE;
  }

  player_gx = MMDL_GetOldGridPosX( cp_playermmdl );
  player_gz = MMDL_GetOldGridPosZ( cp_playermmdl );
  
  return FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( my_gx, my_gz, player_gx, player_gz, grid, cp_wk->move_dir );
}

//----------------------------------------------------------------------------
/**
 *	@brief  grid���O�ɂ���̂��̃O���b�h�P�ʂ̃`�F�b�N
 *
 *	@param	mygx
 *	@param	mygz
 *	@param	pl_gx
 *	@param	pl_gz
 *	@param	grid 
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( s16 mygx, s16 mygz, s16 pl_gx, s16 pl_gz, u16 grid, u16 move_dir )
{
  s32 grid_dif;
  s32 grid_dif_2;

  switch( move_dir )
  {
  case DIR_UP:
    grid_dif = mygz - pl_gz;
    grid_dif_2 = mygx - pl_gx;
    break;
  case DIR_DOWN:
    grid_dif = pl_gz - mygz;
    grid_dif_2 = mygx - pl_gx;
    break;
  case DIR_LEFT:
    grid_dif = mygx - pl_gx;
    grid_dif_2 = mygz - pl_gz;
    break;
  case DIR_RIGHT:
    grid_dif = pl_gx - mygx;
    grid_dif_2 = mygz - pl_gz;
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  if( ((grid_dif <= grid) && (grid_dif >= 0)) && (grid_dif_2 == 0) )
  {
    return TRUE;
  }
  return FALSE;
}




//----------------------------------------------------------------------------
/**
 *	@brief  �����V�X�e��  ������
 *
 *	@param	p_wk      ���[�N
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Init( FIELD_CROWD_PEOPLE_NOISE* p_wk, FLDMSGBG* p_fmb, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, HEAPID heapID )
{
  int i;
  u32 rand = GFUser_GetPublicRand(0);
  
  p_wk->wait = 0;
  p_wk->wait_max = NOISE_TIME_MIN + ( rand % (cp_boot->boot_data.wait*NOISE_TIME_WAIT_NUM) );
  p_wk->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_crowd_people_scr_dat, heapID );
  p_wk->p_fmb = p_fmb;

  p_wk->add_idx = rand % NOISE_WK_MAX;

  for( i=0; i<NOISE_WK_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_NOISEWK_Init( &p_wk->wk[i], i );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����V�X�e���@�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Exit( FIELD_CROWD_PEOPLE_NOISE* p_wk )
{
  int i;
  // �SOFF
  for( i=0; i<NOISE_WK_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_NOISEWK_Off( &p_wk->wk[i], p_wk->p_fmb );
  }
  p_wk->p_fmb = NULL;
  
  GFL_MSG_Delete( p_wk->p_msgdata );
  p_wk->p_msgdata = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����V�X�e���@���C��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Main( FIELD_CROWD_PEOPLE_NOISE* p_wk, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, const FIELDMAP_WORK* cp_fieldmap )
{
  int i;
  u32 rand = GFUser_GetPublicRand(0);

  if( FIELDMAP_CheckDoEvent( cp_fieldmap ) == FALSE )
  {
    p_wk->wait ++;

    // �E�G�C�g��������A�����_�����b�Z�[�W�\��
    if( p_wk->wait >= p_wk->wait_max )
    {
      // �\��
      FIELD_CROWD_PEOPLE_NOISEWK_Start( &p_wk->wk[ p_wk->add_idx ], p_wk->p_fmb, p_wk->p_msgdata );
      p_wk->add_idx = (p_wk->add_idx + 1) % NOISE_WK_MAX;

      // �E�G�C�g������
      p_wk->wait = 0;
      p_wk->wait_max = NOISE_TIME_MIN + (rand%(cp_boot->boot_data.wait*NOISE_TIME_WAIT_NUM));
    }

    // ���b�Z�[�W�\��
    for( i=0; i<NOISE_WK_MAX; i++ )
    {
      FIELD_CROWD_PEOPLE_NOISEWK_Main( &p_wk->wk[i], p_wk->p_fmb );
    }
  }
  else
  {
    // �SOFF
    for( i=0; i<NOISE_WK_MAX; i++ )
    {
      FIELD_CROWD_PEOPLE_NOISEWK_Off( &p_wk->wk[i], p_wk->p_fmb );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�̏�����
 *
 *	@param	p_wk  ���[�N
 *	@param	id    ID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Init( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, u16 id )
{
  p_wk->flag = FALSE;

  // �e����͍�����
  p_wk->x     = sc_NOISE_X[id];
  p_wk->y     = sc_NOISE_Y[id];
  p_wk->id    = id;
  p_wk->wait  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�J�n
 *
 *	@param	p_wk
 *	@param	p_fmb 
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Start( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb, GFL_MSGDATA* p_msgdata )
{
  u32 rand = GFUser_GetPublicRand( 0 );
  p_wk->flag = TRUE;
  p_wk->wait = 0;
  
  FLDSUBMSGWIN_Add( p_fmb, p_msgdata, 
      CROWD_NOISE_01 + (rand%NOISE_MSG_MAX), p_wk->id,
      p_wk->x, p_wk->y, NOISE_SIZX, NOISE_SIZY );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Main( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb )
{
  if( p_wk->flag == FALSE )
  {
    return ;
  }
  
  // wait�܂��ď�����
  p_wk->wait ++;
  if( p_wk->wait >= NOISE_WK_MAX )
  {
    FLDSUBMSGWIN_Delete( p_fmb, p_wk->id );
    FIELD_CROWD_PEOPLE_NOISEWK_Clear( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�NOFF����
 */ 
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Clear( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk )
{
  p_wk->wait = 0;
  p_wk->flag = FALSE;

  //���̑��̃f�[�^�͎����g���܂�
}

//----------------------------------------------------------------------------
/**
 *	@brief  OFF�ɂ���
 *
 *	@param	p_wk
 *	@param	p_fmb 
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Off( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb )
{
  if( p_wk->flag == FALSE )
  {
    return ;
  }
  
  FLDSUBMSGWIN_Delete( p_fmb, p_wk->id );
  FIELD_CROWD_PEOPLE_NOISEWK_Clear( p_wk );
}




