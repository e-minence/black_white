//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�FWi-Fi�֘A(�풓)
 * @file   scrcmd_wifi_st.h
 * @author iwasawa
 * @date   2009.12.08
 */
//////////////////////////////////////////////////////////////////////

#pragma once

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
extern VMCMD_RESULT EvCmdWifiRandomMatchEventCall( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief   �o�g�����R�[�_�[�C�x���g���Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * �Ăяo�����[�h�w�� field/script_def.h
 *  SCRCMD_BTL_RECORDER_MODE_VIDEO   (0)
 *  SCRCMD_BTL_RECORDER_MODE_MUSICAL (1)
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdWifiBattleRecorderEventCall( VMHANDLE* core, void* wk );



