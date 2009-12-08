//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�FWi-Fi�֘A(�t�B�[���h�풓)
 * @file   scrcmd_wifi.h
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#pragma once

//--------------------------------------------------------------------
/**
 * @brief   �t�B�[���h�̒ʐM��Ԃ��擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * �R�}���h�̖߂�l game_comm.h GAME_COMM_NO_NULL ��
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdGetFieldCommNo( VMHANDLE* core, void* wk );


//--------------------------------------------------------------------
/**
 * @brief    GSID�������������`�F�b�N����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiCheckMyGSID( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   �F�����蒠�ɓo�^����Ă���l����Ԃ� 
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiGetFriendNum( VMHANDLE* core, void* wk );
//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi�N���u�C�x���g���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiClubEventCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   GTS�l�S�V�G�[�V�����C�x���g���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiGTSEventCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   GeoNet���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiGeoNetCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi�����_���}�b�`���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiRandomMatchCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   �o�g���r�f�I���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiBattleVideoCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   �~���[�W�J���V���b�g���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiMusicalShotCall( VMHANDLE* core, void* wk );

