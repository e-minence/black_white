//======================================================================
/**
 * @file	field_player_grid.c
 * @brief �O���b�h��p �t�B�[���h�v���C���[����
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_player.h"

#include "sound/pm_sndsys.h"

#include "field_player_grid.h"
#include "map_attr.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	PLAYER_MOVE
//--------------------------------------------------------------
typedef enum
{
	PLAYER_MOVE_STOP = 0,
	PLAYER_MOVE_WALK,
	PLAYER_MOVE_TURN,
	PLAYER_MOVE_HITCH,
}PLAYER_MOVE;

//--------------------------------------------------------------
//	PLAYER_SET
//--------------------------------------------------------------
typedef enum
{
	PLAYER_SET_NON = 0,
	PLAYER_SET_STOP,
	PLAYER_SET_WALK,
	PLAYER_SET_TURN,
	PLAYER_SET_HITCH,
  PLAYER_SET_JUMP,
}PLAYER_SET;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_PLAYER_GRID
//--------------------------------------------------------------
struct _TAG_FIELD_PLAYER_GRID
{
	FIELD_PLAYER *fld_player;
	FIELDMAP_WORK *fieldWork;
	int move_state;
	fx16 scale_size;	//FX16_ONE*8
};

//======================================================================
//	proto
//======================================================================
static PLAYER_SET gjiki_CheckMoveStart_Stop(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Walk(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Turn(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Hitch(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_SetMove_Non(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Stop(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Walk(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Turn(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Hitch(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Jump(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_Sound_MoveStop( void );
static void gjiki_Sound_Move( void );

//======================================================================
//	�O���b�h�ړ� �t�B�[���h�v���C���[����
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@����
 * @param	fld_player FIELD_PLAYER
 * @param heapID HEAPID
 * @retval FIELD_PLAYER_GRID
 */
//--------------------------------------------------------------
FIELD_PLAYER_GRID * FIELD_PLAYER_GRID_Init(
		FIELD_PLAYER *fld_player, HEAPID heapID )
{
	FIELD_PLAYER_GRID *g_jiki;
	
	g_jiki = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_GRID) );
	g_jiki->fld_player = fld_player;
	g_jiki->fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
	g_jiki->scale_size = FX16_ONE*8-1;

//SetGridPlayerActTrans( g_jiki->pActCont, &g_jiki->vec_pos );
	return( g_jiki );
}

//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@�폜
 * @param	g_jiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_Delete( FIELD_PLAYER_GRID *g_jiki )
{
	GFL_HEAP_FreeMemory( g_jiki );
}

