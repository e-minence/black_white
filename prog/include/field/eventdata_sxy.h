//============================================================================================
/**
 * @file	eventdata_sxy.h
 * @brief	�C�x���g�N���`�F�b�N�p�֐�
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
#include "field/fldmmdl.h"

//============================================================================================
//============================================================================================
#include "field/eventdata_type.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�o��������T��
 * @param	evdata	�C�x���g�f�[�^�ւ̃|�C���^
 * @param	pos		�T���ʒu
 * @return	int	�o�����f�[�^�̃C���f�b�N�XEXIT_ID_NONE�̂Ƃ��A�o�����͑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos);

//------------------------------------------------------------------
/**
 * @brief	�o��������T��
 * @param	evdata	�C�x���g�f�[�^�ւ̃|�C���^
 * @param	rail_location		�T�����[�����P�[�V����
 * @return	int	�o�����f�[�^�̃C���f�b�N�XEXIT_ID_NONE�̂Ƃ��A�o�����͑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDByRailLocation(const EVENTDATA_SYSTEM * evdata, const RAIL_LOCATION* rail_location);

//------------------------------------------------------------------
/**
 * @brief	�o���������C���f�b�N�X�w��Ŏ擾����
 * @param	evdata			�C�x���g�f�[�^�ւ̃|�C���^
 * @param	exit_id			�o�������̃C���f�b�N�X
 * @return	CONNECT_DATA	�o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const CONNECT_DATA * EVENTDATA_GetConnectByID(const EVENTDATA_SYSTEM * evdata, u16 exit_id);

//------------------------------------------------------------------
/**
 * @brief	�o�������w�肩��LOCATION���Z�b�g����
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @param	loc			�Z�b�g����LOCATION�ւ̃|�C���^
 * @param	exit_id		�o�������̃C���f�b�N�X
 * @return	BOOL		FALSE�̂Ƃ��A�o������񂪑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL EVENTDATA_SetLocationByExitID(const EVENTDATA_SYSTEM * evdata, LOCATION * loc, u16 exit_id);


//------------------------------------------------------------------
/**
 * @brief	�o��������T��
 * @param	evdata	�C�x���g�f�[�^�ւ̃|�C���^
 * @param	pos		�T���ʒu
 * @return	int	�o�����f�[�^�̃C���f�b�N�XEXIT_ID_NONE�̂Ƃ��A�o�����͑��݂��Ȃ�
 *
 * ��������
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDBySphere(const EVENTDATA_SYSTEM * evdata, const VecFx32 * sphere);

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
extern void CONNECTDATA_SetNextLocation(const CONNECT_DATA * connect, LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief	�o������񂪓���œ�������ǂ����̔���
 * @param	connect		�Q�Ƃ���o�������
 * @return	BOOL		TRUE�̂Ƃ��A����ڑ��o����
 */
//------------------------------------------------------------------
extern BOOL CONNECTDATA_IsSpecialExit(const CONNECT_DATA * connect);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern EXIT_DIR CONNECTDATA_GetExitDir(const CONNECT_DATA * connect);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern EXIT_TYPE CONNECTDATA_GetExitType(const CONNECT_DATA * connect);


//------------------------------------------------------------------
/**
 * @brief	���샂�f���w�b�_�[���擾
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @retval	MMDL_HEADER*
 */
//------------------------------------------------------------------
extern const MMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata );

//------------------------------------------------------------------
/**
 * @brief	���샂�f���w�b�_�[���擾
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @retval	MMDL_HEADER*
 */
//------------------------------------------------------------------
extern const MMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata );

//------------------------------------------------------------------
/**
 * @brief	���샂�f���������擾
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @retval	u16
 */
//------------------------------------------------------------------
extern u16 EVENTDATA_GetNpcCount( const EVENTDATA_SYSTEM *evdata );


//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g���擾
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param pos �`�F�b�N������W
 * @retval NULL = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEvent( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g���擾(�����𖳎�����ver.)
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param pos �`�F�b�N������W
 * @retval NULL = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEvent_XZ( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos ); 

//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g���擾
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param location �`�F�b�N���郌�[�����P�[�V����
 * @retval NULL = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEventRailLocation( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const RAIL_LOCATION* location );


//----------------------------------------------------------------------------
/**
 *	@brief  �o������C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetConnectCenterPos( const CONNECT_DATA * data, VecFx32* pos );
extern void EVENTDATA_GetConnectCenterRailLocation( const CONNECT_DATA * data, RAIL_LOCATION* location );

//----------------------------------------------------------------------------
/**
 *	@brief  BG�b�������C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetBGTalkCenterPos( const BG_TALK_DATA * data, VecFx32* pos );
extern void EVENTDATA_GetBGTalkCenterRailLocation( const BG_TALK_DATA * data, RAIL_LOCATION* location );

//----------------------------------------------------------------------------
/**
 *	@brief  POS�����C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetPosEventCenterPos( const POS_EVENT_DATA * data, VecFx32* pos );
extern void EVENTDATA_GetPosEventCenterRailLocation( const POS_EVENT_DATA * data, RAIL_LOCATION* location );

