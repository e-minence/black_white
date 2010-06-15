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
#include "savedata/player_data.h"
#include "savedata/musical_save.h"
#include "savedata/save_outside.h"


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
	u8 new_data_flag;			///<TRUE:�V�K�f�[�^
	u8 data_exists;			///<�f�[�^�����݂��邩�ǂ���
	u8 total_save_flag;		///<TRUE:�S�̃Z�[�u
	u8 backup_now_save_mode_setup;    ///<TRUE:����Z�b�g�A�b�v�̃t���O�o�b�N�A�b�v
	u8 outside_data_exists;   ///<TRUE:�Ǘ��O�Z�[�u�����݂���
	u8 outside_data_break;    ///<TRUE:�Ǘ��O�Z�[�u�j��
	u8 padding[3];
	u32 first_status;			///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����(bit�w��)
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���p�Z�[�u�f�[�^�ւ̃|�C���^
	GFL_SAVEDATA *sv_extra[SAVE_EXTRA_ID_MAX];
};

//==============================================================================
//	���[�J���ϐ�
//==============================================================================
///�Z�[�u�f�[�^�ւ̃|�C���^
static SAVE_CONTROL_WORK *SaveControlWork = NULL;
extern int BATTLE_EXAMINATION_SAVE_GetWorkSize(void);

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _OutsideSave_SaveErase(void);

//--------------------------------------------------------------
//  �I�[�o�[���CID
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(outside_save);



//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���̏�����
 */
//---------------------------------------------------------------------------
SAVE_CONTROL_WORK * SaveControl_SystemInit(HEAPID heap_id)
{
	SAVE_CONTROL_WORK *ctrl;
	LOAD_RESULT load_ret;
  SAVE_EXTRA_ID extra_id;
  BOOL outside_exists = FALSE;
  BOOL outside_break = FALSE;
  
  //�����Z�[�u�Ńq�[�v���g�p������O�ɊǗ��O�Z�[�u�̑��݃`�F�b�N���s��
  {
    OUTSIDE_SAVE_CONTROL *outsv;
    
    GFL_OVERLAY_Load( FS_OVERLAY_ID(outside_save) );
    
    outsv = OutsideSave_SystemLoad(heap_id);
    outside_exists = OutsideSave_GetExistFlag(outsv);
    outside_break = OutsideSave_GetBreakFlag(outsv);
    OutsideSave_SystemUnload(outsv);
    
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(outside_save) );
  }
  
	ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(SAVE_CONTROL_WORK));
	SaveControlWork = ctrl;
	ctrl->new_data_flag = TRUE;			//�V�K�f�[�^
	ctrl->total_save_flag = TRUE;		//�S�̃Z�[�u
	ctrl->data_exists = FALSE;			//�f�[�^�͑��݂��Ȃ�
	ctrl->outside_data_exists = outside_exists;
	ctrl->outside_data_break = outside_break;
	ctrl->sv_normal = GFL_SAVEDATA_Create(&SaveParam_Normal, heap_id);

#if DEBUG_ONLY_FOR_ohno
  {  //�e�f�[�^�̃T�C�Y�𓾂�
    int i;
    for(i=0;i<SaveParam_Normal.table_max;i++){
      FUNC_GET_SIZE func = SaveParam_Normal.table[i].get_size;
      
        OS_TPrintf(">><< size %d\n",func());
    }
  }
