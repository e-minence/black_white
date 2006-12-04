//=============================================================================================
/**
 * @file	procsys.c
 * @brief	�v���Z�X�Ǘ��V�X�e��
 * @date	2005.07.25
 */
//=============================================================================================

#include <nitro.h>
#include <nnsys.h>

#include "gflib.h"

//#include "pm_overlay.h"

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

	BOOL call_flag;				///<�q�v���Z�X�Ăяo���̃��N�G�X�g�t���O
	BOOL jump_flag;				///<���̃��C���v���Z�X�����݂��邩�ǂ����̃t���O
	FSOverlayID next_ov_id;		///<���̃��C���v���Z�X�̃I�[�o�[���CID�i���g�p�j
	const GFL_PROC_DATA * next_data;	///<���̃��C���v���Z�X�̐����f�[�^
	void * next_param;			///<���̃��C���v���Z�X�̃��[�N�ւ̃|�C���^
};


//------------------------------------------------------------------
//
//		�����g�p�֐�
//		���O�����J���Ă��Ȃ�
//
//------------------------------------------------------------------
extern GFL_PROC * GFI_PROC_Create(const GFL_PROC_DATA * data, void * parent_work, const u32 heap_id);
extern void GFI_PROC_Delete(GFL_PROC * proc);
extern BOOL GFI_PROC_Main(GFL_PROC * proc);

extern GFL_PROCSYS * GFI_PROC_SysInit(u32 heap_id);
extern void GFI_PROC_SysMain(GFL_PROCSYS * psys);
extern void GFI_PROC_SysExit(GFL_PROCSYS * psys);

extern void GFI_PROC_SysCallProc(GFL_PROCSYS * psys, FSOverlayID ov_id,
		const GFL_PROC_DATA * procdata, void * pwork);
extern void GFI_PROC_SysSetNextProc(GFL_PROCSYS * psys, FSOverlayID ov_id,
		const GFL_PROC_DATA * procdata, void * pwork);

//=============================================================================================
//
//
//		�O�����J�֐�
//
//		�����֐�GFI_PROC_�`�̃��b�p�[�Ƃ��ėp�ӂ���Ă���
//
//
//=============================================================================================

//------------------------------------------------------------------
/**
 * @brief	PROC�V�X�e���p���[�N�ێ��|�C���^
 * 
 * PROC�V�X�e���̓Q�[�����ɂЂƂ������݂��Ȃ��ƍl������̂�
 * ������������ϐ��Ƃ��ĕێ����Ă����悤�ɏC��
 */
