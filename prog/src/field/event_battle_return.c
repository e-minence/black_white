//============================================================================================
/**
 * @file    event_battle_return.c
 * @brief   �o�g������t�B�[���h�ւ̕��A�Ǘ�
 * @author  taya
 * @date    2009.10.23
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <procsys.h>

// global includes --------------------
#include "system\main.h"
#include "savedata\box_savedata.h"
#include "app\name_input.h"
#include "poke_tool\pokerus.h"
#include "poke_tool\shinka_check.h"
#include "demo\shinka_demo.h"
#include "app/zukan_toroku.h"
#include "poke_tool/poke_memo.h"
#include "field/zonedata.h"  //ZONEDATA_GetPlaceNameID
#include "field/straw_poke_form.h"
#include "gamesystem/game_data.h"
#include "savedata/record.h"

#include "arc_def.h"
#include "message.naix"
#include "msg/msg_zkn.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "item/item.h"
#include "item/itemsym.h"
#include "poke_tool/tokusyu_def.h"
#include "system/gfl_use.h"

#include "battle/batt_bg_tbl.h"
#include "batt_bg_tbl.naix"

// local includes --------------------
#include "event_battle_return.h"

#include "monohiroi_tbl.cdat"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_toroku);


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  STRBUF_SIZE = 32,   ///< �v���C���[�����ꎞ�I�Ɋi�[���邽�߂̃o�b�t�@�T�C�Y
};

/*--------------------------------------------------------------------------*/
/* Main Work                                                                */
/*--------------------------------------------------------------------------*/
typedef struct {

  NAMEIN_PARAM*     nameinParam;
  STRBUF*           strbuf;
  POKEMON_PARAM*    pp;
  ZUKAN_TOROKU_PARAM zukan_toroku_param;
  STRBUF*            box_strbuf;  // NULL�łȂ��Ƃ��|�P�����ߊl�Ń{�b�N�X�]���ɂȂ�
  u32                box_tray;

  SHINKA_DEMO_PARAM*  shinka_param;
  u16                 shinka_poke_pos;
  u16                 shinka_poke_bit;

  HEAPID  heapID;

  // ���[�J��PROC�V�X�e��
  GFL_PROCSYS*  local_procsys;
}BTLRET_WORK;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT BtlRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BtlRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BtlRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void check_lvup_poke( BTLRET_WORK* wk, BTLRET_PARAM* param );
static void monohiroi_mitsuatsume_check( POKEPARTY* ppt );
static void sizenkaihuku_check( POKEPARTY* ppt );  // ����(�Ƃ�����)�����R��(�����񂩂��ӂ�)�Ȃ�o�g����ɏ�Ԉُ���񕜂���
static void minomucchi_form_change_check( BTLRET_PARAM* param, POKEPARTY* ppt, BATT_BG_OBONID obonID );
static BATT_BG_OBONID get_obonID( BTL_FIELD_SITUATION* bfs, HEAPID heapID );
static void sheimi_zukan_touroku_check( BTLRET_PARAM* param, POKEPARTY* ppt );
static void healball_check( POKEMON_PARAM * pp );

/*--------------------------------------------------------------------------*/
/* Proc Table                                                               */
/*--------------------------------------------------------------------------*/
const GFL_PROC_DATA   BtlRet_ProcData = {
  BtlRet_ProcInit,
  BtlRet_ProcMain,
  BtlRet_ProcQuit,
};


//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK* wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTLRET_SYS, 0x1000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(BTLRET_WORK), HEAPID_BTLRET_SYS );

  wk->strbuf = GFL_STR_CreateBuffer( STRBUF_SIZE, HEAPID_BTLRET_SYS );
  wk->nameinParam = NULL;
  wk->pp = NULL;
  wk->box_strbuf = NULL;
  wk->box_tray = 0;
  wk->shinka_param = NULL;
  wk->heapID = HEAPID_BTLRET_SYS;

  // ���[�J��PROC�V�X�e�����쐬
  wk->local_procsys = GFL_PROC_LOCAL_boot( wk->heapID );

  return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK* wk = mywk;

  // ���[�J��PROC�V�X�e����j��
  GFL_PROC_LOCAL_Exit( wk->local_procsys );

  if( wk->box_strbuf )
  {
    GFL_STR_DeleteBuffer( wk->box_strbuf );
  }

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_BTLRET_SYS );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK*  wk = mywk;
  BTLRET_PARAM* param = pwk;

  // ���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( wk->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch( *seq ){
  case 0:
    {
      POKEPARTY* party    = GAMEDATA_GetMyPokemon( param->gameData );
      MYSTATUS*  myStatus = GAMEDATA_GetMyStatus( param->gameData );
      ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gameData );
      BOX_MANAGER* boxman = GAMEDATA_GetBoxManager( param->gameData );

      wk->shinka_poke_pos = 0;
      wk->shinka_poke_bit = 0;

