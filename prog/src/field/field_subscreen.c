//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   field_subscreen.c
 *  @brief
 *  @author  
 *  @date   2009.03.26
 *
 */
//=============================================================================

#include <gflib.h>
#include "system/main.h"
#include "field_subscreen.h"

#include "system/wipe.h"
#include "infowin/infowin.h"
#include "c_gear/c_gear.h"
#include "c_gear/no_gear.h"
#include "field_menu.h"
#include "dowsing/dowsing.h"

#include "sound/snd_viewer.h"
#include "sound/pm_sndsys.h"

#include "test/camera_adjust_view.h"
#include "net_app/union/union_subdisp.h"
#include "field/intrude_subdisp.h"
#include "field/beacon_view.h"
#include "net_app/union/union_main.h"

//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------

#define FLD_SUBSCR_FADE_DIV (1)
#define FLD_SUBSCR_FADE_SYNC (1)

typedef enum
{
  FSS_UPDATE,
  //���[�h��؂�ւ���Ƃ��̃t�F�[�h����
  FSS_CHANGE_FADEOUT,
  FSS_CHANGE_FADEOUT_WAIT,
  FSS_CHANGE_INIT_FUNC,
  FSS_CHANGE_FADEIN,
  FSS_CHANGE_FADEIN_WAIT,
}FIELD_SUBSCREEN_STATE;


//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------

struct _FIELD_SUBSCREEN_WORK {
  FIELD_SUBSCREEN_MODE mode;
  FIELD_SUBSCREEN_MODE nextMode;
  FIELD_SUBSCREEN_STATE state;
  HEAPID heapID;
  FIELD_SUBSCREEN_ACTION action;
  FIELDMAP_WORK * fieldmap;
  union { 
    FIELD_MENU_WORK *fieldMenuWork;
    C_GEAR_WORK* cgearWork;
    NO_GEAR_WORK* nogearWork;
    UNION_SUBDISP_PTR unisubWork;
    INTRUDE_SUBDISP_PTR intsubWork;
    BEACON_VIEW_PTR beaconViewWork;
    DOWSING_WORK* dowsingWork;
    GFL_CAMADJUST * gflCamAdjust;
    GFL_SNDVIEWER * gflSndViewer;
    void * checker;
  };
  u16 angle_h;
  u16 angle_v;
  fx32 len;
};

typedef void INIT_FUNC(FIELD_SUBSCREEN_WORK * , FIELD_SUBSCREEN_MODE prevMode );
typedef void UPDATE_FUNC(FIELD_SUBSCREEN_WORK *, BOOL bActive);
typedef void DRAW_FUNC(FIELD_SUBSCREEN_WORK *, BOOL bActive);
typedef GMEVENT* EVENT_CHECK_FUNC(FIELD_SUBSCREEN_WORK *, BOOL bEvReqOK );
typedef void EXIT_FUNC(FIELD_SUBSCREEN_WORK *);
typedef void ACTION_CALLBACK(FIELD_SUBSCREEN_WORK *,FIELD_SUBSCREEN_ACTION actionno);

typedef struct
{ 
  FIELD_SUBSCREEN_MODE mode;    //�G���[���o�p
  INIT_FUNC * init_func;
  UPDATE_FUNC * update_func;
  DRAW_FUNC * draw_func;
  EVENT_CHECK_FUNC * evcheck_func;
  EXIT_FUNC * exit_func;
  ACTION_CALLBACK * action_callback; ///< �A�N�V�����ɂ��؂�ւ����s���鎖���T�u�X�N���[���ɓ`����֐�
  
}FIELD_SUBSCREEN_FUNC_TABLE;

//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void init_normal_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void actioncallback_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_ACTION actionno );

static void init_topmenu_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void draw_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork );

static void init_union_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void exit_union_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void update_union_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );
static void draw_union_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );

static void init_intrude_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void exit_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void update_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );
static void draw_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );

static void init_beacon_view_subscreen(FIELD_SUBSCREEN_WORK * pWork,FIELD_SUBSCREEN_MODE prevMode);
static void exit_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void update_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );
static void draw_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive );
static GMEVENT* evcheck_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bEvReqOK );

static void init_nogear_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void actioncallback_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_ACTION actionno );

