//============================================================================================
/**
 * @file	regulation.c
 * @brief	�o�g�����M�����[�V�����f�[�^�A�N�Z�X�p�\�[�X
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================

#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "message.naix"
#include "savedata/save_tbl.h"	//SAVEDATA�Q�Ƃ̂���
#include "buflen.h"
#include "savedata/save_control.h"
#include "savedata/regulation.h"
#include "poke_tool/monsno_def.h"

#include "msg/msg_regulation_cup.h"
#include "msg/msg_regulation_rule.h"

#define HEAPID_SAVE_TEMP        (GFL_HEAPID_APP)


//============================================================================================
//============================================================================================

//============================================================================================
//
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	���[�N�T�C�Y�擾
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int Regulation_GetWorkSize(void)
{
  return sizeof(REGULATION);
}

//----------------------------------------------------------
/**
 * @brief	���[�N�T�C�Y�擾
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int RegulationData_GetWorkSize(void)
{
  return sizeof(REGULATION_CARDDATA);
}

//----------------------------------------------------------
/**
 * @brief	�o�g�����M�����[�V�������[�N�̊m��
 * @param	heapID		�������m�ۂ������Ȃ��q�[�v�w��
 * @return	REGULATION*	�擾�������[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
REGULATION* Regulation_AllocWork(HEAPID heapID)
{
  REGULATION* reg;
  reg = GFL_HEAP_AllocClearMemory(heapID, sizeof(REGULATION));
  return reg;
}

//----------------------------------------------------------
/**
 * @brief	REGULATION�̃R�s�[
 * @param	from	�R�s�[��REGULATION�ւ̃|�C���^
 * @param	to		�R�s�[��REGULATION�ւ̃|�C���^
 */
//----------------------------------------------------------
void Regulation_Copy(const REGULATION *pFrom, REGULATION* pTo)
{
  GFL_STD_MemCopy(pFrom, pTo, sizeof(REGULATION));
}

//----------------------------------------------------------
/**
 * @brief	REGULATION�̔�r
 * @param	cmp1   ��r����REGULATION�ւ̃|�C���^
 * @param	cmp2   ��r�����REGULATION�ւ̃|�C���^
 * @return  ��v���Ă�����TRUE
 */
//----------------------------------------------------------
int Regulation_Cmp(const REGULATION* pCmp1,const REGULATION* pCmp2)
{
  if(0 == GFL_STD_MemComp(pCmp1,pCmp2,sizeof(REGULATION)))
  {
    return TRUE;
  }
  return FALSE;
}

//============================================================================================
//
//	REGULATION����̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�o�g�����M�����[�V�����f�[�^�̏�����
 * @param	pReg		REGULATION���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void Regulation_Init(REGULATION* pReg)
{
  GFL_STD_MemClear(pReg, sizeof(REGULATION));
}

//----------------------------------------------------------
/**
 * @brief	�f�W�^���I��؃��M�����[�V�����f�[�^�̏�����
 * @param	pReg		REGULATION���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void RegulationData_Init(REGULATION_CARDDATA* pRegData)
{
  GFL_STD_MemClear(pRegData, sizeof(REGULATION_CARDDATA));
}

//----------------------------------------------------------
/**
 * @brief	�J�b�v���Z�b�g
 * @param	pReg	REGULATION���[�N�|�C���^
 * @param	pCupBuf	�J�b�v�����������o�b�t�@
 */
//----------------------------------------------------------
//void Regulation_SetCupName(REGULATION* pReg, const STRBUF *pCupBuf)
//{
//  GFL_STR_GetStringCode(pCupBuf, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
//}

//----------------------------------------------------------
/**
 * @brief	�J�b�v���擾
 * @param	pReg		    REGULATION���[�N�|�C���^
 * @param	pReturnCupName	�J�b�v��������STRBUF�|�C���^
 * @return	none
 */
