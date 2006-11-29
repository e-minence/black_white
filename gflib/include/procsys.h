//=============================================================================
/**
 * @file	procsys.h
 * @brief	�v���Z�X�Ǘ��w�b�_
 * @date	2005.07.25
 */
//=============================================================================

#ifndef	__PROCSYS_H__
#define	__PROCSYS_H__

#include <nitro.h>
#include <nnsys.h>

//===========================================================================
//
//		��`
//
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X����֐��̌���
 */
//------------------------------------------------------------------
typedef enum {
	GFL_PROC_RES_CONTINUE = 0,		///<����p����
	GFL_PROC_RES_FINISH				///<����I��
}GFL_PROC_RESULT;

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�\���̂ւ̕s���S�^
 *
 * �\���̂̎��ۂ̒�`��procsys.c�ɕ������Ă���
 */
//------------------------------------------------------------------
typedef struct _GFL_PROC GFL_PROC;

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X����\���̂ւ̕s���S�^
 *
 * �\���̂̎��ۂ̒�`��procsys.c�ɕ������Ă���
 */
//------------------------------------------------------------------
typedef struct _GFL_PROCSYS GFL_PROCSYS;

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X����֐��̌^��`
 *
 * �t�B�[���h�A�퓬�A���j���[�Ȃǂ̓���֐��͉��L�̌`���ō쐬����
 */
//------------------------------------------------------------------
typedef GFL_PROC_RESULT (*GFL_PROC_FUNC)(GFL_PROC *, int *, void *, void *);

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X��`�f�[�^
 *
 * �v���Z�X�̏������E���C���E�I���֐���o�^�����f�[�^
 */
//------------------------------------------------------------------
typedef struct {
	GFL_PROC_FUNC init_func;
	GFL_PROC_FUNC main_func;
	GFL_PROC_FUNC end_func;
}GFL_PROC_DATA;

//------------------------------------------------------------------
/**
 * @brief	�����I�[�o�[���C�w��
 */
//------------------------------------------------------------------
#define	NO_OVERLAY_ID	(0xffffffff)


//===========================================================================
//
//		�֐��O���Q��
//
//===========================================================================
extern GFL_PROCSYS * GFL_PROC_SysInit(u32 heap_id);
extern void GFL_PROC_SysMain(GFL_PROCSYS * psys);
extern void GFL_PROC_SysExit(GFL_PROCSYS * psys);

extern void GFL_PROC_SysCallProc(GFL_PROCSYS * psys, FSOverlayID ov_id,
		const GFL_PROC_DATA * procdata, void * pwork);
extern void GFL_PROC_SysSetNextProc(GFL_PROCSYS * psys, FSOverlayID ov_id,
		const GFL_PROC_DATA * procdata, void * pwork);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void * GFL_PROC_AllocWork(GFL_PROC * proc, unsigned int size, u32 heap_id);
extern void GFL_PROC_FreeWork(GFL_PROC * proc);



#endif /* __PROCSYS_H__ */