static void init_light_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_light_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_light_subscreen( FIELD_SUBSCREEN_WORK* pWork );

static void init_touchcamera_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_touchcamera_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_touchcamera_subscreen( FIELD_SUBSCREEN_WORK* pWork );

static void init_soundviewer_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode );
static void update_soundviewer_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void exit_soundviewer_subscreen( FIELD_SUBSCREEN_WORK* pWork );

static void init_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE prevMode );
static void exit_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork );
static void update_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );
static void draw_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive );

static FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_CGearCheck(FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static const FIELD_SUBSCREEN_FUNC_TABLE funcTable[] =
{ 
  { // C�M�A
    FIELD_SUBSCREEN_NORMAL,       
    init_normal_subscreen,
    update_normal_subscreen,
    NULL ,
    NULL ,
    exit_normal_subscreen,
    actioncallback_normal_subscreen,
  },
  { // �t�B�[���h���j���[
    FIELD_SUBSCREEN_TOPMENU,      
    init_topmenu_subscreen,
    update_topmenu_subscreen,
    draw_topmenu_subscreen ,
    NULL ,
    exit_topmenu_subscreen,
    NULL ,
  },
  { // ���j�I�������
    FIELD_SUBSCREEN_UNION,        
    init_union_subscreen,
    update_union_subscreen,
    draw_union_subscreen ,
    NULL ,
    exit_union_subscreen,
    NULL ,
  },
  { // �N��
    FIELD_SUBSCREEN_INTRUDE,      
    init_intrude_subscreen,
    update_intrude_subscreen,
    draw_intrude_subscreen ,
    NULL ,
    exit_intrude_subscreen,
    NULL ,
  },
  { // ����Ⴂ���ʉ��
    FIELD_SUBSCREEN_BEACON_VIEW,  
    init_beacon_view_subscreen,
    update_beacon_view_subscreen,
    draw_beacon_view_subscreen ,
    evcheck_beacon_view_subscreen,
    exit_beacon_view_subscreen,
    NULL ,
  },
  { // C�M�A�̖��擾��
    FIELD_SUBSCREEN_NOGEAR,       
    init_nogear_subscreen,
    update_nogear_subscreen,
    NULL ,
    NULL ,
    exit_nogear_subscreen,
    actioncallback_nogear_subscreen,
  },
  { // �_�E�W���O
    FIELD_SUBSCREEN_DOWSING,
    init_dowsing_subscreen,
    update_dowsing_subscreen,
    draw_dowsing_subscreen,
    NULL,
    exit_dowsing_subscreen,
    NULL,
  },
  { // �f�o�b�O���C�g����p�l��
    FIELD_SUBSCREEN_DEBUG_LIGHT,  
    init_light_subscreen,
    update_light_subscreen,
    NULL ,
    NULL ,
    exit_light_subscreen,
    NULL ,
  },
  { // �f�o�b�O�J����
    FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,  
    init_touchcamera_subscreen,
    update_touchcamera_subscreen,
    NULL ,
    NULL ,
    exit_touchcamera_subscreen,
    NULL ,
  },
  { // �f�o�b�O�T�E���h
    FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,  
    init_soundviewer_subscreen,
    update_soundviewer_subscreen,
    NULL ,
    NULL ,
    exit_soundviewer_subscreen,
    NULL ,
  }
};

//----------------------------------------------------------------------------
/**
 *  @brief  ����ʂ̏�����
 *  
 *  @param  heapID  �q�[�v�h�c
 *  @param  mode    ���샂�[�h�w��
 */
//-----------------------------------------------------------------------------
FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID,
    FIELDMAP_WORK * fieldmap, FIELD_SUBSCREEN_MODE mode )
{
  FIELD_SUBSCREEN_WORK* pWork = GFL_HEAP_AllocClearMemory(heapID, sizeof(FIELD_SUBSCREEN_WORK));
  GF_ASSERT(mode < FIELD_SUBSCREEN_MODE_MAX);
  GF_ASSERT(funcTable[mode].mode == mode);
  pWork->state = FSS_UPDATE;
  pWork->heapID = heapID;
  pWork->checker = NULL;
  pWork->fieldmap = fieldmap;
  pWork->mode = FIELD_SUBSCREEN_CGearCheck(pWork,mode);
  pWork->nextMode = pWork->mode;
  
  funcTable[pWork->mode].init_func(pWork,FIELD_SUBSCREEN_FIRST_CALL);
  
  return pWork;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����ʂ̔j������
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @return mode
 */
//-----------------------------------------------------------------------------
u8 FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork )
{
  u8 mode = pWork->mode;
  GF_ASSERT(funcTable[pWork->mode].mode == pWork->mode);
  funcTable[pWork->mode].exit_func(pWork);

#if PM_DEBUG
  {
    switch(mode){
    case FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER:
    case FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA:
    case FIELD_SUBSCREEN_DEBUG_LIGHT:
      mode = FIELD_SUBSCREEN_NORMAL;
      break;
    }
  }
#endif
  
  GFL_HEAP_FreeMemory(pWork);
  return mode;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����ʂ̍X�V����
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork )
{
  switch( pWork->state )
  {
  case FSS_UPDATE:
    funcTable[pWork->mode].update_func(pWork,(NULL==GAMESYSTEM_GetEvent(FIELDMAP_GetGameSysWork(pWork->fieldmap))));
    break;

  //���[�h��؂�ւ���Ƃ��̃t�F�[�h����
  case FSS_CHANGE_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );
    pWork->state = FSS_CHANGE_FADEOUT_WAIT;
    break;
#if 0    
  case FSS_CHANGE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      funcTable[pWork->mode].exit_func(pWork);
      pWork->state = FSS_CHANGE_INIT_FUNC;
    }
    break;

  case FSS_CHANGE_INIT_FUNC:
    funcTable[pWork->nextMode].init_func(pWork,pWork->mode);
    pWork->mode = pWork->nextMode;
    pWork->state = FSS_CHANGE_FADEIN;
    break;

  case FSS_CHANGE_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );
    pWork->state = FSS_CHANGE_FADEIN_WAIT;
    break;