//----------------------------------------------------------
void Regulation_GetCupName(const REGULATION* pReg,STRBUF* pReturnCupName)
{
 // GFL_STR_SetStringCodeOrderLength(pReturnCupName, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
  GFL_MSGDATA* pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                          NARC_message_regulation_cup_dat, HEAPID_SAVE_TEMP );
  GFL_MSG_GetString( pMsgData, pReg->cupNo, pReturnCupName );
  GFL_MSG_Delete(pMsgData);
}

//----------------------------------------------------------
/**
 * @brief	�J�b�v���O�擾�iSTRBUF�𐶐��j
 * @param	pReg	���[�N�ւ̃|�C���^
 * @param	heapID	STRBUF�𐶐�����q�[�v��ID
 * @return	STRBUF	���O���i�[����STRBUF�ւ̃|�C���^
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateCupName(const REGULATION* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_CUPNAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  Regulation_GetCupName(pReg,tmpBuf);
//  GFL_STR_SetStringCode( tmpBuf, pReg->cupName );
  return tmpBuf;

}

//----------------------------------------------------------
/**
 * @brief	���[�����Z�b�g
 * @param	pReg	REGULATION���[�N�|�C���^
 * @param	pCupBuf	�J�b�v�����������o�b�t�@
 */
//----------------------------------------------------------
//void Regulation_SetRuleName(REGULATION* pReg, const STRBUF *pRuleBuf)
//{
//  GFL_STR_GetStringCode(pRuleBuf, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));
//}

//----------------------------------------------------------
/**
 * @brief	���[�����擾
 * @param	pReg		    REGULATION���[�N�|�C���^
 * @param	pReturnCupName	���[����������STRBUF�|�C���^
 * @return	none
 */
//----------------------------------------------------------
void Regulation_GetRuleName(const REGULATION* pReg,STRBUF* pReturnRuleName)
{
//  GFL_STR_SetStringCodeOrderLength(pReturnRuleName, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));

  GFL_MSGDATA* pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                          NARC_message_regulation_cup_dat, HEAPID_SAVE_TEMP );
  GFL_MSG_GetString( pMsgData, pReg->ruleNo, pReturnRuleName );
  GFL_MSG_Delete(pMsgData);

}

//----------------------------------------------------------
/**
 * @brief	���[�����O�擾�iSTRBUF�𐶐��j
 * @param	pReg	���[�N�ւ̃|�C���^
 * @param	heapID	STRBUF�𐶐�����q�[�v��ID
 * @return	STRBUF	���O���i�[����STRBUF�ւ̃|�C���^
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateRuleName(const REGULATION* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_RULENAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  Regulation_GetRuleName(pReg,tmpBuf);

  //  GFL_STR_SetStringCode( tmpBuf, pReg->ruleName );
  return tmpBuf;
}

//----------------------------------------------------------
/**
 * @brief	�p�����[�^�[���擾����
 * @param	pReg	REGULATION���[�N�|�C���^
 * @param	type	REGULATION_PARAM_TYPE enum �̂ǂꂩ
 */
