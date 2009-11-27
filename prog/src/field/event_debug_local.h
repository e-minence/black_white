/*
 *  @file   event_debug_local.h
 *  @brief  �t�B�[���h�f�o�b�O���j���[�@���[�J�����L�w�b�_
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */

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
  GFL_MSGDATA *msgData;
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
  u16 msg_arc_id;     ///<���p���郁�b�Z�[�W�A�[�J�C�u�̎w��
  u16 max;            ///<���ڍő吔
  const FLDMENUFUNC_LIST * menulist;  ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  const FLDMENUFUNC_HEADER * menuH;   ///<���j���[�\���w��f�[�^
  u8 px, py, sx, sy;
  MAKE_LIST_FUNC * makeListFunc;      ///<���j���[�����֐��ւ̃|�C���^�i�Œ胁�j���[�̏ꍇ��NULL)
  GET_MAX_FUNC * getMaxFunc;          ///<���ڍő吔�擾�֐��ւ̃|�C���^�i�Œ胁�j���[�̏ꍇ��NULL)
}DEBUG_MENU_INITIALIZER;

//======================================================================
// extern 
//======================================================================
/*
 *  @brief  �f�o�b�O���j���[������
 */
extern FLDMENUFUNC * DebugMenuInit(
    FIELDMAP_WORK * fieldmap, HEAPID heapID, const DEBUG_MENU_INITIALIZER * init );
FLDMENUFUNC * DebugMenuInitEx(
    FIELDMAP_WORK * fieldmap, HEAPID heapID, const DEBUG_MENU_INITIALIZER * init, void* cb_work );
