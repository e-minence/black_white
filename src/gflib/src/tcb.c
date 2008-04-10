//=============================================================================================
/**
 * @file	tcb.c
 * @brief	�ėp�^�X�N���䃂�W���[���{��
 * @author	tamada, taya	(GAME FREAK Inc.)
 * @date	2006.11.06
 * @since	2003.01.20
 */
//=============================================================================================

#include <nitro.h>
#include <nnsys.h>
#include "include/gflib.h"
#include "include/tcb.h"
//#include "include/heapsys.h"

//=============================================================================================
//	
//=============================================================================================
#define TASK_ADDPROC_CHECK		///< ���̃t���O���L�����ƁATCB_Add ���쒆�Ɋ��荞�݂�TCB_Main���Ă΂ꂽ���A�������s��Ȃ��B



//=============================================================================================
//	��`
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum{
	TCB_ENABLE = 0,
	TCB_DISABLE = 1,
};

//------------------------------------------------------------------
/** @brief	TCB�\���̂̒�`
 *
 * TCB�\���̂́A����D�揇�ʂɏ]���ă����N�����B
 * ���̂��ߑo���������N���\������悤�ɑO��ւ̃|�C���^�����B
 *
 * �܂��A���ꂼ��̎��ۂ̏����ɕK�v�ȃ��[�N�G���A�͎������A
 * TCB�o�^���Ɏg�p���郏�[�N�G���A�������Ƃ��ēn���B
 *
 * ���[�U�[���x���ł͎Q�Ƃ���K�v�̂Ȃ������o�ւ̃A�N�Z�X�𐧌����邽�߁A
 * ���̍\���̂̓��W���[�������炵���A�N�Z�X�ł��Ȃ��B
 * �K�v�ȕ����̓A�N�Z�X�֐����o�R����悤�ɂȂ��Ă���B
 */
//------------------------------------------------------------------
struct _GFL_TCB {
	GFL_TCBSYS* sys;				///<�������Ǘ����Ă���TCB�V�X�e���ւ̃|�C���^
	GFL_TCB * prev;				///<�O��TCB�ւ̃|�C���^
	GFL_TCB * next;				///<����TCB�ւ̃|�C���^
	u32 pri;					///<�v���C�I���e�B
	void * work;				///<���[�N�G���A�ւ̃|�C���^
	GFL_TCB_FUNC * func;			///<���s�֐��ւ̃|�C���^

	u32 sw_flag;				///<�֐��ǉ��t���O
};	// 52 bytes


//------------------------------------------------------------------
/**
 * @brief	TCB����\���̂̒�`
 */
//------------------------------------------------------------------
struct _GFL_TCBSYS {
	u32			tcb_max;		///< �o�^�\��TCB�̍ő吔
	u32			tcb_stack_ptr;	///< ���M�������Ă���TCB�p�X�^�b�N�̃|�C���^
	GFL_TCB			tcb_first;		///< TCB�擪����
	GFL_TCB **	tcb_stack;		///< TCB�p�X�^�b�N
	GFL_TCB*		tcb_table;		///< TCB���̃e�[�u��
	BOOL		adding_flag;	///< TCB�ǉ��������s���t���O�i���̃t���O�������Ă���Ԃ�Main�����s���Ȃ��j


	GFL_TCB *		now_chain;		///< ���C�����[�v������g�p�A�������ێ�TCB�|�C���^
	GFL_TCB *		next_chain;		///< ���C�����[�v������g�p�A�������ێ�TCB�|�C���^
};	// 68 bytes


//==============================================================
// Prototype
//==============================================================
static inline void TCB_WorkClear( GFL_TCBSYS* tcbsys, GFL_TCB * tcb);
static void InitTCBStack( GFL_TCBSYS* tcbsys );
static GFL_TCB * PopTCB( GFL_TCBSYS* tcbsys );
static BOOL PushTCB( GFL_TCBSYS* tcbsys, GFL_TCB * tcb );
static GFL_TCB * AddTask( GFL_TCBSYS* tcbsys, GFL_TCB_FUNC * func, void* work, u32 pri );

//------------------------------------------------------------------
/**
 *	TCB�V�X�e���g�p�O�̏�����
 */
//------------------------------------------------------------------
static void TCB_Init( GFL_TCBSYS* tcbsys );



