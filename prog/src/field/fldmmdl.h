//==============================================================================
/**
 * @file	fldmmdl.h
 * @brief	�t�B�[���h���샂�f��
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDMMDL_H_FILE
#define FLDMMDL_H_FILE

#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "fldmmdl_header.h"
#include "fldmmdl_code.h"

#include "fieldmap_local.h"

#define FIELDMAP_TEST_FLDMMDL

//==============================================================================
//	debug
//==============================================================================
//----
//#ifdef PM_DEBUG
//----
//#define DEBUG_ALIES			//��`�ŃG�C���A�X�e�X�g
//----
//#endif	//PM_DEBUG
//----

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	fx
//--------------------------------------------------------------
#define NUM_FX32(a)	(FX32_ONE*(a))							///<����->FX32�^��
#define FX32_NUM(a)	((a)/FX32_ONE)							///<FX32->������
#define NUM_FX16(a)	(FX16_ONE*(a))							///<����->FX16�^��
#define FX16_NUM(a)	((a)/FX16_ONE)							///<FX16->������

//--------------------------------------------------------------
//	�O���b�h
//--------------------------------------------------------------
#define GRID_ONE	(1)				///<1�O���b�h
#define GRID		(16)			///<1�O���b�h�P�ʁB�����W16
#define GRID_HALF	(GRID>>1)		///<1/2�O���b�h
#define GRID_SHIFT	(4)				///<�O���b�h�����W�ϊ��ɕK�v�ȃV�t�g��
#define GRID_SIZE(a) ((a)<<GRID_SHIFT)		///<�O���b�h���T�C�Y�ϊ�
#define SIZE_GRID(a) ((a)>>GRID_SHIFT)		///<���T�C�Y�O���b�h�ϊ�
#define GRID_SIZE_FX32(a) (GRID_SIZE(a)*FX32_ONE) ///<�O���b�h���T�C�YFX32�ϊ�
#define SIZE_GRID_FX32(a) (SIZE_GRID(a)/FX32_ONE) ///<���T�C�Y�O���b�hFX32�ϊ�
#define GRID_FX32 (GRID*FX32_ONE) ///<1�O���b�h����
#define GRID_HALF_FX32 (GRID_FX32>>1) ///<1/2�O���b�h����

//--------------------------------------------------------------
//	�����O���b�h
//--------------------------------------------------------------
#if 0
#define H_GRID_ONE	(1)										///<1�O���b�h
#define H_GRID		(8)										///<1�O���b�h�P�ʁB�����W8
#define H_GRID_2	(H_GRID*2)

#define H_GRID_HALF	(H_GRID>>1)							///<1/2�O���b�h
#define H_GRID_SHIFT	(3)								///<�O���b�h�����W�ϊ��ɕK�v�ȃV�t�g��
#define H_GRID_SIZE(a) ((a)<<H_GRID_SHIFT)					///<�O���b�h���T�C�Y�ϊ�
#define SIZE_H_GRID(a) ((a)>>H_GRID_SHIFT)					///<���T�C�Y�O���b�h�ϊ�
#define H_GRID_SIZE_FX32(a) (H_GRID_SIZE(a)*FX32_ONE)		///<�O���b�h���T�C�YFX32�ϊ�
#define SIZE_H_GRID_FX32(a) (SIZE_H_GRID(a)/FX32_ONE)		///<���T�C�Y�O���b�hFX32�ϊ�
#define H_GRID_FX32 (H_GRID*FX32_ONE)						///<1�O���b�h����

#define H_GRID_FELLOW_SIZE (H_GRID_ONE*2)			///<OBJ���m�̓����蔻��@�����q�b�g�T�C�Y
#define H_GRID_G_GRID(a) ((a)/2) ///<�����O���b�h->�n�ʃO���b�h�T�C�Y��
#define G_GRID_H_GRID(a) ((a)*2) ///<�n�ʃO���b�h->�����O���b�h�T�C�Y��
#endif

///<OBJ���m�̓����蔻��@�����q�b�g�T�C�Y
#define H_GRID_FELLOW_SIZE (GRID_ONE)

//--------------------------------------------------------------
//	�ړ�����
//--------------------------------------------------------------
//#define	MOVE_LIMIT_NOT (0xffff)				///<�ړ���������->�Z�[�u�f�[�^s8�ׁ̈A�ύX
#define	MOVE_LIMIT_NOT (-1)								///<�ړ���������

//--------------------------------------------------------------
//	���x
//--------------------------------------------------------------
#define FRAME_1	(1)											///<1�t���[��
#define FRAME_1_SYNC (2)									///<1�t���[����V�V���N��
#define GRID_FRAME_1_FX32 (FX32_ONE/FRAME_1_SYNC)			///<�O���b�h1�t���[���ړ���

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
//	�r���{�[�h�A�N�^�[�A�j���[�V�����t���[��
//--------------------------------------------------------------
#define BLACTANM_FRAME_1 (FX32_ONE)							///<1�t���[�����x
#define BLACTANM_FRAME_2 (BLACTANM_FRAME_1 * 2)				///<2�t���[�����x
#define BLACTANM_FRAME_3 (BLACTANM_FRAME_1 * 3)				///<2�t���[�����x
#define BLACTANM_FRAME_4 (BLACTANM_FRAME_1 * 4)				///<4�t���[�����x
#define BLACTANM_FRAME_8 (BLACTANM_FRAME_1 * 8)				///<8�t���[�����x

#define BLACTANM_FRAME_05 (BLACTANM_FRAME_1/2)				///<1/2�t���[�����x

#define BLACTANM_STEP_FRAME (8)								///<����ړ��A�j���̃t���[����
#define BLACTANM_STEP_DASH_FRAME (4)						///<�_�b�V������ړ��A�j���t���[����

//--------------------------------------------------------------
//	���W�␮
//--------------------------------------------------------------
#define FLDMMDL_VEC_X_GRID_OFFS_FX32 (GRID_HALF_FX32)		///<X�����O���b�h�␮
#define FLDMMDL_VEC_Y_GRID_OFFS_FX32 (0)						///<Y�����O���b�h�␮
#define FLDMMDL_VEC_Z_GRID_OFFS_FX32 (GRID_HALF_FX32)		///<Z�����O���b�h�␮

//--------------------------------------------------------------
//	�\�����W�␮
//--------------------------------------------------------------
#define FLDMMDL_BLACT_X_GROUND_OFFS_FX32 (0)					///<X�����n�ʕ␮
#define FLDMMDL_BLACT_Y_GROUND_OFFS_FX32 (0)					///<Y�����n�ʕ␮
#define FLDMMDL_BLACT_Z_GROUND_OFFS_FX32 (FX32_ONE*6)		///<Z�����n�ʕ␮

//Y�����n�ʕ␮<<20050901 del saito
//#define FLDMMDL_BLACT_Y_GROUND_OFFS_FX32 (FX32_ONE*7)
//Z�����n�ʕ␮<<20050901 del saito
//#define FLDMMDL_BLACT_Z_GROUND_OFFS_FX32 (-(FX32_ONE*4))

//--------------------------------------------------------------
///	�t�B�[���h���샂�f�����������o�^FLDMMDL_AddFile()��
///	1�t���[���ɒǉ��ł���ő吔�B
/// 05.07.29 ����ł�1�t���[����4��ȏ�r���{�[�h��ǉ����鎖���o���Ȃ��B
/// 05.08.04 ������...��?
//--------------------------------------------------------------
//#define FLDMMDL_ADD_1FRAME_MAX (4)

//--------------------------------------------------------------
///	�t�B�[���h���샂�f���@V�u�����N���̃e�N�X�`���]���ő��
//--------------------------------------------------------------
//#define FLDMMDL_BLACT_VINTR_TEX_LOAD_MAX (6)
#define FLDMMDL_BLACT_VINTR_TEX_LOAD_MAX (32)

//--------------------------------------------------------------
///	�t�B�[���h���샂�f���@�Q�X�g�ǉ��Ǘ�������1�t���[������
///	�r���{�[�h�A�N�^�[��ǉ��ł���ő吔
/// 05.09.16 ����͈�̂ň�t�B�������ׂ��B
//--------------------------------------------------------------
//#define FLDMMDL_GUEST_BLACT_ADD_ONE_FRAME_MAX (1)
#define FLDMMDL_GUEST_BLACT_ADD_ONE_FRAME_MAX (4)

//--------------------------------------------------------------
///	���M�����[�@�Q�X�g����
//--------------------------------------------------------------
typedef enum
{
	REGULAR = 0,											///<���M�����[
	GUEST,													///<�Q�X�g
	NOT_REGULAR_GUEST,										///<���M�����[�ł��Q�X�g�ł�����
}REGULAR_GUEST;

//--------------------------------------------------------------
///	�e�N�X�`���o�^����
//--------------------------------------------------------------
typedef enum
{
	TEX_NON = 0,
	TEX_REGULAR,
	TEX_GUEST,
	TEX_REGULAR_RESERVE,
	TEX_GUEST_RESERVE,
}TEXREG_TYPE;

//--------------------------------------------------------------
//	���\�[�X�}�l�[�W���Ǘ���
//--------------------------------------------------------------
#define FLDMMDL_RESM_REG_MAX_MDL (4)							///<���M�����[���f��ID�Ǘ���
#define FLDMMDL_RESM_GST_MAX_MDL (4)							///<�Q�X�g���f��ID�Ǘ���
#define FLDMMDL_RESM_REG_MAX_ANM (4)							///<���M�����[�A�j��ID�Ǘ���
#define FLDMMDL_RESM_GST_MAX_ANM (4)							///<�Q�X�g�A�j��ID�Ǘ���
#define FLDMMDL_RESM_REG_MAX_TEX (24)						///<���M�����[�e�N�X�`��ID�Ǘ���
#define FLDMMDL_RESM_GST_MAX_TEX (8)							///<�Q�X�g�e�N�X�`��ID�Ǘ���

#define FLDMMDL_RESM_MAX_MDL (8)								///<���f��ID�Ǘ��ő吔
#define FLDMMDL_RESM_MAX_ANM (8)								///<�A�j��ID�Ǘ��ő吔
#define FLDMMDL_RESM_MAX_TEX (32)							///<�e�N�X�`��ID�Ǘ��ő吔
#define FLDMMDL_FRAME_TRANS_MAX (10)	//1Frame�ɓ]���ł���f�[�^���ő�

#define FLDMMDL_LRES_SIZE_MDL (0x1000)						///<���f�����O�f�[�^�T�C�Y �W��
#define FLDMMDL_LRES_SIZE_ANM (0x0080)						///<�A�j���f�[�^�T�C�Y�@�W��
#define FLDMMDL_LRES_MAXSIZE_MDL (FLDMMDL_LRES_SIZE_MDL*FLDMMDL_RESM_MAX_MDL)
#define FLDMMDL_LRES_MAXSIZE_ANM (FLDMMDL_LRES_SIZE_ANM*FLDMMDL_RESM_MAX_ANM)

//--------------------------------------------------------------
///	���M�����[�ŕK���풓���鐔
//--------------------------------------------------------------
enum
{
	FLDMMDL_RESM_REG_ALWAYS_HERO = 0,						///<���@
	FLDMMDL_RESM_REG_ALWAYS_KANBAN,							///<�Ŕ�
	FLDMMDL_RESM_REG_ALWAYS_ETC,								///<���̑�
	
	FLDMMDL_RESM_REG_ALWAYS_NUM,								///<�ő�
};

//--------------------------------------------------------------
//	FLDMMDLSYS �X�e�[�^�X�r�b�g
//--------------------------------------------------------------
#define FLDMMDLSYS_STABIT_NON				(0)				///<�X�e�[�^�X����
#define FLDMMDLSYS_STABIT_DRAW_INIT_COMP	(1<<0)			///<�`�揉��������
#define FLDMMDLSYS_STABIT_MOVE_PROC_STOP	(1<<1)	///<���쏈����~ �X�̓��쏈�����s��Ȃ�
#define FLDMMDLSYS_STABIT_DRAW_PROC_STOP	(1<<2)	///<�`�揈����~ �X�̕`�揈�����s��Ȃ�
#define FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT	(1<<3)			///<�e��t���Ȃ�

//--------------------------------------------------------------
//	FLDMMDL �X�e�[�^�X�r�b�g
//--------------------------------------------------------------

#define FLDMMDL_STABIT_NON					(0)				///<�X�e�[�^�X����
#define FLDMMDL_STABIT_USE					(1<<0)			///<�g�p����\��
#define FLDMMDL_STABIT_MOVE					(1<<1)			///<�ړ�����\��
#define FLDMMDL_STABIT_MOVE_START			(1<<2)			///<�ړ��J�n��\��
#define FLDMMDL_STABIT_MOVE_END				(1<<3)			///<�ړ��I����\��
#define FLDMMDL_STABIT_ACMD					(1<<4)			///<�A�j���[�V�����R�}���h��
#define FLDMMDL_STABIT_ACMD_END				(1<<5)			///<�A�j���[�V�����R�}���h�I��
#define FLDMMDL_STABIT_PAUSE_MOVE			(1<<6)			///<�ړ��ꎞ��~
#define FLDMMDL_STABIT_PAUSE_DIR			(1<<7)			///<�\�������ꎞ��~
#define FLDMMDL_STABIT_PAUSE_ANM			(1<<8)			///<�A�j���ꎞ��~
#define FLDMMDL_STABIT_VANISH				(1<<9)			///<��\��
#define FLDMMDL_STABIT_ZONE_DEL_NOT			(1<<10)			///<�]�[���؂�ւ��ɂ��폜�֎~
#define FLDMMDL_STABIT_ATTR_GET_ERROR		(1<<11)			///<�A�g���r���[�g�擾���s
#define FLDMMDL_STABIT_HEIGHT_GET_ERROR		(1<<12)			///<�����擾���s
#define FLDMMDL_STABIT_HEIGHT_VANISH_OFF	(1<<13)			///<�����擾�s���̔�\����OFF
#define FLDMMDL_STABIT_DRAW_PROC_INIT_COMP	(1<<14)			///<�`�揉��������
#define FLDMMDL_STABIT_SHADOW_SET			(1<<15)			///<�e�Z�b�g�ς�
#define FLDMMDL_STABIT_JUMP_START			(1<<16)			///<�W�����v�J�n
#define FLDMMDL_STABIT_JUMP_END				(1<<17)			///<�W�����v�I��
#define FLDMMDL_STABIT_FELLOW_HIT_NON		(1<<18)			///<OBJ���m�̓����蔻�薳��
#define FLDMMDL_STABIT_TALK_OFF				(1<<19)			///<�b����������
#define FLDMMDL_STABIT_SHADOW_VANISH		(1<<20)			///<�e�\���A��\��
#define FLDMMDL_STABIT_DRAW_PUSH			(1<<21)			///<�`�揈����ޔ�
#define FLDMMDL_STABIT_BLACT_ADD_PRAC		(1<<22)			///<�r���{�[�h�A�N�^�[�ǉ���
#define FLDMMDL_STABIT_HEIGHT_GET_OFF		(1<<23)			///<�����擾�����Ȃ�
#define FLDMMDL_STABIT_REFLECT_SET			(1<<24)			///<�f�荞�݂��Z�b�g����
#define FLDMMDL_STABIT_ALIES				(1<<25)			///<�G�C���A�X�ł���
#define FLDMMDL_STABIT_EFFSET_SHOAL			(1<<26)			///<�󐣃G�t�F�N�g���Z�b�g
#define FLDMMDL_STABIT_ATTR_OFFS_OFF		(1<<27)			///<�A�g���r���[�g�I�t�Z�b�g�ݒ�OFF
#define FLDMMDL_STABIT_BRIDGE				(1<<28)			///<���ړ����ł���
#define FLDMMDL_STABIT_HEIGHT_EXPAND		(1<<29)			///<�g�������ɔ�������

//--------------------------------------------------------------
///	FLDMMDL ����r�b�g
//--------------------------------------------------------------
#define FLDMMDL_MOVEBIT_NON					(0)				///<����
#define FLDMMDL_MOVEBIT_SHADOW_SET			(1<<0)			///<�e���Z�b�g����
#define FLDMMDL_MOVEBIT_GRASS_SET			(1<<1)			///<�����Z�b�g����
#define FLDMMDL_MOVEBIT_ATTR_GET_OFF		(1<<2)			///<�A�g���r���[�g�擾����؍s��Ȃ�

//--------------------------------------------------------------
//	�q�b�g�`�F�b�N�r�b�g
//--------------------------------------------------------------
#define FLDMMDL_MOVE_HIT_BIT_NON	(0)		///<�q�b�g����
#define FLDMMDL_MOVE_HIT_BIT_LIM	(1<<0)	///<�ړ������ɂ��q�b�g
#define FLDMMDL_MOVE_HIT_BIT_ATTR	(1<<1)	///<�A�g���r���[�g�q�b�g
#define FLDMMDL_MOVE_HIT_BIT_OBJ	(1<<2)	///<OBJ���m�̏Փ�
#define FLDMMDL_MOVE_HIT_BIT_HEIGHT	(1<<3)	///<���፷�ɂ��q�b�g
#define FLDMMDL_MOVE_HIT_BIT_OUTRANGE (1<<4) ///<�͈͊O�q�b�g

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
	FLDMMDL_FOOTMARK_NORMAL,			///<�Q�{��
	FLDMMDL_FOOTMARK_CYCLE,			///<���]��
}FLDMMDL_FOOTMARKTYPE;

//--------------------------------------------------------------
///	�f�肱�ݎ��
//--------------------------------------------------------------
typedef enum
{
	FLDMMDL_REFLECT_NON = 0,			///<�f�肱�ݖ���
	FLDMMDL_REFLECT_BLACT,			///<�r���{�[�h�f�肱��
}FLDMMDL_REFLECTTYPE;

//--------------------------------------------------------------
///	�t�B�[���h���샂�f������v���C�I���e�B�I�t�Z�b�g
//--------------------------------------------------------------
enum
{
	FLDMMDL_TCBPRI_OFFS_ST = 32,		///<��{
	FLDMMDL_TCBPRI_OFFS_PAIR,			///<�y�A����
	FLDMMDL_TCBPRI_OFFS_AFTER,			///<�t�B�[���h���샂�f��������
};

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
//	�^�Ē�`
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL_H						FLDMMDL_H;
typedef struct _TAG_FLDMMDL_MOVE_PROC_LIST			FLDMMDL_MOVE_PROC_LIST;
typedef struct _TAG_FLDMMDL_DRAW_PROC_LIST			FLDMMDL_DRAW_PROC_LIST;
typedef struct _TAG_FLDMMDL_DRAW_PROC_LIST_REG		FLDMMDL_DRAW_PROC_LIST_REG;
typedef struct _TAG_FLDMMDL_BLACTANM_PUSH			FLDMMDL_BLACTANM_PUSH;
//typedef struct _TAG_FLDMMDL_BLACT_HEADER			FLDMMDL_BLACT_HEADER;
typedef struct _TAG_FLDMMDL_BLACT_ADD_TBL			FLDMMDL_BLACT_ADD_TBL;
typedef struct _TAG_FLDMMDL_BLACT_CONT			FLDMMDL_BLACT_CONT;
typedef struct _TAG_FLDMMDL_RESMH					FLDMMDL_RESMH;
typedef struct _TAG_FLDMMDL_RESMNARC				FLDMMDL_RESMNARC;
//typedef struct _TAG_FLDMMDL_BLACT_HEADER_ID		FLDMMDL_BLACT_HEADER_ID;
typedef struct _TAG_FLDMMDL_ACMD_LIST				FLDMMDL_ACMD_LIST;

typedef struct _TAG_FLDMMDL_RENDER_CONT * FLDMMDL_RENDER_CONT_PTR;

//--------------------------------------------------------------
///	FLDMMDLSYS
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDLSYS FLDMMDLSYS;

//--------------------------------------------------------------
///	FLDMMDL
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL FLDMMDL;

//--------------------------------------------------------------
///	FLDMMDL_BLACTCONT
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL_BLACTCONT FLDMMDL_BLACTCONT;

//--------------------------------------------------------------
///	�֐���`
//--------------------------------------------------------------
typedef void (*FLDMMDL_MOVE_PROC_INIT)( FLDMMDL * );		///<���쏉�����֐�
typedef void (*FLDMMDL_MOVE_PROC)( FLDMMDL * );			///<����֐�
typedef void (*FLDMMDL_MOVE_PROC_DEL)( FLDMMDL * );		///<����폜�֐�
typedef void (*FLDMMDL_MOVE_PROC_RET)( FLDMMDL * );		///<���앜�A�֐�
typedef void (*FLDMMDL_DRAW_PROC_INIT)( FLDMMDL * );		///<�`�揉�����֐�
typedef void (*FLDMMDL_DRAW_PROC)( FLDMMDL * );			///<�`��֐�
typedef void (*FLDMMDL_DRAW_PROC_DEL)( FLDMMDL * );		///<�`��폜�֐�
typedef void (*FLDMMDL_DRAW_PROC_PUSH)( FLDMMDL * );		///<�`��ޔ��֐�
typedef void (*FLDMMDL_DRAW_PROC_POP)( FLDMMDL * );		///<�`�敜�A�֐�

//--------------------------------------------------------------
///	FLDMMDL_MOVE_PROC_LIST�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_MOVE_PROC_LIST
{
	int move_code;											///<����R�[�h
	FLDMMDL_MOVE_PROC_INIT init_proc;						///<�������֐�
	FLDMMDL_MOVE_PROC move_proc;							///<����֐�
	FLDMMDL_MOVE_PROC_DEL delete_proc;					///<�폜�֐�
	FLDMMDL_MOVE_PROC_RET return_proc;					///<���앜�A�֐�
};
															///FLDMMDL_MOVE_PROC_LIST�T�C�Y
#define FLDMMDL_MOVE_PROC_LIST_SIZE (sizeof(FLDMMDL_MOVE_PROC_LIST))

//--------------------------------------------------------------
///	FLDMMDL_DRAW_PROC_LIST�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_DRAW_PROC_LIST
{
	FLDMMDL_DRAW_PROC_INIT init_proc;						///<�������֐�
	FLDMMDL_DRAW_PROC draw_proc;							///<�`��֐�
	FLDMMDL_DRAW_PROC_DEL delete_proc;					///<�폜�֐�
	FLDMMDL_DRAW_PROC_PUSH push_proc;						///<�ޔ��֐�
	FLDMMDL_DRAW_PROC_POP pop_proc;						///<���A�֐�
};
															///FLDMMDL_DRAW_PROC_LIST�T�C�Y
#define FLDMMDL_DRAW_PROC_LIST_SIZE (sizeof(FLDMMDL_DRAW_PROC_LIST))

//--------------------------------------------------------------
///	FLDMMDL_DRAW_PROC_LIST_CODE_REG �\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_DRAW_PROC_LIST_REG
{
	u32 code;												///<�o�^�R�[�h
	const FLDMMDL_DRAW_PROC_LIST *list;					///<�`��֐����X�g
};
														///FLDMMDL_DRAW_PROC_LIST_REG�T�C�Y
#define FLDMMDL_DRAW_PROC_LIST_REG_SIZE (sizeof(FLDMMDL_DRAW_PROC_LIST_REG))

//--------------------------------------------------------------
///	FLDMMDL_BLACTANM_PUSH �\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_BLACTANM_PUSH
{
	u16 vanish;								///<�\���A��\��
	u16 anm_offs;							///<�A�j���I�t�Z�b�g
	fx32 frame;								///<�t���[��
};

#define FLDMMDL_BLACTANM_PUSH_SIZE (sizeof(FLDMMDL_BLACTANM_PUSH))

//--------------------------------------------------------------
///	FLDMMDL_BLACT_HEADER �\����
//--------------------------------------------------------------
#if 0
struct _TAG_FLDMMDL_BLACT_HEADER
{
	u32 obj_code;					///<OBJ�R�[�h
	BLACT_HEADER *head;				///<�r���{�[�h�A�N�^�[�w�b�_
};
										///FLDMMDL_BLACT_HEADER�T�C�Y
#define FLDMMDL_BLACT_HEADER_SIZE (sizeof(FLDMMDL_BLACT_HEADER))
#endif

//--------------------------------------------------------------
///	BLACTADD_RESERVE_WORK�\����
//--------------------------------------------------------------
#if 0
typedef struct
{
	int code;												///<�\������OBJ�R�[�h
	FLDMMDL *	fmmdl;									///<�ǉ�����Ώۂ�FLDMMDL *
	BLACT_WORK_PTR	*act;									///<�ǉ����ꂽ�r���{�[�h *�i�[��
}BLACTADD_RESERVE;

#define BLACTADD_RESERVE_SIZE (sizeof(BLACTADD_RESERVE))
#endif

//--------------------------------------------------------------
///	TEXLOAD_WORK�\����
//--------------------------------------------------------------
#if 0
typedef struct
{
	u32 arc_id;												///<���[�h����A�[�J�C�uID
	int resm_tex_id;										///<�o�^ID
	REGULAR_GUEST reg_type;									///<�o�^�^�C�v REGULAR? GUEST?
	TEXRES_MANAGER_PTR resm_tex;							///<�o�^RES(�󎯕�
}TEXLOAD_WORK;

#define TEXLOAD_WORK_SIZE (sizeof(TEXLOAD_WORK))
#endif

//--------------------------------------------------------------
///	VTEX_WORK�\����
//--------------------------------------------------------------
#if 0
typedef struct
{
	vu32 trans_end;											///<�]���I���t���O
	int resm_tex_id;										///<�o�^ID
	TEXRES_MANAGER_PTR resm_tex;							///<�o�^RES
}VTEX_WORK;

#define VTEX_WORK_SIZE (sizeof(VTEX_WORK))
#endif

//--------------------------------------------------------------
///	BLACT_VTRANS_WORK�\����
//--------------------------------------------------------------
#if 0
typedef struct
{
	s16 max;												///<�e�����o�[�ő吔
	s16 load_max;											///<���[�h�ő吔
	vs16 load_count;										///<���[�h��
	vu16 access_flag;										///<�A�N�Z�X���t���O
	BLACTADD_RESERVE *blact_reserve_work_tbl;				///<BLACTA
	TEXLOAD_WORK *tex_load_work_tbl;						///<TEXLOAD_WORK *
	VTEX_WORK *vtex_work_tbl;								///<VTEX_WORK *
	FLDMMDL_BLACT_CONT *cont;								///<FLDMMDL_BLACT_CONT *
	GFL_TCB * tcb_load;										///<TCB �f�[�^���[�h
	GFL_TCB * tcb_blactadd;									///<TCB �r���{�[�h�ǉ�
	GFL_TCB * tcb_vintr;										///<TCB VIntr
	GFL_TCB * tcb_vwait;										///<TCB VWait
}BLACT_VTRANS_WORK;

#define BLACT_VTRANS_WORK_SIZE (sizeof(BLACT_VTRANS_WORK))
#endif

//--------------------------------------------------------------
///	FLDMMDL_BLACT_CONT �\����
//--------------------------------------------------------------
#if 0
struct _TAG_FLDMMDL_BLACT_CONT
{
	int idx;											///<�r���{�[�h�A�N�^�[�C���f�b�N�X
	int max;											///<�r���{�[�h�A�N�^�[�ő吔
	
	int resm_id_reg_max_mdl;							///<���\�[�XID���M�����[�Ǘ������f��
	int resm_id_gst_max_mdl;							///<���\�[�XID�Q�X�g�Ǘ������f��
	int resm_id_reg_max_anm;							///<���\�[�XID���M�����[�Ǘ����A�j��
	int resm_id_gst_max_anm;							///<���\�[�XID�Q�X�g�Ǘ����A�j��
	int resm_id_reg_max_tex;							///<���\�[�XID���M�����[�Ǘ����e�N�X�`��
	int resm_id_gst_max_tex;							///<���\�[�XID �Q�X�g�Ǘ��� �e�N�X�`��
	int resm_id_tbl_mdl[FLDMMDL_RESM_MAX_MDL];			///<���\�[�XID���f���i�[�e�[�u��
	int resm_id_tbl_anm[FLDMMDL_RESM_MAX_ANM];			///<���\�[�XID�A�j���i�[�e�[�u��
	int resm_id_tbl_tex[FLDMMDL_RESM_MAX_TEX];			///<���\�[�XID�e�N�X�`���i�[�e�[�u��
	
//	GFL_TCB * tcb_ptr_blact_add;							///<�r���{�[�h�A�N�^�[�ǉ��Ǘ�GFL_TCB *
//	GFL_TCB * tcb_ptr_vintr;								///<V�u�����N���荞�ݏ���GFL_TCB *
	BLACT_SET_PTR blact_setptr;							///<�r���{�[�h�A�N�^�[�Z�b�g�|�C���^
//	BLACT_HEADER *blact_head_tbl;		///<�r���{�[�h�A�N�^�[�w�b�_�[�e�[�u��
//	RES_MANAGER_PTR	mdl_res_manage;						///<���f�����\�[�X�}�l�[�W���[
//	RES_MANAGER_PTR	anm_res_manage;						///<�A�j�����\�[�X�}�l�[�W���[
//	TEXRES_MANAGER_PTR	tex_res_manage;					///<�e�N�X�`�����\�[�X�}�l�[�W���[
//	FLDMMDL_BLACT_HEADER *head_manage;				///<�A�N�^�[�w�b�_�[�Ǘ�
	
	LRES *lres_mdl;										///<���f�����O�p���\�[�X
	LRES *lres_anm;										///<�A�j���p���\�[�X
	
	BLACT_VTRANS_WORK *vtrans_work;						///<BLACT_VTRANS_WORK
	
	const FLDMMDLSYS *fos;						///<FLDMMDLSYS *
};
														//FLDMMDL_BLACT_CONT�T�C�Y
#define FLDMMDL_BLACT_CONT_SIZE (sizeof(FLDMMDL_BLACT_CONT))
#endif

//--------------------------------------------------------------
///	FLDMMDL_RESMH �\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_RESMH
{
	int id;									///<�o�^����ID
	const char *path;						///<�o�^����f�[�^�ւ̃p�X
};

#define FLDMMDL_RESMH_SIZE (sizeof(FLDMMDL_RESMH))

//--------------------------------------------------------------
///	FLDMMDL_RESMNARC �\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_RESMNARC
{
	int id;
	int narc_id;
};

#define FLDMMDL_RESMNARC_SIZE (sizeof(FLDMMDL_RESMNARC))

//--------------------------------------------------------------
///	FLDMMDL_BLACT_HEADER_ID�\����
//--------------------------------------------------------------
#if 0
struct _TAG_FLDMMDL_BLACT_HEADER_ID
{
	int tex_id;											///<�e�N�X�`��ID
	int mdl_id;											///<���f��ID
	int anm_id;											///<�A�j��ID
//	const char * const head_path;						///<�w�b�_�[�p�X
	const BLACT_ANIME_TBL *anm_tbl;						///<�A�j���e�[�u�� *
};

#define FLDMMDL_BLACT_HEADER_ID_SIZE (sizeof(FLDMMDL_BLACT_HEADER_ID))
#endif

//--------------------------------------------------------------
///	FLDMMDL_ACMD_LIST�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_ACMD_LIST
{
	u16 code;					///<���s����A�j���[�V�����R�}���h�R�[�h
	u16 num;					///<code���s��
};

#define FLDMMDL_ACMD_LIST_SIZE (sizeof(FLDMMDL_ACMD_LIST))

//--------------------------------------------------------------
//	OBJCODE_SEEDDATA�\����
//--------------------------------------------------------------
#if 0
typedef struct
{
	u32 code_miki;			///<��OBJCODE
	u32 code_flower;		///<��OBJCODE
	u32 code_fruit;			///<��OBJCODE
}OBJCODE_SEEDDATA;
#endif

//--------------------------------------------------------------
///	OBJCODE_STATE�\����
//--------------------------------------------------------------
typedef struct
{
	int code;				///<OBJ�R�[�h
	u32 type_draw:4;		///<FLDMMDL_DRAWTYPE
	u32 type_shadow:2;		///<FLDMMDL_SHADOWTYPE
	u32 type_footmark:4;	///<FLDMMDL_FOOTMARKTYPE
	u32 type_reflect:2;		///<FLDMMDL_REFLECTTYPE;
	u32 dmy:20;				///<bit�]��
}OBJCODE_STATE;

//--------------------------------------------------------------
///	CODEOFFS
//--------------------------------------------------------------
typedef struct
{
	u32 code;
	VecFx32 offs;
}CODEOFFS;

typedef u32 MATR;

//==============================================================================
//	extern
//==============================================================================
//--------------------------------------------------------------
///	fldmmdl_blact.c
//--------------------------------------------------------------
extern FLDMMDL_BLACTCONT * FLDMMDL_BLACTCONT_Setup(
	FLDMMDLSYS *pFldMMdlSys, GFL_BBDACT_SYS *pBbdActSys, HEAPID heapID );
extern void FLDMMDL_BLACTCONT_Release(
	FLDMMDLSYS *pFldMMdlSys, FLDMMDL_BLACTCONT *pBlActCont );
extern GFL_BBDACT_ACTUNIT_ID FLDMMDL_BLACTCONT_AddActor(
	FLDMMDL *pFldMMdl, u32 id );
extern void FLDMMDL_BLACTCONT_DeleteActor(
	FLDMMDL *pFldMMdl, GFL_BBDACT_ACTUNIT_ID actID );

//--------------------------------------------------------------
//	fldmmdl.c
//--------------------------------------------------------------
extern HEAPID FLDMMDLSYS_GetHeapID( const FLDMMDLSYS *fos );
extern HEAPID FLDMMDL_GetHeapID( const FLDMMDL *fmmdl );
extern GFL_TCBSYS * FLDMMDLSYS_GetTCBSYS( const FLDMMDLSYS *fos );
extern GFL_TCBSYS * FLDMMDL_GetTCBSYS( FLDMMDL *fmmdl );
extern FIELD_MAIN_WORK * FLDMMDLSYS_GetFieldMainWork( FLDMMDLSYS *fldmmdlsys );
extern void FLDMMDLSYS_SetBlActCont( FLDMMDLSYS *fldmmdlsys, FLDMMDL_BLACTCONT *pBlActCont );
extern FLDMMDL_BLACTCONT * FLDMMDLSYS_GetBlActCont( FLDMMDLSYS *fldmmdlsys );

extern void FLDMMDLSYS_UpdateMove( FLDMMDLSYS *fos );

extern FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL *fmmdl );

extern void FLDMMDL_SetBlActID(
	FLDMMDL *fldmmdl, GFL_BBDACT_ACTUNIT_ID blActID );
extern GFL_BBDACT_ACTUNIT_ID FLDMMDL_GetBlActID( FLDMMDL *fldmmdl );
extern const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos );

extern FLDMMDLSYS * FLDMMDLSYS_Init(
	FIELD_MAIN_WORK *pFldMainWork,
	const FLDMAPPER *pG3DMapper, HEAPID heapID, int max );

extern void FLDMMDLSYS_DrawInit( FLDMMDLSYS *fos,
		int tex_max, int reg_tex_max,
		const int *tex_tbl, int frm_trans_max );
extern void FLDMMDLSYS_Delete( FLDMMDLSYS *sys );
extern void FLDMMDLSYS_DrawDelete( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_DeleteAll( FLDMMDLSYS *sys );

extern void FLDMMDLSYS_DeleteMMdl( FLDMMDLSYS *fos );

extern FLDMMDL * FLDMMDL_AddH(
		const FLDMMDLSYS *sys, const FLDMMDL_H *head, int zone_id );
extern FLDMMDL * FLDMMDL_AddHMake(
		const FLDMMDLSYS *sys, int x, int z, int dir, int obj, int move, int zone_id );
extern FLDMMDL * FLDMMDL_AddHEvent(
		const FLDMMDLSYS *fos, int id, int max, int zone, const FLDMMDL_H *head );
extern void FLDMMDL_DrawResetAlone( FLDMMDL * fmmdl, int code );
extern void FLDMMDL_DrawReset( FLDMMDL * fmmdl, int code );
extern void FLDMMDL_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DeleteEvent( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawDelete( FLDMMDL * fmmdl );
extern void FLDMMDL_DeleteAll( FLDMMDLSYS *sys );
extern void FLDMMDL_PushAll( FLDMMDLSYS *sys );
extern void FLDMMDL_DrawProcPushAll( FLDMMDLSYS *fos );
extern void FLDMMDL_PopAll( FLDMMDLSYS *sys );
extern void FLDMMDL_DrawProcPopAll( FLDMMDLSYS *fos );

#if 0
extern void FLDMMDLSYS_DataSaveAll(
		FIELDSYS_WORK *fsys, const FLDMMDLSYS *fos,
		FLDMMDL_SAVE_DATA_PTR save, int max );
#endif
extern void FLDMMDLSYS_DataLoadAll(
		const FLDMMDLSYS *fos, FLDMMDL_SAVE_DATA_PTR save, int max );

//extern void FLDMMDL_AddFile( const FLDMMDLSYS *sys, int zone, const char *name );
extern void FLDMMDL_AddBin(
		const FLDMMDLSYS *sys, int zone, int max, const FLDMMDL_H *bin );

extern FLDMMDL * FLDMMDLSYS_OBJIDSearch( const FLDMMDLSYS *fos, int id );
extern FLDMMDL * FLDMMDLSYS_MoveCodeSearch( const FLDMMDLSYS *fos, int mv_code );
extern int FLDMMDLSYS_FieldOBJSearch(
	const FLDMMDLSYS *fos, FLDMMDL ** fmmdl, int *no, u32 check_bit );
extern int FLDMMDL_OBJCodeUseSearch( const FLDMMDL * fmmdl, int code );
extern int FLDMMDL_EventIDAliesCheck( const FLDMMDL * fmmdl );
extern int FLDMMDL_TCBPriGet( const FLDMMDL * fmmdl, int offset );
extern int FLDMMDL_CheckSameID( const FLDMMDL * fmmdl, int obj_id, int zone_id );
extern int FLDMMDL_CheckSameIDOBJCodeIn(
		const FLDMMDL * fmmdl, int code, int obj_id, int zone_id );

extern void FLDMMDLSYS_ZoneUpdateDelete(
		FLDMMDLSYS *fos, int old_zone, int new_zone, int max, const FLDMMDL_H *head );

extern FLDMMDL * FLDMMDLSYS_OBJIDMoveCodeSearch(
		const FLDMMDLSYS *fos, int obj_id, int mv_code );

extern void FLDMMDLSYS_OBJMaxSet( FLDMMDLSYS *sys, int max );
extern int FLDMMDLSYS_OBJMaxGet( const FLDMMDLSYS *sys );
extern int FLDMMDLSYS_OBJCountGet( const FLDMMDLSYS *sys );
extern void FLDMMDLSYS_StatusBit_Init( FLDMMDLSYS *sys, u32 bit );
extern void FLDMMDLSYS_StatusBit_ON( FLDMMDLSYS *sys, u32 bit );
extern void FLDMMDLSYS_StatusBit_OFF( FLDMMDLSYS *sys, u32 bit );
extern u32 FLDMMDLSYS_StatusBit_Check( const FLDMMDLSYS *sys, u32 bit );
extern void FLDMMDLSYS_TCBStandardPrioritySet( FLDMMDLSYS *sys, int pri );
extern int FLDMMDLSYS_TCBStandardPriorityGet( const FLDMMDLSYS *sys );
extern FLDMMDL_BLACT_CONT * FLDMMDLSYS_BlActContGet( const FLDMMDLSYS *sys );
extern void FLDMMDLSYS_RenderContSet( FLDMMDLSYS *sys, FLDMMDL_RENDER_CONT_PTR render);
extern FLDMMDL_RENDER_CONT_PTR FLDMMDLSYS_RenderContGet( const FLDMMDLSYS *sys );
extern void FLDMMDLSYS_FieldOBJWorkSet( FLDMMDLSYS *sys, FLDMMDL * ptr );
extern const FLDMMDL * FLDMMDLSYS_FieldOBJWorkGet( const FLDMMDLSYS *sys );
extern FLDMMDL * FLDMMDLSYS_fmmdlWorkGet( const FLDMMDLSYS *sys );
extern void FLDMMDLSYS_FieldOBJWorkInc( const FLDMMDL ** fmmdl );
extern void FLDMMDLSYS_fmmdlWorkInc( FLDMMDL ** fmmdl );
#if 0
extern void FLDMMDLSYS_FieldSysWorkSet( FLDMMDLSYS *fos, FIELDSYS_WORK *fsys );
extern FIELDSYS_WORK * FLDMMDLSYS_FieldSysWorkGet( const FLDMMDLSYS *fos );
#else
//extern void FLDMMDLSYS_FieldSysWorkSet( FLDMMDLSYS *fos, void *fsys );
//extern void * FLDMMDLSYS_FieldSysWorkGet( const FLDMMDLSYS *fos );
#endif
extern void FLDMMDLSYS_ArcHandleSet( FLDMMDLSYS *fos, ARCHANDLE *handle );
extern ARCHANDLE * FLDMMDLSYS_ArcHandleGet( const FLDMMDLSYS *fos );

extern void FLDMMDL_StatusBit_Init( FLDMMDL * fmmdl, u32 bit );
extern u32 FLDMMDL_StatusBit_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBit_ON( FLDMMDL * fmmdl, u32 bit );
extern void FLDMMDL_StatusBit_OFF( FLDMMDL * fmmdl, u32 bit );
extern u32 FLDMMDL_StatusBit_Check( const FLDMMDL * fmmdl, u32 bit );
extern u32 FLDMMDL_StatusBit_CheckEasy( const FLDMMDL * fmmdl, u32 bit );
extern void FLDMMDL_MoveBit_Init( FLDMMDL * fmmdl, u32 bit );
extern u32 FLDMMDL_MoveBit_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_MoveBit_ON( FLDMMDL * fmmdl, u32 bit );
extern void FLDMMDL_MoveBit_OFF( FLDMMDL * fmmdl, u32 bit );
extern u32 FLDMMDL_MoveBit_Check( const FLDMMDL * fmmdl, u32 bit );
extern void FLDMMDL_OBJIDSet( FLDMMDL * fmmdl, u32 obj_id );
extern u32 FLDMMDL_OBJIDGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_ZoneIDSet( FLDMMDL * fmmdl, int zone_id );
extern int FLDMMDL_ZoneIDGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_OBJCodeSet( FLDMMDL * fmmdl, u32 code );
extern u32 FLDMMDL_OBJCodeGet( const FLDMMDL * fmmdl );
extern u32 FLDMMDL_OBJCodeGetSeedIn( const FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCodeSet( FLDMMDL * fmmdl, u32 code );
extern u32 FLDMMDL_MoveCodeGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_EventTypeSet( FLDMMDL * fmmdl, u32 type );
extern u32 FLDMMDL_EventTypeGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_EventFlagSet( FLDMMDL * fmmdl, u32 type );
extern u32 FLDMMDL_EventFlagGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_EventIDSet( FLDMMDL * fmmdl, u32 id );
extern u32 FLDMMDL_EventIDGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_DirHeaderSet( FLDMMDL * fmmdl, int dir );
extern u32 FLDMMDL_DirHeaderGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_DirDispSetForce( FLDMMDL * fmmdl, int dir );
extern void FLDMMDL_DirDispCheckSet( FLDMMDL * fmmdl, int dir );
extern int FLDMMDL_DirDispGet( const FLDMMDL * fmmdl );
extern int FLDMMDL_DirDispOldGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_DirMoveSet( FLDMMDL * fmmdl, int dir );
extern int FLDMMDL_DirMoveGet( const FLDMMDL * fmmdl );
extern int FLDMMDL_DirMoveOldGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_DirMoveDispCheckSet( FLDMMDL * fmmdl, int dir );
extern void FLDMMDL_ParamSet( FLDMMDL * fmmdl, int param, FLDMMDL_H_PARAM no );
extern int FLDMMDL_ParamGet( const FLDMMDL * fmmdl, FLDMMDL_H_PARAM param );
extern void FLDMMDL_MoveLimitXSet( FLDMMDL * fmmdl, int x );
extern int FLDMMDL_MoveLimitXGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_MoveLimitZSet( FLDMMDL * fmmdl, int z );
extern int FLDMMDL_MoveLimitZGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_DrawStatusSet( FLDMMDL * fmmdl, u32 st );
extern u32 FLDMMDL_DrawStatusGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_MoveTcbPtrSet( FLDMMDL * fmmdl, GFL_TCB * tcb );
extern GFL_TCB * FLDMMDL_MoveTcbPtrGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_MoveTcbDelete( const FLDMMDL * fmmdl );
extern void FLDMMDL_FieldOBJSysSet( FLDMMDL * fmmdl, const FLDMMDLSYS *sys );
extern const FLDMMDLSYS * FLDMMDL_FieldOBJSysGet( const FLDMMDL * fmmdl );
extern void * FLDMMDL_MoveProcWorkInit( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_MoveProcWorkGet( FLDMMDL * fmmdl );
extern void * FLDMMDL_MoveSubProcWorkInit( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_MoveSubProcWorkGet( FLDMMDL * fmmdl );
extern void * FLDMMDL_MoveCmdWorkInit( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_MoveCmdWorkGet( FLDMMDL * fmmdl );
extern void * FLDMMDL_DrawProcWorkInit( FLDMMDL * fmmdl, int size );
extern void * FLDMMDL_DrawProcWorkGet( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveInitProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC_INIT init );
extern void FLDMMDL_MoveInitProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC move );
extern void FLDMMDL_MoveProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDeleteProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC_DEL del );
extern void FLDMMDL_MoveDeleteProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveReturnProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawInitProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_INIT init );
extern void FLDMMDL_DrawInitProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC draw );
extern void FLDMMDL_DrawProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawDeleteProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_DEL del );
extern void FLDMMDL_DrawDeleteProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawPushProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_PUSH push );
extern void FLDMMDL_DrawPushProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawPopProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_POP pop );
extern void FLDMMDL_DrawPopProcCall( FLDMMDL * fmmdl );
extern void FLDMMDL_AcmdCodeSet( FLDMMDL * fmmdl, int code );
extern int FLDMMDL_AcmdCodeGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_AcmdSeqSet( FLDMMDL * fmmdl, int no );
extern void FLDMMDL_AcmdSeqInc( FLDMMDL * fmmdl );
extern int FLDMMDL_AcmdSeqGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_NowMapAttrSet( FLDMMDL * fmmdl, u32 attr );
extern u32 FLDMMDL_NowMapAttrGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_OldMapAttrSet( FLDMMDL * fmmdl, u32 attr );
extern u32 FLDMMDL_OldMapAttrGet( const FLDMMDL * fmmdl );
#if 0
extern FIELDSYS_WORK * FLDMMDL_FieldSysWorkGet( const FLDMMDL * fmmdl );
#endif
extern int FLDMMDL_TCBStandardPriorityGet( const FLDMMDL * fmmdl );
extern int FLDMMDL_ZoneIDGetAlies( const FLDMMDL * fmmdl );

#if 0
extern void FLDMMDLSYS_StatusBitON_PauseAll( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_StatusBitOFF_PauseAll( FLDMMDLSYS *fos );
#else
extern void FLDMMDLSYS_MoveStopAll( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_MoveStopAllClear( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_MovePauseAll( FLDMMDLSYS *fos );
extern void FLDMMDLSYS_MovePauseAllClear( FLDMMDLSYS *fos );
#endif
extern int FLDMMDLSYS_DrawInitCompCheck( const FLDMMDLSYS *fos );
extern u32 FLDMMDL_FieldOBJSysStatusBitCheck( const FLDMMDL * fmmdl, u32 bit );
extern void FLDMMDLSYS_ShadowJoinSet( FLDMMDLSYS *fos, int flag );
extern int FLDMMDLSYS_ShadowJoinCheck( const FLDMMDLSYS *fos );

extern int FLDMMDL_StatusBitCheck_Use( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitON_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitOFF_Move( FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_Move( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitON_MoveStart( FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitOFF_MoveStart( FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_MoveStart( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitON_MoveEnd( FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitOFF_MoveEnd( FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_MoveEnd( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitON_DrawProcInitComp( FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitOFF_DrawProcInitComp( FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_DrawProcInitComp( const FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_Vanish( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_Vanish( FLDMMDL * fmmdl, int flag );
extern void FLDMMDL_StatusBitSet_FellowHit( FLDMMDL * fmmdl, int flag );
extern void FLDMMDL_StatusBitSet_Move( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_Talk( FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_TalkOFF( FLDMMDL * fmmdl, int flag );
extern void FLDMMDL_MovePause( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePauseClear( FLDMMDL * fmmdl );
extern int FLDMMDL_MovePauseCheck( const FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBit_DrawInitCompCheck( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_HeightGetOFF( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_HeightOFFCheck( const FLDMMDL * fmmdl );
extern void FLDMMDL_NotZoneDeleteSet( FLDMMDL * fmmdl, int flag );
extern void FLDMMDL_StatusBitSet_Alies( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_Alies( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_ShoalEffectSet( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_ShoalEffectSet( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_AttrOffsOFF( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_AttrOffsOFF( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_Bridge( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_Bridge( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_Reflect( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_Reflect( const FLDMMDL * fmmdl );
extern int FLDMMDL_StatusBitCheck_Acmd( const FLDMMDL * fmmdl );
extern void FLDMMDL_StatusBitSet_HeightExpand( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_StatusBitCheck_HeightExpand( const FLDMMDL * fmmdl );

extern void FLDMMDL_MoveBitSet_AttrGetOFF( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_MoveBitCheck_AttrGetOFF( const FLDMMDL * fmmdl );


extern int FLDMMDL_InitPosGX_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_InitPosGX_Set( FLDMMDL * fmmdl, int x );
extern int FLDMMDL_InitPosGY_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_InitPosGY_Set( FLDMMDL * fmmdl, int y );
extern int FLDMMDL_InitPosGZ_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_InitPosGZ_Set( FLDMMDL * fmmdl, int z );
extern int FLDMMDL_OldPosGX_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_OldPosGX_Set( FLDMMDL * fmmdl, int x );
extern int FLDMMDL_OldPosGY_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_OldPosGY_Set( FLDMMDL * fmmdl, int y );
extern int FLDMMDL_OldPosGZ_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_OldPosGZ_Set( FLDMMDL * fmmdl, int z );
extern int FLDMMDL_NowPosGX_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_NowPosGX_Set( FLDMMDL * fmmdl, int x );
extern void FLDMMDL_NowPosGX_Add( FLDMMDL * fmmdl, int x );
extern int FLDMMDL_NowPosGY_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_NowPosGY_Set( FLDMMDL * fmmdl, int y );
extern void FLDMMDL_NowPosGY_Add( FLDMMDL * fmmdl, int y );
extern int FLDMMDL_NowPosGZ_Get( const FLDMMDL * fmmdl );
extern void FLDMMDL_NowPosGZ_Set( FLDMMDL * fmmdl, int z );
extern void FLDMMDL_NowPosGZ_Add( FLDMMDL * fmmdl, int z );
extern void FLDMMDL_VecPosGet( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_VecPosSet( FLDMMDL * fmmdl, const VecFx32 *vec );
extern const VecFx32 * FLDMMDL_VecPosPtrGet( const FLDMMDL * fmmdl );
extern fx32 FLDMMDL_VecPosYGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_VecDrawOffsGet( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_VecDrawOffsSet( FLDMMDL * fmmdl, const VecFx32 *vec );
extern VecFx32 * FLDMMDL_VecDrawOffsPtrGet( FLDMMDL * fmmdl );
extern void FLDMMDL_VecDrawOffsOutSideGet( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_VecDrawOffsOutSideSet( FLDMMDL * fmmdl, const VecFx32 *vec );
extern void FLDMMDL_VecAttrOffsGet( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_VecAttrOffsSet( FLDMMDL * fmmdl, const VecFx32 *vec );
extern int FLDMMDL_HeightGridGet( const FLDMMDL * fmmdl );

extern void FLDMMDL_H_IDSet( FLDMMDL_H *head, int id );
extern int FLDMMDL_H_IDGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_OBJCodeSet( FLDMMDL_H *head, int code );
extern int FLDMMDL_H_OBJCodeGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_MoveCodeSet( FLDMMDL_H *head, int code );
extern int FLDMMDL_H_MoveCodeGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_EventTypeSet( FLDMMDL_H *head, int type );
extern int FLDMMDL_H_EventTypeGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_EventFlagSet( FLDMMDL_H *head, int flag );
extern int FLDMMDL_H_EventFlagGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_EventIDSet( FLDMMDL_H *head, int id );
extern int FLDMMDL_H_EventIDGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_DirSet( FLDMMDL_H *head, int dir );
extern int FLDMMDL_H_DirGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_ParamSet( FLDMMDL_H *head, int param, FLDMMDL_H_PARAM no );
extern int FLDMMDL_H_ParamGet( const FLDMMDL_H *head, FLDMMDL_H_PARAM no );
extern void FLDMMDL_H_MoveLimitXSet( FLDMMDL_H *head, int x );
extern int FLDMMDL_H_MoveLimitXGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_MoveLimitZSet( FLDMMDL_H *head, int z );
extern int FLDMMDL_H_MoveLimitZGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_PosXSet( FLDMMDL_H *head, int x );
extern int FLDMMDL_H_PosXGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_PosYSet( FLDMMDL_H *head, int y );
extern int FLDMMDL_H_PosYGet( const FLDMMDL_H *head );
extern void FLDMMDL_H_PosZSet( FLDMMDL_H *head, int z );
extern int FLDMMDL_H_PosZGet( const FLDMMDL_H *head );

extern FLDMMDL * FLDMMDL_SearchGridPos(
	const FLDMMDLSYS *sys, int x, int z, int old_hit );
extern void FLDMMDL_VecPosDirInit( FLDMMDL * fmmdl, const VecFx32 *vec, int dir );
extern void FLDMMDL_GPosDirInit( FLDMMDL * fmmdl, int x, int y, int z, int dir );
extern void FLDMMDL_MoveCodeChange( FLDMMDL * fmmdl, u32 code );
extern void FLDMMDL_OBJIDChange( FLDMMDL * fmmdl, int id );

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
//	fieldobj_movedata.c
//--------------------------------------------------------------
extern const FLDMMDL_MOVE_PROC_LIST * const DATA_FieldOBJMoveProcListTbl[];
extern int (* const * const DATA_AcmdActionTbl[ACMD_MAX])( FLDMMDL * );
const int * const DATA_AcmdCodeDirChangeTbl[];

//--------------------------------------------------------------
//	fieldobj_drawdata.c
//--------------------------------------------------------------
extern const FLDMMDL_DRAW_PROC_LIST DATA_FieldOBJDraw_Non;

#if 0
extern const FLDMMDL_DRAW_PROC_LIST_REG DATA_FieldOBJDrawProcListRegTbl[];
#endif

extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_RenderOBJ[];
extern const int DATA_FIELDOBJ_RenderOBJMax;

extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Tex[];
extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Mdl[];
extern const FLDMMDL_RESMNARC DATA_FLDMMDL_ResmNArcTbl_Anm[];
extern const int DATA_FLDMMDL_Regular_Tex[];
extern const int DATA_FLDMMDL_Regular_Mdl[];
extern const int DATA_FLDMMDL_Regular_Anm[];
//extern const FLDMMDL_BLACT_HEADER_ID DATA_FLDMMDL_BlActHeaderPathIDTbl[];
extern const int DATA_FLDMMDL_BlActFogEnableOFFTbl[];

extern const OBJCODE_STATE DATA_FieldOBJCodeDrawStateTbl[];

#if 0
extern const OBJCODE_SEEDDATA DATA_OBJCodeSeedDataTbl[];
#endif

extern const CODEOFFS DATA_FieldOBJRenderOBJDrawOffset[];

//--------------------------------------------------------------
//	fieldobj_blact.c
//--------------------------------------------------------------
#if 0
extern void FLDMMDL_BlActCont_Init(
		FLDMMDL_BLACT_CONT *cont,
		const FLDMMDLSYS *fos,
		int max, int pri,
		int tex_max, int reg_tex_max, const int *rg_tex_tbl,
		int frm_trans_max );
extern void FLDMMDL_BlActCont_Delete( FLDMMDL_BLACT_CONT *cont );

extern TEXREG_TYPE FLDMMDL_BlActAddRegularGuestCode(
		FLDMMDL * fmmdl, BLACT_WORK_PTR *act, int code );
extern TEXREG_TYPE FLDMMDL_BlActAddRegularGuest( FLDMMDL * fmmdl, BLACT_WORK_PTR *act );
extern BLACT_WORK_PTR FLDMMDL_BlActAddCode( FLDMMDL * fmmdl, int code );
extern void FLDMMDL_BlActDeleteCode(
		const FLDMMDL * fmmdl, BLACT_WORK_PTR *act, int code );
extern void FLDMMDL_BlActDelete( const FLDMMDL * fmmdl, BLACT_WORK_PTR *act );
extern void FLDMMDL_BlActDeleteCodeCancel( const FLDMMDL * fmmdl, BLACT_WORK_PTR *act, int code );
extern void FLDMMDL_BlActAnmPush( BLACT_WORK_PTR act, FLDMMDL_BLACTANM_PUSH *push );
extern void FLDMMDL_BlActAnmPop( BLACT_WORK_PTR act, FLDMMDL_BLACTANM_PUSH *push );

extern int FLDMMDL_BlActHeaderGetOBJCode( const FLDMMDLSYS *fos,
		u32 code, BLACT_HEADER *head );

extern void FLDMMDL_BlActAdd_Guest( FLDMMDL * fmmdl, BLACT_WORK_PTR *act, int code );
extern void FLDMMDL_BlActResmRegularAdd_Tex( const FLDMMDLSYS *fos, int id );
extern void FLDMMDL_BlActResmRegularDelete_Tex( const FLDMMDLSYS *fos, int id );
extern void FLDMMDL_BlActResm_MdlIDCheckDelete_Guest( const FLDMMDLSYS *fos, int id );
extern int FLDMMDL_BlActResManage_MdlIDRegistCheck( const FLDMMDLSYS *fos, int id );
extern void FLDMMDL_BlActCont_ResmGuestDeleteAll(
		FLDMMDLSYS *fos, FLDMMDL_BLACT_CONT *cont );
extern int FLDMMDL_BlActCont_fmmdlSearch_OBJCode(
	FLDMMDLSYS *fos, int code, const FLDMMDL * ignore_obj );
extern int FLDMMDL_BlActCont_fmmdlSearch_MdlID(
	const FLDMMDLSYS *fos, int mdl_id, const FLDMMDL * ignore_obj );
extern int FLDMMDL_BlActCont_fmmdlSearch_AnmID(
	FLDMMDLSYS *fos, int anm_id, const FLDMMDL * ignore_obj );
extern void FLDMMDL_BlActResmRegularAdd_Anm( const FLDMMDLSYS *fos, int id );
extern void FLDMMDL_BlActResmRegularDelete_Anm( const FLDMMDLSYS *fos, int id );
extern void FLDMMDL_BlActResm_ResIDGuestUseCheckDelete(
		const FLDMMDLSYS *fos, int code, FLDMMDL * fmmdl );

extern void FLDMMDL_BlActCont_SetPtrSet( FLDMMDL_BLACT_CONT *cont, BLACT_SET_PTR set );
extern BLACT_SET_PTR FLDMMDL_BlActCont_SetPtrGet( FLDMMDL_BLACT_CONT *cont );
extern void FLDMMDL_BlActCont_MaxSet( FLDMMDL_BLACT_CONT *cont, int max );
extern int FLDMMDL_BlActCont_MaxGet( const FLDMMDL_BLACT_CONT *cont );
extern void FLDMMDL_BlActCont_MdlResManageSet(
		FLDMMDL_BLACT_CONT *cont, RES_MANAGER_PTR mdl );
extern RES_MANAGER_PTR FLDMMDL_BlActCont_MdlResManageGet( const FLDMMDL_BLACT_CONT *cont );
extern void FLDMMDL_BlActCont_AnmResManageSet(
		FLDMMDL_BLACT_CONT *cont, RES_MANAGER_PTR anm );
extern RES_MANAGER_PTR FLDMMDL_BlActCont_AnmResManageGet( const FLDMMDL_BLACT_CONT *cont );
extern void FLDMMDL_BlActCont_TexResManageSet(
		FLDMMDL_BLACT_CONT *cont, TEXRES_MANAGER_PTR tex );
extern TEXRES_MANAGER_PTR FLDMMDL_BlActCont_TexResManageGet(
		const FLDMMDL_BLACT_CONT *cont );

extern int FLDMMDL_BlActOBJCodeMdlIDGet( int code );
extern int FLDMMDL_BlActOBJCodeAnmIDGet( int code );

#if 0
BLACT_WORK_PTR FLDMMDL_MakeBlActAdd(
		u32 code, const VecFx32 *vec, BLACT_SET_PTR set, BLACT_HEADER *head );
#endif

extern void FLDMMDL_BlActAddPracFlagSet( FLDMMDL * fmmdl, int flag );
extern int FLDMMDL_BlActAddPracFlagCheck( FLDMMDL * fmmdl );

extern BOOL FLDMMDL_BlActPosSet( FLDMMDL * fmmdl, BLACT_WORK_PTR act );
extern void FLDMMDL_BlActFlagVanishSet( FLDMMDL * fmmdl, BLACT_WORK_PTR act );

extern int FLDMMDL_BlActAnmOffsNo_DirWalkGet( int dir );
extern int FLDMMDL_BlActAnmOffsNo_DirDashGet( int dir );
extern int FLDMMDL_BlActAnmOffsNo_DirTakeOFFGet( int dir );

extern BLACT_WORK_PTR FLDMMDL_BlActDummyDrawReset(
		FLDMMDL * fmmdl, int code,
		void (*dmy_del_call_proc)(void*,CONST_BLACT_WORK_PTR),
		void *dmy_del_call_proc_work );
#endif

//--------------------------------------------------------------
//	fieldobj_move.c
//--------------------------------------------------------------
extern BOOL FLDMMDL_GetMapAttr(
	const FLDMMDL *fmmdl, const VecFx32 *pos, u32 *attr );
extern BOOL FLDMMDL_GetMapHeight(
	const FLDMMDL *fmmdl, const VecFx32 *pos, fx32 *height );

extern void FLDMMDL_MoveInit( FLDMMDL * fmmdl );
extern void FLDMMDL_Move( FLDMMDL * fmmdl );

extern u32 FLDMMDL_PosHitCheck( const FLDMMDLSYS *fos, int x, int z );
extern u32 FLDMMDL_MoveHitCheck(
		const FLDMMDL * fmmdl, const VecFx32 *vec, int x, int y, int z, int dir );
extern u32 FLDMMDL_MoveHitCheckNow( const FLDMMDL * fmmdl, int x, int y, int z, int dir );
extern u32 FLDMMDL_MoveHitCheckDir( const FLDMMDL * fmmdl, int dir );
extern int FLDMMDL_MoveHitCheckFellow( const FLDMMDL * fmmdl, int x, int y, int z );
extern int FLDMMDL_MoveHitCheckLimit( const FLDMMDL * fmmdl, int x, int y, int z );
extern int FLDMMDL_MoveHitCheckAttr( const FLDMMDL * fmmdl, int x, int z, int dir );
extern int FLDMMDL_MapAttrKindCheck_Water( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_Sand( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_Snow( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_MostShallowSnow( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_Bridge( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_BridgeV( FLDMMDL * fmmdl, u32 attr );
extern int FLDMMDL_MapAttrKindCheck_BridgeH( FLDMMDL * fmmdl, u32 attr );

extern int FLDMMDL_DirAddValueGX( int dir );
extern int FLDMMDL_DirAddValueGZ( int dir );

extern void FLDMMDL_NowGPosDirAdd( FLDMMDL * fmmdl, int dir );
extern void FLDMMDL_GPosUpdate( FLDMMDL * fmmdl );
extern u32 FLDMMDL_NextDirAttrGet( FLDMMDL * fmmdl, int dir );

extern void FLDMMDL_VecPosAdd( FLDMMDL * fmmdl, const VecFx32 *val );
extern void FLDMMDL_VecPosDirAdd( FLDMMDL * fmmdl, int dir, fx32 val );
extern int FLDMMDL_VecPosNowHeightGetSet( FLDMMDL * fmmdl );
extern int FLDMMDL_NowPosMapAttrSet( FLDMMDL * fmmdl );
extern void FieldOBJTool_VecPosDirAdd( int dir, VecFx32 *vec, fx32 val );
extern void FieldOBJTool_GridCenterPosGet( int gx, int gz, VecFx32 *vec );
extern void FieldOBJTool_VectorGridPosGet( int *gx, int *gy, int *gz, const VecFx32 *vec );

extern void FLDMMDL_DrawInitAfterMoveProcCall( FLDMMDL * fmmdl );

extern int FieldOBJTool_DirFlip( int dir );
extern int FieldOBJTool_DirRange( int ax, int az, int bx, int bz );
#if 0
extern int FieldOBJTool_GetHeight( FIELDSYS_WORK *fsys, VecFx32 *vec );
extern int FieldOBJTool_GetHeightExpand( FIELDSYS_WORK *fsys, VecFx32 *vec, int eflag );
#endif

//--------------------------------------------------------------
//	fieldobj_move_0.c
//--------------------------------------------------------------
extern void FLDMMDL_MoveDirRnd_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndUL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndUR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndDL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndDR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndUDL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndUDR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndULR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndDLR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndUD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRndLR_Init( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveDirRnd_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDirRnd_Delete( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveRnd_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRndV_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRndH_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRndHLim_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRndRect_Init( FLDMMDL * fmmdl );

extern void FLDMMDL_MvRnd_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveDir_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveUp_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveDown_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveLeft_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRight_Init( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveSpin_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveSpinLeft_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveSpinRight_Init( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveRewar_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRewar_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveRoute2_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRoute2_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveRouteURLD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRLDU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDURL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLDUR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteULRD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLRDU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDULR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRDUL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLUDR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteUDRL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRLUD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDRLU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRUDL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteUDLR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLRUD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDLRU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRoute3_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveRouteUL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteUR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteDL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteLU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRouteRD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveRoute4_Move( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fieldobj_move_1.c
//--------------------------------------------------------------
extern void FLDMMDL_MoveSubProcInit( FLDMMDL * fmmdl );
extern int FLDMMDL_MoveSub( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fieldobj_move_2.c
//--------------------------------------------------------------
extern void FLDMMDL_MovePair_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePair_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePair_Delete( FLDMMDL * fmmdl );

extern void FLDMMDL_MovePairTr_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePairTr_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePairTr_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_MovePairTr_Return( FLDMMDL * fmmdl );
extern FLDMMDL * FLDMMDL_MovePairSearch( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveHideSnow_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHideSand_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHideGround_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHideKusa_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHide_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHide_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveHide_Return( FLDMMDL * fmmdl );

#if 0
extern void FLDMMDL_MoveHideEoaPtrSet( FLDMMDL * fmmdl, EOA_PTR eoa );
extern EOA_PTR FLDMMDL_MoveHideEoaPtrGet( FLDMMDL * fmmdl );
#endif
extern void FLDMMDL_MoveHidePullOffFlagSet( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveCopyU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyLGrassU_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyLGrassD_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyLGrassL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopyLGrassR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveCopy_Move( FLDMMDL * fmmdl );

extern void FLDMMDL_AlongWallL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_AlongWallR_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_AlongWallLRL_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_AlongWallLRR_Init( FLDMMDL * fmmdl );

extern void FLDMMDL_AlongWall_Move( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fieldobj_draw.c
//--------------------------------------------------------------
extern void FLDMMDL_Draw( FLDMMDL * fmmdl );

extern const OBJCODE_STATE * FLDMMDL_OBJCodeDrawStateGet( const FLDMMDL * fmmdl );

extern int FLDMMDL_DrawPauseCheck( const FLDMMDL * fmmdl );
extern void * FLDMMDL_DrawArcDataAlloc( const FLDMMDLSYS *fos, u32 datID, int fb );
extern void FLDMMDL_DrawVecPosTotalGet( const FLDMMDL * fmmdl, VecFx32 *vec );
extern void FLDMMDL_DirDispDrawSet( FLDMMDL * fmmdl, int dir );

extern void FLDMMDL_DrawNon_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawNon_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawNon_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawNon_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawNon_Pop( FLDMMDL * fmmdl );

//--------------------------------------------------------------
///	fieldobj_draw_0.c
//--------------------------------------------------------------
#if 0
extern BLACT_WORK_PTR FLDMMDL_DrawBlAct00_BlActPtrGet( FLDMMDL * fmmdl );
#endif

extern void FLDMMDL_DrawBlAct00_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00AnmNon_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00AnmOneP_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlAct00_Pop( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActPcWoman_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActKoiking_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActKoiking_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActKoiking_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActKoiking_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActKoiking_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActBird_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActHero_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHero_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroAnmNon_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHero_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHero_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHero_Pop( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActHeroCycle_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActHeroSwim_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroSp_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroWater_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroFishing_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroPoketch_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActHeroBanzai_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawModel00_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_Pop( FLDMMDL * fmmdl );

//extern void FLDMMDL_DrawModel00_Board_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_SnowBall_Init( FLDMMDL * fmmdl );
//extern void FLDMMDL_DrawModel00_Book_Init( FLDMMDL * fmmdl );
//extern void FLDMMDL_DrawModel00_Door2_Init( FLDMMDL * fmmdl );
//extern void FLDMMDL_DrawModel00_Board_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawModel00_SnowBall_Pop( FLDMMDL * fmmdl );
//extern void FLDMMDL_DrawModel00_Book_Pop( FLDMMDL * fmmdl );
//extern void FLDMMDL_DrawModel00_Door2_Pop( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawSnowBallBreakSet( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActOff_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_Draw( FLDMMDL * fmmdl );
extern u16 FLDMMDL_DrawBlActOff_RotateXGet( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_RotateXSet( FLDMMDL * fmmdl, u16 rx );
extern u16 FLDMMDL_DrawBlActOff_RotateYGet( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_RotateYSet( FLDMMDL * fmmdl, u16 rx );
extern u16 FLDMMDL_DrawBlActOff_RotateZGet( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActOff_RotateZSet( FLDMMDL * fmmdl, u16 rx );

extern void FLDMMDL_DrawBlActBldAgunomu_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActBldAgunomu_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActBldAgunomu_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActBldAgunomu_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActAgunomu_Draw( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawBlActTw7SpPoke_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActTw7SpPoke_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActTw7SpPoke_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActTw7SpPoke_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawBlActTw7SpPoke_Draw( FLDMMDL * fmmdl );

//--------------------------------------------------------------
///	fieldobj_draw_legendpoke.c
//--------------------------------------------------------------
extern void FLDMMDL_DrawLegend_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawLegend_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawLegend_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawLegend_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawLegend_Pop( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawReiAiHai_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawReiAiHai_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawReiAiHai_Draw( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fieldobj_acmd.c
//--------------------------------------------------------------
extern int FLDMMDL_AcmdSetCheck( const FLDMMDL * fmmdl );
extern void FLDMMDL_AcmdSet( FLDMMDL * fmmdl, int code );
extern void FLDMMDL_CmdSet( FLDMMDL * fmmdl, int code );
extern int FLDMMDL_AcmdEndCheck( const FLDMMDL * fmmdl );
extern int FLDMMDL_AcmdEnd( FLDMMDL * fmmdl );
extern void FLDMMDL_AcmdFree( FLDMMDL * fmmdl );

extern GFL_TCB * FLDMMDL_AcmdListSet( FLDMMDL * fmmdl, const FLDMMDL_ACMD_LIST *list );
extern int FLDMMDL_AcmdListEndCheck( GFL_TCB * tcb );
extern void FLDMMDL_AcmdListEnd( GFL_TCB * tcb );

extern int FLDMMDL_AcmdCodeDirChange( int dir, int code );
extern int FLDMMDL_AcmdCodeDirGet( int code );

extern void FLDMMDL_AcmdAction( FLDMMDL * fmmdl );
extern int FLDMMDL_CmdAction( FLDMMDL * fmmdl );

//--------------------------------------------------------------
//	fieldobj_seed.c
//--------------------------------------------------------------
#if 0
extern int FLDMMDL_OBJCodeSeedCheck( int code );
extern int FLDMMDL_OBJCodeSeedGet( const FLDMMDL * fmmdl );
extern void FLDMMDL_SeedHarvestFlagSet( FLDMMDL * fmmdl );

extern void FLDMMDL_MoveSeed_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveSeed_Move( FLDMMDL * fmmdl );
extern void FLDMMDL_MoveSeed_Delete( FLDMMDL * fmmdl );

extern void FLDMMDL_DrawSeed_Init( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawSeed_Draw( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawSeed_Delete( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawSeed_Push( FLDMMDL * fmmdl );
extern void FLDMMDL_DrawSeed_Pop( FLDMMDL * fmmdl );

extern BLACT_WORK_PTR FLDMMDL_SeedBlActPtrGet( FLDMMDL * fmmdl );
#endif

//--------------------------------------------------------------
///	fieldobj_render.c
//--------------------------------------------------------------
#if 0
extern FLDMMDL_RENDER_CONT_PTR FLDMMDL_RenderCont_Init(
		const FLDMMDLSYS *fos, u32 heap_id );
extern void FLDMMDL_RenderCont_Delete( FLDMMDL_RENDER_CONT_PTR rdcont );

extern void FLDMMDL_RenderCont_RegistInit( FLDMMDL_RENDER_CONT_PTR rdcont, int max );

extern int FLDMMDL_RenderCont_OBJRegSimple( FLDMMDL_RENDER_CONT_PTR rdcont, 
		int code, ARCHANDLE *handle, u32 data_id, u32 heap_id );
extern void FLDMMDL_RenderCont_OBJRegFree( FLDMMDL_RENDER_CONT_PTR rdcont, int code );
extern NNSG3dRenderObj * FLDMMDL_RenderCont_OBJRenderGet(
		FLDMMDL_RENDER_CONT_PTR rdcont, int code );
extern NNSG3dResMdl * fmmdlrdcont_OBJModelGet( FLDMMDL_RENDER_CONT_PTR rdcont, int code );

extern BLACT_WORK_PTR FLDMMDL_DrawLegend_GetBlActWorkPtr( FLDMMDL * fmmdl );
#endif

//--------------------------------------------------------------
///	fieldobj_debug.c
//--------------------------------------------------------------
#if 0
#ifdef DEBUG_ALIES
extern FLDMMDL_H * DEBUG_FLDMMDL_ZoneUpdateDeleteAliesHeader(
		int new_zone_id, int *head_max, const FLDMMDL_H *head );
extern void DEBUG_FLDMMDL_ZoneUpdateDeleteAliesHeaderDelete( FLDMMDL_H *head );
extern FLDMMDL_H * DEBUG_FLDMMDL_AddBinHeaderAlies(
		int zone_id, int *head_max, const FLDMMDL_H *head );
extern void DEBUG_FLDMMDL_AddBinHeaderAliesDelete( FLDMMDL_H *head );
#endif
#endif

#endif	//FLDMMDL_H_FILE
