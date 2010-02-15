//======================================================================
/**
 * @file	fldmmdl.h
 * @brief	���샂�f��
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#ifndef MMDL_H_FILE
#define MMDL_H_FILE

#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "include/field/fieldmap_proc.h"
#include "include/field/field_dir.h"

#include "field_g3dobj.h"

#include "field_g3d_mapper.h"
#include "field_nogrid_mapper.h"
#include "eventwork.h"
#include "map_attr.h"
#include "field_effect.h"

#include "arc/fieldmap/fldmmdl_objcode.h"
#include "fldmmdl_code.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#define MMDL_PL_NULL	//��`�Ńv���`�i�ōs���Ă�������������
//#define MMDL_BLACT_HEAD3_TEST //��`�ŎO���g�e�X�g
#define DEBUG_MMDL	//��`�Ńf�o�b�O�@�\�L��
//#define DEBUG_MMDL_FRAME_60 //��`��1/60�t���[���œ�������z��

#ifdef DEBUG_MMDL
#define DEBUG_MMDL_PRINT //��`�Ńf�o�b�O�o�͗L��

#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_MMDL_DEVELOP ///<��Ǝ҂̂ݗL���ȃf�o�b�O����
#endif


#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE // ON�Ń������T�C�Y���v�Z

#endif

#define DEBUG_OBJCODE_STR_LENGTH (16) //���샂�f��OBJ�R�[�h�����񒷂�

//#define MMDL_BBD_DRAW_OFFS_Z_USE //�L���Ńr���{�[�h�\���ʒuZ�␳�L��

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
#define MMDL_VEC_X_GRID_OFFS_FX32 (GRID_HALF_FX32)///<X�����O���b�h�␮
#define MMDL_VEC_Y_GRID_OFFS_FX32 (0)///<Y�����O���b�h�␮
#define MMDL_VEC_Z_GRID_OFFS_FX32 (GRID_HALF_FX32)///<Z�����O���b�h�␮

//--------------------------------------------------------------
//	�\�����W�␮
//--------------------------------------------------------------
#define MMDL_BLACT_X_GROUND_OFFS_FX32 (0) ///<X�����n�ʕ␮
#define MMDL_BLACT_Y_GROUND_OFFS_FX32 (0) ///<Y�����n�ʕ␮
#define MMDL_BLACT_Z_GROUND_OFFS_FX32 (FX32_ONE*6) ///<Z�����n�ʕ␮

//--------------------------------------------------------------
///	MMDLSYS_STABIT MMDLSYS�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
typedef enum
{
	MMDLSYS_STABIT_NON = (0),///<�X�e�[�^�X����
	MMDLSYS_STABIT_DRAW_INIT_COMP = (1<<0),///<�`�揉��������
	MMDLSYS_STABIT_MOVE_PROC_STOP = (1<<1),///<���쏈����~
	MMDLSYS_STABIT_DRAW_PROC_STOP = (1<<2),///<�`�揈����~
	MMDLSYS_STABIT_SHADOW_JOIN_NOT = (1<<3),///<�e��t���Ȃ�
	MMDLSYS_STABIT_MOVE_INIT_COMP = (1<<4),///<���쏈������������
}MMDLSYS_STABIT;

//--------------------------------------------------------------
///	MMDL_STABIT MMDL�X�e�[�^�X�r�b�g �Z�[�u����܂�
// ��ɉB���\��
//--------------------------------------------------------------
typedef enum
{
	MMDL_STABIT_NON=(0),///<�X�e�[�^�X����
	MMDL_STABIT_USE=(1<<0),///<�g�p����\��
	MMDL_STABIT_MOVEPROC_INIT=(1<<1),///<���쏉�������s����
	MMDL_STABIT_VANISH=(1<<2),///<��\��
	MMDL_STABIT_PAUSE_DIR=(1<<3),///<�\�������ꎞ��~
	MMDL_STABIT_PAUSE_ANM=(1<<4),///<�A�j���ꎞ��~
	MMDL_STABIT_ZONE_DEL_NOT=(1<<5),///<�]�[���؂�ւ��ɂ��폜�֎~
	MMDL_STABIT_HEIGHT_VANISH_OFF=(1<<6),///<�����擾�s���̔�\����OFF
	MMDL_STABIT_FELLOW_HIT_NON=(1<<7),///<OBJ���m�̓����蔻�薳��
	MMDL_STABIT_TALK_OFF=(1<<8),///<�b����������
	MMDL_STABIT_HEIGHT_GET_OFF=(1<<9),///<�����擾�����Ȃ�
  MMDL_STABIT_ATTR_GET_OFF=(1<<10),///<�A�g���r���[�g�擾�����Ȃ�
	MMDL_STABIT_ALIES=(1<<11),///<�G�C���A�X�ł���
	MMDL_STABIT_ATTR_OFFS_OFF=(1<<12),///<�A�g���r���[�g�I�t�Z�b�g�ݒ�OFF
	MMDL_STABIT_RAIL_MOVE = (1<<13),///<���[������
}MMDL_STABIT;

//MMDL_STABIT_HEIGHT_EXPAND=(1<<13),///<�g�������ɔ�������
//MMDL_STABIT_BRIDGE=(1<<28),///<���ړ����ł���

//--------------------------------------------------------------
///	MMDL_MOVEBIT MMDL����r�b�g �Z�[�u����܂���
//  ��ɉB���\��
//--------------------------------------------------------------
typedef enum
{
	MMDL_MOVEBIT_NON=(0),///<����
	MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER=(1<<0),///<���앜�A�֐����ĂԕK�v����
	MMDL_MOVEBIT_DRAW_PROC_INIT_COMP=(1<<1),///<�`�揉��������
	MMDL_MOVEBIT_DRAW_PUSH=(1<<2),///<�`�揈����ޔ�
	MMDL_MOVEBIT_PAUSE_MOVE=(1<<3),///<�ړ��ꎞ��~
  MMDL_MOVEBIT_MOVE=(1<<4),///<�ړ�����\��
	MMDL_MOVEBIT_MOVE_START=(1<<5),///<�ړ��J�n��\��
	MMDL_MOVEBIT_MOVE_END=(1<<6),///<�ړ��I����\��
	MMDL_MOVEBIT_JUMP_START=(1<<7),///<�W�����v�J�n
	MMDL_MOVEBIT_JUMP_END=(1<<8),///<�W�����v�I��
	MMDL_MOVEBIT_ACMD=(1<<9),///<�A�j���[�V�����R�}���h��
	MMDL_MOVEBIT_ACMD_END=(1<<10),///<�A�j���[�V�����R�}���h�I��
	MMDL_MOVEBIT_ATTR_GET_ERROR=(1<<11),///<�A�g���r���[�g�擾���s
	MMDL_MOVEBIT_HEIGHT_GET_ERROR=(1<<12),///<�����擾���s
  MMDL_MOVEBIT_BLACT_ADD_PRAC=(1<<13),///<�r���{�[�h�A�N�^�[�ǉ���
	MMDL_MOVEBIT_SHADOW_SET=(1<<14),///<�e���Z�b�g����
	MMDL_MOVEBIT_SHADOW_VANISH=(1<<15),///<�e�\���A��\��
	MMDL_MOVEBIT_GRASS_SET=(1<<16),///<�����Z�b�g����
	MMDL_MOVEBIT_REFLECT_SET=(1<<17),///<�f�荞�݂��Z�b�g����
	MMDL_MOVEBIT_EFFSET_SHOAL=(1<<18),///<�󐣃G�t�F�N�g���Z�b�g
  MMDL_MOVEBIT_REJECT_PAUSE_MOVE=(1<<19),///<����|�[�Y�����ۂ���
}MMDL_MOVEBIT;

//--------------------------------------------------------------
///	MMDL_MOVEHITBIT MMDL�ړ��q�b�g�r�b�g
//--------------------------------------------------------------
typedef enum
{
	MMDL_MOVEHITBIT_NON=(0),///<�q�b�g����
	MMDL_MOVEHITBIT_LIM=(1<<0),///<�ړ������ɂ��q�b�g
	MMDL_MOVEHITBIT_ATTR=(1<<1),///<�A�g���r���[�g�q�b�g
	MMDL_MOVEHITBIT_OBJ=(1<<2),///<OBJ���m�̏Փ�
	MMDL_MOVEHITBIT_HEIGHT=(1<<3),///<���፷�ɂ��q�b�g
	MMDL_MOVEHITBIT_OUTRANGE=(1<<4),///<�͈͊O�q�b�g
  
  MMDL_MOVEHITBIT_BITMAX = (5), ///<�g�p���Ă���r�b�g�ő�
}MMDL_MOVEHITBIT;

//--------------------------------------------------------------
///	�t�B�[���h���샂�f���w�b�_�[��������
//--------------------------------------------------------------
typedef enum
{
	MMDL_PARAM_0 = 0,
	MMDL_PARAM_1,
	MMDL_PARAM_2,
}MMDL_H_PARAM;

//--------------------------------------------------------------
///	�`��^�C�v
//--------------------------------------------------------------
typedef enum
{
	MMDL_DRAWTYPE_NON = 0,		///<�`�斳��
	MMDL_DRAWTYPE_BLACT,			///<�r���{�[�h�`��
	MMDL_DRAWTYPE_MDL,			///<���f�����O�`��
}MMDL_DRAWTYPE;

//--------------------------------------------------------------
///	�e���
//--------------------------------------------------------------
typedef enum
{
	MMDL_SHADOW_NON,			///<�e����
	MMDL_SHADOW_ON,				///<�e�A��
}MMDL_SHADOWTYPE;

//--------------------------------------------------------------
///	���Վ��
//--------------------------------------------------------------
typedef enum
{
	MMDL_FOOTMARK_NON = 0,		///<���Ֆ���
	MMDL_FOOTMARK_NORMAL,		///<�Q�{��
	MMDL_FOOTMARK_CYCLE,			///<���]��
}MMDL_FOOTMARKTYPE;

//--------------------------------------------------------------
///	�f�肱�ݎ��
//--------------------------------------------------------------
typedef enum
{
	MMDL_REFLECT_NON = 0,		///<�f�肱�ݖ���
	MMDL_REFLECT_BLACT,			///<�r���{�[�h�f�肱��
}MMDL_REFLECTTYPE;

//--------------------------------------------------------------
///	�r���{�[�h�A�N�^�[�A�j���[�V�������
//--------------------------------------------------------------
typedef enum
{
	MMDL_BLACT_ANMTBLNO_NON,		///<�A�j������
	MMDL_BLACT_ANMTBLNO_HERO,	///<���@��p
	MMDL_BLACT_ANMTBLNO_BLACT,	///<�ėp�A�j��
  MMDL_BLACT_ANMTBLNO_CYCLEHERO_OLD, ///<���]�Ԏ��@��p ��
  MMDL_BLACT_ANMTBLNO_SWIMHERO, ///<�g��莩�@��p
  MMDL_BLACT_ANMTBLNO_FLIP, ///<�l���ėp �t���b�v�g�p
  MMDL_BLACT_ANMTBLNO_NONFLIP, ///<�l���ėp �t���b�v���g�p
  MMDL_BLACT_ANMTBLNO_CYCLEHERO, ///<���]�Ԏ��@��p
  MMDL_BLACT_ANMTBLNO_ONECELL, ///<��p�^�[���̂�
  MMDL_BLACT_ANMTBLNO_ITEMGET, ///<���@�A�C�e���Q�b�g
  MMDL_BLACT_ANMTBLNO_PCAZUKE, ///<���@�|�P�Z���a��
  MMDL_BLACT_ANMTBLNO_SAVE, ///<���@���|�[�g
  MMDL_BLACT_ANMTBLNO_PCWOMAN, ///<PC�o
  MMDL_BLACT_ANMTBLNO_POKE_FLIP, ///<�|�P�����@���]�g�p
  MMDL_BLACT_ANMTBLNO_POKE_NONFLIP, ///<�|�P�������]���g�p
  MMDL_BLACT_ANMTBLNO_FISHINGHERO, ///<�ނ莩�@ new
  MMDL_BLACT_ANMTBLNO_CUTINHERO, ///<�J�b�g�C�����@ new
  MMDL_BLACT_ANMTBLNO_YUREHERO, ///<�����h�ꎩ�@ new
	MMDL_BLACT_ANMTBLNO_MAX,		///<�ő�
}MMDL_BLACT_ANMTBLNO;

//--------------------------------------------------------------
/// �r���{�[�h�A�N�^�[���f���T�C�Y
//--------------------------------------------------------------
typedef enum
{
  MMDL_BLACT_MDLSIZE_32x32, //32x32
  MMDL_BLACT_MDLSIZE_16x16, //16x16
  MMDL_BLACT_MDLSIZE_64x64, //64x64
  MMDL_BLACT_MDLSIZE_MAX,
}MMDL_BLACT_MDLSIZE;

//--------------------------------------------------------------
///	�`��֐��ԍ�
//--------------------------------------------------------------
typedef enum
{
	MMDL_DRAWPROCNO_NON,		///<�`�斳��
	MMDL_DRAWPROCNO_HERO,	///<���@��p
	MMDL_DRAWPROCNO_BLACT,	///<�r���{�[�h�ėp
  MMDL_DRAWPROCNO_CYCLEHERO, ///<���]�Ԏ��@��p
  MMDL_DRAWPROCNO_SWIMHERO, ///<�g��莩�@��p
  MMDL_DRAWPROCNO_BLACTALWAYSANIME_16, //�r���{�[�h�ėp ���16Frame�A�j��
  MMDL_DRAWPROCNO_ITEMGET, ///<���@�A�C�e���Q�b�g
  MMDL_DRAWPROCNO_PCAZUKE, ///<���@PC�a��
  MMDL_DRAWPROCNO_ONEANMLOOP, ///<�P�p�^�[���A�j�����[�v
//  MMDL_DRAWPROCNO_ONEANM, ///<�P�p�^�[���A�j��
  MMDL_DRAWPROCNO_PCWOMAN, ///<PC�o
  MMDL_DRAWPROCNO_TPOKE, ///<�A������|�P����
  MMDL_DRAWPROCNO_TPOKE_FLY, ///<�A������|�P����(�A�j���Đ��X�s�[�h1/2) 
  MMDL_DRAWPROCNO_MODEL, ///<�|���S�����f��
  MMDL_DRAWPROCNO_FISHINGHERO, ///<�ނ莩�@��p new
  MMDL_DRAWPROCNO_YUREHERO, ///<�����h�ꎩ�@ new
  MMDL_DRAWPROCNO_BLAACTALWAYSANIME_32, //�r���{�[�h�ėp ���32Frame�A�j��
	MMDL_DRAWPROCNO_MAX,		///<�ő�
}MMDL_DRAWPROCNO;

//--------------------------------------------------------------
///	�t�B�[���h���샂�f������v���C�I���e�B�I�t�Z�b�g
//--------------------------------------------------------------
enum
{
	MMDL_TCBPRI_OFFS_ST = 32,		///<��{
	MMDL_TCBPRI_OFFS_PAIR,			///<�y�A����
	MMDL_TCBPRI_OFFS_AFTER,			///<�t�B�[���h���샂�f��������
};

//--------------------------------------------------------------
///	�Z�[�u�p���[�N�v�f��
//--------------------------------------------------------------
#define MMDL_SAVEMMDL_MAX (64)

//--------------------------------------------------------------
//	���f���ő吔
//--------------------------------------------------------------
#define MMDL_MDL_MAX (64)

//--------------------------------------------------------------
/// �r���{�[�h�A�N�^�[ID ������ID
//--------------------------------------------------------------
#define MMDL_BLACTID_NULL (0xffff)

//--------------------------------------------------------------
/// OBJ�R�[�h���� �O���p�����^�ƈ�v
//--------------------------------------------------------------
typedef enum
{
  MMDL_OBJCODESEX_MALE = 0, ///<�j��
  MMDL_OBJCODESEX_FEMALE, ///<����
  MMDL_OBJCODESEX_NON, ///<���ʖ���
  MMDL_OBJCODESEX_MAX, ///<�ő�
}MMDL_OBJCODESEX;

//--------------------------------------------------------------
/// MMDL_HEADER �|�W�V�����^�C�v 
//--------------------------------------------------------------
typedef enum
{
  MMDL_HEADER_POSTYPE_GRID = 0, ///<�O���b�h�|�W�V�����^�C�v
  MMDL_HEADER_POSTYPE_RAIL,     ///<���[���|�W�V�����^�C�v

  MMDL_HEADER_POSTYPE_MAX, ///<�ő�
}MMDL_HEADER_POSTYPE;

//--------------------------------------------------------------
/// MMDL_HEADER �|�W�V�����o�b�t�@�T�C�Y 
//--------------------------------------------------------------
#define MMDL_HEADER_POSBUF_SIZE ( 8 )

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	�^��`
//--------------------------------------------------------------
///MMDLSYS
typedef struct _TAG_MMDLSYS MMDLSYS;
//typedef struct _TAG_MMDLSYS FLDMMDLSYS; ///<����
///MMDL
typedef struct _TAG_MMDL MMDL;
//typedef struct _TAG_MMDL FLDMMDL; ///<����
///MMDL_BLACTCONT
typedef struct _TAG_MMDL_BLACTCONT MMDL_BLACTCONT;
///MMDL_SAVEDATA
typedef struct _TAG_MMDL_SAVEDATA MMDL_SAVEDATA;
///MMDL_ROCKPOS
typedef struct _TAG_MMDL_ROCKPOS MMDL_ROCKPOS;
///MMDL_G3DOBJCONT
typedef struct _TAG_MMDL_G3DOBJCONT MMDL_G3DOBJCONT;

//--------------------------------------------------------------
///	�A�g���r���[�g�^ ��:pl map_attr.h ��ɏ����܂��B
//--------------------------------------------------------------
typedef u32 MATR;

//--------------------------------------------------------------
///	�֐���`
//--------------------------------------------------------------
typedef void (*MMDL_MOVE_PROC_INIT)(MMDL*);///<���쏉�����֐�
typedef void (*MMDL_MOVE_PROC)(MMDL*);///<����֐�
typedef void (*MMDL_MOVE_PROC_DEL)(MMDL*);///<����폜�֐�
typedef void (*MMDL_MOVE_PROC_RECOVER)(MMDL*);///<���앜�A�֐�
typedef void (*MMDL_DRAW_PROC_INIT)(MMDL*);///<�`�揉�����֐�
typedef void (*MMDL_DRAW_PROC)(MMDL*);///<�`��֐�
typedef void (*MMDL_DRAW_PROC_DEL)(MMDL*);///<�`��폜�֐�
typedef void (*MMDL_DRAW_PROC_PUSH)(MMDL*);///<�`��ޔ��֐�
typedef void (*MMDL_DRAW_PROC_POP)(MMDL*);///<�`�敜�A�֐�
typedef u32 (*MMDL_DRAW_PROC_GET)(MMDL*,u32);///<�`��擾�֐�

//--------------------------------------------------------------
/// �r���{�[�h�A�N�^�[�ǉ����̏������֐�
/// idx �ǉ����̃A�N�^�[�C���f�b�N�X
/// init_work �A�N�^�[�ǉ����Ɏw�肵���C�ӂ̃��[�N
//--------------------------------------------------------------
typedef void (*MMDL_BLACTCONT_ADDACT_USERPROC)(u16 idx,void *init_work);


//--------------------------------------------------------------
///	MMDL_HEADER�\����
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

  u32 pos_type; ///<�|�W�V�����^�C�v
  u8  pos_buf[ MMDL_HEADER_POSBUF_SIZE ]; ///<�|�W�V�����o�b�t�@
  
}MMDL_HEADER;

typedef MMDL_HEADER FLDMMDL_HEADER; //����


//--------------------------------------------------------------
///	MMDL_GRIDPOS�\����
//--------------------------------------------------------------
typedef struct {
  s16 gx;			///<�O���b�hX
  s16 gy;			///<�O���b�hY
  s16 gz;			///<�O���b�hZ
  s16 dmy;    ///<4byte�]��
} MMDL_GRIDPOS;

//--------------------------------------------------------------
///	MMDL_HEADER_GRIDPOS�\����
// pos_type��MMDL_HEADER_POSTYPE_GRID�̏ꍇ��
// pos_buf�̓��e
//--------------------------------------------------------------
typedef struct {
  unsigned short gx;			///<�O���b�hX
  unsigned short gz;			///<�O���b�hZ
  int y;						///<Y�l fx32�^
} MMDL_HEADER_GRIDPOS;

//--------------------------------------------------------------
///	MMDL_HEADER_RAILPOS�\����
// pos_type��MMDL_HEADER_POSTYPE_RAIL�̏ꍇ��
// pos_buf�̓��e
//--------------------------------------------------------------
typedef struct {
  u16 rail_index;       ///<���[���C���f�b�N�X
  u16 front_grid;       ///<�O�����[���O���b�h���W
  s16 side_grid;        ///<�T�C�h���[���O���b�h���W
} MMDL_HEADER_RAILPOS;

//--------------------------------------------------------------
///	MMDL_MOVE_PROC_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	u32 move_code;///<����R�[�h
	MMDL_MOVE_PROC_INIT init_proc;///<�������֐�
	MMDL_MOVE_PROC move_proc;///<����֐�
	MMDL_MOVE_PROC_DEL delete_proc;///<�폜�֐�
	MMDL_MOVE_PROC_RECOVER recover_proc;///<���앜���֐�
}MMDL_MOVE_PROC_LIST;

///MMDL_MOVE_PROC_LIST�T�C�Y
#define MMDL_MOVE_PROC_LIST_SIZE (sizeof(MMDL_MOVE_PROC_LIST))

//--------------------------------------------------------------
///	MMDL_DRAW_PROC_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	MMDL_DRAW_PROC_INIT init_proc;///<�������֐�
	MMDL_DRAW_PROC draw_proc;///<�`��֐�
	MMDL_DRAW_PROC_DEL delete_proc;///<�폜�֐�
	MMDL_DRAW_PROC_PUSH push_proc;///<�ޔ��֐�
	MMDL_DRAW_PROC_POP pop_proc;///<���A�֐�
	MMDL_DRAW_PROC_GET get_proc;///<�擾�֐�
}MMDL_DRAW_PROC_LIST;

///MMDL_DRAW_PROC_LIST�T�C�Y
#define MMDL_DRAW_PROC_LIST_SIZE (sizeof(MMDL_DRAW_PROC_LIST))

//--------------------------------------------------------------
///	MMDL_RESMNARC �\����
//--------------------------------------------------------------
typedef struct
{
	int id;
	int narc_id;
}MMDL_RESMNARC;

#define MMDL_RESMNARC_SIZE (sizeof(MMDL_RESMNARC))

//--------------------------------------------------------------
///	MMDL_ACMD_LIST�\����
//--------------------------------------------------------------
typedef struct
{
	u16 code;///<���s����A�j���[�V�����R�}���h�R�[�h
	u16 num;///<code���s��
}MMDL_ACMD_LIST;

#define MMDL_ACMD_LIST_SIZE (sizeof(MMDL_ACMD_LIST))

//--------------------------------------------------------------
///	OBJCODE_STATE�\����
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
typedef struct
{
	int code;				///<OBJ�R�[�h
	u32 type_draw:4;		///<MMDL_DRAWTYPE
	u32 type_shadow:2;		///<MMDL_SHADOWTYPE
	u32 type_footmark:4;	///<MMDL_FOOTMARKTYPE
	u32 type_reflect:2;		///<MMDL_REFLECTTYPE
	u32 dmy:20;				///<bit�]��
}OBJCODE_STATE;
#endif

//--------------------------------------------------------------
///	OBJCODE_PARAM�\���́@�O���f�[�^�ƈ�v 28byte
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
//	12 ����
//--------------------------------------------------------------
typedef struct
{
	u16 code;			///<OBJ�R�[�h
	u8 draw_type;		///<MMDL_DRAWTYPE
	u8 draw_proc_no;	///<MMDL_DRAWPROCNO
  
	u8 shadow_type;		///<MMDL_SHADOWTYPE
	u8 footmark_type;	///<MMDL_FOOTMARKTYPE
	u8 reflect_type;	///<MMDL_REFLECTTYPE
	u8 mdl_size;		///<MMDL_BLACT_MDLSIZE
  
	u8 tex_size;		///<GFL_BBD_TEXSIZDEF... �e�N�X�`���T�C�Y
	u8 anm_id;			///<MMDL_BLACT_ANMTBLNO
  u8 sex;         ///<MMDL_OBJCODESEX
  u8 size_width;  ///<�����T�C�Y
  
  u8 size_depth;  ///<���s�T�C�Y
  s8 offs_x;      ///<X�I�t�Z�b�g
  s8 offs_y;      ///<Y�I�t�Z�b�g
  s8 offs_z;      ///<Z�I�t�Z�b�g
  
  u8 buf[12];     ///<�f�[�^�o�b�t�@
}OBJCODE_PARAM;

typedef struct 
{
	u16 res_idx;		///<���\�[�X�C���f�b�N�X
  u8 padding[10];
}OBJCODE_PARAM_BUF_BBD;

typedef struct
{
  u16 res_idx_mdl;
  u16 res_idx_tex;
  u16 res_idx_anm[3];
  u8 padding[2];
}OBJCODE_PARAM_BUF_MDL;

///�O�����琶�������OBJCODE_PARAM�z��̗v�f���̊i�[�T�C�Y
/// 0-3 OBJCODE_PARAM�v�f��
/// 4... �v�f������OBJCODE_PARAM�z��
#define OBJCODE_PARAM_TOTAL_NUMBER_SIZE (sizeof(u32))

//--------------------------------------------------------------
///	MMDL_BBDACT_ANMTBL�\����
//--------------------------------------------------------------
typedef struct
{
	const GFL_BBDACT_ANM * const *pAnmTbl;
	u32 anm_max;
}MMDL_BBDACT_ANMTBL;

//--------------------------------------------------------------
/// MMDL_CHECKSAME_DATA�\����
//--------------------------------------------------------------
typedef struct
{
  u16 id;
  u16 code;
  u16 zone_id;
  u16 dmy;
}MMDL_CHECKSAME_DATA;

//--------------------------------------------------------------
/// MMDL_BLACTWORK_USER
//--------------------------------------------------------------
typedef struct
{
  u16 flag; //�A�N�^�[�̏󋵂��Ǘ�
  GFL_BBDACT_ACTUNIT_ID actID; //�A�N�^�[ID
}MMDL_BLACTWORK_USER;

//--------------------------------------------------------------
/// MMDL_RECT
//--------------------------------------------------------------
typedef struct
{
  s16 left;
  s16 top;
  s16 right;
  s16 bottom;
}MMDL_RECT;

//======================================================================
//	extern
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ���샂�f���ւ̃|�C���^�擾
 * @param gamedata GAMEDATA�ւ̃|�C���^
 * @retval MMDLSYS�ւ̃|�C���^
 *
 * ���Ԃ�gamesystem/game_data.c�ɂ��邪�A�A�N�Z�X�����̂���
 * ������ɔz�u����
 */
