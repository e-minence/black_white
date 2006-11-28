//=============================================================================
/**
 * @file	procsys.c
 * @brief	�v���Z�X�Ǘ��V�X�e��
 * @date	2005.07.25
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
//#include "common.h"
//#include "system\pm_overlay.h"

#include "gflib.h"

#include "procsys.h"

#include "heapsys.h"


//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�����̓���V�[�P���X
 */
//------------------------------------------------------------------
enum {
	SEQ_OVERLAY_LOAD = 0,
	SEQ_INIT,
	SEQ_MAIN,
	SEQ_END,
	SEQ_OVERLAY_UNLOAD
};


//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�\���̒�`
 */
//------------------------------------------------------------------
struct _GFL_PROC{
	GFL_PROCSYS * sys;
	GFL_PROC_DATA data;				///<�v���Z�X��`�f�[�^�ւ̃|�C���^
	FSOverlayID overlay_id;
	int proc_seq;				///<�v���Z�X�����V�[�P���X
	int subseq;					///<�v���Z�X�֐��̓���V�[�P���X
	void * parent_work;			///<��ʃ��[�N�ւ̃|�C���^
	void * work;				///<�v���Z�X�������[�N�ւ̃|�C���^
	GFL_PROC * parent;				///<��ʃv���Z�X�ւ̃|�C���^
	GFL_PROC * child;				///<���ʃv���Z�X�ւ̃|�C���^
};

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�V�X�e���\���̒�`
 */
//------------------------------------------------------------------
struct _GFL_PROCSYS{
	u32 heap_id;				///<�g�p����q�[�v�w��
	FSOverlayID ov_id;			///<���݂̃��C���v���Z�X�̃I�[�o�[���CID�i���g�p�j
	GFL_PROC * proc;			///<���݂̃��C���v���Z�X�̃|�C���^

	BOOL next_flag;				///<���̃��C���v���Z�X�����݂��邩�ǂ����̃f�[�^
	GFL_PROC_DATA next_data;	///<���̃��C���v���Z�X�̐����f�[�^
	void * next_param;			///<���̃��C���v���Z�X�̃��[�N�ւ̃|�C���^
	FSOverlayID next_ov_id;		///<���̃��C���v���Z�X�̃I�[�o�[���CID�i���g�p�j
};


//===========================================================================
//
//		�֐�
//
//===========================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
GFL_PROCSYS * GFL_PROC_SysInit(u32 heap_id)
{
	GFL_PROCSYS * psys = GFL_HEAP_AllocMemory(heap_id, sizeof(GFL_PROCSYS));
	psys->proc = NULL;
	psys->heap_id = heap_id;
	return psys;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysMain(GFL_PROCSYS * psys)
{
	BOOL result;
	if (psys->proc == NULL) {
		OS_Halt();
		return;
	}

	result = GFL_PROC_Main(psys->proc);

	if (result == TRUE) {
		if (psys->next_flag == FALSE) {
			GFL_PROC * parent;
			parent = psys->proc->parent;
			GFL_PROC_Delete(psys->proc);
			psys->proc = parent;
			psys->proc->child = NULL;
		} else {
			psys->next_flag = FALSE;
			GFL_PROC_Delete(psys->proc);
			psys->proc = GFL_PROC_Create(&psys->next_data, psys->next_param, psys->heap_id);
		}
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysExit(GFL_PROCSYS * psys)
{
	if (psys->proc != NULL) {
		OS_Halt();
	}
	GFL_HEAP_FreeMemory(psys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysSetNextProc(GFL_PROCSYS * psys, const GFL_PROC_DATA * procdata, void * param)
{
	psys->next_flag = TRUE;
	psys->next_data = *procdata;
	psys->next_param = param;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysCallProc(GFL_PROCSYS * psys, const GFL_PROC_DATA * procdata, void * param)
{
	GFL_PROC * proc;
	GFL_PROC * parent;
	proc = GFL_PROC_Create(procdata, param, psys->heap_id);

	if (psys->proc == NULL) {
		//��ԍŏ��̃v���Z�X�̏ꍇ
		psys->proc = proc;
	} else {
		//�T�u�v���Z�X�̏ꍇ
		proc->parent = psys->proc;
		psys->proc->child = proc;
		psys->proc = proc;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysCallNextProc(GFL_PROCSYS * psys, GFL_PROC * proc)
{
	if (psys->proc) {
		psys->proc->child = proc;
	}
	psys->proc = proc;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X��������
 * @param	func		�v���Z�X����֐�
 * @param	parent_work	��ʂ�������n�����[�N�ւ̃|�C���^
 * @param	heap_id	�g�p����q�[�v
 * @return	PROC		���������v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
GFL_PROC * GFL_PROC_Create(const GFL_PROC_DATA * data, void * parent_work, const u32 heap_id)
{
	GFL_PROC * proc;
	proc = GFL_HEAP_AllocMemory(heap_id, sizeof(GFL_PROC));
	proc->data = *data;
	proc->overlay_id = NO_OVERLAY_ID;
	proc->proc_seq = 0;
	proc->subseq = 0;
	proc->parent_work = parent_work;
	proc->work = NULL;
	proc->parent = NULL;
	proc->child = NULL;
	return proc;
}

//------------------------------------------------------------------
/**
 * @brief	�q�v���Z�X�𐶐�����
 * @param	proc		���݂̓���v���Z�X�ւ̃|�C���^
 * @param	func		�v���Z�X����֐�
 * @param	parent_work	��ʂ�������n�����[�N�ւ̃|�C���^
 * @param	heap_id	�g�p����q�[�v
 * @return	PROC		���������v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
GFL_PROC * GFL_PROC_CreateChild(GFL_PROC * proc, const GFL_PROC_DATA * data, void * parent_work, const u32 heap_id)
{
	GFL_PROC * child;
	child = GFL_PROC_Create(data, parent_work, heap_id);
	proc->child = child;
	child->parent = proc;
	return child;
}
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X���폜����
 * @param	proc	�v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFL_PROC_Delete(GFL_PROC * proc)
{
	SDK_ASSERT(proc->work == NULL);
	GFL_HEAP_FreeMemory(proc);
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�����[�N�̊m��
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @param	size	�m�ۂ��郏�[�N�T�C�Y
 * @param	heap_id	�g�p����q�[�v
 * @return	void *	�m�ۂ����v���Z�X�����[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void * GFL_PROC_AllocWork(GFL_PROC * proc, unsigned int size, u32 heap_id)
{
	proc->work = GFL_HEAP_AllocMemory(heap_id, size);
	return proc->work;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�����[�N�̃|�C���^�擾
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @return	void *	�v���Z�X�����[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void * GFL_PROC_GetWork(GFL_PROC * proc)
{
	SDK_ASSERT(proc->work != NULL);
	return proc->work;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�����[�N�̉��
 * @param	proc	�v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFL_PROC_FreeWork(GFL_PROC * proc)
{
	SDK_ASSERT(proc->work != NULL);
	GFL_HEAP_FreeMemory(proc->work);
	proc->work = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	��ʃ��[�N�ւ̃|�C���^�擾
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @return	void *	��ʃ��[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void * GFL_PROC_GetParentWork(GFL_PROC * proc)
{
	SDK_ASSERT(proc->parent_work != NULL);
	return proc->parent_work;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X���쏈�����C��
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @retval	TRUE	�v���Z�X����I��
 * @retval	FALSE	�v���Z�X����p����
 */
//------------------------------------------------------------------
BOOL GFL_PROC_Main(GFL_PROC * proc)
{
	GFL_PROC_RESULT result;

	switch (proc->proc_seq) {
	case SEQ_OVERLAY_LOAD:
		if(proc->overlay_id != NO_OVERLAY_ID){
			//TPRINTF("OVERLAY ID = %d\n", proc->overlay_id);
			//Overlay_Load( proc->overlay_id, OVERLAY_LOAD_NOT_SYNCHRONIZE);
		}
		proc->proc_seq = SEQ_INIT;

		/* fallthru */
			
	case SEQ_INIT:
		result = proc->data.init_func(proc, &proc->subseq);
		if (result == PROC_RES_FINISH) {
			proc->proc_seq = SEQ_MAIN;
			proc->subseq = 0;
		}
		break;
	case SEQ_MAIN:
		result = proc->data.main_func(proc, &proc->subseq);
		if (result == PROC_RES_FINISH) {
			proc->proc_seq = SEQ_END;
			proc->subseq = 0;
		}
		break;
	case SEQ_END:
		result = proc->data.end_func(proc, &proc->subseq);
		if (result == PROC_RES_FINISH) {
			if(proc->overlay_id != NO_OVERLAY_ID){
				//Overlay_UnloadID( proc->overlay_id );
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}


