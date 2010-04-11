//==============================================================================
/**
 * @file    save_outside.c
 * @brief   �Z�[�u�Ǘ��O�̈�̃Z�[�u�V�X�e��
 * @author  matsuda
 * @date    2010.04.10(�y)
 */
//==============================================================================
#include "savedata/save_control.h"



extern SAVE_CONTROL_WORK * SaveControl_SystemInit(HEAPID heap_id);
extern LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl);
extern void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl);
extern void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id);
extern void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl);


//--------------------------------------------------------------
//  �O���Z�[�u�֘A
//--------------------------------------------------------------
extern void * SaveControl_Extra_DataPtrGet(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, GFL_SVDT_ID gmdata_id);
extern void SaveControl_Extra_ClearData(SAVE_CONTROL_WORK * ctrl, SAVE_EXTRA_ID extra_id);
extern void SaveControl_Extra_Erase(
  SAVE_CONTROL_WORK *ctrl, SAVE_EXTRA_ID extra_id, u32 heap_temp_id);

//==================================================================
/**
 * �Ǘ��O�Z�[�u�f�[�^�V�X�e�����쐬
 *
 * @param   heap_id		
 *
 * @retval  OUTSIDE_SAVE_CONTROL *		
 */
//==================================================================
OUTSIDE_SAVE_CONTROL * OutsideSave_SystemInit(HEAPID heap_id)
{
  OUTSIDE_SAVE_CONTROL * outsv;
  
  outsv = GFL_HEAP_AllocClearMemory(heap_id, sizeof(OUTSIDE_SAVE_CONTROL));
  return outsv;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�V�X�e����j��
 *
 * @param   ctrl		
 */
//==================================================================
void OutsideSave_SystemExit(SAVE_CONTROL_WORK *ctrl)
{
  GFL_HEAP_FreeMemory(ctrl);
}

///�Ǘ��O�Z�[�u�f�[�^�V�X�e���\����
typedef struct{
  GFL_SAVEDATA *sv;
  BOOL data_exists;         ///<TRUE:�f�[�^�����݂���@FALSE:���݂��Ȃ�
  BOOL data_break;          ///<TRUE:�f�[�^���j�󂳂�Ă���
}OUTSIDE_SAVE_CONTROL;

///�Ǘ��O�Z�[�u�f�[�^����ID
enum{
  OUTSIDE_GMDATA_ID_MYSTERY,  //�s�v�c�ȑ��蕨
};

//--------------------------------------------------------------
//  �Ǘ��O�Z�[�u�f�[�^�̃e�[�u��
//--------------------------------------------------------------
static const GFL_SAVEDATA_TABLE SaveDataTbl_Outside[] = {
  {
    OUTSIDE_GMDATA_ID_MYSTERY,
    (FUNC_GET_SIZE)BattleRec_GetWorkSize,
    (FUNC_INIT_WORK)BattleRec_WorkInit,
  },
};

//--------------------------------------------------------------
/**
 * @brief   �Ǘ��O�Z�[�u�p�����[�^
 */
//--------------------------------------------------------------
static const GFL_SVLD_PARAM SaveParam_OutsideTbl = {
  {//�퓬�^��F����
    SaveDataTbl_Outside,
    NELEMS(SaveDataTbl_Outside),
    OUTSIDE_MM_MYSTERY,              //�o�b�N�A�b�v�̈�擪�A�h���X
    OUTSIDE_MM_MYSTERY_MIRROR,       //�~���[�����O�̈�擪�A�h���X
    SAVESIZE_OUTSIDE_MYSTERY,        //�g�p����o�b�N�A�b�v�̈�̑傫��
    SAVE_MAGIC_NUMBER,
  },
};


//==================================================================
/**
 * �Ǘ��O�Z�[�u�f�[�^�̃��[�h
 *
 * @param   ctrl		    �Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @param   heap_id		  �ǂݍ��񂾃f�[�^�����̃q�[�v�ɓW�J
 *
 * @retval  LOAD_RESULT		���[�h����
 * 
 * �g�p���I�������K��OutsideSave_Unload�ŉ�����Ă�������(���[�h�o���Ȃ������Ƃ��Ă�)
 */
//==================================================================
LOAD_RESULT OutsideSave_Load(SAVE_CONTROL_WORK *ctrl, HEAPID heap_id)
{
	LOAD_RESULT load_ret;

  GF_ASSERT(ctrl->sv == NULL);
	ctrl->sv = GFL_SAVEDATA_Create(&SaveParam_OutsideTbl, heap_id);
	
	load_ret = GFL_BACKUP_Load(ctrl->sv, heap_id);

	switch(load_ret){
	case LOAD_RESULT_OK:
	case LOAD_RESULT_NG:
		ctrl->data_exists = TRUE;
		break;
	case LOAD_RESULT_BREAK:			///<�j��A�����s�\
	  ctrl->data_break = TRUE;
	  //break fall throw
	case LOAD_RESULT_NULL:		  ///<�f�[�^�Ȃ�
	case LOAD_RESULT_ERROR:			///<�@��̏�(Flash������)�Ȃǂœǂݎ��s�\
	default:
    ctrl->data_exists = FALSE;
	  break;
	}
	
	OS_TPrintf("�Ǘ��O�Z�[�u���[�h���� load_ret = %d\n", load_ret);
	return load_ret;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�f�[�^���A�����[�h
 *
 * @param   ctrl		
 */
//==================================================================
void OutsideSave_Unload(SAVE_CONTROL_WORK *ctrl)
{
  GFL_SAVEDATA_Delete(ctrl->sv);
  ctrl->sv = NULL;
}

//--------------------------------------------------------------
/**
 * @brief   �Ǘ��O�Z�[�u�f�[�^�̕����Z�[�u������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
void OutsideSave_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl)
{
	GFL_BACKUP_SAVEASYNC_Init(ctrl->sv);
}

//--------------------------------------------------------------
/**
 * @brief   �Ǘ��O�Z�[�u�f�[�^�̕����Z�[�u���C������
 * @param   ctrl		�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT OutsideSave_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl)
{
	SAVE_RESULT result;
	
	result = GFL_BACKUP_SAVEASYNC_Main(ctrl->sv);
	if(result == SAVE_RESULT_OK){
		ctrl->data_exists = TRUE;
	}
	return result;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�f�[�^�̊e�Z�[�u���[�N�̃|�C���^���擾����
 *
 * @param   ctrl		    
 * @param   gmdata_id		�擾�������Z�[�u�f�[�^��ID
 *
 * @retval  void *		  �Z�[�u�̈�ւ̃|�C���^
 */
//==================================================================
void * OutsideSave_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id)
{
	return GFL_SAVEDATA_Get(ctrl->sv, gmdata_id);
}


//==================================================================
/**
 * �Ǘ��O�Z�[�u�f�[�^�̈����������
 *
 * @param   heap_temp_id		
 */
//==================================================================
void SaveControl_Outside_Erase(HEAPID heap_temp_id)
{
  GFL_BACKUP_FlashErase(heap_temp_id, OUTSIDE_MM_MYSTERY, 0x800);
}


///�Ǘ��O�Z�[�u�F�j���
typedef enum{
  OUTSIDE_BREAK_OK,     ///<���ʐ���
  OUTSIDE_BREAK_A,      ///<A�ʔj��
  OUTSIDE_BREAK_B,      ///<B�ʔj��
  OUTSIDE_BREAK_ALL,    ///<���ʔj��
}OUTSIDE_BREAK;

///�Ǘ��O�Z�[�u�F�}�W�b�N�i���o�[
#define OUTSIDE_MAGIC_NUMBER    (0xa10f49ae)

///�Ǘ��O�s�v�c�f�[�^�ŏ����ł���J�[�h��
#define OUTSIDE_MYSTERY_MAX     (3)

///�~���[�����O�̐�
enum{
  _MIRROR_A,
  _MIRROR_B,
  
  _MIRROR_NUM,
};

///�Ǘ��O�Z�[�u�F�s�v�c�ȑ��蕨
typedef struct{
  u8 recv_flag[MYSTERY_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  GIFT_PACK_DATA card[OUTSIDE_MYSTERY_MAX];		// �J�[�h���
  RECORD_CRC crc;   //CRC & �Í����L�[   4 byte
}OUTSIDE_MYSTERY;

///�Ǘ��O�Z�[�u�F���ۂɃt���b�V���ɋL�^�����f�[�^(���ꂪ���̂܂܃t���b�V���ɏ������)
typedef struct{
  u32 magic_number;   ///<�f�[�^�̑��ݗL���������}�W�b�N�i���o�[
                      ///< (�폜�͂��������������ׁA�K���擪�ɔz�u���鎖�I�I)
  u32 global_count;
  u16 crc;            ///<OUTSIDE_MYSTERY��CRC�l
  u16 padding;
  OUTSIDE_MYSTERY mystery;
}OUTSIDE_MYSTERY_SAVEDATA;

///�Ǘ��O�Z�[�u�R���g���[���V�X�e��
typedef struct{
  OUTSIDE_MYSTERY_SAVEDATA mystery_save;
  OUTSIDE_BREAK break_flag;        ///<�t���b�V���̔j���(OUTSIDE_BREAK_xxx)
  u8 data_exists;                  ///<TRUE:�f�[�^�����݂���  FALSE:���݂��Ȃ�
  u8 padding[3];
}OUTSIDE_SAVE_CONTROL;


//==================================================================
/**
 * �Ǘ��O�Z�[�u�̃V�X�e�����쐬���A���[�h
 *
 * @param   heap_id		
 *
 * @retval  OUTSIDE_MYSTERY_SAVESYSTEM *		�쐬���ꂽ�Ǘ��O�Z�[�u�V�X�e���ւ̃|�C���^
 */
//==================================================================
OUTSIDE_MYSTERY_SAVESYSTEM * OutsideSave_SystemLoad(HEAPID heap_id)
{
  OUTSIDE_SAVE_CONTROL *outsv_ctrl;
  OUTSIDE_MYSTERY_SAVESYSTEM *buf[OUTSIDE_MIRROR_NUM];
  BOOL load_ret[OUTSIDE_MIRROR_NUM];  //FALSE:�f�[�^�����݂��Ȃ�
  BOOL crc_break[OUTSIDE_MIRROR_NUM]; //TRUE:CRC����v���Ȃ�
  int no;
  int ok_side = -1;
  
  outsv_ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(OUTSIDE_SAVE_CONTROL));
  buf[_MIRROR_A] = &oubsv_ctrl->mystery_save;  //A�ʂ̓V�X�e���̃o�b�t�@�����̂܂܎g�p
  buf[_MIRROR_B] = GFL_HEAP_AllocClearMemory(heap_id, sizeof(OUTSIDE_MYSTERY_SAVESYSTEM));
  
  for(no = 0; no < OUTSIDE_MIRROR_NUM; no++){
    load_ret[no] = FALSE;
    crc_break[no] = FALSE;
  }
  
  load_ret[_MIRROR_A] = GFL_BACKUP_DirectFlashLoad(OUTSIDE_MM_MYSTERY, buf_a, SAVESIZE_OUTSIDE_MYSTERY);
  load_ret[_MIRROR_B] = GFL_BACKUP_DirectFlashLoad(OUTSIDE_MM_MYSTERY_MIRROR, buf_b, SAVESIZE_OUTSIDE_MYSTERY);
  
  for(no = 0; no < OUTSIDE_MIRROR_NUM; no++){
    if(load_ret[no] == TRUE){
      //�}�W�b�N�i���o�[�Ńf�[�^�̑��݃`�F�b�N
      if(buf[no]->magic_number != OUTSIDE_MAGIC_NUMBER){
        load_ret[no] = FALSE;
        continue;
      }
      
      //CRC�`�F�b�N
      if(GFL_STD_CrcCalc(&buf[no]->mystery, sizeof(OUTSIDE_MYSTERY)) != buf[no]->crc){
        crc_break[no] = TRUE;
      }
    }
  }
  
  //���[�h���ʂ�CRC���`�F�b�N���āA�g�p����ʂƔj��󋵂𒲂ׂ�
  outsv_ctrl->data_exists = FALSE;
  if(load_ret[_MIRROR_A] == FALSE && load_ret[_MIRROR_B] == FALSE){
    //���ʃf�[�^�Ȃ�
    ;
  }
  else if(load_ret[_MIRROR_A] == TRUE && load_ret[_MIRROR_B] == FALSE){
    if(crc_break[_MIRROR_A] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //A�ʂ݂̂��邪�A�f�[�^�j��
    }
    else{
      ok_side = _MIRROR_A;
    }
  }
  else if(load_ret[_MIRROR_A] == FALSE && load_ret[_MIRROR_B] == TRUE){
    if(crc_break[_MIRROR_B] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //B�ʂ݂̂��邪�A�f�[�^�j��
    }
    else{
      ok_side = _MIRROR_B;
    }
  }
  else if(load_ret[_MIRROR_A] == TRUE && load_ret[_MIRROR_B] == TRUE){
    if(crc_break[_MIRROR_A] == TRUE && crc_break[_MIRROR_B] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //���ʃf�[�^�j��
    }
    else if(crc_break[_MIRROR_A] == FALSE && crc_break[_MIRROR_B] == FALSE){
      if(buf[_MIRROR_A]->global_count >= buf[_MIRROR_B]->global_count
          || (buf[_MIRROR_A]->global_count == 0 && buf[_MIRROR_B]->global_count == 0xffffffff)){
        ok_side = _MIRROR_A;
      }
      else{
        ok_side = _MIRROR_B;
      }
    }
    else if(crc_break[_MIRROR_A] == FALSE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_B;
      ok_side = _MIRROR_A;
    }
    else{
      outsv_ctrl->break_flag = OUTSIDE_BREAK_A;
      ok_side = _MIRROR_B;
    }
  }
  
  //B�ʃo�b�t�@�͔j������̂ŁA����ȃf�[�^��B�ʂɂ������ꍇ��A�ʃo�b�t�@�ɃR�s�[
  if(ok_side == _MIRROR_B){
    GFL_STD_MemCopy(buf[_MIRROR_B], buf[_MIRROR_A], sizeof(OUTSIDE_MYSTERY_SAVESYSTEM));
  }
  GFL_HEAP_FreeMemory(buf[_MIRROR_B]);
  return outsv_ctrl;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�V�X�e����j������
 *
 * @param   outsv_ctrl		
 */
//==================================================================
void OutsideSave_SystemUnload(OUTSIDE_MYSTERY_SAVESYSTEM *outsv_ctrl)
{
  GFL_HEAP_FreeMemory(outsv_ctrl);
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F�����Z�[�u����������
 *
 * @param   outsv_ctrl		
 */
//==================================================================
void OutsideSave_SaveAsyncInit(OUTSIDE_MYSTERY_SAVESYSTEM *outsv_ctrl)
{
  outsv_ctrl->save_seq = 0;
  if(outsv_ctrl->break_flag == OUTSIDE_BREAK_B){
    outsv_ctrl->write_side = _MIRROR_B; //B�ʂ����Ă���ꍇ��B�ʂ���
  }
  else{
    outsv_ctrl->write_side = _MIRROR_A; //����ȊO�͑S��A�ʂ���
  }
  
  outsv_ctrl->mystery_save.magic_number = OUTSIDE_MAGIC_NUMBER;
  outsv_ctrl->mystery_save.global_counter++;
  outsv_ctrl->mystery_save.crc 
    = GFL_STD_CrcCalc(&outsv_ctrl->mystery_save.mystery, sizeof(OUTSIDE_MYSTERY));
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F�����Z�[�u���C������
 *
 * @param   outsv_ctrl		
 */
//==================================================================
void OutsideSave_SaveAsyncMain(OUTSIDE_MYSTERY_SAVESYSTEM *outsv_ctrl)
{
  u32 flash_src;
  BOOL err_result;
  
  switch(outsv_ctrl->save_seq){
  case 0:
  case 2:
    OS_TPrintf("�Ǘ��O�Z�[�u write_side = %d\n", outsv_ctrl->write_side);
    flash_src = outsv_ctrl->write_side == _MIRROR_A ? OUTSIDE_MM_MYSTERY : OUTSIDE_MM_MYSTERY_MIRROR;
    outsv_ctrl->lock_id = GFL_BACKUP_DirectFlashSaveAsyncInit(
      flash_src, &outsv_ctrl->mystery_save, sizeof(OUTSIDE_MYSTERY_SAVEDATA) );
    outsv_ctrl->save_seq++;
    break;
  case 1:
  case 3:
    if(GFL_BACKUP_DirectFlashSaveAsyncMain(outsv_ctrl->lock_id, &err_result) == TRUE){
      OS_TPrintf("�Ǘ��O�Z�[�u result(1=����) = %d\n", err_result);
      outsv_ctrl->write_side ^= 1;
      outsv_ctrl->save_seq++;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F����
 *
 * �����p�̃������m�ۂ̕K�v���⑬�x�Ȃǂ̍l������A�}�W�b�N�i���o�[�������������܂�
 */
//==================================================================
void OutsideSave_SaveErase(void)
{
  u32 erase_magic_number = 0;
  
  OS_TPrintf("== outside Erase start  ==\n");
  //�}�W�b�N�i���o�[����������
  GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY, &erase_magic_number, sizeof(u32));
  GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY_MIRROR, &erase_magic_number, sizeof(u32));
  OS_TPrintf("== outside Erase finish ==\n");
}