//--------------------------------------------------------------
extern MMDLSYS * GAMEDATA_GetMMdlSys(GAMEDATA *gamedata);

//--------------------------------------------------------------
///	fldmmdl.c
//--------------------------------------------------------------
extern u32 MMDL_BUFFER_GetWorkSize( void );
extern void MMDL_BUFFER_InitBuffer( void *p );

extern MMDLSYS * MMDLSYS_CreateSystem(
    HEAPID heapID, u32 max, MMDL_ROCKPOS *rockpos );
extern void MMDLSYS_FreeSystem( MMDLSYS *fos );

extern void MMDLSYS_SetupProc( MMDLSYS *fos, HEAPID heapID,
    GAMEDATA *gdata, FIELDMAP_WORK *fieldmap,
    const FLDMAPPER *pG3DMapper, FLDNOGRID_MAPPER* pNOGRIDMapper );
extern void MMDLSYS_DeleteProc( MMDLSYS *fos );
extern void MMDLSYS_UpdateProc( MMDLSYS *fos );
extern void MMDLSYS_VBlankProc( MMDLSYS *fos );

extern void MMDLSYS_SetupDrawProc( MMDLSYS *fos, const u16 *angleYaw );

extern MMDL * MMDLSYS_AddMMdl(
	MMDLSYS *fos, const MMDL_HEADER *header, int zone_id );
