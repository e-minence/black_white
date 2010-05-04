//==============================================================================
/**
 * @file    game_beacon_nw.c
 * @brief   �Q�[�����̃r�[�R�����Ǘ��V�X�e��(NOT_WIFI�I�[�o�[���C)
 * @author  matsuda & iwasawa
 * @date    2010.05.01
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "msg/msg_beacon_status.h"
#include "savedata/encount_sv.h"
#include "savedata/misc.h"
#include "net/net_whpipe.h"
#include "savedata/my_pms_data.h"
#include "app/townmap_util.h"
#include "net_app\union\union_beacon_tool.h"
#include "app/research_radar/research_data.h"   //QUESTION_NUM_PER_TOPIC
#include "app/research_radar/question_id.h"
#include "app/research_radar/questionnaire_index.h"
#include "savedata/playtime.h"
#include "field/research_team_def.h"

#include "game_beacon_local.h"

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================

//==============================================================================
//
//  �r�[�R���Z�b�g
//
//==============================================================================
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattlePokeStart(u16 monsno)
{
  if( GAMEBEACON_SUB_PokeBeaconTypeIsSpecial( monsno )){
    GAMEBEACON_Set_BattleSpPokeStart(monsno);
  }else{
    GAMEBEACON_Set_BattleWildPokeStart(monsno);
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattlePokeVictory(u16 monsno)
{
  if( GAMEBEACON_SUB_PokeBeaconTypeIsSpecial( monsno )){
    GAMEBEACON_Set_BattleSpPokeVictory(monsno);
  }else{
    GAMEBEACON_Set_BattleWildPokeVictory(monsno);
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleWildPokeStart(u16 monsno)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_WILD_POKE_START) == FALSE){ return; };
  BEACONINFO_Set_BattleWildPokeStart(&GameBeaconSys->send.info, monsno);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�쐶�̃|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleWildPokeStart(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_WILD_POKE_START;
  info->action.monsno = monsno;
 
  BEACONINFO_Set_Details_BattleWildPoke(info, monsno);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�̃|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleWildPokeVictory(u16 monsno)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleWildPokeVictory(&GameBeaconSys->send.info, monsno);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�쐶�̃|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleWildPokeVictory(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃ|�P�����Ƒΐ���J�n���܂���
 * @param   tr_no   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpPokeStart(u16 monsno)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_POKE_START) == FALSE){ return; };
  BEACONINFO_Set_BattleSpPokeStart(&GameBeaconSys->send.info, monsno);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃ|�P�����Ƒΐ���J�n���܂���
 * @param   tr_no   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpPokeStart(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_POKE_START;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_BattleSpPoke(info, monsno);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃ|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpPokeVictory(u16 monsno)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleSpPokeVictory(&GameBeaconSys->send.info, monsno);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃ|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpPokeVictory(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY;
  info->action.monsno = monsno;
  
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleTrainerStart(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_TRAINER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleTrainerStart(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleTrainerStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_TRAINER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleTrainerVictory(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleTrainerVictory(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�W�����[�_�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleLeaderStart(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_LEADER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleLeaderStart(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleLeaderStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_LEADER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleLeader(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�W�����[�_�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleLeaderVictory(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleLeaderVictory(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�W�����[�_�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleLeaderVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpTrainerStart(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_TRAINER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleSpTrainerStart(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpTrainerStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_TRAINER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleSpTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpTrainerVictory(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_TRAINER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleSpTrainerVictory(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_TRAINER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�`�����s�I���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleChampionStart(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_CHAMPION_START) == FALSE){ return; };
  BEACONINFO_Set_BattleChampionStart(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�`�����s�I���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleChampionStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_CHAMPION_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleSpTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�`�����s�I���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleChampionVictory(u16 tr_no)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleChampionVictory(&GameBeaconSys->send.info, tr_no);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�`�����s�I���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleChampionVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * G�p���[����
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void GAMEBEACON_Set_GPower(GPOWER_ID g_power_id)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_GPOWER) == FALSE){ return; };
  BEACONINFO_Set_GPower(&GameBeaconSys->send.info, g_power_id);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * G�p���[����
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void BEACONINFO_Set_GPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id)
{
  info->action.action_no = GAMEBEACON_ACTION_GPOWER;
  info->g_power_id = g_power_id;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�t�B�[���h�ɗ����Ă���A�C�e�����v���C���[������
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void GAMEBEACON_Set_SpItemGet(u16 item)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SP_ITEM_GET) == FALSE){ return; };
  BEACONINFO_Set_SpItemGet(&GameBeaconSys->send.info, item);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�t�B�[���h�ɗ����Ă���A�C�e�����v���C���[������
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void BEACONINFO_Set_SpItemGet(GAMEBEACON_INFO *info, u16 item)
{
  info->action.action_no = GAMEBEACON_ACTION_SP_ITEM_GET;
  info->action.itemno = item;

  BEACONINFO_Set_Details_Walk(info);
}
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����}�ӂ�����
 */
