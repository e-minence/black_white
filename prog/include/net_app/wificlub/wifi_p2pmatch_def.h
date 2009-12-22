//============================================================================================
/**
 * @file	wifi_p2pmatch_def.h
 * @bfief	WIFI P2P�ڑ��̃}�b�`���O�Ɋւ����`  �X�N���v�g�Ŏg�p
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================
#ifndef __WIFI_P2PMATCH_DEF_H__
#define __WIFI_P2PMATCH_DEF_H__


// �e�f�o�b�N�p��`
#ifdef PM_DEBUG
#define WFP2P_DEBUG	// �f�o�b�N�@�\��ON

#ifdef WFP2P_DEBUG
//#define WFP2P_DEBUG_EXON	// �قڕK�v�Ȃ����X�g���\������
//#define WFP2P_DEBUG_PLON_PC	// �v���C���[�̏����ʒu�p�\�R���O
#endif

#endif

// P2PMATCH �ւ̎w��
#define WIFI_P2PMATCH_DPW     (1)   // ���̌�DPW�ֈڍs����
#define WIFI_P2PMATCH_P2P     (2)   // ���̂܂�P2P�����Ɉڍs



// P2PMATCH ����̖߂�l
typedef enum {
  WIFI_P2PMATCH_NONE,
  WIFI_P2PMATCH_SBATTLE50,   // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_SBATTLE100,   // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_SBATTLE_FREE,      // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_DBATTLE50,        // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_DBATTLE100,        // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_DBATTLE_FREE,      // �ʐM�ΐ�Ăяo��
  WIFI_P2PMATCH_TRADE,         // �|�P�����g���[�h�Ăяo��
  WIFI_P2PMATCH_TVT,      // �g�����V�[�o
  WIFI_P2PMATCH_END,           // �ʐM�ؒf���Ă܂��B�I�����܂��B
  WIFI_P2PMATCH_UTIL,          // �ڑ��ݒ���Ăяo���܂��B�߂�܂���
  WIFI_P2PMATCH_DPW_END,       // ���E�ΐ�p�ݒ�I��
} WIFI_P2PMATCH_KINDENUM;

  
typedef enum{
	WIFI_BATTLEFLAG_SINGLE,	// �V���O���o�g��
	WIFI_BATTLEFLAG_DOUBLE,	// �_�u���o�g��
	WIFI_BATTLEFLAG_MULTI,	// �}���`�o�g��
} WIFI_BATTLEFLAG;

#endif //__WIFI_P2PMATCH_DEF_H__