//=============================================================================================
//
//	���[�J���֐�
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief	TCB�\���̂̏�����
 *
 * TCB�̃����o�ϐ�������������
 *
 * @param	tcb	TCB�ւ̃|�C���^
 * @return	none	
 */
//------------------------------------------------------------------
static inline void TCB_WorkClear( GFL_TCBSYS* tcbsys, GFL_TCB * tcb)
{
	tcb->sys = tcbsys;

	tcb->prev = 
	tcb->next = &(tcbsys->tcb_first);

	tcb->pri = 0;
	tcb->work = NULL;
	tcb->func = NULL;
}
//------------------------------------------------------------------
/**
 * @brief	TCB�X�^�b�N������
 *
 * ���g�pTCB��ێ�����X�^�b�N������������
 */
//------------------------------------------------------------------
static void InitTCBStack( GFL_TCBSYS* tcbsys )
{
	int i;

	for( i=0; i<tcbsys->tcb_max; i++ )
	{
		TCB_WorkClear( tcbsys, &(tcbsys->tcb_table[i]) );
		tcbsys->tcb_stack[i] = tcbsys->tcb_table + i;
	}
	tcbsys->tcb_stack_ptr = 0;
}

//------------------------------------------------------------------
/**
 * @brief	TCB�X�^�b�N����̎��o��
 *
 * @retval	NULL�ȊO	TCB�ւ̃|�C���^
 * @retval	NULL		���o���Ɏ��s�i�X�^�b�N���󂾂����ꍇ�j
 */
//------------------------------------------------------------------
static GFL_TCB * PopTCB( GFL_TCBSYS* tcbsys )
{
	GFL_TCB * tcb;
	if(tcbsys->tcb_stack_ptr == tcbsys->tcb_max)
	{
		return NULL;
	}
	tcb = tcbsys->tcb_stack[ tcbsys->tcb_stack_ptr ];
	tcbsys->tcb_stack_ptr++;
	return tcb;
}

//------------------------------------------------------------------
/**
	@brief	TCB�X�^�b�N�ւ̃v�b�V��

	@param	tcb		�v�b�V������TCB�̃|�C���^
	@retval	TRUE	����
	@retval	FALSE	���s�i�X�^�b�N�������ς��̏ꍇ�j
*/
//------------------------------------------------------------------
static BOOL PushTCB( GFL_TCBSYS* tcbsys, GFL_TCB * tcb )
{
	if(tcbsys->tcb_stack_ptr == 0)
	{
		return FALSE;
	}
	TCB_WorkClear( tcbsys, tcb );	//�l���N���A���Ă���X�^�b�N�ɐς�
	tcbsys->tcb_stack_ptr--;
	tcbsys->tcb_stack[ tcbsys->tcb_stack_ptr ] = tcb;

	return TRUE;
}


//=============================================================================================
//
//	���J�֐�
//
//=============================================================================================

//------------------------------------------------------------------
/**
 * TCB �V�X�e�����\�z����̂ɕK�v�ȃ������ʂ��v�Z
 *
 * @param   task_max		�ғ��ł���ő�^�X�N��
 *
 * @retval  u32		�������T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
u32 GFL_TCB_CalcSystemWorkSize( u32 task_max )
{
	return sizeof(GFL_TCBSYS) + (sizeof(GFL_TCB *)+sizeof(GFL_TCB)) * task_max;
}

//------------------------------------------------------------------
/**
 * TCB�V�X�e�����쐬
 *
 * @param   task_max		�ғ��ł���ő�^�X�N��
 * @param   work_area		�V�X�e���쐬�ɕK�v�[���ȃT�C�Y�̃��[�N�G���A�A�h���X
 *
 * @retval  GFL_TCBSYS*		�쐬���ꂽTCB�V�X�e���|�C���^
 *
 * work_area �ɕK�v�ȃT�C�Y�́AGFL_TCB_CalcSystemWorkSize �Ōv�Z����B
 */
