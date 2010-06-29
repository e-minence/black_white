//============================================================================================
/**
 * @file	poke_regulation.c
 * @bfief	���M�����[�V�����ɓK�����邩�̃c�[��
 * @author	k.ohno
 * @date	06.05.25
 */
//============================================================================================

#include <gflib.h>
#include "buflen.h"
#include "savedata/save_control.h"
#include "poke_tool/poke_regulation.h"
#include "poke_tool/pokeparty.h"
#include "item/itemsym.h"
#include "print/global_msg.h"

#include "poke_tool/monsno_def.h"

#include "arc_def.h"
#include "regulation.naix"

#include "pm_define.h"
#include "msg/msg_regulation.h"
#include "message.naix"

#define _POKENO_NONE  (0)          // �|�P�����ԍ��łȂ��ԍ�

static const int PokeRegulationCheckPokeParty_Func2( const REGULATION* reg , POKEPARTY *party , const u8 *checkArr ,  u8 *retArr , int *errBit );

//------------------------------------------------------------------
/**
 * @brief   �`���|�P�������ǂ���
 * @param   monsno  �����X�^�[�ԍ�
 * @return  ���v������TRUE
 */
//------------------------------------------------------------------

BOOL PokeRegulationCheckLegend(u16 monsno)
{
  int i;
  static const u16 cut_check_monsno[] = {
    MONSNO_151,				// �~���E
    MONSNO_251,			// �Z���r�B
    MONSNO_385,			// �W���[�`
    MONSNO_386,		// �f�I�L�V�X
    MONSNO_490,			// �}�i�t�B
    MONSNO_491,		// �_�[�N���C
    MONSNO_492,			// �V�F�C�~
    MONSNO_493,		// �A���Z�E�X
		MONSNO_654,	// �_���^�j�X
		MONSNO_655,		// �����f�B�A
		MONSNO_656,		// �C���Z�N�^
		MONSNO_657,			// �r�N�e�B�j
  };

  for(i=0;i < elementof(cut_check_monsno) ;i++){
    if(cut_check_monsno[i] == monsno){
      return TRUE;
    }
  }
  return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief  �|�P���������M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�
 * @param   REGULATION     ���M�����[�V�����\���̃|�C���^
 * @param   POKEMON_PARAM  �|�P�p��
 * @return  ���v������TRUE
 */
//------------------------------------------------------------------

const int PokeRegulationCheckPokePara(const REGULATION* pReg, POKEMON_PARAM* pp)
{
  u16 mons = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
  u16 item = (u16)PP_Get( pp, ID_PARA_item, NULL );
  int ans,level,weight,range;

  if(pReg==NULL){
    return TRUE;
  }
  //LV
  ans = Regulation_GetParam(pReg, REGULATION_LEVEL);
  range = Regulation_GetParam(pReg, REGULATION_LEVEL_RANGE);
  level = PP_Get(pp, ID_PARA_level, NULL);
  // ���x������������ꍇ����
  switch(range){
  case REGULATION_LEVEL_RANGE_OVER:
    if(ans > level){
      return POKEFAILEDBIT_LEVEL;
    }
    break;
  case REGULATION_LEVEL_RANGE_LESS:
    if(ans < level){
      return POKEFAILEDBIT_LEVEL;
    }
    break;
  }
  //���܂��Q��s��
  if( PP_Get(pp, ID_PARA_tamago_flag, NULL ) != 0 ){
    return POKEFAILEDBIT_EGG;
  }
  // �Q���֎~�|�P���ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_POKE_BIT, mons)){
    return POKEFAILEDBIT_VETO_POKE;
  }
  //�������ݕs�A�C�e�����ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_ITEM, item)){
    return POKEFAILEDBIT_VETO_ITEM;
  }
  return POKEFAILEDBIT_NONE;
}

//------------------------------------------------------------------
/**
 * @brief  �|�P�p�[�e�B�����M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�
 * @param   REGULATION     ���M�����[�V�����\���̃|�C���^
 * @param   POKEMON_PARAM
 * @param   party��I�������z��  0�ȊO���I�����Ă���
 * @return  POKE_REG_RETURN_ENUM
 */
//------------------------------------------------------------------

