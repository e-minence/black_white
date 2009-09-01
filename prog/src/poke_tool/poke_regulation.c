//============================================================================================
/**
 * @file	poke_regulation.c
 * @bfief	���M�����[�V�����ɓK�����邩�̃c�[��
 * @author	k.ohno
 * @date	06.05.25
 */
//============================================================================================

#include "common.h"
#include "poketool/poke_regulation.h"
#include "poketool/pokeparty.h"
#include "itemtool/itemsym.h"

#include "poketool/monsno.h"
#include "savedata/regulation_data.h"

#define _POKENO_NONE  (0)          // �|�P�����ԍ��łȂ��ԍ�


//------------------------------------------------------------------
/**
 * �|�P���������M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�
 * @param   POKEMON_PARAM
 * @param   REGULATION
 * @return  ���v������TRUE
 */
//------------------------------------------------------------------

BOOL PokeRegulationCheckPokePara(const REGULATION* pReg, POKEMON_PARAM* pp)
{
  u16 mons = (u16)PokeParaGet( pp, ID_PARA_monsno, NULL );
  u16 item = (u16)PokeParaGet( pp, ID_PARA_item, NULL );
  int ans,level,weight;

  if(pReg==NULL){
    return TRUE;
  }
  //LV
  ans = Regulation_GetParam(pReg, REGULATION_LEVEL);
  range = Regulation_GetParam(pReg, REGULATION_LEVEL_RANGE);
  level = PokeParaGet(pp, ID_PARA_level, NULL);
  // ���x������������ꍇ����
  switch(range){
  case REGULATION_LEVEL_RANGE_OVER:
    if(ans > level){
      return FALSE;
    }
    break;
  case REGULATION_LEVEL_RANGE_LESS:
    if(ans < level){
      return FALSE;
    }
    break;
  }
  //���܂��Q��s��
  if( PokeParaGet(pp, ID_PARA_tamago_flag, NULL ) != 0 ){
    return FALSE;
  }
  // �Q���֎~�|�P���ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_POKE_BIT, mons)){
    return FALSE;
  }
  //�������ݕs�A�C�e�����ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_ITEM, item)){
    return FALSE;
  }
  return TRUE;
}

//------------------------------------------------------------------
/**
 * �|�P�p�[�e�B�����M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�
 * @param   POKEMON_PARAM
 * @param   REGULATION
 * @param   party��I�������z��  0�ȊO���I�����Ă���
 * @return
 */
//------------------------------------------------------------------

int PokeRegulationMatchFullPokeParty(const REGULATION* pReg, POKEPARTY * party, u8* sel)
{
  POKEMON_PARAM* pp;
  int ans,cnt = 0,j,i,level = 0,form=0;
  u16 monsTbl[6],itemTbl[6],formTbl[6];

  if(pReg==NULL){
    return POKE_REG_OK;
  }
  for(i = 0; i < 6 ;i++){
    monsTbl[i] = _POKENO_NONE;
    formTbl[i] = _POKENO_NONE;
    itemTbl[i] = ITEM_DUMMY_DATA;
    if(sel[i]){
      cnt++;
    }
  }

  //�S�̐�
  ans = Regulation_GetParam(pReg, REGULATION_NUM_LO);
  if(cnt < ans){
    return POKE_REG_NUM_FAILED;  // ���������ĂȂ�
  }
  ans = Regulation_GetParam(pReg, REGULATION_NUM_HI);
  if(cnt > ans){
    return POKE_REG_NUM_FAILED;  // ���������ĂȂ�
  }
  for(i = 0; i < 6 ;i++){
    if(sel[i]){
      int pid = sel[i] - 1;
      pp = PokeParty_GetMemberPointer(party, pid);
      if(PokeRegulationCheckPokePara(pReg, pp) == FALSE){
        return POKE_REG_ILLEGAL_POKE; // �̂�������������
      }
      monsTbl[i] = (u16)PokeParaGet( pp, ID_PARA_monsno, NULL );
      itemTbl[i] = (u16)PokeParaGet( pp, ID_PARA_item, NULL );
      formTbl[i] = (u16)PokeParaGet( pp, ID_PARA_form_no, NULL );
      level += PokeParaGet(pp,ID_PARA_level,NULL);
    }
  }
  //���vLV
  ans = Regulation_GetParam(pReg, REGULATION_LEVEL_TOTAL);
  if((level > ans) && (ans != 0)){
    return POKE_REG_TOTAL_LV_FAILED;
  }
  // �����|�P����
  ans = Regulation_GetParam(pReg, REGULATION_BOTH_POKE);
  if((ans == FALSE) && (cnt > 1)){  // �����|�P�����͂��߂� ��̈ȏ�̏ꍇ
    for(i = 0; i < (6-1); i++){
      for(j = i + 1;j < 6; j++){
        if((monsTbl[i] == monsTbl[j]) && (monsTbl[i] != _POKENO_NONE)){
          return POKE_REG_BOTH_POKE;
        }
      }
    }
  }
  // �����A�C�e��
  ans = Regulation_GetParam(pReg, REGULATION_BOTH_ITEM);
  if((ans == FALSE) && (cnt > 1)){  // �����A�C�e���͂��߂� ��̈ȏ�̏ꍇ
    for(i = 0; i < (6-1); i++){
      for(j = i + 1;j < 6; j++){
        if((itemTbl[i] == itemTbl[j]) && (monsTbl[i] != _POKENO_NONE) && (ITEM_DUMMY_DATA != itemTbl[i])){
          return POKE_REG_BOTH_ITEM;
        }
      }
    }
  }
  ans = Regulation_GetParam(pReg, REGULATION_MUST_POKE);
  form = Regulation_GetParam(pReg, REGULATION_MUST_POKE_FORM);
  if((ans) && (cnt > 1)){ //�K�{�|�P����������
    for(i = 0; i < 6; i++){
      if(monsTbl[i] == ans){
        if(formTbl[i] == form){
          break;
        }
      }
    }
  }
  if(i==6){
    return POKE_REG_NO_MASTPOKE;
  }

  return POKE_REG_OK;
}

