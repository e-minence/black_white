//============================================================================================
/**
 * @file	itemuse.h
 * @brief	�t�B�[���h�A�C�e���g�p����
 * @author	Hiroyuki Nakamura
 * @date	05.12.12
 */
//============================================================================================

#pragma once

#include "app/itemuse.h"
#include "gamesystem/gamesystem.h"

//============================================================================================
//	�萔��`
//============================================================================================
// �A�C�e���g�p���[�N
struct _ITEMUSE_WORK{
	GMEVENT * event;	// �C�x���g
	u16	item;					// �A�C�e���ԍ�
	u8	use_poke;				// �g�p����|�P����
} ;

// �A�C�e���g�p�`�F�b�N���[�N
 struct _ITEMCHECK_WORK{
	int	zone_id;	// �]�[��ID
	int PlayerForm;	//�@���@�̌`��i���]�Ԃɏ���Ă��邩�Ƃ��j
	
	u16 Companion:1;	//�@�A�������Ԃ��H
	u16 Disguise:1;		//�@���P�b�g�c�ϑ������H
	u16 dmy:14;		//�@���P�b�g�c�ϑ������H

	u16 FrontAttr;	//�@���@�O���A�g���r���[�g
	u16 NowAttr;	//�@���@���݃A�g���r���[�g
	u16 SeedInfo;	//�g�p�\�ȃA�C�e�����i���̂݊֘A�j
//	PLAYER_STATE_PTR player;	//���@���Q�ƃ|�C���^
   GAMESYS_WORK * gsys;
} ;

