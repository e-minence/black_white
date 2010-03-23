//============================================================================================
/**
 * @file	eventdata_local.h
 * @brief �C�x���g�z�u�f�[�^�̓����\����`
 * @date	2008.12.05
 * @author	tamada GAMEFREAK inc.
 */
//============================================================================================

#pragma once

#include "field/eventdata_type.h"
#include "fldmmdl.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {

  u8 bg_count;
  u8 npc_count;
  u8 connect_count;
  u8 pos_count;

  u32 buf[];  ///<�f�[�^�@TalkBG NPC CONNECT POS �̏��Ɋi�[

}EVENTDATA_TABLE;


//------------------------------------------------------------------
///�h�A�����N�p�f�[�^�\����
//------------------------------------------------------------------
/// �O���b�h�|�W�V����
typedef struct {
  s16 x;              ///<
  s16 y;
  s16 z;
  u16 sizex;
  u16 sizez;
} CONNECT_DATA_GPOS;

/// ���[���|�W�V����
typedef struct {
  u16 rail_index;       ///<���[���C���f�b�N�X
  u16 front_grid;       ///<�O�����[���O���b�h���W  
  s16 side_grid;        ///<�T�C�h���[���O���b�h���W
  u16 front_grid_size;  ///<�O���͈̓O���b�h�T�C�Y
  u16 side_grid_size;   ///<�T�C�h�͈̓O���b�h�T�C�Y
  u16 rail_way;         ///<���[���L�[����
} CONNECT_DATA_RPOS;

// �|�W�V�����o�b�t�@�T�C�Y
#define CONNECT_DATA_POSBUF_SIZE  (12)

/// �h�A�����N�f�[�^�\����
struct _CONNECT_DATA{
	//VecFx32 pos;
	u16	link_zone_id;
	u16	link_exit_id;
	u8	exit_dir;
	u8	exit_type;

  u16 pos_type; // �|�W�V�����^�C�v EVENTDATA_POSTYPE
  u8  pos_buf[CONNECT_DATA_POSBUF_SIZE];
};

//------------------------------------------------------------------
///BG�b�����f�[�^�\����
//------------------------------------------------------------------
/// �O���b�h�|�W�V����
typedef struct {
	int	gx;			// X���W
	int	gz;			// Y���W
	int	height;		// ����
} BG_TALK_DATA_GPOS;

/// ���[���|�W�V����
typedef struct {
  u16 rail_index;       ///<���[���C���f�b�N�X
  u16 front_grid;       ///<�O�����[���O���b�h���W
  s16 side_grid;        ///<�T�C�h���[���O���b�h���W
} BG_TALK_DATA_RPOS;

// �|�W�V�����\���̃T�C�Y
#define BG_TALK_DATA_POSBUF_SIZE  (12)

/// BG�b�����f�[�^�\����
struct _BG_TALK_DATA{
	u16	id;			// ID
	u16	type;		// �f�[�^�^�C�v
	u16	dir;		// �b�����������^�C�v

  u16 pos_type; // �|�W�V�����^�C�v EVENTDATA_POSTYPE
  u8  pos_buf[BG_TALK_DATA_POSBUF_SIZE];
};

//------------------------------------------------------------------
///POS�����C�x���g�f�[�^�\����
//------------------------------------------------------------------
/// �O���b�h�|�W�V����
typedef struct {
	u16	gx;			//x
	u16	gz;			//z
	u16	sx;			//sizeX
	u16	sz;			//sizeZ
	s16	height;		//height
} POS_EVENT_DATA_GPOS;

/// ���[���|�W�V����
typedef struct {
  u16 rail_index;       ///<���[���C���f�b�N�X
  u16 front_grid;       ///<�O�����[���O���b�h���W
  s16 side_grid;        ///<�T�C�h���[���O���b�h���W
  u16 front_grid_size;  ///<�O���͈̓O���b�h�T�C�Y
  u16 side_grid_size;   ///<�T�C�h�͈̓O���b�h�T�C�Y
} POS_EVENT_DATA_RPOS;

// �|�W�V�����o�b�t�@�T�C�Y
#define POS_EVENT_DATA_POSBUF_SIZE  (12)

/// POS�����C�x���g�f�[�^�\����
struct _POS_EVENT_DATA{
	u16	id;			//ID
	u16 param;
	u16 workID;
  u16 check_type;

  u16 pos_type; // �|�W�V�����^�C�v EVENTDATA_POSTYPE
  u8  pos_buf[POS_EVENT_DATA_POSBUF_SIZE];
};


//------------------------------------------------------------------
//  �e�C�x���g�f�[�^�̎擾�֐�
//------------------------------------------------------------------
extern const BG_TALK_DATA* EVENTDATA_SYS_HEADER_GetTalkBgEvent( const EVENTDATA_TABLE* cp_header );
extern const MMDL_HEADER* EVENTDATA_SYS_HEADER_GetMmdlEvent( const EVENTDATA_TABLE* cp_header );
extern const CONNECT_DATA* EVENTDATA_SYS_HEADER_GetConnectEvent( const EVENTDATA_TABLE* cp_header );
extern const POS_EVENT_DATA* EVENTDATA_SYS_HEADER_GetPosEvent( const EVENTDATA_TABLE* cp_header );

//------------------------------------------------------------------
//  �C�x���g�f�[�^�@���@�ォ��ǉ��ݒ肷�邽�߂̊֐�
//------------------------------------------------------------------
extern void EVENTDATA_SYS_ReloadEventDataEx(EVENTDATA_SYSTEM * evdata, u16 bg_num, u16 mmdl_num, u16 connect_num, u16 pos_num );
extern u32 EVENTDATA_SYS_AddTalkBgEvent( EVENTDATA_SYSTEM * evdata, const BG_TALK_DATA* cp_data );
extern u32 EVENTDATA_SYS_AddMmdlEvent( EVENTDATA_SYSTEM * evdata, const MMDL_HEADER* cp_data );
extern u32 EVENTDATA_SYS_AddConnectEvent( EVENTDATA_SYSTEM * evdata, const CONNECT_DATA* cp_data );
extern u32 EVENTDATA_SYS_AddPosEvent( EVENTDATA_SYSTEM * evdata, const POS_EVENT_DATA* cp_data );

//------------------------------------------------------------------
//  �ڑ��f�[�^�@���@�ォ��폜���邽�߂̊֐�
//------------------------------------------------------------------
extern void EVENTDATA_SYS_DelConnectEvent( EVENTDATA_SYSTEM * evdata, u16 link_zone_id, u16 link_exit_id );
extern void EVENTDATA_SYS_DelConnectEventIdx( EVENTDATA_SYSTEM * evdata, u16 idx );


//------------------------------------------------------------------
//  �_�~�[POS�C�x���g�̃`�F�b�N
//------------------------------------------------------------------
extern BOOL EVENTDATA_SYS_CheckPosDummyEvent( const EVENTDATA_SYSTEM *evdata, const VecFx32* pos );
