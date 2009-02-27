//======================================================================
/**
 *
 * @file	fldmmdl.h
 * @brief	�t�B�[���h���샂�f��
 * @author	kagaya
 * @data	05.07.13
 *
 */
//======================================================================
#ifndef FLDMMDL_H_FILE
#define FLDMMDL_H_FILE

#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "field_g3d_mapper.h"

#include "fldmmdl_code.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#define FLDMMDL_PL_NULL	//��`�Ńv���`�i�ōs���Ă�������������
//#define FLDMMDL_BLACT_HEAD3_TEST //��`�ŎO���g�e�X�g

//--------------------------------------------------------------
//	fx
//--------------------------------------------------------------
#define NUM_FX32(a)	(FX32_ONE*(a))	///<����->FX32�^��
#define FX32_NUM(a)	((a)/FX32_ONE)	///<FX32->������
#define NUM_FX16(a)	(FX16_ONE*(a))	///<����->FX16�^��
#define FX16_NUM(a)	((a)/FX16_ONE)	///<FX16->������

//--------------------------------------------------------------
//	�O���b�h
//--------------------------------------------------------------
#define GRID_ONE	(1)	///<1�O���b�h
#define GRID		(16)///<1�O���b�h�P�ʁB�����W16
#define GRID_HALF	(GRID>>1)///<1/2�O���b�h
#define GRID_SHIFT	(4)///<�O���b�h�����W�ϊ��ɕK�v�ȃV�t�g��
#define GRID_SIZE(a) ((a)<<GRID_SHIFT)///<�O���b�h���T�C�Y�ϊ�
#define SIZE_GRID(a) ((a)>>GRID_SHIFT)///<���T�C�Y�O���b�h�ϊ�
#define GRID_SIZE_FX32(a) (GRID_SIZE(a)*FX32_ONE)///<�O���b�h���T�C�YFX32�ϊ�
#define SIZE_GRID_FX32(a) (SIZE_GRID(a)/FX32_ONE)///<���T�C�Y�O���b�hFX32�ϊ�
#define GRID_FX32 (GRID*FX32_ONE) ///<1�O���b�h����
#define GRID_HALF_FX32 (GRID_FX32>>1) ///<1/2�O���b�h����

//--------------------------------------------------------------
//	�����O���b�h
//--------------------------------------------------------------
#if 0
#define H_GRID_ONE	(1)		///<1�O���b�h
#define H_GRID		(8)		///<1�O���b�h�P�ʁB�����W8
#define H_GRID_2	(H_GRID*2)

#define H_GRID_HALF	(H_GRID>>1)	///<1/2�O���b�h
#define H_GRID_SHIFT	(3)		///<�O���b�h�����W�ϊ��ɕK�v�ȃV�t�g��
#define H_GRID_SIZE(a) ((a)<<H_GRID_SHIFT) ///<�O���b�h���T�C�Y�ϊ�
#define SIZE_H_GRID(a) ((a)>>H_GRID_SHIFT) ///<���T�C�Y�O���b�h�ϊ�
#define H_GRID_SIZE_FX32(a) (H_GRID_SIZE(a)*FX32_ONE) ///<�O���b�h���T�C�YFX32�ϊ�
#define SIZE_H_GRID_FX32(a) (SIZE_H_GRID(a)/FX32_ONE) ///<���T�C�Y�O���b�hFX32�ϊ�
#define H_GRID_FX32 (H_GRID*FX32_ONE) ///<1�O���b�h����

#define H_GRID_FELLOW_SIZE (H_GRID_ONE*2) ///<OBJ���m�̓����蔻��@�����q�b�g�T�C�Y
#define H_GRID_G_GRID(a) ((a)/2) ///<�����O���b�h->�n�ʃO���b�h�T�C�Y��
#define G_GRID_H_GRID(a) ((a)*2) ///<�n�ʃO���b�h->�����O���b�h�T�C�Y��
#endif

///<OBJ���m�̓����蔻��@�����q�b�g�T�C�Y
#define H_GRID_FELLOW_SIZE (GRID_ONE)

//--------------------------------------------------------------
//	�ړ�����
//--------------------------------------------------------------
//#define MOVE_LIMIT_NOT (0xffff) ///<�ړ���������>�Z�[�u�f�[�^s8�ׁ̈A�ύX
#define	MOVE_LIMIT_NOT (-1) ///<�ړ���������

//--------------------------------------------------------------
//	���x
//--------------------------------------------------------------
#define FRAME_1	(1)			///<1�t���[��
#define FRAME_1_SYNC (2)	///<1�t���[����V�V���N��
#define GRID_FRAME_1_FX32 (FX32_ONE/FRAME_1_SYNC) ///<�O���b�h1frame�ړ���

#define GRID_FRAME_32 (32)
#define GRID_FRAME_16 (16)
#define GRID_FRAME_8 (8)
#define GRID_FRAME_4 (4)
#define GRID_FRAME_2 (2)
#define GRID_FRAME_1 (1)

#define GRID_VALUE_SPEED_32 (GRID_FX32/GRID_FRAME_32)
#define GRID_VALUE_SPEED_16 (GRID_FX32/GRID_FRAME_16)
#define GRID_VALUE_SPEED_8 (GRID_FX32/GRID_FRAME_8)
#define GRID_VALUE_SPEED_4 (GRID_FX32/GRID_FRAME_4)
#define GRID_VALUE_SPEED_2 (GRID_FX32/GRID_FRAME_2)
#define GRID_VALUE_SPEED_1 (GRID_FX32/GRID_FRAME_1)

#define GRID_VALUE_SPEED_FRAME(g,f) ((GRID_FX32*g)/f)