#endif
  //��������
  case FSS_CHANGE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      funcTable[pWork->mode].exit_func(pWork);
      funcTable[pWork->nextMode].init_func(pWork,pWork->mode);
      pWork->mode = pWork->nextMode;
//      WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
//                      WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );
      pWork->state = FSS_CHANGE_FADEIN;
    }
    break;

  case FSS_CHANGE_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );
    pWork->state = FSS_CHANGE_FADEIN_WAIT;
    break;

  case FSS_CHANGE_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      pWork->state = FSS_UPDATE;
    }
    funcTable[pWork->mode].update_func(pWork,(NULL==GAMESYSTEM_GetEvent(FIELDMAP_GetGameSysWork(pWork->fieldmap))));
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����ʂ̕`�揈��
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_SUBSCREEN_Draw( FIELD_SUBSCREEN_WORK* pWork )
{
  switch( pWork->state )
  {
  case FSS_UPDATE:
    if( funcTable[pWork->mode].draw_func != NULL )
    {
      funcTable[pWork->mode].draw_func(pWork, (NULL==GAMESYSTEM_GetEvent(FIELDMAP_GetGameSysWork(pWork->fieldmap))));
    }
    break;

  //���[�h��؂�ւ���Ƃ��̃t�F�[�h����
  case FSS_CHANGE_FADEOUT:
    break;
    
  case FSS_CHANGE_FADEOUT_WAIT:
    break;

  case FSS_CHANGE_INIT_FUNC:
    break;

  case FSS_CHANGE_FADEIN:
    break;

  case FSS_CHANGE_FADEIN_WAIT:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����ʂ̃C�x���g�N���`�F�b�N����
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT* FIELD_SUBSCREEN_EventCheck( FIELD_SUBSCREEN_WORK* pWork, BOOL bEvReqOK )
{
  switch( pWork->state )
  {
  case FSS_UPDATE:
    if( funcTable[pWork->mode].evcheck_func != NULL )
    {
      return funcTable[pWork->mode].evcheck_func( pWork, bEvReqOK );
    }
    break;
  default:
    break;
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 * @brief         �؂�ւ��\���H(�J�ڒ���NG)
 * @param pWork
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @param mode
 */
//----------------------------------------------------------------------------
const BOOL FIELD_SUBSCREEN_CanChange( FIELD_SUBSCREEN_WORK* pWork )
{
  if( pWork->state == FSS_UPDATE )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 * @brief         CGEAR�̃t���O�����ĉ����������ǂ����؂�ւ��܂�
 * @param pWork     FIELD_SUBSCREEN_WORK
 * @param new_mode    �؂�ւ��郂�[�h
 * @return  mode    �؂�ւ�郂�[�h
 */
//----------------------------------------------------------------------------

static FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_CGearCheck(FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode)
{
  if(FIELD_SUBSCREEN_NORMAL==new_mode){
    GAMEDATA* pGame = GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(pWork->fieldmap));
    if(!CGEAR_SV_GetCGearONOFF(CGEAR_SV_GetCGearSaveData(GAMEDATA_GetSaveControlWork(pGame)))){
      return FIELD_SUBSCREEN_NOGEAR;
    }
  }
  return new_mode;
}


//----------------------------------------------------------------------------
/**
 * @brief         FIELD_SUBSCREEN_ChangeForce�ƈႢ�t�F�[�h���Ă���؂�ւ��܂�
 * @param pWork
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @param mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode)
{ 
  GF_ASSERT(new_mode < FIELD_SUBSCREEN_MODE_MAX);
  GF_ASSERT(funcTable[new_mode].mode == new_mode);

  pWork->nextMode = FIELD_SUBSCREEN_CGearCheck(pWork,new_mode);
  pWork->state = FSS_CHANGE_FADEOUT;

}

//----------------------------------------------------------------------------
/**
 * @brief         FIELD_SUBSCREEN_Change�ƈႢ�����ɐ؂�ւ��܂�
 * @param pWork
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @param mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_ChangeForce( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode)
{
  GF_ASSERT(new_mode < FIELD_SUBSCREEN_MODE_MAX);
  GF_ASSERT(funcTable[new_mode].mode == new_mode);
  new_mode=FIELD_SUBSCREEN_CGearCheck(pWork,new_mode);
  funcTable[pWork->mode].exit_func(pWork);
  funcTable[new_mode].init_func(pWork,pWork->mode);
  pWork->mode = new_mode;
  pWork->state = FSS_UPDATE;
}

//----------------------------------------------------------------------------
/**
 * @brief         FIELD_SUBSCREEN�������� �ʂ̃T�u�X�N���[���ɐ؂�ւ��܂�
 * @param pWork
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @param mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_ChangeFromWithin( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode, BOOL bFade)
{
  GF_ASSERT(new_mode < FIELD_SUBSCREEN_MODE_MAX);
  GF_ASSERT(funcTable[new_mode].mode == new_mode);
  new_mode=FIELD_SUBSCREEN_CGearCheck(pWork,new_mode);
  pWork->nextMode = new_mode;
  if(bFade){
    pWork->state = FSS_CHANGE_FADEOUT;
  }
  else{
    pWork->state = FSS_CHANGE_FADEOUT_WAIT;
  }
}


//----------------------------------------------------------------------------
/**
 * @brief
 * @param pWork   �T�u�X�N���[�����䃏�[�N�ւ̃|�C���^
 * @return  FIELD_SUBSCREEN_MODE  ���݂̉���ʃ��[�h
 */
//----------------------------------------------------------------------------
FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork)
{ 
  return pWork->mode;
}

//----------------------------------------------------------------------------
/**
 * @brief  �A�N�V������Ԃ��擾����
 * @param  mode
 */
//----------------------------------------------------------------------------
FIELD_SUBSCREEN_ACTION FIELD_SUBSCREEN_GetAction( FIELD_SUBSCREEN_WORK* pWork)
{
  if(pWork){
    return pWork->action;
  }
  return FIELD_SUBSCREEN_ACTION_NONE;
}

//----------------------------------------------------------------------------
/**
 * @brief  �A�N�V������Ԃ�ݒ肷��
 * @param  mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_SetAction( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno)
{
  if(pWork){
    pWork->action = actionno;
  }
}

//----------------------------------------------------------------------------
/**
 * @brief  �A�N�V������Ԃ�������
 * @param  mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_GrantPermission( FIELD_SUBSCREEN_WORK* pWork )
{
  if(pWork){
    if(funcTable[pWork->mode].action_callback){
      funcTable[pWork->mode].action_callback(pWork, pWork->action);
    }
  }
  FIELD_SUBSCREEN_SetAction(pWork, FIELD_SUBSCREEN_ACTION_NONE);
}

//----------------------------------------------------------------------------
/**
 * @brief  �A�N�V������Ԃ�����
 * @param  mode
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_ResetAction( FIELD_SUBSCREEN_WORK* pWork)
{
  FIELD_SUBSCREEN_SetAction(pWork, FIELD_SUBSCREEN_ACTION_NONE);
}


#ifdef  PM_DEBUG
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork)
{ 
  return pWork->checker;
}
#endif  //PM_DEBUG
//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  CGEAR�̏�����
 *  
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_normal_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  pWork->cgearWork = CGEAR_Init(CGEAR_SV_GetCGearSaveData(
    GAMEDATA_GetSaveControlWork(
    GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(pWork->fieldmap))
    )), pWork, FIELDMAP_GetGameSysWork(pWork->fieldmap));
}

//----------------------------------------------------------------------------
/**
 *  @brief  �C���t�H�[�o�[�̔j��
 */
//-----------------------------------------------------------------------------
static void exit_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  CGEAR_Exit(pWork->cgearWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�N�V�������s��ꂽ����ʒm
 */
//-----------------------------------------------------------------------------
static void actioncallback_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno)
{
  CGEAR_ActionCallback(pWork->cgearWork, actionno);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �C���t�H�[�o�[�̍X�V
 */
//-----------------------------------------------------------------------------
static void update_normal_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  CGEAR_Main(pWork->cgearWork,bActive);
}

//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  NOGEAR�̏�����
 *  
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_nogear_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  pWork->nogearWork = NOGEAR_Init(CGEAR_SV_GetCGearSaveData(
    GAMEDATA_GetSaveControlWork(
    GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(pWork->fieldmap))
    )), pWork, FIELDMAP_GetGameSysWork(pWork->fieldmap));
}

//----------------------------------------------------------------------------
/**
 *  @brief  NOGEAR�[�̔j��
 */
//-----------------------------------------------------------------------------
static void exit_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  NOGEAR_Exit(pWork->nogearWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�N�V�������s��ꂽ����ʒm
 */
//-----------------------------------------------------------------------------
static void actioncallback_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno)
{
  NOGEAR_ActionCallback(pWork->nogearWork, actionno);
}

//----------------------------------------------------------------------------
/**
 *  @brief  NOGEAR�̍X�V
 */
//-----------------------------------------------------------------------------
static void update_nogear_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  NOGEAR_Main(pWork->nogearWork,bActive);
}


//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  �ԊO���f�o�b�O�̏�����
 *  
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_debugred_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  // BG3 SUB (�C���t�H�o�[
  static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000,0x6000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
  };
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( pWork->fieldmap );
  GAME_COMM_SYS_PTR commSys = GAMESYSTEM_GetGameCommSysPtr(gameSys);


  GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );
  GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
  
  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

  INFOWIN_Init( FIELD_SUBSCREEN_BGPLANE , FIELD_SUBSCREEN_PALLET , commSys , pWork->heapID);
 // FIELD_SUBSCREEN_SetAction(pWork, FIELD_SUBSCREEN_ACTION_DEBUGIRC);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ԊO���f�o�b�O�̔j��
 */
