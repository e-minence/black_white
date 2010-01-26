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

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_crowd_people.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// ����V�[�P���X
enum
{
  // �ʏ�
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT,// ���슮���҂�
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK,    // ���f�{�w��
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_END,     // �I��

  // �Ӓn��
  FIELD_CROWD_PEOPLE_SEQ_NASTY_WALKWAIT,// ���슮���҂�
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
#define FIELD_CROWD_PEOPLE_MAX  (20)

// �Q�WOBJ����ID
#define FIELD_CROWD_PEOPLE_ID (MMDL_ID_GIMMICK) // �d�|���悤OBJID���g�p�����Ă��炤

// �l����{�l�w�b�_�[
static const MMDL_HEADER sc_MMDL_HEADER = 
{
  FIELD_CROWD_PEOPLE_ID,  ///<����ID
  0,                      ///<�\������OBJ�R�[�h
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

//-------------------------------------
///	�ʏ퓮��@�萔
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_GRID (2) // �����O�܂Ń`�F�b�N����̂��H


//-------------------------------------
///	�Ӓn������@�萔
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID (1) // �����O�܂Ń`�F�b�N����̂��H



//-------------------------------------
///	�N�����
//=====================================
#define FIELD_CROWD_PEOPLE_BOOT_POINT_MAX (2)
#define FIELD_CROWD_PEOPLE_OBJ_CODE_MAX (4)   // �\��OBJ_CODE�̍ő吔


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

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
} FIELD_CROWD_PEOPLE_BOOL_CONTROL;



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
} FIELD_CROWD_PEOPLE;




//-------------------------------------
///	�_�~�[�N�����
#include "arc/fieldmap/fldmmdl_objcode.h"
//=====================================
static const FIELD_CROWD_PEOPLE_BOOT_DATA sc_FIELD_CROWD_PEOPLE_BOOT_DATA = 
{
  30,
  2,
  3,
  { BUSINESSMAN, OL, MAID },

  {
    {
      // top bottom left right
      0, 0, 16, 19,
      // move_dir
      DIR_DOWN,
      // move_grid
      63
    },
    {
      // top bottom left right
      63, 63, 16, 19,
      // move_dir
      DIR_UP,
      // move_grid
      63
    },
  },
  
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------





static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Delete( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Update( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


//-------------------------------------
///	���[�N�Ǘ�
//=====================================
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id );
static void FIELD_CROWD_PEOPLE_WK_Exit( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_Main( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 obj_code, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid );
static void FIELD_CROWD_PEOPLE_WK_ClearMove( FIELD_CROWD_PEOPLE_WK* p_wk );


// �N���Ǘ�
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int zone_id );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk );

// �^�C�v���Ƃ̓���
static void FIELD_CROWD_PEOPLE_WK_SetUpNormal( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_SetUpNasty( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_MainNormal( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_MainNasty( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );


// �������O�Ɏ��@�����Ȃ����`�F�b�N
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( const FIELD_CROWD_PEOPLE_WK* cp_wk, const FIELD_PLAYER* cp_player, u16 grid );


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
static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;
  int zone_id;

  // �Ǘ��V�X�e�����[�N������
  p_wk->p_player  = FIELDMAP_GetFieldPlayer( p_fieldmap );
  p_wk->p_fos     = FIELDMAP_GetMMdlSys( p_fieldmap );
  zone_id = FIELDMAP_GetZoneID( p_fieldmap );

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Init( &p_wk->people_wk[i], p_wk->p_fos, zone_id ); 
  }

  // �]�[������N������ǂݍ���
  {
    //GFL_STD_MemCopy( &sc_FIELD_CROWD_PEOPLE_BOOT_DATA, &p_wk->boot_data, sizeof(FIELD_CROWD_PEOPLE_BOOT_DATA) );
  }
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
  

  
  // �Ǘ��V�X�e�����[�N�X�V
  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Main( &p_wk->people_wk[i], p_wk->p_fos, p_wk->p_player ); 
  }
}







//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N  ������
 *
 *	@param	p_wk      ���[�N
 *	@param	p_fos     ���샂�f���V�X�e��
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id )
{
  p_wk->p_mmdl    = MMDLSYS_AddMMdl( p_fos, &sc_MMDL_HEADER, zone_id ); 
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
    offset.x = -((FIELD_CONST_GRID_SIZE/2)<<FX32_SHIFT) + GFUser_GetPublicRand(FIELD_CONST_GRID_SIZE<<FX32_SHIFT);
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
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�@����Z�b�g�A�b�v
 *
 *	@param	p_wk        ���[�N
 *	@param	p_fos       ���샂�f���V�X�e��
 *	@param	cp_player   ��l�����[�N
 *	@param  obj_code    ������
 *	@param  type        ����^�C�v
 *	@param	gx          �J�n�O���b�h�ʒu
 *	@param	gz          �J�n�O���b�h�ʒu
 *	@param	move_dir    �������
 *	@param  move_grid   ����O���b�h����
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 obj_code, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid )
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
    MMDL_HEADER header = sc_MMDL_HEADER;
    header.obj_code = obj_code;
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
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int zone_id )
{
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
  s32 grid_dif;
  const MMDL* cp_playermmdl = FIELD_PLAYER_GetMMdl( cp_player );
  
  player_gx = MMDL_GetGridPosX( cp_playermmdl );
  player_gz = MMDL_GetGridPosZ( cp_playermmdl );
  my_gx = MMDL_GetGridPosX( cp_wk->p_mmdl );
  my_gz = MMDL_GetGridPosZ( cp_wk->p_mmdl );
  
  switch( cp_wk->move_dir )
  {
  case DIR_UP:
    grid_dif = my_gz - player_gz;
    break;
  case DIR_DOWN:
    grid_dif = player_gz - my_gz;
    break;
  case DIR_LEFT:
    grid_dif = my_gx - player_gx;
    break;
  case DIR_RIGHT:
    grid_dif = player_gx - my_gx;
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  if( grid_dif <= grid )
  {
    return TRUE;
  }
  return FALSE;
}




