//============================================================================
//                         Pokemon Network Library
//============================================================================
#ifndef ___POKE_NET_OPT___
#define ___POKE_NET_OPT___

#include "poke_net_dscomp.h"

//==============================================
//                 �u����������
//==============================================

//==============================================
//              �N���X/�\���̒�`
//==============================================

//==============================================
//                   �֐���`
//==============================================
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void POKE_NET_OptEncode(const unsigned char *_In, long _Size, unsigned char *_Out);	// �I�v�V�����G���R�[�h
void POKE_NET_OptDecode(const unsigned char *_In, long _Size, unsigned char *_Out);	// �I�v�V�����f�R�[�h
long POKE_NET_GetRequestSize(long _reqno);											// ���N�G�X�g�T�C�Y�擾
long POKE_NET_GetResponseMaxSize(long _reqno);										// ���X�|���X�ő�T�C�Y�擾
u64 POKE_NET_EncodePlayDataID(u64 _id);												// �v���C�f�[�^�h�c�̈Í���
u64 POKE_NET_DecodePlayDataID(u64 _id);												// �v���C�f�[�^�h�c�̕�����

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ___POKE_NET_OPT___
