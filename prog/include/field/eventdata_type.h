//============================================================================================
/**
 * @file	eventdata_type.h
 * @brief	�C�x���g�N���`�F�b�N�p�f�[�^��`�w�b�_
 * @author  tamada GAMEFREAK inc.
 * @date	2005.10.14
 *
 * resource/fldmapdata/eventdata/�̃C�x���g�f�[�^�R���o�[�^������Q�Ƃ���Ă���B
 * ���̂��߂��̒��ɋL�q�ł�����e�ɂ͂��Ȃ萧�������邱�Ƃ𒍈ӂ��邱�ƁB
 *
 * 2008.11.20	DP�v���W�F�N�g����R�s�[�B
 * 2009.07.25 eventdata_sxy.h���番��
 */
//============================================================================================
#pragma once

//#include "field/eventdata_system.h"
//#include "field/location.h"
//#include "field/fldmmdl.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
///  �����ڑ��w��pID�F�]�[���w��
//------------------------------------------------------------------
#define ZONE_ID_NONE    (0xffff)

//------------------------------------------------------------------
/// �����ڑ��w��pID�F�o�����w��
//------------------------------------------------------------------
#define EXIT_ID_NONE		(0xffff)

//------------------------------------------------------------------
/// �ڑ��C�x���g�̎��
//------------------------------------------------------------------
typedef enum {
	EXIT_TYPE_NONE = 0,
	EXIT_TYPE_MAT,
	EXIT_TYPE_STAIRS,
	EXIT_TYPE_DOOR,
	EXIT_TYPE_WALL,
  EXIT_TYPE_WARP,
  EXIT_TYPE_INTRUDE,
  EXIT_TYPE_SP1,
  EXIT_TYPE_SP2,
  EXIT_TYPE_SP3,
  EXIT_TYPE_SP4,
  EXIT_TYPE_SP5,
  EXIT_TYPE_SP6,
  EXIT_TYPE_SP7,
  EXIT_TYPE_SP8,
  EXIT_TYPE_SP9,
  EXIT_TYPE_SP10,
  EXIT_TYPE_SP11,
  EXIT_TYPE_SP12,
  EXIT_TYPE_SP13,
  EXIT_TYPE_SP14,
  EXIT_TYPE_SP15,
  EXIT_TYPE_SP16,
  EXIT_TYPE_SP17,
  EXIT_TYPE_SP18,
  EXIT_TYPE_SP19,
  EXIT_TYPE_SP20,
  EXIT_TYPE_SP21,
  EXIT_TYPE_SP22,
  EXIT_TYPE_SP23,
  EXIT_TYPE_SP24,
  EXIT_TYPE_SP25,
  EXIT_TYPE_SP26,
  EXIT_TYPE_SP27,
  EXIT_TYPE_SP28,
  EXIT_TYPE_SP29,
  EXIT_TYPE_SP30,
  EXIT_TYPE_SP31,
  EXIT_TYPE_SP32,
  EXIT_TYPE_SP33,
  EXIT_TYPE_SP34,
  EXIT_TYPE_SP35,
  EXIT_TYPE_SP36,
  EXIT_TYPE_SP37,
  EXIT_TYPE_SP38,
  EXIT_TYPE_SP39,
  EXIT_TYPE_SP40,
  EXIT_TYPE_SP41,
  EXIT_TYPE_SP42,
  EXIT_TYPE_SP43,
  EXIT_TYPE_SP44,
  EXIT_TYPE_SP45,
  EXIT_TYPE_SP46,
  EXIT_TYPE_SP47,
  EXIT_TYPE_SP48,
  EXIT_TYPE_SP49,
  EXIT_TYPE_SP50,

  EXIT_TYPE_MAX,
}EXIT_TYPE;

//------------------------------------------------------------------
/// �o���������w��
//------------------------------------------------------------------
typedef enum {
	EXIT_DIR_NON = 0,
	EXIT_DIR_UP,
	EXIT_DIR_DOWN,
	EXIT_DIR_LEFT,
	EXIT_DIR_RIGHT,

	EXIT_DIR_MAX
}EXIT_DIR;

//------------------------------------------------------------------
/// POS�C�x���g�̎��
//------------------------------------------------------------------
typedef enum {
  POS_CHECK_TYPE_NORMAL = 0,
  POS_CHECK_TYPE_UP,
  POS_CHECK_TYPE_DOWN,
  POS_CHECK_TYPE_LEFT,
  POS_CHECK_TYPE_RIGHT,

  POS_CHECK_TYPE_MAX,

  POS_CHECK_TYPE_DUMMY,  // �܂������������Ȃ��_�~�[�C�x���g
}POS_CHECK_TYPE;


//------------------------------------------------------------------
/// BG�C�x���g�̎��
//------------------------------------------------------------------
typedef enum{
  BG_TALK_TYPE_NORMAL,  //�b���|���C�x���g
  BG_TALK_TYPE_BOARD, //�ŔC�x���g
  BG_TALK_TYPE_HIDE,  //�B���A�C�e��
}BG_TALK_TYPE;

//------------------------------------------------------------------
/// BG�C�x���g�̘b��������ގw��
//------------------------------------------------------------------
typedef enum{
  BG_TALK_DIR_DOWN = 0,
  BG_TALK_DIR_LEFT,
  BG_TALK_DIR_RIGHT,
  BG_TALK_DIR_UP,
  BG_TALK_DIR_ALL,
  BG_TALK_DIR_SIDE,
  BG_TALK_DIR_UPDOWN,
}BG_TALK_DIR;

//------------------------------------------------------------------
// �|�W�V�����^�C�v
//------------------------------------------------------------------
typedef enum
{
  EVENTDATA_POSTYPE_GRID = 0,   // �O���b�h�|�W�V����
  EVENTDATA_POSTYPE_RAIL,   // ���[���|�W�V����

  EVENTDATA_POSTYPE_MAX,  // �ő吔
} EVENTDATA_POSTYPE;


//------------------------------------------------------------------
///�h�A�����N�p�f�[�^�\����
//------------------------------------------------------------------
typedef struct _CONNECT_DATA CONNECT_DATA;

//------------------------------------------------------------------
///BG�b�����f�[�^�\����
//------------------------------------------------------------------
typedef struct _BG_TALK_DATA BG_TALK_DATA;

//------------------------------------------------------------------
///POS�����C�x���g�f�[�^�\����
//------------------------------------------------------------------
typedef struct _POS_EVENT_DATA POS_EVENT_DATA;

