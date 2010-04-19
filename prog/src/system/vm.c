//============================================================================================
/**
 * @file	vm.c
 * @brief	���z�C���^�[�v���^�}�V���@���C������(�X�N���v�g�R�}���h���s�Ɏg�p�����)
 * @author	tamada GAME FREAK inc.
 * @date	01.11.07
 *
 * 08.11.12	tamada	DP���玝���Ă��Ĕėp���E�g��������������悤�����J�n
 */
//============================================================================================

#include <gflib.h>
#include "system/vm.h"
#include "system/vm_cmd.h"


//============================================================================================
//	��`
//============================================================================================
#define	VM_ERROR(...)	GF_ASSERT_MSG(0, __VA_ARGS__)

// ���z�}�V���̓����Ԓ�`
typedef enum{
	VMSTAT_HALT,		///< ��~(����I���j
	VMSTAT_RUN,			///< ���쒆
	VMSTAT_WAIT,		///< �҂���ԁi�`�F�b�N���[�`���Ăяo���j
}VMSTATUS;

//���W�X�^��r�̌��ʒ�`
enum {
	MINUS_RESULT = 0,	///<��r���ʂ��}�C�i�X
	EQUAL_RESULT,		///<��r���ʂ��C�R�[��
	PLUS_RESULT			///<��r���ʂ��v���X
};

//============================================================================================
//
//	�O���[�o���ϐ�
//
//============================================================================================
//��������p�e�[�u��
static const u8 ConditionTable[6][3] =
{
//	  MINUS  EQUAL  PLUS
	{ TRUE,	 FALSE, FALSE },	// LT
	{ FALSE, TRUE,  FALSE },	// EQ
	{ FALSE, FALSE, TRUE  },	// GT
	{ TRUE,  TRUE,  FALSE },	// LE
	{ FALSE, TRUE,  TRUE  },	// GE
	{ TRUE,  FALSE, TRUE  }		// NE
};
//const u32 ScriptBreakPoint = 0;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���z�}�V�����[�N�̏�����
 */
//------------------------------------------------------------------
static void clear(VMHANDLE * core)
{
	core->adrs = NULL;
	core->cmp_flag = 0;
	core->status = VMSTAT_HALT;
	core->routine = NULL;
	//�X�^�b�N�N���A
	core->stackcount = 0;
	GFL_STD_MemFill(core->stack, 0, sizeof(VM_VALUE) * core->init_value.stack_size);
	//���W�X�^�N���A
	GFL_STD_MemFill(core->vm_register, 0, sizeof(VM_VALUE) * core->init_value.reg_size);
}