//------------------------------------------------------------------
GFL_TCBSYS*  GFL_TCB_Init( u32 task_max, void* work_area )
{
	GFL_TCBSYS* tcbsys;

	GF_ASSERT( work_area );

	tcbsys = work_area;

	tcbsys->tcb_stack = (GFL_TCB **) ((u8*)(tcbsys) + sizeof(GFL_TCBSYS));
	tcbsys->tcb_table = (GFL_TCB*) ((u8*)(tcbsys->tcb_stack) + sizeof(GFL_TCB *)*task_max);

	tcbsys->tcb_max = task_max;
	tcbsys->tcb_stack_ptr = 0;
	tcbsys->adding_flag = FALSE;

	TCB_Init( tcbsys );

	return tcbsys;
}

//------------------------------------------------------------------
/**
 *	�V�X�e���g�p�O�̏�����
 */
//------------------------------------------------------------------
static void TCB_Init( GFL_TCBSYS* tcbsys )
{
	//�X�^�b�N�̏�����
	InitTCBStack( tcbsys );
	//�擪�u���b�N�̏�����
	TCB_WorkClear( tcbsys, &tcbsys->tcb_first );

	tcbsys->now_chain = tcbsys->tcb_first.next;
}

//------------------------------------------------------------------
//	TCB���C��
//------------------------------------------------------------------
void GFL_TCB_Main( GFL_TCBSYS* tcbsys )
{
	#ifdef TASK_ADDPROC_CHECK
	if( tcbsys->adding_flag )
	{
		return;
	}
	#endif

	tcbsys->now_chain = tcbsys->tcb_first.next;	//���������s�̎n�߂̏ꏊ
	while (tcbsys->now_chain != &(tcbsys->tcb_first))
	{
		//���s�֐����Ńu���b�N���폜���ꂽ���p�ɃA�h���X��ۑ�
		tcbsys->next_chain = tcbsys->now_chain->next;

		if(tcbsys->now_chain->sw_flag == TCB_ENABLE){//�o�^����̓���������
			if(tcbsys->now_chain->func != NULL){
				tcbsys->now_chain->func(tcbsys->now_chain, tcbsys->now_chain->work);
			}
		}else{
			tcbsys->now_chain->sw_flag = TCB_ENABLE;
		}
		tcbsys->now_chain = tcbsys->next_chain;	//���̘A����
	}
	tcbsys->now_chain->func = NULL;
}

//------------------------------------------------------------------
/**
 *	@brief	TCB�V�X�e���I��
 *  @param	tcbsys		TCB�V�X�e�����[�N�|�C���^
 */
//------------------------------------------------------------------
void GFL_TCB_Exit( GFL_TCBSYS* tcbsys )
{
#if 0
	tcbsys->now_chain = tcbsys->tcb_first.next;	//���������s�̎n�߂̏ꏊ
	while (tcbsys->now_chain != &(tcbsys->tcb_first))
	{
		//���s�֐����Ńu���b�N���폜���ꂽ���p�ɃA�h���X��ۑ�
		tcbsys->next_chain = tcbsys->now_chain->next;

		if(tcbsys->now_chain->sw_flag == TCB_ENABLE){//�o�^����̓���������
			if(tcbsys->now_chain->func != NULL){
				tcbsys->now_chain->func(tcbsys->now_chain, tcbsys->now_chain->work);
			}
		}else{
			tcbsys->now_chain->sw_flag = TCB_ENABLE;
		}
		tcbsys->now_chain = tcbsys->next_chain;	//���̘A����
	}
	tcbsys->now_chain->func = NULL;
#endif
}

//------------------------------------------------------------------------------
/**
	@brief	TCB��ǉ�����
	@param	tcbsys	TCB�V�X�e���|�C���^
	@param	func	GFL_TCB_FUNC:�֘A�t������s�֐��|�C���^
	@param	work	void*:�֘A�t���郏�[�N�G���A�ւ�void�^�|�C���^
	@param	pri		u32:�^�X�N�v���C�I���e�B

	@return	TCB *	�ǉ�����TCB�������|�C���^
*/
//------------------------------------------------------------------------------
GFL_TCB * GFL_TCB_AddTask( GFL_TCBSYS* tcbsys, GFL_TCB_FUNC * func, void* work, u32 pri )
{
	GFL_TCB *  ret;

	tcbsys->adding_flag = TRUE;
	ret = AddTask( tcbsys, func, work, pri );
	tcbsys->adding_flag = FALSE;
	return ret;

}
//------------------------------------------------------------------
/**
 * TCB�ǉ�����
 *
 * @param   tcbsys		TCB�V�X�e���|�C���^
 * @param   func		TCB�Ɋ֘A�t����֐��|�C���^
 * @param   work		TCB�Ɋ֘A�t���郏�[�N�G���A�ւ̃|�C���^
 * @param   pri			�^�X�N�v���C�I���e�B
 *
 * @retval  GFL_TCB *		�ǉ�����TCB�|�C���^
 */
