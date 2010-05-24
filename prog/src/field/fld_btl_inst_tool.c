//======================================================================
/**
 * @file  fld_btl_inst_tool.c
 * @brief  �{�݃o�g���Ăяo�����̋��ʕ���
 *
 * @note  �{�݁Finstitution
 * ���W���[�����FFBI_TOOL_
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "event_fieldmap_control.h"
#include "poke_tool/poke_regulation.h"
#include "app/pokelist.h"
#include "app/p_status.h"
#include "battle/battle.h"

#include "event_battle.h"

#include "fieldmap.h"

#include "field/field_msgbg.h"
#include "message.naix"

#include "fld_btl_inst_tool.h"


#include "item/itemsym.h"
#include "waza_tool/wazano_def.h"

#if 0
#ifdef PM_DEBUG
static BOOL check_TrainerType(u8 tr_type);
#endif  //PM_DEBUG
#endif

//--------------------------------------------------------------
/// �|�P�������X�g�I���C�x���g
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldmap;
  
  PSTATUS_DATA StatusData;
  PLIST_DATA ListData;
  REGULATION Regulation;
  
  PL_RETURN_TYPE *ReturnMode;
  PL_SELECT_POS *ResultSelect;
  u8 *ResultNoAry;

  POKEPARTY *SrcParty;
  POKEPARTY *DstParty;
}EVENT_WORK_POKE_LIST;

static GMEVENT_RESULT PokeSelEvt( GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * �|�P�������X�g�Ăяo���C�x���g�쐬
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param inType        ���X�g�^�C�v
 * @param inReg         ���M�����[�V����
 * @param pp            �Ώۃ|�P�p�[�e�B�|�C���^
 * @param outSelNum     �I�����X�g�ԍ��z��ւ̃|�C���^
 * @param outResult     ���X�g���ʑI�����ꂽ�ʒu
 * @param outRetMode    ���X�g�߂�^�C�v
 * @param outParty      �m�t�k�k�łȂ��ꍇ�A���ʂ��i�[����|�P�p�[�e�B
 *
 * @retval GMEVENT      �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *FBI_TOOL_CreatePokeListEvt(
    GAMESYS_WORK *gsys,
    const PL_LIST_TYPE inType, const PL_MODE_TYPE inListMode, const int inReg,
    POKEPARTY *inTargetParty,
    u8 *outSelNoAry, PL_SELECT_POS *outResult, PL_RETURN_TYPE *outRetMode, POKEPARTY *outParty )
{
  GMEVENT *event;
  FIELDMAP_WORK *fieldmap;
  EVENT_WORK_POKE_LIST *work;

  fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  event = GMEVENT_Create( gsys, NULL,
      PokeSelEvt, sizeof(EVENT_WORK_POKE_LIST) );

  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldmap = fieldmap;
  work->ResultNoAry = outSelNoAry;
  work->ResultSelect = outResult;
  work->ReturnMode = outRetMode;
  work->SrcParty = inTargetParty;
  work->DstParty = outParty;
  {
    PLIST_DATA *list = &work->ListData;
    PokeRegulation_LoadData(inReg, &work->Regulation);  //���M�����[�V�������[�h
    list->pp = inTargetParty;
    list->reg = &work->Regulation;
    list->type = inType;
    list->mode = inListMode;
  }
  {
    PSTATUS_DATA *st = &work->StatusData;
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    ZUKAN_SAVEDATA *zukanSave = GAMEDATA_GetZukanSave( gdata );
    MI_CpuClear8( st, sizeof(PSTATUS_DATA) );
    st->ppd = inTargetParty;
    st->ppt = PST_PP_TYPE_POKEPARTY;
    st->max = PokeParty_GetPokeCount( inTargetParty );
    st->mode = PST_MODE_NORMAL;
    st->page = PPT_INFO;
    st->zukan_mode = ZUKANSAVE_GetZenkokuZukanFlag( zukanSave );
  }

  return event;

}

//--------------------------------------------------------------
/**
 * �|�P�������X�g�I���C�x���g
 * @param event GMEVENT
 * @param seq event sequence
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PokeSelEvt( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK_POKE_LIST *work = wk;
  GAMESYS_WORK *gsys = work->gsys;
  
  switch( *seq )
  {
  case 0:
		GMEVENT_CallEvent( event,
        EVENT_FieldFadeOut_Black(gsys,work->fieldmap,FIELD_FADE_WAIT) );
    (*seq)++;
    break;
  case 1:
		GMEVENT_CallEvent( event, EVENT_FieldClose(gsys,work->fieldmap) );
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent( event, EVENT_PokeSelect(
          gsys,work->fieldmap, &work->ListData, &work->StatusData) );
    (*seq)++;
    break;
  case 3:
		GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
    (*seq)++;
    break;
  case 4:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black(
          gsys, work->fieldmap, FIELD_FADE_WAIT) );

    MI_CpuCopy8( work->ListData.in_num, work->ResultNoAry, 6 );
    *work->ResultSelect = work->ListData.ret_sel;
    *work->ReturnMode = work->ListData.ret_mode;

    //�|�P�p�[�e�B�Ƀf�[�^�Z�b�g
    if ( (*work->ResultSelect != PL_SEL_POS_EXIT) &&
         (*work->ResultSelect != PL_SEL_POS_EXIT2) && 
         (work->DstParty != NULL) ){
      int i;
      int num = PokeParty_GetPokeCountMax( work->DstParty );
      NOZOMU_Printf("party num %d\n", num);
      for( i=0;i < num; i++ )
      {
        POKEMON_PARAM *param;
        int pos;
        pos = work->ResultNoAry[i]-1;
        if( pos >= 6 ){
          GF_ASSERT( 0 );
          work->ResultNoAry[i] = 1;
        }
        param = PokeParty_GetMemberPointer( work->SrcParty, pos );
        PokeParty_Add( work->DstParty, param );
      }
    }
    (*seq)++;
    break;
  case 5:
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}



//===�ΐ�O���b�Z�[�W�o�͊֘A===
#include "bsubway_scrwork.h"

typedef struct
{
  GAMESYS_WORK *gsys;
  VecFx32 balloonWinPos;
  FLDTALKMSGWIN *balloonWin;
//  FLDSYSWIN_STREAM *sysWin;
  STRBUF *strBuf;
  u16 obj_id;
  u8 padding[2];
}EVENT_WORK_TRAINER_BEFORE_MSG;

static GMEVENT_RESULT TrainerBeforeMsgEvt( GMEVENT *event, int *seq, void *wk );


//--------------------------------------------------------------
/**
 * �ΐ�O���b�Z�[�W �Ăяo��
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * FBI_TOOL_CreateTrainerBeforeMsgEvt(
    GAMESYS_WORK *gsys, BSUBWAY_PARTNER_DATA *tr_data, int tr_idx, u16 obj_id )
{
  GMEVENT *event;
  GAMEDATA *gdata;
  FLDMSGBG *msgbg;
  FIELDMAP_WORK *fieldmap;
  EVENT_WORK_TRAINER_BEFORE_MSG *work;
  
  gdata = GAMESYSTEM_GetGameData( gsys );
  fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  msgbg = FIELDMAP_GetFldMsgBG( fieldmap );
  
  event = GMEVENT_Create( gsys, NULL,
    TrainerBeforeMsgEvt, sizeof(EVENT_WORK_TRAINER_BEFORE_MSG) );
  
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  
  if( tr_data->bt_trd.appear_word[0] == 0xffff ){ //ROM MSG
    GFL_MSGDATA *msgdata;
    
    work->strBuf = GFL_STR_CreateBuffer( 92, HEAPID_PROC );

    msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_tower_trainer_dat, HEAPID_PROC );
    
    GFL_MSG_GetString(
        msgdata, 
        tr_data->bt_trd.appear_word[1],
        work->strBuf );
    
    GFL_MSG_Delete( msgdata );
  }
  else
  { //�ȈՉ�b    
    PMS_DATA *pms = (PMS_DATA*)tr_data[tr_idx].bt_trd.appear_word;
    work->strBuf = PMSDAT_ToString( pms, HEAPID_PROC );
    OS_Printf( "BSW TRAINER BEFORE MSG : IDX = %d, KAIWA MSG\n", tr_idx );
/**    
    GFL_MSGDATA *msgdata;
    
    msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_tower_trainer_dat, HEAPID_PROC );
    
    GFL_MSG_GetString(
        msgdata,
        0,
        work->strBuf );
    
    GFL_MSG_Delete( msgdata );
*/    
  }

  { //mmdl
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys( gdata );
    MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
    MMDL_GetVectorPos( mmdl, &work->balloonWinPos );
  }
  
  { //�����o���E�B���h�E
     work->balloonWin = FLDTALKMSGWIN_AddStrBuf(
        msgbg,
        FLDTALKMSGWIN_IDX_LOWER,
        &work->balloonWinPos,
        work->strBuf, TALKMSGWIN_TYPE_NORMAL, TAIL_SETPAT_NONE );
  }
  
