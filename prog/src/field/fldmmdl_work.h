//======================================================================
/**
 * @file  fldmmdl_work.h
 * @brief  ���샂�f�� ���[�N��`
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#ifdef MMDL_SYSTEM_FILE
#ifndef MMDL_WORK_H_FILE
#define MMDL_WORK_H_FILE

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  MMDL ����A�`��֐����[�N�T�C�Y (byte size)
//--------------------------------------------------------------
#define MMDL_MOVE_WORK_SIZE    (16)  ///<����֐��p���[�N�T�C�Y
#define MMDL_MOVE_SUB_WORK_SIZE  (16)  ///<����T�u�֐��p���[�N�T�C�Y
#define MMDL_MOVE_CMD_WORK_SIZE  (16)  ///<����R�}���h�p���[�N�T�C�Y
#define MMDL_DRAW_WORK_SIZE    (32)  ///<�`��֐��p���[�N�T�C�Y

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  MMDLSYS�\����
//--------------------------------------------------------------
struct _TAG_MMDLSYS
{
  u32 status_bit;          ///<�X�e�[�^�X�r�b�g
  
  u16 mmdl_max;          ///<MMDL�ő吔
  s16 mmdl_count;        ///<�t�B�[���h���샂�f�����ݐ�
  
  HEAPID sysHeapID;        ///<�V�X�e���p �q�[�vID
  HEAPID heapID;          ///<�q�[�vID
  
  s16 tcb_pri;          ///<TCB�v���C�I���e�B
  u8 padding0;
  u8 padding1;
  
  ARCHANDLE *arcH_res; ///<���샂�f�����\�[�X�A�[�J�C�u�n���h��

  MMDL *pMMdlBuf;      ///<MMDL���[�N *
  void *pTCBSysWork;        ///<TCB���[�N
  GFL_TCBSYS *pTCBSys;      ///<TCBSYS*
  
  MMDL_BLACTCONT *pBlActCont;  ///<MMDL_BLACTCONT
  MMDL_G3DOBJCONT *pObjCont; ///<MMDL_G3DOBJCONT
  
  MMDL_ROCKPOS *rockpos; ///<�����肫����W �Z�[�u�f�[�^�|�C���^
  
  u8 *pOBJCodeParamBuf;      ///<OBJCODE_PARAM�o�b�t�@
  const OBJCODE_PARAM *pOBJCodeParamTbl; ///<OBJCODE_PARAM
  
  GAMEDATA *gdata; ///<GAMEDATA
  const FLDMAPPER *pG3DMapper;  ///<FLDMAPPER
  FLDNOGRID_MAPPER *pNOGRIDMapper;  ///<FLDNOGRID_MAPPER
  void *fieldMapWork; ///<FIELDMAP_WORK <-�e���[�N�P�ʂł̐ڑ����ǂ��B
  const u16 *targetCameraAngleYaw; //�O���[�o���ŎQ�Ƃ���J����
};

#define MMDLSYS_SIZE (sizeof(MMDLSYS)) ///<MMDLSYS�T�C�Y

//--------------------------------------------------------------
///  MMDL�\����
//--------------------------------------------------------------
struct _TAG_MMDL
{
  u32 status_bit;        ///<�X�e�[�^�X�r�b�g
  u32 move_bit;        ///<����r�b�g
  
  u16 obj_id;          ///<OBJ ID
  u16 zone_id;        ///<�]�[�� ID
  u16 obj_code;        ///<OBJ�R�[�h
  u16 move_code;        ///<����R�[�h
  u16 event_type;        ///<�C�x���g�^�C�v
  u16 event_flag;        ///<�C�x���g�t���O
  u16 event_id;        ///<�C�x���gID
  u16 dir_head;        ///<MMDL_H�w�����
  u16 dir_disp;        ///<���݌����Ă������
  u16 dir_move;        ///<���ݓ����Ă������
  u16 dir_disp_old;      ///<�ߋ��̓����Ă�������
  u16 dir_move_old;      ///<�ߋ��̓����Ă�������
  
  u16 param0;          ///<�w�b�_�w��p�����^
  u16 param1;          ///<�w�b�_�w��p�����^
  u16 param2;          ///<�w�b�_�w��p�����^
  
  u16 acmd_code;        ///<�A�j���[�V�����R�}���h�R�[�h
  u16 acmd_seq;        ///<�A�j���[�V�����R�}���h�V�[�P���X
  u16 draw_status;      ///<�`��X�e�[�^�X
  s16 move_limit_x;      ///<X�����ړ�����
  s16 move_limit_z;      ///<Z�����ړ�����
  s16 gx_init;        ///<�����O���b�hX
  s16 gy_init;        ///<�����O���b�hY
  s16 gz_init;        ///<�����O���b�hZ
  s16 gx_old;          ///<�ߋ��O���b�hX
  s16 gy_old;          ///<�ߋ��O���b�hY
  s16 gz_old;          ///<�ߋ��O���b�hZ
  s16 gx_now;          ///<���݃O���b�hX
  s16 gy_now;          ///<���݃O���b�hY
  s16 gz_now;          ///<���݃O���b�hZ
  VecFx32 vec_pos_now;    ///<���ݎ������W
  VecFx32 vec_draw_offs;    ///<�\�����W�I�t�Z�b�g
  VecFx32 vec_draw_offs_outside;  ///<�O���w��\�����W�I�t�Z�b�g
  VecFx32 vec_attr_offs;    ///<�A�g���r���[�g�ɂ����W�I�t�Z�b�g
  u32 now_attr;        ///<���݂̃}�b�v�A�g���r���[�g
  u32 old_attr;        ///<�ߋ��̃}�b�v�A�g���r���[�g
  
  u8 gx_size; ///<�O���b�hX�����T�C�Y
  u8 gz_size; ///<�O���b�hZ�����T�C�Y
  s8 offset_x; ///<�I�t�Z�b�gX
  s8 offset_y; ///<�I�t�Z�b�gY
  
  s8 offset_z; ///<�I�t�Z�b�gZ
  u8 padding[3]; ///<4byte�]��
  
  GFL_TCB *pTCB;        ///<����֐�TCB*
  MMDLSYS *pMMdlSys;    ///<MMDLSYS*
  
  const MMDL_MOVE_PROC_LIST *move_proc_list; ///<����֐����X�g
  const MMDL_DRAW_PROC_LIST *draw_proc_list; ///<�`��֐����X�g
  
  u8 move_proc_work[MMDL_MOVE_WORK_SIZE];///����֐��p���[�N
  u8 move_sub_proc_work[MMDL_MOVE_SUB_WORK_SIZE];///����T�u�֐��p���[�N
  u8 move_cmd_proc_work[MMDL_MOVE_CMD_WORK_SIZE];///����R�}���h�p���[�N
  u8 draw_proc_work[MMDL_DRAW_WORK_SIZE];///�`��֐��p���[�N
};

#define MMDL_SIZE (sizeof(MMDL)) ///<MMDL�T�C�Y 224

#endif //MMDL_WORK_H_FILE
#endif //MMDL_SYSTEM_FILE
