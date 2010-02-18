//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�r�[�R���ʐM/G�p���[�֘A
 * @file   scrcmd_beacon.h
 * @author iwasawa
 * @date   2010.02.15
 */
//////////////////////////////////////////////////////////////////////

#pragma once

//--------------------------------------------------------------------
/**
 * @brief �r�[�R�����M���N�G�X�g�Ăяo�� 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:w
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdBeaconSetRequest( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief �t�B�j�b�V���҂���G�p���[�`�F�b�N 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdCheckGPowerFinish( VMHANDLE* core, void* wk );

//--------------------------------------------------------------------
/**
 * @brief G�p���[�t�B�[���h�G�t�F�N�g
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:
 */
//--------------------------------------------------------------------
extern VMCMD_RESULT EvCmdGPowerUseEffect( VMHANDLE* core, void* wk );