//------------------------------------------------------------------
/**
 * �ċA�I��LV�������Z���ĖړI�ɒB������TRUE
 * @param   �����X�^�[�ԍ��̃e�[�u��
 * @param   ���x���������Ă�̃e�[�u��
 * @param   ���̃|�P�������������ꍇ��MARK
 * @param   totalPokeLv   �c���LV
 * @param   nowPokeIndex  ������v�Z����|�P����
 * @param   partyNum     CUP�Q���̎c��|�P������
 * @param   �|�P�p�[�e�B�[��
 * @return  ���v������TRUE
 */
//------------------------------------------------------------------

static BOOL _totalLevelCheck(u16* pMonsTbl,u16* pLevelTbl,u16* pMarkTbl,
                             int totalPokeLv,int nowPokeIndex, int partyNum, int cnt)
{
  int total = totalPokeLv;
  int i;
  int party = partyNum;

  if((pMonsTbl[nowPokeIndex] != _POKENO_NONE) && (pMarkTbl[nowPokeIndex] == 0)){
    total -= pLevelTbl[nowPokeIndex];
    if(total < 0){
      return FALSE;
    }
    party--;
    if(party == 0){
      return TRUE;
    }
    pMarkTbl[nowPokeIndex] = 1;  // �}�[�N����
  }
  for(i = nowPokeIndex+1;i < cnt;i++){  // �ċA������
    if(_totalLevelCheck(pMonsTbl,pLevelTbl,pMarkTbl, total, i, party, cnt)){
      return TRUE;
    }
  }
  pMarkTbl[nowPokeIndex] = 0;  // �}�[�N�͂���
  return FALSE;
}

//------------------------------------------------------------------
/**
 * �|�P�p�[�e�B���Ƀ��M�����[�V�����ɓK�����Ă���
 *   �p�[�e�B�[���g�߂邩�ǂ������ׂ� �K���O�̃|�P���������Ă����v
 *   �莝���A�C�e���͌������Ă��Ȃ�
 * @param   POKEMON_PARAM
 * @param   REGULATION
 * @param   ZKN_HEIGHT_GRAM_PTR
 * @return  _POKE_REG_NUM_FAILED �� _POKE_REG_TOTAL_FAILED �� _POKE_REG_OK
 */
//------------------------------------------------------------------

int PokeRegulationMatchPartialPokeParty(const REGULATION* pReg, POKEPARTY * party)
{
  POKEMON_PARAM* pp;
  int ans,cnt,j,i,level = 0;
  u16 monsTbl[6],levelTbl[6],markTbl[6];
  int partyNum;

  cnt = PokeParty_GetPokeCount(party);
  partyNum = cnt;

  MI_CpuClear8(markTbl,6*sizeof(u16));

  for(i = 0; i < cnt ;i++){
    pp = PokeParty_GetMemberPointer(party, i);
    monsTbl[i] = (u16)PokeParaGet( pp, ID_PARA_monsno, NULL );
    levelTbl[i] = PokeParaGet(pp,ID_PARA_level,NULL);
    if(PokeRegulationCheckPokePara(pReg, pp ) == FALSE){
      monsTbl[i] = _POKENO_NONE; // �ő̂��������������̂ŏ���
      partyNum--;
    }
  }
  // �����|�P���������߂ȏꍇ
  ans = Regulation_GetParam(pReg, REGULATION_BOTH_POKE);
  if((ans == 0) && (cnt > 1)){  // �����|�P������LV�̒Ⴂ�����c��
    for(i = 0; i < (cnt-1); i++){
      for(j = i + 1;j < cnt; j++){
        if((monsTbl[i] == monsTbl[j]) && (monsTbl[i] != _POKENO_NONE)){
          if(levelTbl[i] > levelTbl[j]){
            monsTbl[i] = _POKENO_NONE; // �������̂�����
          }
          else{
            monsTbl[j] = _POKENO_NONE; // �������̂�����
          }
          partyNum--;
        }
      }
    }
  }

  //�S�̐�
  ans = Regulation_GetParam(pReg, REGULATION_POKE_NUM);
  if(partyNum < ans){
    return POKE_REG_NUM_FAILED;  // �S�̐�������Ȃ�
  }
  partyNum = ans;

  //�c�����|�P�����̍��vLV�g�ݍ��킹����
  ans = Regulation_GetParam(pReg, REGULATION_TOTAL_LEVEL);
  if(ans == 0){
    return POKE_REG_OK;  // LV�����Ȃ�
  }
  for(i = 0;i < cnt;i++){
    if(_totalLevelCheck(monsTbl,levelTbl,markTbl, ans, i, partyNum, cnt)){
      return POKE_REG_OK;
    }
  }
  return POKE_REG_TOTAL_LV_FAILED;
}


