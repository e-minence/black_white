//======================================================================
/**
 *
 * @file	ari_debug.h
 * @brief	�L��ėp�f�o�b�O
 * @author	ariizumi
 * @data	08.10.16
 */
//======================================================================
#ifndef ARI_DEBUG_H__
#define ARI_DEBUG_H__

//�f�o�b�O�L���̃t���O
#if DEBUG_ONLY_FOR_ariizumi_nobuhiko | DEBUG_ONLY_FOR_ohno | DEBUG_ONLY_FOR_matsuda | DEBUG_ONLY_FOR_iwao_kazumasa
#define DEB_ARI 1
#else
#define DEB_ARI 0
#endif

//�f�o�b�O�p�}�N��
#if DEB_ARI
#define ARI_TPrintf(...) (void)((OS_TFPrintf(2,__VA_ARGS__)))
#define ARI_Printf(...)  (void)((OS_FPrintf(2,__VA_ARGS__)))
#else
#define ARI_TPrintf(...) ((void)0)
#define ARI_Printf(...)  ((void)0)
#endif //DEB_ARI

#define F32_CONST(v)	FX_FX32_TO_F32(v)
//�Ƃ肠������`�������̂ŉ��t�F�[�h�X�s�[�h��`
#define ARI_FADE_SPD	(0)	//��i�K�ς��Ƒ҂t���[���������炱���16�t���[��

#endif //ARI_DEBUG_H__