//------------------------------------------------------------------
static GFL_TCB * AddTask( GFL_TCBSYS* tcbsys, GFL_TCB_FUNC * func, void* work, u32 pri )
{
	GFL_TCB * get;
	GFL_TCB * find;

	get = PopTCB( tcbsys );	//�󂢂Ă���TCB���E��
	if(get == NULL)
	{
		//�o�^���悤�Ƃ������^�X�N���Ȃ�����
		OS_TPrintf("ERR : TCB wasn't get!");
		return NULL;
	}

	//�p�����[�^���Z�b�g
	get->pri = pri;
	get->work = work;
	get->func = func;
	//����t���O�ݒ�
	if(tcbsys->now_chain->func != NULL){
		//����t���O�ݒ�(�ʂ̃^�X�N����ǉ�����悤�Ƃ��Ă���ꍇ�A�o�^����̓��������鏈��)
		if(tcbsys->now_chain->pri <= pri){
			get->sw_flag = TCB_DISABLE;
		}else{
			get->sw_flag = TCB_ENABLE;
		}
	}
	else{
		//�{�^�X�N�O����̓o�^
		get->sw_flag = TCB_ENABLE;
	}

	//���s���X�g�ɐڑ�(�����v���C�I���e�B�̎��ɂ́A��ɓo�^�����ق�����)
	for(find = tcbsys->tcb_first.next; find != &(tcbsys->tcb_first); find = find->next)
	{
		if(find->pri > get->pri){	//find�̑O��get���Ȃ���
			get->prev = find->prev;
			get->next = find;
			find->prev->next = get;
			find->prev = get;
			if(find == tcbsys->next_chain){
				tcbsys->next_chain = get;
			}
			return get;
		}
	}
	//�Ō�܂Ōq���Ƃ��낪������Ȃ���΍Ō�Ɍq��
	if(tcbsys->next_chain == &(tcbsys->tcb_first)){
		tcbsys->next_chain = get;
	}
	get->prev = tcbsys->tcb_first.prev;	//���܂ł̍Ō�
	get->next = &(tcbsys->tcb_first);	//�������Ō�
	tcbsys->tcb_first.prev->next = get;	//���܂ōŌゾ�����u���b�N�̎��͎���
	tcbsys->tcb_first.prev = get;		//���X�g�̍Ō���X�V
	return get;
}

//------------------------------------------------------------------------------
/**
	@brief	TCB����������
	@param	tcb		TCB�|�C���^
*/
//------------------------------------------------------------------------------
void GFL_TCB_DeleteTask( GFL_TCB * tcb )
{
	//�A���V�X�e���̎��̃����N�悪�폜�Ώۂ̏ꍇ�̏���
	if(tcb->sys->next_chain == tcb){
		tcb->sys->next_chain = tcb->next;	//�폜�Ώ�TCB�̎�TCB�֘A�������ړ�
	}
	//���X�g����폜
	tcb->prev->next = tcb->next;
	tcb->next->prev = tcb->prev;

	PushTCB( tcb->sys, tcb );
}


//==================================================================
//	TCB�A�N�Z�X�֐�
//==================================================================

//------------------------------------------------------------------
//TCB�̓���֐���؂�ւ���
//------------------------------------------------------------------
void GFL_TCB_ChangeFunc(GFL_TCB * tcb, GFL_TCB_FUNC * func)
{
	tcb->func = func;
}

//------------------------------------------------------------------
//TCB�̃��[�N�A�h���X�擾
//------------------------------------------------------------------
void * GFL_TCB_GetWork(GFL_TCB * tcb)
{
	return tcb->work;
}

//------------------------------------------------------------------
//TCB�v���C�I���e�B�̎擾
//------------------------------------------------------------------
u32 GFL_TCB_GetPriority(const GFL_TCB * tcb)
{
	return tcb->pri;
}