//--------------------------------------------------------------
//	�r���{�[�h�A�j���[�V�����t���[��
//--------------------------------------------------------------
#define BLACTANM_FRAME_1 (FX32_ONE) ///<1�t���[�����x
#define BLACTANM_FRAME_2 (BLACTANM_FRAME_1*2) ///<2�t���[�����x
#define BLACTANM_FRAME_3 (BLACTANM_FRAME_1*3) ///<2�t���[�����x
#define BLACTANM_FRAME_4 (BLACTANM_FRAME_1*4) ///<4�t���[�����x
#define BLACTANM_FRAME_8 (BLACTANM_FRAME_1*8) ///<8�t���[�����x
#define BLACTANM_FRAME_05 (BLACTANM_FRAME_1/2) ///<1/2�t���[�����x
#define BLACTANM_STEP_FRAME (8) ///<����ړ��A�j���̃t���[����
#define BLACTANM_STEP_DASH_FRAME (4) ///<�_�b�V������ړ��A�j���t���[����

//--------------------------------------------------------------
//	���W�␮
//--------------------------------------------------------------
#define FLDMMDL_VEC_X_GRID_OFFS_FX32 (GRID_HALF_FX32)///<X�����O���b�h�␮
#define FLDMMDL_VEC_Y_GRID_OFFS_FX32 (0)///<Y�����O���b�h�␮
#define FLDMMDL_VEC_Z_GRID_OFFS_FX32 (GRID_HALF_FX32)///<Z�����O���b�h�␮

//--------------------------------------------------------------
//	�\�����W�␮
//--------------------------------------------------------------
#define FLDMMDL_BLACT_X_GROUND_OFFS_FX32 (0) ///<X�����n�ʕ␮
#define FLDMMDL_BLACT_Y_GROUND_OFFS_FX32 (0) ///<Y�����n�ʕ␮
#define FLDMMDL_BLACT_Z_GROUND_OFFS_FX32 (FX32_ONE*6) ///<Z�����n�ʕ␮

//--------------------------------------------------------------
///	FLDMMDLSYS_STABIT FLDMMDLSYS�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
typedef enum
{
	FLDMMDLSYS_STABIT_NON = (0),///<�X�e�[�^�X����
	FLDMMDLSYS_STABIT_DRAW_INIT_COMP = (1<<0),///<�`�揉��������
	FLDMMDLSYS_STABIT_MOVE_PROC_STOP = (1<<1),///<���쏈����~
	FLDMMDLSYS_STABIT_DRAW_PROC_STOP = (1<<2),///<�`�揈����~
	FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT = (1<<3),///<�e��t���Ȃ�
	FLDMMDLSYS_STABIT_MOVE_INIT_COMP = (1<<4),///<���쏈������������
}FLDMMDLSYS_STABIT;

//--------------------------------------------------------------
///	FLDMMDL_STABIT FLDMMDL�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_STABIT_NON=(0),///<�X�e�[�^�X����
	FLDMMDL_STABIT_USE=(1<<0),///<�g�p����\��
	FLDMMDL_STABIT_MOVE=(1<<1),///<�ړ�����\��
	FLDMMDL_STABIT_MOVE_START=(1<<2),///<�ړ��J�n��\��
	FLDMMDL_STABIT_MOVE_END=(1<<3),///<�ړ��I����\��
	FLDMMDL_STABIT_ACMD=(1<<4),///<�A�j���[�V�����R�}���h��
	FLDMMDL_STABIT_ACMD_END=(1<<5),///<�A�j���[�V�����R�}���h�I��
	FLDMMDL_STABIT_PAUSE_MOVE=(1<<6),///<�ړ��ꎞ��~
	FLDMMDL_STABIT_PAUSE_DIR=(1<<7),///<�\�������ꎞ��~
	FLDMMDL_STABIT_PAUSE_ANM=(1<<8),///<�A�j���ꎞ��~
	FLDMMDL_STABIT_VANISH=(1<<9),///<��\��
	FLDMMDL_STABIT_ZONE_DEL_NOT=(1<<10),///<�]�[���؂�ւ��ɂ��폜�֎~
	FLDMMDL_STABIT_ATTR_GET_ERROR=(1<<11),///<�A�g���r���[�g�擾���s
	FLDMMDL_STABIT_HEIGHT_GET_ERROR=(1<<12),///<�����擾���s
	FLDMMDL_STABIT_HEIGHT_VANISH_OFF=(1<<13),///<�����擾�s���̔�\����OFF
	FLDMMDL_STABIT_DRAW_PROC_INIT_COMP=(1<<14),///<�`�揉��������
	FLDMMDL_STABIT_SHADOW_SET=(1<<15),///<�e�Z�b�g�ς�
	FLDMMDL_STABIT_JUMP_START=(1<<16),///<�W�����v�J�n
	FLDMMDL_STABIT_JUMP_END=(1<<17),///<�W�����v�I��
	FLDMMDL_STABIT_FELLOW_HIT_NON=(1<<18),///<OBJ���m�̓����蔻�薳��
	FLDMMDL_STABIT_TALK_OFF=(1<<19),///<�b����������
	FLDMMDL_STABIT_SHADOW_VANISH=(1<<20),///<�e�\���A��\��
	FLDMMDL_STABIT_DRAW_PUSH=(1<<21),///<�`�揈����ޔ�
	FLDMMDL_STABIT_BLACT_ADD_PRAC=(1<<22),///<�r���{�[�h�A�N�^�[�ǉ���
	FLDMMDL_STABIT_HEIGHT_GET_OFF=(1<<23),///<�����擾�����Ȃ�
	FLDMMDL_STABIT_REFLECT_SET=(1<<24),///<�f�荞�݂��Z�b�g����
	FLDMMDL_STABIT_ALIES=(1<<25),///<�G�C���A�X�ł���
	FLDMMDL_STABIT_EFFSET_SHOAL=(1<<26),///<�󐣃G�t�F�N�g���Z�b�g
	FLDMMDL_STABIT_ATTR_OFFS_OFF=(1<<27),///<�A�g���r���[�g�I�t�Z�b�g�ݒ�OFF
	FLDMMDL_STABIT_BRIDGE=(1<<28),///<���ړ����ł���
	FLDMMDL_STABIT_HEIGHT_EXPAND=(1<<29),///<�g�������ɔ�������
}FLDMMDL_STABIT;

