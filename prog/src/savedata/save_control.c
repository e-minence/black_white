//==============================================================================
/**
 * @file	save_control.c
 * @brief	�Z�[�u�R���g���[���֘A
 * @author	matsuda
 * @date	2008.08.28(��)
 */
//==============================================================================
#include <gflib.h>
#include <backup_system.h>
#include "system/main.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�Z�[�u�n�̊֐����Ńe���|�����Ƃ��Ďg�p����q�[�v��ID
#define HEAPID_SAVE_TEMP        (GFL_HEAPID_APP)

//==============================================================================
//	�\���̒�`
//==============================================================================
///�Z�[�u�f�[�^�Ǘ����[�N�\����
struct _SAVE_CONTROL_WORK{
	BOOL new_data_flag;			///<TRUE:�V�K�f�[�^
	BOOL data_exists;			///<�f�[�^�����݂��邩�ǂ���
	BOOL total_save_flag;		///<TRUE:�S�̃Z�[�u
	u32 first_status;			///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����(bit�w��)
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���p�Z�[�u�f�[�^�ւ̃|�C���^
	GFL_SAVEDATA *sv_extra[SAVE_EXTRA_ID_MAX];
};

//==============================================================================
//	���[�J���ϐ�
//==============================================================================
///�Z�[�u�f�[�^�ւ̃|�C���^
static SAVE_CONTROL_WORK *SaveControlWork = NULL;


//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���̏�����
 */
//---------------------------------------------------------------------------
SAVE_CONTROL_WORK * SaveControl_SystemInit(int heap_id)
{
	SAVE_CONTROL_WORK *ctrl;
	LOAD_RESULT load_ret;

	ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(SAVE_CONTROL_WORK));
	SaveControlWork = ctrl;
	ctrl->new_data_flag = TRUE;			//�V�K�f�[�^
	ctrl->total_save_flag = TRUE;		//�S�̃Z�[�u
	ctrl->data_exists = FALSE;			//�f�[�^�͑��݂��Ȃ�
	ctrl->sv_normal = GFL_SAVEDATA_Create(&SaveParam_Normal, heap_id);


	//�f�[�^���݃`�F�b�N���s���Ă���
	load_ret = GFL_BACKUP_Load(ctrl->sv_normal, HEAPID_SAVE_TEMP);
	ctrl->first_status = 0;
	switch(load_ret){
	case LOAD_RESULT_OK:				///<�f�[�^����ǂݍ���
		ctrl->total_save_flag = FALSE;	//�S�̃Z�[�u�̕K�v�͂Ȃ�
										//NG�̏ꍇ��TRUE�̂܂܂Ȃ̂őS�̃Z�[�u�ɂȂ�
		//break;
		/* FALL THROUGH */
	case LOAD_RESULT_NG:				///<�f�[�^�ُ�
		//�܂Ƃ��ȃf�[�^������悤�Ȃ̂œǂݍ��ށ@OK or NG(����ǂݏo�����~���[�ǂݏo�����o����)
		ctrl->new_data_flag = FALSE;		//�V�K�f�[�^�ł͂Ȃ�
		ctrl->data_exists = TRUE;			//�f�[�^�͑��݂���
		if(load_ret == LOAD_RESULT_NG){	//OK��NG������������ʂ�̂ŉ��߂ă`�F�b�N
			ctrl->first_status |= FST_NORMAL_NG_BIT;
		}

	#if 0	//��check�@�O�����o����܂Ō��
		//�O���Z�[�u�̑��݃`�F�b�N
		{
			LOAD_RESULT frontier_result, video_result;
			
			ExtraNewCheckLoadData(sv, &frontier_result, &video_result);
			if(frontier_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= FST_FRONTIER_BREAK_BIT;
			}
			else if(frontier_result == LOAD_RESULT_NG){
				ctrl->first_status |= FST_FRONTIER_NG_BIT;
			}
			if(video_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= FST_VIDEO_BREAK_BIT;
			}
			else if(video_result == LOAD_RESULT_NG){
				ctrl->first_status |= FST_VIDEO_NG_BIT;
			}
		}
	#endif
		break;
	case LOAD_RESULT_BREAK:			///<�j��A�����s�\
		OS_TPrintf("LOAD:�f�[�^�j��\n");
		ctrl->first_status |= FST_NORMAL_BREAK_BIT;
		//break;
		/* FALL THROUGH */
	case LOAD_RESULT_NULL:		///<�f�[�^�Ȃ�
	case LOAD_RESULT_ERROR:			///<�@��̏�(Flash������)�Ȃǂœǂݎ��s�\
		//�V�K or �f�[�^�j��Ȃ̂ŃN���A����
		OS_TPrintf("LOAD:�f�[�^�����݂��Ȃ�\n");
		GFL_SAVEDATA_Clear(ctrl->sv_normal);
		break;
	default:
		GF_ASSERT("LOAD:��O�G���[�I");
		break;
	}

	OS_TPrintf("first_status = %d\n", ctrl->first_status);
	
#if 0	//��check
#ifdef	PM_DEBUG
	{
		int i, rest;
		for (i = 0; i < SVBLK_ID_MAX; i++) {
			OS_TPrintf("[%d] ",i);
			OS_TPrintf("(%04x x %02d) - %05x = %05x\n",
					SEC_DATA_SIZE,sv->blkinfo[i].use_sec,
					sv->blkinfo[i].size,
					SEC_DATA_SIZE * sv->blkinfo[i].use_sec - sv->blkinfo[i].size);
		}
		rest = SECTOR_MAX - GetTotalSector(sv);
		if (rest > 0) {
			OS_TPrintf("%2d sector(0x%05x) left.\n", rest, 0x1000 * rest);
		}
	}
#endif
#endif

	return ctrl;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^�擾
 * @return	SAVE_CONTROL_WORK	�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 *
 * ��{�I�ɂ̓Z�[�u�f�[�^�ւ̃O���[�o���Q�Ƃ͔��������B���̂��߁A���̊֐���
 * �g�p����ӏ��͌��d�ɐ�������Ȃ���΂Ȃ�Ȃ��B�ł���΃v���O���}���[�_�[��
 * �����Ȃ���Ύg�p�ł��Ȃ��悤�ɂ������B
 * �ςȃA�N�Z�X��������C����������܂��B�g�p���@�ɂ͒��ӂ��Ă��������B
 */
//---------------------------------------------------------------------------
SAVE_CONTROL_WORK * SaveControl_GetPointer(void)
{
	GF_ASSERT(SaveControlWork != NULL);
    OS_TPrintf("%x\n",(int)SaveControlWork);
	return SaveControlWork;
}

//--------------------------------------------------------------
/**
 * @brief   �ʏ�f�[�^�̃��[�h
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  ���[�h����
 */
//--------------------------------------------------------------
LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl)
{
	LOAD_RESULT result = GFL_BACKUP_Load(ctrl->sv_normal, HEAPID_SAVE_TEMP);
	
	switch(result){
	case LOAD_RESULT_OK:
	case LOAD_RESULT_NG:
		ctrl->data_exists = TRUE;
		break;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   �ʏ�f�[�^�̃Z�[�u
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl)
{
	SAVE_RESULT result;
	
	result = GFL_BACKUP_Save(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
		ctrl->new_data_flag = FALSE;
		ctrl->data_exists = TRUE;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   �ʏ�f�[�^�̕����Z�[�u������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl)
{
	GFL_BACKUP_SAVEASYNC_Init(ctrl->sv_normal);
}

//--------------------------------------------------------------
/**
 * @brief   �ʏ�f�[�^�̕����Z�[�u���C������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl)
{
	SAVE_RESULT result;
	
	result = GFL_BACKUP_SAVEASYNC_Main(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
		ctrl->new_data_flag = FALSE;
		ctrl->data_exists = TRUE;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   �ʏ�Z�[�u�f�[�^�̊e�Z�[�u���[�N�̃|�C���^���擾����
 *
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   gmdata_id	�擾�������Z�[�u�f�[�^��ID
 *
 * @retval  �Z�[�u�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------
void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id)
{
	return GFL_SAVEDATA_Get(ctrl->sv_normal, gmdata_id);
}

//--------------------------------------------------------------
/**
 * @brief   �V�K�f�[�^�t���O���擾����
 *
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�V�K�f�[�^�ł���
 */
//--------------------------------------------------------------
BOOL SaveControl_NewDataFlagGet(SAVE_CONTROL_WORK *ctrl)
{
	return ctrl->new_data_flag;
}

//---------------------------------------------------------------------------
/**
 * @brief	�ŏ��̓ǂݍ��݌��ʂ�Ԃ�
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	LOAD_RESULT	�ǂݍ��݌��ʁisavedata_def.h�Q�Ɓj
 */
//---------------------------------------------------------------------------
u32 SaveControl_GetLoadResult(const SAVE_CONTROL_WORK * sv)
{
	return sv->first_status;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���݃t���O���擾
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�Z�[�u�f�[�^�����݂���
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetExistFlag(const SAVE_CONTROL_WORK * sv)
{
	return sv->data_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̏�����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 *
 * SaveControl_Erase�ƈႢ�A�t���b�V���ɏ������܂Ȃ��B
 * �Z�[�u�f�[�^�������ԂŁu�������傩��v�V�ԏꍇ�Ȃǂ̏���������
 */
//---------------------------------------------------------------------------
void SaveControl_ClearData(SAVE_CONTROL_WORK * ctrl)
{
	ctrl->new_data_flag = TRUE;				//�V�K�f�[�^�ł���
	ctrl->total_save_flag = TRUE;			//�S�̃Z�[�u����K�v������
	GFL_SAVEDATA_Clear(ctrl->sv_normal);
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^�̏���
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl)
{
	GFL_BACKUP_Erase(ctrl->sv_normal, HEAPID_SAVE_TEMP);
}

//==================================================================
/**
 * �O���Z�[�u�f�[�^�̃��[�h
 *
 * @param   ctrl		    �Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @param   heap_id		  �ǂݍ��񂾃f�[�^�����̃q�[�v�ɓW�J
 *
 * @retval  LOAD_RESULT		���[�h����
 * 
 * �g�p���I�������K��SaveControl_Extra_Unload�ŉ�����Ă�������
 */
//==================================================================
LOAD_RESULT SaveControl_Extra_Load(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, int heap_id)
{
	LOAD_RESULT load_ret;

  GF_ASSERT(ctrl->sv_extra[extra_id] == NULL);
	ctrl->sv_extra[extra_id] = GFL_SAVEDATA_Create(&SaveParam_ExtraTbl[extra_id], heap_id);
	load_ret = GFL_BACKUP_Load(ctrl->sv_extra[extra_id], heap_id);
	
	OS_TPrintf("�O���Z�[�u���[�h���� extra_id = %d, load_ret = %d\n", extra_id, load_ret);
	return load_ret;
}

//==================================================================
/**
 * �O���Z�[�u�f�[�^�̉��
 *
 * @param   ctrl		
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 */
//==================================================================
void SaveControl_Extra_Unload(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
  GFL_SAVEDATA_Delete(ctrl->sv_extra[extra_id]);
  ctrl->sv_extra[extra_id] = NULL;
}

//--------------------------------------------------------------
/**
 * @brief   �O���f�[�^�̕����Z�[�u������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
void SaveControl_Extra_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
	GFL_BACKUP_SAVEASYNC_Init(ctrl->sv_extra[extra_id]);
}

//--------------------------------------------------------------
/**
 * @brief   �O���f�[�^�̕����Z�[�u���C������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT SaveControl_Extra_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id)
{
	SAVE_RESULT result;
	
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
	result = GFL_BACKUP_SAVEASYNC_Main(ctrl->sv_extra[extra_id]);
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�f�[�^�̊e�Z�[�u���[�N�̃|�C���^���擾����
 *
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @param   gmdata_id	�擾�������Z�[�u�f�[�^��ID
 *
 * @retval  �Z�[�u�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------
void * SaveControl_Extra_DataPtrGet(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, GFL_SVDT_ID gmdata_id)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
	return GFL_SAVEDATA_Get(ctrl->sv_extra[extra_id], gmdata_id);
}

//---------------------------------------------------------------------------
/**
 * @brief	  �O���Z�[�u�f�[�^�̏�����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 *
 * SaveControl_Erase�ƈႢ�A�t���b�V���ɏ������܂Ȃ��B
 * �Z�[�u�f�[�^�������ԂŁu�������傩��v�V�ԏꍇ�Ȃǂ̏���������
 */
//---------------------------------------------------------------------------
void SaveControl_Extra_ClearData(SAVE_CONTROL_WORK * ctrl, SAVE_EXTRA_ID extra_id)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
	GFL_SAVEDATA_Clear(ctrl->sv_extra[extra_id]);
}

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�f�[�^�̏���
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @param   heap_temp_id    ���̊֐����ł̂݃e���|�����Ƃ��Ďg�p����q�[�vID
 */
//--------------------------------------------------------------
void SaveControl_Extra_Erase(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, u32 heap_temp_id)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] != NULL);
	GFL_BACKUP_Erase(ctrl->sv_extra[extra_id], heap_temp_id);
}


//==============================================================================
//	�f�o�b�O�p�֐�
//==============================================================================
#ifdef PM_DEBUG
GFL_SAVEDATA * DEBUG_SaveData_PtrGet(void)
{
	SAVE_CONTROL_WORK *ctrl = SaveControl_GetPointer();
	return ctrl->sv_normal;
}
#endif