extern MMDL * MMDLSYS_AddMMdlParam( MMDLSYS *fos,
    s16 gx, s16 gz, u16 dir,
    u16 id, u16 code, u16 move, int zone_id );
extern void MMDLSYS_SetMMdl( MMDLSYS *fos,
	const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork );
extern MMDL * MMDLSYS_AddMMdlHeaderID( MMDLSYS *fos,
	const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork,
  u16 objID );
extern void MMDL_Delete( MMDL * mmdl );
extern void MMDL_DeleteEvent( MMDL * mmdl, EVENTWORK *evwork );
extern void MMDLSYS_DeleteMMdl( const MMDLSYS *fos );


extern void MMDLSYS_Push( MMDLSYS *mmdlsys );
extern void MMDLSYS_Pop( MMDLSYS *mmdlsys );

extern u32 MMDL_SAVEDATA_GetWorkSize( void );
extern void MMDL_SAVEDATA_Init( void *p );
extern void MMDL_SAVEDATA_Save(
	MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata );
extern void MMDL_SAVEDATA_Load(
	MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata );

extern void MMDLSYS_Push( MMDLSYS *mmdlsys );
extern void MMDLSYS_Pop( MMDLSYS *mmdlsys );

extern void MMDL_UpdateMoveProc( MMDL *mmdl );

