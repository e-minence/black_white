//============================================================================================
/**
 * @file  leader_board.c
 * @brief ���[�_�[�{�[�h��ʁiWifi�o�g���T�u�E�F�C�_�E�����[�h�f�[�^�j
 * @author  Akito Mori / Hiroyuki Nakamura
 * @date  09.12.03
 */
//============================================================================================
#ifndef __LEADER_BOARD_H__
#define __LEADER_BOARD_H__

#include "gamesystem/game_data.h"
#include "savedata/bsubway_savedata.h"

//============================================================================================
//  �萔��`
//============================================================================================

// �O���ݒ�f�[�^
typedef struct {
  GAMEDATA  *gamedata;  // �Z�[�u�f�[�^
  int       rank_no;    // �����NNO
  int       train_no;   // �g���C��NO
//  BSUBWAY_WIFI_DATA   *bSubwayWifiData;   // �o�g���T�u�E�F�C�f�[�^
//  BSUBWAY_LEADER_DATA *bSubwayLeaderData; // �o�g���T�u�E�F�C���[�_�[�f�[�^
}LEADERBOARD_PARAM;



//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );



extern const GFL_PROC_DATA LeaderBoardProcData;

#endif  // __LEADER_BOARD_H__
