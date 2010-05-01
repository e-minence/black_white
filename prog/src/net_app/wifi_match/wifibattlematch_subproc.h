//======================================================================
/**
 * @file	irc_battle_subproc.h
 * @brief	�ԊO���o�g�� ���X�g�E�X�e�[�^�X�Ăяo��->WIFI_MATCH�ւ����Ă���PROC��
 * @author	ariizumi -> nagihashi
 * @data	09/01/19
 *
 */
//======================================================================
#pragma once
#include "savedata/regulation.h"
#include "poke_tool/poke_tool.h"
#include "gamesystem/gamedata_def.h"
#include "poke_tool/pokeparty.h"
#include "pm_define.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define WIFIBATTLEMATCH_MEMBER_NUM (2)


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum 
{
  WIFIBATTLEMATCH_SUBPROC_RESULT_SUCCESS,
  WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_NEXT_LOGIN,  //�G���[���m�iWIFI�̂݁j
  WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_RETURN_LIVE, //�G���[���m(LIVE�̂�)
  WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_DISCONNECT_WIFI, //�ؒf���ꂽ�iWIFI�̂݁j
} WIFIBATTLEMATCH_SUBPROC_RESULT;



//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{ 
  REGULATION          *regulation;                  //[in]���M�����[�V����
  POKEPARTY           *p_party;                     //[in]�I�ԃ|�P����
  const STRCODE   *enemyName;                       //[in]����̖��O
  u8        enemySex;                               //[in]����̐���
  u8        pad2[3];                                //�p�f�B���O
  POKEPARTY *enemyPokeParty;                        //[in]����̃|�P����
  GAMEDATA            *gameData;                    //[in]�Q�[���f�[�^
  u8                  dummy[3];
  POKEPARTY           *p_select_party;              //[out]�o�ꂷ��|�P�p�[�e�B(Alloc���Ă�������)
  WIFIBATTLEMATCH_SUBPROC_RESULT  result;           //[out]�I���R�[�h
} WIFIBATTLEMATCH_SUBPROC_PARAM;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	proc data
//--------------------------------------------------------------
extern const GFL_PROC_DATA WifiBattleMatch_Sub_ProcData;
