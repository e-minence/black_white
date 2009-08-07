//============================================================================================
/**
 * @file	world_trade.h
 * @bfief	���E��������
 * @author	Akito Mori
 * @date	06.04.16
 */
//============================================================================================
#ifndef _WORLD_TRADE_H_
#define _WORLD_TRADE_H_

#include "savedata/worldtrade_data.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "savedata/record.h"
#include "savedata/system_data.h"
#include "savedata/myitem_savedata.h"
#include "savedata/box_savedata.h"
#include "savedata/wifilist.h"
#include "../../src/net_app/wifi_worldtrade/worldtrade_adapter.h"

//============================================================================================
//	��`
//============================================================================================

// �v���Z�X��`�f�[�^
extern GFL_PROC_RESULT WorldTradeProc_Init( GFL_PROC * proc, int * seq, void * param, void * work );
extern GFL_PROC_RESULT WorldTradeProc_Main( GFL_PROC * proc, int * seq, void * param, void * work );
extern GFL_PROC_RESULT WorldTradeProc_End( GFL_PROC * proc, int * seq, void * param, void * work );

// �|�C���^�Q�Ƃ����ł��鐢�E�������[�N�\����
typedef struct _WORLDTRADE_WORK WORLDTRADE_WORK;	


typedef struct{
	WORLDTRADE_DATA *worldtrade_data;	// ���E�����f�[�^
	SYSTEMDATA		*systemdata;		// �V�X�e���Z�[�u�f�[�^�iDPW���C�u�����p�Ƃ������R�[�h��ۑ��j
	POKEPARTY       *myparty;			// �莝���|�P����
	BOX_DATA        *mybox;				// �{�b�N�X�f�[�^
	ZUKAN_WORK      *zukanwork;			// �}�Ӄf�[�^
	WIFI_LIST		*wifilist;			// �Ƃ������蒠
	WIFI_HISTORY	*wifihistory;		// �n���V�f�[�^
	MYSTATUS		*mystatus;			// �g���[�i�[���
	CONFIG			*config;			// �R���t�B�O�\����
	RECORD			*record;			// ���R�[�h�R�[�i�[�|�C���^
	MYITEM			*myitem;			// �A�C�e���|�P�b�g�|�C���^
	int				zukanmode;			// �}�Ӄ��[�h�i�V���I�E�E�S��)
	int				profileId;			// GameSpy�v���t�@�C��ID
	int				contestflag;		// �R���e�X�g�����Ă邩�t���O
	int				connect;			// ����f�r�h�c���擾�����i1:�擾���Ă��� 0:���������Ă�)
	SAVE_CONTROL_WORK * savedata;

}WORLDTRADE_PARAM;

#endif