//==================================================================
void GAMEBEACON_Set_ZukanComplete(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_ZUKAN_COMPLETE) == FALSE){ return; };
  BEACONINFO_Set_ZukanComplete(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����}�ӂ�����
 */
//==================================================================
void BEACONINFO_Set_ZukanComplete(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_ZUKAN_COMPLETE;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F������󂯂��񐔂��K�萔�𒴂���
 * @param   thankyou_count	�����
 */
//==================================================================
void GAMEBEACON_Set_ThankyouOver(u32 thankyou_count)
{
  static const u8 regu_count[] = {10, 30, 50, 100, 200};
  int i;
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_THANKYOU_OVER) == FALSE){ return; };
  for(i = 0; i < NELEMS(regu_count); i++){
    if(thankyou_count == regu_count[i]){
      BEACONINFO_Set_ThankyouOver(&GameBeaconSys->send.info, thankyou_count);
      GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
      return;
    }
  }
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F������󂯂��񐔂��K�萔�𒴂���
 * @param   thankyou_count	�����
 */
//==================================================================
void BEACONINFO_Set_ThankyouOver(GAMEBEACON_INFO *info, u32 thankyou_count)
{
  info->action.action_no = GAMEBEACON_ACTION_THANKYOU_OVER;
  info->action.thankyou_count = thankyou_count;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���j�I�����[������
 */
//==================================================================
void GAMEBEACON_Set_UnionIn(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_UNION_IN) == FALSE){ return; };
  BEACONINFO_Set_UnionIn(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���j�I�����[������
 */
//==================================================================
void BEACONINFO_Set_UnionIn(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_UNION_IN;

  BEACONINFO_Set_Details_Walk(info);
}
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬���瓦���o����
 */
//==================================================================
void GAMEBEACON_Set_Escape(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_ESCAPE) == FALSE){ return; };
  BEACONINFO_Set_Escape(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�퓬���瓦���o����
 */
//==================================================================
void BEACONINFO_Set_Escape(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_ESCAPE;

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������HP��5���ȏ㏭�Ȃ��Ȃ��Ă���
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_HPLittle(const STRBUF *nickname)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_HP_LITTLE) == FALSE){ return; };
  BEACONINFO_Set_HPLittle(&GameBeaconSys->send.info, nickname);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������HP��5���ȏ㏭�Ȃ��Ȃ��Ă���
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_HPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_HP_LITTLE;
  GAMEBEACON_SUB_StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������PP��1�ł�0�ɂȂ��Ă���
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PPLittle(const STRBUF *nickname)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_PP_LITTLE) == FALSE){ return; };
  BEACONINFO_Set_PPLittle(&GameBeaconSys->send.info, nickname);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������PP��1�ł�0�ɂȂ��Ă���
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_PPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_PP_LITTLE;
  GAMEBEACON_SUB_StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�퓬�̃|�P�������m��
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_Dying(const STRBUF *nickname)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_DYING) == FALSE){ return; };
  BEACONINFO_Set_Dying(&GameBeaconSys->send.info, nickname);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P�������m��
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_Dying(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_DYING;
  GAMEBEACON_SUB_StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�퓬�̃|�P��������Ԉُ�
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_StateIsAbnormal(const STRBUF *nickname)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_STATE_IS_ABNORMAL) == FALSE){ return; };
  BEACONINFO_Set_StateIsAbnormal(&GameBeaconSys->send.info, nickname);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P��������Ԉُ�
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_StateIsAbnormal(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_STATE_IS_ABNORMAL;
  GAMEBEACON_SUB_StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�t�B�[���h�Z���g�p
 *
 * @param   wazano    �Z�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_FieldSkill(u16 wazano)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_FIELD_SKILL) == FALSE){ return; };
  BEACONINFO_Set_FieldSkill(&GameBeaconSys->send.info, wazano);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�t�B�[���h�Z���g�p
 *
 * @param   info		
 * @param   wazano		�Z�ԍ�
 */
//==================================================================
void BEACONINFO_Set_FieldSkill(GAMEBEACON_INFO *info, u16 wazano)
{
  info->action.action_no = GAMEBEACON_ACTION_FIELD_SKILL;
  info->action.wazano = wazano;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S����ĉ�����󂯎����
 */
//==================================================================
void GAMEBEACON_Set_SodateyaEgg( void )
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SODATEYA_EGG) == FALSE){ return; };
  BEACONINFO_Set_SodateyaEgg(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�^�}�S����ĉ�����󂯎����
 *
 * @param   info		
 */
