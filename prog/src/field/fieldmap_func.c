//============================================================================================
/**
 * @file	fieldmap_func.c
 * @brief	�t�B�[���h�}�b�v��œ��삷�鐧��^�X�N�V�X�e��
 * @author	tamada GAME FREAK inc.
 * @date	2006.03.15
 * @date	2009.07.22	DS����ڐA	tomoya 
 *
 */
//============================================================================================

#include "gflib.h"

#include "fieldmap.h"

#include "fieldmap_func_def.h"
#include "fieldmap_func.h"

//============================================================================================
//============================================================================================
typedef struct {
	u32 dmy[8];			///<�ėp���[�N
}FREE_WORK;

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�p����^�X�N�V�X�e���̃V�X�e�����[�N��`
 */
//------------------------------------------------------------------
struct FLDMAPFUNC_SYS{
	FIELDMAP_WORK * fsys;		///<�t�B�[���h����V�X�e���ւ̃|�C���^
	int heapID;					///<�g�p����q�[�v��ID
	int max;					///<FLDMAPFUNC�̍ő吔
	FLDMAPFUNC_WORK * array;	///<FLDMAPFUNC�p���[�N�̔z��ւ̃|�C���^
	GFL_TCBSYS * mainsys;			///<���C���pTCB
	GFL_TCBSYS * drawsys;			///<�`��pTCB
};

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�p����^�X�N�̐��䃏�[�N��`
 */
//------------------------------------------------------------------
struct FLDMAPFUNC_WORK{
	FLDMAPFUNC_SYS * sys;			///<����V�X�e���ւ̃|�C���^
	GFL_TCB* tcb;					///<�ێ����Ă���TCB�ւ̃|�C���^
	GFL_TCB* drawtcb;				///<�ێ����Ă���`��TCB�ւ̃|�C���^
	const FLDMAPFUNC_DATA * data;	///<��`�f�[�^�ւ̃|�C���^
	void * free_work;				///<���ꂼ��Ŏg�p���郏�[�N�ւ̃|�C���^
  FSOverlayID ov_id;      ///< �I�[�o�[���CID
};