extern BOOL MMDL_CheckPossibleAcmd( const MMDL * mmdl );
extern void MMDL_SetAcmd( MMDL * mmdl, u16 code );
extern void MMDL_SetLocalAcmd( MMDL * mmdl, u16 code );
extern BOOL MMDL_CheckEndAcmd( const MMDL * mmdl );
extern BOOL MMDL_EndAcmd( MMDL * mmdl );
extern void MMDL_FreeAcmd( MMDL * mmdl );

extern u32 MMDLSYS_CheckStatusBit(
	const MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
extern u16 MMDLSYS_GetMMdlMax( const MMDLSYS *mmdlsys );
extern u16 MMDLSYS_GetMMdlCount( const MMDLSYS *mmdlsys );
extern u16 MMDLSYS_GetTCBPriority( const MMDLSYS *mmdlsys );
extern HEAPID MMDLSYS_GetHeapID( const MMDLSYS *mmdlsys );
extern const MMDL * MMDLSYS_GetMMdlBuffer(
		const MMDLSYS *mmdlsys );
extern void MMDLSYS_SetArcHandle(
		MMDLSYS *mmdlsys, ARCHANDLE *handle );
extern ARCHANDLE * MMDLSYS_GetArcHandle( MMDLSYS *mmdlsys );
extern GFL_TCBSYS * MMDLSYS_GetTCBSYS( MMDLSYS *fos );
extern ARCHANDLE * MMDLSYS_GetResArcHandle( MMDLSYS *fos );
extern void MMDLSYS_SetBlActCont(
		MMDLSYS *mmdlsys, MMDL_BLACTCONT *pBlActCont );
extern MMDL_BLACTCONT * MMDLSYS_GetBlActCont( MMDLSYS *mmdlsys );
extern void MMDLSYS_SetG3dObjCont(
	MMDLSYS *mmdlsys, MMDL_G3DOBJCONT *objcont );
extern MMDL_G3DOBJCONT * MMDLSYS_GetG3dObjCont( MMDLSYS *mmdlsys );

extern const FLDMAPPER * MMDLSYS_GetG3DMapper( const MMDLSYS *fos );
extern FLDNOGRID_MAPPER * MMDLSYS_GetNOGRIDMapper( const MMDLSYS *fos );
extern void * MMDLSYS_GetFieldMapWork( MMDLSYS *fos );
extern GAMEDATA * MMDLSYS_GetGameData( MMDLSYS *fos );
extern u16 MMDLSYS_GetTargetCameraAngleYaw( const MMDLSYS *mmdlsys );

extern void MMDL_OnStatusBit( MMDL *mmdl, MMDL_STABIT bit );
extern void MMDL_OffStatusBit( MMDL *mmdl, MMDL_STABIT bit );
extern MMDL_STABIT MMDL_GetStatusBit( const MMDL * mmdl );
extern u32 MMDL_CheckStatusBit(const MMDL *mmdl, MMDL_STABIT bit);
extern MMDL_MOVEBIT MMDL_GetMoveBit( const MMDL * mmdl );
extern void MMDL_OnMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit );
extern void MMDL_OffMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit );
extern u32 MMDL_CheckMoveBit(const MMDL *mmdl,MMDL_MOVEBIT bit);
extern void MMDL_SetOBJID( MMDL * mmdl, u16 obj_id );
extern u16 MMDL_GetOBJID( const MMDL * mmdl );
extern void MMDL_SetZoneID( MMDL * mmdl, u16 zone_id );
extern u16 MMDL_GetZoneID( const MMDL * mmdl );
extern void MMDL_SetOBJCode( MMDL * mmdl, u16 code );
extern u16 MMDL_GetOBJCode( const MMDL * mmdl );
extern void MMDL_SetMoveCode( MMDL * mmdl, u16 code );
extern u16 MMDL_GetMoveCode( const MMDL * mmdl );
extern void MMDL_SetEventType( MMDL * mmdl, u16 type );
extern u16 MMDL_GetEventType( const MMDL * mmdl );
extern void MMDL_SetEventFlag( MMDL * mmdl, u16 flag );
extern u16 MMDL_GetEventFlag( const MMDL * mmdl );
extern void MMDL_SetEventID( MMDL * mmdl, u16 id );
extern u16 MMDL_GetEventID( const MMDL * mmdl );
extern BOOL MMDL_CheckAliesEventID( const MMDL * mmdl );
extern u32 MMDL_GetDirHeader( const MMDL * mmdl );
extern void MMDL_SetForceDirDisp( MMDL * mmdl, u16 dir );
extern void MMDL_SetDirDisp( MMDL * mmdl, u16 dir );
extern u16 MMDL_GetDirDisp( const MMDL * mmdl );
extern u16 MMDL_GetDirDispOld( const MMDL * mmdl );
extern void MMDL_SetDirMove( MMDL * mmdl, u16 dir );
extern u16 MMDL_GetDirMove( const MMDL * mmdl );
extern u16 MMDL_GetDirMoveOld( const MMDL * mmdl );
extern void MMDL_SetDirAll( MMDL * mmdl, u16 dir );
extern void MMDL_SetParam(MMDL *mmdl, u16 param, MMDL_H_PARAM no);
extern u16 MMDL_GetParam(const MMDL * mmdl, MMDL_H_PARAM param);
extern void MMDL_SetMoveLimitX( MMDL * mmdl, s16 x );
extern s16 MMDL_GetMoveLimitX( const MMDL * mmdl );
extern void MMDL_SetMoveLimitZ( MMDL * mmdl, s16 z );
extern s16 MMDL_GetMoveLimitZ( const MMDL * mmdl );
extern void MMDL_SetDrawStatus( MMDL * mmdl, u16 st );
extern u16 MMDL_GetDrawStatus( const MMDL * mmdl );
extern MMDLSYS * MMDL_GetMMdlSys( const MMDL * mmdl );
extern void * MMDL_InitMoveProcWork( MMDL * mmdl, int size );
extern void * MMDL_GetMoveProcWork( MMDL * mmdl );
extern void * MMDL_InitMoveSubProcWork( MMDL * mmdl, int size );
extern void * MMDL_GetMoveSubProcWork( MMDL * mmdl );
extern void * MMDL_InitMoveCmdWork( MMDL * mmdl, int size );
extern void * MMDL_GetMoveCmdWork( MMDL * mmdl );
extern void * MMDL_InitDrawProcWork( MMDL * mmdl, int size );
extern void * MMDL_GetDrawProcWork( MMDL * mmdl );
extern void MMDL_CallMoveInitProc( MMDL * mmdl );
extern void MMDL_CallMoveProc( MMDL * mmdl );
extern void MMDL_CallMoveDeleteProc( MMDL * mmdl );
extern void MMDL_CallMovePopProc( MMDL * mmdl );
extern void MMDL_CallDrawInitProc( MMDL * mmdl );
extern void MMDL_CallDrawProc( MMDL * mmdl );
extern void MMDL_CallDrawDeleteProc( MMDL * mmdl );
extern void MMDL_CallDrawPushProc( MMDL * mmdl );
extern void MMDL_CallDrawPopProc( MMDL * mmdl );
extern u32 MMDL_CallDrawGetProc( MMDL *mmdl, u32 state );
extern void MMDL_SetAcmdCode( MMDL * mmdl, u16 code );
extern u16 MMDL_GetAcmdCode( const MMDL * mmdl );
extern void MMDL_SetAcmdSeq( MMDL * mmdl, u16 no );
extern void MMDL_IncAcmdSeq( MMDL * mmdl );
extern u16 MMDL_GetAcmdSeq( const MMDL * mmdl );
extern void MMDL_SetMapAttr( MMDL * mmdl, u32 attr );
extern u32 MMDL_GetMapAttr( const MMDL * mmdl );
extern void MMDL_SetMapAttrOld( MMDL * mmdl, u32 attr );
extern u32 MMDL_GetMapAttrOld( const MMDL * mmdl );
extern u16 MMDL_GetZoneIDAlies( const MMDL * mmdl );
extern s16 MMDL_GetInitGridPosX( const MMDL * mmdl );
extern void MMDL_SetInitGridPosX( MMDL * mmdl, s16 x );
extern s16 MMDL_GetInitGridPosY( const MMDL * mmdl );
extern void MMDL_SetInitGridPosY( MMDL * mmdl, s16 y );
extern s16 MMDL_GetInitGridPosZ( const MMDL * mmdl );
extern void MMDL_SetInitGridPosZ( MMDL * mmdl, s16 z );
extern s16 MMDL_GetOldGridPosX( const MMDL * mmdl );
extern void MMDL_SetOldGridPosX( MMDL * mmdl, s16 x );
extern s16 MMDL_GetOldGridPosY( const MMDL * mmdl );
extern void MMDL_SetOldGridPosY( MMDL * mmdl, s16 y );
extern s16 MMDL_GetOldGridPosZ( const MMDL * mmdl );
extern void MMDL_SetOldGridPosZ( MMDL * mmdl, s16 z );
extern void MMDL_GetGridPos( const MMDL * mmdl, MMDL_GRIDPOS* pos );
extern s16 MMDL_GetGridPosX( const MMDL * mmdl );
extern void MMDL_SetGridPosX( MMDL * mmdl, s16 x );
extern void MMDL_AddGridPosX( MMDL * mmdl, s16 x );
extern s16 MMDL_GetGridPosY( const MMDL * mmdl );
extern void MMDL_SetGridPosY( MMDL * mmdl, s16 y );
extern void MMDL_AddGridPosY( MMDL * mmdl, s16 y );
extern s16 MMDL_GetGridPosZ( const MMDL * mmdl );
extern void MMDL_SetGridPosZ( MMDL * mmdl, s16 z );
extern void MMDL_AddGridPosZ( MMDL * mmdl, s16 z );
extern const VecFx32 * MMDL_GetVectorPosAddress( const MMDL * mmdl );
extern void MMDL_GetVectorPos( const MMDL * mmdl, VecFx32 *vec );
extern void MMDL_SetVectorPos( MMDL * mmdl, const VecFx32 *vec );
extern fx32 MMDL_GetVectorPosY( const MMDL * mmdl );
extern void MMDL_GetVectorDrawOffsetPos(
		const MMDL * mmdl, VecFx32 *vec );