//==================================================================
void BEACONINFO_Set_SodateyaEgg(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SODATEYA_EGG;
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S���z��
 *
 * @param   monsno		�z�������^�}�S�̃|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_EggHatch( u16 monsno )
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_EGG_HATCH) == FALSE){ return; };
  BEACONINFO_Set_EggHatch(&GameBeaconSys->send.info, monsno );
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S���z��
 *
 * @param   info		
 * @param   monsno		�z�������^�}�S�̃|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_EggHatch(GAMEBEACON_INFO *info, u16 monsno )
{
  info->action.action_no = GAMEBEACON_ACTION_EGG_HATCH;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F��������
 */
//==================================================================
void GAMEBEACON_Set_Shoping(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SHOPING) == FALSE){ return; };
  BEACONINFO_Set_Shoping(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F��������
 */
//==================================================================
void BEACONINFO_Set_Shoping(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SHOPING;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C���풆
 */
//==================================================================
void GAMEBEACON_Set_Subway(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SUBWAY) == FALSE){ return; };
  BEACONINFO_Set_Subway(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C���풆
 */
//==================================================================
void BEACONINFO_Set_Subway(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C�A����
 *
 * @param   victory_count		�A����
 */
//==================================================================
void GAMEBEACON_Set_SubwayStraightVictories(u32 victory_count)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES) == FALSE){ return; };
  BEACONINFO_Set_SubwayStraightVictories(&GameBeaconSys->send.info, victory_count);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C�A����(���E���h��)
 *
 * @param   victory_count		�����i�񂾃��E���h��
 *
 * �L�^�Ƃ��ĐL�тĂ���<�A����>�ł͂Ȃ��A7��1�Z�b�g�̉����ڂ���1�`7�œn���Ă�������
 */
//==================================================================
void BEACONINFO_Set_SubwayStraightVictories(GAMEBEACON_INFO *info, u32 victory_count)
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES;
  info->action.victory_count = victory_count;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C�Ńg���t�B�[�擾
 *
 * @param   victory_count		�A����
 */
//==================================================================
void GAMEBEACON_Set_SubwayTrophyGet( void )
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_SUBWAY_TROPHY_GET) == FALSE){ return; };
  BEACONINFO_Set_SubwayTrophyGet(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C�Ńg���t�B�[�Q�b�g
 *
 * @param   victory_count		�A����
 */
//==================================================================
void BEACONINFO_Set_SubwayTrophyGet( GAMEBEACON_INFO *info )
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY_TROPHY_GET;
  info->action.victory_count = 0;
  
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���C�A���n�E�X�ɒ��풆
 */
//==================================================================
void GAMEBEACON_Set_TrialHouse(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_TRIALHOUSE) == FALSE){ return; };
  BEACONINFO_Set_TrialHouse(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���C�A���n�E�X�ɒ��풆
 */
//==================================================================
void BEACONINFO_Set_TrialHouse(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_TRIALHOUSE;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���C�A���n�E�X�Ń����N�m��
 *
 * @param   rank      TH_RANK_xxx
 */
//==================================================================
void GAMEBEACON_Set_TrialHouseRank(u8 rank)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_TRIALHOUSE_RANK) == FALSE){ return; };
  BEACONINFO_Set_TrialHouseRank(&GameBeaconSys->send.info, rank);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���C�A���n�E�X�Ń����N�m��
 *
 * @param   info		
 * @param   rank		TH_RANK_xxx
 */
//==================================================================
void BEACONINFO_Set_TrialHouseRank(GAMEBEACON_INFO *info, u8 rank)
{
  info->action.action_no = GAMEBEACON_ACTION_TRIALHOUSE_RANK;
  info->action.trial_house_rank = rank;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�ϗ��Ԃɏ����
 */
//==================================================================
void GAMEBEACON_Set_FerrisWheel(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_FERRIS_WHEEL) == FALSE){ return; };
  BEACONINFO_Set_FerrisWheel(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�ϗ��Ԃɏ����
 */
//==================================================================
void BEACONINFO_Set_FerrisWheel(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_FERRIS_WHEEL;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�V�t�^�[����
 */
//==================================================================
void GAMEBEACON_Set_PokeShifter(void)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_POKESHIFTER) == FALSE){ return; };
  BEACONINFO_Set_PokeShifter(&GameBeaconSys->send.info);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�V�t�^�[����
 */
//==================================================================
void BEACONINFO_Set_PokeShifter(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_POKESHIFTER;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�~���[�W�J������
 *
 * @param   nickname		�Q�������|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_Musical(const STRBUF *nickname)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_MUSICAL) == FALSE){ return; };
  BEACONINFO_Set_Musical(&GameBeaconSys->send.info, nickname);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�~���[�W�J������
 *
 * @param   info		
 * @param   nickname		�Q�������|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_Musical(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_MUSICAL;
  GAMEBEACON_SUB_StrbufNicknameCopy(nickname, info->action.normal.nickname);

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���l�̃v���C���[����󂯎����G�p���[���g�p
 *
 * @param   gpower_id		G�p���[ID
 */
//==================================================================
void GAMEBEACON_Set_OtherGPowerUse(GPOWER_ID gpower_id)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_OTHER_GPOWER_USE) == FALSE){ return; };
  BEACONINFO_Set_OtherGPowerUse(&GameBeaconSys->send.info, gpower_id);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���l�̃v���C���[����󂯎����G�p���[���g�p
 *
 * @param   info		
 * @param   gpower_id		G�p���[ID
 */
//==================================================================
void BEACONINFO_Set_OtherGPowerUse(GAMEBEACON_INFO *info, GPOWER_ID gpower_id)
{
  info->action.action_no = GAMEBEACON_ACTION_OTHER_GPOWER_USE;
  info->action.gpower_id = gpower_id;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�u���肪�Ƃ��I�v
 *
 * @param   gamedata		
 * @param   target_trainer_id   ��������鑊��̃g���[�i�[ID
 */
//==================================================================
void GAMEBEACON_Set_Thankyou(GAMEDATA *gamedata, u32 target_trainer_id)
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_THANKYOU) == FALSE){ return; };
  BEACONINFO_Set_Thankyou(&GameBeaconSys->send.info, gamedata, target_trainer_id);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�u���肪�Ƃ��I�v
 *
 * @param   gamedata		
 * @param   target_trainer_id   ��������鑊��̃g���[�i�[ID
 */
//==================================================================
void BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id)
{
  const MISC *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(gamedata) );

  STRTOOL_Copy(MISC_CrossComm_GetThankyouMessage(misc), 
  info->action.thanks.thankyou_message, GAMEBEACON_THANKYOU_MESSAGE_LEN);
  info->action.action_no = GAMEBEACON_ACTION_THANKYOU;
  info->action.thanks.target_trainer_id = target_trainer_id;
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�ꌾ���b�Z�[�W
 *
 * @param   gamedata		
 * @param   free_word   �ꌾ���b�Z�[�W
 */
//==================================================================
void GAMEBEACON_Set_FreeWord( const STRBUF *free_word )
{
  if(GAMEBEACON_SUB_CheckPriority(GAMEBEACON_ACTION_FREEWORD) == FALSE){ return; };
  BEACONINFO_Set_FreeWord(&GameBeaconSys->send.info, free_word);
  GAMEBEACON_SUB_SendBeaconSetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�ꌾ���b�Z�[�W
 *
 * @param   gamedata		
 * @param   free_word   �ꌾ���b�Z�[�W
 */
//==================================================================
void BEACONINFO_Set_FreeWord(GAMEBEACON_INFO *info, const STRBUF *free_word)
{
  GFL_STR_GetStringCode(free_word, info->action.freeword_message, GAMEBEACON_FREEWORD_MESSAGE_LEN);
  info->action.action_no = GAMEBEACON_ACTION_FREEWORD;
}

//==================================================================
/**
 * ���M���̑��Z�b�g�F������󂯂���
 *
 * @param   count		
 */
//==================================================================
static void GAMEBEACON_Set_ThanksRecvCount(u32 count)
{
  BEACONINFO_Set_ThanksRecvCount(&GameBeaconSys->send.info, count);
}

//==================================================================
/**
 * ���̑��Z�b�g�F������󂯂���
 *
 * @param   count		
 */
//==================================================================
void BEACONINFO_Set_ThanksRecvCount(GAMEBEACON_INFO *info, u32 count)
{
  info->thanks_recv_count = count;
}

//==================================================================
/**
 * ���M���̑��Z�b�g�F���������l��
 *
 * @param   count		
 */
//==================================================================
static void GAMEBEACON_Set_SuretigaiCount(u32 count)
{
  BEACONINFO_Set_SuretigaiCount(&GameBeaconSys->send.info, count);
}

//==================================================================
/**
 * ���̑��Z�b�g�F���������l��
 *
 * @param   count		
 */
//==================================================================
void BEACONINFO_Set_SuretigaiCount(GAMEBEACON_INFO *info, u32 count)
{
  info->suretigai_count = count;
}

//==============================================================================
//  �ڍ׃p�����[�^
//==============================================================================

