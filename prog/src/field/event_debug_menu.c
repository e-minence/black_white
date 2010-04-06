//======================================================================
/**
 *
 * @file  event_debug_menu.c
 * @brief �t�B�[���h�f�o�b�O���j���[�̐���C�x���g
 * @author  kagaya
 * @date  2008.11.13
 */
//======================================================================
#ifdef  PM_DEBUG

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

#include "fieldmap.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"
#include "event_gamestart.h"

#include "net_app/irc_match/ircbattlemenu.h"
#include "event_ircbattle.h"
#include "event_wificlub.h"
#include "event_gtsnego.h"
#include "field_subscreen.h"
#include "sound/pm_sndsys.h"

#include "font/font.naix"

#include  "field/weather_no.h"
#include  "weather.h"
#include  "msg/msg_d_tomoya.h"

#include "field_debug.h"

#include "field_event_check.h"
#include "event_debug_item.h" //EVENT_DebugItemMake
#include "event_debug_numinput.h"
#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p
#include  "item/itemsym.h"  //ITEM_DATA_MAX
#include  "item/item.h"  //ITEM_CheckEnable
#include "app/townmap.h"
#include "../ui/debug/ui_template.h"
#include "savedata/shortcut.h"
#include "event_debug_beacon.h"
#include "app/waza_oshie.h"

#include "field_buildmodel.h"

#include "field_sound.h"

#include "script.h" //SCRIPT_ChangeScript
#include "../../../resource/fldmapdata/script/debug_scr_def.h"  //SCRID_DEBUG_COMMON
#include "../../../resource/fldmapdata/script/hiden_def.h"  //SCRID_HIDEN_DIVING
#include "../../../resource/fldmapdata/script/pasokon_def.h"  //SCRID_PC

#include "eventwork.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "event_wifibattlematch.h"

#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

//CGEAR PICTURE
#include "c_gear.naix"
#include "./c_gear/c_gear.h"
#include "./c_gear/event_cgearget.h"
#include "savedata/save_tbl.h"
#include "savedata/c_gear_picture.h"

#include "event_debug_local.h"

#include "event_debug_season_display.h" // for DEBUG_EVENT_FLDMENU_SeasonDispSelect
#include "event_debug_demo3d.h" // for DEBUG_EVENT_FLDMENU_Demo3DSelect
#include "event_debug_menu_make_egg.h"  // for DEBUG_EVENT_FLDMENU_MakeEgg

#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SJIStoStrcode

#include "event_debug_wifimatch.h"
#include "event_battlerecorder.h"
#include "event_debug_mvpoke.h"
#include "field_bbd_color.h"

#include "gamesystem/pm_weather.h"

#include "debug/debug_mystery_card.h"

#include "bsubway_scr.h"
#include "event_wifi_bsubway.h"
#include "event_bsubway.h"
#include "scrcmd_bsubway.h"

#include "savedata/battle_box_save.h"
#include "event_geonet.h"
#include "app/name_input.h"
#include "waza_tool\wazano_def.h"

#include "savedata/symbol_save.h"
#include "savedata/symbol_save_notwifi.h"
#include "savedata/symbol_save_field.h"
#include "event_gts.h"
#include "src\musical\musical_debug.h"

#include "test/performance.h"

FS_EXTERN_OVERLAY( d_iwasawa );

//======================================================================
//  define
//======================================================================
#define DEBUG_MENU_PANO (14)
#define DEBUG_FONT_PANO (15)

#define D_MENU_CHARSIZE_X (18)    //���j���[����
#define D_MENU_CHARSIZE_Y (16)    //���j���[�c��

#define LINER_CAM_KEY_WAIT    (30)
#define LINER_CAM_KEY_COUNT    (15)

//======================================================================
//  typedef struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static FLDMENUFUNC * DEBUGFLDMENU_InitExPos(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos );
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y);
static u32 DebugMenu_GetQuickJumpIdx( u32 strID );

static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );

static BOOL debugMenuCallProc_OpenStartComm( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenStartInvasion( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenGTSNegoMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk );


static debugMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk );
static debugMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk );
static debugMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_EventFlagScript( DEBUG_MENU_EVENT_WORK *now_wk );
static BOOL debugMenuCallProc_ScriptSelect( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_GameEnd( DEBUG_MENU_EVENT_WORK * wk );

static BOOL debugMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk );


