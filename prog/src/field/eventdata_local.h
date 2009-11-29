//============================================================================================
/**
 * @file	eventdata_local.h
 * @brief
 * @date	2008.12.05
 * @author	tamada GAMEFREAK inc.
 */
//============================================================================================

#pragma once

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _EVENTDATA_HEADER {
	u32 bg_count;
	u32 obj_count;
	u32 connect_count;
	u32 pos_count;
};


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