//  work->sysWin = FLDSYSWIN_STREAM_Add( msgbg, NULL, 19 );
  return( event );
}

//--------------------------------------------------------------
/**
 * �ΐ�O���b�Z�[�W
 * @param event GMEVENT
 * @param seq event sequence
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT TrainerBeforeMsgEvt(
    GMEVENT *event, int *seq, void *wk )
{
  
  EVENT_WORK_TRAINER_BEFORE_MSG *work = wk;
  GAMESYS_WORK *gsys = work->gsys;
/**  
  switch( *seq )
  {
  case 0:
    FLDSYSWIN_STREAM_PrintStrBufStart( work->sysWin, 0, 0, work->strBuf );
    (*seq)++;
    break;
  case 1:
    if( FLDSYSWIN_STREAM_Print(work->sysWin) == TRUE ){
      (*seq)++;
    }
    break;
  case 2:
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      FLDSYSWIN_STREAM_Delete( work->sysWin );
      GFL_STR_DeleteBuffer( work->strBuf );
      return( GMEVENT_RES_FINISH );
    }
    break;
  }
*/
  switch( *seq )
  {
  case 0:
    if( FLDTALKMSGWIN_Print(work->balloonWin) == TRUE ){
      (*seq)++;
    }
    break;
  case 1:
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      FLDTALKMSGWIN_StartClose( work->balloonWin );
      (*seq)++;
    }
    break;
  case 2:
    if( FLDTALKMSGWIN_WaitClose(work->balloonWin) == TRUE ){
      GFL_STR_DeleteBuffer( work->strBuf );
      return( GMEVENT_RES_FINISH );
    }
  }
  return( GMEVENT_RES_CONTINUE );
}


//===�o�g���p�����Z�b�g�֘A===

#include "gamesystem/btl_setup.h"
#include "sound/pm_sndsys.h"
#include "pm_define.h"
#include "tr_tool/trtype_def.h"
#include "battle/battle.h"

static BSP_TRAINER_DATA *CreateBSPTrainerData( HEAPID heapID );
static void MakePokePara( const BSUBWAY_POKEMON *src, POKEMON_PARAM *dest );
static u16 GetRand( void );
static u8 GetPowerRnd(u16 tr_no);
static void GetPokemonRomData( BSUBWAY_POKEMON_ROM_DATA *prd, int index );
static void * GetTrainerRomData( u16 tr_no, HEAPID heapID );

static BOOL IsValidEasyTalk(BSUBWAY_TRAINER *bsw_trainer);

//--------------------------------------------------------------
/// �A�C�e���e�[�u��
//  �����|�P��������̓����_���ł��Ă��邪�������[�v�h�~�̂��߁A
//  ������x�܂킵����A�|�P�����̕s��v�݂̂��`�F�b�N���āA
//  �A�C�e�����Œ�Ŏ������邽�߂̃A�C�e���e�[�u��
//--------------------------------------------------------------
static const u16 BattleTowerPokemonItem[]={
  ITEM_HIKARINOKONA,
  ITEM_RAMUNOMI,
  ITEM_TABENOKOSI,
  ITEM_SENSEINOTUME,
};

//--------------------------------------------------------------
/**
 * BATTLE_SETUP_PARAM�쐬
 * @param gsys GAMESYS_WORK
 * @retval BATTLE_SETUP_PARAM
 */