extern void MMDL_SetVectorDrawOffsetPos(
		MMDL * mmdl, const VecFx32 *vec );
extern void MMDL_GetVectorOuterDrawOffsetPos(
		const MMDL * mmdl, VecFx32 *vec );
extern void MMDL_SetVectorOuterDrawOffsetPos(
		MMDL * mmdl, const VecFx32 *vec );
extern void MMDL_GetVectorAttrDrawOffsetPos(
		const MMDL * mmdl, VecFx32 *vec );
extern void MMDL_SetVectorAttrDrawOffsetPos(
		MMDL * mmdl, const VecFx32 *vec );
extern s16 MMDL_GetHeightGrid( const MMDL * mmdl );
extern MMDL_BLACTCONT * MMDL_GetBlActCont( MMDL *mmdl );
extern u8 MMDL_GetGridSizeX( const MMDL *mmdl );
extern u8 MMDL_GetGridSizeZ( const MMDL *mmdl );
extern void MMDL_GetControlOffsetPos( const MMDL *mmdl, VecFx32 *pos );

extern BOOL MMDLSYS_CheckCompleteDrawInit( const MMDLSYS *mmdlsys );
extern void MMDLSYS_SetCompleteDrawInit( MMDLSYS *mmdlsys, BOOL flag );
extern void MMDLSYS_SetJoinShadow( MMDLSYS *fos, BOOL flag );
extern BOOL MMDLSYS_CheckJoinShadow( const MMDLSYS *fos );