#ifdef PM_DEBUG
      // �f�o�b�O�s����A����L�[�����Ȃ��瓦�������Ɍ��ʃR�[�h��������
      if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_L | PAD_BUTTON_R) )
      {
        param->btlResult->result = BTL_RESULT_WIN;
        if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){   // X���� = ����
          param->btlResult->result = BTL_RESULT_LOSE;
        }
      }
#endif

      // �쐶or�Q�[�����ʏ�g���[�i�[�i�T�u�E�F�C�����j�̌�̂ݍs������
      if( (param->btlResult->competitor == BTL_COMPETITOR_WILD)
      ||  (param->btlResult->competitor == BTL_COMPETITOR_TRAINER)
      ){
        // ���x���A�b�v�i���`�F�b�N
        check_lvup_poke( wk, param );
        // �p�[�e�B�f�[�^�����߂�
        PokeParty_Copy( param->btlResult->party[ BTL_CLIENT_PLAYER ], party );

        //�|�P���X�������`���`�F�b�N
        POKERUS_CheckCatchPokerus( party );
        POKERUS_CheckContagion( party );

        //���̂Ђ낢�����܂��݂`�F�b�N�i�������̂݁j
        if( param->btlResult->result == BTL_RESULT_WIN )
        {
          monohiroi_mitsuatsume_check( party );
        }

        //�~�m���b�`�t�H�����`�F���W�`�F�b�N
        {
          BATT_BG_OBONID obonID = get_obonID( &param->btlResult->fieldSituation, wk->heapID );
          minomucchi_form_change_check( param, party, obonID );
        }

        //�V�F�C�~�}�ӓo�^�`�F�b�N
        sheimi_zukan_touroku_check( param, party );

        // �����Â������₷
        if( param->btlResult->getMoney > 0 ){
          MISC_AddGold( GAMEDATA_GetMiscWork(param->gameData), param->btlResult->getMoney);
        }else if( param->btlResult->getMoney < 0 ){
          //MISC_??Gold��unsigned�̒l�����󂯎��Ȃ�
          MISC_SubGold( GAMEDATA_GetMiscWork(param->gameData), -(param->btlResult->getMoney) );
        }
      }

      // ����(�Ƃ�����)���R��(�����񂩂��ӂ�)�̃`�F�b�N
      {
        sizenkaihuku_check( party );  // ����(�Ƃ�����)�����R��(�����񂩂��ӂ�)�Ȃ�o�g����ɏ�Ԉُ���񕜂���
      }

      // �ߊl����
      if( param->btlResult->result == BTL_RESULT_CAPTURE )
      {
        BOOL zenkoku_flag = FALSE;  // �S���}�ӂ̂Ƃ�TRUE
        BOOL get_first = FALSE;  // ���ߊl�̂Ƃ�TRUE

        wk->pp = PokeParty_GetMemberPointer(
                                param->btlResult->party[ BTL_CLIENT_ENEMY1 ], param->btlResult->capturedPokeIdx );

        // �e���Z�b�g���Ă邯�ǁA�{���̓G���J�E���g�O�Ƀt�B�[���h���Őݒ肷�ׂ��H
        MyStatus_CopyNameString( myStatus, wk->strbuf );
        PP_Put( wk->pp, ID_PARA_oyaname, (u32)(wk->strbuf) );

        // �莝�����{�b�N�X�ɒu����
        if( !( PokeParty_GetPokeCount(party) < PokeParty_GetPokeCountMax(party) ) )
        {
          int tray_num;
          int pos;
          BOOL empty;

          GFL_MSGDATA* msgdata;
          BOOL flag_pcname;

          // �]�������{�b�N�X�̃g���C�i���o�[���m�肷��
          tray_num = (int)BOXDAT_GetCureentTrayNumber( boxman );
          pos = 0;
          empty = BOXDAT_GetEmptyTrayNumberAndPos( boxman, &tray_num, &pos );  // ����̌��ʂ�BOXDAT_PutPokemon�Ɠ����͂�
          GF_ASSERT_MSG( empty, "BtlRet_ProcMain : �{�b�N�X�ɋ󂫂�����܂���ł����B\n" );
          wk->box_tray = (u32)tray_num;

          // STRBUF���쐬����
          msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, HEAPID_BTLRET_SYS );
          flag_pcname = EVENTWORK_CheckEventFlag( GAMEDATA_GetEventWork(param->gameData), SYS_FLAG_PCNAME );
          wk->box_strbuf = GFL_MSG_CreateString( msgdata, (flag_pcname)?(ZKN_POKEGET_BOX_02):(ZKN_POKEGET_BOX_01) );
          GFL_MSG_Delete( msgdata );
        }

        // �g���[�i�[����
        {
          PLAYER_WORK* player_wk = GAMEDATA_GetMyPlayerWork( param->gameData );
          POKE_MEMO_SetTrainerMemoPP(
              wk->pp,
              POKE_MEMO_SET_CAPTURE,
              myStatus,
              ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID( player_wk ) ),
              wk->heapID );
        }

        // �S���}�ӂ��ۂ�
        //zenkoku_flag = ZUKANSAVE_GetZukanMode( zukan_savedata );  // TRUE�̂Ƃ��S���}�ӂƂ��Ĉ����Ă���
        zenkoku_flag = ZUKANSAVE_GetZenkokuZukanFlag( zukan_savedata );  // TRUE�̂Ƃ��S���}�ӂ������Ă���

        // �}�ӓo�^��� or �j�b�N�l�[�������m�F��� ��
        get_first = !ZUKANSAVE_GetPokeGetFlag( zukan_savedata, (u16)( PP_Get(wk->pp, ID_PARA_monsno, NULL) ) );
        // �}�ӓo�^�i�߂܂����j
        {
          ZUKANSAVE_SetPokeSee( zukan_savedata, wk->pp );  // ����  // �}�Ӄt���O���Z�b�g����
          ZUKANSAVE_SetPokeGet( zukan_savedata, wk->pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
        }

        // ZUKAN_TOROKU_PARAM�̐ݒ�
        {
          wk->zukan_toroku_param.pp               = wk->pp;
          wk->zukan_toroku_param.b_zenkoku_flag   = zenkoku_flag;
          wk->zukan_toroku_param.box_strbuf       = wk->box_strbuf;
          wk->zukan_toroku_param.box_manager      = boxman;
          wk->zukan_toroku_param.box_tray         = wk->box_tray;
          wk->zukan_toroku_param.gamedata         = param->gameData;
        }
        if( get_first )
        {
          wk->zukan_toroku_param.launch           = ZUKAN_TOROKU_LAUNCH_TOROKU;
        }
        else
        {
          wk->zukan_toroku_param.launch           = ZUKAN_TOROKU_LAUNCH_NICKNAME;
        }
        // ���[�J��PROC�Ăяo��
        GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(zukan_toroku), &ZUKAN_TOROKU_ProcData, &(wk->zukan_toroku_param) );
        (*seq)++;
      }else{
        (*seq) = 4;
      }
    }
    break;

  case 1:
    {
      // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        BOX_MANAGER* boxman = GAMEDATA_GetBoxManager( param->gameData );

        BOOL nickname = FALSE;
        if( wk->zukan_toroku_param.result == ZUKAN_TOROKU_RESULT_NICKNAME )
        {
          nickname = TRUE;
        }

        if( nickname )
        {
          MISC *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(param->gameData) );
          // ���O���͉�ʂ�
          wk->nameinParam = NAMEIN_AllocParamPokemonCapture( wk->heapID, wk->pp, NAMEIN_POKEMON_LENGTH, NULL,
                                                           wk->box_strbuf, boxman, wk->box_tray,  misc );

          // ���[�J��PROC�Ăяo��
          GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(namein), &NameInputProcData, wk->nameinParam );
          (*seq)++;
        }
        else
        {
          (*seq) = 3;
        }
      }
      else
      {
        return GFL_PROC_RES_CONTINUE;
      }
    }
    break;

  case 2:
    // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
    if( local_proc_status != GFL_PROC_MAIN_VALID )
    {
      if( !NAMEIN_IsCancel(wk->nameinParam) ){
        NAMEIN_CopyStr( wk->nameinParam, wk->strbuf );
        PP_Put( wk->pp, ID_PARA_nickname, (u32)(wk->strbuf) );

        // �|�P�����Ƀj�b�N�l�[����t������
        {
          RECORD* rec = GAMEDATA_GetRecordPtr(param->gameData);
          RECORD_Inc(rec, RECID_NICKNAME);
        }
      }
      NAMEIN_FreeParam( wk->nameinParam );
      wk->nameinParam = NULL;
      (*seq)++;
    }
    else
    {
      return GFL_PROC_RES_CONTINUE;
    }
    break;

  case 3:
    {
      POKEPARTY* party    = GAMEDATA_GetMyPokemon( param->gameData );

			healball_check( wk->pp );

      if( wk->box_strbuf == NULL ){
        PokeParty_Add( party, wk->pp );
      }else{
        BOX_MANAGER* boxman = GAMEDATA_GetBoxManager( param->gameData );
        BOXDAT_PutPokemon( boxman, PP_GetPPPPointer(wk->pp) );
      }
    }
    (*seq)++;
    break;
  case 4:
    //�i���`�F�b�N
    if( wk->shinka_poke_bit )
    {
      u16 after_mons_no = 0;
      int pos;
      SHINKA_COND cond;
      POKEPARTY* party  = GAMEDATA_GetMyPokemon( param->gameData );
      (*seq) = 6;
      OS_TPrintf("�i���|�P����������\n");
      while( wk->shinka_poke_bit )
      {
        if( wk->shinka_poke_bit & 1 )
        {
          POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, wk->shinka_poke_pos );
          after_mons_no = SHINKA_Check( party, pp, SHINKA_TYPE_LEVELUP, param->btlResult->fieldSituation.zoneID,
                                        GAMEDATA_GetSeasonID( param->gameData ), &cond, wk->heapID );
          pos = wk->shinka_poke_pos;
        }
        wk->shinka_poke_bit = wk->shinka_poke_bit >> 1;
        wk->shinka_poke_pos++;
        if( after_mons_no )
        {
          //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
          //wk->shinka_param = SHINKADEMO_AllocParam( wk->heapID, param->gameData, party, after_mons_no, pos, cond, FALSE );
          {
            SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( wk->heapID, sizeof( SHINKA_DEMO_PARAM ) );
            sdp->gamedata          = param->gameData;
            sdp->ppt               = party;
            sdp->after_mons_no     = after_mons_no;
            sdp->shinka_pos        = pos;
            sdp->shinka_cond       = cond;
            sdp->b_field           = FALSE;
            sdp->b_enable_cancel   = TRUE;
            wk->shinka_param       = sdp;
          }
          // ���[�J��PROC�Ăяo��
          GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->shinka_param );
          (*seq) = 5;
          break;
        }
      }
    }
    else
    {
      (*seq) = 6;
    }
    break;
  case 5:
    // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
    if( local_proc_status != GFL_PROC_MAIN_VALID )
    {
      //SHINKADEMO_FreeParam( wk->shinka_param );
      {
        SHINKA_DEMO_PARAM* sdp = wk->shinka_param;
        GFL_HEAP_FreeMemory( sdp );
      }
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      wk->shinka_param = NULL;
      if( wk->shinka_poke_bit )
      {
        (*seq) = 4;
      }
      else
      {
        (*seq)++;
      }
    }
    else
    {
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case 6:
  default:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * ���x���A�b�v���Ă���|�P�����𒲂ׂĐi���`�F�b�N���邩���f����
 */
//--------------------------------------------------------------------------
static void check_lvup_poke( BTLRET_WORK* wk, BTLRET_PARAM* param )
{
  //�����A������A�Q�b�g�ȊO�͐i���`�F�b�N���Ȃ��i������A�Q�b�g��2vs2�쐶�ł��肤��̂Łj
  if( ( param->btlResult->result != BTL_RESULT_WIN ) &&
      ( param->btlResult->result != BTL_RESULT_RUN ) &&
      ( param->btlResult->result != BTL_RESULT_CAPTURE ) )
  {
    return;
  }

  // �쐶���Q�[�����g���[�i�[�i�T�u�E�F�C�����j�Ƃ̑ΐ�ȊO�̓`�F�b�N���Ȃ��i���x���A�b�v���Ȃ��͂��j
  if( (param->btlResult->competitor != BTL_COMPETITOR_WILD)
  &&  (param->btlResult->competitor != BTL_COMPETITOR_TRAINER)
  ){
    OS_TPrintf("�T�u�E�F�C���[�h�ȂǂŐi���`�F�b�N���s��Ȃ�\n");
    return;
  }

  {
    POKEPARTY* party    = GAMEDATA_GetMyPokemon( param->gameData );
    POKEMON_PARAM*  old_pp;
    POKEMON_PARAM*  new_pp;
    int i;
    int max = PokeParty_GetPokeCount( party );

    for( i = 0 ; i < max ; i++ )
    {
      old_pp = PokeParty_GetMemberPointer( party, i );
      new_pp = PokeParty_GetMemberPointer( param->btlResult->party[ BTL_CLIENT_PLAYER ], i );
      if( PP_Get( old_pp, ID_PARA_level, NULL ) < PP_Get( new_pp, ID_PARA_level, NULL ) )
      {
        wk->shinka_poke_bit |= ( 1 << i );
      }
    }
  }
}

