//======================================================================
/**
 * @file	irc_battle_subproc.h
 * @brief	�ԊO���o�g�� ���X�g�E�X�e�[�^�X�Ăяo��
          ->WIFI_MATCH�ւ����Ă���PROC��
          ->WIFI_CLUB�ւ����Ă���
 * @author	ariizumi -> nagihashi -> ariizumi
 * @data	10/02/26
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
#define WIFICLUB_BATTLE_MEMBER_NUM (2)


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum 
{
  WIFICLUB_BATTLE_SUBPROC_RESULT_SUCCESS,
  WIFICLUB_BATTLE_SUBPROC_RESULT_ERROR_NEXT_LOGIN,
} WIFICLUB_BATTLE_SUBPROC_RESULT;



//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{ 
  REGULATION          *regulation;                  //[in]���M�����[�V����
  POKEPARTY           *selfPokeParty;               //[in]�����̃|�P����
  //�o�g���{�b�N�X�̑Ή����l���ꉞppp�̓R�����g
  //POKEMON_PASO_PARAM  *ppp[TEMOTI_POKEMAX];         //[in]�I�ԃ|�P����
  const STRCODE   *enemyName;                             //[in]����̖��O
  u8        enemySex;                               //[in]����̐���
  u8        pad2[3];                                //�p�f�B���O
  POKEPARTY *enemyPokeParty;                             //[in]����̃|�P����
  GAMEDATA            *gameData;                    //[in]�Q�[���f�[�^
  u8                  comm_selected_num;            //[in]���łɑI�яI������l�� �O������
  u8                  dummy[3];
  POKEPARTY           *p_party;                     //[out]�o�ꂷ��|�P�p�[�e�B(Alloc���Ă�������)
  POKEPARTY  *netParty[2];                   //[out]����̏o�ꂷ��|�P�p�[�e�B(Alloc���Ă�������)
  WIFICLUB_BATTLE_SUBPROC_RESULT  result;           //[out]�I���R�[�h
} WIFICLUB_BATTLE_SUBPROC_PARAM;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	proc data
//--------------------------------------------------------------
extern const GFL_PROC_DATA WifiClubBattle_Sub_ProcData;