//--------------------------------------------------------------
///	FLDMMDL_MOVEBIT FLDMMDL����r�b�g
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_MOVEBIT_NON=(0),///<����
	FLDMMDL_MOVEBIT_SHADOW_SET=(1<<0),///<�e���Z�b�g����
	FLDMMDL_MOVEBIT_GRASS_SET=(1<<1),///<�����Z�b�g����
	FLDMMDL_MOVEBIT_ATTR_GET_OFF=(1<<2),///<�A�g���r���[�g�擾����؍s��Ȃ�
	FLDMMDL_MOVEBIT_MOVEPROC_INIT=(1<<3),///<���쏉�������s����
	FLDMMDL_MOVEBIT_NEED_MOVEPROC_RECOVER=(1<<4),///<���앜�A�֐����ĂԕK�v
}FLDMMDL_MOVEBIT;

//--------------------------------------------------------------
///	FLDMMDL_MOVEHITBIT FLDMMDL�ړ��q�b�g�r�b�g
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_MOVEHITBIT_NON=(0),///<�q�b�g����
	FLDMMDL_MOVEHITBIT_LIM=(1<<0),///<�ړ������ɂ��q�b�g
	FLDMMDL_MOVEHITBIT_ATTR=(1<<1),///<�A�g���r���[�g�q�b�g
	FLDMMDL_MOVEHITBIT_OBJ=(1<<2),///<OBJ���m�̏Փ�
	FLDMMDL_MOVEHITBIT_HEIGHT=(1<<3),///<���፷�ɂ��q�b�g
	FLDMMDL_MOVEHITBIT_OUTRANGE=(1<<4),///<�͈͊O�q�b�g
}FLDMMDL_MOVEHITBIT;

//--------------------------------------------------------------
///	�t�B�[���h���샂�f���w�b�_�[��������
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_PARAM_0 = 0,
	FLDMMDL_PARAM_1,
	FLDMMDL_PARAM_2,
}FLDMMDL_H_PARAM;

//--------------------------------------------------------------
///	�`��^�C�v
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_DRAWTYPE_NON = 0,		///<�`�斳��
	FLDMMDL_DRAWTYPE_BLACT,			///<�r���{�[�h�`��
	FLDMMDL_DRAWTYPE_MDL,			///<���f�����O�`��
}FLDMMDL_DRAWTYPE;

//--------------------------------------------------------------
///	�e���
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_SHADOW_NON = 0,			///<�e����
	FLDMMDL_SHADOW_ON,				///<�e�A��
}FLDMMDL_SHADOWTYPE;

//--------------------------------------------------------------
///	���Վ��
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_FOOTMARK_NON = 0,		///<���Ֆ���
	FLDMMDL_FOOTMARK_NORMAL,		///<�Q�{��
	FLDMMDL_FOOTMARK_CYCLE,			///<���]��
}FLDMMDL_FOOTMARKTYPE;

//--------------------------------------------------------------
///	�f�肱�ݎ��
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_REFLECT_NON = 0,		///<�f�肱�ݖ���
	FLDMMDL_REFLECT_BLACT,			///<�r���{�[�h�f�肱��
}FLDMMDL_REFLECTTYPE;

//--------------------------------------------------------------
///	�r���{�[�h�A�N�^�[�A�j���[�V�������
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_BLACT_ANMTBLNO_NON,		///<�A�j������
	FLDMMDL_BLACT_ANMTBLNO_HERO,	///<���@��p
	FLDMMDL_BLACT_ANMTBLNO_BLACT,	///<�ėp�A�j��
	FLDMMDL_BLACT_ANMTBLNO_MAX,		///<�ő�
}FLDMMDL_BLACT_ANMTBLNO;

//--------------------------------------------------------------
///	�`��֐��ԍ�
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_DRAWPROCNO_NON,		///<�`�斳��
	FLDMMDL_DRAWPROCNO_HERO,	///<���@��p
	FLDMMDL_DRAWPROCNO_BLACT,	///<�r���{�[�h�ėp
	FLDMMDL_DRAWPROCNO_MAX,		///<�ő�
}FLDMMDL_DRAWPROCNO;

//--------------------------------------------------------------
///	�t�B�[���h���샂�f������v���C�I���e�B�I�t�Z�b�g
//--------------------------------------------------------------
enum
{
	FLDMMDL_TCBPRI_OFFS_ST = 32,		///<��{
	FLDMMDL_TCBPRI_OFFS_PAIR,			///<�y�A����
	FLDMMDL_TCBPRI_OFFS_AFTER,			///<�t�B�[���h���샂�f��������
};

//--------------------------------------------------------------
///	�Z�[�u�p���[�N�v�f��
//--------------------------------------------------------------
#define FLDMMDL_SAVEMMDL_MAX (20)

//--------------------------------------------------------------
//	���f���ő吔
//--------------------------------------------------------------
#define FLDMMDL_MDL_MAX (20)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	�^��`
//--------------------------------------------------------------
///FLDMMDLSYS
typedef struct _TAG_FLDMMDLSYS FLDMMDLSYS;
///FLDMMDL
typedef struct _TAG_FLDMMDL FLDMMDL;
///FLDMMDL_BLACTCONT
typedef struct _TAG_FLDMMDL_BLACTCONT FLDMMDL_BLACTCONT;
///FLDMMDL_SAVEDATA
typedef struct _TAG_FLDMMDL_SAVEDATA FLDMMDL_SAVEDATA;

//--------------------------------------------------------------
///	�A�g���r���[�g�^ ��:map_attr.h
//--------------------------------------------------------------
typedef u32 MATR;