#endif

  //---------------- �����Z�[�u���O���Z�[�u ----------------
	//�f�[�^���݃`�F�b�N���s���Ă���
	load_ret = GFL_BACKUP_Load(ctrl->sv_normal, HEAPID_SAVE_TEMP);
  if(load_ret == LOAD_RESULT_OK || load_ret == LOAD_RESULT_NG){
    //�f�[�^��ǂ߂Ă�����Z�b�g�A�b�v��������Ă��Ȃ��Ȃ�f�[�^���������ɂ���
    if(SaveData_GetNowSaveModeSetup(ctrl) == TRUE){
      load_ret = LOAD_RESULT_NULL;
      OS_TPrintf("����Z�b�g�A�b�v�ׁ݂̂̈A�f�[�^��������\n");
    }
  }
	ctrl->first_status = 0;
	if(outside_break == TRUE){
    ctrl->first_status |= FST_OUTSIDE_MYSTERY_BREAK_BIT;
  }
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

	  for(extra_id = 0; extra_id < SAVE_EXTRA_ID_MAX; extra_id++){
      if(SaveData_CheckExtraMagicKey(ctrl, extra_id) == TRUE){  //�Z�[�u�o���̂���O�������`�F�b�N
        LOAD_RESULT extra_result;
        extra_result = SaveControl_Extra_Load(ctrl, extra_id, HEAPID_SAVE_TEMP);
        SaveControl_Extra_Unload(ctrl, extra_id);
        switch(extra_result){
      	case LOAD_RESULT_OK:				///<�f�[�^����ǂݍ���
      	case LOAD_RESULT_NG:				///<�f�[�^�ُ�
          if(extra_id == SAVE_EXTRA_ID_MUSICAL_DIST){
            MUSICAL_SAVE* mus_save = MUSICAL_SAVE_GetMusicalSave( ctrl );
            MUSICAL_SAVE_SetEnableDistributData( mus_save, TRUE );
          }
      	  break;
      	case LOAD_RESULT_NULL:  		///<�f�[�^�Ȃ�
      	case LOAD_RESULT_BREAK:			///<�j��A�����s�\
      	case LOAD_RESULT_ERROR:			///<�@��̏�Ȃǂœǂݎ��s�\
    			ctrl->first_status |= 1 << (FST_EXTRA_START + extra_id);
          if(extra_id == SAVE_EXTRA_ID_MUSICAL_DIST){
            MUSICAL_SAVE* mus_save = MUSICAL_SAVE_GetMusicalSave( ctrl );
            MUSICAL_SAVE_SetEnableDistributData( mus_save, FALSE );
          }
      	  break;
      	}
      }
      else if(extra_id == SAVE_EXTRA_ID_MUSICAL_DIST){
        MUSICAL_SAVE* mus_save = MUSICAL_SAVE_GetMusicalSave( ctrl );
        MUSICAL_SAVE_SetEnableDistributData( mus_save, FALSE );
      }
    }
		break;
	case LOAD_RESULT_BREAK:			///<�j��A�����s�\
		ctrl->first_status |= FST_NORMAL_BREAK_BIT;
		//break;
		/* FALL THROUGH */
	case LOAD_RESULT_NULL:		///<�f�[�^�Ȃ�
	case LOAD_RESULT_ERROR:			///<�@��̏�(Flash������)�Ȃǂœǂݎ��s�\
		//�V�K or �f�[�^�j��Ȃ̂ŃN���A����
		GFL_SAVEDATA_Clear(ctrl->sv_normal);
		break;
	default:
		GF_ASSERT(0); //LOAD:��O�G���[�I
		break;
	}

	return ctrl;
}

//==================================================================
/**
 * �Z�[�u�V�X�e����j��
 */
