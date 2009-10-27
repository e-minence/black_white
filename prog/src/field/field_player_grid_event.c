//======================================================================
/**
 * @file  field_player_grid_event.c
 * @brief �O���b�h��p �t�B�[���h�v���C���[����@�C�x���g�֘A
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_player.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap_ctrl.h"
#include "fieldmap_ctrl_grid.h"
#include "field_player_grid.h"
#include "map_attr.h"

#include "fldeff_namipoke.h"

#include "field_sound.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static GMEVENT * gjiki_CheckMoveHitEvent(
    FIELD_PLAYER_GRID *gjiki, u16 dir, PLAYER_EVENTBIT evbit );
static GMEVENT * gjiki_CheckEventKairiki(
    FIELD_PLAYER_GRID *gjiki, u16 dir, PLAYER_MOVE_VALUE val );

static GMEVENT * gjiki_SetEventKairiki(
    FIELD_PLAYER_GRID *gjiki, u16 dir, MMDL *mmdl, BOOL ana_flag );
static GMEVENT_RESULT event_Kairki( GMEVENT *event, int *seq, void *wk );

static MMDL * gjiki_SearchDirMMdl( FIELD_PLAYER_GRID *gjiki, u16 dir );

//======================================================================
//  �O���b�h���@�@�C�x���g�ړ��֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�C�x���g�ړ��`�F�b�N
 * @param *gjiki FIELD_PLAYER_GRID 
 * @param key_trg ���̓L�[�g���K���
 * @param key_cont ���̓L�[�R���e�j���[���
 * @param evbit PLAYER_EVENTBIT
 * @retval BOOL TRUE=���@�C�x���g�ړ�����
 */
