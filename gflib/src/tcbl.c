//=============================================================================================
/**
 * @file	tcb_util.c
 * @author	tamada GAME FREAK inc.
 * @date	2007.02.02
 * @brief	TCB + �������Ǘ�
 *
 * ���܂ł�TCB�̋@�\�ɉ����A�e�^�X�N���ێ����郏�[�N�������̎擾�E�J����
 * �V�X�e�������ł����Ȃ��悤�ɂ����B
 */
//=============================================================================================


#include <nitro.h>
#include <nnsys.h>

#include "gflib.h"
//#include "tcb.h"
#include "heapsys.h"

#include "tcbl.h"

//=============================================================================================
//=============================================================================================
//#define TCB_INLINE	inline
#define TCB_INLINE	

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _GFL_TCBL {
	GFL_TCBLSYS * tcbsys;		///<��������^�X�N�V�X�e���ւ̃|�C���^
	GFL_TCBL * prev;			///<�O�̃^�X�N�ւ̃|�C���^
	GFL_TCBL * next;			///<���̃^�X�N�ւ̃|�C���^
	u32 pri;					///<�^�X�N�̃v���C�I���e�B�w��
	void * work;				///<�^�X�N���[�N�ւ̃|�C���^�iNULL�̂Ƃ��A�m�ۍςݗ̈���g�p�j
	GFL_TCBLFUNC * func;		///<�^�X�N�֐��ւ̃|�C���^
};

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _GFL_TCBLSYS {
	u32 tcb_max;				///<�ێ��ł���^�X�N�̍ő吔
	u32 work_size;				///<�W���Ŋm�ۂ��Ă��郏�[�N�̃T�C�Y
	HEAPID heap_id;				///<�g�p����q�[�vID
	BOOL add_flag;				///<
	BOOL sucide_flag;			///<���E�������ǂ����̃t���O
	void  * tcb_array;			///<�^�X�N�p�ɕێ������������ւ̃|�C���^
	GFL_TCBL tcb_first;			///<�o�������X�g�擪�ɂȂ�_�~�[�^�X�N
	GFL_TCBL * now_chain;		///<���ݎ��s���̃^�X�N�ւ̃|�C���^
	GFL_TCBL * stack_top;		///<��g�p��Ԃ̃^�X�N���X�g�ւ̃|�C���^
};


//=============================================================================================
//
//
//	�c�[���֐�
//
//
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static TCB_INLINE GFL_TCBL * GetTCBPtr(GFL_TCBLSYS * tcbsys, u32 offset)
{
	return (GFL_TCBL *)( (u32)(tcbsys->tcb_array) + offset * (sizeof(GFL_TCBL) + tcbsys->work_size) );
}

//------------------------------------------------------------------
/**
 * @brief	���g�p�^�X�N���X�g����^�X�N���擾
 */
//------------------------------------------------------------------
static TCB_INLINE GFL_TCBL * PopTCB(GFL_TCBLSYS * tcbsys)
{
	GFL_TCBL * tcb;
	tcb = tcbsys->stack_top;
	if (tcbsys->stack_top == NULL) {
		//Pop�ł��Ȃ�
		return NULL;
	}
	tcbsys->stack_top =tcb->next;
	tcb->next->prev = NULL;

	return tcb;
}

//------------------------------------------------------------------
/**
 * @brief	���g�p�^�X�N���X�g�ւ̃^�X�N�̓o�^
 */
//------------------------------------------------------------------
static TCB_INLINE void PushTCB(GFL_TCBLSYS * tcbsys, GFL_TCBL * tcb)
{
	GFL_TCBL * stack_top;
	stack_top = tcbsys->stack_top;
	tcb->prev = NULL;
	tcb->next = stack_top;
	if (stack_top) {
		stack_top->prev = tcb;
	}
	tcbsys->stack_top = tcb;
	OS_Printf("top:%08x\n", tcbsys->stack_top);
}


