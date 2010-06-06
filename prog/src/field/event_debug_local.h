//======================================================================
/*
 *  @file   event_debug_local.h
 *  @brief  �t�B�[���h�f�o�b�O���j���[�@���[�J�����L�w�b�_
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */
//======================================================================

#pragma once

//======================================================================
//  typedef struct
//======================================================================
//--------------------------------------------------------------
/// DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
typedef struct _TAG_DEBUG_MENU_EVENT_WORK DEBUG_MENU_EVENT_WORK;

//--------------------------------------------------------------
/// ���j���[�Ăяo���֐�
/// BOOL TRUE=�C�x���g�p�� FALSE==�f�o�b�O���j���[�C�x���g�I��
//--------------------------------------------------------------
typedef BOOL (* DEBUG_MENU_CALLPROC)(DEBUG_MENU_EVENT_WORK*);

//--------------------------------------------------------------
/// DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_DEBUG_MENU_EVENT_WORK
{
  HEAPID heapID;
  GMEVENT *gmEvent;
  GAMESYS_WORK *gmSys;
  GAMEDATA * gdata;
  FIELDMAP_WORK * fieldWork;
  
  DEBUG_MENU_CALLPROC call_proc;
  FLDMENUFUNC *menuFunc;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef void MAKE_LIST_FUNC( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef u16 GET_MAX_FUNC( GAMESYS_WORK* gsys, void* c_work );

//--------------------------------------------------------------
/**
 * @brief   ���j���[�������\����
 *
 * @note
 * �σ��j���[�̏ꍇ�AmakeListFunc/getMaxFunc����`����K�v������B
 * �g�p���@�ɂ��Ă͂ǂ��ł��W�����v�Ȃǂ��Q�Ƃ̂��ƁB
 */
//--------------------------------------------------------------
typedef struct {
  u16 msg_dat_id;     ///<���p���郁�b�Z�[�W�A�[�J�C�u�̎w��
  u16 max;            ///<���ڍő吔�i�Œ胊�X�g�łȂ��ꍇ�A�O�j
  const FLDMENUFUNC_LIST * menulist;  ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  const FLDMENUFUNC_HEADER * menuH;   ///<���j���[�\���w��f�[�^
  u8 px, py, sx, sy;
  MAKE_LIST_FUNC * makeListFunc;      ///<���j���[�����֐��ւ̃|�C���^�i�Œ胁�j���[�̏ꍇ��NULL)
  GET_MAX_FUNC * getMaxFunc;          ///<���ڍő吔�擾�֐��ւ̃|�C���^�i�Œ胁�j���[�̏ꍇ��NULL)
}DEBUG_MENU_INITIALIZER;

//======================================================================
// extern 
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
extern const FLDMENUFUNC_HEADER DATA_DebugMenuList_Default;
extern const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel;

//--------------------------------------------------------------
/*
 *  @brief  �f�o�b�O���j���[������
 */
//--------------------------------------------------------------
extern FLDMENUFUNC * DEBUGFLDMENU_Init(
    FIELDMAP_WORK * fieldmap, HEAPID heapID, const DEBUG_MENU_INITIALIZER * init );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern FLDMENUFUNC * DEBUGFLDMENU_InitEx(
    FIELDMAP_WORK * fieldmap, HEAPID heapID, const DEBUG_MENU_INITIALIZER * init, void* cb_work );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern GFL_MSGDATA * DEBUGFLDMENU_CreateMSGDATA( FIELDMAP_WORK * fieldmap, u32 arcDatIDMsg );

//======================================================================
//
//    �T�u���j���[�C�x���g
//
//======================================================================
FS_EXTERN_OVERLAY( debug_jump );
//--------------------------------------------------------------
///�T�u���j���[�C�x���g�F�f�o�b�O�X�N���v�g�I��
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_FLDMENU_DebugScript( const DEBUG_MENU_EVENT_WORK * now_wk );

//--------------------------------------------------------------
///�T�u���j���[�C�x���g�F�}�b�v�W�����v�i����Łj
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_FLDMENU_JumpEasy( GAMESYS_WORK * gsys, void * wk );

//--------------------------------------------------------------
///�T�u���j���[�C�x���g�F�G�ߕω�
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_FLDMENU_ChangeSeason( GAMESYS_WORK * gsys, void * wk );

//--------------------------------------------------------------
///�T�u���j���[�C�x���g�F�}�b�v�W�����v�i�S�]�[��ID�Łj
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_FLDMENU_JumpAllZone( GAMESYS_WORK * gsys, void * wk );