//--------------------------------------------------------------
GMEVENT * FIELD_PLAYER_GRID_CheckMoveEvent( FIELD_PLAYER *fld_player,
    int key_trg, int key_cont, PLAYER_EVENTBIT evbit )
{
  u16 dir;
  MMDL *mmdl;
  GMEVENT *event;
  FIELDMAP_WORK *fieldmap;
  FIELD_PLAYER_GRID *gjiki;
  
  event = NULL;
  fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
  gjiki = FIELDMAP_GetPlayerGrid( fieldmap );
  mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  dir = FIELD_PLAYER_GRID_GetKeyDir( gjiki, key_cont );
  
#if 0 //�����ړ��C�x���g
 	if( Player_MoveBitCheck_Force(jiki) == TRUE ){
		if( JikiEventCheck_ForceMoveHitEvent(fsys,jiki,flag) == TRUE ){
			return( TRUE );
		}
#endif
  
  //�ړ��J�n�\�`�F�b�N
  if( FIELD_PLAYER_GRID_CheckStartMove(gjiki,dir) == FALSE ){
    return( NULL );
  }
  
#if 0 //�L�[���͖����ł���������C�x���g�`�F�b�N
	if( dir == DIR_NOT ){
		if( JikiEventCheck_KeyOFFEvent(fsys,jiki,dir,flag) == TRUE ){
			return( TRUE );
		}
		
		return( FALSE );
	}
#endif
 
  //�C�x���g�q�b�g�`�F�b�N
  event = gjiki_CheckMoveHitEvent( gjiki, dir, evbit );
  
  if( event != NULL ){
    return( event );
  }
  
  return( NULL );
}

//======================================================================
//  �ړ��q�b�g�n�C�x���g�����`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��q�b�g�n�C�x���g�����`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir  �ړ������BDIR_UP��
 * @param evbit PLAYER_EVENTBIT
 * @retval GMEVENT* NULL=�C�x���g��������
 */
//--------------------------------------------------------------
static GMEVENT * gjiki_CheckMoveHitEvent(
    FIELD_PLAYER_GRID *gjiki, u16 dir, PLAYER_EVENTBIT evbit )
{
  GMEVENT *event = NULL;
  PLAYER_MOVE_VALUE val;
  
  val = FIELD_PLAYER_GRID_GetMoveValue( gjiki, dir );
  
#ifdef DEBUG_ONLY_FOR_kagaya
  evbit |= PLAYER_EVENTBIT_KAIRIKI;
#endif
  
  if( (evbit & PLAYER_EVENTBIT_KAIRIKI) ){
    event = gjiki_CheckEventKairiki( gjiki, dir, val );
  }
  
  return( event );
}

//--------------------------------------------------------------
/**
 * ���͈ړ��`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ������BDIR_UP��
 * @param val PLAYER_MOVE_VALUE
 * @retval GMEVENT* NULL=�ړ�����
 */
//--------------------------------------------------------------
static GMEVENT * gjiki_CheckEventKairiki(
    FIELD_PLAYER_GRID *gjiki, u16 dir, PLAYER_MOVE_VALUE val )
{
  if( val == PLAYER_MOVE_VALUE_WALK )
  {
    MMDL *mmdl = gjiki_SearchDirMMdl( gjiki, dir );
    
    if( mmdl != NULL )
    {
      u16 code = MMDL_GetOBJCode( mmdl );
      
      if( code == ROCK )
      {
        u32 ret;
        BOOL flag;
        KAGAYA_Printf( "���͊�q�b�g\n" );
        
        ret = MMDL_HitCheckMoveDir( mmdl, dir );
        ret &= ~MMDL_MOVEHITBIT_LIM; //�ړ������͖���
        
        flag = FALSE;
        
        if( (ret & MMDL_MOVEHITBIT_ATTR) )
        {
          VecFx32 pos;
          MAPATTR attr;
          MAPATTR_VALUE attr_val;
          FIELDMAP_WORK *fieldmap;
          FIELD_PLAYER *fld_player;
          FLDMAPPER *mapper;
          
          fld_player = FIELD_PLAYER_GRID_GetFieldPlayer( gjiki );
          fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
          mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
          
          MMDL_GetVectorPos( mmdl, &pos );
          MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
          
          attr = MAPATTR_GetAttribute( mapper, &pos );
          attr_val = MAPATTR_GetAttrValue( attr );
          
          if( MAPATTR_VALUE_CheckKairikiAna(attr_val) == TRUE )
          {
            ret = MMDL_MOVEHITBIT_NON;
            flag = TRUE;
          }
        }
        
        if( ret == MMDL_MOVEHITBIT_NON )
        {
          KAGAYA_Printf( "���͊J�n\n" );
          return( gjiki_SetEventKairiki(gjiki,dir,mmdl,flag) );
        }
      }
    }
  }
  
  return( NULL );
}

//======================================================================
//  ���̓C�x���g
//======================================================================
#define KAIRIKI_FALLDOWN_HEIGHT (NUM_FX32(-14))

//--------------------------------------------------------------
/// EVENT_KAIRIKI_WORK
//--------------------------------------------------------------
typedef struct
{
  FIELD_PLAYER_GRID *gjiki;
  u16 dir;
  u16 ana_flag;
  s16 frame;
  fx32 bottom;
  VecFx32 offs;
  MMDL *mmdl;
  MMDL *mmdl_jiki;
}EVENT_KAIRIKI_WORK;

//--------------------------------------------------------------
/**
 * ���̓C�x���g����
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ�����
 * @param mmdl �ړ��Ώۂ̓��샂�f��
 * @param ana_flag �ړ��悪���͌��ł���B
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
static GMEVENT * gjiki_SetEventKairiki(
    FIELD_PLAYER_GRID *gjiki, u16 dir, MMDL *mmdl, BOOL ana_flag )
{
  GMEVENT *event;
  GAMESYS_WORK *gsys;
  FIELD_PLAYER *fld_player;
  EVENT_KAIRIKI_WORK *work;
  
  fld_player = FIELD_PLAYER_GRID_GetFieldPlayer( gjiki );
  gsys = FIELD_PLAYER_GetGameSysWork( fld_player );
  
  event = GMEVENT_Create( gsys, NULL,
      event_Kairki, sizeof(EVENT_KAIRIKI_WORK) );
  
  work = GMEVENT_GetEventWork( event );
  MI_CpuClear8( work, sizeof(EVENT_KAIRIKI_WORK) );
  
  work->gjiki = gjiki;
  work->dir = dir;
  work->ana_flag = ana_flag;
  work->mmdl = mmdl;
  work->mmdl_jiki = FIELD_PLAYER_GetMMdl( fld_player );
  
  return( event );
}

//--------------------------------------------------------------
/**
 * ���̓C�x���g
 * @param event GMEVNET
 * @param seq event seq
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_Kairki( GMEVENT *event, int *seq, void *wk )
{
  EVENT_KAIRIKI_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    if( MMDL_CheckPossibleAcmd(work->mmdl) == FALSE ){
      break;
    }
    
    if( MMDL_CheckPossibleAcmd(work->mmdl_jiki) == FALSE ){
      if( FIELD_PLAYER_GRID_SetMoveStop(work->gjiki) == FALSE ){
        break;
      }
    }
    
    {
      u16 code = MMDL_ChangeDirAcmdCode( work->dir, AC_WALK_U_16F );
      MMDL_SetAcmd( work->mmdl, code );

      code = MMDL_ChangeDirAcmdCode( work->dir, AC_STAY_WALK_U_8F );
      MMDL_SetAcmd( work->mmdl_jiki, code );
      
      PMSND_PlaySE( SEQ_SE_FLD_03 );
    }
    
    (*seq)++;
    break;
  case 1:
    if( MMDL_CheckEndAcmd(work->mmdl) == FALSE ){
      break;
    }
    
    if( MMDL_CheckEndAcmd(work->mmdl_jiki) == FALSE ){
      break;
    }
    
    if( work->ana_flag == FALSE ){
      (*seq) = 4;
    }else{
      VecFx32 pos;
      
      MMDL_GetVectorPos( work->mmdl, &pos );
      MMDL_GetMapPosHeight( work->mmdl, &pos, &work->bottom );
      work->bottom += KAIRIKI_FALLDOWN_HEIGHT;
      
      MMDL_SetStatusBitHeightGetOFF( work->mmdl, TRUE );
      MMDL_SetStatusBitFellowHit( work->mmdl, FALSE );

      (*seq)++;
    }

    PMSND_PlaySE( SEQ_SE_FLD_04 );
    break;
  case 2: //���͌��͂܂�
    {
      VecFx32 pos;
      MMDL_GetVectorPos( work->mmdl, &pos );
  
      if( work->bottom < pos.y ){
        pos.y -= 0x4000;
        if( work->bottom > pos.y ){
          pos.y = work->bottom;
        }
      }else if( work->bottom > pos.y ){
        pos.y += 0x4000;
        if( work->bottom < pos.y ){
          pos.y = work->bottom;
        }
      }
     
  	  MMDL_SetVectorPos( work->mmdl, &pos );
     
      if( work->bottom == pos.y ){
        work->offs.y = NUM_FX32( 2 );
        (*seq)++;
      }
    }
    break;
  case 3: //���͌��͂܂� ���n��̐U��
    MMDL_SetVectorOuterDrawOffsetPos( work->mmdl, &work->offs );
    work->offs.y = -work->offs.y;
    work->frame++;
    
    if( (work->frame&0x04) ){
      if( work->offs.y < 0 ){
        work->offs.y += NUM_FX32( 1 );
      }else if( work->offs.y > 0 ){
        work->offs.y -= NUM_FX32( 1 );
      }
    }
    
    if( work->offs.y == 0 ){
      VecFx32 pos;
      MMDL_GetVectorPos( work->mmdl, &pos );
      MMDL_InitPosition( work->mmdl, &pos, work->dir );
      MMDL_SetVectorOuterDrawOffsetPos( work->mmdl, &work->offs );
      MMDL_ROCKPOS_SavePos( work->mmdl );
      (*seq)++;
    }
    break;
  case 4:
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �g���C�x���g
//======================================================================
typedef struct
{
  u16 seq;
  s16 wait;
  u16 end_flag;
  u16 dir;
  u16 kishi_flag;
  FIELDMAP_WORK *fieldmap;
  FIELD_PLAYER *fld_player;
  FIELD_PLAYER_GRID *gjiki;
}EVWORK_NAMINORI;

static void evtcb_NaminoriStart( GFL_TCB *tcb, void *wk );

//--------------------------------------------------------------
/**
 * �g���J�n�C�x���g�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param dir �ړ����� DIR_UP��
 * @param attr �g�����s��MAPATTR
 * @param heapID �C�x���g���s�pHEAPID
 * @retval GFL_TCB*
 */
//--------------------------------------------------------------
GFL_TCB * FIELD_PLAYER_GRID_SetEventNaminoriStart(
    FIELD_PLAYER *fld_player, u16 dir, MAPATTR attr, HEAPID heapID )
{
  EVWORK_NAMINORI *work;
  
  work = GFL_HEAP_AllocClearMemoryLo( heapID, sizeof(EVWORK_NAMINORI) );
  
  work->fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
  work->dir = dir;
  work->fld_player = fld_player;
  
  {
    work->gjiki = FIELDMAP_GetPlayerGrid( work->fieldmap );
  }
  
  {
    MAPATTR_VALUE val;
    val = MAPATTR_GetAttrValue( attr );
    
    if( MAPATTR_VALUE_CheckShore(val) == TRUE ){
      work->kishi_flag = TRUE;
    }
  }
  
  {
    GFL_TCBSYS *tcbsys = FIELDMAP_GetFieldmapTCBSys( work->fieldmap );
    GFL_TCB *tcb = GFL_TCB_AddTask( tcbsys, evtcb_NaminoriStart, work, 0 );
    return( tcb );
  }
}

//--------------------------------------------------------------
/**
 * �g���J�n�C�x���g�@�`�F�b�N
 * @param tcb FIELD_PLAYER_GRID_SetEventNaminoriStart()�߂�l
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckEventNaminoriStart( GFL_TCB *tcb )
{
  EVWORK_NAMINORI *work;
  work = GFL_TCB_GetWork( tcb );
  return( work->end_flag );
}

//--------------------------------------------------------------
/**
 * �g���J�n�C�x���g�@�폜
 * @param tcb FIELD_PLAYER_GRID_SetEventNaminoriStart()�߂�l
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_DeleteEventNaminoriStart( GFL_TCB *tcb )
{
  EVWORK_NAMINORI *work;
  work = GFL_TCB_GetWork( tcb );
  GFL_HEAP_FreeMemory( work );
  GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------
/**
 * �g���J�n�C�x���g�^�X�N
 * @param tcb GFL_TCB*
 * @param wk tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void evtcb_NaminoriStart( GFL_TCB *tcb, void *wk )
{
  FLDEFF_TASK *task;
  EVWORK_NAMINORI *work = wk;
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( work->fld_player );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( work->fld_player );
  
  switch( work->seq )
  {
  case 0: //�g���|�P�����o��
    {
      u16 dir;
      s16 gx,gz;
      fx32 height;
      VecFx32 pos;
      dir = MMDL_GetDirDisp( mmdl );
      gx = MMDL_GetGridPosX( mmdl );
      gz = MMDL_GetGridPosZ( mmdl );
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
      
      {
        fx32 range = GRID_FX32; //�P�}�X��
        
        if( work->kishi_flag == TRUE ){
          range <<= 1; //�Q�}�X��
        }
      
        MMDL_TOOL_AddDirVector( dir, &pos, range );
      }
      
      height = 0;
      MMDL_GetMapPosHeight( mmdl, &pos, &height );
      pos.y = height;
     
      {
        FLDEFF_CTRL *fectrl;
        fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
        task = FLDEFF_NAMIPOKE_SetMMdl( fectrl, dir, &pos, mmdl, FALSE );
        FIELD_PLAYER_GRID_SetEffectTaskWork( work->gjiki, task );
      }
    }
    
    work->seq++;
    break;
  case 1:
    work->wait++;
    if( work->wait > 15 ){
      work->seq++;
    }
    break;
  case 2:
    {
      u16 dir = MMDL_GetDirDisp( mmdl );
      u16 ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_1G_8F );
      
      if( work->kishi_flag == TRUE ){
        ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
      }
      
      MMDL_SetAcmd( mmdl, ac );
    }
    work->seq++;
    break;
  case 3:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      MMDL_EndAcmd( mmdl );
      task = FIELD_PLAYER_GRID_GetEffectTaskWork( work->gjiki );
      FLDEFF_NAMIPOKE_SetJointFlag( task, TRUE );
      FIELD_PLAYER_SetNaminori( work->gjiki );
      
      work->end_flag = TRUE;
      work->seq++;
    }
    break;
  case 4:
    break;
  }
}

//======================================================================
//  ��o��C�x���g
//======================================================================
#define TAKI_MOVE_FRAME (64)
#define TAKI_MOVE_FRAME_Z_START (32)

//--------------------------------------------------------------
/// EVENT_TAKINOBORI_WORK
//--------------------------------------------------------------
typedef struct
{
  s16 seq_no;
  u16 dir;
  u16 end_flag;
  u16 frame;
  VecFx32 target_pos;
  VecFx32 add_val;
  FIELD_PLAYER *fld_player;
  FIELDMAP_WORK *fieldmap;
  FIELD_PLAYER_GRID *gjiki;
}TAKINOBORI_WORK;

static void evtcb_Takinobori( GFL_TCB *tcb, void *wk );

//--------------------------------------------------------------
/**
 * ��o��C�x���g�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param dir �ړ����� DIR_UP=��o�� DIR_DOWN=�ꉺ��
 * @param no �Z���g�p����莝���|�P�����ʒu�ԍ�
 * @param heapID �C�x���g���s�pHEAPID
 * @retval
 */
//--------------------------------------------------------------
GFL_TCB * FIELD_PLAYER_GRID_SetEventTakinobori(
    FIELD_PLAYER *fld_player, u16 dir, u16 no, HEAPID heapID )
{
  TAKINOBORI_WORK *work;
  
  work = GFL_HEAP_AllocClearMemoryLo( heapID, sizeof(TAKINOBORI_WORK) );
  
  work->dir = dir;
  work->fld_player = fld_player;
  work->fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
  
  {
    work->gjiki = FIELDMAP_GetPlayerGrid( work->fieldmap );
  }
  
  {
    GFL_TCBSYS *tcbsys = FIELDMAP_GetFieldmapTCBSys( work->fieldmap );
    GFL_TCB *tcb = GFL_TCB_AddTask( tcbsys, evtcb_Takinobori, work, 0 );
    return( tcb );
  }
}

//--------------------------------------------------------------
/**
 * ��o��C�x���g�@�`�F�b�N
 * @param tcb FIELD_PLAYER_GRID_SetEventTakinobori()�߂�l
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckEventTakinobori( GFL_TCB *tcb )
{
  TAKINOBORI_WORK *work;
  work = GFL_TCB_GetWork( tcb );
  return( work->end_flag );
}

//--------------------------------------------------------------
/**
 * ��o��C�x���g�@�폜
 * @param tcb FIELD_PLAYER_GRID_SetEventTakinobori()�߂�l
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_DeleteEventTakinobori( GFL_TCB *tcb )
{
  TAKINOBORI_WORK *work;
  work = GFL_TCB_GetWork( tcb );
  GFL_HEAP_FreeMemory( work );
  GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------
/**
 * ��o��C�x���g
 * @param tcb GFL_TCB*
 * @param wk tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void evtcb_Takinobori( GFL_TCB *tcb, void *wk )
{
  VecFx32 pos;
  TAKINOBORI_WORK *work = wk;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( work->fld_player );

  switch( work->seq_no ){
  case 0:
    {
      VecFx32 n_pos;
      
      MMDL_GetVectorPos( mmdl, &n_pos );
      pos = n_pos;
      MMDL_TOOL_AddDirVector( work->dir, &pos, GRID_FX32*2 );
      MMDL_GetMapPosHeight( mmdl, &pos, &work->target_pos.y );
      work->target_pos.x = pos.x;
      work->target_pos.z = pos.z;
      
      work->add_val.x = 0;
      work->add_val.y = work->target_pos.y - n_pos.y;
      work->add_val.z = work->target_pos.z - n_pos.z;
      work->add_val.y /= TAKI_MOVE_FRAME;
      work->add_val.z /= TAKI_MOVE_FRAME;
      
      MMDL_SetStatusBitHeightGetOFF( mmdl, TRUE );
      work->seq_no++;
    }
  case 1: //���A����J�n
    MMDL_GetVectorPos( mmdl, &pos );
    
    if( work->dir == DIR_UP ){
      pos.y += work->add_val.y;
      
      if( pos.y > work->target_pos.y ){
        pos.y = work->target_pos.y;
      }
    }else{
      pos.z += work->add_val.z;

      if( pos.z > work->target_pos.z ){
        pos.z = work->target_pos.z;
      }
    }
    
    MMDL_SetVectorPos( mmdl, &pos );
    
    work->frame++;
    if( work->frame >= TAKI_MOVE_FRAME_Z_START ){
      work->frame = 0;
      work->seq_no++;
    }
    break;
  case 2: //�ړ�
    MMDL_GetVectorPos( mmdl, &pos );
    pos.y += work->add_val.y;
    pos.z += work->add_val.z;

    if( work->dir == DIR_UP ){
      if( pos.y > work->target_pos.y ){
        pos.y = work->target_pos.y;
      }
      if( pos.z < work->target_pos.z ){
        pos.z = work->target_pos.z;
      }
    }else{
      if( pos.y < work->target_pos.y ){
        pos.y = work->target_pos.y;
      }
      if( pos.z > work->target_pos.z ){
        pos.z = work->target_pos.z;
      }
    }
    
    MMDL_SetVectorPos( mmdl, &pos );
    
    work->frame++;
    if( work->frame >= TAKI_MOVE_FRAME ){
      MMDL_InitPosition( mmdl, &work->target_pos, work->dir );
      MMDL_SetStatusBitHeightGetOFF( mmdl, FALSE );
      work->end_flag = TRUE;
      work->seq_no++;
    }
    break;
  }
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ʒu����w������ɓ��샂�f�����݂邩
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �Q�ƕ����BDIR_UP��
 * @retval MMDL* NULL=���Ȃ�
 */
//--------------------------------------------------------------
static MMDL * gjiki_SearchDirMMdl( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MMDL *hmmdl,*mmdl = FIELD_PLAYER_GRID_GetMMdl( gjiki );
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  int gx = MMDL_GetGridPosX( mmdl ) + MMDL_TOOL_GetDirAddValueGridX( dir );
  int gz = MMDL_GetGridPosZ( mmdl ) + MMDL_TOOL_GetDirAddValueGridZ( dir );
  fx32 y = MMDL_GetVectorPosY( mmdl );
  
  hmmdl = MMDLSYS_SearchGridPosEx( mmdlsys, gx, gz, y, 1, FALSE );
  return( hmmdl );
}