//------------------------------------------------------------------
/**
 * @brief	�^�X�N�p�\���̂̏���������
 */
//------------------------------------------------------------------
static void ClearTCBWork(GFL_TCBLSYS * tcbsys, GFL_TCBL * tcb)
{
	tcb->tcbsys = tcbsys;
	tcb->prev = &(tcbsys->tcb_first);
	tcb->next = &(tcbsys->tcb_first);
	tcb->pri = 0;
	tcb->work = NULL;
	tcb->func = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�^�X�N�̏�������
 */
//------------------------------------------------------------------
static void DeleteTCB(GFL_TCBL * tcb)
{
	tcb->func = NULL;
	if (tcb->work) {
		GFL_HEAP_FreeMemory(tcb->work);
	}
	//���X�g����폜
	tcb->prev->next = tcb->next;
	tcb->next->prev = tcb->prev;

	PushTCB(tcb->tcbsys, tcb);
}

//------------------------------------------------------------------
/**
 * @brief	�^�X�N�p���[�N�ւ̃|�C���^�擾
 */
//------------------------------------------------------------------
static void * GetWork(GFL_TCBL * tcb)
{
	if (tcb->work == NULL) {
		return (u8 *)tcb + sizeof(GFL_TCBL);
	} else {
		return tcb->work;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void InitSysWork( GFL_TCBLSYS * tcbsys )
{
	int i;
	GFL_TCBL * prev = NULL;
	GFL_TCBL * now;

	for (i = 0; i < tcbsys->tcb_max; i++) {
		now = GetTCBPtr(tcbsys, i);
		ClearTCBWork(tcbsys,now);
		PushTCB(tcbsys, now);
	}
}

//=============================================================================================
//
//
//		�O�����J�֐�
//
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief	TCBL:����������
 * @param	sys_heap_id
 * @param	task_max
 * @param	work_size
 */
//------------------------------------------------------------------
GFL_TCBLSYS * GFL_TCBL_SysInit( HEAPID sys_heap_id, HEAPID usr_heap_id, u32 task_max, u32 work_size)
{
	GFL_TCBLSYS * tcbsys;

	tcbsys = GFL_HEAP_AllocMemory(sys_heap_id, sizeof(GFL_TCBLSYS));
	tcbsys->tcb_max = task_max;
	tcbsys->work_size = work_size;
	tcbsys->heap_id = usr_heap_id;
	tcbsys->add_flag = FALSE;
	tcbsys->sucide_flag = FALSE;
	//4�o�C�g���E�ɓK������悤�ɐ��K������
	work_size = ROUNDUP(work_size, 4);
	tcbsys->tcb_array = GFL_HEAP_AllocMemory(sys_heap_id, (sizeof(GFL_TCBL) + work_size) * task_max);
	tcbsys->stack_top = NULL;
	InitSysWork(tcbsys);
	ClearTCBWork(tcbsys, &(tcbsys->tcb_first));
	tcbsys->now_chain = NULL;
	return tcbsys;
}

//------------------------------------------------------------------
/**
 * @brief	TCBL:���C������
 * @param	tcbsys	TCBL���䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFL_TCBL_SysMain( GFL_TCBLSYS * tcbsys )
{
	GFL_TCBL * now;
	GFL_TCBL * head = &(tcbsys->tcb_first);

	tcbsys->now_chain = head->next;
	while (tcbsys->now_chain != head) {
		tcbsys->sucide_flag = FALSE;
		tcbsys->now_chain->func(tcbsys->now_chain, GetWork(tcbsys->now_chain));
		if (tcbsys->sucide_flag) {
			//���E�����ꍇ
			GFL_TCBL * next = tcbsys->now_chain->next;
			DeleteTCB(tcbsys->now_chain);
			tcbsys->now_chain = next;
		} else {
			tcbsys->now_chain = tcbsys->now_chain->next;
		}
	}
	tcbsys->now_chain = NULL;

}

//------------------------------------------------------------------
/**
 * @brief	TCBL:�I������
 * @param	tcbsys	TCBL���䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFL_TCBL_SysExit( GFL_TCBLSYS * tcbsys )
{
	//���s����ׂ��^�X�N���c���Ă�����G���[�ɂ���
	if(tcbsys->tcb_first.next != &(tcbsys->tcb_first)) {
		OS_Printf("%08X != %08X\n", tcbsys->tcb_first.next, &(tcbsys->tcb_first));
		GF_ASSERT(0);
	}
	GFL_HEAP_FreeMemory(tcbsys->tcb_array);
	GFL_HEAP_FreeMemory(tcbsys);
}

//------------------------------------------------------------------
/**
 * @brief	TCBL�̍폜����
 * @param	tcb		TCBL�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GFL_TCBL_Delete(GFL_TCBL * tcb)
{
	GF_ASSERT(tcb->func != NULL);
	if (tcb->tcbsys->now_chain == tcb) {
		//���E�̏ꍇ�̓����N�̂Ȃ��ւ����V�X�e���ɔC����
		tcb->tcbsys->sucide_flag = TRUE;
		return;
	} else {
		//���E�̏ꍇ�͂��̏�ŏ���
		DeleteTCB(tcb);
	}
}


//------------------------------------------------------------------
/**
 * @brief	TCBL�̐�������
 * @param	tcbsys
 * @param	func
 * @param	work_size
 * @param	pri
 * @return	GFL_TCBL *
 */
//------------------------------------------------------------------
GFL_TCBL * GFL_TCBL_Create(GFL_TCBLSYS * tcbsys, GFL_TCBLFUNC * func, u32 work_size, u32 pri)
{
	GFL_TCBL * get;
	GFL_TCBL * find;
	GFL_TCBL * head = &(tcbsys->tcb_first);

	get = PopTCB(tcbsys);
	if (get == NULL) {
		//TCB������Ȃ��Ȃ����ꍇ
		return NULL;
	}
	get->pri = pri;
	get->func = func;
	if (work_size <= tcbsys->work_size) {
		get->work = NULL;
	} else {
		get->work = GFL_HEAP_AllocMemory(tcbsys->heap_id, work_size);
	}

	//���s���X�g�ɐڑ�(�����v���C�I���e�B�̎��ɂ́A��ɓo�^�����ق�����)
	for (find = head->next; find != head; find = find->next) {
		if (find->pri > get->pri) {
			get->prev = find->prev;
			get->next = find;
			find->prev->next = get;
			find->prev = get;
			return get;
		}
	}
	//�Ō�܂Ōq���Ƃ��낪������Ȃ���΍Ō�Ɍq��
	/*
	get->prev = head->prev;		//���܂ł̍Ō�
	get->next = head;			//�������Ō�
	head->prev->next = get;		//���܂ōŌゾ�����u���b�N�̎��͎���
	head->prev = get;			//���X�g�̍Ō���X�V
	*/
	get->prev = tcbsys->tcb_first.prev;	//���܂ł̍Ō�
	get->next = &(tcbsys->tcb_first);	//�������Ō�
	tcbsys->tcb_first.prev->next = get;	//���܂ōŌゾ�����u���b�N�̎��͎���
	tcbsys->tcb_first.prev = get;		//���X�g�̍Ō���X�V
	return get;
}

//------------------------------------------------------------------
/**
 * @brief	TCBL�̃��[�N�擾
 * @param	tcb		TCBL�ւ̃|�C���^
 * @return	void *
 */
//------------------------------------------------------------------
void * GFL_TCBL_GetWork(GFL_TCBL * tcb)
{
	return GetWork(tcb);
}

//------------------------------------------------------------------
/**
 * @brief
 * @param	tcb		TCBL�ւ̃|�C���^
 * @return	u32
 */
//------------------------------------------------------------------
u32 GFL_TCBL_GetPriority(const GFL_TCBL * tcb)
{
	return tcb->pri;
}