extern void MMDLSYS_StopProc( MMDLSYS *mmdlsys );
extern void MMDLSYS_PlayProc( MMDLSYS *mmdlsys );
extern void MMDLSYS_PauseMoveProc( MMDLSYS *mmdlsys );
extern void MMDLSYS_ClearPauseMoveProc( MMDLSYS *mmdlsys );
extern BOOL MMDLSYS_GetPauseMoveFlag( MMDLSYS* mmdlsys );

extern u32 MMDL_CheckMMdlSysStatusBit(
	const MMDL *mmdl, MMDLSYS_STABIT bit );
extern BOOL MMDL_CheckStatusBitUse( const MMDL *mmdl );
extern void MMDL_OnMoveBitMove( MMDL *mmdl );
extern void MMDL_OffMoveBitMove( MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitMove( const MMDL *mmdl );
extern void MMDL_OnMoveBitMoveStart( MMDL * mmdl );
extern void MMDL_OffMoveBitMoveStart( MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitMoveStart( const MMDL * mmdl );
extern void MMDL_OnMoveBitMoveEnd( MMDL * mmdl );
extern void MMDL_OffMoveBitMoveEnd( MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitMoveEnd( const MMDL * mmdl );
extern void MMDL_OnMoveBitCompletedDrawInit( MMDL * mmdl );
extern void MMDL_OffMoveBitCompletedDrawInit( MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitCompletedDrawInit(const MMDL * mmdl);
extern BOOL MMDL_CheckStatusBitVanish( const MMDL * mmdl );
extern void MMDL_SetStatusBitVanish( MMDL * mmdl, BOOL flag );
extern void MMDL_SetStatusBitFellowHit( MMDL * mmdl, BOOL flag );
extern void MMDL_SetMoveBitMove( MMDL * mmdl, int flag );
extern BOOL MMDL_CheckStatusBitTalk( MMDL * mmdl );
extern void MMDL_SetStatusBitTalkOFF( MMDL * mmdl, BOOL flag );
extern void MMDL_OnMoveBitMoveProcPause( MMDL * mmdl );
extern void MMDL_OffMoveBitMoveProcPause( MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitMoveProcPause( const MMDL * mmdl );
extern BOOL MMDL_CheckCompletedDrawInit( const MMDL * mmdl );
extern void MMDL_SetStatusBitHeightGetOFF( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckStatusBitHeightGetOFF( const MMDL * mmdl );
extern void MMDL_SetStatusBitNotZoneDelete( MMDL * mmdl, BOOL flag );
extern void MMDL_SetStatusBitAlies( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckStatusBitAlies( const MMDL * mmdl );
extern void MMDL_SetMoveBitShoalEffect( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckMoveBitShoalEffect( const MMDL * mmdl );
extern void MMDL_SetStatusBitAttrOffsetOFF( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckStatusBitAttrOffsetOFF( const MMDL * mmdl );
#ifndef MMDL_PL_NULL
extern void MMDL_SetStatusBitBridge( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckStatusBitBridge( const MMDL * mmdl );
#endif
extern void MMDL_SetMoveBitReflect( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckMoveBitReflect( const MMDL * mmdl );
extern BOOL MMDL_CheckMoveBitAcmd( const MMDL * mmdl );
extern void MMDL_SetMoveBitRejectPauseMove( MMDL *mmdl, BOOL flag );
extern BOOL MMDL_CheckMoveBitRejectPauseMove( const MMDL *mmdl );
#if 0 //wb null
extern void MMDL_SetStatusBitHeightExpand( MMDL * mmdl, BOOL flag );
extern BOOL MMDL_CheckStatusBitHeightExpand( const MMDL * mmdl );
#endif

extern void MMDL_SetStatusBitAttrGetOFF( MMDL * mmdl, BOOL flag );
extern int MMDL_CheckStatusBitAttrGetOFF( const MMDL * mmdl );

extern BOOL MMDLSYS_SearchUseMMdl(
	const MMDLSYS *fos, MMDL **mmdl, u32 *no );
extern MMDL * MMDLSYS_SearchGridPos(
	const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit );
extern MMDL * MMDLSYS_SearchGridPosEx(
	const MMDLSYS *sys, s16 x, s16 z, fx32 height, fx32 y_diff, BOOL old_hit );
extern MMDL * MMDLSYS_SearchMoveCode(
	const MMDLSYS *fos, u16 mv_code );
extern MMDL * MMDLSYS_SearchOBJID( const MMDLSYS *fos, u16 id );
extern MMDL * MMDLSYS_SearchMMdlPlayer( MMDLSYS *mmdlsys );
extern void MMDLSYS_DeleteZoneUpdateMMdl( MMDLSYS *fos );

extern BOOL MMDL_SearchUseOBJCode( const MMDL *mmdl, u16 code );
extern void MMDL_InitPosition(
	MMDL * mmdl, const VecFx32 *vec, u16 dir );
extern void MMDL_InitGridPosition(
    MMDL * mmdl, s16 gx, s16 gy, s16 gz, u16 dir );
extern void MMDL_ChangeMoveParam( MMDL *mmdl, const MMDL_HEADER *head );
extern void MMDL_ChangeMoveCode( MMDL *mmdl, u16 code );
extern void MMDL_ChangeOBJID( MMDL * mmdl, u16 id );
extern void MMDL_InitCheckSameData(
    const MMDL *mmdl, MMDL_CHECKSAME_DATA *outData );
extern BOOL MMDL_CheckSameData(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data );
extern BOOL MMDL_CheckSameDataIDOnly(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data );

extern const OBJCODE_PARAM * MMDLSYS_GetOBJCodeParam(
		const MMDLSYS *mmdlsys, u16 code );
extern const OBJCODE_PARAM * MMDL_GetOBJCodeParam(
		const MMDL *mmdl, u16 code );
extern const OBJCODE_PARAM_BUF_BBD * MMDL_GetOBJCodeParamBufBBD(
    const OBJCODE_PARAM *param );
extern const OBJCODE_PARAM_BUF_MDL * MMDL_GetOBJCodeParamBufMDL(
    const OBJCODE_PARAM *param );

extern u16 MMDL_TOOL_GetWorkOBJCode( const EVENTWORK *ev, u16 code );

extern void MMDL_MoveInitProcDummy( MMDL * mmdl );
extern void MMDL_MoveProcDummy( MMDL * mmdl );
extern void MMDL_MoveDeleteProcDummy( MMDL * mmdl );
extern void MMDL_MoveReturnProcDummy( MMDL * mmdl );

extern void MMDL_DrawInitProcDummy( MMDL * mmdl );
extern void MMDL_DrawProcDummy( MMDL * mmdl );
extern void MMDL_DrawDeleteProcDummy( MMDL * mmdl );
extern void MMDL_DrawPushProcDummy( MMDL * mmdl );
extern void MMDL_DrawPopProcDummy( MMDL * mmdl );

extern void MMDL_ChangeOBJCode( MMDL *mmdl, u16 code );

extern u32 MMDL_ROCKPOS_GetWorkSize( void );
extern void MMDL_ROCKPOS_Init( void *p );
extern void MMDL_ROCKPOS_SavePos( const MMDL *mmdl );
extern BOOL MMDLSYS_ROCKPOS_CheckRockFalled(
    const MMDLSYS *mmdlsys, const VecFx32 *pos );
extern BOOL MMDL_CheckOBJCodeKairiki( const u16 code );


extern void MMDLHEADER_SetGridPos( MMDL_HEADER* head, u16 gx, u16 gz, int y );
extern void MMDLHEADER_SetRailPos( MMDL_HEADER* head, u16 index, u16 front, u16 side );

#ifdef DEBUG_MMDL
extern u8 * DEBUG_MMDL_GetOBJCodeString( u16 code, HEAPID heapID );
#endif


//--------------------------------------------------------------
//	fldmmdl_movedata.c
//--------------------------------------------------------------
extern const MMDL_MOVE_PROC_LIST * const DATA_FieldOBJMoveProcListTbl[MV_CODE_MAX];
extern int (* const * const DATA_AcmdActionTbl[ACMD_MAX])( MMDL * );
extern int (* const * const DATA_RailAcmdActionTbl[ACMD_MAX])( MMDL * );
extern const int * const DATA_AcmdCodeDirChangeTbl[];

//--------------------------------------------------------------
//	fldmmdl_drawdata.c
//--------------------------------------------------------------
extern const MMDL_DRAW_PROC_LIST * const
	DATA_MMDL_DRAW_PROC_LIST_Tbl[MMDL_DRAWPROCNO_MAX];

extern const MMDL_BBDACT_ANMTBL
	DATA_MMDL_BBDACT_ANM_ListTable[MMDL_BLACT_ANMTBLNO_MAX];

extern const u16 DATA_MMDL_BLACT_MdlSize[MMDL_BLACT_MDLSIZE_MAX][2];

#ifndef MMDL_PL_NULL
extern const OBJCODE_STATE DATA_FieldOBJCodeDrawStateTbl[];
#endif

#ifndef MMDL_PL_NULL
extern const MMDL_DRAW_PROC_LIST DATA_FieldOBJDraw_Non;

extern const MMDL_RESMNARC DATA_MMDL_ResmNArcTbl_RenderOBJ[];
extern const int DATA_FIELDOBJ_RenderOBJMax;

extern const MMDL_RESMNARC DATA_MMDL_ResmNArcTbl_Tex[];
extern const MMDL_RESMNARC DATA_MMDL_ResmNArcTbl_Mdl[];
extern const MMDL_RESMNARC DATA_MMDL_ResmNArcTbl_Anm[];
extern const int DATA_MMDL_Regular_Tex[];
extern const int DATA_MMDL_Regular_Mdl[];
extern const int DATA_MMDL_Regular_Anm[];
extern const int DATA_MMDL_BlActFogEnableOFFTbl[];
#endif

//--------------------------------------------------------------
//	fldmmdl_move.c
//--------------------------------------------------------------
extern void MMDL_InitMoveProc( MMDL * mmdl );
extern void MMDL_UpdateMove( MMDL * mmdl );

extern u32 MMDL_HitCheckMove( const MMDL *mmdl,
	const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir );
extern u32 MMDL_HitCheckMoveCurrent(
	const MMDL * fmmdl, s16 x, s16 y, s16 z, u16 dir );
extern u32 MMDL_HitCheckMoveDir( const MMDL * mmdl, u16 dir );
extern void MMDL_CreateHitCheckRect(
    const MMDL *mmdl, s16 x, s16 z, MMDL_RECT *rect );
extern BOOL MMDL_HitCheckRect(
    const MMDL_RECT *rect0, const MMDL_RECT *rect1 );
extern BOOL MMDL_HitCheckFellow( const MMDL *mmdl0, const MMDL *mmdl1,
    s16 x0, s16 y0, s16 z0, BOOL old_hit );
extern int MMDL_HitCheckMoveFellow(
	const MMDL * mmdl, s16 x, s16 y, s16 z );
extern BOOL MMDL_HitCheckXZ( const MMDL *mmdl, s16 x0, s16 z0, BOOL old_hit );
extern int MMDL_HitCheckMoveLimit(
	const MMDL * mmdl, s16 x, s16 z );
extern BOOL MMDL_GetMapPosAttr(
	const MMDL *mmdl, const VecFx32 *pos, u32 *attr );
extern BOOL MMDL_GetMapPosHeight(
	const MMDL *mmdl, const VecFx32 *pos, fx32 *height );

extern void MMDL_UpdateGridPosDir( MMDL * mmdl, u16 dir );
extern void MMDL_UpdateGridPosCurrent( MMDL * mmdl );
extern MAPATTR MMDL_GetMapDirAttr( MMDL * mmdl, u16 dir );

extern void MMDL_AddVectorPos( MMDL * mmdl, const VecFx32 *val );
extern void MMDL_AddVectorPosDir( MMDL * mmdl, u16 dir, fx32 val );
extern BOOL MMDL_UpdateCurrentHeight( MMDL * mmdl );
extern BOOL MMDL_UpdateCurrentMapAttr( MMDL * mmdl );
extern BOOL MMDL_CheckPlayerDispSizeRect( const MMDL *mmdl );

extern void MMDL_CallMoveProcAfterDrawInit( MMDL * mmdl );

extern s16 MMDL_TOOL_GetDirAddValueGridX( u16 dir );
extern s16 MMDL_TOOL_GetDirAddValueGridZ( u16 dir );
extern void MMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val );
extern void MMDL_TOOL_AddDirGrid( u16 dir, s16 *gx, s16 *gz, s16 val );
extern void MMDL_TOOL_GetCenterGridPos( s16 gx, s16 gz, VecFx32 *vec );
extern void MMDL_TOOL_VectorPosToGridPos(
	s16 *gx, s16 *gy, s16 *gz, const VecFx32 *vec );
extern void MMDL_TOOL_GridPosToVectorPos(
    const s16 gx, const s16 gy, const s16 gz, VecFx32 *pos );
extern u16 MMDL_TOOL_FlipDir( u16 dir );
extern u16 MMDL_TOOL_GetRangeDir( int ax, int az, int bx, int bz );
extern u16 MMDL_TOOL_GetAngleYawToDirFour( u16 dir, u16 angleYaw );

//--------------------------------------------------------------
//	fldmmdl_move_1.c
//--------------------------------------------------------------
extern void MMDL_MoveSubProcInit( MMDL * mmdl );
extern int MMDL_MoveSub( MMDL * mmdl );

//--------------------------------------------------------------
//	fldmmdl_move_2.c
//--------------------------------------------------------------
extern void MMDL_SetMoveHideEffectTask( MMDL * mmdl, FLDEFF_TASK *task );
extern FLDEFF_TASK * MMDL_GetMoveHideEffectTask( MMDL * mmdl );
extern void MMDL_MoveHidePullOffFlagSet( MMDL * mmdl );

//--------------------------------------------------------------
//	fldmmdl_draw.c
//--------------------------------------------------------------
extern void MMDLSYS_InitDraw( MMDLSYS *fos );
extern void MMDLSYS_DeleteDraw( MMDLSYS *fos );

extern void MMDL_UpdateDraw( MMDL * mmdl );


#ifndef MMDL_PL_NULL
extern const OBJCODE_STATE * MMDL_TOOL_GetOBJCodeState( u16 code );
extern const OBJCODE_STATE * MMDL_GetOBJCodeState( const MMDL *mmdl );
#endif

extern BOOL MMDL_CheckDrawPause( const MMDL * mmdl );
extern void * MMDL_DrawArcDataAlloc(
		const MMDLSYS *fos, u32 datID, int fb );
extern void MMDL_GetDrawVectorPos(
		const MMDL * mmdl, VecFx32 *vec );

//--------------------------------------------------------------
//	fldmmdl_draw_0.c
//--------------------------------------------------------------
extern BOOL MMDL_DrawYureHero_SetAnimeFrame( MMDL *mmdl, u32 frame );

//--------------------------------------------------------------
//	fldmmdl_acmd.c
//--------------------------------------------------------------
extern GFL_TCB * MMDL_SetAcmdList(
	MMDL * mmdl, const MMDL_ACMD_LIST *list );
extern BOOL MMDL_CheckEndAcmdList( GFL_TCB * tcb );
extern void MMDL_EndAcmdList( GFL_TCB * tcb );

extern u16 MMDL_ChangeDirAcmdCode( u16 dir, u16 code );
extern u16 MMDL_GetAcmdDir( u16 code );

extern void MMDL_ActionAcmd( MMDL * mmdl );
extern BOOL MMDL_ActionLocalAcmd( MMDL * mmdl );

//--------------------------------------------------------------
///	fldmmdl_blact.c
//--------------------------------------------------------------
extern void MMDL_BLACTCONT_Setup( MMDLSYS *mmdlsys,
	GFL_BBDACT_SYS *pBbdActSys, int res_max );
extern void MMDL_BLACTCONT_Release( MMDLSYS *mmdlsys );
extern void MMDL_BLACTCONT_Update( MMDLSYS *mmdlsys );
extern void MMDL_BLACTCONT_ProcVBlank( MMDLSYS *mmdlsys );
extern void MMDL_BLACTCONT_SetGlobalScaleOne( MMDLSYS *mmdlsys );

extern void MMDL_BLACTCONT_AddResourceTex(
	MMDLSYS *mmdlsys, const u16 *code, int max );

extern BOOL MMDL_BLACTCONT_AddActor(
    MMDL *mmdl, u16 code, GFL_BBDACT_ACTUNIT_ID *outID );
extern void MMDL_BLACTCONT_DeleteActor( MMDL *mmdl, u32 actID );

extern BOOL MMDL_BLACTCONT_USER_AddActor( MMDLSYS *mmdlsys,
    u16 code, MMDL_BLACTWORK_USER *userAct, const VecFx32 *pos,
    MMDL_BLACTCONT_ADDACT_USERPROC init_proc, void *init_work );
extern void MMDL_BLACTCONT_USER_DeleteActor(
    MMDLSYS *mmdlsys, MMDL_BLACTWORK_USER *userAct );

extern GFL_BBDACT_SYS * MMDL_BLACTCONT_GetBbdActSys(
		MMDL_BLACTCONT *pBlActCont );
extern GFL_BBDACT_RESUNIT_ID MMDL_BLACTCONT_GetResUnitID(
		MMDL_BLACTCONT *pBlActCont );

extern BOOL MMDL_BLACTCONT_CheckOBJCodeRes(
		MMDLSYS *mmdlsys, u16 code );
extern BOOL MMDL_BLACTCONT_AddOBJCodeRes(
    MMDLSYS *mmdlsys, u16 code, BOOL trans, BOOL guest );
extern void MMDL_BLACTCONT_DeleteOBJCodeRes( MMDLSYS *mmdlsys, u16 code );

extern const MMDL_BBDACT_ANMTBL * MMDL_BLACTCONT_GetObjAnimeTable(
  const MMDLSYS *mmdlsys, u16 code );

#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE
extern u32 DEBUG_MMDL_GetUseResourceMemorySize( void );
#endif

//--------------------------------------------------------------
//	fldmmdl_railmove.c
//--------------------------------------------------------------
extern void MMDL_InitRailMoveProc( MMDL * mmdl );
extern void MMDL_UpdateRailMove( MMDL * mmdl );
extern MAPATTR MMDL_GetRailLocationAttr( const MMDL * mmdl, const RAIL_LOCATION* location );

extern u16 MMDL_RAIL_GetAngleYawToDirFour( MMDL * mmdl, u16 angleYaw );


//--------------------------------------------------------------
//	fldmmdl_railmove_0.c
//--------------------------------------------------------------
extern void MMDL_SetRailHeaderBefore( MMDL* mmdl, const MMDL_HEADER* head );
extern void MMDL_SetRailHeaderAfter( MMDL* mmdl, const MMDL_HEADER* head );

// ���P�[�V�����ۑ��̈揈��
// ���̏����́AMOVE_PROC������������Ă��Ȃ��^�C�~���O�ŁASetRailHeaderAfter�����삵���Ƃ��ɁARailWork�̏����ݒ��n�����߂̏����ł��B
//����R�[�h�p���[�N���g�p���Ă��܂��B
extern BOOL MMdl_CheckSetUpLocation( const MMDL* mmdl );
extern BOOL MMdl_GetSetUpLocation( const MMDL* mmdl, RAIL_LOCATION* location );
//-----------------------------------------------

// ���[�����P�[�V�����̎擾
extern void MMDL_SetRailLocation( MMDL * fmmdl, const RAIL_LOCATION* location );
extern void MMDL_GetRailLocation( const MMDL * fmmdl, RAIL_LOCATION* location );
extern void MMDL_GetOldRailLocation( const MMDL * fmmdl, RAIL_LOCATION* location );
// DIR�����̃��P�[�V�����̎擾�@BOOL�ňړ��ł��邩�`�F�b�N���s���B
// FALSE���A���Ă��Ă��Alocation�ɂ́A�ړ���̃��P�[�V�����������Ă���B
// ���̒l�́A�C�x���g�`�F�b�N�ȂǂŎg�p���Ă���B
extern BOOL MMDL_GetRailFrontLocation( const MMDL *mmdl, RAIL_LOCATION* location );
extern BOOL MMDL_GetRailDirLocation( const MMDL *mmdl, u16 dir, RAIL_LOCATION* location );
extern void MMDL_Rail_UpdateGridPosDir( MMDL *mmdl, u16 dir );

extern FIELD_RAIL_WORK* MMDL_GetRailWork( const MMDL * fmmdl );
extern void MMDL_UpdateRail( MMDL* fmmdl );
extern BOOL MMDL_ReqRailMove( MMDL * fmmdl, u16 dir, s16 wait );
extern u32 MMDL_HitCheckRailMove( const MMDL *mmdl,
	const RAIL_LOCATION* now_location, const RAIL_LOCATION* next_location );
extern u32 MMDL_HitCheckRailMoveCurrent( const MMDL *mmdl, const RAIL_LOCATION* next_location );
extern u32 MMDL_HitCheckRailMoveDir( const MMDL *mmdl, u16 dir );
extern BOOL MMDL_HitCheckRailMoveFellow(
	const MMDL * mmdl, const RAIL_LOCATION* location );
extern MMDL * MMDLSYS_SearchRailLocation( const MMDLSYS *sys, const RAIL_LOCATION* location, BOOL old_hit );

// ���[���ړ��@���̑O������
extern void MMDL_Rail_GetFrontWay( const MMDL *mmdl, VecFx16* way );
extern void MMDL_Rail_GetDirLineWay( const MMDL *mmdl, u16 dir, VecFx16* way );

//--------------------------------------------------------------
//	fldmmdl_g3dobj.c
//--------------------------------------------------------------
extern void MMDL_G3DOBJCONT_Setup(
    MMDLSYS *mmdlsys, FLD_G3DOBJ_CTRL *g3dobj_ctrl );
extern void MMDL_G3DOBJCONT_Delete( MMDLSYS *mmdlsys );
extern FLD_G3DOBJ_CTRL * MMDL_G3DOBJCONT_GetFldG3dObjCtrl( MMDL *mmdl );
extern FLD_G3DOBJ_OBJIDX MMDL_G3DOBJCONT_AddObject( MMDL *mmdl, u16 code );
extern void MMDL_G3DOBJCONT_DeleteObject(
    MMDL *mmdl, FLD_G3DOBJ_OBJIDX idx );

#endif //MMDL_H_FILE
