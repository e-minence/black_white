//============================================================================================
/**
 * @file		box2.h
 * @brief		�{�b�N�X���
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2
 */
//============================================================================================
#pragma	once

#include "savedata/box_savedata.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "gamesystem/game_data.h"
#include "poke_tool/pokeparty.h"
#include "box_mode.h"
/*
#include "savedata/savedata_def.h"
//#include "system/procsys.h"
#include "system/keytouch_status.h"
*/

//============================================================================================
//	�萔��`
//============================================================================================

typedef int BOX_MODE;

// �O���ݒ�f�[�^
typedef struct {
	// [in]
  GAMEDATA * gamedata;			  // �Q�[���f�[�^ ( �o�b�O/�X�e�[�^�X��Y�{�^���o�^�Ɏg�p )
	BOX_MANAGER * sv_box;				// �{�b�N�X�Z�[�u�f�[�^
	POKEPARTY * pokeparty;			// �莝�����P����
	MYITEM_PTR	myitem;					// �����A�C�e���i�o�b�O�Ŏg�p�j
	MYSTATUS * mystatus;				// �v���C���[�f�[�^�i�o�b�O�Ŏg�p�j

  CONFIG * cfg;								// �R���t�B�O�f�[�^
  u32	zknMode;								// �}�Ӄi���o�[�\�����[�h

	BOX_MODE	callMode;					// �Ăяo�����[�h

/*
	// [in] 
	SAVEDATA * savedata;
	KEYTOUCH_STATUS * kt_status;	// �L�[�^�b�`�X�e�[�^�X

	// [out]
//	BOOL	modifiedFlag;	///< �{�b�N�X���e���ύX���ꂽ��TRUE������
*/

}BOX2_GFL_PROC_PARAM;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//============================================================================================
//	�O���[�o��
//============================================================================================

// �{�b�N�XPROC�f�[�^
extern const GFL_PROC_DATA BOX2_ProcData;
FS_EXTERN_OVERLAY(box);
