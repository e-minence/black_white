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
  case DBGSTR_SelectAction_Done:        return "�A�N�V�����I���I��\n";
  case DBGSTR_CLIENT_SelectActionSkip:  return "ACT�I��(%d�̖ځj�X�L�b�v\n";
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
