//============================================================================================
/**
 * @file	regulation.c
 * @brief	�o�g�����M�����[�V�����f�[�^�A�N�Z�X�p�\�[�X
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================

#include <gflib.h>

#include "pm_define.h"
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
//-------------------------------------
///	  �f�W�^���I��،����ڏ��
//=====================================
struct _REGULATION_VIEWDATA
{
  u16 mons_no[ TEMOTI_POKEMAX ];
  u8  form_no[ TEMOTI_POKEMAX ];
  u8  item_flag;  //6�̕��̃r�b�g
  u8  sex_flag;  //6�Ε��̃r�b�g  �|�P�����A�C�R���p�̐��ʂȂ̂Ł����P���̑����O
  u8  dummy[ 10 ];
  /* ���L�̐ߖ�
   *
  u8  item_flag[ TEMOTI_POKEMAX ];
  u8  sex[ TEMOTI_POKEMAX ];*/
} ;

//-------------------------------------
///	  �Z�[�u�f�[�^�{��
//=====================================
struct _REGULATION_SAVEDATA
{ 
  REGULATION_CARDDATA card_data[ REGULATION_CARD_TYPE_MAX ];
  REGULATION_VIEWDATA view_data[ REGULATION_CARD_TYPE_MAX ];
};


//============================================================================================
//
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V�����Z�[�u�f�[�^�̃T�C�Y
 *
 *	@return �T�C�Y
 */
//-----------------------------------------------------------------------------
int RegulationSaveData_GetWorkSize(void)
{ 
  return sizeof(REGULATION_SAVEDATA);
}
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
void Regulation_Init(REGULATION * my)
{
  GFL_STD_MemClear(my, sizeof(REGULATION));
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

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V�������J�[�h�f�[�^����擾
 *
 *	@param	REGULATION_CARDDATA *pRegData �J�[�h�f�[�^
 *
 *	@return ���M�����[�V����
 */
//-----------------------------------------------------------------------------
REGULATION* RegulationData_GetRegulation(REGULATION_CARDDATA *pRegData)
{ 
  return &pRegData->regulation_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�h�f�[�^�փ��M�����[�V������ݒ�
 *
 *	@param	*pRegData           �J�[�h�f�[�^
 *	@param	REGULATION* pReg    ���M�����[�V����
 */
//-----------------------------------------------------------------------------
void RegulationData_SetRegulation(REGULATION_CARDDATA *pRegData, const REGULATION* pReg)
{ 

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
  case REGULATION_CAMERA:  //    #�J�������[�h
    ret = pReg->CAMERA;
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
  case REGULATION_BTLCOUNT:  ///<  �퓬�K���
    ret = pReg->BTL_COUNT;
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
  case REGULATION_CAMERA:  //    #�J�������[�h
    pReg->CAMERA  = param;
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
  case REGULATION_BTLCOUNT:  ///<  �퓬�K���
    pReg->BTL_COUNT = param;
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
  case REGULATION_VETO_SHOOTER_ITEM:  ///<  �V���[�^�[�֎~����
    if(pReg->VETO_SHOOTER_ITEM[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V��������V���[�^�[�ݒ�
 *
 *	@param	REGULATION* cp_reg          ���M�����[�V����
 *	@param	SHOOTER_ITEM_BIT_WORK *p_wk �V���[�^�[���[�N
 */
//-----------------------------------------------------------------------------
void Regulation_GetShooterItem( const REGULATION* cp_reg, SHOOTER_ITEM_BIT_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(SHOOTER_ITEM_BIT_WORK) );
  p_wk->shooter_use = Regulation_GetParam( cp_reg , REGULATION_SHOOTER );
  GFL_STD_MemCopy( cp_reg->VETO_SHOOTER_ITEM, p_wk->bit_tbl, SHOOTER_ITEM_BIT_TBL_MAX );
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

//----------------------------------------------------------------------------
/**
 *	@brief  �J�b�v���擾
 *
 *	@param	const REGULATION_CARDDATA* pReg   ���[�N�ւ̃|�C���^
 *
 *	@return �J�b�v����������STRCODE�̃|�C���^
 */
//-----------------------------------------------------------------------------
const STRCODE *Regulation_GetCardCupNamePointer( const REGULATION_CARDDATA* pReg )
{ 
  return pReg->cupname;
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
  case REGULATION_CARD_BGM:         ///< ���Ŏg�p����ȁi�V�[�P���X�ԍ������̂܂ܓ����Ă���킯�ł͂Ȃ�
    ret = pReg->bgm_no;
    break;

  case REGULATION_CARD_SAMEMATCH:         ///< �����l�Ƃ̑ΐ�������邩�ǂ����i���C�u���ł����g��Ȃ����ΐ킵���l��macaddres��ۑ����Ă��Ȃ�����j
    ret = pReg->same_match;
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
  case REGULATION_CARD_BGM:         ///< ���Ŏg�p����ȁi�V�[�P���X�ԍ������̂܂ܓ����Ă���킯�ł͂Ȃ�
    pReg->bgm_no  = param;
    break;

  case REGULATION_CARD_SAMEMATCH:   ///< �����l�Ƃ̑ΐ�������邩�ǂ���
    pReg->same_match  = param;
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V�����J�[�h��CRC�`�F�b�N (�J�Ã��[�N status�ɒl��������Ă���ƃ`�F�b�N�ł��܂���)
 *
 *	@param	const REGULATION* pReg  �`�F�b�N�������
 *
 *	@return TRUE�Ő���  FALSE�ŕs��
 */
//-----------------------------------------------------------------------------
BOOL Regulation_CheckCrc( const REGULATION_CARDDATA* pReg )
{ 
  OS_TPrintf( "CRC�`�F�b�N buffer[0x%x]== calc[0x%x]\n", pReg->crc, GFL_STD_CrcCalc( pReg, sizeof(REGULATION_CARDDATA) - 2 ));
  return pReg->crc == GFL_STD_CrcCalc( pReg, sizeof(REGULATION_CARDDATA) - 2 );
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�ւ̃|�C���^�擾
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param	���{�ڂ̃��M�����[�V�����f�[�^��
 * @return	REGULATION	���[�N�ւ̃|�C���^  �����f�[�^�̏ꍇNULL
 */
//----------------------------------------------------------
REGULATION_CARDDATA* RegulationSaveData_GetRegulationCard( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type )
{
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  return &p_save->card_data[ type ];
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�̕ۑ�
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param	const REGULATION  ���M�����[�V�����f�[�^
 * @return	none
 */
//----------------------------------------------------------
void RegulationSaveData_SetRegulation(REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type, const REGULATION_CARDDATA* pReg)
{
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  p_save->card_data[ type ] = *pReg;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	* my  ���[�N
 */
//-----------------------------------------------------------------------------
void RegulationView_Init(REGULATION_VIEWDATA * my)
{ 
  GFL_STD_MemClear( my, sizeof(REGULATION_VIEWDATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V���������ڃf�[�^�擾
 *
 *	@param	REGULATION_SAVEDATA *p_save �Z�[�u�f�[�^
 *	@param	REGULATION_CARD_TYPE type   �^�C�v
 *
 *	@return �����ڃf�[�^
 */
//-----------------------------------------------------------------------------
REGULATION_VIEWDATA* RegulationSaveData_GetRegulationView( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type )
{ 
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  return &p_save->view_data[ type ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�擾
 *
 *	@param	const REGULATION_VIEWDATA *pView  ���[�N
 *	@param	type                              �擾�^�C�v
 *	@param	idx                               �C���f�b�N�X�i�U�܂Łj
 *
 *	@return �f�[�^
 */
//-----------------------------------------------------------------------------
int RegulationView_GetParam( const REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx )
{ 
  GF_ASSERT( idx < TEMOTI_POKEMAX );

  switch( type )
  { 
  case REGULATION_VIEW_MONS_NO:
    return pView->mons_no[ idx ];

  case REGULATION_VIEW_FROM_NO:
    return pView->form_no[ idx ];

  case REGULATION_VIEW_ITEM_FLAG:
    return (pView->item_flag >> idx) & 0x1;

  case REGULATION_VIEW_SEX:
    return (pView->sex_flag >> idx) & 0x1;

  default:
    GF_ASSERT(0);
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�ݒ�
 *
 *	@param	REGULATION_VIEWDATA *pView  ���[�N
 *	@param	type                        �擾�^�C�v
 *	@param	idx                         �C���f�b�N�X
 *	@param	param                       �ݒ肷��f�[�^
 */
//-----------------------------------------------------------------------------
void RegulationView_SetParam( REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx, int param )
{ 
  GF_ASSERT( idx < TEMOTI_POKEMAX );

  switch( type )
  { 
  case REGULATION_VIEW_MONS_NO:
    pView->mons_no[ idx ]   = param;
    break;

  case REGULATION_VIEW_FROM_NO:
    pView->form_no[ idx ]   = param;
    break;

  case REGULATION_VIEW_ITEM_FLAG:
    pView->item_flag |= ((param == TRUE)<<idx);
    break;

  case REGULATION_VIEW_SEX:
    //�|�P�����A�C�R���p�̐��ʂȂ̂Ł��̂݃I��
    pView->sex_flag |= ((param == PTL_SEX_FEMALE)<<idx);
    break;

  default:
    GF_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ȒP�f�[�^�ݒ�  �i�o�g���{�b�N�X�̃|�P�����p�[�e�B����ݒ肷��j
 *
 *	@param	REGULATION_VIEWDATA *pView  ���[�N
 *	@param	POKEPARTY *cp_party         �|�P�����p�[�e�B
 */
//-----------------------------------------------------------------------------
void RegulationView_SetEazy( REGULATION_VIEWDATA *pView, const POKEPARTY *cp_party )
{ 
  int i;

  RegulationView_Init( pView );

  for( i = 0; i < PokeParty_GetPokeCount( cp_party);i ++ )
  { 
    POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( cp_party, i );
    if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
    { 
      RegulationView_SetParam( pView, REGULATION_VIEW_MONS_NO, i, PP_Get( p_pp, ID_PARA_monsno, NULL ) );
      RegulationView_SetParam( pView, REGULATION_VIEW_FROM_NO, i, PP_Get( p_pp, ID_PARA_form_no, NULL ) );
      RegulationView_SetParam( pView, REGULATION_VIEW_ITEM_FLAG, i, PP_Get( p_pp, ID_PARA_item, NULL ) != 0 );
      RegulationView_SetParam( pView, REGULATION_VIEW_SEX, i, PP_Get( p_pp, ID_PARA_sex, NULL ) );

    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V�����f�[�^�ւ̃|�C���^
 *
 *	@param	* my  �|�C���^
 */
//-----------------------------------------------------------------------------
void RegulationSaveData_Init(REGULATION_SAVEDATA * my)
{ 
  GFL_STD_MemClear(my, sizeof(REGULATION_SAVEDATA));
}

//----------------------------------------------------------
/**
 * @brief	���M�����[�V�����f�[�^�ւ̃|�C���^�擾
 * @param	pSave    	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	REGULATION	���[�N�ւ̃|�C���^  �����f�[�^�̏ꍇNULL
 */
//----------------------------------------------------------
REGULATION_SAVEDATA* SaveData_GetRegulationSaveData(SAVE_CONTROL_WORK* pSave)
{
  return SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
}

#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�p�Ƀf�[�^��ݒ�
 *
 *	@param	REGULATION_CARDDATA* pReg ���[�N
 */
//-----------------------------------------------------------------------------
void Regulation_SetDebugData( REGULATION_CARDDATA* pReg )
{ 
  { 
    REGULATION  *p_data = &pReg->regulation_buff;
    p_data->cupNo   = 1;
    p_data->ruleNo  = 0;
    p_data->NUM_LO  = 1;
    p_data->NUM_HI  = 6;
    p_data->LEVEL   = 1;
    p_data->LEVEL_RANGE = REGULATION_LEVEL_RANGE_NORMAL;
    p_data->LEVEL_TOTAL = 600;
    p_data->BOTH_POKE   = 1;
    p_data->BOTH_ITEM   = 1;
    GFL_STD_MemClear( p_data->VETO_POKE_BIT, REG_POKENUM_MAX_BYTE );
    GFL_STD_MemClear( p_data->VETO_ITEM, REG_ITEMNUM_MAX_BYTE );
    p_data->MUST_POKE   = 0;
    p_data->MUST_POKE_FORM   = 0;
    p_data->SHOOTER     = 0;
    p_data->TIME_VS     = 99;
    p_data->TIME_COMMAND= 99;
    p_data->NICKNAME    = 1;
    p_data->SHOW_POKE   = 0;
    p_data->SHOW_POKE_TIME  = 99;
    p_data->BATTLE_TYPE = REGULATION_BATTLE_SINGLE;
    p_data->BTL_COUNT = 3;
    GFL_STD_MemClear( p_data->VETO_SHOOTER_ITEM,SHOOTER_ITEM_BIT_TBL_MAX );
  }
  GFL_STD_MemClear( pReg->cupname, sizeof(STRCODE)*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );
  pReg->cupname[ 0] = L'�f';
  pReg->cupname[ 1] = L'�o';
  pReg->cupname[ 2] = L'�b';
  pReg->cupname[ 3] = L'�N';
  pReg->cupname[ 4] = L'��';
  pReg->cupname[ 5] = L'��';
  pReg->cupname[ 6] = L'��';
  pReg->cupname[ 7] = L'��';
  pReg->cupname[ 8] = 0xFFFF;
  pReg->ver         = 0xFFFFFFFF;
  pReg->no          = 1;
  pReg->start_year  = 10;
  pReg->start_month = 1;
  pReg->start_day   = 22;
  pReg->end_year    = 10;
  pReg->end_month   = 12;
  pReg->end_day     = 24;
  pReg->status      = 0;
  pReg->bgm_no      = 1;
  pReg->code        = LANG_JAPAN;
  pReg->crc         = GFL_STD_CrcCalc( pReg, sizeof(REGULATION_CARDDATA) - 2 );
}
#include "debug/debug_str_conv.h"
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�v�����g
 *
 *	@param	const REGULATION_CARDDATA* cpReg ���[�N
 */
//-----------------------------------------------------------------------------
void Regulation_PrintDebug( const REGULATION_CARDDATA* cpReg )
{ 
  const REGULATION  *cp_data = &cpReg->regulation_buff;

  OS_TPrintf( "reg size =%d\n", sizeof(REGULATION));
  OS_TPrintf( "card size=%d\n", sizeof(REGULATION_CARDDATA));
  OS_TPrintf( "cupno    =%d\n", cp_data->cupNo);
  OS_TPrintf( "rule_num =%d\n", cp_data->ruleNo); ///< ���[���ԍ�
  OS_TPrintf( "num_lo   =%d\n", cp_data->NUM_LO); ///<    #�Q��������
  OS_TPrintf( "num_hi   =%d\n", cp_data->NUM_HI); ///<    #�Q�������
  OS_TPrintf( "level    =%d\n", cp_data->LEVEL);  ///<    #�Q�����x��
  OS_TPrintf( "lv_range =%d\n", cp_data->LEVEL_RANGE); ///<    #���x���͈�
  OS_TPrintf( "lv_total =%d\n", cp_data->LEVEL_TOTAL); ///<    #���x�����v
  OS_TPrintf( "both_poke=%d\n", cp_data->BOTH_POKE); ///<    #�����|�P����
  OS_TPrintf( "both_item=%d\n", cp_data->BOTH_ITEM);  ///<    #�����ǂ���             60
  OS_TPrintf( "veto_poke=0x%x\n", cp_data->VETO_POKE_BIT[REG_POKENUM_MAX_BYTE]);  ///<    #�Q���֎~�|�P���� 142
  OS_TPrintf( "veto_item=0x%d\n", cp_data->VETO_ITEM[REG_ITEMNUM_MAX_BYTE]); ///<    #�������݋֎~���� 218
  OS_TPrintf( "must_poke=%d\n", cp_data->MUST_POKE);   ///<    #�K�{�|�P����
  OS_TPrintf( "must_form=%d\n", cp_data->MUST_POKE_FORM);   ///<    #�K�{�|�P�����t�H����
  OS_TPrintf( "shooter  =%d\n", cp_data->SHOOTER);    ///<    #�V���[�^�[
  OS_TPrintf( "time_vs  =%d\n", cp_data->TIME_VS);     ///<    #�ΐ펞��
  OS_TPrintf( "time_cmd =%d\n", cp_data->TIME_COMMAND); ///<    #���͎���
  OS_TPrintf( "nick_view=%d\n", cp_data->NICKNAME); ///<    #�j�b�N�l�[���\��
  OS_TPrintf( "camera   =%d\n", cp_data->CAMERA);  ///<     #�J�������[�h
  OS_TPrintf( "show_poke=%d\n", cp_data->SHOW_POKE);  ///<    #�|�P������������
  OS_TPrintf( "show_time=%d\n", cp_data->SHOW_POKE_TIME);  ///<�|�P����������������
  OS_TPrintf( "btl_type =%d\n", cp_data->BATTLE_TYPE);    ///< �o�g���^�C�v  230

  { 
    char str[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE];
    DEB_STR_CONV_StrcodeToSjis( cpReg->cupname, str, WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE ); 
    OS_TPrintf( "cupname =[%s]\n", str );
  }
  OS_TPrintf( "rom_ver  =%d\n", cpReg->ver);//�J�Z�b�g�o�[�W�����F
  OS_TPrintf( "cup_no   =%d\n", cpReg->no);//���No.
  OS_TPrintf( "countory =%d\n", cpReg->code);//���R�[�h�F
  OS_TPrintf( "start y  =%d\n", cpReg->start_year);//�J�n�N�F00-99
  OS_TPrintf( "start m  =%d\n", cpReg->start_month);//�J�n���F01-12
  OS_TPrintf( "start d  =%d\n", cpReg->start_day);//�J�n���F01-31
  OS_TPrintf( "end   y  =%d\n", cpReg->end_year);//�I���N�F00-99
  OS_TPrintf( "end   m  =%d\n", cpReg->end_month);//�I�����F01-12
  OS_TPrintf( "endy  d  =%d\n", cpReg->end_day);//�I�����F01-31
  OS_TPrintf( "status   =%d\n", cpReg->status);  //����ԁF net/dreamworld_netdata.h��DREAM_WORLD_MATCHUP_TYPE�̒l
  OS_TPrintf( "crc      =%d\n", cpReg->crc);  //����������
  
}
#endif //PM_DEBUG
