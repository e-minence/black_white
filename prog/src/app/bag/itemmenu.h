//======================================================================
/**
 * @file	  itemmenu.h
 * @brief	  �A�C�e�����j���[
 * @author	k.ohno
 * @date	  2009.06.30
 */
//======================================================================

#pragma once

#include <gflib.h>
#include "app/bag.h"


// ���j���[����
enum {
	BAG_MENU_TSUKAU = 0,	// ����
	BAG_MENU_ORIRU,			// �����
	BAG_MENU_MIRU,			// �݂�
	BAG_MENU_UMERU,			// ���߂�
	BAG_MENU_HIRAKU,		// �Ђ炭
	BAG_MENU_SUTERU,		// ���Ă�
	BAG_MENU_TOUROKU,		// �Ƃ��낭
	BAG_MENU_KAIZYO,		// ��������
	BAG_MENU_MOTASERU,		// ��������
	BAG_MENU_TAGUWOMIRU,	// �^�O���݂�
	BAG_MENU_KETTEI,		// �����Ă�
	BAG_MENU_YAMERU,		// ��߂�
	BAG_MENU_IDOU,		    // ���ǂ�
	BAG_MENU_URU,			// ����
	BAG_MENU_TSUKAU_NP,		// �����i�؂̎��v�����^�[�p�j
	BAG_MENU_TOMERU,		// �Ƃ߂�i�f�a�v���C���[�I���p�j

	BAG_MENU_MAX
};

enum{
	BAG_MENU_USE=0,		// �����E�݂�E�����
	BAG_MENU_SUB,		// ���Ă�E�Ƃ��낭�E��������
	BAG_MENU_GIVE,		// ��������
	BAG_MENU_ITEMMOVE,	// ���ǂ�
	BAG_MENU_CANCEL,
	BAG_MENUTBL_MAX,
};
