#ifdef PM_DEBUG 
///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�w�|�P�������������x
 * @file   event_debug_rewrite_poke.c
 * @author obata
 * @date   2010.05.06
 *
 * ��event_debug_menu.c ���ړ�
 */
///////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "system/main.h"
#include "field/fieldmap_proc.h"
#include "field/field_msgbg.h"
#include "debug/debug_str_conv.h"
#include "event_debug_local.h"
#include "fieldmap.h"

#include "message.naix"
#include "font/font.naix"
#include "msg/msg_d_field.h"

#include "event_fieldmap_control.h"
#include "event_debug_rewrite_poke.h"


//======================================================================
//  �f�o�b�O���j���[ �|�P�����쐬
//======================================================================
#include "debug/debug_makepoke.h"
FS_EXTERN_OVERLAY(debug_makepoke);

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

//--------------------------------------------------------------
/**
 * �C�x���g�F�|�P�����쐬
 * @param gsys
 * @param wk
 * @return GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu_ReWritePoke( GAMESYS_WORK *gsys, void* wk )
{ 
  DEBUG_MENU_EVENT_WORK* debug_work = wk;
  FIELDMAP_WORK *p_field  = debug_work->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_REWRITEPOKE_EVENT_WORK *p_ev_wk;
  GMEVENT       *p_event;

  //�C���F���g
  p_event = GMEVENT_Create( gsys, debug_work->gmEvent, 
      debugMenuReWritePoke, sizeof(DEBUG_REWRITEPOKE_EVENT_WORK) );

  p_ev_wk = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_ev_wk, sizeof(DEBUG_REWRITEPOKE_EVENT_WORK) );

  //���[�N�ݒ�
  p_ev_wk->p_gamesys  = gsys;
  p_ev_wk->p_event    = p_event;
  p_ev_wk->p_field    = p_field;
  p_ev_wk->heapID     = heapID;
  p_ev_wk->p_src_pp   = PP_Create( 1, 1, 1, heapID );
  {
    POKEPARTY *p_party  = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(gsys) );
    p_ev_wk->pp =  PokeParty_GetMemberPointer( p_party, 0 );

    POKETOOL_CopyPPtoPP( p_ev_wk->pp, p_ev_wk->p_src_pp );
  }
  p_ev_wk->p_mp_work.dst  = p_ev_wk->pp;
  p_ev_wk->p_mp_work.oyaStatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(gsys) );

  return p_event;
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

#endif // PM_DEBUG 