//============================================================================================
/**
 *  �������̂Ђ낢�A�݂��߂̃`�F�b�N
 *
 *  @param[in]  ppt POKEPARTY�\����
 */
//============================================================================================
static  void  monohiroi_mitsuatsume_check( POKEPARTY* ppt )
{
  int rnd;
  int i,j,k;
  u16 monsno;
  u16 itemno;
  u8  speabi;
  u8  LvOffset;
  POKEMON_PARAM *pp;

  for( i = 0 ; i < PokeParty_GetPokeCount( ppt ) ; i++ )
  {
    pp = PokeParty_GetMemberPointer( ppt, i );
    monsno = PP_Get( pp, ID_PARA_monsno_egg, NULL );
    itemno = PP_Get( pp, ID_PARA_item, NULL );
    speabi = PP_Get( pp, ID_PARA_speabino, NULL );
    //���̂Ђ낢�`�F�b�N
    if( ( speabi == TOKUSYU_MONOHIROI ) &&
        ( monsno != 0 ) &&
        ( monsno != MONSNO_TAMAGO ) &&
        ( itemno == 0) &&
        ( GFUser_GetPublicRand( 10 ) == 0 ) )
    {
      rnd = GFUser_GetPublicRand( 100 );
      LvOffset = ( PP_Get( pp, ID_PARA_level, NULL ) - 1 ) / 10;
      if( LvOffset >= 10 )
      {
        LvOffset = 9; //���x����100�𒴂��Ȃ���΂��肦�Ȃ����A�O�̂���
      }
      for( j = 0 ; j < 9 ; j++ )
      {
        if( ItemProb[ j ] > rnd )
        {
          PP_Put( pp, ID_PARA_item, MonohiroiTable1[ LvOffset + j ] );
          break;
        }
        else if( ( rnd >= 98 ) && ( rnd <= 99 ) )
        {
          PP_Put( pp, ID_PARA_item, MonohiroiTable2[ LvOffset + ( 99 - rnd ) ] );
          break;
        }
      }
    }
    //�݂��߃`�F�b�N
    if( ( speabi == TOKUSYU_MITUATUME ) &&
        ( monsno != 0 ) &&
        ( monsno != MONSNO_TAMAGO ) &&
        ( itemno == 0 ) )
    {
      j = 0;
      k = 10;
      LvOffset = PP_Get( pp, ID_PARA_level, NULL );
      while( LvOffset > k )
      {
        j++;
        k += 10;
      }
      //�e�[�u���I�[�o�[���Ď����邽�߂ɃA�T�[�g�����܂�
      GF_ASSERT( j < 10 );
      if( GFUser_GetPublicRand( 100 ) < MitsuatsumeProb[ j ] )
      {
        j = ITEM_AMAIMITU;
        PP_Put( pp, ID_PARA_item, j );
      }
    }
  }
}

