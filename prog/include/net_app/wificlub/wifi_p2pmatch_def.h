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


#endif //__WIFI_P2PMATCH_DEF_H__