//----------------------------------------------------------
int Regulation_GetParam(const REGULATION* pReg, REGULATION_PARAM_TYPE type)
{
  int ret = 0;

  switch(type){
  case REGULATION_CUPNUM:
    ret = pReg->cupNo;
    break;
  case REGULATION_RULENUM:
    ret = pReg->ruleNo;
    break;
  case REGULATION_NUM_LO: //    #�Q��������
    ret = pReg->NUM_LO;
    break;
  case REGULATION_NUM_HI:         //�Q�������
    ret = pReg->NUM_HI;
    break;
  case REGULATION_LEVEL:  //    #�Q�����x��
    ret = pReg->LEVEL;
    break;
  case REGULATION_LEVEL_RANGE: //    #���x���͈�
    ret = pReg->LEVEL_RANGE;
    break;
  case REGULATION_LEVEL_TOTAL: //    #���x�����v
    ret = pReg->LEVEL_TOTAL;
    break;
  case REGULATION_BOTH_POKE: //    #�����|�P����
    ret = pReg->BOTH_POKE;
    break;
  case REGULATION_BOTH_ITEM:  //    #�����ǂ���
    ret = pReg->BOTH_ITEM;
    break;
  case REGULATION_MUST_POKE:   //    #�K�{�|�P����
    ret = pReg->MUST_POKE;
    break;
  case REGULATION_SHOOTER:    //    #�V���[�^�[
    ret = pReg->SHOOTER;
    break;
  case REGULATION_TIME_VS:     //    #�ΐ펞��
    ret = pReg->TIME_VS;
    break;
  case REGULATION_TIME_COMMAND: //    #���͎���
    ret = pReg->TIME_COMMAND;
    break;
  case REGULATION_NICKNAME: //    #�j�b�N�l�[���\��
    ret = pReg->NICKNAME;
    break;
  case REGULATION_AGE_LO:  //    #�N����ȏ�
    ret = pReg->AGE_LO;
    break;
  case REGULATION_AGE_HI:  //    #�N����ȉ�
    ret = pReg->AGE_HI;
    break;
  case REGULATION_SHOW_POKE:  //    #�|�P������������
    ret = pReg->SHOW_POKE;
    break;
  case REGULATION_SHOW_POKE_TIME:  //    #�|�P��������������������
    ret = pReg->SHOW_POKE_TIME;
    break;
  case REGULATION_BATTLETYPE:  //   �o�g���^�C�v
    ret = pReg->BATTLE_TYPE;
    break;
  }
  return ret;
}

//----------------------------------------------------------
/**
 * @brief	�p�����[�^�[����������
 * @param	pReg	REGULATION���[�N�|�C���^
 * @param	type	REGULATION_PARAM_TYPE enum �̂ǂꂩ
 * @param	param	�������ޒl
 * @return  �������������񂾂�TRUE  �s���Ȓl�̏ꍇFALSE
 */
