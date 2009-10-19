//======================================================================
/**
 * @file	irc_battle_local_def.h
 * @brief	�ԊO���o�g��
 * @author	ariizumi
 * @data	09/10/19
 *
 * ���W���[�����FIRC_BATTLE
 */
//======================================================================
#pragma once

#include "battle/battle.h"
#include "net/network_define.h"
#include "print/printsys.h"

#include "net_app/irc_battle.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define IRC_BATTLE_MEMBER_NUM (2)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//�X�e�[�g
typedef enum
{
  IBS_FADEIN,
  IBS_FADEIN_WAIT,
  IBS_INIT_NET,
  IBS_INIT_NET_WAIT,
  
  IBS_IRC_CONNECT_MSG,
  IBS_IRC_CONNECT_MSG_WAIT,
  
  IBS_IRC_TRY_CONNECT,
  IBS_WAIT_POST_MATCHDATA,
  
  IBS_MATCHDATA_IS_SAME,  //�}�b�`�f�[�^��v
  IBS_MATCHDATA_IS_SAME_WAIT,

  IBS_MACHIDATA_NOT_SAME, //�}�b�`�f�[�^�̕s��v
  IBS_MACHIDATA_NOT_SAME_WAIT,
  
  IBS_SELECT_POKE_FADEOUT,//�|�P�����I��
  IBS_SELECT_POKE_FADEOUT_WAIT,
  IBS_SELECT_POKE_WAIT,
  IBS_SELECT_POKE_FADEIN,
  IBS_SELECT_POKE_FADEIN_WAIT,
  IBS_SELECT_POKE_TIMMING,  //�����҂�
  
  IBS_SEND_BATTLE_DATA,
  IBS_START_BATTLE, //�o�g���J�n�ҋ@
  IBS_START_BATTLE_WAIT,
  IBS_START_BATTLE_TIMMING,
  
  IBS_BATTLE_FADEOUT,//�o�g��
  IBS_BATTLE_FADEOUT_WAIT,
  IBS_BATTLE_WAIT,
  IBS_BATTLE_FADEIN,
  IBS_BATTLE_FADEIN_WAIT,
  IBS_BATTLE_TIMMING,  //�����҂�

  IBS_DRAW_RESULT,       //���ʔ��\
  IBS_DRAW_RESULT_WAIT,  

  IBS_FADEOUT,
  IBS_FADEOUT_WAIT,
  IBS_EXIT_NET,
  IBS_EXIT_NET_WAIT,
  
  IBS_MAX,
}IRC_BATTLE_STATE;

//���M���
typedef enum
{
  IBST_MATCH_DATA = GFL_NET_CMD_IRC_BATTLE,  //�}�b�`���O��r�f�[�^
  IBST_BATTLE_DATA,
  
  IBST_MAX,
}IRC_BATTLE_SEND_TYPE;

typedef enum
{
  IBT_POKELIST_END,
  IBT_BATTLE_START,
  IBT_BATTLE_END,
}IRC_BATTLE_TIMMING;
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _IRC_BATTLE_SUBPROC_WORK IRC_BATTLE_SUBPROC_WORK;

//�}�b�`���O��r�f�[�^�\����
typedef struct
{
  u8  championsipNumber;  //�����
  u8  championsipLeague;  //�������[�O
  u8  pad[2];
  //�ꉞ�傫���Ȃ邱�Ƒz�肵�A��M�o�b�t�@����̃p�P�b�g�ő���
}IRC_BATTLE_COMPARE_MATCH;

//�o�g���p���l�f�[�^�\����
//���M����POKEPARTY���f�[�^�̌��ɂ���
typedef struct
{
  STRCODE   name[PERSON_NAME_SIZE+EOM_SIZE];
  u8        sex;
  u8        pad2[3];
  POKEPARTY *pokeParty;
}IRC_BATTLE_BATTLE_DATA;

typedef struct
{
  HEAPID heapId;
  IRC_BATTLE_STATE state;
  
  IRC_BATTLE_COMPARE_MATCH selfCmpareData;
  IRC_BATTLE_COMPARE_MATCH postCmpareData[IRC_BATTLE_MEMBER_NUM];
  BOOL isPostCompareData[IRC_BATTLE_MEMBER_NUM];
  
  //�o�g���f�[�^(IRC_BATTLE_BATTLE_DATA�̌���POKEPARTY�����Ă���
  void *sendBattleData;
  void *postBattleData[IRC_BATTLE_MEMBER_NUM];
  BOOL isPostBattleData[IRC_BATTLE_MEMBER_NUM];
  BATTLE_SETUP_PARAM battleParam;
  
  //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT        *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  STRBUF          *msgStr;

  IRC_BATTLE_SUBPROC_WORK *subProcWork;

  IRC_BATTLE_INIT_WORK *initWork;
}IRC_BATTLE_WORK;


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
#pragma mark [>proc 
extern POKEPARTY* IRC_BATTLE_BattleData_GetPokeParty( void *battleData );