//======================================================================
//	�O���b�h�ړ� �t�B�[���h�v���C���[����@�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ� �t�B�[���h�v���C���[����@�ړ�
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_Move(
		FIELD_PLAYER_GRID *g_jiki, int key_trg, int key_cont )
{
	u16 dir;
	BOOL debug_flag;
	PLAYER_SET set;
	FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( g_jiki->fld_player );
	
	dir = DIR_NOT;
	if( (key_cont&PAD_KEY_UP) ){
		dir = DIR_UP;
	}else if( (key_cont&PAD_KEY_DOWN) ){
		dir = DIR_DOWN;
	}else if( (key_cont&PAD_KEY_LEFT) ){
		dir = DIR_LEFT;
	}else if( (key_cont&PAD_KEY_RIGHT) ){
		dir = DIR_RIGHT;
	}
	
	debug_flag = FALSE;
	if( key_cont & PAD_BUTTON_R ){
		debug_flag = TRUE;
	}
	
	set = PLAYER_SET_NON;
	switch( g_jiki->move_state ){
	case PLAYER_MOVE_STOP:
		set = gjiki_CheckMoveStart_Stop(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_WALK:
		set = gjiki_CheckMoveStart_Walk(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_TURN:
		set = gjiki_CheckMoveStart_Turn(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_HITCH:
		set = gjiki_CheckMoveStart_Hitch(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	default:
		GF_ASSERT( 0 );
	}
	
	switch( set ){
	case PLAYER_SET_NON:
		gjiki_SetMove_Non(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_STOP:
		gjiki_SetMove_Stop(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_WALK:
		gjiki_SetMove_Walk(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_TURN:
		gjiki_SetMove_Turn(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_HITCH:
		gjiki_SetMove_Hitch(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
		break;
  case PLAYER_SET_JUMP:
		gjiki_SetMove_Jump(
			g_jiki, fmmdl, key_trg, key_cont, dir, debug_flag );
    break;
	}
}

//======================================================================
//	�ړ��J�n�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Stop(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( FLDMMDL_CheckPossibleAcmd(fmmdl) == TRUE ){
		if( dir != DIR_NOT ){
			u16 old_dir;
			old_dir = FLDMMDL_GetDirDisp( fmmdl );
			
			if( dir != old_dir && debug_flag == FALSE ){
				return( PLAYER_SET_TURN );
			}
			
			return( gjiki_CheckMoveStart_Walk(
				g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
		}
		
		return( PLAYER_SET_STOP );
	}
	
	return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Walk(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( FLDMMDL_CheckPossibleAcmd(fmmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT )
  {
		return( gjiki_CheckMoveStart_Stop(
			g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	{
		u32 hit = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		
		if( debug_flag == TRUE )
    {
			if( hit != FLDMMDL_MOVEHITBIT_NON &&
          !(hit&FLDMMDL_MOVEHITBIT_OUTRANGE) )
      {
				hit = FLDMMDL_MOVEHITBIT_NON;
			}
		}
		
		if( hit == FLDMMDL_MOVEHITBIT_NON )
    {
			return( PLAYER_SET_WALK );
		}
    
    if( (hit & FLDMMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      FLDMMDL_GetVectorPos( fmmdl, &pos );
      FLDMMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = FLDMMDL_GetMapPosAttr( fmmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VAL val = MAPATTR_GetMapAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetMapAttrFlag( attr );
        
        switch( val ) //�W�����v�A�g���r���[�g�`�F�b�N
        {
        case 0x72:
          attr_dir = DIR_RIGHT;
          break;
        case 0x73:
          attr_dir = DIR_LEFT;
          break;
        case 0x74:
          attr_dir = DIR_UP;
          break;
        case 0x75:
          attr_dir = DIR_DOWN;
          break;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //�W�����v������v
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
	}
	
	return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Turn(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( FLDMMDL_CheckPossibleAcmd(fmmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( gjiki_CheckMoveStart_Stop(
			g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( gjiki_CheckMoveStart_Walk(
		g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Hitch(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( FLDMMDL_CheckPossibleAcmd(fmmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( gjiki_CheckMoveStart_Stop(
			g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( gjiki_CheckMoveStart_Walk(
		g_jiki,fmmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//	�ړ��Z�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Non(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Stop(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	if( dir == DIR_NOT ){
		dir = FLDMMDL_GetDirDisp( fmmdl );
	}
	
	code = FLDMMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
	FLDMMDL_SetAcmd( fmmdl, code );
	g_jiki->move_state = PLAYER_MOVE_STOP;
	
  FIELD_PLAYER_SetMoveValue( g_jiki->fld_player, PLAYER_MOVE_VALUE_STOP );

	gjiki_Sound_MoveStop();
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Walk(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	if( debug_flag == TRUE ){
		code = AC_WALK_U_2F;
	}else if( key_cont & PAD_BUTTON_B ){
		code = AC_DASH_U_4F;
	}else{
		code = AC_WALK_U_8F;
	}
	
	if( key_cont & PAD_BUTTON_A ){ //kari
		code = AC_JUMP_U_2G_16F;
	}
	
	code = FLDMMDL_ChangeDirAcmdCode( dir, code );
	
	FLDMMDL_SetAcmd( fmmdl, code );
	g_jiki->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( g_jiki->fld_player, PLAYER_MOVE_VALUE_WALK );
	
	gjiki_Sound_Move();
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Turn(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = FLDMMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
	
	FLDMMDL_SetAcmd( fmmdl, code );
	g_jiki->move_state = PLAYER_MOVE_TURN;
	
  FIELD_PLAYER_SetMoveValue( g_jiki->fld_player, PLAYER_MOVE_VALUE_TURN );
	gjiki_Sound_MoveStop();
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Hitch(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = FLDMMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_32F );
	
	FLDMMDL_SetAcmd( fmmdl, code );
	g_jiki->move_state = PLAYER_MOVE_HITCH;
	
  FIELD_PLAYER_SetMoveValue( g_jiki->fld_player, PLAYER_MOVE_VALUE_STOP );
	gjiki_Sound_MoveStop();
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �W�����v
 * @param	g_jiki FIELD_PLAYER_GRID
 * @param fmmdl FLDMMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Jump(
	FIELD_PLAYER_GRID *g_jiki, FLDMMDL *fmmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	code = FLDMMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
	
	FLDMMDL_SetAcmd( fmmdl, code );
	g_jiki->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_SetMoveValue( g_jiki->fld_player, PLAYER_MOVE_VALUE_WALK );
	gjiki_Sound_Move();
}


//======================================================================
//	�T�E���h
//======================================================================
//--------------------------------------------------------------
/**
 * �T�E���h�e�X�g�@���@��~
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_Sound_MoveStop( void )
{
	//�@�� �T�E���h�e�X�g�i��~�j
	u16 trackBit = 0xfcff; // track 9,10 OFF
	PMSND_ChangeBGMtrack(trackBit);
	//�@��
}

//--------------------------------------------------------------
/**
 * �T�E���h�e�X�g�@���@�ړ�
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_Sound_Move( void )
{
	//�@�� �T�E���h�e�X�g�i�ړ��j
	u16 trackBit = 0xffff; // �Strack ON
	PMSND_ChangeBGMtrack(trackBit);
	//�@��
}
