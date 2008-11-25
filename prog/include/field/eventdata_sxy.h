//============================================================================================
/**
 * @file	event_check_data.h
 * @brief	�C�x���g�N���`�F�b�N�p�f�[�^��`�w�b�_
 * @date	2005.10.14
 *
 * src/fielddata/eventdata/�̃C�x���g�f�[�^�R���o�[�^������Q�Ƃ���Ă���B
 * ���̂��߂��̒��ɋL�q�ł�����e�ɂ͂��Ȃ萧�������邱�Ƃ𒍈ӂ��邱�ƁB
 *
 * 2008.11.20	DP�v���W�F�N�g����R�s�[�B�Ȃ̂ŏ�L���߂͍��̂Ƃ��떳��
 */
//============================================================================================
#pragma once

#include "field/eventdata_system.h"
#include "field/location.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
///����ڑ��w��pID
//------------------------------------------------------------------
#define	ZONE_ID_SPECIAL		(0x0fff)
#define	SPECIAL_SPEXIT01	(0x0100)

enum {
	EXIT_TYPE_NONE = 0,
	EXIT_TYPE_UP,
	EXIT_TYPE_DOWN,
	EXIT_TYPE_LEFT,
	EXIT_TYPE_RIGHT,
};
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _EVENTDATA_HEADER EVENTDATA_HEADER;

struct _EVENTDATA_HEADER {
	u32 bg_count;
	u32 obj_count;
	u32 connect_count;
	u32 pos_count;
};
//------------------------------------------------------------------
///�h�A�����N�p�f�[�^�\����
//------------------------------------------------------------------
typedef struct _CONNECT_DATA CONNECT_DATA;

struct _CONNECT_DATA{
	VecFx32 pos;
	u16	link_zone_id;
	u16	link_exit_id;
	s16	exit_type;
};

//------------------------------------------------------------------
///BG�b�����f�[�^�\����
//------------------------------------------------------------------
typedef struct _BG_TALK_DATA BG_TALK_DATA;

struct _BG_TALK_DATA{
	u16	id;			// ID
	u16	type;		// �f�[�^�^�C�v
	int	gx;			// X���W
	int	gz;			// Y���W
	int	height;		// ����
	u16	dir;		// �b�����������^�C�v
};

//------------------------------------------------------------------
///POS�����C�x���g�f�[�^�\����
//------------------------------------------------------------------
typedef struct _POS_EVENT_DATA POS_EVENT_DATA;

struct _POS_EVENT_DATA{
	u16	id;			//ID
	u16	gx;			//x
	u16	gz;			//z
	u16	sx;			//sizeX
	u16	sz;			//sizeZ
	u16	height;		//height
	u16 param;
	u16 workID;
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern const CONNECT_DATA * EVENTDATA_SearchConnectByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos);
extern const CONNECT_DATA * EVENTDATA_GetConnectByID(const EVENTDATA_SYSTEM * evdata, u16 exit_id);
extern void CONNECTDATA_SetLocation(const CONNECT_DATA * connect, LOCATION * loc);


