//======================================================================
/**
 *
 * @file	fieldobj_header.h
 * @brief	�t�B�[���h���샂�f���w�b�_�[�\���̒�`�w�b�_�[
 * @author	kagaya
 * @data	05.07.13
 *
 */
//======================================================================
#ifndef FLDMMDL_HEADER_FILE
#define FLDMMDL_HEADER_FILE

//======================================================================
//	typedef strcut
//======================================================================
//--------------------------------------------------------------
//	�f�o�b�O
//--------------------------------------------------------------

//--------------------------------------------------------------
//	FLDMMDL ����A�`��֐����[�N�T�C�Y (byte size)
//--------------------------------------------------------------
#define FLDMMDL_MOVE_WORK_SIZE		(16)	///<����֐��p���[�N�T�C�Y
#define FLDMMDL_MOVE_SUB_WORK_SIZE	(16)	///<����T�u�֐��p���[�N�T�C�Y
#define FLDMMDL_MOVE_CMD_WORK_SIZE	(16)	///<����R�}���h�p���[�N�T�C�Y
#define FLDMMDL_DRAW_WORK_SIZE		(32)	///<�`��֐��p���[�N�T�C�Y

//--------------------------------------------------------------
///	FLDMMDL_HEADER�\����
//--------------------------------------------------------------
typedef struct
{
	unsigned short id;			///<����ID
	unsigned short obj_code;	///<�\������OBJ�R�[�h
	unsigned short move_code;	///<����R�[�h
	unsigned short event_type;	///<�C�x���g�^�C�v
	unsigned short event_flag;	///<�C�x���g�t���O
	unsigned short event_id;	///<�C�x���gID
	short dir;					///<�w�����
	unsigned short param0;		///<�w��p�����^ 0
	unsigned short param1;		///<�w��p�����^ 1
	unsigned short param2;		///<�w��p�����^ 2
	short move_limit_x;			///<X�����ړ�����
	short move_limit_z;			///<Z�����ړ�����
	unsigned short gx;			///<�O���b�hX
	unsigned short gz;			///<�O���b�hZ
	int gy;						///<Y�l fx32�^
}FLDMMDL_HEADER;

//--------------------------------------------------------------
///	FLDMMDL_SAVEDATA�\����
//--------------------------------------------------------------
typedef struct
{
	u32 status_bit;			///<�X�e�[�^�X�r�b�g
	u32 move_bit;			///<����r�b�g
	u8 obj_id;				///<OBJ ID
	u8 move_code;			///<����R�[�h
	s8 move_limit_x;		///<X�����ړ�����
	s8 move_limit_z;		///<Z�����ړ�����
	s8 dir_head;			///<FLDMMDL_H�w�����
	s8 dir_disp;			///<���݌����Ă������
	s8 dir_move;			///<���ݓ����Ă������
	u8 dummy;				///<�_�~�[
	u16 zone_id;			///<�]�[�� ID
	u16 obj_code;			///<OBJ�R�[�h
	u16 event_type;			///<�C�x���g�^�C�v
	u16 event_flag;			///<�C�x���g�t���O
	u16 event_id;			///<�C�x���gID
	s16 param0;				///<�w�b�_�w��p�����^
	s16 param1;				///<�w�b�_�w��p�����^
	s16 param2;				///<�w�b�_�w��p�����^
	s16 gx_init;			///<�����O���b�hX
	s16 gy_init;			///<�����O���b�hY
	s16 gz_init;			///<�����O���b�hZ
	s16 gx_now;				///<���݃O���b�hX
	s16 gy_now;				///<���݃O���b�hY
	s16 gz_now;				///<���݃O���b�hZ
	fx32 fx32_y;			///<fx32�^�̍����l
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];///<����֐��p���[�N
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];///<����T�u�֐��p���[�N
}FLDMMDL_SAVEDATA;

///<FLDMMDL_SAVE_DATA size
#define FLDMMDL_SAVE_DATA_SIZE (sizeof(FLDMMDL_SAVE_DATA))

#endif //FLDMMDL_HEADER_FILE