//==================================================================
void SaveControl_SystemExit(void)
{
  if(SaveControlWork != NULL){
    GFL_SAVEDATA_Delete(SaveControlWork->sv_normal);
    GFL_HEAP_FreeMemory(SaveControlWork);
    SaveControlWork = NULL;
  }
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
  OS_TPrintf("SAVE_CONTROL_WORK�ւ�Global�Q��\n");
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
    {
      MUSICAL_SAVE* mus_save = MUSICAL_SAVE_GetMusicalSave( ctrl );
      MUSICAL_SAVE_SetEnableDistributData( mus_save, FALSE );
      if(SaveData_CheckExtraMagicKey(ctrl, SAVE_EXTRA_ID_MUSICAL_DIST) == TRUE){
        LOAD_RESULT extra_result;
        extra_result = SaveControl_Extra_Load(ctrl, SAVE_EXTRA_ID_MUSICAL_DIST, HEAPID_SAVE_TEMP);
        SaveControl_Extra_Unload(ctrl, SAVE_EXTRA_ID_MUSICAL_DIST);
        switch(extra_result){
      	case LOAD_RESULT_OK:				///<�f�[�^����ǂݍ���
      	case LOAD_RESULT_NG:				///<�f�[�^�ُ�
          MUSICAL_SAVE_SetEnableDistributData( mus_save, TRUE );
          break;
        }
      }
    }
    
    //�j�󂳂�Ă���O���Z�[�u������Ȃ烊���N��������������
    if(ctrl->first_status > 0){
      SAVE_EXTRA_ID extra_id;
      for(extra_id = 0; extra_id < SAVE_EXTRA_ID_MAX; extra_id++){
        if(ctrl->first_status & (1 << (FST_EXTRA_START + extra_id))){
          SaveData_ClearExtraMagicKey(ctrl, extra_id);
        }
      }
    }
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

  SaveData_SetNowSaveModeSetupOFF(ctrl);
  
	result = GFL_BACKUP_Save(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
    if(SaveData_GetNowSaveModeSetup(ctrl) == FALSE){
  		ctrl->new_data_flag = FALSE;
	  	ctrl->data_exists = TRUE;
	  }
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
  ctrl->backup_now_save_mode_setup = SaveData_GetNowSaveModeSetup(ctrl);
  SaveData_SetNowSaveModeSetupOFF(ctrl);
  
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
	
	if(ctrl->outside_data_exists == TRUE || ctrl->outside_data_break == TRUE){
    _OutsideSave_SaveErase();
    ctrl->outside_data_exists = FALSE;
    return SAVE_RESULT_CONTINUE;
  }
  
	result = GFL_BACKUP_SAVEASYNC_Main(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
    if(SaveData_GetNowSaveModeSetup(ctrl) == FALSE){
  		ctrl->new_data_flag = FALSE;
	  	ctrl->data_exists = TRUE;
	  }
	}
	return result;
}

//==================================================================
/**
 * �����Z�[�u�̃L�����Z������
 *
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 */
//==================================================================
void SaveControl_SaveAsyncCancel(SAVE_CONTROL_WORK *ctrl)
{
  GFL_BACKUP_SAVEASYNC_Cancel(ctrl->sv_normal);
  if(ctrl->backup_now_save_mode_setup == TRUE){
    SaveData_SetNowSaveModeSetupON(ctrl);
  }
}

//==================================================================
/**
 * SaveControl_SaveAsyncMain�Ŏ��s���̃Z�[�u�����o�C�g�ڂ܂ŏ������񂾂����擾����
 *
 * @param   ctrl		
 *
 * @retval  u32	  �������݂����������T�C�Y(SaveControl_GetActualSize��actual_size * 2(���ʃT�C�Y))
 */
//==================================================================
u32 SaveControl_GetSaveAsyncMain_WritingSize(SAVE_CONTROL_WORK *ctrl)
{
  return GFL_BACKUP_SAVEASYNC_Main_WritingSize(ctrl->sv_normal);
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

//==================================================================
/**
 * �Z�[�u�f�[�^���[�N�̐擪�A�h���X���擾
 *
 * @param   ctrl    		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   get_size		�Z�[�u�f�[�^�̎��T�C�Y���������郏�[�N�̃|�C���^
 *
 * @retval  const void *		�Z�[�u�f�[�^���[�N�̐擪�A�h���X
 *
 * GPF�ŃZ�[�u�f�[�^�S�]���p�ɗp�ӂ����֐��ł�
 * ���̉ӏ��ł͎g�p���Ȃ�����!
 */
//==================================================================
const void * SaveControl_GetSaveWorkAdrs(SAVE_CONTROL_WORK *ctrl, u32 *get_size)
{
  return GFL_SAVEDATA_GetSaveWorkAdrs(ctrl->sv_normal, get_size);
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
BOOL SaveControl_NewDataFlagGet(const SAVE_CONTROL_WORK *ctrl)
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
BOOL SaveData_GetExistFlag(SAVE_CONTROL_WORK * sv)
{
	return sv->data_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	�f�[�^�㏑���`�F�b�N(���l�̃��|�[�g�㏑���`�F�b�N)
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	TRUE		���ɂ���f�[�^�ɕʂ̃f�[�^���㏑�����悤�Ƃ��Ă���
 * @retval	FALSE		�f�[�^���Ȃ����A�����f�[�^�ł���
 */
//---------------------------------------------------------------------------
BOOL SaveControl_IsOverwritingOtherData(SAVE_CONTROL_WORK * sv)
{
	if (SaveControl_NewDataFlagGet(sv) == TRUE && SaveData_GetExistFlag(sv) == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	�Ǘ��O�Z�[�u�f�[�^���݃t���O���擾
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�Z�[�u�f�[�^�����݂���
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetOutsideExistFlag(SAVE_CONTROL_WORK * sv)
{
	return sv->outside_data_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Ǘ��O�Z�[�u�f�[�^���݃t���O���Z�b�g
 * @param	sv			              �Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param outside_data_exists		TRUE�̂Ƃ��A�Z�[�u�f�[�^�����݂���
 */
//---------------------------------------------------------------------------
void SaveData_SetOutsideExistFlag(SAVE_CONTROL_WORK * sv, BOOL outside_data_exists)
{
	sv->outside_data_exists = outside_data_exists;
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

//--------------------------------------------------------------
/**
 * �Z�[�u�����s�����ꍇ�̃Z�[�u�T�C�Y���擾����
 *
 * @param   sv		        �Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param   actual_size		�Z�[�u�������T�C�Y(CRC�e�[�u���Ȃǂ̃V�X�e���n�̃f�[�^�͏����܂�)
 * @param   total_size		�Z�[�u�S�̂̃T�C�Y(CRC�e�[�u���Ȃǂ̃V�X�e���n�̃f�[�^�͏����܂�)
 *
 * ������r�ׁ̈A�t���b�V���A�N�Z�X���܂��B
 */
//--------------------------------------------------------------
void SaveControl_GetActualSize(SAVE_CONTROL_WORK *ctrl, u32 *actual_size, u32 *total_size)
{
  GFL_SAVEDATA_GetActualSize(ctrl->sv_normal, actual_size, total_size);
}

//==================================================================
/**
 * �w��ID�̕����������t���b�V�����璼�ڃ��[�h����
 *
 * @param   sv            
 * @param   gmdataid		  ���[�h����Ώۂ�ID
 * @param   side_a_or_b		0:A�ʂ��烍�[�h�@�@1:B�ʂ��烍�[�h
 * @param   dst		        �f�[�^�W�J��
 * @param   load_size     ���[�h����o�C�g�T�C�Y
 *
 * @retval  BOOL		TRUE:���[�h�����@FALSE:���[�h���s
 */
//==================================================================
BOOL SaveControl_PageFlashLoad(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdataid, BOOL side_a_or_b, void *dst, u32 load_size)
{
  return GFL_BACKUP_Page_FlashLoad(ctrl->sv_normal, gmdataid, side_a_or_b, dst, load_size);
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
 * �g�p���I�������K��SaveControl_Extra_Unload�ŉ�����Ă�������(���[�h�o���Ȃ������Ƃ��Ă�)
 */
//==================================================================
LOAD_RESULT SaveControl_Extra_Load(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, HEAPID heap_id)
{
	LOAD_RESULT load_ret;
  u32 *link_ptr;

  GF_ASSERT(ctrl->sv_extra[extra_id] == NULL);
	ctrl->sv_extra[extra_id] = GFL_SAVEDATA_Create(&SaveParam_ExtraTbl[extra_id], heap_id);
	
	if(SaveData_CheckExtraMagicKey(ctrl, extra_id) == FALSE){
    //���݂̓����Z�[�u�Ɋ֘A�t�����Ă���O���͖����ׁA�V�X�e�������쐬���A���[�h�����͍s��Ȃ�
    return LOAD_RESULT_NULL;
  }
	
  link_ptr = SaveData_GetExtraLinkPtr(ctrl, extra_id);
	load_ret = GFL_BACKUP_Extra_Load(ctrl->sv_extra[extra_id], heap_id, *link_ptr);
	
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

//==================================================================
/**
 * �O���Z�[�u�����Ƀ��[�h�ς݂����ׂ�
 *
 * @param   ctrl		
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 *
 * @retval  BOOL		TRUE:���[�h�ς݁B�@FALSE:���[�h����Ă��Ȃ�
 */
//==================================================================
BOOL SaveControl_Extra_CheckLoad(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id)
{
  if(ctrl->sv_extra[extra_id] != NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �O���Z�[�u�f�[�^�̃��[�h(�Z�[�u���[�N�͊O������n��)
 *
 * @param   ctrl		    �Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @param   heap_id		  �Z�[�u�V�X�e���Ŏg�p����q�[�vID
 * @param   work        ���[�h�����f�[�^�����̃��[�N�ɓW�J
 * @param   work_size   work�̃T�C�Y
 *
 * @retval  LOAD_RESULT		���[�h����
 * 
 * �g�p���I�������K��SaveControl_Extra_UnloadWork�ŉ�����Ă�������(���[�h�o���Ȃ������Ƃ��Ă�)
 */
//==================================================================
LOAD_RESULT SaveControl_Extra_LoadWork(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, HEAPID heap_id, void *work, u32 work_size)
{
	LOAD_RESULT load_ret;
  u32 *link_ptr;
  
  GF_ASSERT(ctrl->sv_extra[extra_id] == NULL);
  ctrl->sv_extra[extra_id] 
    = GFL_SAVEDATA_CreateEx(&SaveParam_ExtraTbl[extra_id], heap_id, work, work_size, TRUE );

	if(SaveData_CheckExtraMagicKey(ctrl, extra_id) == FALSE){
    //���݂̓����Z�[�u�Ɋ֘A�t�����Ă���O���͖����ׁA�V�X�e�������쐬���A���[�h�����͍s��Ȃ�
    return LOAD_RESULT_NULL;
  }

  link_ptr = SaveData_GetExtraLinkPtr(ctrl, extra_id);
	load_ret = GFL_BACKUP_Extra_Load(ctrl->sv_extra[extra_id], heap_id, *link_ptr);
	
	OS_TPrintf("�O���Z�[�u���[�h���� extra_id = %d, load_ret = %d\n", extra_id, load_ret);
	return load_ret;
}

//==================================================================
/**
 * �O���Z�[�u�f�[�^�̃V�X�e���݂̂��쐬(�Z�[�u���[�N�͊O������n��)
 *    work�œn����Ă��郏�[�N�̓��e�����̂܂܃��[�h�����f�[�^�Ƃ��ĔF�����A
 *    �t���b�V������f�[�^�̓��[�h���Ȃ�
 *    �@���t���b�V�����烍�[�h�������ꍇ�͂��̊֐��ł͂Ȃ��ASaveControl_Extra_LoadWork��
 *        �g�p���Ă��������B
 *    �O���Z�[�u�ŁA�Z�[�u���O�ɃZ�[�u�V�X�e�����쐬����ꍇ�̎g�p��z�肵�Ă��܂��B
 *
 * @param   ctrl		    �Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 * @param   heap_id		  �Z�[�u�V�X�e���Ŏg�p����q�[�vID
 * @param   work        ���[�h�����f�[�^�����̃��[�N�ɓW�J
 * @param   work_size   work�̃T�C�Y
 * 
 * �g�p���I�������K��SaveControl_Extra_UnloadWork�ŉ�����Ă�������
 */
//==================================================================
void SaveControl_Extra_SystemSetup(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, HEAPID heap_id, void *work, u32 work_size)
{
  GF_ASSERT(ctrl->sv_extra[extra_id] == NULL);
  ctrl->sv_extra[extra_id] 
    = GFL_SAVEDATA_CreateEx(&SaveParam_ExtraTbl[extra_id], heap_id, work, work_size, FALSE );
	
	OS_TPrintf("�O���Z�[�u���[�h�����Z�b�g�A�b�v extra_id = %d\n", extra_id);
}

//==================================================================
/**
 * �O���Z�[�u�f�[�^�̉��
 *
 * @param   ctrl		
 * @param   extra_id		�O���Z�[�u�f�[�^�ԍ�
 */
//==================================================================
void SaveControl_Extra_UnloadWork(SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id)
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
  
  SaveData_SetExtraMagicKey(ctrl, extra_id);
  GFL_BACKUP_SAVEASYNC_EXTRA_Init(ctrl->sv_normal, ctrl->sv_extra[extra_id], 
    SaveData_GetExtraLinkPtr(ctrl, extra_id));
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
  result = GFL_BACKUP_SAVEASYNC_EXTRA_Main(ctrl->sv_normal, ctrl->sv_extra[extra_id]);
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

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F����
 *
 * �����p�̃������m�ۂ̕K�v���⑬�x�Ȃǂ̍l������A�}�W�b�N�i���o�[�������������܂�
 *
 * �����Z�[�u���s���ɏ������s���̂ŁA���ꂾ��save_control.c�Ɠ����풓�ɔz�u
 */
//==================================================================
static void _OutsideSave_SaveErase(void)
{
  u32 erase_magic_number = 0;
  
  OS_TPrintf("== outside Erase start  ==\n");
  //�}�W�b�N�i���o�[����������
  GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY, &erase_magic_number, sizeof(u32));
  GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY_MIRROR, &erase_magic_number, sizeof(u32));
  OS_TPrintf("== outside Erase finish ==\n");
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

GFL_SAVEDATA * DEBUG_SaveDataExtra_PtrGet(SAVE_EXTRA_ID extra_id)
{
	SAVE_CONTROL_WORK *ctrl = SaveControl_GetPointer();
	return ctrl->sv_extra[extra_id];
}

///�Ǘ��O�Z�[�u��j�󂷂�
void DEBUG_OutsideSave_Brea(int mirror)
{
  u64 break_data = 0x401a9f4a;
  
  if(mirror == 0){
    GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY + 12, &break_data, sizeof(break_data));
  }
  else{
    GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY_MIRROR + 12, &break_data, sizeof(break_data));
  }
}
#endif

