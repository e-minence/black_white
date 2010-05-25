//=============================================================================
/**
 *
 *	@file		event_debug_demo3d.c
 *	@brief  �f�o�b�O���j���|�F�f���Đ�
 *	@author		hosaka genya �� miyuki iwasawa
 *	@data		2009.12.07
 *
 */
//=============================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_comm.h"

#include "demo/demo3d.h"

#include "fieldmap.h"

#include "message.naix"  // NARC_xxxx
#include "msg/msg_d_field.h"  // for DEBUG_FIELD_STR_xxxx
#include "event_debug_local.h"  // for DEBUG_MENU_INITIALIZER

#include "event_fieldmap_control.h"

#include "event_debug_demo3d.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//======================================================================================
// ���C�x���g���[�N
//======================================================================================
typedef struct _SCENE_TABLE{
  u16 demo_id:5;  
  u16 scene_id:4;  
  u16 bgm_change:1;
  u16 fadeout_type:1;
  u16 fadein_type:1;
}SCENE_TABLE;

typedef struct
{
  HEAPID           heapID;
  GAMESYS_WORK*      gsys;
  GAMEDATA*         gdata;
  GAME_COMM_SYS_PTR gcsp;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  DEMO3D_PARAM   param;

  const SCENE_TABLE*   scene;
  BOOL  net_off_flag;
} EVENT_WORK;


#define FADE_BLACK  (0)
#define FADE_WHITE  (1)

static const SCENE_TABLE DATA_SceneTable[] = {
  { DEMO3D_ID_C_CRUISER, 0, FALSE, FADE_BLACK, FADE_BLACK,   },       //1
  { DEMO3D_ID_C_CRUISER_PORT, 0, FALSE, FADE_BLACK, FADE_BLACK,   },  //2
	{ DEMO3D_ID_G_SHIP_PORT, 0, FALSE, FADE_BLACK, FADE_BLACK,   },     //3
	{ DEMO3D_ID_INTRO_TOWN, 0, TRUE, FADE_WHITE, FADE_WHITE },          //4
	{ DEMO3D_ID_N_CASTLE, 0, TRUE, FADE_BLACK, FADE_BLACK,   },         //5
	{ DEMO3D_ID_N_LEGEND_BORN_B, 0, TRUE, FADE_BLACK, FADE_BLACK,   },  //6
	{ DEMO3D_ID_N_LEGEND_BORN_W, 0, TRUE, FADE_BLACK, FADE_BLACK,   },  //7
	{ DEMO3D_ID_N_LEGEND_COME_B, 0, TRUE, FADE_BLACK, FADE_BLACK,   },  //8
	{ DEMO3D_ID_N_LEGEND_COME_W, 0, TRUE, FADE_BLACK, FADE_BLACK,   },  //9
	{ DEMO3D_ID_N_LEGEND_MEAT_B, 0, FALSE, FADE_BLACK, FADE_BLACK,   }, //10
	{ DEMO3D_ID_N_LEGEND_MEAT_W, 0, FALSE, FADE_BLACK, FADE_BLACK,   }, //11
	{ DEMO3D_ID_RIZA_BRIDGE, 0, FALSE, FADE_BLACK, FADE_BLACK,   },     //12
	{ DEMO3D_ID_V_BOAT_PORT, 0, FALSE, FADE_BLACK, FADE_BLACK,   },     //13
	{ DEMO3D_ID_F_WHEEL, 0, FALSE, FADE_BLACK, FADE_BLACK,   },         //14
	{ DEMO3D_ID_F_WHEEL, 1, FALSE, FADE_BLACK, FADE_BLACK,   },         //15
	{ DEMO3D_ID_F_WHEEL, 2, FALSE, FADE_BLACK, FADE_BLACK,   },         //16
	{ DEMO3D_ID_F_WHEEL, 3, FALSE, FADE_BLACK, FADE_BLACK,   },         //17
	{ DEMO3D_ID_F_WHEEL, 4, FALSE, FADE_BLACK, FADE_BLACK,   },         //18
	{ DEMO3D_ID_F_WHEEL, 5, FALSE, FADE_BLACK, FADE_BLACK,   },         //19
	{ DEMO3D_ID_F_WHEEL, 6, FALSE, FADE_BLACK, FADE_BLACK,   },         //20
	{ DEMO3D_ID_F_WHEEL, 7, FALSE, FADE_BLACK, FADE_BLACK,   },         //21
	{ DEMO3D_ID_F_WHEEL, 8, FALSE, FADE_BLACK, FADE_BLACK,   },         //22
	{ DEMO3D_ID_ENDING_B, 0, FALSE, FADE_BLACK, FADE_BLACK,   },        //23
	{ DEMO3D_ID_ENDING_W, 0, FALSE, FADE_BLACK, FADE_BLACK,   },        //24
	{ DEMO3D_ID_TITLE_B, 0, FALSE, FADE_BLACK, FADE_BLACK,   },        //23
	{ DEMO3D_ID_TITLE_W, 0, FALSE, FADE_BLACK, FADE_BLACK,   },        //24
};

