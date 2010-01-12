//=============================================================================================
/**
 * @file  btl_debug_print.c
 * @brief �|�P����WB �o�g���V�X�e�� �f�o�b�OPrint�@�\
 * @author  taya
 *
 * @date  2010.01.07  �쐬
 */
//=============================================================================================

#include "btl_util.h"
#include "btl_debug_print.h"


/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BtlPrintType GPrintType;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void print_type( void );
static void print_file_info( const char* filename, int line );



//=============================================================================================
/**
 * �v�����g���̃w�b�_�^�C�v�ʒm���󂯎��
 *
 * @param   type
 */
//=============================================================================================
void BTL_DEBUGPRINT_SetType( BtlPrintType type )
{
  GPrintType = type;
}

void BTL_DEBUGPRINT_PrintDump( const char* caption, const void* data, u32 size )
{
  OS_TPrintf( "[DUMP] %s (%dbytes)\n", caption, size );

  if( size > 8 ){ size = 8; }

  {
    const u8* p = data;
    u32 i;
    for(i=0; i<size; i++)
    {
      OS_TPrintf("%02x ", p[i]);
    }
    OS_TPrintf("\n");
  }
}

const char* BTL_DEBUGPRINT_GetFormatStr( BtlDebugStrID strID )
{
  switch( strID ){
  case DBGSTR_SETUP_DONE:               return "�Z�b�g�A�b�v����\n";
  case DBGSTR_DEBUGFLAG_BIT:            return "�f�o�b�O�t���Obit=%04x\n";
  case DBGSTR_ADAPTER_SEND_START:       return " Adapter-%d, �R�}���h %d �𑗐M�J�n���܂�\n";
  case DBGSTR_ADAPTER_RECV_DONE:        return " [Adapter] Received from All Clients\n";
  case DBGSTR_ADAPTER_RECVED_A_CLIENT:  return " [Adapter] Received from AdapterID=%d\n";
  case DBGSTR_CLIENT_RETURN_CMD_START:  return "ID[%d], �ԐM�J�n��\n";
  case DBGSTR_CLIENT_RETURN_CMD_DONE:   return "ID[%d], �ԐM���܂���\n";
  case DBGSTR_CLIENT_SelectActionSkip:  return "Act�I��(%d�̖ځj�X�L�b�v\n";
  case DBGSTR_CLIENT_SelActPokeDead:    return "����łăX�L�b�v�Ȃ̂�CheckedCnt���̂܂�\n";
  case DBGSTR_CLIENT_SelectActionStart: return "Act�I��(%d�̖�=ID:%d�j�J�n���܂�  checked %d poke\n";
  case DBGSTR_CLIENT_SelectActionDone:  return "�J�o�[�ʒu��(%d)�I���A�A�N�V�������M��\n";
  case DBGSTR_CLIENT_ReturnSeqDone:     return "�ԐM�V�[�P���X�I��\n";
  case DBGSTR_CLIENT_WazaLockInfo:      return "���U���b�N�FClient[%d] �O��g�������U�� %d, idx=%d, targetPos=%d\n";
  case DBGSTR_MAIN_SwapPartyMember:     return "�p�[�e�B�����o�[����ւ� %d <-> %d\n";
  case DBGSTR_NET_DecideClientID:       return "NetID:%d -> clientID=%d\n";
  case DBGSTR_NET_SendSucceed:          return "  ���M����\n";
  case DBGSTR_NET_RecvedServerParam:    return "netID=%d, �T�[�o�p�����[�^��M���܂����B\n";
  case DBGSTR_NET_RecvedPartyData:      return "netID=%d, clientID=%d �̃p�[�e�B�f�[�^��M����, pData=%p, buf=%p\n";
  case DBGSTR_NET_PartyDataComplete:    return "�p�[�e�B�f�[�^���ݎ�M����  member=%d\n";
  case DBGSTR_NET_SendCmdDone:          return "�}�V��(%d) �� %d bytes ���M����\n";
  case DBGSTR_NET_RecvedAllClientsData: return "�S�N���C�A���g����̃f�[�^�Ԃ��Ă���\n";
  case DBGSTR_NET_ReturnToServerTrying: return "Try Return to Server %d byte ...";
  case DBGSTR_NET_RecvedClientData:     return "[BTLNET] recv from netID[%d], size=%d\n";
  case DBGSTR_CALCDMG_BaseDamage:       return "��b�_���[�W�l (%d)\n";
  case DBGSTR_CALCDMG_RangeHosei:       return "�Ώې��ɂ��_���[�W�␳:%d\n";
  case DBGSTR_CALCDMG_WeatherHosei:     return "�V��ɂ��␳������, �␳��=%08x, dmg=%d->%d\n";
  case DBGSTR_CALCDMG_RAW_DAMAGE:       return "�З�:%d, Lv:%d, ��������:%d, �ڂ�����:%d,  ... �f�_��:%d\n";
  case DBGSTR_CALCDMG_Critical:         return "�N���e�B�J��������f�_��->%d\n";
  case DBGSTR_CALCDMG_RandomHosei:      return "�����_���␳:%d%%  -> �f�_��=%d\n";
  case DBGSTR_CALCDMG_TypeMatchHosei:   return "�^�C�v��v�␳:%d%%  -> �f�_��=%d\n";
  case DBGSTR_CALCDMG_TypeAffInfo:      return "�^�C�v����:%02d -> �_���[�W�l�F%d\n";
  case DBGSTR_CALCDMG_DamageResult:     return "�_���󂯃|�P����=%d, ratio=%08x, Damage=%d -> %d\n";
  case DBGSTR_CALCDMG_DamageMarume:     return "�_���󂯃|�P�����̂g�o�l�ɂ܂��->%d\n";
  case DBGSTR_STR_StdStrID:             return " STD:strID=%d\n";
  case DBGSTR_STR_SetStrID:             return " SET:strID=%d\n";
  case DBGSTR_STR_StdStrInfo:           return "STD STR ID=%d, argCnt=%d\n";
  case DBGSTR_STR_Arg:                  return "  arg(%d)=%d\n";
  case DBGSTR_STR_SetPoke1:             return "ms set_poke1 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_STR_SetPoke2:             return "ms set_poke2 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_PRINT_BufIdx:             return "bufIdx=%d ....\n";
  case DBGSTR_STR_TagSetNumber:         return "[TAG] SetNumber : keta=%d, value=%d\n";
  case DBGSTR_STR_SetPokeNickname:      return "setPokeNickName ID=%d ....\n";
  case DBGSTR_STR_SetTagWazaName:       return "[TAG] Set WazaName ... waza=%d\n";
  case DBGSTR_STR_SetItemName:          return "set Item Name argIdx=%d, ID=%d ....\n";
  case DBGSTR_SVFL_ActOrderStart:       return "�|�P[%d =monsno:%d]�̃A�N�V�������s...\n";
  case DBGSTR_SVFL_ActOrder_Fight:      return "�y���������z�������B���U[%d]���A�ʒu[%d]�̑���ɁB\n";
  case DBGSTR_SVFL_ActOrder_Item:       return "�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n";
  case DBGSTR_SVFL_ActOrder_Change:     return "�y�|�P�����z�������B�ʒu%d <- �|�P%d \n";
  case DBGSTR_SVFL_ActOrder_Escape:     return "�y�ɂ���z�������B\n";
  case DBGSTR_SVFL_ActOrder_Dead:       return "�y�Ђ񂵁z�Ȃ̂ŉ�������\n";

  case DBGSTR_SVFL_CorrectTarget_Info:  return "�U���|�P[%d]�i�ʒu=%d�j -> �_�����|�P[%d]�i�ʒu=%d�j����ł���̂ŕ␳����\n";
  case DBGSTR_SVFL_CorrectHitFarOn:     return "���U[%d] ���uON �̕␳�Ώی���\n";
  case DBGSTR_SVFL_CorrectHitFarOff:    return "���U[%d] ���uOFF �̕␳�Ώی���\n";
  case DBGSTR_SVFL_CorrectTargetNum:    return "�␳�Ώې�  %d / %d \n";
  case DBGSTR_SVFL_CorrectTargetDiff:   return "�Ώی��̋��� Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectTargetHP:     return "�Ώی��̎cHP Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectResult:       return "�␳��̑Ώۃ|�P������ �ʒu=%d, ID=%d\n";
  case DBGSTR_SVFL_AtkPower:            return "�U����=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_DefGuard:            return "�h���=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_WazaPower:           return "���U�З͂�%d  (ratio=%08x)\n";
  case DBGSTR_SVFL_UncategoryWazaInfo:  return "�����ރ��U �U��PokeID=%d, �Ώۃ|�P��=%d/%d\n";
  case DBGSTR_SVFL_QueWritePtr:         return "[* SVF *]         Que WritePtr=%d\n";
  case DBGSTR_SVFL_ClientPokeStillAlive:  return "�N���C�A���g_%d (SIDE:%d) �̃|�P�͂܂����̂������Ă���\n";
  case DBGSTR_SVFL_ClientPokeDeadAll:   return "�N���C�A���g_%d (SIDE:%d) �̃|�P�͑S�ł���\n";

  case DBGSTR_SERVER_FlowResult:        return "�T�[�o�[��������=%d\n";
  case DBGSTR_SERVER_SendShooterChargeCmd:    return "�V���[�^�[�`���[�W�R�}���h���s\n";
  case DBGSTR_SERVER_ShooterChargeCmdDoneAll: return "�S�N���C�A���g�ŃV���[�^�[�`���[�W�R�}���h�����I��\n";
  case DBGSTR_SERVER_SendActionSelectCmd:     return "�A�N�V�����I���R�}���h���s\n";
  case DBGSTR_SERVER_ActionSelectDoneAll:     return "�A�N�V�����󂯕t������\n";

  case DBGSTR_SC_PutCmd:                return "[QUE]PutCmd=%d, Format=%02x, argCnt=%d, args=";
  case DBGSTR_SC_ReservedPos:           return "[QUE]reserved pos=%d, wp=%d\n";
  case DBGSTR_SC_WriteReservedPos:      return "[QUE]Write Reserved Pos ... pos=%d, cmd=%d";
  case DBGSTR_SC_PutMsgParam:           return "[QUE] PUT MSG SC=%d, StrID=%d";
  case DBGSTR_SC_PutMsg_SE:             return "  SE_ID=%d\n";
  case DBGSTR_SC_ArgsEqual:             return " args = ";
  case DBGSTR_EV_AddFactor:             return "[ADD] Factor=%p Depend=%d Type=%d, Pri=%06x [ADD]\n";
  case DBGSTR_EV_DelFactor:             return "[DEL] DEL Factor=%p Depend=%d, Type=%d [DEL]\n";
  case DBGSTR_EV_LinkInfoHeader:        return "***** [[EV Chain]] *****\n";
  case DBGSTR_EV_LinkInfoFooder:        return "************************\n\n";
  case DBGSTR_EV_LinkPtr:               return "%p->";
  case DBGSTR_EV_LinkEmpty:             return " empty...\n";
  case DBGSTR_EVAR_Push:                return "PUSH [%5d] SP=%d\n";
  case DBGSTR_EVAR_Pop:                 return "Pop! [%5d] SP=%d\n";



  case DBGSTR_val_comma:  return "%d,";
  case DBGSTR_done: return "done!";
  case DBGSTR_LF: return "\n";

  }
  return NULL;
}


