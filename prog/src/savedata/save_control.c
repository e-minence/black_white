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
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
///�Z�[�u�f�[�^�Ǘ����[�N�\����
struct _SAVE_CONTROL_WORK{
	BOOL new_data_flag;			///<TRUE:�V�K�f�[�^
	BOOL total_save_flag;		///<TRUE:�S�̃Z�[�u
	u32 first_status;			///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����(bit�w��)
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���p�Z�[�u�f�[�^�ւ̃|�C���^
	GFL_SAVEDATA *sv_box;		///<�{�b�N�X�p�Z�[�u�f�[�^�ւ̃|�C���^
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
	GFL_SAVEDATA * sv;
	LOAD_RESULT load_ret;

	ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(SAVE_CONTROL_WORK));
	SaveControlWork = ctrl;
	ctrl->new_data_flag = TRUE;			//�V�K�f�[�^
	ctrl->total_save_flag = TRUE;		//�S�̃Z�[�u
	ctrl->sv_normal = GFL_SAVEDATA_Create(&SaveParam_Normal);


	//�f�[�^���݃`�F�b�N���s���Ă���
	load_ret = GFL_BACKUP_Load(ctrl->sv_normal);
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
		if(load_ret == LOAD_RESULT_NG){	//OK��NG������������ʂ�̂ŉ��߂ă`�F�b�N
			ctrl->first_status |= NORMAL_NG_BIT;
		}

	#if 0	//��check�@�O�����o����܂Ō��
		//�O���Z�[�u�̑��݃`�F�b�N
		{
			LOAD_RESULT frontier_result, video_result;
			
			ExtraNewCheckLoadData(sv, &frontier_result, &video_result);
			if(frontier_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= FRONTIER_BREAK_BIT;
			}
			else if(frontier_result == LOAD_RESULT_NG){
				ctrl->first_status |= FRONTIER_NG_BIT;
			}
			if(video_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= VIDEO_BREAK_BIT;
			}
			else if(video_result == LOAD_RESULT_NG){
				ctrl->first_status |= VIDEO_NG_BIT;
			}
		}
	#endif
		break;
	case LOAD_RESULT_BREAK:			///<�j��A�����s�\
		OS_TPrintf("LOAD:�f�[�^�j��\n");
		ctrl->first_status |= NORMAL_BREAK_BIT;
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
	return GFL_BACKUP_Load(ctrl->sv_normal);
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
	return GFL_BACKUP_Save(ctrl->sv_normal);
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
	return GFL_BACKUP_SAVEASYNC_Main(ctrl->sv_normal);
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