#define DEMO_SCENE_MAX  (NELEMS(DATA_SceneTable))

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk );
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work );

//=============================================================================
/**
 *							�f�[�^�e�[�u��
 */
//=============================================================================

/// �ǂ��ł��W�����v�@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

// �f�o�b�O���j���[������
static const DEBUG_MENU_INITIALIZER DebugDemo3DJumpMenuData = {
  NARC_message_d_field_dat,
  0,
  NULL,
  &DATA_DebugMenuList_ZoneSel, //���p
  1, 1, 16, 17,
  DebugMenuList_Make,
  DebugMenuList_GetMax,
};

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�J�n
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	heapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_Demo3DSelect( GAMESYS_WORK * gsys, void* parent_wk )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, NULL, debugMenuDemo3DSelectEvent, sizeof(EVENT_WORK) );
   // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->gdata    = GAMESYSTEM_GetGameData( gsys );
  work->gcsp     = GAMESYSTEM_GetGameCommSysPtr( gsys );
  work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->heapID   = FIELDMAP_GetHeapID( work->fieldmap );
  work->menuFunc = NULL;

  return event;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g Main����
 *
 *	@param	GMEVENT *event
 *	@param	*seq
 *	@param	*wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK *work = wk;
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr( work->gsys );
 
  switch( (*seq) )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  
                                        &DebugDemo3DJumpMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){ //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }
      work->scene = &DATA_SceneTable[ret]; 
      IWASAWA_Printf("Demo Select = %d\n",ret);

      work->net_off_flag = GAMESYSTEM_GetNetOffEventFlag( work->gsys );
      GAMESYSTEM_SetNetOffEventFlag( work->gsys, TRUE );

      if( GameCommSys_BootCheck( work->gcsp ) == GAME_COMM_NO_NULL ){
        (*seq) += 2;
      }else{
        GameCommSys_ExitReq( work->gcsp );
        (*seq) += 1;
      }
    }
    break;
  case 2:
    if(GameCommSys_BootCheck( work->gcsp) == GAME_COMM_NO_NULL){
      (*seq) += 1;
    }
    break;
  case 3:
    // �t�F�[�h�A�E�g
    if( work->scene->fadeout_type == FADE_BLACK ){
      GMEVENT_CallEvent(event, EVENT_FieldFadeOut_Black( work->gsys, work->fieldmap, FIELD_FADE_WAIT));
    }else{
      GMEVENT_CallEvent(event, EVENT_FieldFadeOut_White( work->gsys, work->fieldmap, FIELD_FADE_WAIT));
    }
    if( work->scene->bgm_change ){
      PMSND_FadeOutBGM(PMSND_FADE_FAST);
    }
		(*seq) ++;
		break;
  case 4:
    if( work->scene->bgm_change ){
      PMSND_PauseBGM(TRUE);
      PMSND_PushBGM();
    }
    DEMO3D_PARAM_SetFromRTC( &work->param, work->gsys, work->scene->demo_id, work->scene->scene_id );
    GMEVENT_CallEvent( event, EVENT_FieldSubProcNoFade( work->gsys, work->fieldmap,
        FS_OVERLAY_ID(demo3d), &Demo3DProcData, &work->param ) );
		(*seq) ++;
		break;
	case 5:
    GAMESYSTEM_SetNetOffEventFlag( work->gsys, work->net_off_flag );
    //�l�b�gOff�t���O�𗎂Ƃ��^�C�~���O���t�B�[���h���A�ɊԂɍ���Ȃ��̂ŋ������A������
    GAMESYSTEM_CommBootAlways(work->gsys);
    if( work->scene->bgm_change ){
      PMSND_PopBGM();
      PMSND_PauseBGM(FALSE);
      PMSND_FadeInBGM(PMSND_FADE_NORMAL);
    }
    if( work->scene->fadein_type == FADE_BLACK ){
      GMEVENT_CallEvent(event, EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap, FIELD_FADE_WAIT));
    }else{
      GMEVENT_CallEvent(event, EVENT_FieldFadeIn_White( work->gsys, work->fieldmap, FIELD_FADE_WAIT));
    }
		(*seq) ++;
    break;
  case 6:
    (*seq) = 0;
    break;
  }
  return ( GMEVENT_RES_CONTINUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������ ���X�g����
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	*list
 *	@param	heapID
 *	@param	msgData
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id;
  STRBUF *strBuf1 = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap  = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG          = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgData    = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
  
  for( id = 0; id < DEMO_SCENE_MAX; id++ ){
    GFL_STR_ClearBuffer( strBuf1 );
    GFL_MSG_GetString( pMsgData, DEBUG_FIELD_DEMO3D_DEMO_01 + id, strBuf1 );  // ��������擾
    FLDMENUFUNC_AddStringListData( list, strBuf1, id, heapID );
  }
  
  GFL_MSG_Delete( pMsgData );
  GFL_HEAP_FreeMemory( strBuf1 );

}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������ ���X�g���ڐ���Ԃ�
 *
 *	@param	GAMESYS_WORK* gsys
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return DEMO_SCENE_MAX;
}