//-----------------------------------------------------------------------------
static void exit_debugred_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  INFOWIN_Exit();
  GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ԊO���f�o�b�O�̍X�V
 */
//-----------------------------------------------------------------------------
static void update_debugred_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  INFOWIN_Update();
}

//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̏�����
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_topmenu_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  // BG3 SUB (�C���t�H�o�[
  static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x00000,0x5800,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
  };
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( pWork->fieldmap );
  GAME_COMM_SYS_PTR commSys = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  u32 zoneId = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gameSys) );
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gameData );

  BOOL isScrollIn = FALSE;

  if( prevMode == FIELD_SUBSCREEN_NORMAL ||
      prevMode == FIELD_SUBSCREEN_NOGEAR ||
      prevMode == FIELD_SUBSCREEN_UNION )
  {
    isScrollIn = TRUE;
  }

  GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );
  GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
  GFL_BG_ClearScreenCode( FIELD_SUBSCREEN_BGPLANE , 0 );
  
  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);
  
  pWork->fieldMenuWork = FIELD_MENU_InitMenu( HEAPID_FIELD_SUBSCREEN , pWork->heapID , pWork , pWork->fieldmap , isScrollIn );
  if(FIELDMENU_GetMenuType(ev,zoneId)!=FIELD_MENU_UNION){
    INFOWIN_Init( FIELD_SUBSCREEN_BGPLANE , FIELD_SUBSCREEN_PALLET , commSys , pWork->heapID);
  }
  // INFOWIN�̃X�N���[���������Ƀ^�b�`�o�[����������(�̂�INFOWIN_Init�̌�ɒu���Ăˁj
  FIELDMENU_RewriteInfoScreen( pWork->heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̔j��
 */
//-----------------------------------------------------------------------------
static void exit_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( pWork->fieldmap );
  u32 zoneId = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gameSys) );
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gameData );

  if(FIELDMENU_GetMenuType(ev,zoneId)!=FIELD_MENU_UNION){
    INFOWIN_Exit();
  }
  FIELD_MENU_ExitMenu( pWork->fieldMenuWork );
  GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̍X�V
 */