static BOOL debugMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ForceSave( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_CaptureList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_FieldAveStress( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugMakePoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugReWritePoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugSecretItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugPDWItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_MyItemMax( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_SetBtlBox( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ChangeName( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_ChangePlayerSex( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_WifiGts( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_GDS( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_UITemplate( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Jump( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_NumInput( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_Kairiki( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlLinerCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlDelicateCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenu_ControlShortCut( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenu_CreateMusicalShotData( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BeaconFriendCode( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_WifiBattleMatch( DEBUG_MENU_EVENT_WORK *wk );
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
static BOOL debugMenuCallProc_WazaOshie( DEBUG_MENU_EVENT_WORK *p_wk );


static BOOL debugMenuCallProc_UseMemoryDump( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_SeasonDisplay( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugSake( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugAtlas( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Geonet( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_BattleRecorder( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Ananukenohimo( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Anawohoru( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Teleport( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Diving( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Demo3d( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugMvPokemon( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BBDColor( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_MakeEgg( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_EncEffList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_DebugMakeUNData( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_BSubway( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_GPowerList( DEBUG_MENU_EVENT_WORK *wk );
static GMEVENT_RESULT debugMenuGPowerListEvent(GMEVENT *event, int *seq, void *wk );
static void DEBUG_SetMenuWorkGPower(GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list,
  HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DEBUG_GetGPowerMax( GAMESYS_WORK* gsys, void* cb_work );

static BOOL debugMenuCallProc_FieldSkillList( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardList( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardPoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardItem( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardGPower( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_Zukan( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugZoneJump( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_AllMapCheck( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_RingTone( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_PCScript( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_EventPokeCreate( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_SymbolPokeCreate( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_SymbolPokeList( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_SymbolPokeKeepLargeFull( DEBUG_MENU_EVENT_WORK * wk );
static BOOL debugMenuCallProc_SymbolPokeKeepSmallFull( DEBUG_MENU_EVENT_WORK * wk );
static BOOL debugMenuCallProc_SymbolPokeFreeLargeFull( DEBUG_MENU_EVENT_WORK * wk );
static BOOL debugMenuCallProc_SymbolPokeFreeSmallFull( DEBUG_MENU_EVENT_WORK * wk );
static BOOL debugMenuCallProc_SymbolPokeCountup( DEBUG_MENU_EVENT_WORK * wk );
static BOOL debugMenuCallProc_FadeSpeedChange( DEBUG_MENU_EVENT_WORK * wk );

//======================================================================
//  �f�o�b�O���j���[���X�g
//======================================================================
//--------------------------------------------------------------
/// �f�o�b�O���j���[���X�g
/// �f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
  { DEBUG_FIELD_TITLE_00, (void*)BMPMENULIST_LABEL },       //���W�����v�i�ړ��p�j
  { DEBUG_FIELD_STR38, debugMenuCallProc_DebugSkyJump },    //�����
  { DEBUG_FIELD_STR43, debugMenuCallProc_Jump },            //�W�����v�i���j�I�����j
  { DEBUG_FIELD_STR05, debugMenuCallProc_MapZoneSelect },   //�]�[���I���W�����v
  { DEBUG_FIELD_STR06, debugMenuCallProc_MapSeasonSelect},  //�l�G�W�����v
  { DEBUG_FIELD_STR05_b,   debugMenuCallProc_DebugZoneJump }, //�]�[���W�����v
  { DEBUG_FIELD_STR09, debugMenuCallProc_AllMapCheck }, //�I�[���}�b�v�`�F�b�N

  { DEBUG_FIELD_TITLE_02, (void*)BMPMENULIST_LABEL },       //���t�B�[���h
  { DEBUG_FIELD_STRESS, debugMenuCallProc_FieldAveStress },    //���ϕ���
  { DEBUG_FIELD_STR17, debugMenuCallProc_FieldPosData },    //���W���݂�
  { DEBUG_FIELD_STR02, debugMenuCallProc_ControlLinerCamera },  //�J�����ȒP����
  { DEBUG_FIELD_STR52, debugMenuCallProc_ControlDelicateCamera }, //�J�����S������
  { DEBUG_FIELD_EVENT_CONTROL, debugMenuCallProc_EventFlagScript }, //�C�x���g����
  { DEBUG_FIELD_STR03, debugMenuCallProc_ScriptSelect },    //�X�N���v�g���s
  { DEBUG_FIELD_STR13, debugMenuCallProc_MMdlList },        //���f�����X�g
  { DEBUG_FIELD_STR15, debugMenuCallProc_ControlLight },      //���C�g
  { DEBUG_FIELD_STR16, debugMenuCallProc_WeatherList },       //�Ă�
  { DEBUG_FIELD_STR36, debugMenuCallProc_ControlFog },        //�t�H�O����
  { DEBUG_FIELD_STR_SUBSCRN, debugMenuCallProc_SubscreenSelect }, //����ʑ���
  { DEBUG_FIELD_FSKILL, debugMenuCallProc_FieldSkillList },  //�t�B�[���h�Z�i�g���A���͓��j
  { DEBUG_FIELD_MVPOKE,   debugMenuCallProc_DebugMvPokemon },     //���ǂ��|�P����
  { DEBUG_FIELD_STR62,   debugMenuCallProc_BBDColor },            //�r���{�[�h�̐F
  { DEBUG_FIELD_ENCEFF, debugMenuCallProc_EncEffList },           //�G���J�E���g�G�t�F�N�g

  { DEBUG_FIELD_TITLE_01, (void*)BMPMENULIST_LABEL },       //���V�X�e��
  { DEBUG_FIELD_NUMINPUT, debugMenuCallProc_NumInput },     //���l����
  { DEBUG_FIELD_STR04, debugMenuCallProc_GameEnd },         //�Q�[���I��
  { DEBUG_FIELD_STR60, debugMenuCallProc_ForceSave },       //�����Z�[�u
  { DEBUG_FIELD_STR53, debugMenuCallProc_UseMemoryDump },   //��������
  { DEBUG_FIELD_STR22, debugMenuCallProc_ControlRtcList },  //����
  { DEBUG_FIELD_STR61, debugMenuCallProc_CaptureList },     //�L���v�`��
  { DEBUG_FIELD_STR40, debugMenuCallProc_ChangePlayerSex },   //��l�����ʕύX
  //{ DEBUG_FIELD_STR35, debugMenuCallProc_FadeSpeedChange },   //�t�F�[�h���x�ύX

  { DEBUG_FIELD_TITLE_03, (void*)BMPMENULIST_LABEL },       //���f�[�^�쐬
  { DEBUG_FIELD_STR41, debugMenuCallProc_DebugMakePoke },   //�|�P�����쐬
  { DEBUG_FIELD_STR65, debugMenuCallProc_DebugReWritePoke },//�|�P������������
  { DEBUG_FIELD_STR32, debugMenuCallProc_DebugItem },       //�A�C�e���쐬
  { DEBUG_FIELD_STR33, debugMenuCallProc_DebugSecretItem }, //�B����A�C�e���쐬
  { DEBUG_FIELD_STR34, debugMenuCallProc_DebugPDWItem },    //PDW�A�C�e���쐬
  { DEBUG_FIELD_STR37, debugMenuCallProc_BoxMax },          //�{�b�N�X�ő�
  { DEBUG_FIELD_STR39, debugMenuCallProc_MyItemMax },       //�A�C�e���ő�
  { DEBUG_FIELD_MAKE_EGG,   debugMenuCallProc_MakeEgg },          //�^�}�S�쐬
  { DEBUG_FIELD_MAKE_UNDATA,   debugMenuCallProc_DebugMakeUNData }, //���A�f�[�^�쐬
  { DEBUG_FIELD_MYSTERY_00, debugMenuCallProc_MakeMysteryCardList },//�ӂ����Ȃ�������̃J�[�h�쐬
  { DEBUG_FIELD_STR63, debugMenuCallProc_SetBtlBox },  //�s���`�F�b�N��ʂ�|�P�������쐬
  { DEBUG_FIELD_STR64, debugMenuCallProc_ChangeName },  //��l�������Đݒ�
  { DEBUG_FIELD_STR67, debugMenuCallProc_EventPokeCreate },  //�C�x���g�|�P�����쐬
  //{ DEBUG_FIELD_STR69, debugMenuCallProc_SymbolPokeCreate },  //�V���{���|�P�����쐬
  { DEBUG_FIELD_STR69, debugMenuCallProc_SymbolPokeList },  //�V���{���|�P�����쐬

  { DEBUG_FIELD_TITLE_06, (void*)BMPMENULIST_LABEL },       //��������
  { DEBUG_FIELD_STR19, debugMenuCallProc_OpenClubMenu },      //WIFI�N���u
  { DEBUG_FIELD_GPOWER, debugMenuCallProc_GPowerList},      //G�p���[
  { DEBUG_FIELD_C_CHOICE00, debugMenuCallProc_OpenCommDebugMenu },  //�ʐM�J�n
  { DEBUG_FIELD_STR49, debugMenuCallProc_BeaconFriendCode },  //�Ƃ������R�[�h�z�M
  { DEBUG_FIELD_STR57, debugMenuCallProc_DebugSake },             //�T�P����
  { DEBUG_FIELD_STR58, debugMenuCallProc_DebugAtlas },            //�A�g���X����
  { DEBUG_FIELD_GEONET, debugMenuCallProc_Geonet },         //�W�I�l�b�g�Ăяo��

  { DEBUG_FIELD_TITLE_04, (void*)BMPMENULIST_LABEL },       //���A�v��
  { DEBUG_FIELD_STR44, debugMenuCallProc_UITemplate },        //UI�e���v���[�g
  { DEBUG_FIELD_DEMO3D,   debugMenuCallProc_Demo3d },             //�RD�f��

  { DEBUG_FIELD_TITLE_05, (void*)BMPMENULIST_LABEL },       //���X�^�b�t�p
  { DEBUG_FIELD_STR70, debugMenuCallProc_PCScript },        //�p�\�R���Ăяo��
  { DEBUG_FIELD_STR47, debugMenu_ControlShortCut },           //Y�{�^���o�^�ő�
  { DEBUG_FIELD_STR51  , debugMenuCallProc_OpenGTSNegoMenu }, //GTS�l�S
  { DEBUG_FIELD_STR55,   debugMenuCallProc_CGEARPictureSave },//C�M�A�[�ʐ^
  { DEBUG_FIELD_STR21 , debugMenuCallProc_MusicalSelect },    //�~���[�W�J��
  { DEBUG_FIELD_STR42, debugMenuCallProc_WifiGts },           //GTS
  { DEBUG_FIELD_STR48, debugMenuCallProc_GDS },               //GDS
  { DEBUG_FIELD_STR50, debugMenuCallProc_WazaOshie },         //�Z����
  { DEBUG_FIELD_STR56, debugMenuCallProc_WifiBattleMatch },   //WIFI���E�ΐ�
  { DEBUG_FIELD_SEASON_DISPLAY, debugMenuCallProc_SeasonDisplay },//�G�ߕ\��
  { DEBUG_FIELD_STR59, debugMenuCallProc_BattleRecorder },        //�o�g�����R�[�_�[
  { DEBUG_FIELD_BSW_00, debugMenuCallProc_BSubway },                //�o�g���T�u�E�F�C
  { DEBUG_FIELD_ZUKAN_04, debugMenuCallProc_Zukan },            //������
  { DEBUG_FIELD_STR66,  debugMenuCallProc_RingTone },           //���M��
  { DEBUG_FIELD_STR47, debugMenu_CreateMusicalShotData },           //�~���[�W�J���f�[�^�쐬
};


//------------------------------------------------------------------------
/*
 *  QUICKSJUMP
 *  ��L�̃��X�g��STRID�������ƁASTART�{�^�����������Ƃ��ɁA���̈ʒu�܂ŃJ�[�\���������܂�
 */
//------------------------------------------------------------------------
#if defined DEBUG_ONLY_FOR_toru_nagihashi
  #define QuickJumpStart   DEBUG_FIELD_STR48
#elif defined DEBUG_ONLY_FOR_matsumiya
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_masafumi_saitou
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_suginaka_katsunori
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_mizuguchi_mai || defined DEBUG_ONLY_FOR_mai_ando
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_murakami_naoto
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_ohno
  #define QuickJumpStart   DEBUG_FIELD_STR19
//#elif defined DEBUG_ONLY_FOR_
//  #define QuickJumpStart
#endif

//--------------------------------------------------------------
/// ���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
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
  2,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
  DebugMenu_IineCallBack,  //�P�s���Ƃ̃R�[���o�b�N
};

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugMenuData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_DebugMenuList),
  DATA_DebugMenuList,
  &DATA_DebugMenuListHeader,
  1, 1, D_MENU_CHARSIZE_X, D_MENU_CHARSIZE_Y,
  NULL,
  NULL,
};

//======================================================================
//  �C�x���g�F�t�B�[���h�f�o�b�O���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @param page_id �f�o�b�O���j���[�y�[�W
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  HEAPID heapID, u16 page_id )
{
  DEBUG_MENU_EVENT_WORK * dmew;
  GMEVENT * event;

  event = GMEVENT_Create(
    gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));

  dmew = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );

  dmew->gmSys = gsys;
  dmew->gmEvent = event;
  dmew->gdata = GAMESYSTEM_GetGameData( gsys );
  dmew->fieldWork = fieldWork;
  dmew->heapID = heapID;


  // POS�\���X�V�@���@��~
  {
    FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( fieldWork );
    FIELD_DEBUG_SetPosUpdateFlag( debug, FALSE );
  }

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static FLDMENUFUNC * DebugMenuInitCore(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, void* cb_work, u16 list_pos, u16 cursor_pos )
{
  GAMESYS_WORK * gsys;
  FLDMENUFUNC * ret;
  GFL_MSGDATA *msgData;
  FLDMSGBG *msgBG;
  FLDMENUFUNC_LISTDATA *listdata;
  FLDMENUFUNC_HEADER menuH;
  u16 max;

  gsys = FIELDMAP_GetGameSysWork( fieldmap );
  msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  msgData = FLDMSGBG_CreateMSGDATA( msgBG, init->msg_arc_id );

  if (init->getMaxFunc) {
    max = init->getMaxFunc( gsys, cb_work );
  } else {
    max = init->max;
  }
  if (init->makeListFunc) {
    listdata = FLDMENUFUNC_CreateListData( max, heapID );
    init->makeListFunc( gsys, listdata, heapID, msgData, cb_work );
  } else {
    listdata = FLDMENUFUNC_CreateMakeListData( init->menulist, max, msgData, heapID );
  }

  menuH = *(init->menuH);
  {
    u8 sy = (max * 2 < init->sy) ? max * 2 : init->sy;
    FLDMENUFUNC_InputHeaderListSize( &menuH, max, init->px, init->py, init->sx, sy );
  }
  ret = FLDMENUFUNC_AddMenuList( msgBG, &menuH, listdata, list_pos, cursor_pos );
  GFL_MSG_Delete( msgData );
  return ret;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
FLDMENUFUNC * DEBUGFLDMENU_Init(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, 0, 0 );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
FLDMENUFUNC * DEBUGFLDMENU_InitEx(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, void* cb_work )
{
  return DebugMenuInitCore( fieldmap, heapID, init, cb_work, 0, 0 );
}

static FLDMENUFUNC * DEBUGFLDMENU_InitExPos(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, list_pos, cursor_pos );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�f�o�b�O���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch (*seq) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
#ifdef QuickJumpStart
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
        {
          u32 idx = DebugMenu_GetQuickJumpIdx( QuickJumpStart );
          FLDMENUFUNC_DeleteMenu( work->menuFunc );
          work->menuFunc  = DEBUGFLDMENU_InitExPos( work->fieldWork, work->heapID,  &DebugMenuData, idx-4, 4 );
        }
#endif  //QuickJumpStart
#ifdef QuickJumpSelect
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
        {
          u32 idx = DebugMenu_GetQuickJumpIdx( QuickJumpSelect );
          FLDMENUFUNC_DeleteMenu( work->menuFunc );
          work->menuFunc  = DEBUGFLDMENU_InitExPos( work->fieldWork, work->heapID,  &DebugMenuData, idx-4, 4 );
        }
#endif  //QuickJumpSelect

        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        (*seq)++;
      }else{              //����
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( work->call_proc != NULL ){
        if( work->call_proc(work) == TRUE ){
          return( GMEVENT_RES_CONTINUE );
        }
      }

      // POS�\���X�V�@���@�ĊJ
      {
        FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( work->fieldWork );
        FIELD_DEBUG_SetPosUpdateFlag( debug, TRUE );
      }
      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �P�s���Ƃ̃R�[���o�b�N
 *
 *  @param  lw    ���[�N
 *  @param  param ����
 *  @param  y     Y�ʒu
 */
//-----------------------------------------------------------------------------
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y)
{
  if(param == BMPMENULIST_LABEL){
    BmpMenuList_TmpColorChange( lw,4,0,4);
  }else{
    BmpMenuList_TmpColorChange(lw,1,0,2);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �֐��̈ʒu���擾
 *
 *  @param  void *p_adrs  �T�[�`����֐�
 *
 *  @return �֐��̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 DebugMenu_GetQuickJumpIdx( u32 strID )
{
  int i;
  for( i = 0; i < NELEMS(DATA_DebugMenuList); i++ )
  {
    if( DATA_DebugMenuList[i].str_id == strID )
    {
      return i;
    }
  }
  GF_ASSERT_MSG( 0, "�f�o�b�O���j���[�̃N�C�b�N�W�����v���Ԉ���Ă��܂�\n" );
  return 0;
}





//======================================================================
//  �ėp���X�g�@�w�b�_�[
//======================================================================
const FLDMENUFUNC_HEADER DATA_DebugMenuList_Default =
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

/// �ǂ��ł��W�����v�@���j���[�w�b�_�[
const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
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

//======================================================================
//  �f�o�b�O���j���[�Ăяo��
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL debugMenuCallProc_EventFlagScript( DEBUG_MENU_EVENT_WORK *now_wk )
{
  SCRIPT_ChangeScript( now_wk->gmEvent, SCRID_DEBUG_COMMON, NULL, HEAPID_FIELDMAP );
  return TRUE;
}
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�f�o�b�O�X�N���v�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ScriptSelect( DEBUG_MENU_EVENT_WORK *now_wk )
{
  GMEVENT * new_event = DEBUG_EVENT_FLDMENU_DebugScript( now_wk );
  GMEVENT_ChangeEvent( now_wk->gmEvent, new_event );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ʐM�f�o�b�O�q���j���[
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_GameEnd( DEBUG_MENU_EVENT_WORK * wk )
{
  GMEVENT * new_event = DEBUG_EVENT_GameEnd( wk->gmSys, wk->fieldWork );
  GMEVENT_ChangeEvent( wk->gmEvent, new_event );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�W�����v
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Jump( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_JumpEasy, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpEasy( wk->gmSys, NULL ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�l�G�}�b�v�Ԉړ�
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static debugMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_ChangeSeason, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_ChangeSeason( wk->gmSys, NULL ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ǂ��ł��W�����v
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_JumpAllZone, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpAllZone( wk->gmSys, NULL ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief �^�}�S��������
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_MakeEgg( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parentEvent = wk->gmEvent;
  GAMESYS_WORK* gameSystem  = wk->gmSys;
  HEAPID        heapID      = wk->heapID;

  GMEVENT_ChangeEvent( parentEvent, DEBUG_EVENT_FLDMENU_MakeEgg( gameSystem, heapID ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ʐM�f�o�b�O�q���j���[
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  const HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
#if 0 //�ʐM�V�X�e���ύX�ׁ̈AFix 2009.09.03(��)
  FIELD_COMM_DEBUG_WORK *work;

  GMEVENT_Change( event,
    FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
  work = GMEVENT_GetEventWork( event );
  FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );
#else
  GF_ASSERT(0);
#endif

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * WiFiClub���j���[�Ăт���
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  GMEVENT *next_event;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;

  EVENT_WiFiClubChange(gameSys, fieldWork, event);
//  CGEARGET_EVENT_Change(gameSys, event);

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * GTSNego���j���[�Ăт���
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenGTSNegoMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;

  EVENT_GTSNegoChangeDebug(gameSys, fieldWork, event);
  return( TRUE );
}



#if 0
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O �|�P�������Z�[�u�f�[�^�ɓ����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
#include "savedata/dreamworld_data.h"
#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "savedata/record.h"
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData(wk->gmSys);
    DREAMWORLD_SAVEDATA *pdw = SaveControl_DataPtrGet(GAMEDATA_GetSaveControlWork(gamedata), GMDATA_ID_DREAMWORLD);
    POKEPARTY *party = GAMEDATA_GetMyPokemon(gamedata);
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, 0);
    POKEMON_PASO_PARAM  *ppp = PP_GetPPPPointer( pp );

    OS_TPrintf("before monsno = %d, level = %d, item = %d\n", PPP_Get(ppp, ID_PARA_monsno, NULL), PPP_Get(ppp, ID_PARA_level, NULL), PPP_Get(ppp, ID_PARA_item, NULL));
    DREAMWORLD_SV_SetSleepPokemon(pdw, pp);

//    pp = DREAMWORLD_SV_GetSleepPokemon(pdw);
  //  OS_TPrintf("after monsno = %d, level = %d, item = %d\n", PPP_Get(ppp, ID_PARA_monsno, NULL), PPP_Get(ppp, ID_PARA_level, NULL), PPP_Get(ppp, ID_PARA_item, NULL));

    {//My�X�e�[�^�X
      MYSTATUS* pMy = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(wk->gmSys));

      OS_TPrintf("before myst gold=%d,  id=%d, sex=%d\n",pMy->gold,  pMy->id, pMy->sex);
      pMy->gold = 511;
      pMy->id = 88776655;
      pMy->sex = 0;
      OS_TPrintf("after myst gold=%d,  id=%d, sex=%d\n",pMy->gold, pMy->id, pMy->sex);
    }

    {//���R�[�h
      long* rec = (long*)SaveData_GetRecord(GAMEDATA_GetSaveControlWork(gamedata));

      OS_TPrintf("before record capture=%d, fishing=%d\n", rec[RECID_CAPTURE_POKE], rec[RECID_FISHING_SUCCESS]);
      rec[RECID_CAPTURE_POKE] = 7896;
      rec[RECID_FISHING_SUCCESS] = 3;
      OS_TPrintf("after record capture=%d, fishing=%d\n", rec[RECID_CAPTURE_POKE], rec[RECID_FISHING_SUCCESS]);
    }
  }

 // GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpEasy( wk->gmSys, wk->heapID ) );
  return( FALSE );
}


//--------------------------------------------------------------
/**
 * @brief     CGear�̊G��ROM����ǂݏo���Z�[�u�̈�ɓ����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval    BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
#endif
#if 1
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));

  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_CGEAR_PICTURE);
  CGEAR_PICTURE_SAVEDATA* pPic=(CGEAR_PICTURE_SAVEDATA*)pCGearWork;

  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, HEAPID_FIELDMAP,
                             pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );
  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->picture, CGEAR_DECAL_SIZE_MAX);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    GFL_STD_MemCopy(palData->pRawData, pPic->palette, CGEAR_PICTURTE_PAL_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    GFL_STD_MemCopy(palData->pRawData, pPic->palette, CGEAR_PICTURTE_PAL_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);


  GFL_ARC_CloseDataHandle( p_handle );

  crc = GFL_STD_CrcCalc( pPic, CGEAR_PICTURTE_ALL_SIZE );

  SaveControl_Extra_SaveAsyncInit(pSave,SAVE_EXTRA_ID_CGEAR_PICUTRE);
  while(1){
    if(SAVE_RESULT_OK==SaveControl_Extra_SaveAsyncMain(pSave,SAVE_EXTRA_ID_CGEAR_PICUTRE)){
      break;
    }
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
  GFL_HEAP_FreeMemory(pCGearWork);
  CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(pSave), TRUE);  //CGEAR�\���L��
  CGEAR_SV_SetCGearPictureONOFF(CGEAR_SV_GetCGearSaveData(pSave),TRUE);  //CGEAR�f�J�[���L��
  CGEAR_SV_SetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave),crc);  //CGEAR�f�J�[���L��

  return( FALSE );
}
#endif

#if 0    //�}�Ӄe�X�g

#include "include/savedata/zukan_wp_savedata.h"
// �Z�[�u�f�[�^
typedef struct  {
	// �J�X�^���O���t�B�b�N�L����
	u8	customChar[ZUKANWP_SAVEDATA_CHAR_SIZE];
	// �J�X�^���O���t�B�b�N�p���b�g
	u16	customPalette[ZUKANWP_SAVEDATA_PAL_SIZE];
	// �f�[�^�L���t���O
	BOOL	flg;
} TESTZUKAN_DATA;



static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  int size,i;
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));
  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  NNSG2dScreenData* pScrData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_ZUKAN_WALLPAPER);
  TESTZUKAN_DATA* pPic=(TESTZUKAN_DATA*)pCGearWork;

  pPic->flg=TRUE;

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->customChar, ZUKANWP_SAVEDATA_CHAR_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    for(i=0;i<16;i++){
      GFL_STD_MemCopy(palData->pRawData, &pPic->customPalette[32*i], 32);
    }
  }
  GFL_HEAP_FreeMemory(pArc);

  GFL_ARC_CloseDataHandle( p_handle );

  size = ZUKANWP_SAVEDATA_CHAR_SIZE+ZUKANWP_SAVEDATA_PAL_SIZE*2;
  crc = GFL_STD_CrcCalc( pPic, size );

  {
    int i;
    u8* pU8 = (u8*)pPic;
    for(i=0;i<size;i++){
      OS_TPrintf("0x%x ",pU8[i]);
      if((i%16)==15){
        OS_TPrintf("\n");
      }
    }
  }
  
  OS_TPrintf("-----%x \n",crc);

  GFL_HEAP_FreeMemory(pCGearWork);

  return( FALSE );
}
#endif


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo�� ���l����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_NumInput( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GMEVENT *event;

  event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
    FS_OVERLAY_ID( d_numinput ), DEBUG_EVENT_FLDMENU_NumInput, wk );

  GMEVENT_ChangeEvent( wk->gmEvent, event );

  return( TRUE );
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;
  FIELD_SUBSCREEN_WORK * subscreen;
}DEBUG_MENU_EVENT_SUBSCRN_SELECT_WORK, DMESSWORK;

//--------------------------------------------------------------
//  proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSubscreenSelectEvent(
    GMEVENT *event, int *seq, void *wk );
static void setupTouchCameraSubscreen(DMESSWORK * dmess);
static void setupSoundViewerSubscreen(DMESSWORK * dmess);
static void setupNormalSubscreen(DMESSWORK * dmess);
static void setupTopMenuSubscreen(DMESSWORK * dmess);
static void setupUnionSubscreen(DMESSWORK * dmess);
static void setupDebugLightSubscreen(DMESSWORK * dmess);
static void setupCGearFirstSubscreen(DMESSWORK * dmess);


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�l�G�}�b�v�Ԉړ�
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static debugMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  HEAPID heapID = wk->heapID;
  GMEVENT *event = wk->gmEvent;
  GAMESYS_WORK *gsys = wk->gmSys;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DMESSWORK *work;

    GMEVENT_Change( event,
      debugMenuSubscreenSelectEvent, sizeof(DMESSWORK) );
    work = GMEVENT_GetEventWork( event );
    GFL_STD_MemClear( work, sizeof(DMESSWORK) );

    work->gmSys = gsys;
    work->gmEvent = event;
    work->heapID = heapID;
    work->fieldWork = fieldWork;
    work->subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
    return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_SubcreenMenuList[FIELD_SUBSCREEN_MODE_MAX] =
{
  { DEBUG_FIELD_STR_SUBSCRN01, (void*)setupTouchCameraSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN02, (void*)setupSoundViewerSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN03, (void*)setupNormalSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN04, (void*)setupTopMenuSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN01, (void*)setupDebugLightSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN05, (void*)setupUnionSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN06, (void*)setupCGearFirstSubscreen },
};

static const DEBUG_MENU_INITIALIZER DebugSubscreenSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubcreenMenuList),
  DATA_SubcreenMenuList,
  &DATA_DebugMenuList_Default, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �C�x���g�F������ʃW�����v
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSubscreenSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DMESSWORK *work = wk;

  switch( *seq )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubscreenSelectData );
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

      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }else{
        typedef void (* CHANGE_FUNC)(DMESSWORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        //FIELD_SUBSCREEN_ChangeForce(FIELDMAP_GetFieldSubscreenWork(work->fieldWork), ret);
        return( GMEVENT_RES_FINISH );
      }
    }
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}

//--------------------------------------------------------------
/**
 * @brief �T�u�X�N���[���̃J��������ɓ���������
 *
 * @todo  �J�������o�C���h���Ă���A�ǂ̂悤�ɐ؂藣�����H���m�肳����
 */
//--------------------------------------------------------------
static void setupTouchCameraSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
  {
    void * inner_work;
    FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(dmess->fieldWork);
    inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(dmess->subscreen);
    FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS, dmess->heapID);
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupSoundViewerSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupNormalSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_NORMAL);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupTopMenuSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_TOPMENU);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupUnionSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_UNION);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupDebugLightSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_LIGHT);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupCGearFirstSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_CGEARFIRST);
}


//--------------------------------------------------------------
//  �~���[�W�J���n
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(musical);
#include "musical/musical_local.h"
#include "musical/musical_define.h"
#include "musical/musical_system.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_stage_sys.h"
#include "poke_tool/poke_tool.h"  //�h���X�A�b�v���f�[�^�p
#include "poke_tool/monsno_def.h" //�h���X�A�b�v���f�[�^�p
#include "event_fieldmap_control.h"
typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  GMEVENT *newEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;

  POKEMON_PARAM *pokePara;
  MUSICAL_POKE_PARAM *musPoke;
  DRESSUP_INIT_WORK *dupInitWork;
  u8  menuRet;
}DEBUG_MENU_EVENT_MUSICAL_SELECT_WORK, DEB_MENU_MUS_WORK;
static GMEVENT_RESULT debugMenuMusicalSelectEvent( GMEVENT *event, int *seq, void *wk );

static void setupMusicalDressup(DEB_MENU_MUS_WORK * work);
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work);

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�~���[�W�J���Ăяo��
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static debugMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  HEAPID heapID = wk->heapID;
  GMEVENT *event = wk->gmEvent;
  GAMESYS_WORK *gsys = wk->gmSys;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEB_MENU_MUS_WORK *work;

    GMEVENT_Change( event,
      debugMenuMusicalSelectEvent, sizeof(DEB_MENU_MUS_WORK) );
    work = GMEVENT_GetEventWork( event );
    GFL_STD_MemClear( work, sizeof(DEB_MENU_MUS_WORK) );

    work->gmSys = gsys;
    work->gmEvent = event;
    work->heapID = heapID;
    work->fieldWork = fieldWork;
    work->dupInitWork = NULL;
    work->musPoke = NULL;
    work->pokePara = NULL;
    return( TRUE );
}

//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_MusicalMenuList[3] =
{
  { DEBUG_FIELD_STR_MUSICAL1, (void*)setupMusicalDressup },
  { DEBUG_FIELD_STR_MUSICAL2, (void*)setupMusicarShowPart },
};

static const DEBUG_MENU_INITIALIZER DebugMusicalSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_MusicalMenuList),
  DATA_MusicalMenuList,
  &DATA_DebugMenuList_Default, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�~���[�W�J���f�o�b�O���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMusicalSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEB_MENU_MUS_WORK *work = wk;

  switch( *seq )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMusicalSelectData );
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

      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }else{
        typedef void (* CHANGE_FUNC)(DEB_MENU_MUS_WORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        GMEVENT_CallEvent(work->gmEvent, EVENT_FSND_PushBGM(work->gmSys, FSND_FADE_NONE));
        (*seq)++;
        return( GMEVENT_RES_CONTINUE );
      }
    }
    break;
  case 2:
    GMEVENT_CallEvent(work->gmEvent, work->newEvent);
    (*seq)++;
    break;
  case 3:
    if( work->dupInitWork != NULL )
    {
      MUSICAL_DRESSUP_DeleteInitWork( work->dupInitWork );
      work->dupInitWork = NULL;
    }
    if( work->musPoke != NULL )
    {
      GFL_HEAP_FreeMemory( work->musPoke );
      work->musPoke = NULL;
    }
    if( work->pokePara != NULL )
    {
      GFL_HEAP_FreeMemory( work->pokePara );
      work->pokePara = NULL;
    }
    (*seq)++;
    break;
  case 4:
    GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(work->gmSys, FSND_FADE_NONE, FSND_FADE_NORMAL));
    (*seq)++;
    break;
  case 5:
    return( GMEVENT_RES_FINISH );
  }

  return GMEVENT_RES_CONTINUE ;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicalDressup(DEB_MENU_MUS_WORK * work)
{
  work->pokePara = PP_Create( MONSNO_PIKUSII , 20 , PTL_SETUP_POW_AUTO , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->musPoke = MUSICAL_SYSTEM_InitMusPoke( work->pokePara , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( GFL_HEAPID_APP|HEAPDIR_MASK , work->musPoke , SaveControl_GetPointer() );

  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &DressUp_ProcData, work->dupInitWork );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work)
{
  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &MusicalStage_ProcData, NULL );
}

//======================================================================
//  �f�o�b�O���j���[�@�J��������
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CTL_LINERCAMERA_WORK ���`�J�������샏�[�N1
//--------------------------------------------------------------
typedef struct
{
  int vanish;
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;
  FLDMSGBG *pMsgBG;
  FLDMSGWIN *pMsgWin;
  STRBUF *pStrBuf;
  u16 Wait;
  u16 Count;
}DEBUG_CTL_LINERCAMERA_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLinerCamera(
    GMEVENT *event, int *seq, void *wk );
static void DampCameraInfo(FIELD_CAMERA * cam);
static BOOL LinerCamKeyContCtl(DEBUG_CTL_LINERCAMERA_WORK *work, const fx32 inAddVal, fx32 *outVal);


//======================================================================
//  �f�o�b�O���j���[�@���샂�f���ꗗ
//======================================================================
//--------------------------------------------------------------
/// DEBUG_MMDL_LIST_EVENT_WORK ���샂�f�����X�g�����p���[�N
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  MMDLSYS *fldmmdlsys;

  u16 obj_code;
  u8 padding[2];
  MMDL *fmmdl;
}DEBUG_MMDLLIST_EVENT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMMdlListEvent(
    GMEVENT *event, int *seq, void *wk );
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code );
static void DEBUG_SetMenuWorkMMdlList(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );

/// ���샂�f�����X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_MMdlList =
{
  1,    //���X�g���ڐ�
  9,   //�\���ő區�ڐ�
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

static const DEBUG_MENU_INITIALIZER DebugMMdlListData = {
  NARC_message_d_field_dat,
  OBJCODETOTAL,
  NULL,
  &DATA_DebugMenuList_MMdlList,
  1, 1, 11, 16,
  DEBUG_SetMenuWorkMMdlList,
  NULL,
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@���샂�f���ꗗ
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_MMDLLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuMMdlListEvent, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    work->fldmmdlsys = GAMEDATA_GetMMdlSys( gdata );
  }

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���샂�f���ꗗ
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMMdlListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MMDLLIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMMdlListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }

      if( ret == FLDMENUFUNC_CANCEL || ret == NONDRAW ){ //�L�����Z��
        FLDMENUFUNC_DeleteMenu( work->menuFunc );
        return( GMEVENT_RES_FINISH );
      }

      work->obj_code = ret;
      
      {
        MMDL *jiki;
        MMDL_HEADER head = {
          0,  ///<����ID
          0,  ///<�\������OBJ�R�[�h
          MV_DMY, ///<����R�[�h
          0,  ///<�C�x���g�^�C�v
          0,  ///<�C�x���g�t���O
          0,  ///<�C�x���gID
          0,  ///<�w�����
          0,  ///<�w��p�����^ 0
          0,  ///<�w��p�����^ 1
          0,  ///<�w��p�����^ 2
          4,  ///<X�����ړ�����
          4,  ///<Z�����ړ�����
          0,  ///<�O���b�hX
          0,  ///<�O���b�hZ
          0,  ///<Y�l fx32�^
        };

        jiki = MMDLSYS_SearchOBJID(
          work->fldmmdlsys, MMDL_ID_PLAYER );
        
        head.id = 250;
        
        MMDLHEADER_SetGridPos( &head,
            MMDL_GetGridPosX( jiki ) + 2,
            MMDL_GetGridPosZ( jiki ),
            MMDL_GetVectorPosY( jiki ) );
        
        head.obj_code = work->obj_code;
        
        work->fmmdl = MMDLSYS_AddMMdl(
          work->fldmmdlsys, &head, 0 );
      }

      (*seq)++;
      break;
    case 2:
      {
        MMDL *mmdl = work->fmmdl;
        int key_cont = GFL_UI_KEY_GetCont();
        u16 code = MMDL_GetOBJCode( mmdl );
        u16 ac = ACMD_NOT;
        
//        MMDL_UpdateMoveProc( work->fmmdl );
        
        if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
          if( (key_cont & PAD_BUTTON_B) ){
            MMDL_Delete( work->fmmdl );
            (*seq) = 1;
            return GMEVENT_RES_CONTINUE;
          }
          
          if( code == SHIN_A || code == MU_A ){
            if( key_cont & PAD_KEY_UP ){
              ac = AC_SHIN_MU_FLY_L;
            }else if( key_cont & PAD_KEY_DOWN ){
              ac = AC_DIR_D;
            }else if( key_cont & PAD_KEY_LEFT ){
              ac = AC_SHIN_MU_GUTARI;
            }else if( key_cont & PAD_KEY_RIGHT ){
              ac = AC_SHIN_MU_FLY_UPPER;
            }
            
            if( ac != ACMD_NOT ){
              VecFx32 offs = {0,0,0};
		          MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
            }
          }else if( code == SHIN_B || code == MU_B ){
            if( key_cont & PAD_KEY_UP ){
              ac = AC_DIR_U;
            }else if( key_cont & PAD_KEY_DOWN ){
              ac = AC_DIR_D;
            }else if( key_cont & PAD_KEY_LEFT ){
              ac = AC_SHIN_MU_HOERU;
            }else if( key_cont & PAD_KEY_RIGHT ){
              ac = AC_SHIN_MU_TURN;
            }
          }else{
            if( key_cont & PAD_KEY_UP ){
              ac = AC_STAY_WALK_U_32F;
            }else if( key_cont & PAD_KEY_DOWN ){
              ac = AC_STAY_WALK_D_32F;
            }else if( key_cont & PAD_KEY_LEFT ){
              ac = AC_STAY_WALK_L_32F;
            }else if( key_cont & PAD_KEY_RIGHT ){
              ac = AC_STAY_WALK_R_32F;
            }
          }
          
          if( ac != ACMD_NOT ){
            MMDL_SetAcmd( mmdl, ac );
          }
        }
      }
      break;
    }
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * ���샂�f��OBJ�R�[�h->STRCODE
 * @param heapID  ������o�b�t�@�m�ۗp�q�[�vID
 * @param code  ��������擾������OBJ�R�[�h
 * @retval  u16*  �����񂪊i�[���ꂽu16*(�J�����K�v
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code )
{
  int i;
  u16 utf16,utf16_eom;
  u16 *pStrBuf;
  u8 *name8;

  pStrBuf = GFL_HEAP_AllocClearMemory( heapID,
      sizeof(u16)*DEBUG_OBJCODE_STR_LENGTH );
  name8 = DEBUG_MMDL_GetOBJCodeString( code, heapID );
  //DEBUG_ConvertAsciiToUTF16( name8, DEBUG_OBJCODE_STR_LENGTH, pStrBuf );
  DEB_STR_CONV_SjisToStrcode( (const char *)name8, pStrBuf, DEBUG_OBJCODE_STR_LENGTH );
  GFL_HEAP_FreeMemory( name8 );

  return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * ���샂�f�����X�g�pBMP_MENULIST_DATA�Z�b�g
 * @param list  �Z�b�g��BMP_MENULIST_DATA
 * @param heapID  ������o�b�t�@�m�ۗpHEAPID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkMMdlList(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  u16 *str;
  int id,max;
  STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_OBJCODE_STR_LENGTH, heapID );

  for( id = OBJCODESTART_BBD; id < OBJCODEEND_BBD; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  for( id = OBJCODESTART_TPOKE; id < OBJCODEEND_TPOKE; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  for( id = OBJCODESTART_MDL; id < OBJCODEEND_MDL; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  GFL_HEAP_FreeMemory( strBuf );
}

//======================================================================
//  �f�o�b�O���j���[�@���C�g����
//======================================================================

//--------------------------------------------------------------
/// DEBUG_CTLIGHT_WORK ���C�g���샏�[�N
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;
}DEBUG_CTLLIGHT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLight(
    GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�J��������
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTLLIGHT_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlLight, sizeof(DEBUG_CTLLIGHT_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTLLIGHT_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * �C�x���g�F�J��������
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLight(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CTLLIGHT_WORK *work = wk;
  FIELD_LIGHT* p_light;

  // ���C�g�擾
  p_light = FIELDMAP_GetFieldLight( work->fieldWork );

  switch( (*seq) ){
  case 0:
    // ���C�g�Ǘ��J�n
    FIELD_LIGHT_DEBUG_Init( p_light, work->heapID );

    // �C���t�H�[�o�[�̔�\��
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // �r�b�g�}�b�v�E�B���h�E������
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // �p���b�g����]��
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // �r�b�g�}�b�v�E�B���h�E���쐬
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // �E�B���h�E
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

    (*seq)++;
  case 1:
    // ���C�g�Ǘ����C��
    FIELD_LIGHT_DEBUG_Control( p_light );
    FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:
    // ���C�g�Ǘ��I��
    FIELD_LIGHT_DEBUG_Exit( p_light );

    // �r�b�g�}�b�v�E�B���h�E�j��
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    // �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  �f�o�b�O���j���[�@�t�H�O����
//======================================================================

//--------------------------------------------------------------
/// DEBUG_CTLFOG_WORK �t�H�O���샏�[�N
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;

}DEBUG_CTLFOG_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlFog(
    GMEVENT *event, int *seq, void *wk );

static BOOL debugMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTLFOG_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlFog, sizeof(DEBUG_CTLFOG_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTLFOG_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * �C�x���g�F�ӂ�������
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlFog(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CTLFOG_WORK *work = wk;
  FIELD_FOG_WORK* p_fog;

  // �t�H�O�擾
  p_fog = FIELDMAP_GetFieldFog( work->fieldWork );

  switch( (*seq) ){
  case 0:

    // �C���t�H�[�o�[�̔�\��
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // �r�b�g�}�b�v�E�B���h�E������
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // �p���b�g����]��
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // �r�b�g�}�b�v�E�B���h�E���쐬
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // �E�B���h�E
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FIELD_FOG_DEBUG_Init( p_fog, work->heapID );


    (*seq)++;
  case 1:
    // �t�H�O�Ǘ����C��
    FIELD_FOG_DEBUG_Control( p_fog );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
    FIELD_FOG_DEBUG_PrintData( p_fog, work->p_win );
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:

    // �r�b�g�}�b�v�E�B���h�E�j��
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    FIELD_FOG_DEBUG_Exit( p_fog );


    // �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  �f�o�b�O���j���[�@�Ă񂫃��X�g
//======================================================================
//--------------------------------------------------------------
/// DEBUG_WEATERLIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_WEATERLIST_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuWeatherListEvent(
    GMEVENT *event, int *seq, void *work );

///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_WeatherList =
{
  1,    //���X�g���ڐ�
  6,    //�\���ő區�ڐ�
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

///�Ă񂫃��j���[���X�g
static const FLDMENUFUNC_LIST DATA_WeatherMenuList[] =
{
  { D_TOMOYA_WEATEHR00, (void*)WEATHER_NO_SUNNY },
  { D_TOMOYA_WEATEHR01, (void*)WEATHER_NO_SNOW },
  { D_TOMOYA_WEATEHR02, (void*)WEATHER_NO_RAIN },
  { D_TOMOYA_WEATEHR03, (void*)WEATHER_NO_STORM },
  { D_TOMOYA_WEATEHR06, (void*)WEATHER_NO_SNOWSTORM },
  { D_TOMOYA_WEATEHR07, (void*)WEATHER_NO_ARARE },
  { D_TOMOYA_WEATEHR09, (void*)WEATHER_NO_RAIKAMI },
  { D_TOMOYA_WEATEHR10, (void*)WEATHER_NO_KAZAKAMI },
  { D_TOMOYA_WEATEHR11, (void*)WEATHER_NO_DIAMONDDUST },
  { D_TOMOYA_WEATEHR12, (void*)WEATHER_NO_MIST },

  /*  ���g�p
  { D_TOMOYA_WEATEHR13, (void*)WEATHER_NO_EVENING_RAIN },
  { D_TOMOYA_WEATEHR05, (void*)WEATHER_NO_SPARK },
  { D_TOMOYA_WEATEHR08, (void*)WEATHER_NO_MIRAGE },
  */
};
#define DEBUG_WEATHERLIST_LIST_MAX  ( NELEMS(DATA_WeatherMenuList) )

static const DEBUG_MENU_INITIALIZER DebugWeatherMenuListData = {
  NARC_message_d_tomoya_dat,
  NELEMS(DATA_WeatherMenuList),
  DATA_WeatherMenuList,
  &DATA_DebugMenuList_WeatherList,
  1, 1, 12, 11,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�V�C���X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_WEATERLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuWeatherListEvent, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�Ă񂫃��X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuWeatherListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_WEATERLIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugWeatherMenuListData );
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

      if( ret != FLDMENUFUNC_CANCEL ){  //����
        GAMEDATA* gdata = GAMESYSTEM_GetGameData( work->gmSys );
        FIELD_WEATHER_Change( FIELDMAP_GetFieldWeather( work->fieldWork ), ret );
        GAMEDATA_SetWeatherNo( gdata, ret );
      }

      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[�@���ϕ���
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@���ϕ��׌v���J�n
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_FieldAveStress( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_PerformanceSetAveTest();
  return( FALSE );
}


//======================================================================
//  �f�o�b�O���j���[�@�ʒu���
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�t�B�[���h�ʒu���
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk )
{
  FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( wk->fieldWork );
  FIELD_DEBUG_SetPosPrint( debug );

  //�`��R�[���o�b�N�̃Z�b�g
  FIELD_DEBUG_SetDrawCallBackFunc(TRUE);
  
  return( FALSE );
}

//======================================================================
/// �f�o�b�N���j���[�@����
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CONTROL_TIME_LIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_CONTROL_TIME_LIST_EVENT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlTimeListEvent(
    GMEVENT *event, int *seq, void *work );


//--------------------------------------------------------------
/// data
//--------------------------------------------------------------
/// ���ԊǗ��^�C�v��
enum{
  CONT_TIME_TYPE_NORMAL,
  CONT_TIME_TYPE_8HOUR,
  CONT_TIME_TYPE_12HOUR,
  CONT_TIME_TYPE_18HOUR,
  CONT_TIME_TYPE_22HOUR,
  CONT_TIME_TYPE_2HOUR,
  CONT_TIME_TYPE_600RATE,
  CONT_TIME_TYPE_60RATE,

  CONT_TIME_TYPE_NUM,
} ;


///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ContTimeList =
{
  1,    //���X�g���ڐ�
  CONT_TIME_TYPE_NUM,   //�\���ő區�ڐ�
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

///�e�X�g�J�������j���[���X�g
static const FLDMENUFUNC_LIST DATA_ControlTimeMenuList[CONT_TIME_TYPE_NUM] =
{
  { DEBUG_FIELD_STR30, (void*)CONT_TIME_TYPE_NORMAL,  },
  { DEBUG_FIELD_STR23, (void*)CONT_TIME_TYPE_8HOUR,   },
  { DEBUG_FIELD_STR24, (void*)CONT_TIME_TYPE_12HOUR,  },
  { DEBUG_FIELD_STR25, (void*)CONT_TIME_TYPE_18HOUR,  },
  { DEBUG_FIELD_STR26, (void*)CONT_TIME_TYPE_22HOUR,  },
  { DEBUG_FIELD_STR27, (void*)CONT_TIME_TYPE_2HOUR,   },
  { DEBUG_FIELD_STR28, (void*)CONT_TIME_TYPE_600RATE, },
  { DEBUG_FIELD_STR29, (void*)CONT_TIME_TYPE_60RATE,  },
};

static const DEBUG_MENU_INITIALIZER DebugControlTimeMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_ControlTimeMenuList),
  DATA_ControlTimeMenuList,
  &DATA_DebugMenuList_ContTimeList,
  1, 1, 9, 13,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�e�X�g�J�������X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuControlTimeListEvent, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���ԑ��샊�X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlTimeListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugControlTimeMenuListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }

      if( ret != FLDMENUFUNC_CANCEL ){  //����
        switch( ret ){
        case CONT_TIME_TYPE_NORMAL:
          GFL_RTC_DEBUG_StopFakeTime();
          break;
        case CONT_TIME_TYPE_8HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 8,0 );
        case CONT_TIME_TYPE_12HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 12,0 );
          break;
        case CONT_TIME_TYPE_18HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 18,0 );
          break;
        case CONT_TIME_TYPE_22HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 22,0 );
          break;
        case CONT_TIME_TYPE_2HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 2,0 );
          break;
        case CONT_TIME_TYPE_600RATE:
          GFL_RTC_DEBUG_StartFakeTime( 600 );
          break;
        case CONT_TIME_TYPE_60RATE:
          GFL_RTC_DEBUG_StartFakeTime( 60 );
          break;
        default:
          break;
        }
      }else{

        FLDMENUFUNC_DeleteMenu( work->menuFunc );


        // �I����
        return( GMEVENT_RES_FINISH );
      }
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[�@�g���
//======================================================================
//--------------------------------------------------------------
/**
 * �g���C�x���g�Ăяo��
 * @param wk DEBUG_MENU_EVENT_WORK*
 * @retval BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk )
{
  if( FIELDMAP_GetMapControlType(wk->fieldWork) != FLDMAP_CTRLTYPE_GRID )
  {
    // ���[���̂Ƃ�
    FIELD_PLAYER* p_fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );
    if( FIELD_PLAYER_GetMoveForm( p_fld_player ) != PLAYER_MOVE_FORM_SWIM ){
      FIELD_PLAYER_SetNaminori( p_fld_player );
    }else{
      FIELD_PLAYER_SetNaminoriEnd( p_fld_player );
    }
    return( FALSE );
  }

  FIELD_EVENT_ChangeNaminoriStart( wk->gmEvent, wk->gmSys, wk->fieldWork );
  return( TRUE );
}

//======================================================================
//  �f�o�b�O���j���[ �|�P�����쐬
//======================================================================
#include "debug/debug_makepoke.h"
FS_EXTERN_OVERLAY(debug_makepoke);
static GMEVENT_RESULT debugMenuMakePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// �f�o�b�O�|�P�����쐬�p���[�N
//=====================================
typedef struct
{
  HEAPID heapID;
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  PROCPARAM_DEBUG_MAKEPOKE p_mp_work;
  POKEMON_PARAM *pp;
} DEBUG_MAKEPOKE_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����쐬
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��  FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMakePoke( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_MAKEPOKE_EVENT_WORK *p_mp_work;

  //�C���F���g
  GMEVENT_Change( p_event, debugMenuMakePoke, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );
  p_mp_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_mp_work, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );

  //���[�N�ݒ�
  p_mp_work->p_gamesys  = p_gamesys;
  p_mp_work->p_event    = p_event;
  p_mp_work->p_field    = p_field;
  p_mp_work->heapID     = heapID;
  p_mp_work->pp = PP_Create( 1,1,PTL_SETUP_ID_AUTO,p_mp_work->heapID );
  p_mp_work->p_mp_work.dst = p_mp_work->pp;
  p_mp_work->p_mp_work.oyaStatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_gamesys) );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�|�P�����쐬�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuMakePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_MAKEPOKE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(debug_makepoke), &ProcData_DebugMakePoke, &p_wk->p_mp_work ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( p_wk->pp != NULL )
    {
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(p_wk->p_gamesys);
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);

      {
        u16 oyaName[6] = {L'��',L'��',L'��',L'��',L'��',0xFFFF};
        PP_Put( p_wk->pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
        PP_Put( p_wk->pp , ID_PARA_oyasex , PTL_SEX_MALE );
      }
      //�莝���ɋ󂫂�����Γ����
      if( PokeParty_GetPokeCount( party ) < 6 )
      {
        PokeParty_Add( party , p_wk->pp );
      }

      GFL_HEAP_FreeMemory( p_wk->pp );
    }
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}


//-------------------------------------
/// �f�o�b�O�|�P�����쐬�p���[�N
//=====================================
typedef struct
{
  HEAPID heapID;
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  PROCPARAM_DEBUG_MAKEPOKE p_mp_work;
  POKEMON_PARAM *pp;
  POKEMON_PARAM *p_src_pp;
} DEBUG_REWRITEPOKE_EVENT_WORK;
static GMEVENT_RESULT debugMenuReWritePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  �|�P������������
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��  FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugReWritePoke( DEBUG_MENU_EVENT_WORK *p_wk )
{ 
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_REWRITEPOKE_EVENT_WORK *p_ev_wk;

  //�C���F���g
  GMEVENT_Change( p_event, debugMenuReWritePoke, sizeof(DEBUG_REWRITEPOKE_EVENT_WORK) );
  p_ev_wk = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_ev_wk, sizeof(DEBUG_REWRITEPOKE_EVENT_WORK) );

  //���[�N�ݒ�
  p_ev_wk->p_gamesys  = p_gamesys;
  p_ev_wk->p_event    = p_event;
  p_ev_wk->p_field    = p_field;
  p_ev_wk->heapID     = heapID;
  p_ev_wk->p_src_pp   = PP_Create( 1, 1, 1, heapID );
  {
    POKEPARTY *p_party  = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(p_gamesys) );
    p_ev_wk->pp =  PokeParty_GetMemberPointer( p_party, 0 );

    POKETOOL_CopyPPtoPP( p_ev_wk->pp, p_ev_wk->p_src_pp );
  }
  p_ev_wk->p_mp_work.dst  = p_ev_wk->pp;
  p_ev_wk->p_mp_work.oyaStatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_gamesys) );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�|�P�������������C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuReWritePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_REWRITEPOKE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(debug_makepoke), &ProcData_DebugMakePoke, &p_wk->p_mp_work ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    {
      //���������Ȃ��f�[�^
      static const u16 sc_id_tbl[]  =
      { 
       ID_PARA_pref_code,
       ID_PARA_get_cassette,
       ID_PARA_get_year,
       ID_PARA_get_month,
       ID_PARA_get_day,
       ID_PARA_birth_year,
       ID_PARA_birth_month,
       ID_PARA_birth_day,
       ID_PARA_get_place,
       ID_PARA_birth_place,
       ID_PARA_pokerus,
       ID_PARA_get_ball,
       ID_PARA_get_level,
       ID_PARA_oyasex,
       ID_PARA_get_ground_id,
       ID_PARA_country_code,
       ID_PARA_style,                    //�������悳
       ID_PARA_beautiful,                //��������
       ID_PARA_cute,                     //���킢��
       ID_PARA_clever,                   //��������
       ID_PARA_strong,                   //�����܂���
       ID_PARA_fur,                      //�щ�
       ID_PARA_sinou_champ_ribbon,       //�V���I�E�`�����v���{��
       ID_PARA_sinou_battle_tower_ttwin_first,     //�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
       ID_PARA_sinou_battle_tower_ttwin_second,    //�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
       ID_PARA_sinou_battle_tower_2vs2_win50,      //�V���I�E�o�g���^���[�^���[�_�u��50�A��
       ID_PARA_sinou_battle_tower_aimulti_win50,   //�V���I�E�o�g���^���[�^���[AI�}���`50�A��
       ID_PARA_sinou_battle_tower_siomulti_win50,  //�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
       ID_PARA_sinou_battle_tower_wifi_rank5,      //�V���I�E�o�g���^���[Wifi�����N�T����
       ID_PARA_sinou_syakki_ribbon,        //�V���I�E����������{��
       ID_PARA_sinou_dokki_ribbon,         //�V���I�E�ǂ������{��
       ID_PARA_sinou_syonbo_ribbon,        //�V���I�E�����ڃ��{��
       ID_PARA_sinou_ukka_ribbon,          //�V���I�E���������{��
       ID_PARA_sinou_sukki_ribbon,         //�V���I�E���������{��
       ID_PARA_sinou_gussu_ribbon,         //�V���I�E���������{��
       ID_PARA_sinou_nikko_ribbon,         //�V���I�E�ɂ������{��
       ID_PARA_sinou_gorgeous_ribbon,      //�V���I�E�S�[�W���X���{��
       ID_PARA_sinou_royal_ribbon,         //�V���I�E���C�������{��
       ID_PARA_sinou_gorgeousroyal_ribbon, //�V���I�E�S�[�W���X���C�������{��
       ID_PARA_sinou_ashiato_ribbon,       //�V���I�E�������ƃ��{��
       ID_PARA_sinou_record_ribbon,        //�V���I�E���R�[�h���{��
       ID_PARA_sinou_history_ribbon,       //�V���I�E�q�X�g���[���{��
       ID_PARA_sinou_legend_ribbon,        //�V���I�E���W�F���h���{��
       ID_PARA_sinou_red_ribbon,           //�V���I�E���b�h���{��
       ID_PARA_sinou_green_ribbon,         //�V���I�E�O���[�����{��
       ID_PARA_sinou_blue_ribbon,          //�V���I�E�u���[���{��
       ID_PARA_sinou_festival_ribbon,      //�V���I�E�t�F�X�e�B�o�����{��
       ID_PARA_sinou_carnival_ribbon,      //�V���I�E�J�[�j�o�����{��
       ID_PARA_sinou_classic_ribbon,       //�V���I�E�N���V�b�N���{��
       ID_PARA_sinou_premiere_ribbon,      //�V���I�E�v���~�A���{��
       ID_PARA_stylemedal_normal,          //�������悳�M��(�m�[�}��)AGB�R���e�X�g
       ID_PARA_stylemedal_super,         //�������悳�M��(�X�[�p�[)AGB�R���e�X�g
       ID_PARA_stylemedal_hyper,         //�������悳�M��(�n�C�p�[)AGB�R���e�X�g
       ID_PARA_stylemedal_master,          //�������悳�M��(�}�X�^�[)AGB�R���e�X�g
       ID_PARA_beautifulmedal_normal,        //���������M��(�m�[�}��)AGB�R���e�X�g
       ID_PARA_beautifulmedal_super,       //���������M��(�X�[�p�[)AGB�R���e�X�g
       ID_PARA_beautifulmedal_hyper,       //���������M��(�n�C�p�[)AGB�R���e�X�g
       ID_PARA_beautifulmedal_master,        //���������M��(�}�X�^�[)AGB�R���e�X�g
       ID_PARA_cutemedal_normal,         //���킢���M��(�m�[�}��)AGB�R���e�X�g
       ID_PARA_cutemedal_super,          //���킢���M��(�X�[�p�[)AGB�R���e�X�g
       ID_PARA_cutemedal_hyper,          //���킢���M��(�n�C�p�[)AGB�R���e�X�g
       ID_PARA_cutemedal_master,         //���킢���M��(�}�X�^�[)AGB�R���e�X�g
       ID_PARA_clevermedal_normal,         //���������M��(�m�[�}��)AGB�R���e�X�g
       ID_PARA_clevermedal_super,          //���������M��(�X�[�p�[)AGB�R���e�X�g
       ID_PARA_clevermedal_hyper,          //���������M��(�n�C�p�[)AGB�R���e�X�g
       ID_PARA_clevermedal_master,         //���������M��(�}�X�^�[)AGB�R���e�X�g
       ID_PARA_strongmedal_normal,         //�����܂����M��(�m�[�}��)AGB�R���e�X�g
       ID_PARA_strongmedal_super,          //�����܂����M��(�X�[�p�[)AGB�R���e�X�g
       ID_PARA_strongmedal_hyper,          //�����܂����M��(�n�C�p�[)AGB�R���e�X�g
       ID_PARA_strongmedal_master,         //�����܂����M��(�}�X�^�[)AGB�R���e�X�g
       ID_PARA_champ_ribbon,           //�`�����v���{��
       ID_PARA_winning_ribbon,           //�E�B�j���O���{��
       ID_PARA_victory_ribbon,           //�r�N�g���[���{��
       ID_PARA_bromide_ribbon,           //�u���}�C�h���{��
       ID_PARA_ganba_ribbon,           //����΃��{��
       ID_PARA_marine_ribbon,            //�}�������{��
       ID_PARA_land_ribbon,            //�����h���{��
       ID_PARA_sky_ribbon,             //�X�J�C���{��
       ID_PARA_country_ribbon,           //�J���g���[���{��
       ID_PARA_national_ribbon,          //�i�V���i�����{��
       ID_PARA_earth_ribbon,           //�A�[�X���{��
       ID_PARA_condition,              //�R���f�B�V����
      };
      int i;
      int id;
      //���������ė~�����Ȃ��f�[�^������

      for( i = 0; i < NELEMS(sc_id_tbl); i++ )
      { 
        id  = sc_id_tbl[i];
        PP_Put( p_wk->pp, id, PP_Get( p_wk->p_src_pp, id, NULL ) );
      }
    }
    GFL_HEAP_FreeMemory( p_wk->p_src_pp );
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}


//======================================================================
//  �f�o�b�O���j���[ �A�C�e��
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�A�C�e���쐬
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugItemMake, NULL );
  //child_event = EVENT_DebugItemMake(gameSys, wk->heapID);
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�B����A�C�e���쐬
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSecretItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugSecretItemMake, NULL );
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   �f�o�b�OPDW�A�C�e���쐬
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugPDWItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugPDWItemMake, NULL );
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����{�b�N�X�Ƀ|�P�����ł����ς��ɂ���
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------

static BOOL debugMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk )
{
#if 1
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  MYSTATUS *myStatus;
  POKEMON_PARAM *pp;
  POKEMON_PASO_PARAM  *ppp;
  const STRCODE *name;

  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gameSys));
  name = MyStatus_GetMyName( myStatus );

  pp = PP_Create(100, 100, 123456, HEAPID_FIELDMAP);

  {
    int i,j,k=1;
		int	monsno=1;
    BOX_MANAGER* pBox = GAMEDATA_GetBoxManager(GAMESYSTEM_GetGameData(gameSys));

    if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
      k=BOX_MAX_TRAY;
    }
		BOXDAT_AddTrayMax( pBox );
		BOXDAT_AddTrayMax( pBox );
    for(i=0;i < k;i++){
      for(j=0;j < 30;j++){
        OS_TPrintf("%d  %d %d�쐬\n",monsno, i, j);
        PP_Setup(pp,  monsno , 100, MyStatus_GetID( myStatus ));

        ppp = (POKEMON_PASO_PARAM  *)PP_GetPPPPointerConst( pp );
        PPP_Put( ppp , ID_PARA_oyaname_raw , (u32)name );
        PPP_Put( ppp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );

        BOXDAT_PutPokemonBox(pBox, i, ppp);

				monsno++;
				if( monsno > MONSNO_END ){
					monsno = 1;
				}
      }
    }
  }

  GFL_HEAP_FreeMemory(pp);
#endif
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�ɃA�C�e�������E�܂ł����
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------

static BOOL debugMenuCallProc_MyItemMax( DEBUG_MENU_EVENT_WORK *wk )
{
  u32 i;
  MYITEM_PTR myitem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(wk->gmSys));

  for( i = 0; i < ITEM_DATA_MAX; i++ ){
    if( ITEM_CheckEnable( i ) ){
      MYITEM_AddItem(myitem , i, 2, GFL_HEAPID_APP );
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief   �s���`�F�b�N��ʂ�|�P�������o�g��BOX�փZ�b�g����
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_SetBtlBox( DEBUG_MENU_EVENT_WORK *wk )
{
#if 1
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));
  MYSTATUS *myStatus;
  POKEMON_PASO_PARAM  *ppp;
  const STRCODE *name;
  POKEPARTY *party = PokeParty_AllocPartyWork( HEAPID_FIELDMAP );
  BATTLE_BOX_SAVE* btlbox = BATTLE_BOX_SAVE_GetBattleBoxSave( sv );
  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gameSys));
  name = MyStatus_GetMyName( myStatus );
  BATTLE_BOX_SAVE_InitWork(btlbox);



  { 
    int i,j;

    static const u16 monsno_tbl1[] =
    { 
      MONSNO_KORATTA,
      MONSNO_OTATI,
      MONSNO_POTIENA,
      MONSNO_BIPPA,
      MONSNO_PHI,
      MONSNO_ZIGUZAGUMA,
    };
    static const u16 waza_tbl1[] =
    { 
      WAZANO_TAIATARI,
      WAZANO_HIKKAKU,
      WAZANO_TAIATARI,
      WAZANO_TAIATARI,
      WAZANO_HATAKU,
      WAZANO_TAIATARI,
    };
    static const u16 monsno_tbl2[] =
    { 
      MONSNO_KAIRIKII,
      MONSNO_OKORIZARU,
      MONSNO_GOUKAZARU,
      MONSNO_HARITEYAMA,
      MONSNO_HERAKUROSU,
      MONSNO_RUKARIO,
    };
    static const u16 waza_tbl2[] =
    { 
      WAZANO_BAKURETUPANTI,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
    };
    u16 poke_name[6] =
      L"�f�o�O�|�P";

    const u16 *p_monsno;
    const u16 *p_waza;
    u32 pow = 0;

    poke_name[ 5 ]  = 0xFFFF;


    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    { 
      p_monsno  = monsno_tbl1;
      p_waza    =waza_tbl1;
      pow       =0;
    }
    else
    { 
      p_monsno  = monsno_tbl2;
      p_waza    =waza_tbl2;
      pow       =252;
    }


    for(i=0;i < PokeParty_GetPokeCountMax(party);i++){
      POKEMON_PARAM *p_temp = PP_Create( p_monsno[ i ], 100, 0, HEAPID_PROC ); 
      PP_Put( p_temp, ID_PARA_waza1, p_waza[ i ] );
      PP_Put( p_temp, ID_PARA_waza2, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_waza3, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_waza4, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_oyaname_raw, (u32)poke_name );
      PP_Put( p_temp, ID_PARA_pow_exp, pow );

      PP_Renew( p_temp );
      PokeParty_Add( party, p_temp );

      GFL_HEAP_FreeMemory( p_temp );
    }
    BATTLE_BOX_SAVE_SetPokeParty( btlbox, party );

  }
  
  GFL_HEAP_FreeMemory(party);
#endif
  return( FALSE );
}
//======================================================================
//  �f�o�b�O���j���[ ��l�����O�ݒ�
//======================================================================
FS_EXTERN_OVERLAY(namein);
//-------------------------------------
/// �f�o�b�O��l�����ύX
//=====================================
typedef struct
{
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  NAMEIN_PARAM   *p_param;
  STRBUF          *p_default_str;
} DEBUG_CHANGENAME_EVENT_WORK;
//-------------------------------------
///   PROTOTYPE
//=====================================
static GMEVENT_RESULT debugEvnetChangeName( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//--------------------------------------------------------------
/**
 * @brief   ��l�����O���Đݒ肷��
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ChangeName( DEBUG_MENU_EVENT_WORK *p_wk )
{ 
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  DEBUG_CHANGENAME_EVENT_WORK  *p_ev_wk;
  GAMEDATA  *p_gamedata = GAMESYSTEM_GetGameData(p_gamesys);
  MYSTATUS  *p_mystatus = GAMEDATA_GetMyStatus( p_gamedata );
  MISC      *p_misc     = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(p_gamedata) );

  //�C���F���g
  GMEVENT_Change( p_event, debugEvnetChangeName, sizeof(DEBUG_CHANGENAME_EVENT_WORK) );
  p_ev_wk = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_ev_wk, sizeof(DEBUG_CHANGENAME_EVENT_WORK) );

  //���[�N�ݒ�
  p_ev_wk->p_gamesys  = p_gamesys;
  p_ev_wk->p_event    = p_event;
  p_ev_wk->p_field    = p_field;

  //�f�t�H���g��
  p_ev_wk->p_default_str  = MyStatus_CreateNameString(p_mystatus, HEAPID_PROC);

  //���O���̓��[�N�ݒ�
  p_ev_wk->p_param  = NAMEIN_AllocParam( HEAPID_PROC, NAMEIN_MYNAME, MyStatus_GetMySex(p_mystatus), 0, NAMEIN_PERSON_LENGTH, p_ev_wk->p_default_str, NULL );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ��l�����O�Đݒ�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugEvnetChangeName( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
    SEQ_EXIT,
  };

  DEBUG_CHANGENAME_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(namein), &NameInputProcData, p_wk->p_param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( !NAMEIN_IsCancel( p_wk->p_param ) )
    { 
      MYSTATUS      *p_mystatus  = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_wk->p_gamesys) );
      NAMEIN_CopyStr( p_wk->p_param, p_wk->p_default_str );
      MyStatus_SetMyNameFromString(p_mystatus, p_wk->p_default_str );
    }

    *p_seq  = SEQ_EXIT;
    break;


  case SEQ_EXIT:
    NAMEIN_FreeParam( p_wk->p_param );
    GFL_STR_DeleteBuffer( p_wk->p_default_str );

    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}



//======================================================================
//  �f�o�b�O���j���[ �������
//======================================================================
FS_EXTERN_OVERLAY(townmap);
//-------------------------------------
/// �f�o�b�O����ԗp���[�N
//=====================================
typedef struct
{
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  TOWNMAP_PARAM   *p_param;
} DEBUG_SKYJUMP_EVENT_WORK;
//-------------------------------------
///   PROTOTYPE
//=====================================
static GMEVENT_RESULT debugMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�����
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��  FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  DEBUG_SKYJUMP_EVENT_WORK  *p_sky;

  //�C���F���g
  GMEVENT_Change( p_event, debugMenuSkyJump, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );
  p_sky = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_sky, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );

  //���[�N�ݒ�
  p_sky->p_gamesys  = p_gamesys;
  p_sky->p_event    = p_event;
  p_sky->p_field    = p_field;

  //�^�E���}�b�v���[�N�ݒ�
  p_sky->p_param  = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(TOWNMAP_PARAM) );
  GFL_STD_MemClear( p_sky->p_param, sizeof(TOWNMAP_PARAM) );
  p_sky->p_param->mode      = TOWNMAP_MODE_DEBUGSKY;
  {
    GAMEDATA    *p_gamedata;
    PLAYER_WORK *p_player;
    p_gamedata  = GAMESYSTEM_GetGameData( p_sky->p_gamesys );
    p_player    = GAMEDATA_GetMyPlayerWork( p_gamedata );
    p_sky->p_param->zoneID    = PLAYERWORK_getZoneID(p_player);
    p_sky->p_param->escapeID  = GAMEDATA_GetEscapeLocation( p_gamedata )->zone_id;
    p_sky->p_param->p_gamesys = p_gamesys;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O����ԃC�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
    SEQ_CHANGE_MAP,
    SEQ_EXIT,
  };

  DEBUG_SKYJUMP_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(townmap), &TownMap_ProcData, p_wk->p_param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( p_wk->p_param->select == TOWNMAP_SELECT_SKY )
    {
      *p_seq  = SEQ_CHANGE_MAP;
    }
    else
    {
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_CHANGE_MAP:
    NAGI_Printf("�]�[��ID %d",p_wk->p_param->zoneID);
    GMEVENT_CallEvent( p_wk->p_event, DEBUG_EVENT_QuickChangeMapDefaultPos( p_wk->p_gamesys, p_wk->p_field, p_wk->p_param->zoneID ) );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    if( p_wk->p_param )
    {
      GFL_HEAP_FreeMemory(p_wk->p_param);
    }
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  �f�o�b�O���j���[�@���@�j���؂�ւ�
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���@�j���؂�ւ�
 * @param wk DEBUG_MENU_EVENT_WORK
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ChangePlayerSex( DEBUG_MENU_EVENT_WORK *wk )
{
  int sex;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldMap = wk->fieldWork;
  u16 zone_id = FIELDMAP_GetZoneID( fieldMap );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( wk->gdata );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( wk->gdata );
  MYSTATUS *s_mystatus = SaveData_GetMyStatus( save );

  sex = MyStatus_GetMySex( mystatus );
  sex ^= 1;
  MyStatus_SetMySex( mystatus, sex );
  MyStatus_SetMySex( s_mystatus, sex );

  {
    GMEVENT * mapchange_event;
    mapchange_event = DEBUG_EVENT_ChangeMapDefaultPos( gsys, fieldMap, zone_id );
    GMEVENT_ChangeEvent( event, mapchange_event );
  }
  return( TRUE );
}

//======================================================================
//  �f�o�b�O���j���[�@GTS��ʂ�
//======================================================================
FS_EXTERN_OVERLAY(worldtrade);
#include "net/dwc_rapcommon.h"
//----------------------------------------------------------------------------
/**
 *  @brief  GTS��ʂւ����C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WifiGts( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_Gts( p_wk->gmSys, p_wk->fieldWork, TRUE ) );
  return TRUE;
}


//======================================================================
//  �f�o�b�O���j���[�@GDS�ڑ�
//======================================================================
#include "net_app/gds_main.h"
FS_EXTERN_OVERLAY(gds_comm);
//-------------------------------------
/// �f�o�b�OGDS�p���[�N
//=====================================
typedef struct
{
  GAMESYS_WORK        *gsys;
  FIELDMAP_WORK     *fieldWork;
} DEBUG_GDS_EVENT_WORK;
static GMEVENT_RESULT debugMenuGDS( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  GTS��ʂւ����C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_GDS( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK *gsys = p_wk->gmSys;
  FIELDMAP_WORK *fieldWork = p_wk->fieldWork;
  GAMEDATA *p_gamedata      = GAMESYSTEM_GetGameData(gsys);
  GMEVENT       *p_event    = p_wk->gmEvent;
  DEBUG_GDS_EVENT_WORK *p_gds;

  if( 1)//WifiList_CheckMyGSID(GAMEDATA_GetWiFiList(p_gamedata) ) )
  {
    //�C���F���g
    GMEVENT_Change( p_event, debugMenuGDS, sizeof(DEBUG_GDS_EVENT_WORK) );
    p_gds = GMEVENT_GetEventWork( p_event );
    p_gds->gsys = gsys;
    p_gds->fieldWork = fieldWork;
  }
  else
  {
    OS_Printf( "GameSpyID���s���Ȃ̂ŁAGTS���J�n���Ȃ�����\n" );
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�OGTS�ڑ��p�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuGDS( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_GDS_EVENT_WORK  *p_gds  = p_wk_adrs;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(p_gds->gsys);
  
  switch(*p_seq )
  {
  case 0:
    if(GameCommSys_BootCheck(game_comm) != GAME_COMM_STATUS_NULL){
      GameCommSys_ExitReq(game_comm);
    }
    (*p_seq)++;
    break;
  case 1:
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_STATUS_NULL){
      (*p_seq)++;
    }
    break;
  case 2:
    {
      GDSPROC_PARAM *gds_param;

      gds_param = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(GDSPROC_PARAM));
      gds_param->gamedata = GAMESYSTEM_GetGameData(p_gds->gsys);
      gds_param->gds_mode = BR_MODE_GLOBAL_MUSICAL;//BR_MODE_GLOBAL_BV;

      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
      { 
        gds_param->gds_mode = BR_MODE_GLOBAL_BV;
      }

      GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_gds->gsys, p_gds->fieldWork,
          NO_OVERLAY_ID, &GdsMainProcData, gds_param ) );
      (*p_seq)++;
    }
    break;

  case 3:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  �f�o�b�O���j���[ UI�e���v���[�g��
//======================================================================
FS_EXTERN_OVERLAY(ui_debug);
//-------------------------------------
/// �f�o�b�OUI�e���v���[�g�p���[�N
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  GMEVENT             *p_event;
  FIELDMAP_WORK       *p_field;
  UI_TEMPLATE_PARAM   param;
} DEBUG_UITEMPLATE_EVENT_WORK;
static GMEVENT_RESULT debugMenuUITemplate( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  UI�e���v���[�g�ڑ�
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_UITemplate( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field    = p_wk->fieldWork;
  DEBUG_UITEMPLATE_EVENT_WORK *p_param;

  //�C���F���g
  GMEVENT_Change( p_event, debugMenuUITemplate, sizeof(DEBUG_UITEMPLATE_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_UITEMPLATE_EVENT_WORK) );

  p_param->p_gamesys  = p_gamesys;
  p_param->param.gamesys  = p_gamesys;
  p_param->p_event    = p_event;
  p_param->p_field    = p_field;

  OS_Printf( "UI�e���v���[�g Start\n" );

  return TRUE;

}
//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�OGTS�ڑ��p�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuUITemplate( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_EXIT,
  };

  DEBUG_UITEMPLATE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
      GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(ui_debug), &UITemplateProcData, &p_wk->param ) );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  ����
//======================================================================
//--------------------------------------------------------------
/**
 * ���͎��s
 * @param wk DEBUG_MENU_EVENT_WORK*
 * @retval BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Kairiki( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( wk->gdata );
  EVENTWORK_SetEventFlag( evwork, SYS_FLAG_KAIRIKI );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@���`�J��������
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlLinerCamera( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTL_LINERCAMERA_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlLinerCamera, sizeof(DEBUG_CTL_LINERCAMERA_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTL_LINERCAMERA_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  {
    FIELD_SUBSCREEN_WORK * subscreen;


    // �J��������͉���ʂōs��
    subscreen = FIELDMAP_GetFieldSubscreenWork(work->fieldWork);
    FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
    {
      void * inner_work;
      FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);
      inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
      FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS, heapID);

      //�J�����̃o�C���h��؂�
      FIELD_CAMERA_FreeTarget(cam);
    }
  }
  // ���[���J�������f�̒�~
  {
    FLDNOGRID_MAPPER* mapper;

    if( FIELDMAP_GetBaseSystemType( work->fieldWork ) == FLDMAP_BASESYS_RAIL )
    {
      mapper = FIELDMAP_GetFldNoGridMapper( work->fieldWork );
      FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    }
  }

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���`�J��������
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLinerCamera(
    GMEVENT *event, int *seq, void *wk )
{
  VecFx32 add = {0,0,0};
  BOOL shift = FALSE;

  DEBUG_CTL_LINERCAMERA_WORK *work = wk;
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);


  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
    FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

    // ���[���J�������f�̍ĊJ
    {
      FLDNOGRID_MAPPER* mapper;

      if( FIELDMAP_GetBaseSystemType( work->fieldWork ) == FLDMAP_BASESYS_RAIL )
      {
        mapper = FIELDMAP_GetFldNoGridMapper( work->fieldWork );
        FLDNOGRID_MAPPER_SetRailCameraActive( mapper, TRUE );
      }
    }
    //�o�C���h���A
    FIELD_CAMERA_BindDefaultTarget(cam);
    return( GMEVENT_RES_FINISH );
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG ){
    DampCameraInfo(cam);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){             //��
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.z);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){             //��O
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.z);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){             //��
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.x);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){             //�E
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.x);
  }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){             //��
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.y);
  }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){             //��
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.y);
  }else{
    work->Wait = 0;
    work->Count = 0;
  }
  if(shift){
    VecFx32 pos;
    FIELD_CAMERA_GetTargetPos( cam, &pos );
    VEC_Add( &pos, &add, &pos );
    FIELD_CAMERA_SetTargetPos( cam, &pos );
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���`�J��������L�[����
 * @param work      �J�������[�N
 * @param inAddval  �C���͌��ʂ̍ۂ̉��Z�l
 * @param outVal    �i�[�o�b�t�@
 * @retval  BOOL    ���Z������TRUE
 */
//--------------------------------------------------------------
static BOOL LinerCamKeyContCtl(DEBUG_CTL_LINERCAMERA_WORK *work, const fx32 inAddVal, fx32 *outVal)
{
  if (work->Wait >= LINER_CAM_KEY_WAIT){
    *outVal = inAddVal;
    return TRUE;
  }else{
    work->Wait++;
    if (work->Count>0){
      work->Count--;
    }else{
      *outVal = inAddVal;
      work->Count = LINER_CAM_KEY_COUNT;
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���`�J�������� ���_���v
 * @param cam   �t�B�[���h�J�����|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
static void DampCameraInfo(FIELD_CAMERA * cam)
{
  u16 pitch;
  u16 yaw;
  fx32 len;
  VecFx32 target;
  u16 fovy;
  VecFx32 shift;

  OS_Printf("--DUMP_FIELD_CAMERA_PARAM--\n");

  pitch = FIELD_CAMERA_GetAnglePitch(cam);
  yaw = FIELD_CAMERA_GetAngleYaw(cam);
  len = FIELD_CAMERA_GetAngleLen(cam);
  fovy = FIELD_CAMERA_GetFovy(cam);
  FIELD_CAMERA_GetTargetOffset( cam, &shift );
  FIELD_CAMERA_GetTargetPos( cam, &target );

  OS_Printf("%d,%d,%d,%d,%d,%d\n", pitch, yaw, len, target.x, target.y, target.z );
#ifdef DEBUG_ONLY_FOR_saitou
  OS_Printf("%d,%d,%d,%d\n", fovy, shift.x, shift.y, shift.z );
#endif
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@Y�{�^���o�^����
 * @param wk  DEBUG_MENU_EVENT_WORK*  ���[�N
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenu_ControlShortCut( DEBUG_MENU_EVENT_WORK *wk )
{
  int i;
  GAMESYS_WORK *gsys = wk->gmSys;
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(wk->gdata);
  SHORTCUT *p_shortcut_sv = SaveData_GetShortCut( p_sv );
  for( i = 0; i < SHORTCUT_ID_MAX; i++ )
  {
    SHORTCUT_SetRegister( p_shortcut_sv, i, TRUE );
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[ �~���[�W�J���V���b�g�쐬
 * @param wk  DEBUG_MENU_EVENT_WORK*  ���[�N
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
FS_EXTERN_OVERLAY( gds_debug );
static BOOL debugMenu_CreateMusicalShotData( DEBUG_MENU_EVENT_WORK *wk )
{
  int i;
  GAMESYS_WORK *gsys = wk->gmSys;
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(wk->gdata);
  MUSICAL_SAVE *musSave   = MUSICAL_SAVE_GetMusicalSave( p_sv );

  GFL_OVERLAY_Load( FS_OVERLAY_ID(gds_debug) );

  MUSICAL_DEBUG_CreateDummyData( MUSICAL_SAVE_GetMusicalShotData( musSave ), MONSNO_PIKATYUU , HEAPID_PROC );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(gds_debug));

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �r�[�R���ł̗F�B�R�[�h�z�M
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BeaconFriendCode( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * new_event;
  new_event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_beacon ), EVENT_DebugBeacon, wk->fieldWork );
  //new_event = EVENT_DebugBeacon( wk->gmSys, wk->fieldWork );
  GMEVENT_ChangeEvent( wk->gmEvent, new_event );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFi���E�ΐ�֐ڑ�
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WifiBattleMatch( DEBUG_MENU_EVENT_WORK *wk )
{
  WIFIBATTLEMATCH_MODE mode = WIFIBATTLEMATCH_MODE_RANDOM;
  WIFIBATTLEMATCH_BTLRULE btl_rule  = WIFIBATTLEMATCH_BTLRULE_SINGLE;
#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L && GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_ROTATE;
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_DOUBLE;
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_TRIPLE;
  }
#endif

  GMEVENT_ChangeEvent( wk->gmEvent, EVENT_WifiBattleMatch( wk->gmSys, wk->fieldWork, DEBUG_MODE(mode), WIFIBATTLEMATCH_POKE_BTLBOX, btl_rule ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �T�P�T�[�o�[�f�o�b�O���[�h��
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ���[�N
 *
 *  @return TRUE�ŃC�x��Čp��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSake( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_DEBUG_WifiMatch( p_wk->gmSys, p_wk->fieldWork, DEBUG_WIFIBATTLEMATCH_MODE_SAKE ) );
  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�g���X�T�[�o�[�f�o�b�O���[�h��
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ���[�N
 *
 *  @return TRUE�ŃC�x��Čp��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugAtlas( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_DEBUG_WifiMatch( p_wk->gmSys, p_wk->fieldWork, DEBUG_WIFIBATTLEMATCH_MODE_ATLAS ) );
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �f�o�b�O���j���[�F�W�I�l�b�g�Ăяo��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Geonet( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_Geonet( p_wk->gmSys ) );
  return TRUE;
}
//======================================================================
//  �o�g�����R�[�_�[
//======================================================================
#include "net_app/battle_recorder.h"
FS_EXTERN_OVERLAY( battle_recorder );

static GMEVENT_RESULT debugBattleRecorder( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// �o�g�����R�[�_�[PROC�C�x���g���[�N
//=====================================
typedef struct
{
  HEAPID heapID;
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK   *p_field;
  BATTLERECORDER_PARAM  param;
} DEBUG_BR_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[��
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ���[�N
 *
 *  @return TRUE�ŃC�x��Čp��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BattleRecorder( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_BR_EVENT_WORK *p_param;

  //�C���F���g
  GMEVENT_Change( p_event, debugBattleRecorder, sizeof(DEBUG_BR_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_BR_EVENT_WORK) );

  //���[�N�ݒ�
  p_param->p_gamesys  = p_gamesys;
  p_param->p_event    = p_event;
  p_param->p_field    = p_field;
  p_param->heapID     = heapID;
  p_param->param.p_gamedata = GAMESYSTEM_GetGameData(p_gamesys);
  p_param->param.mode       = BR_MODE_BROWSE;

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    p_param->param.mode  = BR_MODE_GLOBAL_BV;
  }
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    p_param->param.mode  = BR_MODE_GLOBAL_MUSICAL;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[�Ăяo���C�x���g
 *
 *  @param  GMEVENT *p_event  �C�x���g
 *  @param  *p_seq      �V�[�P���X
 *  @param  *p_wk_adrs  ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugBattleRecorder( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_BR_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(battle_recorder), &BattleRecorder_ProcData, &p_wk->param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//-----------------------------------------------------------------------------
/**
 *    �O���t�B�b�J�[�p
 *      �J�����@���܂����ݒ�
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// �f�o�b�O�J�����@���܂����ݒ�p���[�N
//=====================================
typedef struct
{
  FIELDMAP_WORK*  p_field;
  FIELD_CAMERA*   p_camera;
  HEAPID heapID;

  BOOL draw_help;
  BOOL param_change;

  GFL_BMPWIN* p_win;
} DEBUG_DELICATE_CAMERA_EVENT_WORK;
static GMEVENT_RESULT debugMenuDelicateCamera( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//----------------------------------------------------------------------------
/**
 *  @brief  �N��
 *
 *  @param  wk
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_ControlDelicateCamera( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_DELICATE_CAMERA_EVENT_WORK* p_work;
  GMEVENT* p_event = wk->gmEvent;
  FIELDMAP_WORK* p_fieldWork = wk->fieldWork;
  HEAPID heapID = wk->heapID;

  GMEVENT_Change( p_event, debugMenuDelicateCamera, sizeof(DEBUG_DELICATE_CAMERA_EVENT_WORK) );
  p_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_work, sizeof(DEBUG_DELICATE_CAMERA_EVENT_WORK) );

  p_work->p_field   = p_fieldWork;
  p_work->p_camera  = FIELDMAP_GetFieldCamera( p_fieldWork );
  p_work->heapID    = heapID;

  FIELD_CAMERA_DEBUG_InitControl( p_work->p_camera, heapID );

  // ���[���J�������f�̒�~
  {
    FLDNOGRID_MAPPER* mapper;

    if( FIELDMAP_GetBaseSystemType( p_fieldWork ) == FLDMAP_BASESYS_RAIL )
    {
      mapper = FIELDMAP_GetFldNoGridMapper( p_fieldWork );
      FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    }
  }


  // �\���ʂ̍쐬
  {
    const FLDMAPPER* cp_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldWork );
    fx32 map_size_x, map_size_z;

    FLDMAPPER_GetSize( cp_mapper, &map_size_x, &map_size_z );

    // �C���t�H�[�o�[�̔�\��
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(p_fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // �r�b�g�}�b�v�E�B���h�E������
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // �p���b�g����]��
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, heapID );

      // �r�b�g�}�b�v�E�B���h�E���쐬
      p_work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( p_work->p_win );
      GFL_BMPWIN_TransVramCharacter( p_work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // �E�B���h�E
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, heapID );
      BmpWinFrame_Write( p_work->p_win, TRUE, 1, 15 );

      FIELD_CAMERA_DEBUG_DrawInfo( p_work->p_camera, p_work->p_win, map_size_x, map_size_z );
    }
  }

  return( TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�����@�ڍב���@���C��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuDelicateCamera( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_DELICATE_CAMERA_EVENT_WORK* p_work = p_wk_adrs;
  BOOL result;
  int trg = GFL_UI_KEY_GetTrg();
  int cont = GFL_UI_KEY_GetCont();
  int repeat = GFL_UI_KEY_GetRepeat();
  const FLDMAPPER* cp_mapper = FIELDMAP_GetFieldG3Dmapper( p_work->p_field );
  fx32 map_size_x, map_size_z;

  FLDMAPPER_GetSize( cp_mapper, &map_size_x, &map_size_z );

  // SELECT�I��
  if( trg & PAD_BUTTON_SELECT )
  {
    // �r�b�g�}�b�v�E�B���h�E�j��
    {
      BmpWinFrame_Clear( p_work->p_win, TRUE );
      GFL_BMPWIN_Delete( p_work->p_win );
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
    }

    FIELD_CAMERA_DEBUG_ExitControl( p_work->p_camera );

    // �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(p_work->p_field,
        FIELD_SUBSCREEN_Init( p_work->heapID, p_work->p_field, FIELD_SUBSCREEN_NORMAL ));
    return ( GMEVENT_RES_FINISH );
  }

  // DEBUG�ŁA�X�N���v�g����p�̕\�����o���B
  // �o�b�t�@�����O���[�h���ς���Ă��܂��܂����E�E�E�B
  if( trg & PAD_BUTTON_DEBUG )
  {
    VecFx32 offset;
    VecFx32 target;
    u16 pitch;
    u16 yaw;
    fx32 len;

    // ���`�J�����p�\���p�����[�^���J�����ɐݒ�
    FIELD_CAMERA_GetTargetOffset( p_work->p_camera, &offset );
    FIELD_CAMERA_GetTargetPos( p_work->p_camera, &target );
    pitch = FIELD_CAMERA_GetAnglePitch( p_work->p_camera );
    yaw   = FIELD_CAMERA_GetAngleYaw( p_work->p_camera );
    len   = FIELD_CAMERA_GetAngleLen( p_work->p_camera );
    FIELD_CAMERA_DEBUG_SetUpLinerCameraInfoData( p_work->p_camera );

    // ���s�ړ����܂߂����W��Ԃ��B
    DampCameraInfo( p_work->p_camera );

    // ���ɖ߂�
    FIELD_CAMERA_SetTargetPos( p_work->p_camera, &target );
    FIELD_CAMERA_SetTargetOffset( p_work->p_camera, &offset );
    FIELD_CAMERA_SetAnglePitch( p_work->p_camera, pitch );
    FIELD_CAMERA_SetAngleYaw( p_work->p_camera, yaw );
    FIELD_CAMERA_SetAngleLen( p_work->p_camera, len );
  }


  // �J�����f�o�b�N����
  result = FIELD_CAMERA_DEBUG_Control( p_work->p_camera, trg, cont, repeat );

  // �^�b�`�Ńw���v�\���ؑ�
  if( GFL_UI_TP_GetTrg() )
  {
    p_work->draw_help ^= 1;
    result = TRUE;
  }

  // �w���v�\��
  if( result )
  {
    p_work->param_change = TRUE;
  }
  // �`�悪�d���̂ŁA���삪�������Ă���`��X�V
  else if( p_work->param_change )
  {
    p_work->param_change = FALSE;
    if( p_work->draw_help )
    {
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      FIELD_CAMERA_DEBUG_DrawControlHelp( p_work->p_camera, p_work->p_win );
    }
    else
    {
      // �J���������X�V
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      FIELD_CAMERA_DEBUG_DrawInfo( p_work->p_camera, p_work->p_win, map_size_x, map_size_z );
    }
  }


  return( GMEVENT_RES_CONTINUE );
}




//======================================================================
//  �Z�v���o����ʃe�X�g�Ăяo��
//======================================================================
FS_EXTERN_OVERLAY(waza_oshie);
//-------------------------------------
/// �Z�v���o����ʃe�X�g�p���[�N
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  GMEVENT             *p_event;
  FIELDMAP_WORK       *p_field;
  WAZAOSHIE_DATA      param;
} DEBUG_WAZAOSHIE_EVENT_WORK;
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//----------------------------------------------------------------------------
/**
 *  @brief  �Z�v���o�����
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WazaOshie( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field    = p_wk->fieldWork;
  DEBUG_WAZAOSHIE_EVENT_WORK  *p_param;
  GAMEDATA *gmData = GAMESYSTEM_GetGameData(p_gamesys);
  POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gmData);

  // �C�x���g�N��
  GMEVENT_Change( p_event, debugMenuWazaOshie, sizeof(DEBUG_WAZAOSHIE_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_WAZAOSHIE_EVENT_WORK) );

  p_param->p_gamesys     = p_gamesys;
  p_param->p_event       = p_event;
  p_param->p_field       = p_field;

//  p_param->param.gsys    = p_gamesys;
  p_param->param.pp   = PokeParty_GetMemberPointer( party, 0 );
  p_param->param.myst = SaveData_GetMyStatus(sv);   // �����f�[�^
  p_param->param.cfg  = SaveData_GetConfig(sv);     // �R���t�B�O�f�[�^
  p_param->param.gsys = p_gamesys;
  p_param->param.waza_tbl   = WAZAOSHIE_GetRemaindWaza( p_param->param.pp, HEAPID_WORLD );

  OS_Printf( "�Z���� Start\n" );

  return TRUE;

}
//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�Z�v���o����ʐڑ��p�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_EXIT,
  };

  DEBUG_WAZAOSHIE_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:

    GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
                                                    FS_OVERLAY_ID(waza_oshie), &WazaOshieProcData, &p_wk->param ) );

    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}


//======================================================================
//  �g�p�������̕\���i��v�ȁj
//======================================================================
//-------------------------------------
/// �g�p�������̕\���i��v�ȁj
//=====================================
typedef struct
{
  FIELDMAP_WORK       *p_field;
  GFL_BMPWIN* p_win;
  HEAPID heapID;

  // MESSAGE�f�[�^
  WORDSET*    p_debug_wordset;
  GFL_FONT*   p_debug_font;
  GFL_MSGDATA*  p_debug_msgdata;
  STRBUF*     p_debug_strbuff;
  STRBUF*     p_debug_strbuff_tmp;
} DEBUG_USEMEMORY_EVENT_WORK;
static GMEVENT_RESULT debugMenuUseMemoryDump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
static void debugMenuWriteUseMemoryDump( DEBUG_USEMEMORY_EVENT_WORK* p_wk );


//----------------------------------------------------------------------------
/**
 *  @brief  �������g�p���\��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_UseMemoryDump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DEBUG_USEMEMORY_EVENT_WORK* p_work;
  GMEVENT* p_event = p_wk->gmEvent;
  FIELDMAP_WORK* p_fieldWork = p_wk->fieldWork;
  HEAPID heapID = p_wk->heapID;

  GMEVENT_Change( p_event, debugMenuUseMemoryDump, sizeof(DEBUG_USEMEMORY_EVENT_WORK) );
  p_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_work, sizeof(DEBUG_USEMEMORY_EVENT_WORK) );

  p_work->p_field   = p_fieldWork;
  p_work->heapID    = heapID;

  // MESSAGE���ǂݍ���
  {
    // ���[�h�Z�b�g�쐬
    p_work->p_debug_wordset = WORDSET_Create( heapID );
    p_work->p_debug_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_field_dat, heapID );

    p_work->p_debug_strbuff    = GFL_STR_CreateBuffer( 256, heapID );
    p_work->p_debug_strbuff_tmp  = GFL_STR_CreateBuffer( 256, heapID );

    // �t�H���g�f�[�^
    p_work->p_debug_font = GFL_FONT_Create(
      ARCID_FONT, NARC_font_large_gftr,
      GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  }


  // �\���ʂ̍쐬
  {
    // �C���t�H�[�o�[�̔�\��
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(p_fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // �r�b�g�}�b�v�E�B���h�E������
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // �p���b�g����]��
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, heapID );

      // �r�b�g�}�b�v�E�B���h�E���쐬
      p_work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( p_work->p_win );
      GFL_BMPWIN_TransVramCharacter( p_work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // �E�B���h�E
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, heapID );
      BmpWinFrame_Write( p_work->p_win, TRUE, 1, 15 );
    }

    // �`�揈��
    {
      debugMenuWriteUseMemoryDump( p_work );
    }
  }

  return( TRUE );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �������[�_���v���C��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuUseMemoryDump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_USEMEMORY_EVENT_WORK* p_work = p_wk_adrs;

  // �\��OFF
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    // �S�j��
    {
      BmpWinFrame_Clear( p_work->p_win, TRUE );
      GFL_BMPWIN_Delete( p_work->p_win );
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
    }

    // MESSAGE���j��
    {
      // �t�H���g�f�[�^
      GFL_FONT_Delete( p_work->p_debug_font );
      p_work->p_debug_font = NULL;

      GFL_MSG_Delete( p_work->p_debug_msgdata );
      p_work->p_debug_msgdata = NULL;

      WORDSET_Delete( p_work->p_debug_wordset );
      p_work->p_debug_wordset = NULL;

      GFL_STR_DeleteBuffer( p_work->p_debug_strbuff );
      p_work->p_debug_strbuff = NULL;
      GFL_STR_DeleteBuffer( p_work->p_debug_strbuff_tmp );
      p_work->p_debug_strbuff_tmp = NULL;
    }
    // �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(p_work->p_field,
        FIELD_SUBSCREEN_Init( p_work->heapID, p_work->p_field, FIELD_SUBSCREEN_NORMAL ));

    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������[�_���v�\��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void debugMenuWriteUseMemoryDump( DEBUG_USEMEMORY_EVENT_WORK* p_wk )
{
  /* 1�i�� */
  // �z�u�I�u�W�F�N�g
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, FIELD_BMODEL_MAN_GetUseResourceMemorySize(), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#else // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, 0, 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#endif // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  // ����I�u�W�F
#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, DEBUG_MMDL_GetUseResourceMemorySize(), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#else // DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, 0, 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#endif  // DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  // FIELD�q�[�v�̎c��
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 4, GFI_HEAP_GetHeapFreeSize(p_wk->heapID), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 5, GFI_HEAP_GetHeapAllocatableSize(p_wk->heapID), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  // �\��
  GFL_MSG_GetString( p_wk->p_debug_msgdata, DEBUG_FIELD_STR54, p_wk->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_wk->p_win ), 0, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );
  


  /* 2�i�� */
  // HEAPID FIELD_PRBUF
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 0, GFI_HEAP_GetHeapFreeSize(HEAPID_FIELD_PRBUF), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELD_PRBUF), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  // HEAPID SUBSCREEN
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 2, GFI_HEAP_GetHeapFreeSize(HEAPID_FIELD_SUBSCREEN), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELD_SUBSCREEN), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  
  // �\��
  GFL_MSG_GetString( p_wk->p_debug_msgdata, DEBUG_FIELD_STR68, p_wk->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_wk->p_win ), 96, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  

  GFL_BMPWIN_TransVramCharacter( p_wk->p_win );
}


//----------------------------------------------------------------------------
/**
 * @brief �t�F�[�h���x�ύX
 */
//----------------------------------------------------------------------------
static BOOL debugMenuCallProc_FadeSpeedChange( DEBUG_MENU_EVENT_WORK * wk )
{
  if (GFL_FADE_GetFadeSpeed() != 2)
  {
    GFL_FADE_SetFadeSpeed( 2 );
  } else {
    GFL_FADE_SetDefaultFadeSpeed();
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �G�ߕ\��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_SeasonDisplay( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*         parent = wk->gmEvent;
  GAMESYS_WORK*      gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );

  GMEVENT_ChangeEvent( parent, DEBUG_EVENT_FLDMENU_SeasonDispSelect( gsys, heap_id ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���Ȃʂ��̃q��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Ananukenohimo( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByAnanukenohimo( fieldmap, gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���Ȃ��ق�
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Anawohoru( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByAnawohoru( gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �e���|�[�g
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Teleport( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByTeleport( gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�Z�F�_�C�r���O
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Diving( DEBUG_MENU_EVENT_WORK *wk )
{
  if( FIELDMAP_GetMapControlType(wk->fieldWork) != FLDMAP_CTRLTYPE_GRID )
  {
    FIELD_PLAYER* p_fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );

    // ���[����ł͂�����
    FIELD_PLAYER_SetRequest( p_fld_player, FIELD_PLAYER_REQBIT_DIVING );
    return FALSE;
  }
  SCRIPT_ChangeScript( wk->gmEvent, SCRID_HIDEN_DIVING_MENU, NULL, HEAPID_FIELDMAP );
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief 3D�f���Đ�
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Demo3d( DEBUG_MENU_EVENT_WORK* wk )
{
  GMEVENT*         parent = wk->gmEvent;
  GAMESYS_WORK*      gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );

  GMEVENT_ChangeEvent( parent, DEBUG_EVENT_FLDMENU_Demo3DSelect( gsys, heap_id ) );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * �����Z�[�u
 *
 * @param   wk
 *
 * @retval  BOOL    TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ForceSave( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GMEVENT *event;
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
  GFL_MSGDATA *msgdata;
  FLDMSGWIN *fldmsgwin;

  msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_field_dat, heapID );

  fldmsgwin = FLDMSGWIN_Add(msgBG, msgdata, 1, 19, 30, 4);

  FLDMSGWIN_Print( fldmsgwin, 0, 0, DEBUG_FIELD_STR60_message );
  FLDMSGBG_AllPrint(msgBG);

  //�Z�[�u
  GAMEDATA_Save(GAMESYSTEM_GetGameData(gsys));

  FLDMSGWIN_Delete(fldmsgwin);
  GFL_MSG_Delete(msgdata);

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���M��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_RingTone( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GAMESYS_WORK * gsys = p_wk->gmSys;
  GAMEDATA* gamedata  = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND * fsnd  = GAMEDATA_GetFieldSound( gamedata );
  
  if ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE3 ) == FALSE ) {
    FSND_RequestTVTRingTone( fsnd );
  } else {
    FSND_StopTVTRingTone( fsnd );
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �p�\�R��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_PCScript( DEBUG_MENU_EVENT_WORK *wk )
{
  SCRIPT_ChangeScript( wk->gmEvent, SCRID_PC, NULL, HEAPID_FIELDMAP );
  return TRUE;
}

//======================================================================
//  �f�o�b�O���j���[�@�L���v�`��
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CAPTURELIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  int select_mode;
}DEBUG_CAPTURELIST_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuCaptureListEvent(
    GMEVENT *event, int *seq, void *work );

enum{
  CAPTURE_MODE_NORMAL,
  CAPTURE_MODE_PLAYER_OFF,
  CAPTURE_MODE_NPC_OFF,
};

///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_CaptureList =
{
  1,    //���X�g���ڐ�
  3,    //�\���ő區�ڐ�
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

///�Ă񂫃��j���[���X�g
static const FLDMENUFUNC_LIST DATA_CaptureMenuList[] =
{
  { DEBUG_FIELD_STR61_000, (void*)CAPTURE_MODE_NORMAL },
  { DEBUG_FIELD_STR61_001, (void*)CAPTURE_MODE_PLAYER_OFF },
  { DEBUG_FIELD_STR61_002, (void*)CAPTURE_MODE_NPC_OFF },
};

static const DEBUG_MENU_INITIALIZER DebugCaptureMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_CaptureMenuList),
  DATA_CaptureMenuList,
  &DATA_DebugMenuList_CaptureList,
  1, 1, 12, 11,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�V�C���X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_CaptureList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_CAPTURELIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuCaptureListEvent, sizeof(DEBUG_CAPTURELIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CAPTURELIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�Ă񂫃��X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuCaptureListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CAPTURELIST_EVENT_WORK *work = wk;
  MMDL *fmmdl;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugCaptureMenuListData );
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

      if( ret == FLDMENUFUNC_CANCEL ){  //����
        return GMEVENT_RES_FINISH;
      }

      work->select_mode = ret;
      (*seq)++;
    }
    break;
  case 2:
    switch(work->select_mode){
    case CAPTURE_MODE_NORMAL:
      break;
    case CAPTURE_MODE_PLAYER_OFF:
      {
        FIELD_PLAYER *player = FIELDMAP_GetFieldPlayer( work->fieldWork );
        fmmdl = FIELD_PLAYER_GetMMdl( player );
        MMDL_SetStatusBitVanish( fmmdl, TRUE );
      }
      break;
    case CAPTURE_MODE_NPC_OFF:
      {
        u32 no = 0;
        BOOL ret;
        MMDLSYS *fos = FIELDMAP_GetMMdlSys( work->fieldWork );

        do{
          ret = MMDLSYS_SearchUseMMdl(fos, &fmmdl, &no );
          if(ret == TRUE){
            MMDL_SetStatusBitVanish( fmmdl, TRUE );
          }
        }while(ret == TRUE);
      }
      break;
    }
    (*seq)++;
    break;
  case 3:
    MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
    GX_SetCapture(
        GX_CAPTURE_SIZE_256x192,      // �L���v�`���T�C�Y
        GX_CAPTURE_MODE_A,      // �L���v�`�����[�h
        GX_CAPTURE_SRCA_3D,       // �L���v�`���u�����hA
        GX_CAPTURE_SRCB_VRAM_0x00000,       // �L���v�`���u�����hB
        GX_CAPTURE_DEST_VRAM_D_0x00000,     // �]��Vram
        0,            // �u�����h�W��A
        0);           // �u�����h�W��B

    //LCDC�Ƀ������}�b�s���O
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
  //  GX_SetGraphicsMode(GX_DISPMODE_VRAM_C, GX_BGMODE_0, GX_BG0_AS_3D);

    return GMEVENT_RES_FINISH;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * @brief   �ړ��|�P�����f�o�b�O
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMvPokemon( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event;

  event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
    FS_OVERLAY_ID( d_iwasawa ), EVENT_DebugMovePoke, wk );

  GMEVENT_ChangeEvent( wk->gmEvent, event );

  return( TRUE );
}



//-----------------------------------------------------------------------------
// �r���{�[�h�J���[�@�̒���
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
/// DEBUG_CTLIGHT_WORK ���C�g���샏�[�N
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;
}DEBUG_BBDCOLOR_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlBbdColor(
    GMEVENT *event, int *seq, void *wk );

//----------------------------------------------------------------------------
/**
 *  @brief  �r���{�[�h�J���[���w��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BBDColor( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_BBDCOLOR_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlBbdColor, sizeof(DEBUG_BBDCOLOR_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_BBDCOLOR_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


static GMEVENT_RESULT debugMenuControlBbdColor(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_BBDCOLOR_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    {
      HEAPID heapID = FIELDMAP_GetHeapID( work->fieldWork );
      FLD_BBD_COLOR* p_color = FLD_BBD_COLOR_Create( heapID );
      GFL_BBDACT_SYS* p_bbd_act = FIELDMAP_GetBbdActSys( work->fieldWork );
      GFL_BBD_SYS* p_bbd_sys = GFL_BBDACT_GetBBDSystem( p_bbd_act );
      AREADATA* p_areadata = FIELDMAP_GetAreaData( work->fieldWork );

      FLD_BBD_COLOR_Load( p_color, AREADATA_GetBBDColor(p_areadata) );

      // ���f���Ǘ��J�n
      FLD_BBD_COLOR_DEBUG_Init( p_bbd_sys, p_color, work->heapID );

      FLD_BBD_COLOR_Delete( p_color );
    }

    // �C���t�H�[�o�[�̔�\��
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // �r�b�g�}�b�v�E�B���h�E������
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // �p���b�g����]��
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // �r�b�g�}�b�v�E�B���h�E���쐬
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // �E�B���h�E
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FLD_BBD_COLOR_DEBUG_PrintData( work->p_win );

    (*seq)++;
  case 1:
    // ���C�g�Ǘ����C��
    FLD_BBD_COLOR_DEBUG_Control();
    FLD_BBD_COLOR_DEBUG_PrintData( work->p_win );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:
    FLD_BBD_COLOR_DEBUG_Exit();

    // �r�b�g�}�b�v�E�B���h�E�j��
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    // �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  �f�o�b�O���j���[�@�G���J�E���g�G�t�F�N�g���X�g
//======================================================================
//--------------------------------------------------------------
/// DEBUG_ENCEFF_LIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_ENCEFF_LIST_EVENT_WORK;

///���X�g�ő�
#define ENCEFFLISTMAX (36)

///���X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_EncEffList =
{
  1,    //���X�g���ڐ�
  6,    //�\���ő區�ڐ�
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

///���j���[���X�g
static const FLDMENUFUNC_LIST DATA_EncEffMenuList[ENCEFFLISTMAX] =
{
  { DEBUG_FIELD_ENCEFF01, (void*)0 },
  { DEBUG_FIELD_ENCEFF02, (void*)1 },
  { DEBUG_FIELD_ENCEFF03, (void*)2 },
  { DEBUG_FIELD_ENCEFF04, (void*)3 },
  { DEBUG_FIELD_ENCEFF05, (void*)4 },
  { DEBUG_FIELD_ENCEFF06, (void*)5 },
  { DEBUG_FIELD_ENCEFF07, (void*)6 },
  { DEBUG_FIELD_ENCEFF08, (void*)7 },
  { DEBUG_FIELD_ENCEFF09, (void*)8 },
  { DEBUG_FIELD_ENCEFF10, (void*)9 },
  { DEBUG_FIELD_ENCEFF11, (void*)10 },
  { DEBUG_FIELD_ENCEFF12, (void*)11 },
  { DEBUG_FIELD_ENCEFF13, (void*)12 },
  { DEBUG_FIELD_ENCEFF14, (void*)13 },
  { DEBUG_FIELD_ENCEFF15, (void*)14 },
  { DEBUG_FIELD_ENCEFF16, (void*)15 },
  { DEBUG_FIELD_ENCEFF17, (void*)16 },
  { DEBUG_FIELD_ENCEFF18, (void*)17 },
  { DEBUG_FIELD_ENCEFF19, (void*)18 },
  { DEBUG_FIELD_ENCEFF20, (void*)19 },
  { DEBUG_FIELD_ENCEFF21, (void*)20 },
  { DEBUG_FIELD_ENCEFF22, (void*)21 },
  { DEBUG_FIELD_ENCEFF23, (void*)22 },
  { DEBUG_FIELD_ENCEFF24, (void*)23 },
  { DEBUG_FIELD_ENCEFF25, (void*)24 },
  { DEBUG_FIELD_ENCEFF26, (void*)25 },
  { DEBUG_FIELD_ENCEFF27, (void*)26 },
  { DEBUG_FIELD_ENCEFF28, (void*)27 },
  { DEBUG_FIELD_ENCEFF30, (void*)28 },
  { DEBUG_FIELD_ENCEFF31, (void*)29 },
  { DEBUG_FIELD_ENCEFF29, (void*)30 },
  { DEBUG_FIELD_ENCEFF32, (void*)31 },
  { DEBUG_FIELD_ENCEFF33, (void*)32 },
  { DEBUG_FIELD_ENCEFF34, (void*)33 },
  { DEBUG_FIELD_ENCEFF35, (void*)34 },
  { DEBUG_FIELD_ENCEFF35, (void*)35 },
};

static const DEBUG_MENU_INITIALIZER DebugEncEffMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_EncEffMenuList),
  DATA_EncEffMenuList,
  &DATA_DebugMenuList_EncEffList, //���p
  1, 1, 16, 11,
  NULL,
  NULL
};


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuEncEffListEvent( GMEVENT *event, int *seq, void *wk );


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�G���J�E���g�G�t�F�N�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_EncEffList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_ENCEFF_LIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuEncEffListEvent, sizeof(DEBUG_ENCEFF_LIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_ENCEFF_LIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�G���J�E���g�G�t�F�N�g���X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuEncEffListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ENCEFF_LIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugEncEffMenuListData );
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

      if( ret != FLDMENUFUNC_CANCEL ) //����
      {
        //�G���J�E���g�G�t�F�N�g�R�[��
        OS_Printf("enceff= %d\n",ret);
        ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(work->fieldWork), event, ret);
        (*seq)++;
        return( GMEVENT_RES_CONTINUE );
      }
      else return( GMEVENT_RES_FINISH );
    }
    break;
  case 2:
    {
      ENCEFF_CNT_PTR cnt_ptr = FIELDMAP_GetEncEffCntPtr(work->fieldWork);
      ENCEFF_FreeUserWork(cnt_ptr);
      //�I�[�o�[���C�A�����[�h
      ENCEFF_UnloadEffOverlay(cnt_ptr);
    }

    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, -1 );
    (*seq)++;
    break;
  case 3:
    if( GFL_FADE_CheckFade() == FALSE ){
      return( GMEVENT_RES_FINISH );
    }
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[ ���A�f�[�^�쐬
//======================================================================
#include "debug/debug_make_undata.h"
FS_EXTERN_OVERLAY(debug_make_undata);
static GMEVENT_RESULT debugMenuMakeUNData( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// �f�o�b�O���A�f�[�^�쐬�p���[�N
//=====================================
typedef struct
{
  HEAPID HeapID;
  GAMESYS_WORK    *gsys;
  GMEVENT         *Event;
  FIELDMAP_WORK *FieldWork;
  PROCPARAM_DEBUG_MAKE_UNDATA p_work;
} DEBUG_MAKE_UNDATA_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  ���A�f�[�^�쐬
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��  FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMakeUNData( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *gsys  = p_wk->gmSys;
  GMEVENT       *event    = p_wk->gmEvent;
  FIELDMAP_WORK *fieldWork  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_MAKE_UNDATA_EVENT_WORK *evt_work;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys));

  //�C�x���g�`�F���W
  GMEVENT_Change( event, debugMenuMakeUNData, sizeof(DEBUG_MAKE_UNDATA_EVENT_WORK) );
  evt_work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( evt_work, sizeof(DEBUG_MAKE_UNDATA_EVENT_WORK) );

  //���[�N�ݒ�
  evt_work->gsys = gsys;
  evt_work->Event = event;
  evt_work->FieldWork = fieldWork;
  evt_work->HeapID = heapID;
  evt_work->p_work.wh = SaveData_GetWifiHistory(pSave);

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O���A�f�[�^�쐬�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuMakeUNData( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_MAKE_UNDATA_EVENT_WORK *evt_work = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( evt_work->Event, EVENT_FieldSubProc( evt_work->gsys, evt_work->FieldWork,
        FS_OVERLAY_ID(debug_make_undata), &ProcData_DebugMakeUNData, &evt_work->p_work ) );
    *p_seq  = SEQ_PROC_END;
    break;
  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  �f�o�b�O���j���[�@�o�g���T�u�E�F�C
//======================================================================
#include "../../../resource/fldmapdata/script/bsubway_scr_def.h"

//--------------------------------------------------------------
/// DEBUG_BSUBWAY_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_BSUBWAY_EVENT_WORK;

static const FLDMENUFUNC_HEADER DATA_DebugMenuList_BSubway =
{
  1,    //���X�g���ڐ�
  6,    //�\���ő區�ڐ�
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

enum
{
  DEBUG_BSWAY_ZONE_HOME,
  DEBUG_BSWAY_ZONE_TRAIN,
  
  DEBUG_BSWAY_SCDATA_VIEW,
  DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD,
  DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD,
  DEBUG_BSWAY_WIFI_UPLOAD,
   
  DEBUG_BSWAY_ZONE_SINGLE,
  DEBUG_BSWAY_ZONE_DOUBLE,
  DEBUG_BSWAY_ZONE_MULTI,
  DEBUG_BSWAY_ZONE_WIFI,
  DEBUG_BSWAY_ZONE_S_SINGLE,
  DEBUG_BSWAY_ZONE_S_DOUBLE,
  DEBUG_BSWAY_ZONE_S_MULTI,
  DEBUG_BSWAY_AUTO_SINGLE,
  DEBUG_BSWAY_AUTO_DOUBLE,
  DEBUG_BSWAY_AUTO_MULTI,
  DEBUG_BSWAY_AUTO_S_SINGLE,
  DEBUG_BSWAY_AUTO_S_DOUBLE,
  DEBUG_BSWAY_AUTO_S_MULTI,
  DEBUG_BSWAY_BTL_SINGLE_7,
  DEBUG_BSWAY_BTL_DOUBLE_7,
  DEBUG_BSWAY_BTL_MULTI_7,
  DEBUG_BSWAY_BTL_SINGLE_21,
  DEBUG_BSWAY_BTL_DOUBLE_21,
  DEBUG_BSWAY_BTL_MULTI_21,
  DEBUG_BSWAY_BTL_S_SINGLE_48,
  DEBUG_BSWAY_BTL_S_DOUBLE_48,
  DEBUG_BSWAY_BTL_S_MULTI_48,

  DEBUG_BSWAY_SET_REGU_OFF,
  DEBUG_BSWAY_SET_BTL_SKIP,
};

static const FLDMENUFUNC_LIST DATA_BSubwayMenuList[] =
{
  { DEBUG_FIELD_BSW_01, (void*)DEBUG_BSWAY_ZONE_SINGLE},
  { DEBUG_FIELD_BSW_02, (void*)DEBUG_BSWAY_ZONE_DOUBLE},
  { DEBUG_FIELD_BSW_03, (void*)DEBUG_BSWAY_ZONE_MULTI},
  { DEBUG_FIELD_BSW_04, (void*)DEBUG_BSWAY_ZONE_WIFI},
  { DEBUG_FIELD_BSW_05, (void*)DEBUG_BSWAY_ZONE_S_SINGLE},
  { DEBUG_FIELD_BSW_06, (void*)DEBUG_BSWAY_ZONE_S_DOUBLE},
  { DEBUG_FIELD_BSW_07, (void*)DEBUG_BSWAY_ZONE_S_MULTI},
  { DEBUG_FIELD_BSW_08, (void*)DEBUG_BSWAY_ZONE_TRAIN},
  { DEBUG_FIELD_BSW_09, (void*)DEBUG_BSWAY_ZONE_HOME},
  
  { DEBUG_FIELD_BSW_10, (void*)DEBUG_BSWAY_SCDATA_VIEW },
  { DEBUG_FIELD_WIFI_BSW_01, (void*)DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD },
  { DEBUG_FIELD_WIFI_BSW_02, (void*)DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD },
  { DEBUG_FIELD_WIFI_BSW_03, (void*)DEBUG_BSWAY_WIFI_UPLOAD },
  
  { DEBUG_FIELD_BSW_26, (void*)DEBUG_BSWAY_SET_REGU_OFF},
  { DEBUG_FIELD_BSW_23, (void*)DEBUG_BSWAY_SET_BTL_SKIP},
   
  { DEBUG_FIELD_BSW_11, (void*)DEBUG_BSWAY_AUTO_SINGLE},
  { DEBUG_FIELD_BSW_12, (void*)DEBUG_BSWAY_AUTO_DOUBLE},
  { DEBUG_FIELD_BSW_13, (void*)DEBUG_BSWAY_AUTO_MULTI},
  { DEBUG_FIELD_BSW_14, (void*)DEBUG_BSWAY_AUTO_S_SINGLE},
  { DEBUG_FIELD_BSW_15, (void*)DEBUG_BSWAY_AUTO_S_DOUBLE},
  { DEBUG_FIELD_BSW_16, (void*)DEBUG_BSWAY_AUTO_S_MULTI},
  { DEBUG_FIELD_BSW_17, (void*)DEBUG_BSWAY_BTL_SINGLE_7},
  { DEBUG_FIELD_BSW_18, (void*)DEBUG_BSWAY_BTL_DOUBLE_7},
  { DEBUG_FIELD_BSW_19, (void*)DEBUG_BSWAY_BTL_MULTI_7},
  { DEBUG_FIELD_BSW_20, (void*)DEBUG_BSWAY_BTL_SINGLE_21},
  { DEBUG_FIELD_BSW_21, (void*)DEBUG_BSWAY_BTL_DOUBLE_21},
  { DEBUG_FIELD_BSW_22, (void*)DEBUG_BSWAY_BTL_MULTI_21},
  { DEBUG_FIELD_BSW_27, (void*)DEBUG_BSWAY_BTL_S_SINGLE_48},
  { DEBUG_FIELD_BSW_28, (void*)DEBUG_BSWAY_BTL_S_DOUBLE_48},
  { DEBUG_FIELD_BSW_29, (void*)DEBUG_BSWAY_BTL_S_MULTI_48},
};

#define DEBUG_BSUBWAY_LIST_MAX ( NELEMS(DATA_BSubwayMenuList) )

static const DEBUG_MENU_INITIALIZER DebugBSubwayMenuData = {
  NARC_message_d_field_dat,
  DEBUG_BSUBWAY_LIST_MAX,
  DATA_BSubwayMenuList,
  &DATA_DebugMenuList_BSubway,
  1, 1, 20, 12,
  NULL,
  NULL
};

static void debug_bsw_SetAuto( GAMESYS_WORK *gsys )
{
  u8 flag = BSUBWAY_SCRWORK_DebugGetFlag( gsys );
  flag |= BSW_DEBUG_FLAG_AUTO;
  BSUBWAY_SCRWORK_DebugSetFlag( gsys, flag );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�o�g���T�u�E�F�C�f�o�b�O���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuBSubwayEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_BSUBWAY_EVENT_WORK  *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init(
        work->fieldWork, work->heapID,  &DebugBSubwayMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret,param;
      u16 dummy_ret;
      GMEVENT *next_event = NULL;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }

      switch( ret ){
      case DEBUG_BSWAY_ZONE_HOME: //�z�[���ֈړ�
        param = ZONE_ID_C04R0111;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_TRAIN: //�ԓ��ֈړ�
        param = ZONE_ID_C04R0110;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_SCDATA_VIEW: //��ナ�[�_�[�\��
        next_event = BSUBWAY_EVENT_CallLeaderBoard( work->gmSys );
        break;
      case DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD: //wifi �f�[�^�_�E�����[�h
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_GAMEDATA_DOWNLOAD, &dummy_ret );
        break;
      case DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD:  //wif �����_�E�����[�h
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_SUCCESSDATA_DOWNLOAD, &dummy_ret );
        break;
      case DEBUG_BSWAY_WIFI_UPLOAD: //wifi �f�[�^�A�b�v���[�h
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_SCORE_UPLOAD, &dummy_ret );
        break;
      case DEBUG_BSWAY_ZONE_SINGLE: //�V���O����t��
        param = ZONE_ID_C04R0102;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_DOUBLE: //�_�u����t��
        param = ZONE_ID_C04R0104;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_MULTI: //�}���`��t��
        param = ZONE_ID_C04R0106;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_WIFI: //WiFi��t��
        param = ZONE_ID_C04R0108;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_S_SINGLE: //S�V���O����t��
        param = ZONE_ID_C04R0103;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_S_DOUBLE: //S�_�u����t��
        param = ZONE_ID_C04R0105;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_ZONE_S_MULTI: //S�}���`��t��
        param = ZONE_ID_C04R0107;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, param );
        break;
      case DEBUG_BSWAY_AUTO_SINGLE: //�V���O���I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_SINGLE );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_AUTO_DOUBLE: //�_�u���I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_DOUBLE );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_AUTO_MULTI: //�}���`�I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_MULTI );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_AUTO_S_SINGLE: //S�V���O���I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_SINGLE );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_AUTO_S_DOUBLE: //S�_�u���I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_DOUBLE );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_AUTO_S_MULTI: //S�}���`�I�[�g�퓬
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_MULTI );
        debug_bsw_SetAuto( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_SINGLE_7: //�V���O���V�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_SINGLE );
        BSUBWAY_SCRWORK_DebugFight7( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_DOUBLE_7: //�_�u���V�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_DOUBLE );
        BSUBWAY_SCRWORK_DebugFight7( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_MULTI_7: //�}���`�V�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_MULTI );
        BSUBWAY_SCRWORK_DebugFight7( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_SINGLE_21: //�V���O���Q�P�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_SINGLE );
        BSUBWAY_SCRWORK_DebugFight21( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_DOUBLE_21: //�_�u���Q�P�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_DOUBLE );
        BSUBWAY_SCRWORK_DebugFight21( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_MULTI_21: //�}���`�Q�P�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_MULTI );
        BSUBWAY_SCRWORK_DebugFight21( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_SET_REGU_OFF: //���M���I�t
        {
          u8 flag = BSUBWAY_SCRWORK_DebugGetFlag( work->gmSys );
          flag ^= BSW_DEBUG_FLAG_REGU_OFF;
          BSUBWAY_SCRWORK_DebugSetFlag( work->gmSys, flag );
        }
        break;
      case DEBUG_BSWAY_SET_BTL_SKIP: //�o�g���X�L�b�v
        {
          u8 flag = BSUBWAY_SCRWORK_DebugGetFlag( work->gmSys );
          flag ^= BSW_DEBUG_FLAG_BTL_SKIP;
          BSUBWAY_SCRWORK_DebugSetFlag( work->gmSys, flag );
        }
        break;
      case DEBUG_BSWAY_BTL_S_SINGLE_48: //�r�V���O���S�W�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_SINGLE );
        BSUBWAY_SCRWORK_DebugFight48( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_S_DOUBLE_48: //�r�_�u���S�W�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_DOUBLE );
        BSUBWAY_SCRWORK_DebugFight48( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      case DEBUG_BSWAY_BTL_S_MULTI_48: //�r�}���`�S�W�킩��
        BSUBWAY_SCRWORK_DebugCreateWork( work->gmSys, BSWAY_MODE_S_MULTI );
        BSUBWAY_SCRWORK_DebugFight48( work->gmSys );
        SCRIPT_ChangeScript(
            event, SCRID_BSW_DEBUG_MAP_CHG_TRAIN, NULL, HEAPID_PROC );
        return( GMEVENT_RES_CONTINUE );
      default:
        break;
      }
      
      if( next_event == NULL ){
        return( GMEVENT_RES_FINISH );
      }else{
        GMEVENT_CallEvent( event, next_event );
        (*seq)++;
      }
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_BSubway( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_WEATERLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuBSubwayEvent, sizeof(DEBUG_BSUBWAY_EVENT_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_BSUBWAY_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//======================================================================
//  �f�o�b�O���j���[�@G�p���[
//======================================================================
#include "gamesystem/g_power.h"
#include "msg/msg_d_matsu.h"
//--------------------------------------------------------------
/// DEBUG_GPOWER_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;

  POWER_CONV_DATA *powerdata;
}DEBUG_GPOWER_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuGPowerListEvent(
    GMEVENT *event, int *seq, void *work );

///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_GPowerList =
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
  16,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

static const DEBUG_MENU_INITIALIZER DebugGPowerMenuListData = {
  NARC_message_power_dat,
  NULL,
  NULL,
  &DATA_DebugMenuList_GPowerList,
  1, 1, 24, 19,
  DEBUG_SetMenuWorkGPower,
  DEBUG_GetGPowerMax,
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�V�C���X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_GPowerList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_GPOWER_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuGPowerListEvent, sizeof(DEBUG_GPOWER_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_GPOWER_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�FG�p���[���X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuGPowerListEvent(GMEVENT *event, int *seq, void *wk )
{
  DEBUG_GPOWER_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugGPowerMenuListData );
    work->powerdata = GPOWER_PowerData_LoadAlloc(work->heapID);
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      if(ret == FLDMENUFUNC_CANCEL){
        (*seq)++;
        break;
      }

      if(ret < GPOWER_ID_MAX){
        GPOWER_Set_OccurID(ret, work->powerdata, FALSE);
      }
      else{
        GPOWER_Clear_AllPower();
      }
    }
    break;
  case 2:
    FLDMENUFUNC_DeleteMenu( work->menuFunc );
    GPOWER_PowerData_Unload(work->powerdata);
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * G�p���[�pBMP_MENULIST_DATA�Z�b�g
 * @param list  �Z�b�g��BMP_MENULIST_DATA
 * @param heapID  ������o�b�t�@�m�ۗpHEAPID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkGPower(GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list,
  HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id;
  STRBUF *strBuf = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgDataDMatsu = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_matsu_dat );

  for( id = 0; id < GPOWER_ID_MAX; id++ ){
    GFL_MSG_GetString( msgData,  id, strBuf );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }
  GFL_MSG_GetString(pMsgDataDMatsu, DM_MSG_POWER_RESET, strBuf);
  FLDMENUFUNC_AddStringListData( list, strBuf, GPOWER_ID_MAX, heapID );

  GFL_MSG_Delete( pMsgDataDMatsu );
  GFL_HEAP_FreeMemory( strBuf );
}

//--------------------------------------------------------------
/**
 * @brief G�p���[�p���X�g�ő�l�擾�֐�
 * @param fieldmap
 * @return  �}�b�v�ő吔
 */
//--------------------------------------------------------------
static u16 DEBUG_GetGPowerMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return GPOWER_ID_MAX + 1; // +1 = �u�S�ă��Z�b�g�v
}

//======================================================================
//  �f�o�b�O���j���[�@�t�B�[���h�킴
//======================================================================
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuFieldSkillListEvent(GMEVENT *event, int *seq, void *wk );

static const FLDMENUFUNC_LIST DATA_SubFieldSkillList[] =
{
  { DEBUG_FIELD_STR31, debugMenuCallProc_Naminori },              //�g���
  { DEBUG_FIELD_STR45, debugMenuCallProc_Kairiki },               //����
  { DEBUG_FIELD_ANANUKENOHIMO, debugMenuCallProc_Ananukenohimo }, //���k�P�̕R
  { DEBUG_FIELD_ANAWOHORU, debugMenuCallProc_Anawohoru },         //�����@��
  { DEBUG_FIELD_TELEPORT, debugMenuCallProc_Teleport },           //�e���|�[�g
  { DEBUG_FIELD_FSKILL_01, debugMenuCallProc_Diving },            //�_�C�r���O
};

static const DEBUG_MENU_INITIALIZER DebugSubFieldSkillListSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubFieldSkillList),
  DATA_SubFieldSkillList,
  &DATA_DebugMenuList_Default, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�t�B�[���h�Z���X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_FieldSkillList( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  DEBUG_MENU_EVENT_WORK   temp  = *wk;
  DEBUG_MENU_EVENT_WORK   *work;

  GMEVENT_Change( event,
    debugMenuFieldSkillListEvent, sizeof(DEBUG_MENU_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MENU_EVENT_WORK) );

  *work  = temp;
  work->call_proc = NULL;

  work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubFieldSkillListSelectData );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�Z���X�g
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuFieldSkillListEvent(GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        (*seq)++;
      }else if( ret != FLDMENUFUNC_CANCEL ){  //����
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;

  case 2:
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( work->call_proc != NULL ){
        if( work->call_proc(work) == TRUE ){
          return( GMEVENT_RES_CONTINUE );
        }
      }

      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[�@�ӂ����Ȃ�������̍쐬
//======================================================================
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
#define MYSTERY_DLDATA_DEBUG_PRINT
static const FLDMENUFUNC_LIST DATA_SubMysteryCardMakeList[] =
{
  { DEBUG_FIELD_MYSTERY_01, debugMenuCallProc_MakeMysteryCardPoke },              //�|�P�����쐬
  { DEBUG_FIELD_MYSTERY_02, debugMenuCallProc_MakeMysteryCardItem },               //����쐬
  { DEBUG_FIELD_MYSTERY_03, debugMenuCallProc_MakeMysteryCardGPower },              //G�p���[�쐬
};

static const DEBUG_MENU_INITIALIZER DebugSubMysteryCardMakeData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubMysteryCardMakeList),
  DATA_SubMysteryCardMakeList,
  &DATA_DebugMenuList_Default, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};


static BOOL debugMenuCallProc_MakeMysteryCardList( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  DEBUG_MENU_EVENT_WORK   temp  = *wk;
  DEBUG_MENU_EVENT_WORK   *work;

  //�C�x���g���p
  GMEVENT_Change( event,
    debugMenuFieldSkillListEvent, sizeof(DEBUG_MENU_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MENU_EVENT_WORK) );

  *work  = temp;
  work->call_proc = NULL;

  work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubMysteryCardMakeData );

  return( TRUE );
}

static BOOL debugMenuCallProc_MakeMysteryCardPoke( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );

  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftPokeData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "�ӂ����ȃJ�[�h���Z�b�g���܂��� �C�x���gID=[%d]\n", i );
      break;
    }
  }


  return FALSE;
}
static BOOL debugMenuCallProc_MakeMysteryCardItem( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );
  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftItemData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "�ӂ����ȃJ�[�h���Z�b�g���܂��� �C�x���gID=[%d]\n", i );
      break;
    }
  }

  return FALSE;
}
static BOOL debugMenuCallProc_MakeMysteryCardGPower( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );
  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftGPowerData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "�ӂ����ȃJ�[�h���Z�b�g���܂��� �C�x���gID=[%d]\n", i );
      break;
    }
  }

  return FALSE;
}

//======================================================================
//
//    �}�b�v�S�`�F�b�N
//
//======================================================================

static GMEVENT * EVENT_DEBUG_AllMapCheck( GAMESYS_WORK * gsys, BOOL up_to_flag );
//--------------------------------------------------------------
/// �f�o�b�O���j���[�F�S�}�b�v�`�F�b�N
//--------------------------------------------------------------
static BOOL debugMenuCallProc_AllMapCheck( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT * new_event;
  new_event = EVENT_DEBUG_AllMapCheck( p_wk->gmSys, FALSE );
  GMEVENT_ChangeEvent( p_wk->gmEvent, new_event );
  return TRUE;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  int check_zone_id;
  BOOL up_to_flag;
  int wait;
  GAMESYS_WORK * gsys;
}ALL_MAP_CHECK_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static const u16 ng_zone_table[] = {
  ZONE_ID_UNION,
  ZONE_ID_CLOSSEUM,
  ZONE_ID_CLOSSEUM02,
  ZONE_ID_PLD10,
};

//#include "arc/fieldmap/map_matrix.naix"
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL checkNGZoneID( zone_id )
{
  int i;
#if 0
    if ( ZONEDATA_GetMatrixID(zone_id) == NARC_map_matrix_palacefield_mat_bin )
    {
      continue;
    }
#endif
  for ( i = 0; i < NELEMS(ng_zone_table); i++ )
  {
    if (ng_zone_table[i] == zone_id )
    {
      return TRUE;
    }
  }
  return FALSE;
}
static int getNextZoneID( ALL_MAP_CHECK_WORK* amcw )
{
  int zone_id = amcw->check_zone_id;
  while (TRUE)
  {
    if ( amcw->up_to_flag )
    {
      zone_id ++;
      if ( zone_id >= ZONE_ID_MAX ) break;
    } else {
      zone_id --;
      if ( zone_id < 0 ) break;
    }
    if ( checkNGZoneID( zone_id ) == TRUE ) continue;
    break;
  }
  amcw->check_zone_id = zone_id;
  return zone_id;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT_RESULT allMapCheckEvent( GMEVENT * event, int *seq, void * wk )
{
  ALL_MAP_CHECK_WORK * amcw = wk;
  FIELDMAP_WORK * fieldmap;
  u16 zone_id;

  enum {
    SEQ_INIT,
    SEQ_SEEK_ID,
    SEQ_WAIT
  };
  switch ( *seq )
  {
  case SEQ_INIT:
    if ( amcw->up_to_flag )
    {
      amcw->check_zone_id = -1;
    } else {
      amcw->check_zone_id = ZONE_ID_MAX;
    }
    *seq = SEQ_SEEK_ID;
    /* FALL THROUGH */

  case SEQ_SEEK_ID:
    zone_id = getNextZoneID( amcw );
    if ( zone_id >= ZONE_ID_MAX || zone_id < 0 )
    {
      return GMEVENT_RES_FINISH;
    }
    {
      char buf[ZONEDATA_NAME_LENGTH];
      ZONEDATA_DEBUG_GetZoneName(buf, zone_id);
      OS_TPrintf( "\t[[ALL MAP CHECKING... :%s]]!!\n", buf );
    }
    fieldmap = GAMESYSTEM_GetFieldMapWork( amcw->gsys );
    GMEVENT_CallEvent( event,
        DEBUG_EVENT_ChangeMapDefaultPos( amcw->gsys, fieldmap, zone_id ) );
    *seq = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    amcw->wait ++;
    if ( amcw->wait > 120 )
    {
      amcw->wait = 0;
      *seq = SEQ_SEEK_ID;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * EVENT_DEBUG_AllMapCheck( GAMESYS_WORK * gsys, BOOL up_to_flag )
{
  GMEVENT * new_event;
  ALL_MAP_CHECK_WORK * amcw;
  new_event = GMEVENT_Create( gsys, NULL, allMapCheckEvent, sizeof(ALL_MAP_CHECK_WORK) );
  amcw = GMEVENT_GetEventWork( new_event );
  amcw->up_to_flag = up_to_flag;
  amcw->gsys = gsys;
  return new_event;
}

//======================================================================
//  �f�o�b�O���j���[ �]�[���W�����v
//======================================================================
#include "debug/debug_zone_jump.h"
FS_EXTERN_OVERLAY(debug_zone_jump);
static GMEVENT_RESULT debugMenuZoneJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// �f�o�b�O���A�f�[�^�쐬�p���[�N
//=====================================
typedef struct
{
  HEAPID HeapID;
  GAMESYS_WORK    *gsys;
  GMEVENT         *Event;
  FIELDMAP_WORK *FieldWork;
  PROCPARAM_DEBUG_ZONE_JUMP p_work;
} DEBUG_ZONE_JUMP_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  �]�[���W�����v
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ���[�N
 *
 *  @return TRUE�C�x���g�p��  FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugZoneJump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *gsys  = p_wk->gmSys;
  GMEVENT       *event    = p_wk->gmEvent;
  FIELDMAP_WORK *fieldWork  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_ZONE_JUMP_EVENT_WORK *evt_work;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys));

  //�C�x���g�`�F���W
  GMEVENT_Change( event, debugMenuZoneJump, sizeof(DEBUG_ZONE_JUMP_EVENT_WORK) );
  evt_work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( evt_work, sizeof(DEBUG_ZONE_JUMP_EVENT_WORK) );

  //���[�N�ݒ�
  evt_work->gsys = gsys;
  evt_work->Event = event;
  evt_work->FieldWork = fieldWork;
  evt_work->HeapID = heapID;

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�]�[���W�����v�C�x���g
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            �V�[�P���X
 *  @param  *work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_RESULT,
    SEQ_PROC_END,
  };

  DEBUG_ZONE_JUMP_EVENT_WORK *evt_work = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( evt_work->Event, EVENT_FieldSubProc( evt_work->gsys, evt_work->FieldWork,
        FS_OVERLAY_ID(debug_zone_jump), &ProcData_DebugZoneJump, &evt_work->p_work ) );
    *p_seq  = SEQ_PROC_RESULT;
    break;
  case SEQ_PROC_RESULT:
    if ( evt_work->p_work.Ret )
    {
      GMEVENT * mapchange_event;
      OS_Printf("�]�[��ID�@%d �ɃW�����v\n", evt_work->p_work.ZoneID);
      mapchange_event = DEBUG_EVENT_QuickChangeMapDefaultPos( evt_work->gsys, evt_work->FieldWork, evt_work->p_work.ZoneID );
      GMEVENT_ChangeEvent( p_event, mapchange_event );
    }
    else *p_seq  = SEQ_PROC_END;
    break;
  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================================
//
//  �f�o�b�O���j���[�@�}��
//
//======================================================================================
//======================================================================
//======================================================================
typedef enum {
  DEBUG_ZKNCMD_GLOBAL_GET_FULL = 0,
  DEBUG_ZKNCMD_GLOBAL_SEE_FULL,
  DEBUG_ZKNCMD_RANDOM,
  DEBUG_ZKNCMD_FORM_FULL,
  DEBUG_ZKNCMD_ZENKOKU_FLAG,
  DEBUG_ZKNCMD_VERSION_UP,
  DEBUG_ZKNCMD_LOCAL_GET_FULL,
  DEBUG_ZKNCMD_LOCAL_SEE_FULL,   //7
  DEBUG_ZKNCMD_LANGUAGE_FULL,    //8
  DEBUG_ZKNCMD_LOCAL_SEE_25,
  DEBUG_ZKNCMD_LOCAL_SEE_60,
  DEBUG_ZKNCMD_LOCAL_SEE_120,
}DEBUG_ZKNCMD;

typedef struct {
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;
  ZUKAN_SAVEDATA * sv;
}DEBUG_ZUKAN_WORK;

///���X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER ZukanMenuHeader =
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

///���j���[���X�g
static const FLDMENUFUNC_LIST ZukanMenuList[] =
{
  { DEBUG_FIELD_ZUKAN_01, (void*)DEBUG_ZKNCMD_GLOBAL_GET_FULL }, // �S���ߊl�R���v
  { DEBUG_FIELD_ZUKAN_07, (void*)DEBUG_ZKNCMD_GLOBAL_SEE_FULL }, // �S�������R���v
  { DEBUG_FIELD_ZUKAN_10, (void*)DEBUG_ZKNCMD_LOCAL_GET_FULL },  // �n���ߊl�R���v
  { DEBUG_FIELD_ZUKAN_11, (void*)DEBUG_ZKNCMD_LOCAL_SEE_FULL },  // �n�������R���v
  { DEBUG_FIELD_ZUKAN_09, (void*)DEBUG_ZKNCMD_RANDOM },          // �����_���Z�b�g
  { DEBUG_FIELD_ZUKAN_03, (void*)DEBUG_ZKNCMD_FORM_FULL },       // �t�H�����R���v
  { DEBUG_FIELD_ZUKAN_12, (void*)DEBUG_ZKNCMD_LANGUAGE_FULL },   // ����R���v
  { DEBUG_FIELD_ZUKAN_05, (void*)DEBUG_ZKNCMD_ZENKOKU_FLAG },    // �S���t���O
  { DEBUG_FIELD_ZUKAN_06, (void*)DEBUG_ZKNCMD_VERSION_UP },      // �o�[�W�����A�b�v
  { DEBUG_FIELD_ZUKAN_13, (void*)DEBUG_ZKNCMD_LOCAL_SEE_25 },    // �n�������Q�T
  { DEBUG_FIELD_ZUKAN_14, (void*)DEBUG_ZKNCMD_LOCAL_SEE_60 },    // �n�������U�O
  { DEBUG_FIELD_ZUKAN_15, (void*)DEBUG_ZKNCMD_LOCAL_SEE_120 },   // �n�������P�Q�O
};

static const DEBUG_MENU_INITIALIZER DebugMenuZukanImitializer = {
  NARC_message_d_field_dat,
  NELEMS(ZukanMenuList),
  ZukanMenuList,
  &ZukanMenuHeader,
  1, 1, 16, 16,
  NULL,
  NULL
};

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZukanEvent( GMEVENT *event, int *seq, void *wk );
static void SetZukanDataOne( DEBUG_ZUKAN_WORK * wk, u16 mons, u16 form, u16 lang, u32 mode );
static void SetZukanLocal( DEBUG_ZUKAN_WORK * wk, u16 count, u32 mode );
static void SetZukanDataAll( DEBUG_ZUKAN_WORK * wk );


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�}��
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Zukan( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_ZUKAN_WORK *work;

  GMEVENT_Change( event, debugMenuZukanEvent, sizeof(DEBUG_ZUKAN_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_ZUKAN_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  work->sv = GAMEDATA_GetZukanSave( GAMESYSTEM_GetGameData(wk->gmSys) );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�}��
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZukanEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ZUKAN_WORK *work = wk;

  switch( *seq ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID, &DebugMenuZukanImitializer );
    (*seq)++;
    break;

  case 1:

    switch( FLDMENUFUNC_ProcMenu(work->menuFunc) ){
    case FLDMENUFUNC_NULL:    // ���얳��
      break;

    case FLDMENUFUNC_CANCEL:  // �L�����Z��
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_GLOBAL_GET_FULL:     // �S���ߊl
      {
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          SetZukanDataOne( wk, i, 0, PM_LANG, 0 );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_GLOBAL_SEE_FULL:     // �S������
      {
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          SetZukanDataOne( wk, i, 0, PM_LANG, 1 );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_RANDOM:     // �����_��
      {
        u32 rand;
        u32 max;
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          rand = GFL_STD_MtRand( 3 );
          if( rand == 2 ){ continue; }
          max = ZUKANSAVE_GetFormMax( i );
					SetZukanDataOne( wk, i, GFL_STD_MtRand(max), PM_LANG, rand );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_FORM_FULL:     // �t�H����
/*
      {
        u32 max;
        u16 i, j;
        for( i=1; i<=MONSNO_END; i++ ){
          max = ZUKANSAVE_GetFormMax( i );
          if( max != 1 ){
            for( j=0; j<max; j++ ){
              SetZukanDataOne( wk, i, j, PM_LANG, 0 );
            }
          }
        }
      }
*/
			SetZukanDataAll( wk );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_ZENKOKU_FLAG:     // �S���t���O
      ZUKANSAVE_SetZenkokuZukanFlag( work->sv );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_VERSION_UP:     // �o�[�W�����A�b�v
      ZUKANSAVE_SetGraphicVersionUpFlag( work->sv );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LOCAL_GET_FULL:     // �n���ߊl
      SetZukanLocal( wk, MONSNO_END, 0 );
#if 0
      {
				u16 * buf;
        u32 i;
				buf = POKE_PERSONAL_GetZenkokuToChihouArray( work->heapID, NULL );
        for( i=1; i<=MONSNO_END; i++ ){
					if( buf[i] != 0 ){
						SetZukanDataOne( wk, i, 0, PM_LANG, 0 );
					}
        }
				GFL_HEAP_FreeMemory( buf );
      }
#endif
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LOCAL_SEE_FULL:     // �n������
      SetZukanLocal( wk, MONSNO_END, 1 );
#if 0
      {
				u16 * buf;
        u32 i;
				buf = POKE_PERSONAL_GetZenkokuToChihouArray( work->heapID, NULL );
        for( i=1; i<=MONSNO_END; i++ ){
					if( buf[i] != 0 ){
	          SetZukanDataOne( wk, i, 0, PM_LANG, 1 );
					}
        }
				GFL_HEAP_FreeMemory( buf );
      }
#endif
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LOCAL_SEE_25:
      SetZukanLocal( wk, 25, 1 );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LOCAL_SEE_60:
      SetZukanLocal( wk, 60, 1 );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LOCAL_SEE_120:
      SetZukanLocal( wk, 120, 1 );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case DEBUG_ZKNCMD_LANGUAGE_FULL:     // ����
      {
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          SetZukanDataOne( wk, i, 0, LANG_JAPAN, 0 );
          SetZukanDataOne( wk, i, 0, LANG_ENGLISH, 0 );
          SetZukanDataOne( wk, i, 0, LANG_FRANCE, 0 );
          SetZukanDataOne( wk, i, 0, LANG_ITALY, 0 );
          SetZukanDataOne( wk, i, 0, LANG_GERMANY, 0 );
          SetZukanDataOne( wk, i, 0, LANG_SPAIN, 0 );
          SetZukanDataOne( wk, i, 0, LANG_KOREA, 0 );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

static void SetZukanLocal( DEBUG_ZUKAN_WORK * wk, u16 count, u32 mode )
{
  u16 * buf;
  u32 i;
  buf = POKE_PERSONAL_GetZenkokuToChihouArray( wk->heapID, NULL );
  for( i=1; i<=MONSNO_END; i++ ){
    if( buf[i] != 0 ){
      SetZukanDataOne( wk, i, 0, PM_LANG, mode );
      count --;
    }
    if ( count == 0 ) break;
  }
  GFL_HEAP_FreeMemory( buf );
}

static void SetZukanDataOne( DEBUG_ZUKAN_WORK * wk, u16 mons, u16 form, u16 lang, u32 mode )
{
  POKEMON_PARAM * pp = PP_Create( mons, 50, 0, wk->heapID );
	PP_Put( pp, ID_PARA_form_no, form );
  PP_Put( pp, ID_PARA_country_code, lang );

  if( mode == 0 ){
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
  }else{
    ZUKANSAVE_SetPokeSee( wk->sv, pp );
  }

  GFL_HEAP_FreeMemory( pp );
}


static void SetZukanDataAllOne( DEBUG_ZUKAN_WORK * wk, POKEMON_PARAM * pp, u32 sex, BOOL rare, BOOL first )
{
	u32	mons;
	u32	id;
	u32	rand;
	u32	form_max;
	u32	i;

	mons = PP_Get( pp, ID_PARA_monsno, NULL );
	id   = PP_Get( pp, ID_PARA_id_no, NULL );
	rand = POKETOOL_CalcPersonalRandEx( id, mons, 0, sex, 0, rare );
	PP_SetupEx( pp, mons, 50, id, PTL_SETUP_POW_AUTO, rand );

	form_max = ZUKANSAVE_GetFormMax( mons );

	if( first == TRUE ){
		ZUKANSAVE_SetPokeGet( wk->sv, pp );
	}else{
		ZUKANSAVE_SetPokeSee( wk->sv, pp );
	}

	for( i=1; i<form_max; i++ ){
		rand = POKETOOL_CalcPersonalRandEx( id, mons, i, sex, 0, rare );
		PP_Put( pp, ID_PARA_personal_rnd, rand );
		PP_Put( pp, ID_PARA_form_no, i );
    ZUKANSAVE_SetPokeSee( wk->sv, pp );
	}
}

static void SetZukanDataAll( DEBUG_ZUKAN_WORK * wk )
{
	POKEMON_PERSONAL_DATA * ppd;
	POKEMON_PARAM * pp;
	BOOL fast;
	u32	sex_vec;
	u32	rand;
	u16	i, j;

	for( i=1; i<MONSNO_END; i++ ){
		// ���ʃx�N�g���擾
		ppd = POKE_PERSONAL_OpenHandle( i, 0, wk->heapID );
		sex_vec = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
		POKE_PERSONAL_CloseHandle( ppd );
		// POKEMON_PARAM�쐬
		pp = PP_Create( i, 50, 0, wk->heapID );
		fast = PP_FastModeOn( pp );
		PP_Put( pp, ID_PARA_form_no, 0 );
	  PP_Put( pp, ID_PARA_country_code, LANG_JAPAN );

		// ���ʂȂ�
		if( sex_vec == POKEPER_SEX_UNKNOWN ){
			SetZukanDataAllOne( wk, pp, PTL_SEX_UNKNOWN, FALSE, TRUE );		// ���A����Ȃ�
			SetZukanDataAllOne( wk, pp, PTL_SEX_UNKNOWN, TRUE, FALSE );		// ���A
		// ���ʂ���
		}else{
			// ��������
			if( sex_vec != POKEPER_SEX_FEMALE ){
//				PP_Put( pp, ID_PARA_sex, PTL_SEX_MALE );
				SetZukanDataAllOne( wk, pp, PTL_SEX_MALE, FALSE, TRUE );		// ���A����Ȃ�
				SetZukanDataAllOne( wk, pp, PTL_SEX_MALE, TRUE, FALSE );		// ���A
			}
			// ��������
			if( sex_vec != POKEPER_SEX_MALE ){
//				PP_Put( pp, ID_PARA_sex, PTL_SEX_FEMALE );
				SetZukanDataAllOne( wk, pp, PTL_SEX_FEMALE, FALSE, TRUE );	// ���A����Ȃ�
				SetZukanDataAllOne( wk, pp, PTL_SEX_FEMALE, TRUE, FALSE );	// ���A
			}
		}

		// �e����Z�b�g
	  PP_Put( pp, ID_PARA_country_code, LANG_ENGLISH );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
	  PP_Put( pp, ID_PARA_country_code, LANG_FRANCE );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
	  PP_Put( pp, ID_PARA_country_code, LANG_ITALY );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
	  PP_Put( pp, ID_PARA_country_code, LANG_GERMANY );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
	  PP_Put( pp, ID_PARA_country_code, LANG_SPAIN );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
	  PP_Put( pp, ID_PARA_country_code, LANG_KOREA );
    ZUKANSAVE_SetPokeGet( wk->sv, pp );

		PP_FastModeOff( pp, fast );
	  GFL_HEAP_FreeMemory( pp );

		OS_Printf( "No.%d : �}�Ӓǉ�\n", i );
  }
}



//======================================================================================
//
//
//======================================================================================
//--------------------------------------------------------------
//�C�x���g�|�P�����쐬
//--------------------------------------------------------------
#include "poke_tool/poke_memo.h"
static GMEVENT_RESULT debugMenuEventpokeCreate( GMEVENT *event, int *seq, void *wk );


static const FLDMENUFUNC_LIST DATA_EventPokeCreate[] =
{
  { DEBUG_FIELD_EVEPOKE01, (void*)0 },
  { DEBUG_FIELD_EVEPOKE02, (void*)1 },
  { DEBUG_FIELD_EVEPOKE03, (void*)2 },
  { DEBUG_FIELD_EVEPOKE04, (void*)3 },
  { DEBUG_FIELD_EVEPOKE05, (void*)4 },
  { DEBUG_FIELD_EVEPOKE06, (void*)5 },
  { DEBUG_FIELD_EVEPOKE07, (void*)6 },
  { DEBUG_FIELD_EVEPOKE08, (void*)7 },
  { DEBUG_FIELD_EVEPOKE09, (void*)8 },
  { DEBUG_FIELD_EVEPOKE10, (void*)9 },
  { DEBUG_FIELD_EVEPOKE11, (void*)10 },
  { DEBUG_FIELD_EVEPOKE12, (void*)11 },
};

static const DEBUG_MENU_INITIALIZER DebugEventPokeCreateMenu = {
  NARC_message_d_field_dat,
  NELEMS(DATA_EventPokeCreate),
  DATA_EventPokeCreate,
  &DATA_DebugMenuList_Default, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};

typedef struct
{
  HEAPID heapId;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FLDMENUFUNC *menuFunc;
  FIELDMAP_WORK *fieldWork;
}DEBUG_EVENT_POKE_CREATE;

static BOOL debugMenuCallProc_EventPokeCreate( DEBUG_MENU_EVENT_WORK * wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_EVENT_POKE_CREATE *work;

  GMEVENT_Change( event, debugMenuEventpokeCreate, sizeof(DEBUG_EVENT_POKE_CREATE) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_EVENT_POKE_CREATE) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->fieldWork = fieldWork;
  work->heapId = heapID;

  return TRUE;
}
static GMEVENT_RESULT debugMenuEventpokeCreate( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_EVENT_POKE_CREATE *work = wk;

  switch( *seq ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapId, &DebugEventPokeCreateMenu );
    (*seq)++;
    break;

  case 1:
    {
      const u32 ret = FLDMENUFUNC_ProcMenu(work->menuFunc);
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(work->gmSys);
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);
      BOX_MANAGER *boxData = GAMEDATA_GetBoxManager(gmData);
      
      if( ret == FLDMENUFUNC_NULL )
      {
        return GMEVENT_RES_CONTINUE;
      }
      else
      if( ret == FLDMENUFUNC_CANCEL )
      {
        FLDMENUFUNC_DeleteMenu( work->menuFunc );
        return GMEVENT_RES_FINISH;
      }
      else
      {
        POKEMON_PARAM *pp;
        //�|�P�����쐬
        switch( ret )
        {
        case 0: //10�������Z���r�B
          pp = PP_Create( MONSNO_SEREBHI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_SEREBIXI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 1: //10�������G���e�C
          pp = PP_Create( MONSNO_ENTEI , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 2: //10���������C�R�E
          pp = PP_Create( MONSNO_RAIKOU , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 3: //10�������X�C�N��
          pp = PP_Create( MONSNO_SUIKUN , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 4: //10�������Z���r�B(��
          pp = PP_Create( MONSNO_SEREBHI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_SEREBIXI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 5: //10�������G���e�C(��
          pp = PP_Create( MONSNO_ENTEI , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 6: //10���������C�R�E(��
          pp = PP_Create( MONSNO_RAIKOU , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 7: //10�������X�C�N��(��
          pp = PP_Create( MONSNO_SUIKUN , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_birth_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 8: //�����f�B�A
          pp = PP_Create( MONSNO_655 , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 9: //�_���^�j�X
          pp = PP_Create( MONSNO_654 , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 10: //�C���Z�N�^
          pp = PP_Create( MONSNO_656 , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 11: //�͂��ӃV�F�C�~
          pp = PP_Create( MONSNO_SHEIMI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        }
        {
          u16 oyaName[] = {L'�C',L'�x',L'��',L'�g',L'��',L'��',0xFFFF};
          PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
          PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
        }
        //�莝���ɋ󂫂�����Γ����
        if( PokeParty_GetPokeCount( party ) < 6 )
        {
          PokeParty_Add( party , pp );
        }
        else
        {
          BOXDAT_PutPokemon( boxData , PP_GetPPPPointer( pp ) );
        }
        GFL_HEAP_FreeMemory( pp );
      }
    }
  }

  return GMEVENT_RES_CONTINUE;
}


#include "event_debug_menu_symbol.h"
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�V���{���|�P�����쐬���X�g
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_SymbolPokeList( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * new_event = EVENT_DEBUG_SymbolPokeList( wk );
  GMEVENT_ChangeEvent( wk->gmEvent, new_event );

  return TRUE;
}

#endif  //  PM_DEBUG