//--------------------------------------------------------------
///	�֐���`
//--------------------------------------------------------------
typedef void (*FLDMMDL_MOVE_PROC_INIT)(FLDMMDL*);///<���쏉�����֐�
typedef void (*FLDMMDL_MOVE_PROC)(FLDMMDL*);///<����֐�
typedef void (*FLDMMDL_MOVE_PROC_DEL)(FLDMMDL*);///<����폜�֐�
typedef void (*FLDMMDL_MOVE_PROC_RECOVER)(FLDMMDL*);///<���앜�A�֐�
typedef void (*FLDMMDL_DRAW_PROC_INIT)(FLDMMDL*);///<�`�揉�����֐�
typedef void (*FLDMMDL_DRAW_PROC)(FLDMMDL*);///<�`��֐�
typedef void (*FLDMMDL_DRAW_PROC_DEL)(FLDMMDL*);///<�`��폜�֐�
typedef void (*FLDMMDL_DRAW_PROC_PUSH)(FLDMMDL*);///<�`��ޔ��֐�
typedef void (*FLDMMDL_DRAW_PROC_POP)(FLDMMDL*);///<�`�敜�A�֐�
typedef u32 (*FLDMMDL_DRAW_PROC_GET)(FLDMMDL*,u32);///<�`��擾�֐�

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
	int y;						///<Y�l fx32�^
}FLDMMDL_HEADER;

//--------------------------------------------------------------
///	FLDMMDL_MOVE_PROC_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	u32 move_code;///<����R�[�h
	FLDMMDL_MOVE_PROC_INIT init_proc;///<�������֐�
	FLDMMDL_MOVE_PROC move_proc;///<����֐�
	FLDMMDL_MOVE_PROC_DEL delete_proc;///<�폜�֐�
	FLDMMDL_MOVE_PROC_RECOVER recover_proc;///<���앜���֐�
}FLDMMDL_MOVE_PROC_LIST;

///FLDMMDL_MOVE_PROC_LIST�T�C�Y
#define FLDMMDL_MOVE_PROC_LIST_SIZE (sizeof(FLDMMDL_MOVE_PROC_LIST))

//--------------------------------------------------------------
///	FLDMMDL_DRAW_PROC_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	FLDMMDL_DRAW_PROC_INIT init_proc;///<�������֐�
	FLDMMDL_DRAW_PROC draw_proc;///<�`��֐�
	FLDMMDL_DRAW_PROC_DEL delete_proc;///<�폜�֐�
	FLDMMDL_DRAW_PROC_PUSH push_proc;///<�ޔ��֐�
	FLDMMDL_DRAW_PROC_POP pop_proc;///<���A�֐�
	FLDMMDL_DRAW_PROC_GET get_proc;///<�擾�֐�
}FLDMMDL_DRAW_PROC_LIST;

///FLDMMDL_DRAW_PROC_LIST�T�C�Y
#define FLDMMDL_DRAW_PROC_LIST_SIZE (sizeof(FLDMMDL_DRAW_PROC_LIST))

//--------------------------------------------------------------
///	FLDMMDL_RESMNARC �\����
//--------------------------------------------------------------
typedef struct
{
	int id;
	int narc_id;
}FLDMMDL_RESMNARC;

#define FLDMMDL_RESMNARC_SIZE (sizeof(FLDMMDL_RESMNARC))

//--------------------------------------------------------------
///	FLDMMDL_ACMD_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	u16 code;///<���s����A�j���[�V�����R�}���h�R�[�h
	u16 num;///<code���s��
}FLDMMDL_ACMD_LIST;

#define FLDMMDL_ACMD_LIST_SIZE (sizeof(FLDMMDL_ACMD_LIST))

//--------------------------------------------------------------
///	OBJCODE_STATE�\����
//--------------------------------------------------------------
#ifndef FLDMMDL_PL_NULL
typedef struct
{
	int code;				///<OBJ�R�[�h
	u32 type_draw:4;		///<FLDMMDL_DRAWTYPE
	u32 type_shadow:2;		///<FLDMMDL_SHADOWTYPE
	u32 type_footmark:4;	///<FLDMMDL_FOOTMARKTYPE
	u32 type_reflect:2;		///<FLDMMDL_REFLECTTYPE
	u32 dmy:20;				///<bit�]��
}OBJCODE_STATE;
#endif

//--------------------------------------------------------------
///	OBJCODE_PARAM�\���́@�O���f�[�^�ƈ�v
//	0-1 OBJ�R�[�h
//	2-3 ���\�[�X�A�[�J�C�u�C���f�b�N�X 
//	4	�\���^�C�v
//	5	�����֐�
//	6	�e�\��
//	7	���Վ��
//	8	�f�荞��
//	9	���f���T�C�Y
//	10	�e�N�X�`���T�C�Y
//	11	�A�j��ID
//--------------------------------------------------------------
typedef struct
{
	u16 code;			///<OBJ�R�[�h
	u16 res_idx;		///<���\�[�X�C���f�b�N�X
	u8 draw_type;		///<FLDMMDL_DRAWTYPE
	u8 draw_proc_no;	///<FLDMMDL_DRAWPROCNO
	u8 shadow_type;		///<FLDMMDL_SHADOWTYPE
	u8 footmark_type;	///<FLDMMDL_FOOTMARKTYPE
	u8 reflect_type;	///<LDMMDL_REFLECTTYPE
	u8 mdl_size;		///<���f���T�C�Y
	u8 tex_size;		///<�e�N�X�`���T�C�Y
	u8 anm_id;			///<FLDMMDL_BLACT_ANMTBLNO
}OBJCODE_PARAM;

#define OBJCODE_PARAM_TOTAL_NUMBER_SIZE (sizeof(u32))

//--------------------------------------------------------------
///	FLDMMDL_BBDACT_ANMTBL�\����
//--------------------------------------------------------------
typedef struct
{
	const GFL_BBDACT_ANM * const *pAnmTbl;
	u32 anm_max;
}FLDMMDL_BBDACT_ANMTBL;

//======================================================================
//	extern
//======================================================================
//--------------------------------------------------------------
///	fldmmdl.h
//--------------------------------------------------------------
extern u32 FLDMMDL_BUFFER_GetWorkSize( void );
extern void FLDMMDL_BUFFER_InitBuffer( void *p );