int PokeRegulationMatchFullPokeParty(const REGULATION* pReg, POKEPARTY * party, u8* sel)
{
  if(pReg==NULL)
  {
    return POKE_REG_OK;
  }
  {
    u8 retArr[6];
    int errBit;
    
    const BOOL ret = PokeRegulationCheckPokeParty_Func2( pReg , party , sel , retArr , &errBit );
    if( errBit == POKEFAILEDBIT_NONE )
    {
      return POKE_REG_OK;
    }
    else
    {
      if( (errBit & POKEFAILEDBIT_LEVEL)     ||
          (errBit & POKEFAILEDBIT_VETO_POKE) ||
          (errBit & POKEFAILEDBIT_EGG)       ||
          (errBit & POKEFAILEDBIT_VETO_ITEM) )
      {
        return POKE_REG_ILLEGAL_POKE;
      }
      else
      if( errBit & POKEFAILEDBIT_BOTHPOKE )
      {
        return POKE_REG_BOTH_POKE;
      }
      else
      if( errBit & POKEFAILEDBIT_BOTHITEM )
      {
        return POKE_REG_BOTH_ITEM;
      }
      else
      if( errBit & POKEFAILEDBIT_SUM_LEVEL )
      {
        return POKE_REG_TOTAL_LV_FAILED;
      }
      else
      if( errBit & POKEFAILEDBIT_MAST_POKE )
      {
        return POKE_REG_NO_MASTPOKE;
      }
      else
      if( errBit & POKEFAILEDBIT_NUM )
      {
        return POKE_REG_NUM_FAILED;
      }
    }
    GF_ASSERT_MSG( 0 , "Check failue[%d]\n",errBit );
  }
  return POKE_REG_OK;
}

//------------------------------------------------------------------
/**
 * @brief  �ċA�I��LV�������Z���ĖړI�ɒB������TRUE
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
 * @brief �|�P�p�[�e�B���Ƀ��M�����[�V�����ɓK�����Ă���
 *   �p�[�e�B�[���g�߂邩�ǂ������ׂ� �K���O�̃|�P���������Ă����v
 *   �莝���A�C�e���͌������Ă��Ȃ�
 * @param   POKEMON_PARAM
 * @param   REGULATION
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
    monsTbl[i] = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
    levelTbl[i] = PP_Get(pp,ID_PARA_level,NULL);
    if(PokeRegulationCheckPokePara(pReg, pp ) != POKEFAILEDBIT_NONE){
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
  ans = Regulation_GetParam(pReg, REGULATION_NUM_LO);
  if(partyNum < ans){
    return POKE_REG_NUM_FAILED;  // ���������ĂȂ�
  }
//  ans = Regulation_GetParam(pReg, REGULATION_NUM_HI);  //�����ōő��r�͕K�v�Ȃ�ohno
  //if(partyNum < ans){
 //   return POKE_REG_NUM_FAILED;  // ���������ĂȂ�
//  }
  //partyNum = ans;

  //�c�����|�P�����̍��vLV�g�ݍ��킹����
  /*
    ans = Regulation_GetParam(pReg, REGULATION_LEVEL_TOTAL);
    if(ans == 0){
      return POKE_REG_OK;  // LV�����Ȃ�
    }
    for(i = 0;i < cnt;i++){
      if(_totalLevelCheck(monsTbl,levelTbl,markTbl, ans, i, partyNum, cnt)){
        return POKE_REG_OK;
      }
    }
  */
  {
    //������`�F�b�N
    //�K�{�|�P�����܂߂����v���x���Ȃǂ̃`�F�b�N
    const BOOL ret = PokeRegulationCheckPokeParty_Func( pReg,party,NULL );
    if( ret == TRUE )
    {
      return POKE_REG_OK;
    }
  }
  return POKE_REG_TOTAL_LV_FAILED;
}




//------------------------------------------------------------------
/**
 * @brief  �|�P���������M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ� �V���`��=���������Ή�
 * @param   REGULATION     ���M�����[�V�����\���̃|�C���^
 * @param   POKEMON_PARAM  �|�P�p��
 * @param   FailedBit      �����ᔽ���r�b�g�ŕԂ�
 * @return  ���v������TRUE
 */
//------------------------------------------------------------------

