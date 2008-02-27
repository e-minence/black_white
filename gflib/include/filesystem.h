//===================================================================
/**
 * @file	filesystem.h
 * @brief	ROM�t�@�C���V�X�e��
 * @author	GAME FREAK Inc.
 * @date	06.11.09
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------
/**
 * @brief	�w��t�@�C���̃T�C�Y��Ԃ�
 * @param	path	�t�@�C���ւ̃p�X
 * @return	int	�t�@�C���T�C�Y(bytes)
 */
//------------------------------------------------------------------
extern int GF_GetFileSize(char *path);

//------------------------------------------------------------------
/**
 * @brief	�w��t�@�C���̓ǂݍ���
 * @param	path	�t�@�C���ւ̃p�X
 * @param	buf	�ǂݏo�����f�[�^���i�[����|�C���^
 * @return	int	�ǂݍ��񂾃t�@�C���T�C�Y
 */
//------------------------------------------------------------------
extern int GF_ReadFile(char *path, void *buf);

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif	// __FILESYSTEM_H__
/*  */