//----------------------------------------------------------
void Regulation_SetParam(REGULATION* pReg, REGULATION_PARAM_TYPE type, int param)
{

  switch(type){
  case REGULATION_CUPNUM:
    if(REGULATION_CUP_MAX > param){
      pReg->cupNo = param;
    }
    else{
      GF_ASSERT(0);
    }
    break;
  case REGULATION_RULENUM:
    if(REGULATION_RULE_MAX > param){
      pReg->ruleNo = param;
    }
    else{
      GF_ASSERT(0);
    }
    break;
  case REGULATION_NUM_LO: //    #�Q��������
    if(param >= 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_LO = param;
    break;
  case REGULATION_NUM_HI:         //�Q�������
    if(param >= 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_HI = param;
    break;
  case REGULATION_LEVEL:  //    #�Q�����x��
    if(param > 100){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL = param;
    break;
  case REGULATION_LEVEL_RANGE: //    #���x���͈�
    if(param > REGULATION_LEVEL_RANGE_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_RANGE = param;
    break;
  case REGULATION_LEVEL_TOTAL: //    #���x�����v
    if(param > 600){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_TOTAL = param;
    break;
  case REGULATION_BOTH_POKE: //    #�����|�P����
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_POKE = param;
    break;
  case REGULATION_BOTH_ITEM:  //    #�����ǂ���
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_ITEM = param;
    break;
  case REGULATION_MUST_POKE:   //    #�K�{�|�P����
    if(param >= MONSNO_MAX){ 
      GF_ASSERT(0);
      return;
    }
    pReg->MUST_POKE = param;
    break;
  case REGULATION_MUST_POKE_FORM:   //    #�K�{�|�P�����t�H����
    pReg->MUST_POKE_FORM = param;
    break;
  case REGULATION_SHOOTER:    //    #�V���[�^�[
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOOTER = param;
    break;
  case REGULATION_TIME_VS:     //    #�ΐ펞��
    if(param > REGULATION_VS_TIME){
      GF_ASSERT(0);
      return;
    }
    pReg->TIME_VS = param;
    break;
  case REGULATION_TIME_COMMAND: //    #���͎���
    if(param > REGULATION_COMMAND_TIME){
      GF_ASSERT(0);
      return;
    }
    pReg->TIME_COMMAND = param;
    break;
  case REGULATION_NICKNAME: //    #�j�b�N�l�[���\��
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->NICKNAME = param;
    break;
  case REGULATION_AGE_LO:  //    #�N����ȏ�
    pReg->AGE_LO = param;
    break;
  case REGULATION_AGE_HI:  //    #�N����ȉ�
    pReg->AGE_HI = param;
    break;
  case REGULATION_SHOW_POKE:  //    #�|�P������������
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOW_POKE = param;
    break;
  case REGULATION_SHOW_POKE_TIME:  //    #�|�P����������������
    if(param > REGULATION_SHOW_POKE_TIME_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOW_POKE_TIME = param;
    break;
  case REGULATION_BATTLETYPE:  //  �o�g���^�C�v
    if(param >= REGULATION_BATTLE_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->BATTLE_TYPE = param;
    break;
  }
}

//----------------------------------------------------------
/**
 * @brief	�p�����[�^��BIT�������Ă��邩�ǂ���������
 * @param	pReg	REGULATION���[�N�|�C���^
 * @param	type	REGULATION_PARAM_TYPE enum �̂ǂꂩ
 * @param	no	  �|�P�����ԍ���A�C�e���ԍ�
 * @return  BIT��ON��TRUE
 */
//----------------------------------------------------------
BOOL Regulation_CheckParamBit(const REGULATION* pReg, REGULATION_PARAM_TYPE type, int no)
{
  int buffno = no / 8;
  int bitno = no % 8;
  
  switch(type){
  case REGULATION_VETO_POKE_BIT:
    if(pReg->VETO_POKE_BIT[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  case REGULATION_VETO_ITEM:
    if(pReg->VETO_ITEM[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//----------------------------------------------------------
/**
 * @brief	�J�b�v���擾
 * @param	pReg		    REGULATION_CARDDATA���[�N�|�C���^
 * @param	pReturnCupName	�J�b�v��������STRBUF�|�C���^
 * @return	none
 */
//----------------------------------------------------------

void Regulation_GetCardCupName(const REGULATION_CARDDATA* pReg,STRBUF* pReturnCupName)
{
  GFL_STR_SetStringCodeOrderLength(pReturnCupName, pReg->cupname, (WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE));
}

//----------------------------------------------------------
/**
 * @brief	�J�b�v���O�擾�iSTRBUF�𐶐��j
 * @param	pReg	REGULATION_CARDDATA���[�N�ւ̃|�C���^
 * @param	heapID	STRBUF�𐶐�����q�[�v��ID
 * @return	STRBUF	���O���i�[����STRBUF�ւ̃|�C���^
 */
//----------------------------------------------------------

STRBUF* Regulation_CreateCardCupName(const REGULATION_CARDDATA* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  GFL_STR_SetStringCode( tmpBuf, pReg->cupname );
  return tmpBuf;
}

//----------------------------------------------------------
/**
 * @brief	�f�W�^���I��؃p�����[�^�[�������o��
 * @param	pReg	REGULATION_CARDDATA���[�N�|�C���^
 * @param	type	REGULATION_CARD_PARAM_TYPE enum �̂ǂꂩ
 * @return  �p�����[�^
 */
//----------------------------------------------------------
int Regulation_GetCardParam(const REGULATION_CARDDATA* pReg, REGULATION_CARD_PARAM_TYPE type)
{
  int ret;
  
  switch(type){
  case REGULATION_CARD_ROMVER:
    ret = pReg->ver;
    break;
  case REGULATION_CARD_CUPNO:
    ret = pReg->no;
    break;
  case REGULATION_CARD_LANGCODE:
    ret = pReg->code;
    break;
  case REGULATION_CARD_START_YEAR:  ///< �J�n�N�F00-99
    ret = pReg->start_year;
    break;
  case REGULATION_CARD_START_MONTH: ///< �J�n���F01-12
    ret = pReg->start_month;
    break;
  case REGULATION_CARD_START_DAY:   ///< �J�n���F01-31
    ret = pReg->start_day;
    break;
  case REGULATION_CARD_END_YEAR:    ///< �I���N�F00-99
    ret = pReg->end_year;
    break;
  case REGULATION_CARD_END_MONTH:   ///< �I�����F01-12
    ret = pReg->end_month;
    break;
  case REGULATION_CARD_END_DAY:     ///< �I�����F01-31
    ret = pReg->end_day;
    break;
  case REGULATION_CARD_STATUS:      ///< ����ԁF�O���J�Á^�P�J�Ò��^�Q�I��
    ret = pReg->status;
    break;
  }
  return ret;
}


//----------------------------------------------------------
/**
 * @brief	�p�����[�^�[����������
 * @param	pReg	REGULATION_CARDDATA���[�N�|�C���^
 * @param	type	REGULATION_CARD_PARAM_TYPE enum �̂ǂꂩ
 * @param	param	�������ޒl
 * @return  none
 */
//----------------------------------------------------------
void Regulation_SetCardParam(REGULATION_CARDDATA* pReg, REGULATION_CARD_PARAM_TYPE type, int param)
{
  switch(type){
  case REGULATION_CARD_ROMVER:
    pReg->ver = param;
    break;
  case REGULATION_CARD_CUPNO:
    pReg->no = param;
    break;
  case REGULATION_CARD_LANGCODE:
    pReg->code = param;
    break;
  case REGULATION_CARD_START_YEAR:  ///< �J�n�N�F00-99
    pReg->start_year = param;
    break;
  case REGULATION_CARD_START_MONTH: ///< �J�n���F01-12
    pReg->start_month = param;
    break;
  case REGULATION_CARD_START_DAY:   ///< �J�n���F01-31
    pReg->start_day = param;
    break;
  case REGULATION_CARD_END_YEAR:    ///< �I���N�F00-99
    pReg->end_year = param;
    break;
  case REGULATION_CARD_END_MONTH:   ///< �I�����F01-12
    pReg->end_month = param;
    break;
  case REGULATION_CARD_END_DAY:     ///< �I�����F01-31
    pReg->end_day = param;
    break;
  case REGULATION_CARD_STATUS:      ///< ����ԁF�O���J�Á^�P�J�Ò��^�Q�I��
    pReg->status = param;
    break;
  }
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�ւ̃|�C���^�擾
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param	���{�ڂ̃��M�����[�V�����f�[�^��
 * @return	REGULATION	���[�N�ւ̃|�C���^  �����f�[�^�̏ꍇNULL
 */
//----------------------------------------------------------
REGULATION* SaveData_GetRegulation(SAVE_CONTROL_WORK* pSave, int regNo)
{
  REGULATION_CARDDATA* pRegData = NULL;

  GF_ASSERT(regNo < REGULATION_MAX_NUM);
  if(regNo < REGULATION_MAX_NUM){
    pRegData = SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
    return &pRegData->regulation_buff;
  }
  return NULL;
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�̕ۑ�
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param	const REGULATION  ���M�����[�V�����f�[�^
 * @return	none
 */
//----------------------------------------------------------
void SaveData_SetRegulation(SAVE_CONTROL_WORK* pSave, const REGULATION* pReg, const int regNo)
{
  REGULATION_CARDDATA* pRegData = NULL;

  pRegData = SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
  GF_ASSERT(regNo < REGULATION_MAX_NUM);
  if(regNo < REGULATION_MAX_NUM){
    Regulation_Copy(pReg, &pRegData->regulation_buff);
  }
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�ւ̃|�C���^�擾
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param	���{�ڂ̃��M�����[�V�����f�[�^��
 * @return	REGULATION	���[�N�ւ̃|�C���^  �����f�[�^�̏ꍇNULL
 */
//----------------------------------------------------------
REGULATION_CARDDATA* SaveData_GetRegulationCardData(SAVE_CONTROL_WORK* pSave)
{
  return SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
}