BOOL PokeRegulationCheckPokeParaLookAt(const REGULATION* pReg, POKEMON_PARAM* pp,u32* FailedBit)
{
  u16 mons = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
  u16 item = (u16)PP_Get( pp, ID_PARA_item, NULL );
  int ans,level,weight,range;
  BOOL ret=TRUE;

  if(pReg==NULL){
    return ret;
  }
  //LV
  ans = Regulation_GetParam(pReg, REGULATION_LEVEL);
  range = Regulation_GetParam(pReg, REGULATION_LEVEL_RANGE);
  level = PP_Get(pp, ID_PARA_level, NULL);
  // ���x������������ꍇ����
  switch(range){
  case REGULATION_LEVEL_RANGE_OVER:
    if(ans > level){
      ret = FALSE;
      *FailedBit |= POKEFAILEDBIT_LEVEL;
    }
    break;
  case REGULATION_LEVEL_RANGE_LESS:
    if(ans < level){
      ret = FALSE;
      *FailedBit |= POKEFAILEDBIT_LEVEL;
    }
    break;
  }
  // �Q���֎~�|�P���ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_POKE_BIT, mons)){
    if( PP_Get(pp, ID_PARA_tamago_flag, NULL ) == 0 ){  //�^�}�S�̎Q���֎~�|�P��OK
      ret = FALSE;
      *FailedBit |= POKEFAILEDBIT_VETO_POKE;
    }
  }
  //�������ݕs�A�C�e�����ǂ���
  if(Regulation_CheckParamBit(pReg, REGULATION_VETO_ITEM, item)){
    ret = FALSE;
    *FailedBit |= POKEFAILEDBIT_VETO_ITEM;
  }
  return ret;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�p�[�e�B�����M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ� �V���`��=���������Ή�
            �����ɂ����Ă��Ȃ����̂͐��ɂ�����炸���ׂĂ͂���
            Lv���v�͒��ׂĂ��Ȃ�
 * @param   REGULATION     ���M�����[�V�����\���̃|�C���^
 * @param   POKEPARTY
 * @param   FailedBit      �����ᔽ���r�b�g�ŕԂ�
 * @return  POKE_REG_RETURN_ENUM
 */
//------------------------------------------------------------------

int PokeRegulationMatchLookAtPokeParty(const REGULATION* pReg, POKEPARTY * party, u32* FailedBit)
{
  POKEMON_PARAM* pp;
  int ans,cnt = 0,j,i,level = 0,form=0;
  u16 monsTbl[6],itemTbl[6],formTbl[6];
  int ret = POKE_REG_OK;

  if(pReg==NULL){
    return POKE_REG_OK;
  }
  GF_ASSERT(FailedBit);  //if���ł����ς��ɂȂ�̂ŌĂяo�����ŕK���Z�b�g���鎖
  for(i = 0; i < 6 ;i++){
    monsTbl[i] = _POKENO_NONE;
    formTbl[i] = _POKENO_NONE;
    itemTbl[i] = ITEM_DUMMY_DATA;
  }

  //�S�̐�
  cnt = PokeParty_GetPokeCountBattleEnable(party);

  ans = Regulation_GetParam(pReg, REGULATION_NUM_LO);
  if(cnt < ans){
    *FailedBit |= POKEFAILEDBIT_NUM;
    ret = POKE_REG_NUM_FAILED;  // ���������ĂȂ�
  }
  for(i = 0; i < PokeParty_GetPokeCount(party) ;i++){
    pp = PokeParty_GetMemberPointer(party, i);
    if(PokeRegulationCheckPokeParaLookAt(pReg, pp, FailedBit ) == FALSE){
      ret = POKE_REG_ILLEGAL_POKE; // �̂�������������
    }
    //���܂��Q��s��   //�^�}�S�͎莝���ɂ��Ă��ǂ����@�l���ɂ͐����Ȃ�
    if( PP_Get(pp, ID_PARA_tamago_flag, NULL ) == 0 ){
      monsTbl[i] = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
      itemTbl[i] = (u16)PP_Get( pp, ID_PARA_item, NULL );
      formTbl[i] = (u16)PP_Get( pp, ID_PARA_form_no, NULL );
      level += PP_Get(pp,ID_PARA_level,NULL);
    }
  }
  
  // �����|�P����
  ans = Regulation_GetParam(pReg, REGULATION_BOTH_POKE);
  if((ans == FALSE) && (cnt > 1)){  // �����|�P�����͂��߂� ��̈ȏ�̏ꍇ
    for(i = 0; i < (6-1); i++){
      for(j = i + 1;j < 6; j++){
        if((monsTbl[i] == monsTbl[j]) && (monsTbl[i] != _POKENO_NONE)){
          *FailedBit |= POKEFAILEDBIT_BOTHPOKE;
          ret = POKE_REG_BOTH_POKE;
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
          *FailedBit |= POKEFAILEDBIT_BOTHITEM;
          ret = POKE_REG_BOTH_ITEM;
        }
      }
    }
  }
  ans = Regulation_GetParam(pReg, REGULATION_MUST_POKE);
  form = Regulation_GetParam(pReg, REGULATION_MUST_POKE_FORM);
  i = 0;
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
    *FailedBit |= POKEFAILEDBIT_MAST_POKE;
    ret = POKE_REG_NO_MASTPOKE;
  }

  if( ret == POKE_REG_OK )
  {
    //������`�F�b�N
    //�K�{�|�P�����܂߂����v���x���Ȃǂ̃`�F�b�N
    const BOOL checkRet = PokeRegulationCheckPokeParty_Func( pReg,party,NULL );
    if( checkRet == FALSE )
    {
      ret = POKE_REG_TOTAL_LV_FAILED;
    }
  }
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���x���Ƀ��M�����[�V�����ɂ��������x���␳��������
 *
 *	@param	const REGULATION* pReg  ���M�����[�V����
 *	@param	int               lv    ���x��
 *	@return	int               �␳�ヌ�x��
 */
//-----------------------------------------------------------------------------
static const u32 PokeRegulation_ModifyLevelCalc( const REGULATION* pReg, const int level )
{
  const u32 modfity_level = Regulation_GetParam(pReg, REGULATION_LEVEL);
  //���M�����[�V�������ƂɃ��x���␳��������|�P�������Ⴄ
  switch( Regulation_GetParam(pReg, REGULATION_LEVEL_RANGE) )
  { 
  case REGULATION_LEVEL_RANGE_DRAG_DOWN:  
    //�ȏ�␳�Ȃ̂ŁA�␳���x���ȏ�Ȃ�Ε␳���x���ɂ���
    if( level > modfity_level )
    {
      return modfity_level;
    }
    break;

  case REGULATION_LEVEL_RANGE_SAME:
    //�S�␳�Ȃ̂ŁA�S���s��
    return modfity_level;
    break;

  case REGULATION_LEVEL_RANGE_PULL_UP:
    //�ȉ��␳�Ȃ̂ŕ␳���x���ȉ��Ȃ�Ε␳���x���ɂ���
    if( level < modfity_level )
    {
      return modfity_level;
    }
    break;

  default:
    //����ȊO�͍s��Ȃ�
    break;
  }
  return level;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�p�[�e�B�����M�����[�V�����ɂ��������x���␳��������
 *
 *	@param	const REGULATION* pReg  ���M�����[�V����
 *	@param	*party                  �p�[�e�B
 */
//-----------------------------------------------------------------------------
void PokeRegulation_ModifyLevelPokeParty( const REGULATION* pReg, POKEPARTY *party )
{ 
  int i;
  POKEMON_PARAM *pp;
  const u32 modfity_level = Regulation_GetParam(pReg, REGULATION_LEVEL);
  BOOL is_modify;

  for( i = 0; i < PokeParty_GetPokeCount( party ); i++ )
  {
    pp  = PokeParty_GetMemberPointer( party, i );

    if( PP_Get( pp, ID_PARA_poke_exist, NULL ) )
    { 
      const u32 level = PP_Get( pp , ID_PARA_level , NULL );
      const u32 modLv = PokeRegulation_ModifyLevelCalc( pReg , level );

      //�␳��������
      if( level != modLv )
      { 
        POKETOOL_MakeLevelRevise(pp, modfity_level);
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����p�[�e�B�փj�b�N�l�[���t���O�ɉ��������O�C�����s��
 *
 *	@param	const REGULATION* pReg  ���M�����[�V����
 *	@param	*party                  �|�P�����p�[�e�B
 */
//-----------------------------------------------------------------------------
void PokeRegulation_ModifyNickName( const REGULATION* pReg, POKEPARTY *party, HEAPID heapID )
{ 
  if( Regulation_GetParam( pReg , REGULATION_NICKNAME ) == 0 )
  { 
    int i;
    for( i = 0; i < PokeParty_GetPokeCount( party );i ++ )
    { 
      POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( party, i );
      if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
      { 
        PP_SetDefaultNickName( p_pp );
      }
    }
  }
}
//------------------------------------------------------------------
/**
 * @brief   ROM���烌�M�����[�V�����f�[�^�𓾂�
 * @param   regulation_data_no   regulation_def.h�̒�`�ŌĂяo���Ă�������
 * @param   heap  �ǂݍ��ݗp�̈�
 * @return  ���M�����[�V�����f�[�^�|�C���^  GFL_HEAP_FreeMemory�Ń������J�������Ă�������
 */
//------------------------------------------------------------------
const REGULATION* PokeRegulation_LoadDataAlloc(int regulation_data_no, HEAPID heapid)
{
  REGULATION* pRegData = GFL_ARC_LoadDataAlloc(ARCID_REGULATION, regulation_data_no, heapid);
  return pRegData;
}

//------------------------------------------------------------------
/**
 * @brief   ROM���烌�M�����[�V�����f�[�^�𓾂�(�O�����������[�N��n��)
 * @param   regulation_data_no   regulation_def.h�̒�`�ŌĂяo���Ă�������
 * @param   heap  �ǂݍ��ݗp�̈�
 * @return  ���M�����[�V�����f�[�^�|�C���^  GFL_HEAP_FreeMemory�Ń������J�������Ă�������
 */
//------------------------------------------------------------------
void PokeRegulation_LoadData(int regulation_data_no, REGULATION *reg)
{
  GFL_ARC_LoadData(reg, ARCID_REGULATION, regulation_data_no);
}


//==============================================================================
//
//  ���b�Z�[�W�擾
//
//==============================================================================
//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F�|�P�����̐�
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateNum(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  
  if(pReg->NUM_LO == 1 && pReg->NUM_HI == TEMOTI_POKEMAX){
    msg_id = msg_reg_num_000; //��������
  }
  else if(pReg->NUM_LO == pReg->NUM_HI){
    msg_id = msg_reg_num_001; //�l���Œ�
  }
  else if(pReg->NUM_LO > 1 && pReg->NUM_HI == TEMOTI_POKEMAX){
    msg_id = msg_reg_num_002; //xxx�C�ȏ�
  }
  else{
    msg_id = msg_reg_num_003; // xxx �` xxx �C
  }
  
  WORDSET_RegisterNumber( wordset, 0, pReg->NUM_LO, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( wordset, 1, pReg->NUM_HI, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

  strbuf = GFL_STR_CreateBuffer(64, heap_id);
  GFL_MSG_GetString( rpm->msgdata, msg_id, tempbuf );

  WORDSET_ExpandStr( wordset, strbuf, tempbuf );
  return strbuf;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F�|�P�����̃��x��
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateLv(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  
  if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_NORMAL){
    msg_id = msg_reg_num_000; //�����Ȃ�
  }
  else if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_SAME){
    msg_id = msg_reg_lv_001;  //�S�␳
  }
  else if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_OVER){
    msg_id = msg_reg_lv_003;  //�ȏ�
  }
  else if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_LESS){
    msg_id = msg_reg_lv_002;  //�ȉ�
  }
  else if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_DRAG_DOWN){
    msg_id = msg_reg_lv_004;  //��������
  }
  else if(pReg->LEVEL_RANGE == REGULATION_LEVEL_RANGE_PULL_UP){
    msg_id = msg_reg_lv_005;  //�����グ
  }
  else{
    GF_ASSERT(0);
    msg_id = msg_reg_num_000; //�����Ȃ�
  }
  WORDSET_RegisterNumber( wordset, 0, pReg->LEVEL, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

  strbuf = GFL_STR_CreateBuffer(64, heap_id);
  GFL_MSG_GetString( rpm->msgdata, msg_id, tempbuf );

  WORDSET_ExpandStr( wordset, strbuf, tempbuf );
  return strbuf;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F���ʂȃ|�P����
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateSpPoke(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  int i;
  
  for(i = 0; i < REG_POKENUM_MAX_BYTE; i++){
    if(pReg->VETO_POKE_BIT[i] != 0){
      break;
    }
  }
  if(i == REG_POKENUM_MAX_BYTE){
    msg_id = msg_reg_sp_000;
  }
  else{
    msg_id = msg_reg_sp_001;
  }
  
  strbuf = GFL_MSG_CreateString( rpm->msgdata, msg_id );
  return strbuf;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F�����|�P����
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateSamePoke(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  
  if(pReg->BOTH_POKE == TRUE){
    msg_id = msg_reg_samepoke_000;
  }
  else{
    msg_id = msg_reg_samepoke_001;
  }

  strbuf = GFL_MSG_CreateString( rpm->msgdata, msg_id );
  return strbuf;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F��������
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateSameItem(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  
  if(pReg->BOTH_ITEM == TRUE){
    msg_id = msg_reg_sameitem_000;
  }
  else{
    msg_id = msg_reg_sameitem_001;
  }

  strbuf = GFL_MSG_CreateString( rpm->msgdata, msg_id );
  return strbuf;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬�F�V���[�^�[�̗L��
 */
//--------------------------------------------------------------
static STRBUF * _RegMsg_CreateShooter(const REGULATION* pReg, WORDSET *wordset, REGULATION_PRINT_MSG *rpm, STRBUF *tempbuf, HEAPID heap_id)
{
  STRBUF *strbuf;
  u16 msg_id;
  
  if(rpm->shooter_type == REGULATION_SHOOTER_VALID){
    msg_id = msg_reg_shooter_000;
  }
  else if(rpm->shooter_type == REGULATION_SHOOTER_INVALID){
    msg_id = msg_reg_shooter_001;
  }
  else{
    GF_ASSERT(0);
    msg_id = msg_reg_shooter_000;
  }

  strbuf = GFL_MSG_CreateString( rpm->msgdata, msg_id );
  return strbuf;
}

//==================================================================
/**
 * ���M�����[�V�����ꗗ���j���[�\���p���b�Z�[�W�̃p�b�P�[�W���쐬����
 *
 * @param   pReg		
 * @param   wordset		
 * @param   heap_id		
 * @param   shooter_manual		REGULATION_SHOOTER_TYPE(���M�����[�V������MANUAL�ȊO�̎��͖������܂�)
 *
 * @retval  REGULATION_PRINT_MSG *		
 */
//==================================================================
REGULATION_PRINT_MSG * PokeRegulation_CreatePrintMsg(const REGULATION* pReg, WORDSET *wordset, HEAPID heap_id, int shooter_type)
{
  REGULATION_PRINT_MSG *rpm = GFL_HEAP_AllocClearMemory(heap_id, sizeof(REGULATION_PRINT_MSG));
  STRBUF *tempbuf;
  int category;
  static STRBUF * (* const PrerequisiteFunc[])(const REGULATION *, WORDSET *, REGULATION_PRINT_MSG *, STRBUF *, HEAPID) = {
    _RegMsg_CreateNum,
    _RegMsg_CreateLv,
    _RegMsg_CreateSpPoke,
    _RegMsg_CreateSamePoke,
    _RegMsg_CreateSameItem,
    _RegMsg_CreateShooter,
  };
  SDK_COMPILER_ASSERT(NELEMS(PrerequisiteFunc) == REGULATION_CATEGORY_MAX);

  rpm->msgdata = GFL_MSG_Create( 
    GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_regulation_dat, heap_id );
  
  tempbuf = GFL_STR_CreateBuffer(64, heap_id);
  
  if(pReg->SHOOTER == REGULATION_SHOOTER_MANUAL){
    rpm->shooter_type = shooter_type;
  }
  else{
    rpm->shooter_type = pReg->SHOOTER;
  }
  
  for(category = 0; category < REGULATION_CATEGORY_MAX; category++){
    rpm->category[category] = GFL_MSG_CreateString( rpm->msgdata, msg_reg_000 + category );
    rpm->prerequisite[category] = PrerequisiteFunc[category](pReg, wordset, rpm, tempbuf, heap_id);
  }
  
  GFL_STR_DeleteBuffer(tempbuf);
  
  return rpm;
}

//==================================================================
/**
 * ���M�����[�V�����ꗗ���j���[�\���p���b�Z�[�W�̃p�b�P�[�W���폜����
 *
 * @param   rpm		
 */
//==================================================================
void PokeRegulation_DeletePrintMsg(REGULATION_PRINT_MSG *rpm)
{
  int category;
  
  for(category = 0; category < REGULATION_CATEGORY_MAX; category++){
    GFL_STR_DeleteBuffer(rpm->category[category]);
    GFL_STR_DeleteBuffer(rpm->prerequisite[category]);
  }
  GFL_MSG_Delete(rpm->msgdata);
  GFL_HEAP_FreeMemory(rpm);
}

//==================================================================
/**
 * ���M�����[�V�����p�[�e�B�[�쐬�`�F�b�N
 *
 * @param   reg   ���M�����[�V����
 * @param   party �p�[�e�B�[
 * @param   arr   ���ʊi�[(NULL��)
 *
 * @return  BOOL  �p�[�e�B�쐬�ۃt���O
 */
//==================================================================
static const BOOL PokeRegCheckParty_CalcIdx( u8 *arr );

const BOOL PokeRegulationCheckPokeParty_Func( const REGULATION* reg , POKEPARTY *party , u8 *arr )
{
  //���v���x���ƕK�{�|�P���l�������`�F�b�N
  u8 checkWork[6]={1,2,3,4,5,5};  //���[�v�̏����l(�Ō�̒l�͊֐��̒��ő�����̂�5

  while( PokeRegCheckParty_CalcIdx( checkWork ) )
  {
    if( PokeRegulationCheckPokeParty_Func2( reg , party , checkWork , arr , NULL ) == TRUE )
    {
      return TRUE;
    }
  }
  return FALSE;
}

static const int PokeRegulationCheckPokeParty_Func2( const REGULATION* reg , POKEPARTY *party , const u8 *checkArr , u8 *retArr , int *errBit )
{
  u8 i,j;
  u8 okNum = 0;
  BOOL isOk = TRUE;
  u32 monsNoArr[6] = {0,0,0,0,0,0};
  u32 formNoArr[6] = {0,0,0,0,0,0};
  u32 itemNoArr[6]   = {0,0,0,0,0,0};
  u16 sumLv = 0;
  const int limitLv = Regulation_GetParam(reg, REGULATION_LEVEL_TOTAL);
  //OS_TFPrintf( 3 , "%d,%d,%d,%d,%d,%d\n",checkArr[0],checkArr[1],checkArr[2],checkArr[3],checkArr[4],checkArr[5]);
  if( retArr != NULL )
  {
    for( i=0;i<6;i++ )
    {
      retArr[okNum] = 0;
    }
  }
  if( errBit != NULL )
  {
    *errBit = 0;
  }
  for( i=0;i<6;i++ )
  {
    if( checkArr[i] != 0 )
    {
      const u8 idx = checkArr[i] - 1;
      if( idx < PokeParty_GetPokeCount( party ) )
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party,idx);
        const u32 monsNo = PP_Get( pp, ID_PARA_monsno, NULL );
        const u32 itemNo = PP_Get( pp, ID_PARA_item, NULL );
        const u32 lv = PokeRegulation_ModifyLevelCalc( reg , PP_Get( pp, ID_PARA_level, NULL ) );
        //�̃`�F�b�N
        const int pokeRet = PokeRegulationCheckPokePara(reg, pp);
        if( pokeRet != POKEFAILEDBIT_NONE)
        {
          if( errBit != NULL )
          {
            *errBit |= pokeRet;
          }
          continue;
        }
        //�����|�P�E����`�F�b�N
        isOk = TRUE;
        for( j=0;j<okNum;j++ )
        {
          if( Regulation_GetParam(reg, REGULATION_BOTH_POKE) == FALSE )
          {
            //�|�P���������
            if( monsNoArr[j] == monsNo )
            {
              isOk = FALSE;
              if( errBit != NULL )
              {
                *errBit |= POKEFAILEDBIT_BOTHPOKE;
              }
              break;
            }
          }
          if( Regulation_GetParam(reg, REGULATION_BOTH_ITEM) == FALSE )
          {
            if( itemNo != 0 &&
                itemNoArr[j] == itemNo )
            {
              isOk = FALSE;
              //�A�C�e�������
              if( errBit != NULL )
              {
                *errBit |= POKEFAILEDBIT_BOTHITEM;
              }
              break;
            }
          }
        }
        if( isOk == FALSE )
        {
          continue;
        }
        //���v���x���`�F�b�N
        if( limitLv != 0 &&
            sumLv + lv > limitLv )
        {
          if( errBit != NULL )
          {
            *errBit |= POKEFAILEDBIT_SUM_LEVEL;
          }
          continue;
        }
        
        //��OK�I
        if( retArr != NULL )
        {
          retArr[okNum] = idx+1;
        }
        monsNoArr[okNum] = monsNo;
        formNoArr[okNum] = PP_Get( pp, ID_PARA_form_no, NULL );
        itemNoArr[okNum] = itemNo;
        sumLv += lv;
        okNum++;
        if( okNum >= Regulation_GetParam(reg, REGULATION_NUM_HI ) )
        {
          //�K��l�����B�I
          if( errBit != NULL )
          {
            if( okNum < 6 &&
                checkArr[okNum] != 0 )
            {
              //�܂��v�遨�ꉞ�l���I�[�o�[
              *errBit |= POKEFAILEDBIT_NUM;
            }
          }
          break;
        }
      }
    }
  }
  
  //�ŏI�`�F�b�N
  if( okNum >= Regulation_GetParam(reg, REGULATION_NUM_LO ) )
  {
    //�K�{�`�F�b�N
    const int mustPoke = Regulation_GetParam(reg, REGULATION_MUST_POKE);
    isOk = TRUE;
    if( mustPoke != 0 )
    {
      u8 j;
      const int mustForm = Regulation_GetParam(reg, REGULATION_MUST_POKE_FORM);
      isOk = FALSE;
      for( j=0;j<okNum;j++ )
      {
        if( mustPoke == monsNoArr[j] &&
            mustForm == formNoArr[j] )
        {
          isOk = TRUE;
          break;
        }
      }
    }
    if( isOk == TRUE )
    {
      return TRUE;
    }
    else
    {
      //�K�{�|�P���Ȃ�
      if( errBit != NULL )
      {
        *errBit |= POKEFAILEDBIT_MAST_POKE;
      }
    }
  }
  else
  {
    //�l���s��
    if( errBit != NULL )
    {
      *errBit |= POKEFAILEDBIT_NUM;
    }
  }
  
  return FALSE;
}


static const BOOL PokeRegCheckParty_CalcIdx( u8 *arr )
{
  //for�̏����𔲂��Ă���̂ł����ő��₷
  arr[5]++;
  for( /*none*/ ; arr[0]<=6 ; arr[0]++ )
  {
    for( /*none*/ ; arr[1]<=6 ; arr[1]++ )
    {
      for( /*none*/ ; arr[2]<=6 ; arr[2]++ )
      {
        for( /*none*/ ; arr[3]<=6 ; arr[3]++ )
        {
          for( /*none*/ ; arr[4]<=6 ; arr[4]++ )
          {
            for( /*none*/ ; arr[5]<=6 ; arr[5]++ )
            {
              if( arr[0] != arr[1] && arr[0] != arr[2] && arr[0] != arr[3] &&
                  arr[0] != arr[4] && arr[0] != arr[5] && arr[1] != arr[2] &&
                  arr[1] != arr[3] && arr[1] != arr[4] && arr[1] != arr[5] &&
                  arr[2] != arr[3] && arr[2] != arr[4] && arr[2] != arr[5] &&
                  arr[3] != arr[4] && arr[3] != arr[5] && arr[4] != arr[5] )
              {
                return TRUE;
              }
            }
            arr[5] = 1;
          }
          arr[4] = 1;
        }
        arr[3] = 1;
      }
      arr[2] = 1;
    }
    arr[1] = 1;
  }
  return FALSE;
}