//============================================================================================
/**
 *  ����(�Ƃ�����)���R��(�����񂩂��ӂ�)�̃`�F�b�N
 *
 *  @param[in]  ppt POKEPARTY�\����
 */
//============================================================================================
static void sizenkaihuku_check( POKEPARTY* ppt )  // ����(�Ƃ�����)�����R��(�����񂩂��ӂ�)�Ȃ�o�g����ɏ�Ԉُ���񕜂���
{
  // include/poke_tool/tokusyu_def.h
  // TOKUSYU_SIZENKAIHUKU
  // include/poke_tool/poketype.h
  // POKETOKUSEI_SIZENKAIFUKU

  int i;
  u16 monsno;
  u8  speabi;
  POKEMON_PARAM *pp;

  for( i = 0 ; i < PokeParty_GetPokeCount( ppt ) ; i++ )
  {
    pp = PokeParty_GetMemberPointer( ppt, i );
    monsno = PP_Get( pp, ID_PARA_monsno_egg, NULL );
    speabi = PP_Get( pp, ID_PARA_speabino, NULL );
    //���R��(�����񂩂��ӂ�)�`�F�b�N
    if( ( speabi == TOKUSYU_SIZENKAIHUKU ) &&
        ( monsno != 0 ) &&
        ( monsno != MONSNO_TAMAGO ) )
    {
      // ����ł���Ƃ��͎��R�񕜈ȊO�̓����̃|�P��������Ԉُ킪�񕜂����(�o�g���I�����ɉ񕜂��Ă���Ă���)�̂ŁA
      // �|�P�����������Ă��邩����ł��邩�̔���͕s�v�B
      PP_SetSick( pp, POKESICK_NULL );
    }
  }
}