//--------------------------------------------------------------
BATTLE_SETUP_PARAM * FBI_TOOL_CreateBattleParam(
    GAMESYS_WORK *gsys, const POKEPARTY *my_party, int inPlayMode,
    BSUBWAY_PARTNER_DATA *partner_data,
    BSUBWAY_PARTNER_DATA *ai_multi_data,
    const int inMemNum )
{
  u16 play_mode;
  BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  HEAPID heapID = HEAPID_PROC;
  play_mode = inPlayMode;
  
#ifdef PM_DEBUG  
  { //�Ǝ��Z�b�g�A�b�v�̃������J���Y��`�F�b�N
    u32 size_before,size_after;
    
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( heapID );
    
    BTL_FIELD_SITUATION_SetFromFieldStatus(
        &sit, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
    
    size_before = GFL_HEAP_GetHeapFreeSize( heapID );
    dst = BATTLE_PARAM_Create( heapID );
    BTL_SETUP_Single_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    BATTLE_PARAM_Delete( dst );
    size_after = GFL_HEAP_GetHeapFreeSize( heapID );
    
    OS_Printf(
        "BTL_SETUP_Single_Trainer() before=%xH, after=%xH\n",
        size_before, size_after );
    
    if( size_before != size_after ){
      GF_ASSERT( 0 );
    }
    
    size_before = GFL_HEAP_GetHeapFreeSize( heapID );
    dst = BATTLE_PARAM_Create( heapID );
    BTL_SETUP_Double_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    BATTLE_PARAM_Delete( dst );
    size_after = GFL_HEAP_GetHeapFreeSize( heapID );
    
    OS_Printf(
        "BTL_SETUP_Double_Trainer() before=%xH, after=%xH\n",
        size_before, size_after );
    
    if( size_before != size_after ){
      GF_ASSERT( 0 );
    }
    
    size_before = GFL_HEAP_GetHeapFreeSize( heapID );
    dst = BATTLE_PARAM_Create( heapID );
    BTL_SETUP_AIMulti_Trainer( dst, gdata, &sit,
          TRID_NULL, TRID_NULL, TRID_NULL, heapID );
    BATTLE_PARAM_Delete( dst );
    size_after = GFL_HEAP_GetHeapFreeSize( heapID );
    
    OS_Printf(
        "BTL_SETUP_AIMulti_Trainer() before=%xH, after=%xH\n",
        size_before, size_after );
    
    if( size_before != size_after ){
      GF_ASSERT( 0 );
    }

    {
      int commPos = 0;
      GFL_NETHANDLE *netHandle = GFL_NET_HANDLE_GetCurrentHandle();
      
      size_before = GFL_HEAP_GetHeapFreeSize( heapID );
      dst = BATTLE_PARAM_Create( heapID );
      BTL_SETUP_AIMulti_Comm( dst, gdata,
            netHandle, BTL_COMM_DS, commPos,
            TRID_NULL, TRID_NULL, heapID );
      BATTLE_PARAM_Delete( dst );
      size_after = GFL_HEAP_GetHeapFreeSize( heapID );
      
      OS_Printf(
          "BTL_SETUP_AIMulti_Comm() before=%xH, after=%xH\n",
          size_before, size_after );
    
      if( size_before != size_after ){
        GF_ASSERT( 0 );
      }
    }
  }
#endif
  
  {
//    BTL_FIELD_SITUATION_Init( &sit );
    BTL_FIELD_SITUATION_SetFromFieldStatus(
        &sit, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
    dst = BATTLE_PARAM_Create( heapID );

    //������
    switch( play_mode ){
    case BSWAY_MODE_SINGLE:
    case BSWAY_MODE_S_SINGLE:
    case BSWAY_MODE_WIFI:
      BTL_SETUP_Single_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
      break;
    case BSWAY_MODE_DOUBLE:
    case BSWAY_MODE_S_DOUBLE:
      BTL_SETUP_Double_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
      break;
    case BSWAY_MODE_MULTI:
    case BSWAY_MODE_S_MULTI:
      BTL_SETUP_AIMulti_Trainer( dst, gdata, &sit,
          TRID_NULL, TRID_NULL, TRID_NULL, heapID );
      break;
    case BSWAY_MODE_COMM_MULTI:
    case BSWAY_MODE_S_COMM_MULTI:
      {
        GFL_NETHANDLE *netHandle = GFL_NET_HANDLE_GetCurrentHandle();
        int commPos = 0;
      
        if( GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE ){
          commPos = 2; //�ʐM�q�ł���
        }
      
        BTL_SETUP_AIMulti_Comm( dst, gdata,
            netHandle, BTL_COMM_DS, commPos,
            TRID_NULL, TRID_NULL, heapID );
      }
      break;
    default:
      GF_ASSERT( 0 );
    }

    BTL_SETUP_AllocRecBuffer( dst, heapID );

  }
  
#if 0  
  { //�g���[�i�[�f�[�^�m��
    dst->tr_data[BTL_CLIENT_ENEMY1] = CreateBSPTrainerData( heapID );
  }
#endif
  
  { //�v���C���[�Z�b�g
    BTL_CLIENT_ID client = BTL_CLIENT_PLAYER;
    MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
    BSP_TRAINER_DATA *data;

    //MyStatus
    dst->playerStatus[client] = mystatus;
    //�g���[�i�[�f�[�^�m��
    dst->tr_data[client] = CreateBSPTrainerData( heapID );
    data = dst->tr_data[client];

    MyStatus_CopyNameString(
        (const MYSTATUS*)mystatus, data->name );
    data->tr_type = TRTYPE_HERO +
      MyStatus_GetMySex((const MYSTATUS*)mystatus );

    //�|�P�����Z�b�g
    {
      int i;
      POKEMON_PARAM *entry_pp;
      const POKEMON_PARAM *my_pp;
      POKEPARTY *party = dst->party[client];
      PokeParty_Init( party, TEMOTI_POKEMAX );
      
      entry_pp = GFL_HEAP_AllocMemoryLo(
          heapID, POKETOOL_GetWorkSize() );
      PP_Clear( entry_pp );

      for( i = 0; i < inMemNum; i++ ){
        my_pp = PokeParty_GetMemberPointer( my_party,i );

        POKETOOL_CopyPPtoPP( (POKEMON_PARAM*)my_pp, entry_pp );

        //���x���T�O�␳
        if( PP_Get(my_pp,ID_PARA_level,NULL) != 50 )
        {
          POKETOOL_MakeLevelRevise(entry_pp, 50);
        }

        PokeParty_Add( party, entry_pp );
      }
      GFL_HEAP_FreeMemory( entry_pp );
    }
  }
  
  { //�G�g���[�i�[�Z�b�g
    PMS_DATA *pd;
    BSP_TRAINER_DATA *tr_data;
    BTL_CLIENT_ID client = BTL_CLIENT_ENEMY1;
    BSUBWAY_PARTNER_DATA *bsw_partner;
    BSUBWAY_TRAINER *bsw_trainer;

    //�g���[�i�[�f�[�^�m��
//�o�g���Z�b�g�A�b�v�Ő��������悤�ɂȂ����̂ŕs�v�ƂȂ���
//    dst->tr_data[client] = CreateBSPTrainerData( heapID );
    tr_data = dst->tr_data[client];

    bsw_partner = &partner_data[0];
    bsw_trainer = &bsw_partner->bt_trd;

    if ( play_mode == BSWAY_MODE_WIFI ) tr_data->tr_id = 0;
    else tr_data->tr_id = bsw_trainer->player_id;

    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //�ŋ�

    //name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );

    //win word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    //�T�u�E�F�C�p�[�g�i�[�f�[�^�̏��������f�[�^���O�N���A����Ă���Ȃ�΁A
    //�g���[�i�[�f�[�^�̃��[�N���N���A���ēn��
    //�O�N���A����ĂȂ���΁A�ȈՉ�b�������Ă���Ƃ݂Ȃ��A�f�[�^���Z�b�g����
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }
    else if ( IsValidEasyTalk(bsw_trainer) )   //�ȈՉ�b����
    {
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }

    //�G�|�P�����Z�b�g
    {
      int i;
      POKEMON_PARAM*  pp;
      POKEPARTY *party = dst->party[client];
      PokeParty_Init( party, TEMOTI_POKEMAX );
      pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
      
      for( i = 0; i < inMemNum; i++ ){
        MakePokePara( &(bsw_partner->btpwd[i]), pp );
        PokeParty_Add( party, pp );
      }
      GFL_HEAP_FreeMemory( pp );
    }

    if( dst->multiMode != BTL_MULTIMODE_NONE ) //�}���`
    { //�G�g���[�i�[�Q�ݒ�
      client = BTL_CLIENT_ENEMY2;
//�o�g���Z�b�g�A�b�v�Ő��������悤�ɂȂ����̂ŕs�v�ƂȂ���
//    dst->tr_data[client] = CreateBSPTrainerData( heapID );
      tr_data = dst->tr_data[client];
      bsw_partner = &partner_data[1];
      bsw_trainer = &bsw_partner->bt_trd;
    
      tr_data->tr_id = bsw_trainer->player_id;
      tr_data->tr_type = bsw_trainer->tr_type;
      tr_data->ai_bit = 0x00000087;  //�ŋ�
    
      //�g���[�i�[�f�[�^�@name
      GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
      //�g���[�i�[�f�[�^�@word
      //���ɐݒ薳��
    
      //�|�P�����p�[�e�B
      {
        int i;
        POKEMON_PARAM*  pp;
        POKEPARTY *party = dst->party[client];
        PokeParty_Init( party, TEMOTI_POKEMAX );
        pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
        for( i = 0; i < inMemNum; i++ ){
          MakePokePara( &(bsw_partner->btpwd[i]), pp );
          PokeParty_Add( party, pp );
        }
        GFL_HEAP_FreeMemory( pp );
      }
    }

    if( dst->multiMode == BTL_MULTIMODE_PA_AA ) //AI�}���`
    { //AI�p�[�g�i�[�ݒ�
      client = BTL_CLIENT_PARTNER;
      
      //�g���[�i�[�f�[�^
//�o�g���Z�b�g�A�b�v�Ő��������悤�ɂȂ����̂ŕs�v�ƂȂ���
//      dst->tr_data[client] = CreateBSPTrainerData( heapID );
      tr_data = dst->tr_data[client];
      
      bsw_partner = ai_multi_data;
      bsw_trainer = &bsw_partner->bt_trd;

      tr_data->tr_id = bsw_trainer->player_id;
      tr_data->tr_type = bsw_trainer->tr_type;
      tr_data->ai_bit = 0x00000087;  //�ŋ�
    
      //�g���[�i�[�f�[�^�@name
      GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
      //�g���[�i�[�f�[�^�@word
      PMSDAT_Clear( &tr_data->win_word );
      PMSDAT_Clear( &tr_data->lose_word );
    
      //�|�P�����p�[�e�B
      {
        int i;
        POKEMON_PARAM*  pp;
        POKEPARTY *party = dst->party[client];
        PokeParty_Init( party, TEMOTI_POKEMAX );
        pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
        for( i = 0; i < inMemNum; i++ ){
          MakePokePara( &(bsw_partner->btpwd[i]), pp );
          PokeParty_Add( party, pp );
        }
        GFL_HEAP_FreeMemory( pp );
      }
    }
  }

  BTL_SETUP_SetSubwayMode( dst ); //��ʂ�Z�b�g������ɌĂԎ�
  return dst;
}

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�g���[�i�[�f�[�^����
 * @param heapID HEAPID
 * @retval BSP_TRAINER_DATA*
 */
//--------------------------------------------------------------
static BSP_TRAINER_DATA *CreateBSPTrainerData( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );
  return tr_data;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�̃|�P�����p�����[�^����
 *  @param pwd  [in/out]�|�P�����p�����[�^�̓W�J�ꏊ
 *  @param poke_no  [in]�^���[rom�f�[�^�|�P�����i���o�[
 *  @param poke_id  [in]�|�P�����ɃZ�b�g����id
 *  @param poke_rnd [in]�Z�b�g���������(0�������n���ꂽ��֐����Ő���)
 *  @param pow_rnd  [in]�|�P�����ɃZ�b�g����pow_rnd�l
 *  @param mem_idx  [in]�����o�[index�B��̖�or��̖�
 *  @param itemfix  [in]TRUE�Ȃ�Œ�A�C�e���BFALSE�Ȃ�rom�f�[�^�̃A�C�e��
 *  @param heapID [in]�e���|�������������m�ۂ���q�[�vID
 *  @retval u32 personal_rnd:�������ꂽ�|�P�����̌������l
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_PokemonParamMake
 */
//--------------------------------------------------------------
u32 FBI_TOOL_MakePokemonParam(
    BSUBWAY_POKEMON *pwd,
    u16 poke_no, u32 poke_id, u32 poke_rnd, u8 pow_rnd,
    u8 mem_idx, BOOL itemfix, HEAPID heapID )
{
  int i;
  int  exp;
  u32  personal_rnd;
  u8  friend;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;

  MI_CpuClear8(pwd,sizeof(BSUBWAY_POKEMON));

  //�����f�[�^���[�h
  GetPokemonRomData(&prd_s,poke_no);

  //�����X�^�[�i���o�[
  pwd->mons_no=prd_s.mons_no;

  //�t�H�����i���o�[
  pwd->form_no=prd_s.form_no;

  //��������
  if(itemfix){
    //50��ȏ�܂킵�Ă����t���O��TURE�Ȃ�A
    //�����A�C�e���͌Œ�̂��̂���������
    pwd->item_no=BattleTowerPokemonItem[mem_idx];
  }else{
    //rom�f�[�^�ɓo�^���ꂽ�A�C�e������������
    pwd->item_no=prd_s.item_no;
  }

  //�Ȃ��x��255���f�t�H���g
  friend=255;
  for(i=0;i<WAZA_TEMOTI_MAX;i++){
    pwd->waza[i]=prd_s.waza[i];
    //�������������Ă���Ƃ��́A�Ȃ��x��0�ɂ���
    if(prd_s.waza[i]==WAZANO_YATUATARI){
      friend=0;
    }
  }

  //IDNo
  pwd->id_no=poke_id;

  if(poke_rnd == 0){
    //������          //���v�a�ł͐��i�ƌ������͐؂藣���čl���ėǂ��̂ŁA�Ē��I�����͕s�v
    personal_rnd = POKETOOL_CalcPersonalRandSpec( poke_id, prd_s.mons_no, prd_s.form_no,
        PTL_SEX_SPEC_UNKNOWN, PTL_TOKUSEI_SPEC_BOTH, PTL_RARE_SPEC_FALSE );
    pwd->personal_rnd = personal_rnd;
  }else{
    pwd->personal_rnd = poke_rnd;  //0�łȂ���Έ����̒l���g�p
    personal_rnd = poke_rnd;
  }

  //���i�Z�b�g
  pwd->seikaku=prd_s.chr;

  //�p���[����
  pwd->hp_rnd=pow_rnd;
  pwd->pow_rnd=pow_rnd;
  pwd->def_rnd=pow_rnd;
  pwd->agi_rnd=pow_rnd;
  pwd->spepow_rnd=pow_rnd;
  pwd->spedef_rnd=pow_rnd;

  //�w�͒l
#if 0 //wb null
  exp=0;
  for(i=0;i<6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      exp++;
    }
  }
  if((PARA_EXP_TOTAL_MAX/exp)>255){
    exp=255;
  }else{
    exp=PARA_EXP_TOTAL_MAX/exp;
  }
  for(i = 0;i < 6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      pwd->exp[i]=exp;
    }
  }
#endif

  //�Z�|�C���g
  pwd->pp_count=0;

  //���R�[�h
  pwd->country_code = 0;

  //����
  i= POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi2);
  if(i){
    if(pwd->personal_rnd&1){
      pwd->tokusei=i;
    }else{
      pwd->tokusei=POKETOOL_GetPersonalParam(pwd->mons_no,0,POKEPER_ID_speabi1);
    }
  }else{
    pwd->tokusei=POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi1);
  }

  //�Ȃ��x
  pwd->natuki=friend;

  //�j�b�N�l�[��
  {
   GFL_MSGDATA *msgdata = GFL_MSG_Create(
       GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat,
       heapID );
    GFL_MSG_GetStringRaw( msgdata,
        pwd->mons_no, pwd->nickname, MONS_NAME_SIZE+EOM_SIZE );
    GFL_MSG_Delete( msgdata );
  }
  return personal_rnd;
}

//--------------------------------------------------------------
/**
 *  @brief �o�g���T�u�E�F�C�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
 *  @param src BSUBWAY_POKEMON
 *  @param dest POKEMON_PARAM
 *  @retval nothing
 */
//--------------------------------------------------------------
static void MakePokePara( const BSUBWAY_POKEMON *src, POKEMON_PARAM *dest )
{
  int i;
  u8 buf8,waza_pp;
  u16 buf16;
  u32 buf32;

  PP_Clear(dest);

  //monsno,level,pow_rnd,idno
  PP_SetupEx( dest, src->mons_no, 50, PTL_SETUP_ID_AUTO,
      (src->power_rnd & 0x3FFFFFFF), src->personal_rnd);

  //form_no
  buf8 = src->form_no;
  PP_Put( dest, ID_PARA_form_no, buf8 );

  //�����A�C�e���ݒ�
  PP_Put( dest,ID_PARA_item, src->item_no );

  //�Z�ݒ�
  for(i = 0;i < 4;i++){
    buf16 = src->waza[i];
    PP_Put(dest,ID_PARA_waza1+i,buf16);
    buf8 = (src->pp_count >> (i*2))&0x03;
    PP_Put(dest,ID_PARA_pp_count1+i, buf8);

    //pp�Đݒ�
    waza_pp = (u8)PP_Get(dest,ID_PARA_pp_max1+i,NULL);
    PP_Put(dest,ID_PARA_pp1+i,waza_pp);
  }

  //ID�ݒ�
  buf32 = src->id_no;
  PP_Put(dest,ID_PARA_id_no,buf32);

  //�o���l�ݒ�
  buf8 = src->hp_exp;
  PP_Put(dest,ID_PARA_hp_exp,buf8);
  buf8 = src->pow_exp;
  PP_Put(dest,ID_PARA_pow_exp,buf8);
  buf8 = src->def_exp;
  PP_Put(dest,ID_PARA_def_exp,buf8);
  buf8 = src->agi_exp;
  PP_Put(dest,ID_PARA_agi_exp,buf8);
  buf8 = src->spepow_exp;
  PP_Put(dest,ID_PARA_spepow_exp,buf8);
  buf8 = src->spedef_exp;
  PP_Put(dest,ID_PARA_spedef_exp,buf8);

  //�����ݒ�
  PP_Put(dest,ID_PARA_speabino, src->tokusei);

  //�Ȃ��ǐݒ�
  PP_Put(dest,ID_PARA_friend,src->natuki);

  //���i�Z�b�g
  PP_SetSeikaku( dest, src->seikaku );
  
  { //���O�ݒ�
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        MONS_NAME_SIZE+EOM_SIZE, HEAPID_PROC );
    {
      int i;
      u16 eomCode = GFL_STR_GetEOMCode();
      STRCODE code[MONS_NAME_SIZE+EOM_SIZE];
      const STRCODE *sz = src->nickname;
      
      for( i = 0; i < (MONS_NAME_SIZE+EOM_SIZE); i++ ){
        code[i] = sz[i];
      }
      
      code[i-1] = eomCode;
      GFL_STR_SetStringCode( nick_name, code );
    }
     
    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }

#if 0  
  //NG�l�[���t���O���`�F�b�N
  if( 1 ){ //�f�t�H���g�l�[����W�J����
#if 0
    GFL_MSGDATA *msgdata;
    STRBUF *def_name;

    msgdata= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
        ARCID_MESSAGE, NARC_message_monsname_dat, HEAPID_PROC );
    def_name = GFL_STR_CreateBuffer( HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );

    GFL_MSG_GetString( msgdata, src->mons_no, def_name );
    PP_Put( dest, ID_PARA_nickname, (u32)def_name );

    GFL_STR_DeleteBuffer( def_name );
    GFL_MSG_Delete( msgdata );
#endif
  }else{ //�j�b�N�l�[��
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );
    GFL_STR_SetStringCode( nick_name, src->nickname );
    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }
#endif
  PP_Put(dest,ID_PARA_country_code,src->country_code); //�J���g���[�R�[�h

  //�p�����[�^�Čv�Z
  {
    u32 lv = PP_CalcLevel( dest );
    PP_Put( dest, ID_PARA_level,lv );
  }
}

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�|�P���������f�[�^�̓ǂݏo��
 * @param[in]  prd    �ǂݏo�����|�P�����f�[�^�̊i�[��
 * @param[in]  index  �ǂݏo���|�P�����f�[�^�̃C���f�b�N�X
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_PokemonRomDataGet
 */
//--------------------------------------------------------------
static void GetPokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index)
{
//  index += BSW_PM_ARCDATANO_ORG;
  GFL_ARC_LoadData( (void*)prd, ARCID_BSW_PD, index );
}

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�g���[�i�[�����f�[�^�̓ǂݏo��
 * @param tr_no  [in]�g���[�i�[�i���o�[
 * @param heapID  [in]�������m�ۂ��邽�߂̃q�[�vID
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_TrainerRomDataGet
 */
//--------------------------------------------------------------
static void * GetTrainerRomData( u16 tr_no, HEAPID heapID )
{
  tr_no += BSW_TR_ARCDATANO_ORG;
  OS_Printf( "BSUBWAY load TrainerRomData Num = %d\n", tr_no );
  return GFL_ARC_UTIL_Load( ARCID_BSW_TR, tr_no, 0, heapID );
}

//--------------------------------------------------------------
/**
 * �����擾
 * @retval u16 ����
 * @note �����Œ�̖��c
 */
//--------------------------------------------------------------
static u16 GetRand( void )
{
  return( GFUser_GetPublicRand(0xffffffff)/65535 );
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�g���[�i�[�̎����|�P�����̃p���[���������肷��
 * @param  tr_no  �g���[�i�[�i���o�[
 * @return  �p���[����
 * b_tower_fld.c��b_tower_ev�Ɉړ�
 */
//--------------------------------------------------------------
static u8 GetPowerRnd(u16 tr_no)
{
  u8  pow_rnd;

  if(tr_no<100){
    pow_rnd=(0x1f/8)*1;
  }
  else if(tr_no<120){
    pow_rnd=(0x1f/8)*2;
  }
  else if(tr_no<140){
    pow_rnd=(0x1f/8)*3;
  }
  else if(tr_no<160){
    pow_rnd=(0x1f/8)*4;
  }
  else if(tr_no<180){
    pow_rnd=(0x1f/8)*5;
  }
  else if(tr_no<200){
    pow_rnd=(0x1f/8)*6;
  }
  else if(tr_no<220){
    pow_rnd=(0x1f/8)*7;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�g���[�i�[�f�[�^����
 * �i�����f�[�^��BSUBWAY_PARTNER_DATA�\���̂ɓW�J�j
 * @param tr_data  [in/out]��������BSUBWAY_PARTNAER_DATA�\����
 * @param tr_no [in]�������ɂȂ�g���[�i�[ID
 * @param cnt [in]�g���[�i�[�Ɏ�������|�P�����̐�
 * @param set_poke_no [in]�y�A�g���[�i�[�̃|�P�����iNULL �`�F�b�N�Ȃ��j
 * @param set_item_no [in]�y�A�̃|�P�����̑����A�C�e���iNULL �`�F�b�N�Ȃ��j
 * @param poke [in/out] ���I�|�P������̂̃p�����[�^�i�[��(NULL���ƂȂ��j
 * @param heapID  [in]�q�[�vID
 * @retval TRUE ���I���[�v��50�񒴂��� FALSE ���I���[�v��50��ȓ��ŏI�����
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY ��������tower_tool.c FSRomBattleTowerTrainerDataMake
 * TOWER_AI_MULTI_ONLY �T�����ŁA5�l�O�̎莝���|�P�����𐶐����Ă���
 */
//--------------------------------------------------------------
BOOL FBI_TOOL_MakeRomTrainerData(
    BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no, int cnt,
    u16 *set_poke_no, u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM* poke, HEAPID inHeapID)
{
  BOOL      ret = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;

  //�g���[�i�[�f�[�^�Z�b�g
  trd = FBI_TOOL_AllocTrainerRomData(tr_data,tr_no, inHeapID);

  //�|�P�����f�[�^���Z�b�g
  ret = FBI_TOOL_SetBSWayPokemonParam(
      trd, tr_no, &tr_data->btpwd[0],cnt,
      set_poke_no, set_item_no, poke, inHeapID);
  GFL_HEAP_FreeMemory( trd );
  return ret;
}

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�@rom�g���[�i�[�f�[�^�W�J
 *  BSUBWAY_TRAINER_ROM_DATA�^���������m�ۂ��ĕԂ��̂ŁA
 *  �Ăяo�����������I�ɉ�����邱��
 *  @param tr_data BSUBWAY_PARTER_DATA
 *  @param tr_no �g���[�i�[�i���o�[
 *  @param heapID HEAPID
 *  @retval BSUBWAY_TRAINER_ROM_DATA*
 *  TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 *  TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_TrainerDataGet
 */
//--------------------------------------------------------------
BSUBWAY_TRAINER_ROM_DATA * FBI_TOOL_AllocTrainerRomData(
    BSUBWAY_PARTNER_DATA *tr_data, u16 tr_no, HEAPID inHeapID )
{
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  GFL_MSGDATA *msgdata;
  STRBUF *name;

  msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_btdtrname_dat, inHeapID );
  
  MI_CpuClear8(tr_data, sizeof(BSUBWAY_PARTNER_DATA));
  trd = GetTrainerRomData(tr_no, inHeapID);

  //�g���[�i�[ID���Z�b�g
  tr_data->bt_trd.player_id = tr_no + 1; //1 origin

  //�g���[�i�[�o�����b�Z�[�W
  tr_data->bt_trd.appear_word[0] = 0xFFFF;
  tr_data->bt_trd.appear_word[1] = tr_no*3;
  
  //�g���[�i�[�f�[�^���Z�b�g
  tr_data->bt_trd.tr_type=trd->tr_type;

  NOZOMU_Printf("trno=%d type=%d\n",tr_data->bt_trd.player_id, trd->tr_type);

#if 0  
#ifdef PM_DEBUG  
#if 1 
  //GS�f�[�^����̈ڐA�ɂ�鏈��
  //wb�ł͑��݂��Ă��Ȃ��^�C�v����������
  #if 1
  if( check_TrainerType(trd->tr_type) == FALSE ){
    GF_ASSERT_MSG(0,"unknown trainer trno=%d type=%d",tr_data->bt_trd.player_id, trd->tr_type);
    tr_data->bt_trd.tr_type = TRTYPE_TANPAN;
  }
  #endif
#endif
#endif  //PM_DEBUG
#endif  
  name = GFL_MSG_CreateString( msgdata, tr_no );
  GFL_STR_GetStringCode( name,
      &tr_data->bt_trd.name[0], BUFLEN_PERSON_NAME );
  GFL_STR_DeleteBuffer(name);
  GFL_MSG_Delete( msgdata );
  return trd;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�̃|�P���������߂�
 * @param trd [in]�g���[�i�[�f�[�^
 * @param tr_no [in]�g���[�i�[�i���o�[
 * @param pwd [in/out]BSUBWAY_POKEMON�\����
 * @param cnt [in]�g���[�i�[�Ɏ�������|�P�����̐�
 * @param set_poke_no [in]�y�A�g���[�i�[�|�P���� (NULL���ƃ`�F�b�N�Ȃ�
 * @param set_item_no [in]�y�A�g���[�i�[�|�P�����A�C�e�� (NULL �`�F�b�N�Ȃ�
 * @param poke [in/out]���I�|�P������̂̃p�����[�^�i�[��(NULL �`�F�b�N�Ȃ�
 * @param heapID [in]�q�[�vID
 * @retval  FALSE  ���I���[�v��50��ȓ��ŏI�����
 * @retval  TRUE  ���I���[�v��50��ȓ��ŏI���Ȃ�����
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 */
//--------------------------------------------------------------
BOOL FBI_TOOL_SetBSWayPokemonParam(
    BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no, BSUBWAY_POKEMON *pwd, u8 cnt,
    u16 *set_poke_no, u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID )
{
  int  i,j;
  u8  pow_rnd;
  u8  poke_index;
  u32  id;
  int  set_index;
  int  set_index_no[4];
  u32  set_rnd_no[4];
  u8   set_seikaku[4];
  int  set_count;
  int  loop_count;
  BOOL  ret = 0;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;
  BSUBWAY_POKEMON_ROM_DATA  prd_d;

  //�莝���|�P������MAX��4�̂܂�
  GF_ASSERT(cnt<=4);

  set_count=0;
  loop_count=0;
  while(set_count!=cnt){
//    poke_index = gf_rand()%trd->use_poke_cnt;
    poke_index = GetRand()%trd->use_poke_cnt;
    set_index=trd->use_poke_table[poke_index];
    GetPokemonRomData(&prd_d,set_index);

    //�����X�^�[�i���o�[�̃`�F�b�N�i����̃|�P�����͎����Ȃ��j
    for(i=0;i<set_count;i++){
      GetPokemonRomData(&prd_s,set_index_no[i]);
      if(prd_s.mons_no==prd_d.mons_no){
        break;
      }
    }
    if(i!=set_count){
      continue;
    }

    //�y�A��g��ł���g���[�i�[�̎����|�P�����Ƃ̃`�F�b�N
    if(set_poke_no!=NULL){
      //�����X�^�[�i���o�[�̃`�F�b�N�i����̃|�P�����͎����Ȃ��j
      for(i=0;i<cnt;i++){
        if(set_poke_no[i]==prd_d.mons_no){
          break;
        }
      }
      if(i!=cnt){
        continue;
      }
    }

    //50��܂킵�āA���܂�Ȃ��悤�Ȃ�A����A�C�e���`�F�b�N�͂��Ȃ�
    if(loop_count<50){
      //�����A�C�e���̃`�F�b�N�i����̃A�C�e���͎����Ȃ��j
      for(i=0;i<set_count;i++){
        GetPokemonRomData(&prd_s,set_index_no[i]);
        if((prd_s.item_no)&&(prd_s.item_no==prd_d.item_no)){
          break;
        }
      }
      if(i!=set_count){
        loop_count++;
        continue;
      }
      //�y�A��g��ł���g���[�i�[�̎����|�P�����̑����A�C�e���Ƃ̃`�F�b�N
      if(set_item_no!=NULL){
        //�����A�C�e���̃`�F�b�N�i����̃A�C�e���͎����Ȃ��j
        for(i=0;i<cnt;i++){
          if((set_item_no[i]==prd_d.item_no) && (set_item_no[i]!=0)){
            break;
          }
        }
        if(i!=cnt){
          loop_count++;
          continue;
        }
      }
    }

    set_index_no[set_count]=set_index;
    set_seikaku[set_count] = prd_d.chr;
    set_count++;
  }

  pow_rnd=GetPowerRnd(tr_no);
  id=(GetRand()|(GetRand()<<16));

  if(loop_count >= 50){
    ret = TRUE;
  }
  for(i=0;i<set_count;i++){
    set_rnd_no[i] = FBI_TOOL_MakePokemonParam(&(pwd[i]),
      set_index_no[i],id,0,pow_rnd,i,ret,heapID);
  }
  if(poke == NULL){
    return ret;
  }
  //�|�C���^��NULL�łȂ���΁A���I���ꂽ�|�P�����̕K�v�ȃp�����[�^��Ԃ�
  poke->poke_tr_id = id;
  for(i = 0;i< BSUBWAY_STOCK_PAREPOKE_MAX;i++){
    poke->poke_no[i] = set_index_no[i];
    poke->poke_rnd[i] = set_rnd_no[i];
    poke->seikaku[i] = set_seikaku[i];
  }
  return ret;
}

//--------------------------------------------------------------
/**
 * �ȈՉ�b���݂��邩�H
 * @param bsw_trainer   �o�g���T�u�E�F�C�g���[�i�[�f�[�^
 * @retval  TRUE  ����@FFALSE �Ȃ�
 */
//--------------------------------------------------------------
static BOOL IsValidEasyTalk(BSUBWAY_TRAINER *bsw_trainer)
{
  int i;
  for (i=0;i<4;i++)
  {
    if ( bsw_trainer->win_word[i] != 0 ) return TRUE;
    if ( bsw_trainer->lose_word[i] != 0 ) return TRUE;
  }

  return FALSE;
}



//===�n�a�i�R�[�h�擾===
static const u16 btower_trtype2objcode[][2] =
{
 {TRTYPE_TANPAN,  BOY2},  ///<����p��������
 {TRTYPE_MINI,  GIRL2},  ///<�~�j�X�J�[�g
 {TRTYPE_SCHOOLB,  BOY3},  ///<���キ������
 {TRTYPE_SCHOOLG, GIRL3 },  ///<���キ������
 {TRTYPE_PRINCE,  BOY4},  ///<���ڂ������
 {TRTYPE_PRINCESS,  GIRL4},  ///<�����傤����
 {TRTYPE_KINDERGARTENM, BABYBOY2},    //���񂶁�
 {TRTYPE_KINDERGARTENW, BABYGIRL2},    //���񂶁�
 {TRTYPE_BACKPACKERM, BACKPACKERM},//�o�b�N�p�b�J�[��
 {TRTYPE_BACKPACKERW, BACKPACKERW},//�o�b�N�p�b�J�[��
 {TRTYPE_WAITER,WAITER}, //�E�G�[�^�[
 {TRTYPE_WAITRESS,WAITRESS}, //�E�G�[�g���X
 {TRTYPE_DAISUKIM,  MIDDLEMAN1},  ///<���������N���u
 {TRTYPE_DAISUKIW,  MIDDLEWOMAN1},  ///<���������N���u
 {TRTYPE_DOCTOR, DOCTOR },   //�h�N�^�[
 {TRTYPE_NURSE,  NURSE},    //�i�[�X
 {TRTYPE_CYCLINGM,  CYCLEM},  ///<�T�C�N�����O��
 {TRTYPE_CYCLINGW,  CYCLEW},  ///<�T�C�N�����O��
 {TRTYPE_GENTLE,  GENTLEMAN},  ///<�W�F���g���}��
 {TRTYPE_MADAM,  LADY},  ///<�}�_��
 {TRTYPE_BREEDERM,  BREEDERM},  ///<�|�P�����u���[�_�[
 {TRTYPE_BREEDERW,  BREEDERW},  ///<�|�P�����u���[�_�[
 {TRTYPE_SCIENTISTM,  ASSISTANTM},  //���񂫂イ����
 {TRTYPE_SCIENTISTW,  ASSISTANTW},  //���񂫂イ����
 {TRTYPE_ESPM,  ESPM},  ///<�T�C�L�b�J�[
 {TRTYPE_ESPW,  ESPW},  ///<�T�C�L�b�J�[
 {TRTYPE_KARATE,  FIGHTERM},  //����Ă���
 {TRTYPE_BATTLEG,  FIGHTERW},  ///<�o�g���K�[��
 {TRTYPE_RANGERM,  RANGERM},  ///<�|�P���������W���[
 {TRTYPE_RANGERW,  RANGERW},  ///<�|�P���������W���[
 {TRTYPE_ELITEM,  TRAINERM},  ///<�G���[�g�g���[�i�[
 {TRTYPE_ELITEW,  TRAINERW},  ///<�G���[�g�g���[�i�[
 {TRTYPE_VETERANM,  VETERANM},  ///<�x�e�����g���[�i�[
 {TRTYPE_VETERANW,  VETERANW},  ///<�x�e�����g���[�i�[
 {TRTYPE_FISHING,  FISHING},  ///<��т�
 {TRTYPE_MOUNT,  MOUNTMAN},  ///<��܂��Ƃ�
 {TRTYPE_WORKER1,  WORKMAN},  ///<�����傤����
 {TRTYPE_WORKER2,  WORKMAN2},  ///<�����傤����
 {TRTYPE_JUGGLING, CLOWN },   //�N���E��
 {TRTYPE_ARTIST,  OLDMAN1},  ///<���������
 {TRTYPE_POLICE,  POLICEMAN},  ///<���܂�肳��
 {TRTYPE_HEADS,  BADMAN},  ///<�X�L���w�b�Y
 {TRTYPE_BADRIDER, BADRIDER},   //�ڂ���������
 {TRTYPE_CLEANING, CLEANINGM},   //������������
 {TRTYPE_RAIL,RAILMAN }, //�Ăǂ�����
 {TRTYPE_PILOT, PILOT},   //�p�C���b�g
 {TRTYPE_BUSINESS1, BUSINESSMAN},    //�r�W�l�X�}��1
 {TRTYPE_BUSINESS2, BUSINESSMAN},    //�r�W�l�X�}��2
 {TRTYPE_PARASOL,  AMBRELLA},  ///<�p���\�����˂�����
 {TRTYPE_BAKER,     BAKER },   //�x�[�J���[
 {TRTYPE_CHILDCARE, WOMAN3}, //�ق�����
 {TRTYPE_MAID, MAID},    //���C�h
 {TRTYPE_OL, OL},    //�n�k
};

#define TRTYPE2OBJCODE_MAX  (NELEMS(btower_trtype2objcode))

//--------------------------------------------------------------
/**
 * @brief  �g���[�i�[�^�C�v����l��OBJ�R�[�h��Ԃ�
 * @param tr_type �g���[�i�[�^�C�v
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 FBI_TOOL_GetTrainerOBJCode( u32 inTrType )
{
  int i;

  for( i = 0; i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == inTrType ){
      return btower_trtype2objcode[i][1];
    }
  }
  return BOY1;
}

#if 0
#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief  �g���[�i�[�^�C�v���Ή����Ă��邩
 * @param tr_type �g���[�i�[�^�C�v
 * @retval BOOL TRUE=�Ή�
 */
//--------------------------------------------------------------
static BOOL check_TrainerType(u8 tr_type)
{
  int i;

  for( i = 0;i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == tr_type ){
      return( TRUE );
    }
  }
  return( FALSE );
}

#endif  //PM_DEBUG
#endif