//=============================================================================================
/**
 * �f�o�b�O�p�w�b�_Print
 *
 * @param   fileName
 * @param   line
 */
//=============================================================================================
void BTL_DEBUGPRINT_PrintHeader( const char* fileName, u32 line )
{
  print_type();
  print_file_info( fileName, line );
}
static void print_type( void )
{
  switch( GPrintType ){
  case BTL_PRINTTYPE_SERVER:      OS_TPrintf("SV/"); break;
  case BTL_PRINTTYPE_CLIENT:      OS_TPrintf("CL/"); break;
  case BTL_PRINTTYPE_STANDALONE:  OS_TPrintf("SA/"); break;
  case BTL_PRINTTYPE_UNKNOWN:
  default:
    OS_TPrintf("UN/");
    break;
  }
}
static void print_file_info( const char* filename, int line )
{
  static const struct {
    char* longName;
    char* shortName;
  }names[] = {
    { "btl_main.c",       "MAI" },
    { "btl_server.c",     "SVR" },
    { "btl_client.c",     "CLI" },
    { "btl_adapter.c",    "ADP" },
    { "btl_string.c",     "STR" },
    { "btl_net.c",        "NET" },
    { "btl_calc.c",       "CAL" },
    { "btl_sick.c",       "SIC" },
    { "btl_event.c",      "EVE" },
    { "btl_pokeparam.c",  "BPP" },
    { "btl_server_cmd.c", "CMD" },
    { "btl_server_flow.c","FLW" },
    { "btl_field.c",      "FLD" },
    { "btl_rec.c",        "REC" },
    { "btlv_core.c",      "VIW" },
    { "btlv_scu.c",       "scU" },
    { "btlv_scd.c",       "scD" },
    { NULL,               "OTR" },
  };
  u32 i;

  for(i=0; names[i].longName!=NULL; ++i)
  {
    if( !GFL_STD_StrCmp(names[i].longName, filename) )
    {
      break;
    }
  }
  OS_TPrintf( "[%s-%4d]", names[i].shortName, line);
}