//============================================================================================
/**
 *  �~�m���b�`�t�H�����`�F���W�`�F�b�N
 *
 */
//============================================================================================
static void minomucchi_form_change_check( BTLRET_PARAM* param, POKEPARTY* ppt, BATT_BG_OBONID obonID )
{
  int i;

  //�ʐM�ΐ�A�o�g���T�u�E�F�C�ł̓t�H�����`�F���W���Ȃ�
  if( ( param->btlResult->competitor == BTL_COMPETITOR_SUBWAY ) ||
      ( param->btlResult->competitor == BTL_COMPETITOR_COMM ) )
  {
    return;
  }

  for( i = 0 ; i < PokeParty_GetPokeCount( ppt ) ; i++ )
  {
    if( param->btlResult->fightPokeIndex[ i ] )
    {
      POKEMON_PARAM* pp = PokeParty_GetMemberPointer( ppt, i );
      STRAW_POKE_FORM_ChangeForm( param->gameData, pp, obonID );
    }
  }
}

//============================================================================================
/**
 *  ���~ID�擾
 *
 *  @param[in]  bfs     BTL_FIELD_SITUATION�\����
 *  @param[in]  heapID  �q�[�vID
 */
//============================================================================================
static  BATT_BG_OBONID  get_obonID( BTL_FIELD_SITUATION* bfs, HEAPID heapID )
{
    BATT_BG_TBL_ZONE_SPEC_TABLE*  bbtzst = GFL_ARC_LoadDataAlloc( ARCID_BATT_BG_TBL,
                                                                  NARC_batt_bg_tbl_zone_spec_table_bin,
                                                                  heapID );
    BATT_BG_OBONID  obonID = bbtzst[ bfs->bgType ].stage_file[ bfs->bgAttr ];

    GFL_HEAP_FreeMemory( bbtzst );

    return  obonID;
}

