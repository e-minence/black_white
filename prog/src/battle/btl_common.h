//=============================================================================================
/**
 * @file	btl_common.h
 * @brief	�|�P����WB �o�g���V�X�e�� ���[�J�����ʃw�b�_
 * @author	taya
 *
 * @date	2008.09.06	�쐬
 */
//=============================================================================================
#ifndef __BTL_COMMON_H__
#define __BTL_COMMON_H__

#ifdef TEST_IN_DOS
#include "ds_types.h"
#endif


#include <gflib.h>
#include "pm_define.h"
#include "waza_tool\wazadata.h"

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/


//--------------------------------------------------------------
/**
 *	�萔�Q
 */
//--------------------------------------------------------------
enum {
	BTL_CLIENT_MAX = 4,				///< �ғ�����N���C�A���g��
	BTL_POSIDX_MAX = 3,				///< �N���C�A���g���Ǘ�����ő�̐퓬�ʒu��
	BTL_PARTY_MEMBER_MAX = 6,	///< �p�[�e�B�̃����o�[�ő吔

// ��Ԉُ폈���֘A
	BTL_NEMURI_TURN_MIN = 3,	///< �u�˂ނ�v�ŏ��^�[����
	BTL_NEMURI_TURN_MAX = 8,	///< �u�˂ނ�v�ő�^�[����
	BTL_NEMURI_TURN_RANGE = (BTL_NEMURI_TURN_MAX - BTL_NEMURI_TURN_MIN + 1),

	BTL_MAHI_EXE_RATIO = 25,	///< �u�܂Ёv�ł��т�ē����Ȃ��m��
	BTL_KORI_MELT_RATIO = 20,	///< �u������v���n����m��

	BTL_DOKU_SPLIT_DENOM = 8,					///< �u�ǂ��v�ōő�HP�̉����̂P���邩
	BTL_YAKEDO_SPLIT_DENOM = 8,				///< �u�₯�ǁv�ōő�HP�̉����̂P���邩
	BTL_YAKEDO_DAMAGE_RATIO = 50,			///< �u�₯�ǁv�ŕ����_���[�W�������闦
	BTL_MOUDOKU_SPLIT_DENOM = 16,			///< �����ǂ����A�ő�HP�̉����̂P���邩�i��{�l=�ŏ��l�j
	BTL_MOUDOKU_COUNT_MAX = BTL_MOUDOKU_SPLIT_DENOM-1,	///< �����ǂ����A�_���[�W�{���������܂ŃJ�E���g�A�b�v���邩

};

//--------------------------------------------------------------
/**
 *	�w�c������
 */
//--------------------------------------------------------------
typedef enum {

	BTL_SIDE_1ST,		///< �X�^���h�A���[���Ȃ�v���C���[���A�ʐM�ΐ�Ȃ�T�[�o��
	BTL_SIDE_2ND,

	BTL_SIDE_MAX,

}BtlSide;

//--------------------------------------------------------------
/**
 *	�|�P���������ʒu�������i��ʏ�̌����ڂł͂Ȃ��A�T�O��̈ʒu�j
 */
//--------------------------------------------------------------
typedef enum {

	BTL_POS_1ST_0,		///< �X�^���h�A���[���Ȃ�v���C���[���A�ʐM�ΐ�Ȃ�T�[�o��
	BTL_POS_2ND_0,
	BTL_POS_1ST_1,
	BTL_POS_2ND_1,
	BTL_POS_1ST_2,
	BTL_POS_2ND_2,

	BTL_POS_MAX,

}BtlPokePos;

//--------------------------------------------------------------
/**
 *	�l����̂��l�Ԃ�AI���H
 */
//--------------------------------------------------------------
typedef enum {

	BTL_THINKER_UI = 0,
	BTL_THINKER_AI,

	BTL_THINKER_TYPE_MAX,
	BTL_THINKER_NONE = BTL_THINKER_TYPE_MAX,

}BtlThinkerType;

//--------------------------------------------------------------
/**
 *	�u������v��I���������̔����^�C�v
 */
//--------------------------------------------------------------
typedef enum {

	BTL_ESCAPE_MODE_OK,		///< �����Ɂu�ɂ���v�m��i�쐶��Ȃǁj
	BTL_ESCAPE_MODE_NG,		///< ���傤�Ԃ̂������イ�ɔw�����������Ȃ�
	BTL_ESCAPE_MODE_CONFIRM,	///< �ɂ���ƕ����ɂȂ邯�ǃz���g�ɂ����́H�Ɗm�F����

	BTL_ESCAPE_MODE_MAX,

}BtlEscapeMode;

//--------------------------------------------------------------
/**
 *	�_�u���o�g�����̃`�[���\��
 */
//--------------------------------------------------------------
typedef enum {
	BTL_DBLTEAM_ALONE_ALONE,	///< �lvs�l
	BTL_DBLTEAM_ALONE_TAG,		///< �lvs�^�b�O
	BTL_DBLTEAM_TAG_ALONE,		///< �^�b�Ovs�l
	BTL_DBLTEAM_TAG_TAG,		///< �^�b�Ovs�^�b�O
}BtlDblTeamFormation;




#endif
