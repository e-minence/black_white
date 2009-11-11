//=============================================================================================
/**
 * @file  btl_common.h
 * @brief �|�P����WB �o�g���V�X�e�� ���[�J�����ʃw�b�_
 * @author  taya
 *
 * @date  2008.09.06  �쐬
 */
//=============================================================================================
#ifndef __BTL_COMMON_H__
#define __BTL_COMMON_H__


#include <gflib.h>
#include "pm_define.h"
#include "waza_tool\wazadata.h"

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/


//--------------------------------------------------------------
/**
 *  �萔�Q
 */
//--------------------------------------------------------------
enum {
  BTL_CLIENT_MAX = 4,       ///< �ғ�����N���C�A���g��
  BTL_POSIDX_MAX = 3,       ///< �N���C�A���g���Ǘ�����ő�̐퓬�ʒu��
  BTL_PARTY_MEMBER_MAX = 6, ///< �p�[�e�B�̃����o�[�ő吔
  BTL_FRONT_POKE_MAX = BTL_POSIDX_MAX*2,  ///< �o�g���ɏo�Ă���|�P�����̍ő吔
  BTL_POKEID_MAX = (BTL_CLIENT_MAX*BTL_PARTY_MEMBER_MAX), ///< �|�P����ID�̍ő�l
  BTL_POKEID_NULL = 31,     ///< �������ۏ؂����|�P����ID�i�T�[�o�R�}���h�����̓s����A5bit�Ɏ��܂�悤�Ɂc�j
  BTL_TURNCOUNT_MAX = 9999, ///< �J�E���g����^�[�����̍ő�
  BTL_SHOOTER_ENERGY_MAX = 14,  ///< �V���[�^�[�̒~�σG�l���M�[�ő�l

  BTL_CLIENTID_NULL = BTL_CLIENT_MAX,
};

//--------------------------------------------------------------
/**
 *  �w�c������
 */
//--------------------------------------------------------------
typedef enum {

  BTL_SIDE_1ST,   ///< �X�^���h�A���[���Ȃ�v���C���[���A�ʐM�ΐ�Ȃ�T�[�o��
  BTL_SIDE_2ND,

  BTL_SIDE_MAX,

}BtlSide;

//--------------------------------------------------------------
/**
 *  �|�P���������ʒu�������i��ʏ�̌����ڂł͂Ȃ��A�T�O��̈ʒu�j
 */
//--------------------------------------------------------------
typedef enum {

  BTL_POS_1ST_0,    ///< �X�^���h�A���[���Ȃ�v���C���[���A�ʐM�ΐ�Ȃ�T�[�o��
  BTL_POS_2ND_0,
  BTL_POS_1ST_1,
  BTL_POS_2ND_1,
  BTL_POS_1ST_2,
  BTL_POS_2ND_2,

  BTL_POS_MAX,
  BTL_POS_NULL = BTL_POS_MAX,

}BtlPokePos;

//--------------------------------------------------------------
/**
 *  �l����̂��l�Ԃ�AI���H
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
 *  �u������v��I���������̔����^�C�v
 */
//--------------------------------------------------------------
typedef enum {

  BTL_ESCAPE_MODE_OK,   ///< �����Ɂu�ɂ���v�m��i�쐶��Ȃǁj
  BTL_ESCAPE_MODE_NG,   ///< ���傤�Ԃ̂������イ�ɔw�����������Ȃ�
  BTL_ESCAPE_MODE_CONFIRM,  ///< �ɂ���ƕ����ɂȂ邯�ǃz���g�ɂ����́H�Ɗm�F����

  BTL_ESCAPE_MODE_MAX,

}BtlEscapeMode;

//--------------------------------------------------------------
/**
 *  �_�u���o�g�����̃`�[���\��
 */
//--------------------------------------------------------------
typedef enum {
  BTL_DBLTEAM_ALONE_ALONE,  ///< �lvs�l
  BTL_DBLTEAM_ALONE_TAG,    ///< �lvs�^�b�O
  BTL_DBLTEAM_TAG_ALONE,    ///< �^�b�Ovs�l
  BTL_DBLTEAM_TAG_TAG,    ///< �^�b�Ovs�^�b�O
}BtlDblTeamFormation;

//--------------------------------------------------------------
/**
 *  ���[�e�[�V�����o�g���̉�]����
 */
//--------------------------------------------------------------
typedef enum {
  BTL_ROTATEDIR_NONE = 0,
  BTL_ROTATEDIR_STAY,
  BTL_ROTATEDIR_R,
  BTL_ROTATEDIR_L,
}BtlRotateDir;


#endif