//============================================================================================
/**
 *  �V�F�C�~�}�ӓo�^�`�F�b�N
 *
 *  �X�J�C�t�H�����Ő퓬�ɎQ�����Đ퓬���Ƀ����h�t�H�����ɖ߂����Ƃ��ɐ}�ӓo�^�����鏈��
 */
//============================================================================================
static void sheimi_zukan_touroku_check( BTLRET_PARAM* param, POKEPARTY* ppt )
{
  int i;

  //�쐶��A�g���[�i�[��ȊO�ł͓o�^�`�F�b�N���Ȃ�
  if( ( param->btlResult->competitor != BTL_COMPETITOR_WILD ) &&
      ( param->btlResult->competitor != BTL_COMPETITOR_TRAINER ) )
  {
    return;
  }

  for( i = 0 ; i < PokeParty_GetPokeCount( ppt ) ; i++ )
  {
    if( param->btlResult->fightPokeIndex[ i ] )
    {
      POKEMON_PARAM* pp = PokeParty_GetMemberPointer( ppt, i );
      if( PP_Get( pp, ID_PARA_monsno, NULL ) == MONSNO_SHEIMI ) 
      { 
        ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gameData );
        ZUKANSAVE_SetPokeSee( zukan_savedata, pp );  // ����  // �}�Ӄt���O���Z�b�g����
      }
    }
  }
}

//============================================================================================
/**
 * �q�[���{�[���ŕߊl�����Ƃ��̉񕜏���
 */
//============================================================================================
static void healball_check( POKEMON_PARAM * pp )
{
	BOOL	fast = PP_FastModeOn( pp );

	if( PP_Get( pp, ID_PARA_get_ball, NULL ) == BALLID_HIIRUBOORU ){
		PP_Put( pp, ID_PARA_hp, PP_Get(pp,ID_PARA_hpmax,NULL) );
		PP_SetSick( pp, POKESICK_NULL );
	}

	PP_FastModeOff( pp, fast );
}
