//===========================================================================
/**
 * @file	zonetableformat.h
 * @brief	�]�[�����f�[�^�e�[�u���̒�`
 * @author	tamada GAME FREAK inc.
 * @date	2008.11.12
 *
 * �v���O�������Ə�ɓ����\���̂��ێ�����K�v������̂ŁA���ӁI
 */
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	�]�[�����Ɏ��f�[�^�̍\��
 */
//------------------------------------------------------------------
typedef struct{
	u8 maprsc_id;						///<�}�b�v��ʎw��i�V�K�j
	u8 area_id;							///<AREA_ID�̎w��
	u8 movemodel_id;					///<���샂�f���̎w��
	u8 matrix_arc;						///<�}�b�v�擾�f�[�^�Z�b�g�̎w��
	u16 matrix_id;						///<�}�b�v�}�g���b�N�X�̎w��
	u16 script_id;						///<�X�N���v�g�t�@�C���̃A�[�J�C�uID
	u16 sp_script_id;					///<����X�N���v�g�t�@�C���̃A�[�J�C�uID
	u16 msg_id;							///<���b�Z�[�W�t�@�C���̃A�[�J�C�uID
	u16 bgm_day_id;						///<BGM�w��i���j��ID
	u16 bgm_night_id;					///<BGM�w��i��j��ID
	u16 enc_data_id;					///<�G���J�E���g�w��
	u16 event_data_id;					///<�C�x���g�f�[�^�t�@�C���̃A�[�J�C�uID
	u16 placename_id:8;					///<�n�����b�Z�[�W�̂h�c
	u16 placename_win:8;				///<�n�����b�Z�[�W�p�̃E�B���h�E
	u8 weather_id;						///<�V��w���ID
	u8 camera_id;						///<�J�����w���ID
	u16 maptype:7;							///<�}�b�v�^�C�v�̎w��(MAPTYPE��`�Q�Ɓj
	u16 battle_bg_type:5;				///<�퓬�w�i�̎w��
	u16 bicycle_flag:1;					///<���]�Ԃɏ��邩�ǂ���
	u16 dash_flag:1;						///<�_�b�V���ł��邩�ǂ���
	u16 escape_flag:1;					///<���Ȃʂ��ł��邩�ǂ���
	u16 fly_flag:1;
	s32 sx, sy, sz;			///<�f�t�H���g�X�^�[�g�ʒu�i�f�o�b�O�p�j
}ZONEDATA;