//------------------------------------------------------------------
static GFL_PROCSYS * gfl_procsys;

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysInit(u32 heap_id)
{
	gfl_procsys = GFI_PROC_SysInit(heap_id);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysMain(void)
{
	GFI_PROC_SysMain(gfl_procsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysExit(void)
{
	GFI_PROC_SysExit(gfl_procsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysCallProc(FSOverlayID ov_id, const GFL_PROC_DATA * procdata, void * pwork)
{
	GFI_PROC_SysCallProc(gfl_procsys, ov_id, procdata, pwork);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GFL_PROC_SysSetNextProc(FSOverlayID ov_id, const GFL_PROC_DATA * procdata, void * pwork)
{
	GFI_PROC_SysSetNextProc(gfl_procsys, ov_id, procdata, pwork);
}

//=============================================================================================
//
//		�֐�
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�Ǘ��V�X�e��������
 * @param	heap_id			�g�p����q�[�v��ID
 * @return	GFL_PROCSYS		���䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
GFL_PROCSYS * GFI_PROC_SysInit(u32 heap_id)
{
	GFL_PROCSYS * psys = GFL_HEAP_AllocMemory(heap_id, sizeof(GFL_PROCSYS));
	psys->heap_id = heap_id;
	psys->ov_id = NO_OVERLAY_ID;
	psys->proc = NULL;
	psys->call_flag = FALSE;
	psys->jump_flag = FALSE;
	psys->next_ov_id = NO_OVERLAY_ID;
	psys->next_data = NULL;
	psys->next_param = NULL;
	return psys;
}

//------------------------------------------------------------------
/**
 * brief	�v���Z�X�Ǘ��V�X�e�����C��
 * @param	psys
 */
//------------------------------------------------------------------
void GFI_PROC_SysMain(GFL_PROCSYS * psys)
{
	BOOL result;
	if (psys->proc == NULL) {
		//OS_Halt();
		return;
	}

	result = GFI_PROC_Main(psys->proc);

	if (psys->call_flag) {
		GFL_PROC * proc;
		proc = GFI_PROC_Create(psys->next_data, psys->next_param, psys->heap_id);
		proc->parent = psys->proc;
		psys->proc->child = proc;
		psys->proc = proc;
		psys->call_flag = FALSE;
	} else if (result == TRUE) {
		if (psys->jump_flag) {
			//�v���Z�X�̕�����s�����ꍇ
			GFL_PROC * parent;
			parent = psys->proc->parent;
			GFI_PROC_Delete(psys->proc);
			psys->proc = GFI_PROC_Create(psys->next_data, psys->next_param, psys->heap_id);
			parent->child = psys->proc;
			psys->proc->parent = parent;
			psys->jump_flag = FALSE;
		} else {
			//�P�ɏI������ʃv���Z�X�ɖ߂�ꍇ
			GFL_PROC * parent;
			parent = psys->proc->parent;
			GFI_PROC_Delete(psys->proc);
			psys->proc = parent;
			psys->proc->child = NULL;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�Ǘ��V�X�e���I������
 * @param	psys
 */
//------------------------------------------------------------------
void GFI_PROC_SysExit(GFL_PROCSYS * psys)
{
	if (psys->proc != NULL) {
		OS_Halt();
	}
	GFL_HEAP_FreeMemory(psys);
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�؂�ւ����N�G�X�g
 * @param	psys
 * @param	procdata
 * @param	pwork
 */
//------------------------------------------------------------------
void GFI_PROC_SysSetNextProc(GFL_PROCSYS * psys, FSOverlayID ov_id, 
		const GFL_PROC_DATA * procdata, void * pwork)
{
	psys->jump_flag = TRUE;
	psys->next_ov_id = ov_id;
	psys->next_data = procdata;
	psys->next_param = pwork;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�Ăяo��
 * @param	psys
 * @param	procdata
 * @param	pwork
 */
//------------------------------------------------------------------
void GFI_PROC_SysCallProc(GFL_PROCSYS * psys, FSOverlayID ov_id,
		const GFL_PROC_DATA * procdata, void * pwork)
{
	if (psys->proc == NULL) {
		//��ԍŏ��̃v���Z�X�̏ꍇ
		GFL_PROC * proc;
		proc = GFI_PROC_Create(procdata, pwork, psys->heap_id);
		psys->proc = proc;
	} else {
		//�T�u�v���Z�X�̏ꍇ
		psys->call_flag = TRUE;
		psys->next_ov_id = ov_id;
		psys->next_data = procdata;
		psys->next_param = pwork;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X��������
 * @param	data		�v���Z�X��`�f�[�^�ւ̃|�C���^
 * @param	parent_work	��ʂ�������n�����[�N�ւ̃|�C���^
 * @param	heap_id		�g�p����q�[�v
 * @return	GFL_PROC	���������v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
GFL_PROC * GFI_PROC_Create(const GFL_PROC_DATA * data, void * parent_work, const u32 heap_id)
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
 * @brief	�v���Z�X���폜����
 * @param	proc	�v���Z�X�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFI_PROC_Delete(GFL_PROC * proc)
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
 * @brief	�v���Z�X���쏈�����C��
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @retval	TRUE	�v���Z�X����I��
 * @retval	FALSE	�v���Z�X����p����
 */
//------------------------------------------------------------------
BOOL GFI_PROC_Main(GFL_PROC * proc)
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
		result = proc->data.init_func(proc, &proc->subseq, proc->parent_work, proc->work);
		if (result == GFL_PROC_RES_FINISH) {
			proc->proc_seq = SEQ_MAIN;
			proc->subseq = 0;
		}
		break;
	case SEQ_MAIN:
		result = proc->data.main_func(proc, &proc->subseq, proc->parent_work, proc->work);
		if (result == GFL_PROC_RES_FINISH) {
			proc->proc_seq = SEQ_END;
			proc->subseq = 0;
		}
		break;
	case SEQ_END:
		result = proc->data.end_func(proc, &proc->subseq, proc->parent_work, proc->work);
		if (result == GFL_PROC_RES_FINISH) {
			if(proc->overlay_id != NO_OVERLAY_ID){
				//Overlay_UnloadID( proc->overlay_id );
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}

