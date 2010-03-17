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

  case DBGSTR_CLIENT_RETURN_CMD_START:       return "ID[%d], �ԐM�J�n��\n";
  case DBGSTR_CLIENT_RETURN_CMD_DONE:        return "ID[%d], �ԐM���܂���\n";
  case DBGSTR_CLIENT_SelectActionSkip:       return "Act�I��(%d�̖ځj�X�L�b�v\n";
  case DBGSTR_CLIENT_SelActPokeDead:         return "����łăX�L�b�v�Ȃ̂�CheckedCnt���̂܂�\n";
  case DBGSTR_CLIENT_SelectActionStart:      return "Act�I��(%d�̖�=ID:%d�j�J�n���܂�  checked %d poke\n";
  case DBGSTR_CLIENT_SelectActionDone:       return "�J�o�[�ʒu��(%d)�I���A�A�N�V�������M��\n";
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
  case DBGSTR_CLIENT_UpdateWazaProcResult:          return "���U�v���Z�X���X�V : PokeID=%d, OrgWaza=%d, ActWaza=%d, ActTargetPos=%d, ActEnable=%d\n";
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
  case DBGSTR_CLIENT_ReadRecAct_Fight:     return "   action=Fight, waza=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Change:    return "   action=Change, nextPokeIdx=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Move:      return "   action=Move!\n";
  case DBGSTR_CLIENT_UnknownServerCmd:     return "�p�ӂ���Ă��Ȃ��R�}���hNo[%d]�I\n";
  case DBGSTR_CLIENT_RecPlayerBlackOut:    return "�^��Đ��R���g���[���F�u���b�N�A�E�g\n";
  case DBGSTR_CLIENT_RecPlay_ChapterSkipped: return "client(%d), �w��`���v�^[%d]�ɓ��B����..\n";


  case DBGSTR_BPP_NemuriWakeCheck:        return "�|�P[%d]�̂˂ނ�^�[���ő�l=%d, �o�߃^�[��=%d\n";

  case DBGSTR_VCORE_PokeListStart:      return "�������X�g���  ���[�h=%d   ���ł�%d�̑I������Ă���\n";
  case DBGSTR_VCORE_SelPokeEnd:         return "�|�P�I���������\n";
  case DBGSTR_VCORE_SelPokeEnd_Sel:     return "  [%d]�Ԗ� �I�����ꂽ -> %d�� �i�[\n";
  case DBGSTR_VCORE_SelPokeEnd_Unsel:   return "  [%d]�Ԗ� �I������Ă��Ȃ�\n";

  case DBGSTR_NET_Startup:              return "�����̃l�b�gID=%d, �ڑ������o�[��=%d\n";
  case DBGSTR_NET_DecideClientID:       return "NetID:%d -> clientID=%d\n";
  case DBGSTR_NET_SendSucceed:          return "  ���M����\n";
  case DBGSTR_NET_RecvedServerParam:    return "netID=%d, �T�[�o�p�����[�^��M���܂����B\n";
  case DBGSTR_NET_RecvedPartyData:      return "netID=%d, clientID=%d �̃p�[�e�B�f�[�^��M����, pData=%p, buf=%p\n";
  case DBGSTR_NET_PartyDataComplete:    return "�p�[�e�B�f�[�^���ݎ�M����  member=%d\n";
  case DBGSTR_NET_SendCmdDone:          return "�}�V��(%d) �� %d bytes ���M����\n";
  case DBGSTR_NET_RecvedAllClientsData: return "�S�N���C�A���g����̃f�[�^�Ԃ��Ă���\n";
  case DBGSTR_NET_ReturnToServerTrying: return "Try Return to Server %d byte ...";
  case DBGSTR_NET_RecvedClientData:     return "[BTLNET] recv from netID[%d], size=%d\n";
  case DBGSTR_NET_RecvedServerVersion:  return "�T�[�o�o�[�W������M ... netID=%d, version=%d\n";
  case DBGSTR_NET_ServerDetermine:      return "�T�[�o�� netID=%d �̃}�V���Ɍ���\n";
  case DBGSTR_NET_SendAITrainerData:    return "AI�g���[�i�[�f�[�^���M����, trID=%d\n";
  case DBGSTR_NET_RecvAITrainerData:    return "AI�g���[�i�[�f�[�^��M����, trID=%d\n";
  case DBGSTR_NET_AIPartyInfo:          return "Client(%d)=AI�p�[�e�B�Ɣ��f���� (pokeCnt=%d)\n  monsno=";
  case DBGSTR_NET_CreateAIPartyRecvBuffer: return "AI�p�[�e�B�f�[�^��M�o�b�t�@�𐶐�\n";
  case DBGSTR_NET_RecvedAIPartyData:     return "AI�p�[�e�B�f�[�^��M clientID=%d\n";

  case DBGSTR_CALCDMG_WazaParam:        return "���U���FID=%d, Type=%d\n";
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
  case DBGSTR_SVFL_WazaPower:           return "���UID=%d : �З�=%d  (ratio=%08x)\n";
  case DBGSTR_SVFL_UncategoryWazaInfo:  return "�����ރ��U �U��PokeID=%d, �Ώۃ|�P��=%d/%d\n";
  case DBGSTR_SVFL_QueWritePtr:         return "[* SVF *]         Que WritePtr=%d\n";
  case DBGSTR_SVFL_ClientPokeStillAlive:return "�N���C�A���g_%d (SIDE:%d) �̃|�P�͂܂����̂������Ă���\n";
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
  case DBGSTR_SVFL_ChangePokeReqInfo:   return "�N���C�A���g[%d]   �󂢂Ă���ʒu�̐�=%d\n";
  case DBGSTR_SVFL_TurnStart_Result:    return "�y�^�[���J�n�R�}���h�z�����A�N�V���� %d / %d\n";
  case DBGSTR_SVFL_ActOrderMainStart:   return "�R�}���h�����R�A �����A�N�V�����J�nIndex=%d\n";
  case DBGSTR_SVFL_ActOrderMainDropOut: return "Result=%d�ɂ�钆�r�T�[�o�ԐM\n";
  case DBGSTR_SVFL_ExpCalc_Base:        return "��{�ƂȂ�o���l=%d\n";
  case DBGSTR_SVFL_ExpCalc_MetInfo:     return "���S�|�P[%d]���Ζʂ�������|�P�̐�=%d, ���̓��A�����Ă鐔=%d\n";
  case DBGSTR_SVFL_ExpCalc_DivideInfo:  return "�����o�[Idx[%d]�̃|�P�Ɍo���l%d�𕪔z\n";
  case DBGSTR_SVFL_ExpCalc_Result:      return "�����o�[Idx[%d]�̃|�P�ɑ΂��A�ŏI�o���l=%d\n";
  case DBGSTR_SVFL_ExpAdjustCalc:       return "����Lv=%d, �GLv=%d, �{��=%08x, ��{�o���l=%d -> �␳��o���l=%d\n";
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

  case DBGSTR_POSPOKE_Out:              return " poke[%d] out from pos[%d]\n";
  case DBGSTR_POSPOKE_In:               return " poke[%d] in to pos[%d]\n";

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


  case DBGSTR_SC_PutCmd:                return "[QUE]PutCmd=%d, Format=%02x, argCnt=%d, args=";
  case DBGSTR_SC_ReservedPos:           return "[QUE]reserved pos=%d, wp=%d\n";
  case DBGSTR_SC_WriteReservedPos:      return "[QUE]Write Reserved Pos ... pos=%d, cmd=%d";
  case DBGSTR_SC_PutMsgParam:           return "[QUE] PUT  MSG SC=%d, StrID=%d";
  case DBGSTR_SC_ReadMsgParam:          return "[QUE] READ MSG SC=%d, StrID=%d";
  case DBGSTR_SC_PutMsg_SE:             return "  SE_ID=%d\n";
  case DBGSTR_SC_ArgsEqual:             return " args = ";
  case DBGSTR_SC_ReadCmd:               return "Read cmd=%d\n";
  case DBGSTR_SC_ReserveCmd:            return " -> reserved cmd=%d\n";


  case DBGSTR_EVENT_AddFactorInfo:      return "FactorType=%x, subPri=%x, TotalPriority=%x\n";
  case DBGSTR_EV_AddFactor:             return "[ADD] Factor=%p Depend=%d Type=%d, Pri=%06x [ADD]\n";
  case DBGSTR_EV_DelFactor:             return "[DEL] DEL Factor=%p Depend=%d, Type=%d [DEL]\n";
  case DBGSTR_EV_LinkInfoHeader:        return "***** [[EV Chain]] *****\n";
  case DBGSTR_EV_LinkInfoFooder:        return "************************\n\n";
  case DBGSTR_EV_LinkPtr:               return "%p->";
  case DBGSTR_EV_LinkEmpty:             return " empty...\n";
  case DBGSTR_EVAR_Push:                return "PUSH [%5d] SP=%d\n";
  case DBGSTR_EVAR_Pop:                 return "Pop! [%5d] SP=%d\n";
  case DBGSTR_DEADREC_Add:              return " ���S�L�^ �^�[��%d���� ... pokeID=%d\n";
  case DBGSTR_POKESET_RemoveDeadPoke:   return "�|�P(%d) ���񂾂̂ŃZ�b�g���珜�O\n";

  case DBGSTR_SCU_RelivePokeAct:        return "Pos[%d]=Vpos[%d] �|�P(%d)�������Ԃ�\n";
  case DBGSTR_Item_PinchReactItem:      return "PokeID=%d, �ő�HP=%d, ��HP=%d, n=%d ... ";
  case DBGSTR_Item_PinchReactOn:        return "��������";
  case DBGSTR_HANDWAZA_CombiWazaExe:    return "�|�P(%d)���|�P(%d�j�̃��U(%d)�ɑ����č��̃��U����->����=%d\n";

  case DBGSTR_REC_ReadActStart:         return "rec seek start RP= %d\n";
  case DBGSTR_REC_ReadActSkip:          return "rec seek RotateData skip %d byte\n";
  case DBGSTR_REC_SeekClient:           return "rec seek numClient=%d\n";
  case DBGSTR_REC_ReadActParam:         return "rec ReadPtr=%d, act=%d, waza=%d\n";


  case DBGSTR_csv:  return "%d,";
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
    { "btl_main.c",           "MAI" },
    { "btl_server.c",         "SVR" },
    { "btl_client.c",         "CLI" },
    { "btl_adapter.c",        "ADP" },
    { "btl_string.c",         "STR" },
    { "btl_net.c",            "NET" },
    { "btl_calc.c",           "CAL" },
    { "btl_sick.c",           "SIC" },
    { "btl_event.c",          "EVE" },
    { "btl_pokeparam.c",      "BPP" },
    { "btl_pokeset.c",        "SET" },
    { "btl_pospoke_state.c",  "POS" },
    { "btl_server_cmd.c",     "CMD" },
    { "btl_server_flow.c",    "FLW" },
    { "btl_field.c",          "FLD" },
    { "btl_rec.c",            "REC" },
    { "btlv_core.c",          "VIW" },
    { "btlv_scu.c",           "scU" },
    { "btlv_scd.c",           "scD" },
    { NULL,                   "OTR" },
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
