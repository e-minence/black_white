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
#include "btl_server_cmd.h"
#include "btl_debug_print.h"


/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BtlPrintType GPrintType;

#ifdef PM_DEBUG
/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void print_type( void );
static void print_file_info( const char* filename, int line );
static void printCtrl_Reset( void );
static BOOL  printCtrl_isEnable( u32 idx );
static BOOL printCtrl_isEnableFile( const char* fileName );
static void printCtrl_putCur( void );



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

  printCtrl_Reset();
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
  case DBGSTR_SETUP_DONE:                    return "�Z�b�g�A�b�v����\n";
  case DBGSTR_DEBUGFLAG_BIT:                 return "�f�o�b�O�t���Obit=%04x\n";
  case DBGSTR_ADAPTER_SEND_START:            return " Adapter-%d, �R�}���h %d �𑗐M�J�n���܂�\n";
  case DBGSTR_ADAPTER_RECV_DONE:             return " [Adapter] Received from All Clients\n";
  case DBGSTR_ADAPTER_RECVED_A_CLIENT:       return " [Adapter] Received from AdapterID=%d\n";
  case DBGSTR_ADAPTER_Create:                return "�A�_�v�^(%d�j�쐬�F�ʐM�Ή��t���O=%d\n";

  case DBGSTR_MAIN_PokeConGetByPos:          return "���݂��Ȃ��ʒu(pos=%d, clientID=%d, idx=%d)�̃|�P�����f�[�^���Q�Ƃ���\n";
  case DBGSTR_MAIN_PartyDataNotifyComplete:  return "�p�[�e�B�f�[�^���ݎ�M�ł��܂����B\n";
  case DBGSTR_MAIN_AIPartyDataSendComplete:  return "AI�p�[�e�B�f�[�^��M����\n";
  case DBGSTR_MAIN_SwapPartyMember:          return "�p�[�e�B�����o�[����ւ� %d <-> %d\n";
  case DBGSTR_MAIN_RecvedMultiAITrainer:     return "ClientID=%d, trID=%d set\n";
  case DBGSTR_MAIN_SendAIPartyStart:         return "AI�N���C�A���g��=%d, �p�[�e�B�f�[�^����M�J�n\n";
  case DBGSTR_MAIN_SendAIPartyParam:         return "AI Client=%d �p�[�e�B���M�����i�|�P��=%d\n";
  case DBGSTR_MAIN_GetFrontPokeData:         return "�퓬�ʒu[%d] �̃|�P����... ";
  case DBGSTR_MAIN_GetFrontPokeDataResult:   return "    �N���C�A���g[%d]�� %d �Ԗڂ̃|�P��Ԃ�\n";
  case DBGSTR_MAIN_CommError:                return "�ʐM�G���[���� line=%d\n";
  case DBGSTR_MAIN_CheckHPByLvup:            return "PPLVUP Main line:%d, HP=%d\n";
  case DBGSTR_MAIN_PerappVoiceAdded:         return "ClientID=%d �̃y���b�v�f�[�^�\�z\n";
  case DBGSTR_MAIN_PerappVoiceComplete:      return "�y���b�v�{�C�X���ݎ�M����\n";
  case DBGSTR_MAIN_Illusion1st:              return "%d�Ԗڂɂ���C�����[�W���������|�P����[%d]�̎Q�ƃ|�P��";
  case DBGSTR_MAIN_Illusion2nd:              return " %d�Ԗڂ̃|�P[%d]�ɕύX\n";
  case DBGSTR_MAIN_MultiAITrainer_SeqStart:  return "AI�}���`�p�g���[�i�[�f�[�^����M(idx:%d) �J�n\n";
  case DBGSTR_MAIN_MultiAITrainer_SendDone:  return "AI�}���`�p�g���[�i�[�f�[�^ (ClientID=%d) ���M����\n";
  case DBGSTR_MAIN_CheckResultStart:         return "*** ���s�`�F�b�N ***\n";
  case DBGSTR_MAIN_Result_CommError:         return "  �ʐM�G���[\n";
  case DBGSTR_MAIN_Result_Capture:           return "  �ߊl�I��\n";
  case DBGSTR_MAIN_Result_Escape:            return "  �����ďI��\n";
  case DBGSTR_MAIN_Result_ServerCalc:        return "  �T�[�o�v�Z���ʃR�[�h=%d\n";

  case DBGSTR_CLIENT_RETURN_CMD_START:       return "ID[%d], �ԐM�J�n��\n";
  case DBGSTR_CLIENT_RETURN_CMD_DONE:        return "ID[%d], %d byte �ԐM���܂���\n";
  case DBGSTR_CLIENT_SelectActionSkip:       return "Act�I��(%d�̖ځj�X�L�b�v\n";
  case DBGSTR_CLIENT_SelectActionRoot:       return "Act�I���i%d�̖ځj�J�n .. pokeID=%d, actAdrs=%p\n";
  case DBGSTR_CLIENT_SelActPokeDead:         return "����łăX�L�b�v�Ȃ̂�CheckedCnt���̂܂�\n";
  case DBGSTR_CLIENT_SelectActionStart:      return "Act�I��(%d�̖�=ID:%d�j�J�n���܂�  checked %d poke\n";
  case DBGSTR_CLIENT_SelectActionDone:       return "�J�o�[�ʒu��(%d)�I���A�A�N�V�������M��\n";
  case DBGSTR_CLIENT_SelectActionBacktoRoot: return "�J�o�[�ʒu���B�A���̃|�P(Idx=%d�j�A�N�V�����I�����[�g��\n";
  case DBGSTR_CLIENT_SelectAction_Pokemon:   return "�u�|�P�����v��I��->�I����ʂ�\n";
  case DBGSTR_CLIENT_SelectAction_Fight:     return "�u���������v��I��->�I����ʂ�\n";
  case DBGSTR_CLIENT_SelectChangePoke:       return " �|�P�����I�� ... idx=%d\n";
  case DBGSTR_CLIENT_SelectChangePokeCancel: return " �|�P�����I�΂Ȃ�����\n";
  case DBGSTR_CLIENT_ReturnSeqDone:       return "�ԐM�V�[�P���X�I��\n";
  case DBGSTR_CLIENT_WazaLockInfo:        return "���U���b�N�FClient[%d] �O��g�������U�� %d, targetPos=%d\n";
  case DBGSTR_CLIENT_NoMorePuttablePoke:  return "myID=%d �����킦��|�P�������Ȃ�\n";
  case DBGSTR_CLIENT_NotDeadMember:       return "myID=%d �N������łȂ�����I�ԕK�v�Ȃ�\n";
  case DBGSTR_CLIENT_ChangePokeCmdInfo:   return "myID=%d %d�̑I������K�v����i�擪�ʒu=%d�j mode=%d\n";
  case DBGSTR_CLIENT_NumChangePokeBegin:  return " �SClient, �I�����ׂ��|�P������=%d\n�@�ʒu=";
  case DBGSTR_CLIENT_NumChangePokeResult: return " ����[%d]���I�����ׂ��|�P������=%d\n";
  case DBGSTR_CLIENT_PokeSelCnt:          return "����ւ�%d�̑I��\n";
  case DBGSTR_CLIENT_PokeChangeIdx:       return "�|�P��������ւ� %d�̖� <-> %d�̖�\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk:     return "���������֎~�`�F�b�N�u�����ӂ݁vby pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable:  return "  �u�����ӂ݁v�L���I\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk:    return "���������֎~�`�F�b�N�u���肶�����vby pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable: return "  �u���肶�����v�L���I\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk:      return "���������֎~�`�F�b�N�u����傭�vby pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable:   return "  �u����傭�v�L���I\n";
  case DBGSTR_CLIENT_UpdateWazaProcResult:          return "���U�v���Z�X���X�V : PokeID=%d, OrgWaza=%d, ActWaza=%d, ActWazaType=%d, ActTargetPos=%d, ActEnable=%d\n";
  case DBGSTR_CLIENT_CmdLimitTimeEnable:   return "�R�}���h�I�𐧌����Ԃ��L�� -> %d sec\n";
  case DBGSTR_CLIENT_CmdLimitTimeOver:     return "�R�}���h�I���^�C���A�b�v ... �����I���t���OON\n";
  case DBGSTR_CLIENT_ForcePokeChange:      return "��������ւ� %d/%d PosIdx=%d, MemberIdx=%d\n";
  case DBGSTR_CLIENT_UpdateEnemyBaseHP:    return "����|�P�HP�l���X�V ->%d  (Poke=%d)\n";
  case DBGSTR_CLIENT_ForceQuitByTimeLimit: return "���Ԑ����ɂ�鋭���I��(ClientID=%d)\n";
  case DBGSTR_CLIENT_StartCmd:             return "Client(%d), AdaptCmd(%d) ���s�J�n\n";
  case DBGSTR_CLIENT_RecvedQuitCmd:        return "Client(%d), �o�g���I���R�}���h��M\n";
  case DBGSTR_CLIENT_ReplyToQuitCmd:       return "Client(%d), �o�g���I���R�}���h�ɑ΂��ԐM����\n";
  case DBGSTR_CLIENT_StartRotAct:          return "���[�e�[�V��������J�n clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_EndRotAct:            return "���[�e�[�V��������I�� clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_ReadRecAct:           return "�^��f�[�^�ǂݍ��� clientID=%d, Action count=%d\n";
  case DBGSTR_CLIENT_ReadRecTimerOver:     return "�^��Đ��^�C���I�[�o�[���o myClientID=%d\n";
  case DBGSTR_CLIENT_ReadRecError:         return "�^��Đ��G���[���o myClientID=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Fight:     return "   action=Fight, waza=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Change:    return "   action=Change, nextPokeIdx=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Move:      return "   action=Move!\n";
  case DBGSTR_CLIENT_UnknownServerCmd:     return "�p�ӂ���Ă��Ȃ��R�}���hNo[%d]�I\n";
  case DBGSTR_CLIENT_RecPlayerBlackOut:    return "�^��Đ��R���g���[���F�u���b�N�A�E�g\n";
  case DBGSTR_CLIENT_RecPlay_ChapterSkipped: return "client(%d), �w��`���v�^[%d]�ɓ��B����..\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokStart:    return "�󂯂Ƃ����������ɂ������`�F�b�N ... wazaType=%d\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokFind:     return " %d �Ԗڂ�type=%d���󂯂���|�P������\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokDecide:   return "�󂯂邽�߂̌�֊m��\n";
  case DBGSTR_CLIENT_CHGAI_PreSortList:    return " ����ւ��Ώۃ\�[�g�O ";
  case DBGSTR_CLIENT_CHGAI_AfterSortList:  return " ����ւ��Ώۃ\�[�g�� ";
  case DBGSTR_CLIENT_CHGAI_SortListVal:    return "Idx(%d)=Pow(%d), ";
  case DBGSTR_CLIENT_CHGAI_HOROBI:         return "����ւ������F�ق�т̂���\n";
  case DBGSTR_CLIENT_CHGAI_FusigiNaMamori: return "����ւ������F�ӂ����Ȃ܂���\n";
  case DBGSTR_CLIENT_CHGAI_NoEffWaza:      return "����ւ������F��������\n";
  case DBGSTR_CLIENT_CHGAI_Kodawari:       return "����ւ������F�������n\n";
  case DBGSTR_CLIENT_CHGAI_UkeTok:         return "����ւ������F�󂯂Ƃ�����\n";
  case DBGSTR_CLIENT_CHGAI_SizenKaifuku:   return "����ւ������F�����񂩂��ӂ�\n";
  case DBGSTR_CLIENT_CHGAI_WazaEff:        return "����ւ������F���U�����ɂ��\n";
  case DBGSTR_CLIENT_ROT_R:                return "���[�e�E��]  �Ώۃ|�P=%d\n";
  case DBGSTR_CLIENT_ROT_L:                return "���[�e����]  �Ώۃ|�P=%d\n";
  case DBGSTR_CLIENT_ROT_Determine:        return "���[�e�Ώۃ|�P�A���U�L�� dir=%d, Idx=%d, pokeID=%d\n";
  case DBGSTR_CLIENT_AI_PutPokeStart:      return "myID=%d �퓬�|�P������� %d�̑I��\n";
  case DBGSTR_CLIENT_AI_PutPokeDecide:     return "  -> %d�Ԗڂ�V���ɏo��\n";
  case DBGSTR_CLIENT_CoverRotateDecide:    return "�󂫂𖄂߂郍�[�e�[�V�����m�� ClientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_HPCheckByLvup:        return "PPLVUP Client line:%d, pokeID=%d, HP=%d\n";
  case DBGSTR_CLIENT_OP_HPPlus:            return "�|�P����HP�� : bpp=%p, pokeID=%d, AddHP=%d, ResultHP=%d\n";
  case DBGSTR_CLIENT_OP_MemberIn:          return "�����o�[���� : pos %d<->%d  inPokeID=%d, inPokeHP=%d\n";
  case DBGSTR_CLIENT_StartWazaTargetSel:   return "�ΏۑI���J�n��\n";
  case DBGSTR_CLIENT_MoveAction:           return "���[�u�ʒu pos(%d<->%d) = vpos(%d<->%d)\n";

  case DBGSTR_PSET_Copy:          return "�|�P�Z�b�g�R�s�[  adrs=%p, count=%d\n";
  case DBGSTR_PSET_Add:           return "�|�P�Z�b�g�ǉ�    adrs=%p, pokeID=%d, count=%d/%d\n";
  case DBGSTR_PSET_Remove:        return "�|�P�Z�b�g�폜    adrs=%p, pokeID=%d, count=%d/%d\n";
  case DBGSTR_FIDLD_FuinCheck:    return "�ӂ�����|�P�`�F�b�N %d �̖� : �ӂ�����|�P=%d, �`�F�b�N���U=%d\n";
  case DBGSTR_FIELD_FuinHit:      return "�ӂ�����|�P(ID=%d)���A���U:%d�������Ă����I\n";

  case DBGSTR_BPP_NemuriWakeCheck:   return "�|�P[%d]�̂˂ނ�^�[���ő�l=%d, �o�߃^�[��=%d\n";
  case DBGSTR_BPP_WazaLinkOff:       return "���U���������A�����N�؂�܂��i�V���U�i���o�[=%d�j\n";
  case DBGSTR_BPP_WazaRecord:        return "���U�L�^(PokeID=%d)�F�I�񂾃��U=%d, �h���������U=%d, �_�����ʒu=%d, ���U�^�C�v=%d\n";

  case DBGSTR_VCORE_PokeListStart:      return "�������X�g��� ���[�h=%d ���ł�%d�̑I��, ����I��=%d\n";
  case DBGSTR_VCORE_SelPokeEnd:         return "�|�P�I���������\n";
  case DBGSTR_VCORE_SelPokeEnd_Sel:     return "  [%d]�Ԗ� �I�����ꂽ -> Idx=%d, (%d�� �i�[)\n";
  case DBGSTR_VCORE_SelPokeEnd_Unsel:   return "  [%d]�Ԗ� �I������Ă��Ȃ�\n";
  case DBGSTR_VCORE_PluralDamageInfo:   return "�����̃_���[�W���� (%d��) wazaID=%d\n";
  case DBGSTR_VCORE_PluralDamagePoke:   return "  �Ώۃ|�PID=%d\n";

  case DBGSTR_NET_Startup:              return "�����̃l�b�gID=%d, �ڑ������o�[��=%d\n";
  case DBGSTR_NET_DecideClientID:       return "NetID:%d -> clientID=%d\n";
  case DBGSTR_NET_SendSucceed:          return "  ���M����\n";
  case DBGSTR_NET_RecvedServerParam:    return "netID=%d, �T�[�o�p�����[�^��M���܂����B\n";
  case DBGSTR_NET_RecvedPartyData:      return "netID=%d, clientID=%d �̃p�[�e�B�f�[�^��M����, pData=%p, buf=%p\n";
  case DBGSTR_NET_PartyDataComplete:    return "�p�[�e�B�f�[�^���ݎ�M����  member=%d\n";
  case DBGSTR_NET_PerappVoiceComplete:  return "�y���b�v�{�C�X���ݎ�M����  member=%d\n";
  case DBGSTR_NET_SendCmdDone:          return "�}�V��(%d) �� %d bytes ���M����\n";
  case DBGSTR_NET_RecvedAllClientsData: return "�S�N���C�A���g����̃f�[�^�Ԃ��Ă���\n";
  case DBGSTR_NET_ReturnToServerTrying: return "Try Return to Server %d byte ...";
  case DBGSTR_NET_RecvedClientData:     return "[BTLNET] recv from netID[%d], size=%d\n";
  case DBGSTR_NET_RecvedServerVersion:  return "�T�[�o�o�[�W������M ... netID=%d, version=%d, clientID=%d\n";
  case DBGSTR_NET_ServerDetermine:      return "�T�[�o�� netID=%d �̃}�V���Ɍ���\n";
  case DBGSTR_NET_SendAITrainerData:    return "AI�g���[�i�[�f�[�^���M����, trID=%d\n";
  case DBGSTR_NET_RecvAITrainerData:    return "AI�g���[�i�[�f�[�^��M����, trID=%d\n";
  case DBGSTR_NET_AIPartyInfo:          return "Client(%d)=AI�p�[�e�B�Ɣ��f���� (pokeCnt=%d)\n  monsno=";
  case DBGSTR_NET_CreateAIPartyRecvBuffer: return "AI�p�[�e�B�f�[�^��M�o�b�t�@�𐶐�\n";
  case DBGSTR_NET_RecvedAIPartyData:       return "AI�p�[�e�B�f�[�^��M clientID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufDisable:  return "�y���b�v�{�C�X��M�o�b�t�@�\�z�i�����f�[�^�jnetID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufEnable:   return "�y���b�v�{�C�X��M�o�b�t�@�\�z�i�L���f�[�^�jnetID=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedEnable:  return "�y���b�v�{�C�X��M�����i�L���f�[�^�jnetID=%d, size=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedDisable: return "�y���b�v�{�C�X��M�����i�����f�[�^�jnetID=%d, empty=%d\n";
  case DBGSTR_NET_PerappVoiceCheckRaw:      return "�y���b�v�{�C�X��M�`�F�b�N: ClientID:%d = netID:%d\n";

  case DBGSTR_HEM_Push:     return "[STATE PUSH -%5d] << sp=%d, rp=%d\n";
  case DBGSTR_HEM_Pop:      return "[STATE POP  -%5d]    sp=%d, rp=%d >>\n";
  case DBGSTR_HEM_PushWork: return "[work PUSH]: type=%d, pokeID=%d, size=%3d, sp=%d\n";
  case DBGSTR_HEM_PophWork: return "[work  POP]: type=%d, pokeID=%d, size=%3d, sp=%d, myPos=%d,\n";

  case DBGSTR_CALCDMG_WazaParam:        return "���U���FID=%d, Type=%d\n";
  case DBGSTR_CALCDMG_BaseDamage:       return "��b�_���[�W�l (%d)\n";
  case DBGSTR_CALCDMG_RangeHosei:       return "�Ώې��ɂ��_���[�W�␳:%d  (ratio=%08x)\n";
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
  case DBGSTR_SVFL_ActOrderStart:       return ">>>> ActProc [�J�n] �|�P[ID:%d = monsno:%d] (ptr=%p)\n";
  case DBGSTR_SVFL_ActOrderEnd:         return "<<<< ActProc [�I��] �|�P[ID:%d = monsno:%d] (ptr=%p)\n";
  case DBGSTR_SVFL_ActOrder_Fight:      return "�y���������z�|�P[%d]�̃��U[%d]���A�ʒu[%d]�̑����\n";
  case DBGSTR_SVFL_ActOrder_Item:       return "�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n";
  case DBGSTR_SVFL_ActOrder_Change:     return "�y�|�P�����z�������B�ʒu%d <- �|�P%d \n";
  case DBGSTR_SVFL_ActOrder_Escape:     return "�y�ɂ���z�������B\n";
  case DBGSTR_SVFL_ActOrder_Dead:       return "�y�Ђ񂵁z�Ȃ̂ŉ�������\n";
  case DBGSTR_SVFL_ActOrder_Rotation:   return "�y���[�e�[�V�����z������  ClientID=%d, ��]����=%d\n";
  case DBGSTR_SVFL_CorrectTarget_Info:  return "�U���|�P[%d]�i�ʒu=%d�j -> �_�����|�P[%d]�i�ʒu=%d�j����ł���̂ŕ␳����\n";
  case DBGSTR_SVFL_CorrectHitFarOn:     return "���U[%d] ���uON �̕␳�Ώی���\n";
  case DBGSTR_SVFL_CorrectHitFarOff:    return "���U[%d] ���uOFF �̕␳�Ώی���\n";
  case DBGSTR_SVFL_CorrectTargetNum:    return "�␳�Ώې�  %d / %d \n";
  case DBGSTR_SVFL_CorrectTargetDiff:   return "�Ώی��̋��� Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectTargetHP:     return "�Ώی��̎cHP Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectResult:       return "�␳��̑Ώۃ|�P������ �ʒu=%d, ID=%d\n";
  case DBGSTR_SVFL_AtkPower:            return "�U����=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_DefGuard:            return "�h���=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_WazaPower:           return "���UID=%d : �З�=%d  (ratio=%08x)\n";
  case DBGSTR_SVFL_UncategoryWazaInfo:  return "�����ރ��U �U��PokeID=%d, �Ώۃ|�P��=%d/%d\n";
  case DBGSTR_SVFL_QueWritePtr:         return "[* SVF *]         Que WritePtr=%d\n";
  case DBGSTR_SVFL_ClientPokeStillAlive:return "�N���C�A���g_%d (SIDE:%d) �̃|�P�͂܂�%d�̐����Ă���\n";
  case DBGSTR_SVFL_ClientPokeDeadAll:   return "�N���C�A���g_%d (SIDE:%d) �̃|�P�͑S�ł���\n";
  case DBGSTR_SVFL_PosEffAdd:           return "�ʒu�G�t�F�N�g�n���h������: pos=%d, effect=%d, pokeID=%d\n";
  case DBGSTR_SVFL_PosEffDupFail:       return "�ʒu(%d)��POSEffect[%d]�n���h����ǉ����s�i�g�p�|�P=%d�j\n";
  case DBGSTR_SVFL_HitCheckInfo1:       return "�U���|�P[%d]  ����Rank=%d  ���U�I����=%d\n";
  case DBGSTR_SVFL_HitCheckInfo2:       return "�h��|�P[%d]  ���Rank=%d\n";
  case DBGSTR_SVFL_HitCheckInfo3:       return "���������N=%d, ������=%d, �������␳=%08x\n";
  case DBGSTR_SVFL_HitCheckInfo4:       return "�ŏI������ = %d\n";
  case DBGSTR_SVFL_StartAfterPokeChange:return "�^�[���r���|�P��������ւ���̃T�[�o�[�R�}���h����\n";
  case DBGSTR_SVFL_PokeChangeRootInfo:  return "�N���C�A���g(%d)�̃|�P����(�ʒu%d) ���A%d�Ԗڂ̃|�P�Ƃ��ꂩ����\n";
  case DBGSTR_SVFL_UseItemCall:         return "�V���[�^�[�u�A�C�e���R�[���v�g�p  �Ώ�Poke=%d\n";
  case DBGSTR_SVFL_ChangePokeReqInfo:   return "�N���C�A���g[%d]   �󂢂Ă���ʒu�̐�=%d, �ʒuID= ";
  case DBGSTR_SVFL_TurnStart_Result:    return "�y�^�[���J�n�R�}���h�z�����A�N�V���� %d / %d\n";
  case DBGSTR_SVFL_ActOrderMainStart:   return "�R�}���h�����R�A �����A�N�V�����J�nIndex=%d\n";
  case DBGSTR_SVFL_ActOrderMainDropOut: return "Result=%d�ɂ�钆�r�T�[�o�ԐM\n";
  case DBGSTR_SVFL_ExpCalc_Base:        return "��{�ƂȂ�o���l=%d\n";
  case DBGSTR_SVFL_ExpCalc_MetInfo:     return "���S�|�P[%d]���Ζʂ�������|�P�̐�=%d, ���̓��A�����Ă鐔=%d\n";
  case DBGSTR_SVFL_ExpCalc_DivideInfo:  return "�����o�[Idx[%d]�̃|�P�Ɍo���l%d�𕪔z\n";
  case DBGSTR_SVFL_ExpCalc_Result:      return "�����o�[Idx[%d]�̃|�P�ɑ΂��A�ŏI�o���l=%d\n";
  case DBGSTR_SVFL_ExpAdjustCalc:       return "����Lv=%d, �GLv=%d, num=%d, denom=%d, ��{�o���l=%d -> �␳��o���l=%d\n";
  case DBGSTR_SVFL_RecDataSendComped:   return "����L�^�f�[�^�̑��M����\n";
  case DBGSTR_SVFL_SendServerCmd:       return "�T�[�o�R�}���h���M���܂� ... result=%d\n";
  case DBGSTR_SVFL_AllClientCmdPlayComplete:  return "�S�N���C�A���g�̃R�}���h�Đ��I��...result=%d\n";
  case DBGSTR_SVFL_GotoQuit:            return "�o�g���I����\n";
  case DBGSTR_SVFL_ReqWazaCallActOrder: return "�����U�Ăяo�� [%d] --> [%d], target=%d\n";
  case DBGSTR_SVFL_WazaExeFail_1:       return "���U�o�����s�i�P�j  �|�PID=%d, ���s�R�[�h=%d\n";
  case DBGSTR_SVFL_WazaExeFail_2:       return "���U�o�����s�i�Q�j  �|�PID=%d, ���s�R�[�h=%d\n";
  case DBGSTR_SVFL_HandEx_AddSick:      return "��Ԉُ� poke=%d, code=%d, AlmostFlag=%d\n";
  case DBGSTR_SVFL_CombiWazaCheck:      return "���U[%d]�͍��̃��U�̂ЂƂł�\n";
  case DBGSTR_SVFL_CombiWazaFound:      return "  ��������ɁA�����ʒu(%d�j�Ƀ��U(%d)��ł����i%d�j����������\n";
  case DBGSTR_SVFL_CombiDecide:         return "  �|�P(%d�j���攭�B�|�P(%d)�ƍ��̃��U�\��I\n";
  case DBGSTR_SVFL_StartAfterPokeIn:    return "�󂫈ʒu�Ƀ|�P����������̃T�[�o�[�R�}���h�������J�n����\n";
  case DBGSTR_SVFL_AfterPokeIn_Alive:   return "�����Ă���|�P�����Ƃ̓���ւ������� Cleint=%d, Pos %d <-> %d\n";
  case DBGSTR_SVFL_AfterPokeIn_Dead:    return "�Ђ񂵃|�P�����̋󂫂𖄂߂����ւ������� Pos %d <-> %d\n";
  case DBGSTR_SVFL_PokeDead:            return "�|�P(%d) ���ɂ܂�\n";
  case DBGSTR_SVFL_AddSickFailCode:     return "��Ԉُ편�s�R�[�h=%d\n";
  case DBGSTR_SVFL_AddSickFixed:        return "��Ԉُ팈��F�R�[�h=%d\n";
  case DBGSTR_SVFL_WazaExeStart:        return "�|�P[%d], waza=%d�̃��U�o�������J�n\n";
  case DBGSTR_SVFL_ActionSortInfo:      return "�A�N�V�����\�[�g:Client(%d), actionCnt=%d\n";
  case DBGSTR_SVFL_ActionSortRotation:  return ".. Action Sort Rot nextPokeIdx = %d\n";
  case DBGSTR_SVFL_ActionSortGen:       return ".. Action Sort pokeIdx=%d, pokeID=%d, action=%d\n";
  case DBGSTR_SVFL_TrainerItemTarget:   return "�g���[�i�[�A�C�e���g�p�FtgtIdx=%d, tgtPokeID=%d\n";
  case DBGSTR_SVFL_DoubleTargetIntr:    return "�_�u���G�P�̃^�[�Q�b�g�c���荞�݃|�P=%d\n";
  case DBGSTR_SVFL_DoubleTargetRegister:return "�_�u���G�P�̃^�[�Q�b�g�c�m��|�P=%d\n";
  case DBGSTR_SVFL_DeadAlready:         return "�|�P(%d)���Ɏ���ł�̂Ŏ��S�R�}���h��ł��܂���\n";
  case DBGSTR_SVFL_DeadDiffLevelCheck:  return "�|�P���S�A����lv=%d, �GLv=%d\n";
  case DBGSTR_SVFL_HandExRelive:        return "HandEx : �|�P�����Ԃ� pokeID=%d, HP=%d\n";
  case DBGSTR_SVFL_PutWazaEffect:       return "���U�G�t�F�N�g�R�}���h����: reservedPos=%d, wazaID=%d, atkPos(%d)->defPos(%d), effIndex=%d\n";
  case DBGSTR_SVFL_WazaDmgCmd:          return "���U�_���[�W�R�}���h wazaID=%d, pokeCnt=%d pokeID= ";
  case DBGSTR_SVFL_ActIntr:             return "���荞�݃A�N�V���� adrs=%p, pokeID=%d\n";
  case DBGSTR_SVFL_RelivePokeStart:     return "ClientID=%d �̃|�P�������Ԃ���..";
  case DBGSTR_SVFL_ReliveEndIn:         return "�󂫂�����̂œ��ꂳ����\n";
  case DBGSTR_SVFL_ReliveEndIgnore:     return "�󂫂��Ȃ��̂Ŗ�������\n";
  case DBGSTR_SVFL_RankEffCore:         return "�|�P[%d]�̔\�̓����N(%d)�� %d �i�K�ω� (���U�g�p�|�P=%d�j\n";
  case DBGSTR_SVFL_IntrTargetPoke:      return "���荞�݃^�[�Q�b�g�|�P=%d\n";
  case DBGSTR_SVFL_CheckItemReaction:   return "�|�P[%d]�̑����A�C�e�������`�F�b�N���܂�\n";
  case DBGSTR_SVFL_MagicCoatStart:      return "�}�W�b�N�R�[�g���˕Ԃ� %d ��\n";
  case DBGSTR_SVFL_MagicCoatReflect:    return "�}�W�b�N�R�[�g���˕Ԃ� poke(%d) -> (%d)_%d\n";
  case DBGSTR_SVFL_YokodoriDetermine:   return "�|�P[%d]�̃��U[%d]���悱�ǂ�܂�\n";
  case DBGSTR_SVFL_YokodoriExe:         return "�悱�ǂ肵���|�P=[%d], �Ώۈʒu=%d\n";
  case DBGSTR_SVFL_YokodoriInfo:        return "�悱�ǂ�Ώ�PokeID=%d, �ʒu=%d\n";
  case DBGSTR_SVFL_HandExRoot:          return "ProcHandEX : ADRS=0x%p, type=%d, fFailSkip=%d, fPrevSucceed=%d\n";
  case DBGSTR_SVFL_HandExContFail:      return  "  skip (prev handEx failed)\n";
  case DBGSTR_SVFL_HandExContDead:      return  "  skip (user pokemon dead)\n";
  case DBGSTR_SVFL_HandExSetResult:     return  "  set result = %d\n";
  case DBGSTR_SVFL_HandExTokChangeAdd:  return  "�Ƃ��������������n���h�� pokeID=%d, factor=%p\n";
  case DBGSTR_SVFL_HandExTokChangeEventCall: return "�Ƃ���������������C�x���g�R�[�� pokeID=%d\n";
  case DBGSTR_SVFL_HandExTokChangeEventCallEnd : return "�Ƃ���������������C�x���g�I��\n";
  case DBGSTR_SVFL_MemberInEventBegin: return "�����o�[����C�x���g pokeID=%d �J�n\n";
  case DBGSTR_SVFL_MemberInEventEnd :  return "�����o�[����C�x���g pokeID=%d �I��\n";
  case DBGSTR_SVFS_RegTargetDouble:     return "�_�u���^�[�Q�b�g�F�Ώ۔͈�=%d, �U���|�P=%d, �I���ʒu=%d\n";
  case DBGSTR_SVFS_Result_RestCnt:      return "�c��|�P��  side0=%d, side1=%d\n";
  case DBGSTR_SVFS_LastPokeID:          return "�Ō�܂Ő����Ă��|�PID=%d\n";
  case DBGSTR_SVFL_DeadPokeCount:       return "�|�����|�P��= %d vs %d\n";
  case DBGSTR_SVFL_HPRatio:             return "�cHP [side:%d]  ���a=%d, ����=%d\n";

  case DBGSTR_POSPOKE_Out:              return " poke[%d] out from pos[%d]\n";
  case DBGSTR_POSPOKE_In:               return " poke[%d] in to pos[%d]\n";

  case DBGSTR_CALC_EscInfoAdd:       return "�������FClientID=%d, �ǉ��A�g�[�^��%d��\n";
  case DBGSTR_CALC_EscCheckMyTeam:   return "ClientID=%d �������� ... �����̃`�[��\n";
  case DBGSTR_CALC_EscCheckEnemy:    return "ClientID=%d �������� ... ����̃`�[��\n";
  case DBGSTR_CALC_EscResultBoth1:   return "���`�[�������F�쐶���ʏ�g���[�i�[�Ȃ̂Ŏ������������̂Ɠ���\n";
  case DBGSTR_CALC_EscResultBoth2:   return "���`�[�������F�ʐM�E�T�u�E�F�C�Ȃ��������\n";
  case DBGSTR_CALC_EscResultLose:    return "�����`�[�������������畉��\n";
  case DBGSTR_CALC_EscResultWin:     return "����`�[�������������珟��\n";


  case DBGSTR_SERVER_FlowResult:              return "�T�[�o�[��������=%d\n";
  case DBGSTR_SERVER_SendShooterChargeCmd:    return "�V���[�^�[�`���[�W�R�}���h���s\n";
  case DBGSTR_SERVER_ShooterChargeCmdDoneAll: return "�S�N���C�A���g�ŃV���[�^�[�`���[�W�R�}���h�����I��\n";
  case DBGSTR_SERVER_SendActionSelectCmd:     return "�A�N�V�����I���R�}���h���s\n";
  case DBGSTR_SERVER_ActionSelectDoneAll:     return "�A�N�V�����󂯕t������\n";
  case DBGSTR_SV_ChangePokeOnTheTurn:         return "�^�[���r���̃|�P����ւ�����\n";
  case DBGSTR_SV_PokeInReqForEmptyPos:        return "�󂫈ʒu�ւ̐V�|�P�������N�G�X�g�󂯕t��\n";
  case DBGSTR_SV_StartChangePokeInfo:         return "����ւ��|�P�����I����  ��ւ����|�P��=%d\n";
  case DBGSTR_SV_SendQuitACmad:               return "�S�N���C�A���g�Ƀo�g���I���R�}���h(%d)���M�J�n\n";
  case DBGSTR_SV_ReplyQuitACmad:              return "�S�N���C�A���g����o�g���I���R�}���h��M��������\n";
  case DBGSTR_SV_RestoreAction:               return "�������`�F�b�N�p�T�[�o�FClientID=%d, numAction:%d ����\n";
  case DBGSTR_SV_CmdCheckOK:                  return "�������`�F�b�N�p�T�[�o�F�`�F�b�NOK\n";
  case DBGSTR_SV_CmdCheckNG:                  return "�������`�F�b�N�p�T�[�o�F�`�F�b�NNG!! cmdSize= %d , %d\n";
  case DBGSTR_SV_SendActRecordFailed:         return "�A�N�V�����L�^���M�Ɏ��s����!!\n";
  case DBGSTR_SV_SendActRecord:               return "�A�N�V�����L�^�f�[�^�𑗐M���� (%dbytes)\n";
  case DBGSTR_SV_SendBtlInRecord:             return "�o�g���J�n�`���v�^�f�[�^�𑗐M���� (%dbytes)\n";
  case DBGSTR_SV_ExitBattle:                  return "ExitBattle .. competitor = %d\n";
  case DBGSTR_SV_CommError:                   return "�ʐM�G���[�ɂ��I�� line=%d\n";

  case DBGSTR_SC_PutCmd:                return "[QUE]PutCmd=%d, Format=%02x, argCnt=%d, args=";
  case DBGSTR_SC_ReservedPos:           return "[QUE]reserved pos=%d, wp=%d\n";
  case DBGSTR_SC_WriteReservedPos:      return "[QUE]Write Reserved Pos ... pos=%d, cmd=%d";
  case DBGSTR_SC_PutMsgParam:           return "[QUE] PUT  MSG SC=%d, StrID=%d";
  case DBGSTR_SC_ReadMsgParam:          return "[QUE] READ MSG SC=%d, StrID=%d";
  case DBGSTR_SC_PutMsg_SE:             return "  SE_ID=%d\n";
  case DBGSTR_SC_ArgsEqual:             return " args = ";
  case DBGSTR_SC_ReadCmd:               return "Read cmd=%d\n";
  case DBGSTR_SC_ReserveCmd:            return " -> reserved cmd=%d\n";


  case DBGSTR_EVENT_AddFactorInfo:      return "AddNewFactor type=%x, dependID=%d, pri=%d, adrs=%p\n";
  case DBGSTR_EVENT_RmvFactorCalling:   return " �Ăяo�����ɍ폜���ꂽFactor pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_CallFactorStart:    return " [%p] �Ăяo���܂� dependID=%d, type=%d\n";
  case DBGSTR_EVENT_SkipByNewComerFlg:  return " �V����ɂ��X�L�b�v Factor pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_ClearNewComFlag:    return " �V����t���O�N���A pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_SkipByCallingFlg:   return " ���ȌĂяo�����ɂ��X�L�b�v type=%d, pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_SkipByCheckHandler: return " �n���h���ɂ��X�L�b�v type=%d, pokeID=%d\n";

  case DBGSTR_EV_AddFactor:             return "[ADD] Factor=%p Depend=%d Type=%d, Pri=%06x [ADD]\n";
  case DBGSTR_EV_DelFactor:             return "[DEL] Factor=%p Depend=%d, Type=%d, prev=%p, next=%p\n";
  case DBGSTR_EV_LinkInfoHeader:        return "***** [[EV Chain]] *****\n";
  case DBGSTR_EV_LinkInfoFooder:        return "************************\n\n";
  case DBGSTR_EV_LinkPtr:               return "%p->";
  case DBGSTR_EV_LinkEmpty:             return " empty...\n";
  case DBGSTR_EVAR_Push:                return "PUSH [%5d] SP=%d\n";
  case DBGSTR_EVAR_Pop:                 return "Pop! [%5d] SP=%d\n";
  case DBGSTR_DEADREC_Add:              return " ���S�L�^ �^�[��%d���� ... pokeID=%d\n";
  case DBGSTR_POKESET_RemoveDeadPoke:   return "�|�P(%d) ���񂾂̂ŃZ�b�g���珜�O\n";
  case DBGSTR_SICK_ContProc:            return "�|�P[%d], ��Ԉُ�[%d] ���p����...\n";

  case DBGSTR_SIDE_AddFirst:            return "�T�C�h�G�t�F�N�g����o�^ side=%d, eff=%d, pCnt=%p, cnt=%d\n";
  case DBGSTR_SIDE_NoMoreAdd:           return "�T�C�h�G�t�F�N�g����ȏ�o�^�ł��Ȃ� side=%d, eff=%d\n";

  case DBGSTR_SCU_RelivePokeAct:        return "Pos[%d]=Vpos[%d] �|�P(%d)�������Ԃ�\n";
  case DBGSTR_SCD_DecideTargetPos:      return "�^�[�Q�b�g���� ... hitBtn=%d, hitPos=%d, targetPos=%d\n";
  case DBGSTR_SCD_TargetHitPosConvert:  return "  HitPos:%d -> vpos:%d -> btlPos:%d\n";
  case DBGSTR_SCD_TargetHitPosFix:      return "   btlPos:%d  Selectable !\n";

  case DBGSTR_Item_PinchReactItem:      return "PokeID=%d, �ő�HP=%d, ��HP=%d, n=%d ... ";
  case DBGSTR_Item_PinchReactOn:        return "�A�C�e���g���܂�\n";
  case DBGSTR_HANDWAZA_CombiWazaExe:    return "�|�P(%d)���|�P(%d�j�̃��U(%d)�ɑ����č��̃��U����->����=%d\n";
  case DBGSTR_HANDWAZA_AlreadyRegistered: return "�|�P[%d]�̃��U�n���h��[%d]�͂��łɓo�^�ς�\n";
  case DBGSTR_HANDWAZA_YokorodiDecide:  return "�|�P(%d), �悱�ǂ肵�܂�\n";
  case DBGSTR_HANDITEM_OujaCheck:       return "��������̂��邵�`�F�b�N�F�f�t�H���g�Ђ�ݗ�=%d\n";
  case DBGSTR_HANDITEM_OujaEffective:   return "��������̂��邵�L���F�Ђ�ݗ�=%d\n";
  case DBGSTR_HANDTOK_TRACE_Add:        return "�g���[�X�������܂� pokeID=%d\n";
  case DBGSTR_HANDTOK_PressureIn:       return "�v���b�V���[�������܂�  pokeID=%d, factor=%p\n";


  case DBGSTR_REC_ReadActStart:         return "rec seek start RP= %d\n";
  case DBGSTR_REC_ReadActSkip:          return "rec seek RotateData skip %d byte .. RP=%d \n";
  case DBGSTR_REC_SeekClient:           return "rec seek numClient=%d\n";
  case DBGSTR_REC_ReadActParam:         return "rec ReadPtr=%d, act=%d, waza=%d\n";
  case DBGSTR_REC_CheckMatchClient:     return "rec Read-ClientID=%d, seeking-clientID=%d,  numAction=%d\n";
  case DBGSTR_REC_ReadTimeOverCmd:      return "���Ԑ����ɂ��I���R�}���h��ǂݎ����  RP=%d\n";
  case DBGSTR_REC_TimeOverCmdWrite:     return "���Ԑ����I���R�}���h����������\n";
  case DBGSTR_REC_CheckReadComp:        return "�^��ǂݍ��݊����`�F�b�N�FclientID=%d, RP=%d, dataSize=%d\n";;


  case DBGSTR_csv:  return "%d,";
  case DBGSTR_done: return "done!";
  case DBGSTR_LF: return "\n";

  }
  return NULL;
}

const char* BTL_DEBUGPRINT_GetServerCmdName( int cmd )
{
  switch( cmd ){
  case SC_OP_HP_MINUS:  return "SC_OP_HP_MINUS";           ///< HP�}�C�i�X  [ClientID, �}�C�i�X��]
  case SC_OP_HP_PLUS:  return "SC_OP_HP_PLUS";            ///< HP�v���X    [ClientID, �v���X��]
  case SC_OP_HP_ZERO:  return "SC_OP_HP_ZERO";            ///< HP0 �ɂ��� [ pokeID ]
  case SC_OP_PP_MINUS:  return "SC_OP_PP_MINUS";           ///< PP�}�C�i�X  [ClientID, �}�C�i�X��]
  case SC_OP_PP_MINUS_ORG:  return "SC_OP_PP_MINUS_ORG";           ///< PP�}�C�i�X  [ClientID, �}�C�i�X��]
  case SC_OP_WAZA_USED:  return "SC_OP_WAZA_USED";
  case SC_OP_PP_PLUS:  return "SC_OP_PP_PLUS";            ///< PP�v���X    [ClientID, �v���X��]
  case SC_OP_PP_PLUS_ORG:  return "SC_OP_PP_PLUS_ORG";            ///< PP�v���X    [ClientID, �v���X��]
  case SC_OP_RANK_UP:  return "SC_OP_RANK_UP";            ///< �X�e�[�^�X�����N�A�b�v  [ClientID, StatusType, �v���X��]
  case SC_OP_RANK_DOWN:  return "SC_OP_RANK_DOWN";          ///< �X�e�[�^�X�����N�_�E��  [ClientID, StatusType, �}�C�i�X��]
  case SC_OP_RANK_SET7:  return "SC_OP_RANK_SET7";          ///< �X�e�[�^�X�����N�V��Z�b�g[ pokeID, atk, def, sp_atk, sp_def, agi ]
  case SC_OP_RANK_RECOVER:  return "SC_OP_RANK_RECOVER";       ///< �X�e�[�^�X�����N�i�V��j�������Ă�����̂̂݃t���b�g��
  case SC_OP_RANK_RESET:  return "SC_OP_RANK_RESET";         ///< �X�e�[�^�X�����N�i�V��j�S�Ă��t���b�g��
  case SC_OP_ADD_CRITICAL:  return "SC_OP_ADD_CRITICAL";       ///< �N���e�B�J�������N���Z[ pokeID, (int)value ]
  case SC_OP_SICK_SET:  return "SC_OP_SICK_SET";           ///< ��Ԉُ� [PokeID, Sick, contParam]
  case SC_OP_CURE_POKESICK:  return "SC_OP_CURE_POKESICK";      ///< �|�P�����n��Ԉُ���� [PokeID ]
  case SC_OP_CURE_WAZASICK:  return "SC_OP_CURE_WAZASICK";      ///< ���U�n��Ԉُ���� [PokeID, SickID ]
  case SC_OP_MEMBER_IN:  return "SC_OP_MEMBER_IN";          ///< �����o�[����
  case SC_OP_SET_STATUS:  return "SC_OP_SET_STATUS";         ///< �\�͒l�i�U���A�h�䓙�j����������
  case SC_OP_SET_WEIGHT:  return "SC_OP_SET_WEIGHT";         ///< �̏d�ݒ�
  case SC_OP_WAZASICK_TURNCHECK:  return "SC_OP_WAZASICK_TURNCHECK"; ///<
  case SC_OP_CHANGE_POKETYPE:  return "SC_OP_CHANGE_POKETYPE";    ///< �y�v�Z�z�|�P�����̃^�C�v�ύX�i pokeID, type �j
  case SC_OP_CONSUME_ITEM:  return "SC_OP_CONSUME_ITEM";       ///< ���L�A�C�e���폜
  case SC_OP_UPDATE_USE_WAZA:  return "SC_OP_UPDATE_USE_WAZA";    ///< ���U�v���Z�X�I�����
  case SC_OP_SET_CONTFLAG:  return "SC_OP_SET_CONTFLAG";       ///< �i���t���O�Z�b�g
  case SC_OP_RESET_CONTFLAG:  return "SC_OP_RESET_CONTFLAG";     ///< �i���t���O���Z�b�g
  case SC_OP_SET_TURNFLAG:  return "SC_OP_SET_TURNFLAG";       ///< �^�[���t���O�Z�b�g
  case SC_OP_RESET_TURNFLAG:  return "SC_OP_RESET_TURNFLAG";     ///< �^�[���t���O�������Z�b�g
  case SC_OP_CHANGE_TOKUSEI:  return "SC_OP_CHANGE_TOKUSEI";     ///< �Ƃ�������������
  case SC_OP_SET_ITEM:  return "SC_OP_SET_ITEM";           ///< �A�C�e����������
  case SC_OP_UPDATE_WAZANUMBER:  return "SC_OP_UPDATE_WAZANUMBER";  ///< ���U��������
  case SC_OP_OUTCLEAR:  return "SC_OP_OUTCLEAR";           ///< �ޏꎞ�N���A
  case SC_OP_ADD_FLDEFF:  return "SC_OP_ADD_FLDEFF";         ///< �t�B�[���h�G�t�F�N�g�ǉ�
  case SC_OP_ADD_FLDEFF_DEPEND: return "SC_OP_ADD_FLDEFF_DEPEND";
  case SC_OP_REMOVE_FLDEFF:  return "SC_OP_REMOVE_FLDEFF";      ///< �t�B�[���h�G�t�F�N�g�폜
  case SC_OP_SET_POKE_COUNTER:  return "SC_OP_SET_POKE_COUNTER";   ///< �|�P�����J�E���^�l�Z�b�g
  case SC_OP_BATONTOUCH:  return "SC_OP_BATONTOUCH";         ///< �o�g���^�b�`
  case SC_OP_MIGAWARI_CREATE:  return "SC_OP_MIGAWARI_CREATE";    ///< �݂����쐬
  case SC_OP_MIGAWARI_DELETE:  return "SC_OP_MIGAWARI_DELETE";    ///< �݂����폜
  case SC_OP_SHOOTER_CHARGE:  return "SC_OP_SHOOTER_CHARGE";     ///< �V���[�^�[�G�l���M�[�`���[�W
  case SC_OP_SET_ILLUSION:  return "SC_OP_SET_ILLUSION";        ///< �C�����[�W�����p�Q�ƃ|�P�����ύX
  case SC_OP_CLEAR_CONSUMED_ITEM:  return "SC_OP_CLEAR_CONSUMED_ITEM";///< �A�C�e��������̃N���A
  case SC_OP_CURESICK_DEPEND_POKE: return "SC_OP_CURESICK_DEPEND_POKE";
  case SC_ACT_WAZA_EFFECT:  return "SC_ACT_WAZA_EFFECT";
  case SC_ACT_TAMEWAZA_HIDE:  return "SC_ACT_TAMEWAZA_HIDE";     ///< �y�A�N�V�����z������ƂԂȂǂŉ�ʂ��������E�����ݒ�
  case SC_ACT_WAZA_DMG:  return "SC_ACT_WAZA_DMG";          ///< �y�A�N�V�����z[ AtClient, DefClient, wazaIdx, Affinity ]
  case SC_ACT_WAZA_DMG_PLURAL:  return "SC_ACT_WAZA_DMG_PLURAL";   ///< �y�A�N�V�����z�����̓����_���[�W���� [ pokeCnt, affAbout, ... ]
  case SC_ACT_WAZA_ICHIGEKI:  return "SC_ACT_WAZA_ICHIGEKI";     ///< �y�A�N�V�����z�ꌂ���U����
  case SC_ACT_SICK_ICON:  return "SC_ACT_SICK_ICON";         ///< �y�A�N�V�����z�Q�[�W�ɏ�Ԉُ�A�C�R���\��
  case SC_ACT_CONF_DMG:  return "SC_ACT_CONF_DMG";          ///< �y�A�N�V�����z�����񎩔��_���[�W [ pokeID ]
  case SC_ACT_RANKUP:  return "SC_ACT_RANKUP";            ///< �y�����N�A�b�v���ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  case SC_ACT_RANKDOWN:  return "SC_ACT_RANKDOWN";          ///< �y�����N�_�E�����ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  case SC_ACT_DEAD:  return "SC_ACT_DEAD";              ///< �y�|�P�����Ђ񂵁z
  case SC_ACT_RELIVE:  return "SC_ACT_RELIVE";            ///< �y�|�P���������Ԃ�z
  case SC_ACT_MEMBER_OUT_MSG:  return "SC_ACT_MEMBER_OUT_MSG";    ///< �y�|�P�����ޏꃁ�b�Z�[�W�z[ ClientID, pokeID ]
  case SC_ACT_MEMBER_OUT:  return "SC_ACT_MEMBER_OUT";        ///< �y�|�P�����ޏ�z[ ClientID, memberIdx ]
  case SC_ACT_MEMBER_IN:  return "SC_ACT_MEMBER_IN";         ///< �y�|�P�����C���z[ ClientID, posIdx, memberIdx ]
  case SC_ACT_WEATHER_DMG:  return "SC_ACT_WEATHER_DMG";       ///< �V��ɂ���ă_���[�W����[ weather, pokeCnt ]
  case SC_ACT_WEATHER_START:  return "SC_ACT_WEATHER_START";     ///< �V��ω�
  case SC_ACT_WEATHER_END:  return "SC_ACT_WEATHER_END";       ///< �^�[���`�F�b�N�œV��I��
  case SC_ACT_SIMPLE_HP:  return "SC_ACT_SIMPLE_HP";         ///< �V���v����HP�Q�[�W��������
  case SC_ACT_KINOMI:  return "SC_ACT_KINOMI";            ///< ���݂̂�H�ׂ�
  case SC_ACT_KILL:  return "SC_ACT_KILL";              ///< �����m�����o�i�݂��Â�A�ꌂ���U�Ȃǁj
  case SC_ACT_MOVE:  return "SC_ACT_MOVE";              ///< ���[�u
  case SC_ACT_EXP:  return "SC_ACT_EXP";               ///< �o���l�擾
  case SC_ACT_BALL_THROW:  return "SC_ACT_BALL_THROW";        ///< �{�[������
  case SC_ACT_ROTATION:  return "SC_ACT_ROTATION";          ///< ���[�e�[�V����
  case SC_ACT_CHANGE_TOKUSEI:  return "SC_ACT_CHANGE_TOKUSEI";    ///< �Ƃ������ύX
  case SC_ACT_SWAP_TOKUSEI:  return "SC_ACT_SWAP_TOKUSEI";            ///< �Ƃ������X���b�v
  case SC_ACT_FAKE_DISABLE:  return "SC_ACT_FAKE_DISABLE";      ///< �C�����[�W��������
  case SC_ACT_EFFECT_BYPOS:  return "SC_ACT_EFFECT_BYPOS";      ///< �w��ʒu�ɃG�t�F�N�g����
  case SC_ACT_EFFECT_BYVECTOR:  return "SC_ACT_EFFECT_BYVECTOR";   ///< �w�蔭���ʒu���I�_�ʒu�ŃG�t�F�N�g����
  case SC_ACT_CHANGE_FORM:  return "SC_ACT_CHANGE_FORM";       ///< �t�H�����i���o�[�`�F���W
  case SC_ACT_RESET_MOVE:  return "SC_ACT_RESET_MOVE";        ///< ���Z�b�g���[�u�J�b�g�C��
  case SC_ACT_MIGAWARI_CREATE: return "SC_ACT_MIGAWARI_CREATE";   ///< �݂����쐬
  case SC_ACT_MIGAWARI_DELETE: return "SC_ACT_MIGAWARI_DELETE";   ///< �݂�������
  case SC_ACT_HENSIN: return "SC_ACT_HENSIN";            ///< �ւ񂵂�
  case SC_ACT_MIGAWARI_DAMAGE: return "SC_ACT_MIGAWARI_DAMAGE"; ///< �݂����_���[�W
  case SC_ACT_WIN_BGM: return "SC_ACT_WIN_BGM";           ///< ����BGM�Đ�
  case SC_ACT_MSGWIN_HIDE: return "SC_ACT_MSGWIN_HIDE";  ///< ���b�Z�[�W�E�B���h�E�t�F�[�h�A�E�g
  case SC_TOKWIN_IN:  return "SC_TOKWIN_IN";             ///< �Ƃ������E�B���h�E�\���C�� [ClientID]
  case SC_TOKWIN_OUT:  return "SC_TOKWIN_OUT";            ///< �Ƃ������E�B���h�E�\���A�E�g [ClientID]
  case SC_MSG_WAZA:  return "SC_MSG_WAZA";              ///< ���U���b�Z�[�W�\��[ ClientID, wazaIdx ]
  case SC_MSG_STD:  return "SC_MSG_STD";               ///< ���b�Z�[�W�\�� [MsgID, numArgs, arg1, arg2, ... ]
  case SC_MSG_SET:  return "SC_MSG_SET";               ///< ���b�Z�[�W�\�� [MsgID, numArgs, arg1, arg2, ... ]
  case SC_MSG_STD_SE:  return "SC_MSG_STD_SE";            ///< ���b�Z�[�W�\����SE [MsgID, SENo, numArgs, arg1, arg2, ... ]
  }

  return NULL;
}


//=============================================================================================
/**
 * �o�͗L����Ԃ̃t�@�C��������
 *
 * @param   filename
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_DEBUGPRINT_IsEnable( const char* filename )
{
  return printCtrl_isEnableFile( filename );
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

/**
 *  �t�@�C���� - ���ʃV���{�����ϊ��e�[�u��
 */
static const struct {
  char* longName;
  char* shortName;
}FileNames[] = {
  { "btl_main.c",            "MAI" },
  { "btl_server.c",          "SVR" },
  { "btl_client.c",          "CLI" },
  { "btl_adapter.c",         "ADP" },
  { "btl_string.c",          "STR" },
  { "btl_net.c",             "NET" },
  { "btl_calc.c",            "CAL" },
  { "btl_sick.c",            "SIC" },
  { "btl_event.c",           "EVE" },
  { "btl_pokeparam.c",       "BPP" },
  { "btl_pokeset.c",         "SET" },
  { "btl_pospoke_state.c",   "POS" },
  { "btl_server_cmd.c",      "CMD" },
  { "btl_server_flow.c",     "SVF" },
  { "btl_server_flow_sub.c", "SUB" },
  { "btl_hem.c",             "HEM" },
  { "btl_field.c",           "FLD" },
  { "btl_rec.c",             "REC" },
  { "btlv_core.c",           "VIW" },
  { "btlv_scu.c",            "scU" },
  { "btlv_scd.c",            "scD" },
  { NULL,                    "OTR" },
};

static u32 gPrintBit = 0;
static int gCurrent = 0;

static void print_file_info( const char* filename, int line )
{
  u32 i;

  for(i=0; FileNames[i].longName!=NULL; ++i)
  {
    if( !GFL_STD_StrCmp(FileNames[i].longName, filename) )
    {
      break;
    }
  }
  OS_TPrintf( "[%s-%4d]", FileNames[i].shortName, line);
}


static void printCtrl_Reset( void )
{
  u32 i;
  gPrintBit = 0;
  for(i=0; i<NELEMS(FileNames); ++i){
    gPrintBit |= (1 << i);
  }
}
static BOOL  printCtrl_isEnable( u32 idx )
{
  return (gPrintBit & (1<<idx)) != 0;
}

static BOOL printCtrl_isEnableFile( const char* filename )
{
  u32 i;

  for(i=0; FileNames[i].longName!=NULL; ++i)
  {
    if( !GFL_STD_StrCmp(FileNames[i].longName, filename) )
    {
      break;
    }
  }
  return printCtrl_isEnable( i );
}


static void printCtrl_putCur( void )
{
  OS_TPrintf("[PrintCTRL]  %2d [%s] = ", gCurrent, FileNames[gCurrent].shortName );
  if( printCtrl_isEnable(gCurrent) ){
    OS_TPrintf("ON\n");
  }else{
    OS_TPrintf("OFF\n");
  }

}


void BTL_DEBUGPRINT_Ctrl( void )
{
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    u16 key = GFL_UI_KEY_GetTrg();

    if( key & PAD_KEY_UP ){
      if( --gCurrent < 0 ){
        gCurrent = NELEMS(FileNames) - 1;
      }
      printCtrl_putCur();
    }
    else if( key & PAD_KEY_DOWN ){
      if( ++gCurrent >= (NELEMS(FileNames)) ){
        gCurrent = 0;
      }
      printCtrl_putCur();
    }
    else if( key & PAD_KEY_LEFT ){
      gPrintBit &= ~( 1 << gCurrent);
      printCtrl_putCur();
    }
    else if( key & PAD_KEY_RIGHT ){
      gPrintBit |= ( 1 << gCurrent);
      printCtrl_putCur();
    }
    else if( key & PAD_BUTTON_R )
    {
      if( gPrintBit == 0 ){
        printCtrl_Reset();
        OS_TPrintf("[PrintCTRL] All ON\n");
      }else{
        gPrintBit = 0;
        OS_TPrintf("[PrintCTRL] All OFF\n");
      }
    }
  }
}

#endif