extern FLDMMDLSYS * FLDMMDLSYS_CreateSystem( HEAPID heapID, u32 max );
extern void FLDMMDLSYS_FreeSystem( FLDMMDLSYS *fos );

extern void FLDMMDLSYS_SetupProc(
	FLDMMDLSYS *fos, HEAPID heapID, const FLDMAPPER *pG3DMapper );
extern void FLDMMDLSYS_DeleteProc( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_UpdateProc( FLDMMDLSYS *fos );

extern void FLDMMDLSYS_SetupDrawProc( FLDMMDLSYS *fos );

extern FLDMMDL * FLDMMDLSYS_AddFldMMdl(
	const FLDMMDLSYS *fos, const FLDMMDL_HEADER *header, int zone_id );
extern void FLDMMDL_Delete( FLDMMDL * fmmdl );
extern void FLDMMDLSYS_DeleteMMdl( const FLDMMDLSYS *fos );

extern BOOL FLDMMDL_CheckPossibleAcmd( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetAcmd( FLDMMDL * fmmdl, u16 code );
extern void FLDMMDL_SetLocalAcmd( FLDMMDL * fmmdl, u16 code );
extern BOOL FLDMMDL_CheckEndAcmd( const FLDMMDL * fmmdl );
extern BOOL FLDMMDL_EndAcmd( FLDMMDL * fmmdl );
extern void FLDMMDL_FreeAcmd( FLDMMDL * fmmdl );

extern u32 FLDMMDL_SAVEDATA_GetWorkSize( void );
extern void FLDMMDL_SAVEDATA_Init( void *p );
extern void FLDMMDL_SAVEDATA_Save(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_SAVEDATA *savedata );
extern void FLDMMDL_SAVEDATA_Load(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_SAVEDATA *savedata );

extern u32 FLDMMDLSYS_CheckStatusBit(
	const FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit );
extern u16 FLDMMDLSYS_GetFldMMdlMax( const FLDMMDLSYS *fmmdlsys );
extern u16 FLDMMDLSYS_GetFldMMdlCount( const FLDMMDLSYS *fmmdlsys );
extern u16 FLDMMDLSYS_GetTCBPriority( const FLDMMDLSYS *fmmdlsys );
extern HEAPID FLDMMDLSYS_GetHeapID( const FLDMMDLSYS *fmmdlsys );
extern const FLDMMDL * FLDMMDLSYS_GetFldMMdlBuffer(
		const FLDMMDLSYS *fmmdlsys );
extern void FLDMMDLSYS_SetArcHandle(
		FLDMMDLSYS *fmmdlsys, ARCHANDLE *handle );
extern ARCHANDLE * FLDMMDLSYS_GetArcHandle( FLDMMDLSYS *fmmdlsys );
extern GFL_TCBSYS * FLDMMDLSYS_GetTCBSYS( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_SetBlActCont(
		FLDMMDLSYS *fmmdlsys, FLDMMDL_BLACTCONT *pBlActCont );
extern FLDMMDL_BLACTCONT * FLDMMDLSYS_GetBlActCont( FLDMMDLSYS *fmmdlsys );
extern const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos );

extern void FLDMMDL_OnStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit );
extern void FLDMMDL_OffStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit );
extern FLDMMDL_STABIT FLDMMDL_GetStatusBit( const FLDMMDL * fmmdl );
extern u32 FLDMMDL_CheckStatusBit(const FLDMMDL *fmmdl, FLDMMDL_STABIT bit);
extern FLDMMDL_MOVEBIT FLDMMDL_GetMoveBit( const FLDMMDL * fmmdl );
extern void FLDMMDL_OnMoveBit( FLDMMDL * fmmdl, FLDMMDL_MOVEBIT bit );
extern void FLDMMDL_OffMoveBit( FLDMMDL * fmmdl, FLDMMDL_MOVEBIT bit );
extern u32 FLDMMDL_CheckMoveBit(const FLDMMDL *fmmdl,FLDMMDL_MOVEBIT bit);
extern void FLDMMDL_SetOBJID( FLDMMDL * fmmdl, u16 obj_id );
extern u16 FLDMMDL_GetOBJID( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetZoneID( FLDMMDL * fmmdl, u16 zone_id );
extern u16 FLDMMDL_GetZoneID( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetOBJCode( FLDMMDL * fmmdl, u16 code );
extern u16 FLDMMDL_GetOBJCode( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetMoveCode( FLDMMDL * fmmdl, u16 code );
extern u16 FLDMMDL_GetMoveCode( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetEventType( FLDMMDL * fmmdl, u16 type );
extern u16 FLDMMDL_GetEventType( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetEventFlag( FLDMMDL * fmmdl, u16 flag );
extern u16 FLDMMDL_GetEventFlag( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetEventID( FLDMMDL * fmmdl, u16 id );
extern u16 FLDMMDL_GetEventID( const FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckAliesEventID( const FLDMMDL * fmmdl );
extern u32 FLDMMDL_GetDirHeader( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetForceDirDisp( FLDMMDL * fmmdl, u16 dir );
extern void FLDMMDL_SetDirDisp( FLDMMDL * fmmdl, u16 dir );
extern u16 FLDMMDL_GetDirDisp( const FLDMMDL * fmmdl );
extern u16 FLDMMDL_GetDirDispOld( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetDirMove( FLDMMDL * fmmdl, u16 dir );
extern u16 FLDMMDL_GetDirMove( const FLDMMDL * fmmdl );
extern u16 FLDMMDL_GetDirMoveOld( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetDirAll( FLDMMDL * fmmdl, u16 dir );
extern void FLDMMDL_SetParam(FLDMMDL *fmmdl, int param, FLDMMDL_H_PARAM no);
extern int FLDMMDL_GetParam(const FLDMMDL * fmmdl, FLDMMDL_H_PARAM param);
extern void FLDMMDL_SetMoveLimitX( FLDMMDL * fmmdl, s16 x );
extern s16 FLDMMDL_GetMoveLimitX( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetMoveLimitZ( FLDMMDL * fmmdl, s16 z );
extern s16 FLDMMDL_GetMoveLimitZ( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetDrawStatus( FLDMMDL * fmmdl, u16 st );
extern u16 FLDMMDL_GetDrawStatus( const FLDMMDL * fmmdl );
extern const FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL * fmmdl );
extern void * FLDMMDL_InitMoveProcWork( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_GetMoveProcWork( FLDMMDL * fmmdl );
extern void * FLDMMDL_InitMoveSubProcWork( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_GetMoveSubProcWork( FLDMMDL * fmmdl );
extern void * FLDMMDL_InitMoveCmdWork( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_GetMoveCmdWork( FLDMMDL * fmmdl );
extern void * FLDMMDL_InitDrawProcWork( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_GetDrawProcWork( FLDMMDL * fmmdl );
extern void FLDMMDL_CallMoveInitProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallMoveProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallMoveDeleteProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallMovePopProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallDrawInitProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallDrawProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallDrawDeleteProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallDrawPushProc( FLDMMDL * fmmdl );
extern void FLDMMDL_CallDrawPopProc( FLDMMDL * fmmdl );
extern u32 FLDMMDL_CallDrawGetProc( FLDMMDL *fmmdl, u32 state );
extern void FLDMMDL_SetAcmdCode( FLDMMDL * fmmdl, u16 code );
extern u16 FLDMMDL_GetAcmdCode( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetAcmdSeq( FLDMMDL * fmmdl, u16 no );
extern void FLDMMDL_IncAcmdSeq( FLDMMDL * fmmdl );
extern u16 FLDMMDL_GetAcmdSeq( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetMapAttr( FLDMMDL * fmmdl, u32 attr );
extern u32 FLDMMDL_GetMapAttr( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetMapAttrOld( FLDMMDL * fmmdl, u32 attr );
extern u32 FLDMMDL_GetMapAttrOld( const FLDMMDL * fmmdl );
extern const FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL *fmmdl );
extern u16 FLDMMDL_GetZoneIDAlies( const FLDMMDL * fmmdl );
extern s16 FLDMMDL_GetInitGridPosX( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetInitGridPosX( FLDMMDL * fmmdl, s16 x );
extern s16 FLDMMDL_GetInitGridPosY( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetInitGridPosY( FLDMMDL * fmmdl, s16 y );
extern s16 FLDMMDL_GetInitGridPosZ( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetInitGridPosZ( FLDMMDL * fmmdl, s16 z );
extern s16 FLDMMDL_GetOldGridPosX( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetOldGridPosX( FLDMMDL * fmmdl, s16 x );
extern s16 FLDMMDL_GetOldGridPosY( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetOldGridPosY( FLDMMDL * fmmdl, s16 y );
extern s16 FLDMMDL_GetOldGridPosZ( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetOldGridPosZ( FLDMMDL * fmmdl, s16 z );
extern s16 FLDMMDL_GetGridPosX( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetGridPosX( FLDMMDL * fmmdl, s16 x );
extern void FLDMMDL_AddGridPosX( FLDMMDL * fmmdl, s16 x );
extern s16 FLDMMDL_GetGridPosY( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetGridPosY( FLDMMDL * fmmdl, s16 y );
extern void FLDMMDL_AddGridPosY( FLDMMDL * fmmdl, s16 y );
extern s16 FLDMMDL_GetGridPosZ( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetGridPosZ( FLDMMDL * fmmdl, s16 z );
extern void FLDMMDL_AddGridPosZ( FLDMMDL * fmmdl, s16 z );
extern void FLDMMDL_GetVectorPos( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_SetVectorPos( FLDMMDL * fmmdl, const VecFx32 *vec );
extern fx32 FLDMMDL_GetVectorPosY( const FLDMMDL * fmmdl );
extern void FLDMMDL_GetVectorDrawOffsetPos(
		const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_SetVectorDrawOffsetPos(
		FLDMMDL * fmmdl, const VecFx32 *vec );
extern void FLDMMDL_GetVectorOuterDrawOffsetPos(
		const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_SetVectorOuterDrawOffsetPos(
		FLDMMDL * fmmdl, const VecFx32 *vec );
extern void FLDMMDL_GetVectorAttrDrawOffsetPos(
		const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_SetVectorAttrDrawOffsetPos(
		FLDMMDL * fmmdl, const VecFx32 *vec );
extern s16 FLDMMDL_GetHeightGrid( const FLDMMDL * fmmdl );
extern FLDMMDL_BLACTCONT * FLDMMDL_GetBlActCont( FLDMMDL *fmmdl );

extern BOOL FLDMMDLSYS_CheckCompleteDrawInit( const FLDMMDLSYS *fmmdlsys );
extern void FLDMMDLSYS_SetCompleteDrawInit( FLDMMDLSYS *fmmdlsys, BOOL flag );
extern void FLDMMDLSYS_SetJoinShadow( FLDMMDLSYS *fos, BOOL flag );
extern BOOL FLDMMDLSYS_CheckJoinShadow( const FLDMMDLSYS *fos );

extern void FLDMMDLSYS_StopProc( FLDMMDLSYS *fmmdlsys );
extern void FLDMMDLSYS_PlayProc( FLDMMDLSYS *fmmdlsys );
extern void FLDMMDLSYS_PauseMoveProc( FLDMMDLSYS *fmmdlsys );
extern void FLDMMDLSYS_ClearPauseMoveProc( FLDMMDLSYS *fmmdlsys );

extern u32 FLDMMDL_CheckFldMMdlSysStatusBit(
	const FLDMMDL *fmmdl, FLDMMDLSYS_STABIT bit );
extern BOOL FLDMMDL_CheckStatusBitUse( const FLDMMDL *fmmdl );
extern void FLDMMDL_OnStatusBitMove( FLDMMDL *fmmdl );
extern void FLDMMDL_OffStatusBitMove( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitMove( const FLDMMDL *fmmdl );
extern void FLDMMDL_OnStatusBitMoveStart( FLDMMDL * fmmdl );
extern void FLDMMDL_OffStatusBitMoveStart( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitMoveStart( const FLDMMDL * fmmdl );
extern void FLDMMDL_OnStatusBitMoveEnd( FLDMMDL * fmmdl );
extern void FLDMMDL_OffStatusBitMoveEnd( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitMoveEnd( const FLDMMDL * fmmdl );
extern void FLDMMDL_OnStatusBitCompletedDrawInit( FLDMMDL * fmmdl );
extern void FLDMMDL_OffStatusBitCompletedDrawInit( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitCompletedDrawInit(const FLDMMDL * fmmdl);
extern BOOL FLDMMDL_CheckStatusBitVanish( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitVanish( FLDMMDL * fmmdl, BOOL flag );
extern void FLDMMDL_SetStatusBitFellowHit( FLDMMDL * fmmdl, BOOL flag );
extern void FLDMMDL_SetStatusBitMove( FLDMMDL * fmmdl, int flag );
extern BOOL FLDMMDL_CheckStatusBitTalk( FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitTalkOFF( FLDMMDL * fmmdl, BOOL flag );
extern void FLDMMDL_OnStatusBitMoveProcPause( FLDMMDL * fmmdl );
extern void FLDMMDL_OffStatusBitMoveProcPause( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitMoveProcPause( const FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckCompletedDrawInit( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitHeightGetOFF( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitHeightGetOFF( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitNotZoneDelete( FLDMMDL * fmmdl, BOOL flag );
extern void FLDMMDL_SetStatusBitAlies( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitAlies( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitShoalEffect( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitShoalEffect( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitAttrOffsetOFF( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitAttrOffsetOFF( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitBridge( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitBridge( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitReflect( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitReflect( const FLDMMDL * fmmdl );
extern BOOL FLDMMDL_CheckStatusBitAcmd( const FLDMMDL * fmmdl );
extern void FLDMMDL_SetStatusBitHeightExpand( FLDMMDL * fmmdl, BOOL flag );
extern BOOL FLDMMDL_CheckStatusBitHeightExpand( const FLDMMDL * fmmdl );

extern void FLDMMDL_SetMoveBitAttrGetOFF( FLDMMDL * fmmdl, BOOL flag );
extern int FLDMMDL_CheckMoveBitAttrGetOFF( const FLDMMDL * fmmdl );

extern BOOL FLDMMDLSYS_SearchUseFldMMdl(
	const FLDMMDLSYS *fos, FLDMMDL **fmmdl, u32 *no );
extern FLDMMDL * FLDMMDLSYS_SearchGridPos(
	const FLDMMDLSYS *sys, s16 x, s16 z, BOOL old_hit );
extern FLDMMDL * FLDMMDLSYS_SearchMoveCode(
	const FLDMMDLSYS *fos, u16 mv_code );
extern FLDMMDL * FLDMMDLSYS_SearchOBJID( const FLDMMDLSYS *fos, u16 id );

extern BOOL FLDMMDL_SearchUseOBJCode( const FLDMMDL *fmmdl, u16 code );
extern void FLDMMDL_InitPosition(
	FLDMMDL * fmmdl, const VecFx32 *vec, u16 dir );
extern void FLDMMDL_ChangeMoveCode( FLDMMDL *fmmdl, u16 code );
extern void FLDMMDL_ChangeOBJID( FLDMMDL * fmmdl, u16 id );
extern BOOL FLDMMDL_CheckSameID(
	const FLDMMDL * fmmdl, u16 obj_id, int zone_id );
extern BOOL FLDMMDL_CheckSameIDCode(
	const FLDMMDL * fmmdl, u16 code, u16 obj_id, int zone_id );

extern const OBJCODE_PARAM * FLDMMDLSYS_GetOBJCodeParam(
		const FLDMMDLSYS *fmmdlsys, u16 code );
extern const OBJCODE_PARAM * FLDMMDL_GetOBJCodeParam(
		const FLDMMDL *fmmdl, u16 code );

extern void FLDMMDL_MoveInitProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDeleteProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveReturnProcDummy( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawInitProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawDeleteProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawPushProcDummy( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawPopProcDummy( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fldmmdl_movedata.c
//--------------------------------------------------------------
extern const FLDMMDL_MOVE_PROC_LIST * const DATA_FieldOBJMoveProcListTbl[];
extern int (* const * const DATA_AcmdActionTbl[ACMD_MAX])( FLDMMDL * );
extern const int * const DATA_AcmdCodeDirChangeTbl[];

//--------------------------------------------------------------
//	fldmmdl_drawdata.c
//--------------------------------------------------------------
extern const FLDMMDL_DRAW_PROC_LIST * const
	DATA_FLDMMDL_DRAW_PROC_LIST_Tbl[FLDMMDL_DRAWPROCNO_MAX];

extern const FLDMMDL_BBDACT_ANMTBL
	DATA_FLDMMDL_BBDACT_ANM_ListTable[FLDMMDL_BLACT_ANMTBLNO_MAX];

#ifndef FLDMMDL_PL_NULL
extern const OBJCODE_STATE DATA_FieldOBJCodeDrawStateTbl[];
#endif

#ifndef FLDMMDL_PL_NULL
extern const FLDMMDL_DRAW_PROC_LIST DATA_FieldOBJDraw_Non;

extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_RenderOBJ[];
extern const int DATA_FIELDOBJ_RenderOBJMax;

extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Tex[];
extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Mdl[];
extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Anm[];
extern const int DATA_FLDMMDL_Regular_Tex[];
extern const int DATA_FLDMMDL_Regular_Mdl[];
extern const int DATA_FLDMMDL_Regular_Anm[];
extern const int DATA_FLDMMDL_BlActFogEnableOFFTbl[];
#endif

//--------------------------------------------------------------
//	fldmmdl_move.c
//--------------------------------------------------------------
extern void FLDMMDL_InitMoveProc( FLDMMDL * fmmdl );
extern void FLDMMDL_UpdateMove( FLDMMDL * fmmdl );

extern u32 FLDMMDL_HitCheckMove( const FLDMMDL *fmmdl,
	const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir );
extern u32 FLDMMDL_HitCheckMoveDir( const FLDMMDL * fmmdl, u16 dir );
extern int FLDMMDL_HitCheckMoveFellow(
	const FLDMMDL * fmmdl, s16 x, s16 y, s16 z );
extern int FLDMMDL_HitCheckMoveLimit(
	const FLDMMDL * fmmdl, s16 x, s16 y, s16 z );
extern BOOL FLDMMDL_GetMapPosAttr(
	const FLDMMDL *fmmdl, const VecFx32 *pos, u32 *attr );
extern BOOL FLDMMDL_GetMapPosHeight(
	const FLDMMDL *fmmdl, const VecFx32 *pos, fx32 *height );

extern void FLDMMDL_UpdateGridPosDir( FLDMMDL * fmmdl, u16 dir );
extern void FLDMMDL_UpdateGridPosCurrent( FLDMMDL * fmmdl );
extern u32 FLDMMDL_GetMapDirAttr( FLDMMDL * fmmdl, u16 dir );

extern void FLDMMDL_AddVectorPos( FLDMMDL * fmmdl, const VecFx32 *val );
extern void FLDMMDL_AddVectorPosDir( FLDMMDL * fmmdl, u16 dir, fx32 val );
extern BOOL FLDMMDL_UpdateCurrentHeight( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_UpdateCurrentMapAttr( FLDMMDL * fmmdl );

extern void FLDMMDL_CallMoveProcAfterDrawInit( FLDMMDL * fmmdl );

extern s16 FLDMMDL_TOOL_GetDirAddValueGridX( u16 dir );
extern s16 FLDMMDL_TOOL_GetDirAddValueGridZ( u16 dir );
extern void FLDMMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val );
extern void FLDMMDL_TOOL_GetCenterGridPos( s16 gx, s16 gz, VecFx32 *vec );
extern void FLDMMDL_TOOL_GetVectorPosGrid(
	s16 *gx, s16 *gy, s16 *gz, const VecFx32 *vec );
extern u16 FLDMMDL_TOOL_FlipDir( u16 dir );
extern u16 FLDMMDL_TOOL_GetRangeDir( int ax, int az, int bx, int bz );

//--------------------------------------------------------------
//	fldmmdl_move_1.c
//--------------------------------------------------------------
extern void FLDMMDL_MoveSubProcInit( FLDMMDL * fmmdl );
extern int FLDMMDL_MoveSub( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fldmmdl_move_2.c
//--------------------------------------------------------------
extern void FLDMMDL_MoveHidePullOffFlagSet( FLDMMDL * fmmdl );

#ifndef FLDMMDL_PL_NULL
extern void FLDMMDL_MoveHideEoaPtrSet( FLDMMDL * fmmdl, EOA_PTR eoa );
extern EOA_PTR FLDMMDL_MoveHideEoaPtrGet( FLDMMDL * fmmdl );
#endif

//--------------------------------------------------------------
//	fldmmdl_draw.c
//--------------------------------------------------------------
extern void FLDMMDLSYS_InitDraw( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_DeleteDraw( FLDMMDLSYS *fos );

extern void FLDMMDL_UpdateDraw( FLDMMDL * fmmdl );

#ifndef FLDMMDL_PL_NULL
extern const OBJCODE_STATE * FLDMMDL_TOOL_GetOBJCodeState( u16 code );
extern const OBJCODE_STATE * FLDMMDL_GetOBJCodeState( const FLDMMDL *fmmdl );
#endif

extern BOOL FLDMMDL_CheckDrawPause( const FLDMMDL * fmmdl );
extern void * FLDMMDL_DrawArcDataAlloc(
		const FLDMMDLSYS *fos, u32 datID, int fb );
extern void FLDMMDL_GetDrawVectorPos(
		const FLDMMDL * fmmdl, VecFx32 *vec );

//--------------------------------------------------------------
//	fldmmdl_acmd.c
//--------------------------------------------------------------
extern GFL_TCB * FLDMMDL_SetAcmdList(
	FLDMMDL * fmmdl, const FLDMMDL_ACMD_LIST *list );
extern BOOL FLDMMDL_CheckEndAcmdList( GFL_TCB * tcb );
extern void FLDMMDL_EndAcmdList( GFL_TCB * tcb );

extern u16 FLDMMDL_ChangeDirAcmdCode( u16 dir, u16 code );
extern u16 FLDMMDL_GetAcmdDir( u16 code );

extern void FLDMMDL_ActionAcmd( FLDMMDL * fmmdl );
extern BOOL FLDMMDL_ActionLocalAcmd( FLDMMDL * fmmdl );

//--------------------------------------------------------------
///	fldmmdl_blact.c
//--------------------------------------------------------------
extern void FLDMMDL_BLACTCONT_Setup( FLDMMDLSYS *fmmdlsys,
	GFL_BBDACT_SYS *pBbdActSys, int res_max );
extern void FLDMMDL_BLACTCONT_Release( FLDMMDLSYS *fmmdlsys );

void FLDMMDL_BLACTCONT_AddResourceTex(
	FLDMMDLSYS *fmmdlsys, const u16 *code, int max );

extern GFL_BBDACT_ACTUNIT_ID FLDMMDL_BLACTCONT_AddActor(
		FLDMMDL *fmmdl, u32 code );
extern void FLDMMDL_BLACTCONT_DeleteActor( FLDMMDL *fmmdl, u32 actID );

extern GFL_BBDACT_SYS * FLDMMDL_BLACTCONT_GetBbdActSys(
		FLDMMDL_BLACTCONT *pBlActCont );
extern GFL_BBDACT_RESUNIT_ID FLDMMDL_BLACTCONT_GetResUnitID(
		FLDMMDL_BLACTCONT *pBlActCont );

#endif //FLDMMDL_H_FILE
