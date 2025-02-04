//===================================================================
/**
 * @file	filesystem.h
 * @brief	ROMファイルシステム
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
 * @brief	指定ファイルのサイズを返す
 * @param	path	ファイルへのパス
 * @return	int	ファイルサイズ(bytes)
 */
//------------------------------------------------------------------
extern int GF_GetFileSize(char *path);

//------------------------------------------------------------------
/**
 * @brief	指定ファイルの読み込み
 * @param	path	ファイルへのパス
 * @param	buf	読み出したデータを格納するポインタ
 * @return	int	読み込んだファイルサイズ
 */
//------------------------------------------------------------------
extern int GF_ReadFile(char *path, void *buf);

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif	// __FILESYSTEM_H__
/*  */

