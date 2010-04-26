//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_editor_data.h
 *	@brief	���[���G�f�B�^�[����M���
 *	@author	tomoya takahashi
 *	@date		2009.07.13
 *
 *	���W���[�����F
 *
 *	*���[���G�f�B�^�[PC���ł��g�p���Ă��܂��B
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	����M�f�[�^�^�C�v
//=====================================
typedef enum {
	RE_MCS_DATA_NONE,				// �_�~�[
	RE_MCS_DATA_MCSUSE,			// �_�~�[
	RE_MCS_DATA_RAIL,				// ���[�����			DS<->PC
	RE_MCS_DATA_AREA,				// �G���A���			DS<->PC
	RE_MCS_DATA_ATTR,				// �A�g���r���[�g���	DS<->PC
	RE_MCS_DATA_PLAYERDATA,	// �v���C���[���	DS->PC
	RE_MCS_DATA_CAMERADATA,	// �J�������			DS->PC
	RE_MCS_DATA_SELECTDATA,	// �I�����				DS<-PC
	RE_MCS_DATA_RAIL_REQ,		// ���[�����z�o�����N�G�X�g	DS<-PC
	RE_MCS_DATA_AREA_REQ,		// �G���A���z�o�����N�G�X�g	DS<-PC
	RE_MCS_DATA_ATTR_REQ,		// �A�g���r���[�g���z�o�����N�G�X�g	DS<-PC
	RE_MCS_DATA_PLAYER_REQ,	// ��l�����z�o�����N�G�X�g	DS<-PC
	RE_MCS_DATA_CAMERA_REQ,	// �J�������z�o�����N�G�X�g	DS<-PC
	RE_MCS_DATA_RESET_REQ,	// ��ʃ��Z�b�g���N�G�X�g	DS<-PC
	RE_MCS_DATA_END_REQ,		// �I���N�G�X�g	DS<-PC
	RE_MCS_DATA_RAILLOCATIONDATA,		// ���[�����P�[�V�������				DS->PC
	RE_MCS_DATA_RAILLOCATION_REQ,	// ���[�����P�[�V�������N�G�X�g	DS<-PC

	RE_MCS_DATA_TYPE_MAX,
} RE_MCS_DATA_TYPE;


//-------------------------------------
///	�I�����
//=====================================
typedef enum {
	RE_SELECT_DATA_NONE,
	RE_SELECT_DATA_POINT,
	RE_SELECT_DATA_LINE,
	RE_SELECT_DATA_AREA,

	RE_SELECT_DATA_MAX,
} RE_SELECT_DATA_TYPE;

//-------------------------------------
///	�I����񂲂Ƃ́A�Ǘ��V�[�P���X
//=====================================
typedef enum {
	RE_SELECT_DATA_SEQ_NONE,
	RE_SELECT_DATA_SEQ_FREE_NORMAL,
	RE_SELECT_DATA_SEQ_FREE_CIRCLE,
	RE_SELECT_DATA_SEQ_FREE_GRID,
	RE_SELECT_DATA_SEQ_RAIL,
	RE_SELECT_DATA_SEQ_CAMERA_POS,
	RE_SELECT_DATA_SEQ_CAMERA_TARGET,
	RE_SELECT_DATA_SEQ_LINEPOS_CENTER_POS,

	RE_SELECT_DATA_SEQ_TYPE_MAX,
} RE_SELECT_DATA_SEQ_TYPE;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	����M���header
//=====================================
typedef struct {
	u32 data_type;		// RE_MCS_DATA_TYPE
} RE_MCS_HEADER;

//-------------------------------------
///	���[�����\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	u32				reset;	//DS����M�p�@��ʂ��������񃊃Z�b�g���܂��B
	u32				rail[];
} RE_MCS_RAIL_DATA;

//-------------------------------------
///	�G���A���\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	u32				reset;	//DS����M�p�@��ʂ��������񃊃Z�b�g���܂��B
	u32				area[];
} RE_MCS_AREA_DATA;

//-------------------------------------
///	�G���A���\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	u32				attr[];
} RE_MCS_ATTR_DATA;

//-------------------------------------
///	�v���C���[���\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	fx32					pos_x;
	fx32					pos_y;
	fx32					pos_z;
	fx32					target_length;
} RE_MCS_PLAYER_DATA;
//-------------------------------------
///	�J�������\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	fx32					pos_x;
	fx32					pos_y;
	fx32					pos_z;
	fx32					target_x;
	fx32					target_y;
	fx32					target_z;
	u16						pitch;
	u16						yaw;
	fx32					len;
	fx32					target_offset_x;  //���@�ʒu��̃^�[�Q�b�g�I�t�Z�b�g
	fx32					target_offset_y;
	fx32					target_offset_z;
} RE_MCS_CAMERA_DATA;

//-------------------------------------
///	�I�����\����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	u32						select_data;			// �I���f�[�^�^�C�v	RE_SELECT_DATA_TYPE
	u32						select_index;			// �I���f�[�^�C���f�b�N�X
	u32						select_seq;				// �I���f�[�^�Ǘ��V�[�P���X	RE_SELECT_DATA_SEQ_TYPE
} RE_MCS_SELECT_DATA;

//-------------------------------------
///	���[�����P�[�V����
//=====================================
typedef struct {
	RE_MCS_HEADER header;
	u32						rail_type;			// ���[���^�C�v	FIELD_RAIL_TYPE
	u32						index;					// �f�[�^�C���f�b�N�X
} RE_MCS_RAILLOCATION_DATA;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

#ifdef _cplusplus
}	// extern "C"{
#endif