//-----------------------------------------------------------------------------
static void update_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  FIELD_MENU_UpdateMenu( pWork->fieldMenuWork );
  INFOWIN_Update();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static void draw_topmenu_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  FIELD_MENU_DrawMenu( pWork->fieldMenuWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���j���[�ɍ��ڂ̏����ʒu�ݒ�
 */
//----------------------------------------------------------------------------
const FIELD_MENU_ITEM_TYPE FIELD_SUBSCREEN_GetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork )
{
  if( FIELD_SUBSCREEN_GetMode( pWork ) != FIELD_SUBSCREEN_TOPMENU )
  {
    GF_ASSERT_MSG( 0,"Subscreen mode is not topmenu!!\n" );
    return FMIT_EXIT;
  }
  else
  {
    return (FIELD_MENU_GetMenuItemNo( pWork->fieldMenuWork ));
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���j���[���猈�荀�ڂ̎擾
 */
//----------------------------------------------------------------------------
void FIELD_SUBSCREEN_SetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork , const FIELD_MENU_ITEM_TYPE itemType )
{
  if( FIELD_SUBSCREEN_GetMode( pWork ) != FIELD_SUBSCREEN_TOPMENU )
  {
    GF_ASSERT_MSG( 0,"Subscreen mode is not topmenu!!\n" );
    return;
  }
  else
  {
    FIELD_MENU_SetMenuItemNo( pWork->fieldMenuWork , itemType );
  }

}


//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̏�����
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_union_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  pWork->unisubWork = UNION_SUBDISP_Init(FIELDMAP_GetGameSysWork(pWork->fieldmap));
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̔j��
 */
//-----------------------------------------------------------------------------
static void exit_union_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  UNION_SUBDISP_Exit(pWork->unisubWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̍X�V
 */
//-----------------------------------------------------------------------------
static void update_union_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( pWork->fieldmap );

  if(bActive == TRUE && UnionMain_CheckPlayerFreeMode(gameSys) == FALSE){
    bActive = FALSE;
  }
  UNION_SUBDISP_Update(pWork->unisubWork, bActive);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static void draw_union_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( pWork->fieldmap );

  if(bActive == TRUE && UnionMain_CheckPlayerFreeMode(gameSys) == FALSE){
    bActive = FALSE;
  }
  UNION_SUBDISP_Draw(pWork->unisubWork, bActive);
}


//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̏�����
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_intrude_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  pWork->intsubWork = INTRUDE_SUBDISP_Init(FIELDMAP_GetGameSysWork(pWork->fieldmap));
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̔j��
 */
//-----------------------------------------------------------------------------
static void exit_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  INTRUDE_SUBDISP_Exit(pWork->intsubWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̍X�V
 */
//-----------------------------------------------------------------------------
static void update_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  INTRUDE_SUBDISP_Update(pWork->intsubWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static void draw_intrude_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  INTRUDE_SUBDISP_Draw(pWork->intsubWork);
}

//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  ����Ⴂ��ʂ̏�����
 *  @param  heapID  �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void init_beacon_view_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode)
{
  pWork->beaconViewWork = BEACON_VIEW_Init(FIELDMAP_GetGameSysWork(pWork->fieldmap), pWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����Ⴂ��ʂ̔j��
 */
//-----------------------------------------------------------------------------
static void exit_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  BEACON_VIEW_Exit(pWork->beaconViewWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����Ⴂ��ʂ̍X�V
 */
//-----------------------------------------------------------------------------
static void update_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  BEACON_VIEW_Update(pWork->beaconViewWork, bActive);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����Ⴂ��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static void draw_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  BEACON_VIEW_Draw(pWork->beaconViewWork);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����Ⴂ��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static GMEVENT* evcheck_beacon_view_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bEvReqOK )
{
  return BEACON_VIEW_EventCheck(pWork->beaconViewWork, bEvReqOK );
}

//=============================================================================
//=============================================================================
static void init_light_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{ 

}
static void update_light_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{ 

}
static void exit_light_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{ 

}

//=============================================================================
//=============================================================================
static void init_touchcamera_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  static const GFL_CAMADJUST_SETUP camAdjustData= {
    PAD_BUTTON_SELECT,
    GFL_DISPUT_BGID_S0, GFL_DISPUT_PALID_15,
  };
  pWork->gflCamAdjust = GFL_CAMADJUST_Create(&camAdjustData, pWork->heapID);

}
static void update_touchcamera_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{ 
  if( GFL_CAMADJUST_Main( pWork->gflCamAdjust ) == FALSE )
  { 
    FIELD_SUBSCREEN_Change(pWork, FIELD_SUBSCREEN_NORMAL);
    //GFL_CAMADJUST_Delete( pWork->gflCamAdjust );
    //pWork->gflCamAdjust = NULL;
  }

}
static void exit_touchcamera_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  GFL_CAMADJUST_Delete( pWork->gflCamAdjust );
  pWork->gflCamAdjust = NULL;
}

//=============================================================================
//=============================================================================
static void init_soundviewer_subscreen(FIELD_SUBSCREEN_WORK * pWork, FIELD_SUBSCREEN_MODE prevMode )
{ 
  static const GFL_SNDVIEWER_SETUP sndStatusData= {
    PAD_BUTTON_SELECT,
    GFL_DISPUT_BGID_S0, GFL_DISPUT_PALID_15,
    PMSND_GetNowSndHandlePointer,
    PMSND_GetBGMsoundNo,
    PMSND_GetBGMplayerNoIdx,
    PMSND_CheckOnReverb,
    GFL_SNDVIEWER_CONTROL_ENABLE | GFL_SNDVIEWER_CONTROL_EXIT,
  };

  pWork->gflSndViewer = GFL_SNDVIEWER_Create( &sndStatusData, pWork->heapID );

}

static void update_soundviewer_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{ 
  if ( GFL_SNDVIEWER_Main( pWork->gflSndViewer ) == FALSE )
  { 
    GFL_SNDVIEWER_Delete( pWork->gflSndViewer );
    pWork->gflSndViewer = NULL;
  }
}

static void exit_soundviewer_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{ 
  GFL_SNDVIEWER_Delete( pWork->gflSndViewer );
  pWork->gflSndViewer = NULL;
}


//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  �_�E�W���O��ʂ̏�����
 */
//-----------------------------------------------------------------------------
static void init_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE prevMode )
{
  pWork->dowsingWork = DOWSING_Init( HEAPID_FIELD_SUBSCREEN, pWork->heapID, pWork, pWork->fieldmap );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �_�E�W���O��ʂ̔j��
 */
//-----------------------------------------------------------------------------
static void exit_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork )
{
  DOWSING_Exit( pWork->dowsingWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �_�E�W���O��ʂ̍X�V
 */
//-----------------------------------------------------------------------------
static void update_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork, BOOL bActive )
{
  DOWSING_Update( pWork->dowsingWork, bActive );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �_�E�W���O��ʂ̕`��
 */
//-----------------------------------------------------------------------------
static void draw_dowsing_subscreen( FIELD_SUBSCREEN_WORK* pWork,BOOL bActive )
{
  DOWSING_Draw( pWork->dowsingWork, bActive );
}


//=============================================================================
//=============================================================================