//============================================================================================
//
//
// ���z�}�V������p�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
VMHANDLE * VM_Create(HEAPID heapID, const VM_INITIALIZER * init)
{
	VMHANDLE * core;
	u32 size;
	u8 * ptr;

	size = sizeof(VMHANDLE)
		+ sizeof(u32) * init->reg_size
		+ sizeof(u32) * init->stack_size;
	core = GFL_HEAP_AllocMemory(heapID, size);
	GFL_STD_MemFill(core, 0, size);
	core->init_value = * init;
	ptr = (u8*)core;
	core->vm_register = (u32 *)(ptr + sizeof(VMHANDLE));
	core->stack = (u32*)(ptr + sizeof(VMHANDLE) + sizeof(u32) * init->reg_size);

	return core;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void VM_Delete(VMHANDLE * core)
{
	GFL_HEAP_FreeMemory(core);
}

//------------------------------------------------------------------
/**
 * ���z�}�V��������
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	cmd_tbl		���߃e�[�u���J�n�A�h���X
 * @param	cmd_max		���߃e�[�u���̑傫��
 *
 * @return	none
 */
//------------------------------------------------------------------
void VM_Init(VMHANDLE * core, void * context_work)
{
	clear(core);
	core->context = context_work;
}

//------------------------------------------------------------------
/**
 * ���z�}�V���ɃR�[�h��ݒ�
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	start		���s�R�[�h�̊J�n�A�h���X
 *
 * @return	���TRUE
 */
//------------------------------------------------------------------
void VM_Start( VMHANDLE * core, const VM_CODE * start )
{
	GF_ASSERT(core->status == VMSTAT_HALT);
	clear(core);
	core->adrs = start;
	core->status = VMSTAT_RUN;
}

//------------------------------------------------------------------
/**
 * ���z�}�V�����s�I��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	none
 */
//------------------------------------------------------------------
void VM_End( VMHANDLE * core )
{
	core->status = VMSTAT_HALT;
	core->adrs = NULL;
}

//------------------------------------------------------------------
/**
 * ���z�}�V�����䃁�C��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval	"TRUE = ���s��"
 * @retval	"FALSE = ��~���E���s�I��"
 */
//------------------------------------------------------------------
BOOL VM_Control( VMHANDLE * core )
{
	u16 code;

	switch( (VMSTATUS)core->status ){
	case VMSTAT_HALT:
		return FALSE;

	case VMSTAT_WAIT:
		if( core->routine ){
			if( core->routine(core, core->context) == TRUE ){
				core->routine = NULL;
				core->status = VMSTAT_RUN;
			}
			return TRUE;
		} else {
			VM_ERROR("vm wait routine error\n");
			core->status = VMSTAT_RUN;
		}
		/* FALL THROUGH */

	case VMSTAT_RUN:
		while( TRUE ){
			if( core->adrs == NULL ){
				VM_ERROR("vm adrs null error\n");
				core->status = VMSTAT_HALT;
				return FALSE;
			}
/*	�f�o�b�O����
			if( core->adrs == (VM_CODE *)ScriptBreakPoint ){
				while( TRUE ){
					Halt();
				}
			}
*/

			code = VMGetU16( core );

			if( code >= core->init_value.command_max ) {
				VM_ERROR("vm command error %04x:%08x\n",code, core->adrs - 2);
				core->status = VMSTAT_HALT;
				return FALSE;
			}
			if( core->init_value.command_table[code]( core, core->context )
					== VMCMD_RESULT_SUSPEND ){
				break;
			}
		}
		break;
	default:
		VM_ERROR("vm error status %02x\n", core->status);
		core->status = VMSTAT_HALT;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * ���z�}�V���R���e�L�X�g�擾
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	void *		���z�}�V�����ێ����Ă���R���e�L�X�g�|�C���^��Ԃ�
 *
 * �R���e�L�X�g��VM_Init�ŗ^��������
 *
 */
//------------------------------------------------------------------
void * VM_GetContext( VMHANDLE * core)
{
	return core->context;
}

//============================================================================================
// ���z�}�V������p�T�u���[�`��
//============================================================================================

//------------------------------------------------------------------
/**
 * adrs�̃A�h���X����16bit(2byte)�f�[�^�擾
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	�擾�f�[�^
 */
//------------------------------------------------------------------
u16 VMGetU16( VMHANDLE * core )
{
	u16	val;

	val = (u16)VMGetU8( core );
	val += (u16)VMGetU8( core ) << 8;

	return val;
}

//------------------------------------------------------------------
/**
 * adrs�̃A�h���X����32bit(4byte)�f�[�^�擾
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	�擾�f�[�^
 */
//------------------------------------------------------------------
u32 VMGetU32( VMHANDLE * core )
{
	u32	val;
	u8	a,b,c,d;

	a = VMGetU8( core );
	b = VMGetU8( core );
	c = VMGetU8( core );
	d = VMGetU8( core );

	val = 0;
	val += (u32)d;
	val <<= 8;
	val += (u32)c;
	val <<= 8;
	val += (u32)b;
	val <<= 8;
	val += (u32)a;

	return val;
}

//--------------------------------------------------------------------------------------------
/**
 * �Q�̒l���r
 *
 * @param	r1		�l�P
 * @param	r2		�l�Q
 *
 * @retval	"r1 < r2 : MISUS_RESULT"
 * @retval	"r1 = r2 : EQUAL_RESULT"
 * @retval	"r1 > r2 : PLUS_RESULT"
 */
//--------------------------------------------------------------------------------------------
void VMCompare(VMHANDLE * core, VM_VALUE r1, VM_VALUE r2 )
{
	if( r1 < r2 ){
		core->cmp_flag = MINUS_RESULT;
	}else if( r1 == r2 ){
		core->cmp_flag = EQUAL_RESULT;
	}else {
		core->cmp_flag = PLUS_RESULT;
	}
}

//============================================================================================
//============================================================================================

//------------------------------------------------------------------
/**
 * ���z�}�V���X�^�b�N�v�b�V��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	val
 *
 * @retval	"0 = ����"
 * @retval	"1 = �G���["
 */
//------------------------------------------------------------------
void VMCMD_Push( VMHANDLE * core, VM_VALUE value)
{
	if( core->stackcount + 1 >= core->init_value.stack_size ){
		VM_ERROR("vm stack overflow(%d)\n",core->stackcount);
		return;
	}
	core->stack[core->stackcount] = value;
	core->stackcount ++;
}

//------------------------------------------------------------------
/**
 * ���z�}�V���X�^�b�N�|�b�v
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return
 */
//------------------------------------------------------------------
VM_VALUE VMCMD_Pop( VMHANDLE * core )
{
	if( core->stackcount == 0 ){
		VM_ERROR("vm stack pop error\n");
		return NULL;
	}
	core->stackcount --;

	return core->stack[core->stackcount];
}

//------------------------------------------------------------------
/**
 * ���z�}�V���W�����v����
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	adrs		�W�����v��A�h���X
 *
 * @return	none
 */
//------------------------------------------------------------------
void VMCMD_Jump( VMHANDLE * core, const VM_CODE * adrs )
{
	core->adrs = adrs;
}

//------------------------------------------------------------------
/**
 * ���z�}�V���R�[������
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	adrs		�Ăяo���A�h���X
 *
 * @return	none
 */
//------------------------------------------------------------------
void VMCMD_Call( VMHANDLE * core, const VM_CODE * adrs )
{
	VMCMD_Push( core, (VM_VALUE)core->adrs );
	core->adrs = adrs;
}

//------------------------------------------------------------------
/**
 * ���z�}�V�����^�[������
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	none
 */
//------------------------------------------------------------------
void VMCMD_Return( VMHANDLE * core )
{
	core->adrs = (const VM_CODE *)VMCMD_Pop( core );
}

//------------------------------------------------------------------
/**
 * ���z�}�V�����E�F�C�g��Ԃɐݒ�
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param	func		�E�F�C�g�֐�
 *
 * @return	none
 *
 * @li	TRUE���Ԃ��Ă���܂ŃE�F�C�g�֐��𖈉�Ăт���
 */
//------------------------------------------------------------------
void VMCMD_SetWait( VMHANDLE * core, VM_WAIT_FUNC func )
{
	core->status = VMSTAT_WAIT;
	core->routine = func;
}

//============================================================================================
//============================================================================================


