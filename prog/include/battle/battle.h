//=============================================================================================
/**
 * @file	battle.h
 * @brief	�|�P����WB �o�g���V�X�e�� �O���V�X�e���Ƃ̃C���^�[�t�F�C�X
 * @author	taya
 *
 * @date	2008.09.22	�쐬
 */
//=============================================================================================
#ifndef __BATTLE_H__
#define __BATTLE_H__

#ifdef TEST_IN_DOS
#include "ds_types.h"
#endif

#include <net.h>
#include <procsys.h>

#include "poke_tool/pokeparty.h"

//--------------------------------------------------------------
/**
 *	�쓮�^�C�v
 */
//--------------------------------------------------------------
typedef enum {

	BTL_ENGINE_ALONE,			///< �X�^���h�A����
	BTL_ENGINE_COMM_PARENT,		///< �ʐM�i�e�j
	BTL_ENGINE_COMM_CHILD,		///< �ʐM�i�q�j

}BtlEngineType;

//--------------------------------------------------------------
/**
 *	�ΐ탋�[��
 */
//--------------------------------------------------------------
typedef enum {

	BTL_RULE_SINGLE,		///< �V���O��
	BTL_RULE_DOUBLE,		///< �_�u��
	BTL_RULE_TRIPLE,		///< �g���v��

}BtlRule;

//--------------------------------------------------------------
/**
 *	�ΐ푊��
 */
//--------------------------------------------------------------
typedef enum {

	BTL_COMPETITOR_WILD,		///< �쐶
	BTL_COMPETITOR_TRAINER,		///< �Q�[�����g���[�i�[
	BTL_COMPETITOR_COMM,		///< �ʐM�ΐ�

}BtlCompetitor;

//--------------------------------------------------------------
/**
 *	�ʐM�^�C�v
 */
//--------------------------------------------------------------
typedef enum {

	BTL_COMM_NONE,	///< �ʐM���Ȃ�
	BTL_COMM_DS,	///< DS�����ʐM
	BTL_COMM_WIFI,	///< Wi-Fi�ʐM

}BtlCommMode;


typedef struct {

	BtlEngineType		engine;
	BtlCompetitor		competitor;
	BtlRule				rule;

	GFL_NETHANDLE*		netHandle;
	BtlCommMode			commMode;
	u8							commPos;		///< �ʐM�ΐ�Ȃ玩���̗����ʒu�i��ʐM���͖����j
	u8							netID;			///< NetID

	POKEPARTY*			partyPlayer;	///< �v���C���[�̃p�[�e�B
	POKEPARTY*			partyPartner;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
	POKEPARTY*			partyEnemy1;	///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
	POKEPARTY*			partyEnemy2;	///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j



}BATTLE_SETUP_PARAM;


//--------------------------------------------------------------
/**
 *	Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA BtlProcData;


#endif
