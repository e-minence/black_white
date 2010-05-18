//============================================================================================
/**
 * @file      event_pdc_return.c
 * @brief     �|�P�����h���[���L���b�`����t�B�[���h�ւ̕��A�Ǘ�
 * @author    Koji Kawada
 * @date      2010.03.23
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <procsys.h>

// global includes --------------------
#include "system\main.h"
#include "savedata\box_savedata.h"
#include "app\name_input.h"
#include "app/zukan_toroku.h"
#include "poke_tool/poke_memo.h"
#include "field/zonedata.h"  //ZONEDATA_GetPlaceNameID
#include "gamesystem/game_data.h"
#include "savedata/record.h"

#include "arc_def.h"
#include "message.naix"
#include "msg/msg_zkn.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

// local includes --------------------
#include "event_pdc_return.h"


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
  ZUKAN_TOROKU_PARAM zukan_toroku_param;
  STRBUF*            box_strbuf;  // NULL�łȂ��Ƃ��|�P�����ߊl�Ń{�b�N�X�]���ɂȂ�
  u32                box_tray;

  HEAPID  heapID;

  // ���[�J��PROC�V�X�e��
  GFL_PROCSYS*  local_procsys;
}PDCRET_WORK;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT PdcRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT PdcRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT PdcRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

/*--------------------------------------------------------------------------*/
/* Proc Table                                                               */
/*--------------------------------------------------------------------------*/
const GFL_PROC_DATA   PDCRET_ProcData = {
  PdcRet_ProcInit,
  PdcRet_ProcMain,
  PdcRet_ProcQuit,
};


//--------------------------------------------------------------------------
/**
 * @brief PDCRET_PARAM����
 *
 * @param[in] gameData  GAMEDATA
 * @param[in] result    PDC_RESULT
 * @param[in] pp        POKEMON_PARAM
 * @param[in] heapID    �q�[�vID
 *
 * @retval  PDCRET_PARAM
 */
//--------------------------------------------------------------------------
PDCRET_PARAM*  PDCRET_AllocParam( GAMEDATA* gameData, PDC_RESULT result, POKEMON_PARAM* pp,
                                  HEAPID heapID )
{
  PDCRET_PARAM* param = GFL_HEAP_AllocMemory( heapID, sizeof( PDCRET_PARAM ) );
  param->gameData = gameData;
  param->result   = result;
  param->pp       = pp;
  return param;
}
//--------------------------------------------------------------------------
/**
 * @brief PDCRET_PARAM�j��
 *
 * @param[in,out] param  PDCRET_PARAM
 *
 * @retval  
 */
//--------------------------------------------------------------------------
void  PDCRET_FreeParam( PDCRET_PARAM* param )
{
  GFL_HEAP_FreeMemory( param );
}


//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PdcRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDCRET_WORK* wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PDCRET_SYS, 0x1000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(PDCRET_WORK), HEAPID_PDCRET_SYS );

  wk->strbuf = GFL_STR_CreateBuffer( STRBUF_SIZE, HEAPID_PDCRET_SYS );
  wk->nameinParam = NULL;
  wk->box_strbuf = NULL;
  wk->box_tray = 0;
  wk->heapID = HEAPID_PDCRET_SYS;

  // ���[�J��PROC�V�X�e�����쐬
  wk->local_procsys = GFL_PROC_LOCAL_boot( wk->heapID );

  return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PdcRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDCRET_WORK* wk = mywk;

  // ���[�J��PROC�V�X�e����j��
  GFL_PROC_LOCAL_Exit( wk->local_procsys ); 

  if( wk->box_strbuf )
  {
    GFL_STR_DeleteBuffer( wk->box_strbuf );
  }

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_PDCRET_SYS );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PdcRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDCRET_WORK*  wk = mywk;
  PDCRET_PARAM* param = pwk;

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

      // �ߊl����
      if( param->result == PDC_RESULT_CAPTURE )
      {
        BOOL zenkoku_flag = FALSE;  // �S���}�ӂ̂Ƃ�TRUE
        BOOL get_first = FALSE;  // ���ߊl�̂Ƃ�TRUE

        // �e���Z�b�g���Ă邯�ǁA�{���̓G���J�E���g�O�Ƀt�B�[���h���Őݒ肷�ׂ��H
        MyStatus_CopyNameString( myStatus, wk->strbuf );
        PP_Put( param->pp, ID_PARA_oyaname, (u32)(wk->strbuf) );

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
          GF_ASSERT_MSG( empty, "PdcRet_ProcMain : �{�b�N�X�ɋ󂫂�����܂���ł����B\n" );
          wk->box_tray = (u32)tray_num;

          // STRBUF���쐬����
          msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, HEAPID_PDCRET_SYS );
          flag_pcname = EVENTWORK_CheckEventFlag( GAMEDATA_GetEventWork(param->gameData), SYS_FLAG_PCNAME );
          wk->box_strbuf = GFL_MSG_CreateString( msgdata, (flag_pcname)?(ZKN_POKEGET_BOX_02):(ZKN_POKEGET_BOX_01) );
          GFL_MSG_Delete( msgdata );
        }

        // �g���[�i�[����
        {
          PLAYER_WORK* player_wk = GAMEDATA_GetMyPlayerWork( param->gameData );
          POKE_MEMO_SetTrainerMemoPP(
              param->pp,
              POKE_MEMO_SET_CAPTURE,
              myStatus,
              ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID( player_wk ) ),
              wk->heapID );
        }

        // �S���}�ӂ��ۂ�
        //zenkoku_flag = ZUKANSAVE_GetZukanMode( zukan_savedata );  // TRUE�̂Ƃ��S���}�ӂƂ��Ĉ����Ă���
        zenkoku_flag = ZUKANSAVE_GetZenkokuZukanFlag( zukan_savedata );  // TRUE�̂Ƃ��S���}�ӂ������Ă���

        // �}�ӓo�^��� or �j�b�N�l�[�������m�F��� ��
        get_first = !ZUKANSAVE_GetPokeGetFlag( zukan_savedata, (u16)( PP_Get(param->pp, ID_PARA_monsno, NULL) ) );
        // �}�ӓo�^�i�߂܂����j
        {
          ZUKANSAVE_SetPokeSee( zukan_savedata, param->pp );  // ����  // �}�Ӄt���O���Z�b�g����
          ZUKANSAVE_SetPokeGet( zukan_savedata, param->pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
        }

        // ZUKAN_TOROKU_PARAM�̐ݒ�
        {
          wk->zukan_toroku_param.pp               = param->pp;
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
          wk->nameinParam = NAMEIN_AllocParamPokemonCapture( wk->heapID, param->pp, NAMEIN_POKEMON_LENGTH, NULL,
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
        PP_Put( param->pp, ID_PARA_nickname, (u32)(wk->strbuf) );

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

      if( wk->box_strbuf == NULL ){
        PokeParty_Add( party, param->pp );
      }else{
        BOX_MANAGER* boxman = GAMEDATA_GetBoxManager( param->gameData );
        BOXDAT_PutPokemon( boxman, PP_GetPPPPointer(param->pp) );
      }
    }
    (*seq)++;
    break;
  case 4:
  default:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

