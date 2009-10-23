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