#define	DEFAULT_FREEWORK_SIZE	(sizeof(FREE_WORK))

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F�V�X�e�������A������
 * @param	fsys			�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	heapID			�g�p����q�[�v�̎w��
 * @param	max
 * @return	FLDMAPFUNC_SYS	�t�B�[���h�}�b�v����^�X�N�V�X�e���̃V�X�e�����[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
FLDMAPFUNC_SYS * FLDMAPFUNC_Sys_Create(FIELDMAP_WORK * fsys, HEAPID heapID, int max)
{
	FLDMAPFUNC_SYS * sys;
	u32 tcb_work_size;
	sys = GFL_HEAP_AllocClearMemory(heapID, sizeof(FLDMAPFUNC_SYS));
	sys->fsys = fsys;
	sys->heapID = heapID;
	sys->max = max;
	sys->array = GFL_HEAP_AllocClearMemory(heapID, sizeof(FLDMAPFUNC_WORK) * max);

	///�`��pTCB����
	///�Ȃ�fieldmap_func���`��pTCB�����̂��̗��R
	///�`��p��TCB��fieldmap_work.c/h�Ŏ����Ă��Ă��ǂ��ł����A
	///�`��pTCB�S�̂̔j��������Ƃ��ɖ�肪����܂��B
	///�X�̃^�X�N���Ń��������m�ۂ��Ă���ƁA
	///�S�̔j�����Ƀ��������[�N�������Ă��܂��܂��B
	///�ł��̂ŁA�j���������ʂɗp�ӂ���Ă���
	///fieldmap_func�̒��ɕ`�揈����ǉ�����
	///���������[�N�̋N����Ȃ��悤�ɂ��܂��B
	tcb_work_size = GFL_TCB_CalcSystemWorkSize( max );
	sys->drawsys = GFL_HEAP_AllocClearMemory( heapID, tcb_work_size );
	sys->drawsys = GFL_TCB_Init( max, sys->drawsys );

	sys->mainsys = GFL_HEAP_AllocClearMemory( heapID, tcb_work_size );
	sys->mainsys = GFL_TCB_Init( max, sys->mainsys );
	
	return sys;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F�V�X�e���I���A���
 * @return	sys		�t�B�[���h�}�b�v����^�X�N�V�X�e���̃V�X�e�����[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Sys_Delete(FLDMAPFUNC_SYS * sys)
{
	int i;
	for (i = 0; i < sys->max; i++) {
		FLDMAPFUNC_Delete(&sys->array[i]);
	}
	GFL_HEAP_FreeMemory(sys->array);
	GFL_HEAP_FreeMemory( sys->mainsys );		///<���C��TCB�j��
	GFL_HEAP_FreeMemory( sys->drawsys );		///<�`��TCB�j��
	GFL_HEAP_FreeMemory(sys);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�}�b�v����^�X�N�F���C��
 */
//-----------------------------------------------------------------------------
void FLDMAPFUNC_Sys_Main(FLDMAPFUNC_SYS * sys)
{
	// ���C���^�X�N���s
	GFL_TCB_Main( sys->mainsys );
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F�V�X�e��3D�`��
 * @return	sys		�t�B�[���h�}�b�v����^�X�N�V�X�e���̃V�X�e�����[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Sys_Draw3D(FLDMAPFUNC_SYS * sys)
{
	// �`��^�X�N���s
	GFL_TCB_Main( sys->drawsys );
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�̌Ăяo������
 * @param	tcb		�g�p����TCB�ւ̃|�C���^
 * @param	work	�g�p���郏�[�N�ւ̃|�C���^�i���̏ꍇ�A�K��FLDMAPFUNC_WORK�ւ̃|�C���^�j
 */
//------------------------------------------------------------------
static void FLDMAPFUNC_Tcb(GFL_TCB* tcb, void * work)
{
	FLDMAPFUNC_WORK * fwk = work;
	if( fwk->data->update_func ){
		fwk->data->update_func(fwk, fwk->sys->fsys, fwk->free_work);
	}
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�`��^�X�N�̌Ăяo������
 * @param	tcb		�g�p����TCB�ւ̃|�C���^
 * @param	work	�g�p���郏�[�N�ւ̃|�C���^�i���̏ꍇ�A�K��FLDMAPFUNC_WORK�ւ̃|�C���^�j
 */
//------------------------------------------------------------------
static void FLDMAPFUNC_DrawTcb(GFL_TCB* tcb, void * work)
{
	FLDMAPFUNC_WORK * fwk = work;
	if( fwk->data->draw3d_func ){
		fwk->data->draw3d_func(fwk, fwk->sys->fsys, fwk->free_work);
	}
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F����
 * @param	sys		�t�B�[���h�}�b�v����^�X�N�V�X�e���̃V�X�e�����[�N�ւ̃|�C���^
 * @param	data	�������悤�Ƃ��Ă��鐧��^�X�N�̒�`�f�[�^
 * @return	FLDMAPFUNC_WORK	������������^�X�N�̃��[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
FLDMAPFUNC_WORK * FLDMAPFUNC_Create(FSOverlayID ov_id, FLDMAPFUNC_SYS * sys, const FLDMAPFUNC_DATA * data)
{
	int i;
	FLDMAPFUNC_WORK * fwk;
	for (fwk = sys->array, i = 0; i < sys->max; fwk ++, i++) {
		if (fwk->tcb == NULL) {

      // Overlay�ǂݍ���
      if(ov_id != NO_OVERLAY_ID){
        GFL_OVERLAY_Load( ov_id );
      }
      fwk->ov_id = ov_id;
			fwk->tcb = GFL_TCB_AddTask(sys->mainsys, FLDMAPFUNC_Tcb, fwk, data->pri);
			fwk->drawtcb = GFL_TCB_AddTask(sys->drawsys, FLDMAPFUNC_DrawTcb, fwk, data->pri);
			fwk->sys = sys;
			fwk->data = data;
			GF_ASSERT(fwk->tcb != NULL);
			GF_ASSERT(fwk->drawtcb != NULL);
			if (data->work_size != 0) {
				fwk->free_work = GFL_HEAP_AllocClearMemory(sys->heapID, data->work_size);
			}
			if( data->create_func ){
				data->create_func(fwk, sys->fsys, fwk->free_work);
			}
			return fwk;
		}
	}
	GF_ASSERT_MSG(0, "FLDMAPFUNC:�V�K�ǉ��Ɏ��s���܂���\n");
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F����
 * @param	fwk		FLDMAPFUNC_WORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Delete(FLDMAPFUNC_WORK * fwk)
{
	if (fwk->tcb == NULL) {
		return;
	}
	//�ʏ��������Ăяo��
	if( fwk->data->delete_func ){
		fwk->data->delete_func(fwk, fwk->sys->fsys, fwk->free_work);	}
	if (fwk->data->work_size != 0) {
		GFL_HEAP_FreeMemory(fwk->free_work);
	}
	GFL_TCB_DeleteTask(fwk->tcb);
	GFL_TCB_DeleteTask(fwk->drawtcb);

  // Overlay�j��
  if(fwk->ov_id != NO_OVERLAY_ID){
		GFL_OVERLAY_Unload( fwk->ov_id );
  }
	GFL_STD_MemClear(fwk, sizeof(FLDMAPFUNC_WORK));
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v����^�X�N�F�t���[���[�N�|�C���^�擾
 * @param	fwk		FLDMAPFUNC_WORK�ւ̃|�C���^
 * @retval	�t���[���[�N�̃|�C���^
 *		�����t���O�𗧂Ă�ȂǍ��K�v�Ƃ���
 *		���ɂ��̊֐��ŁA���[�N���擾���邱�ƁB
 *		�����Ń��[�N���j�������̂ŁA�����ƕێ����Ȃ����ƁB
 */
//------------------------------------------------------------------
void * FLDMAPFUNC_GetFreeWork(FLDMAPFUNC_WORK * fwk)
{
	return fwk->free_work;
}

//------------------------------------------------------------------
//------------------------------------------------------------------


//------------------------------------------------------------------
//------------------------------------------------------------------
