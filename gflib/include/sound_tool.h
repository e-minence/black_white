
//=============================================================================================
/**
 * @file	sound_tool.h
 * @brief	サウンドツールルーチン
 * @author	Hisashi Sogabe
 * @date	2007/11/21
 */
//=============================================================================================
#ifndef __SOUND_TOOL_H_
#define __SOUND_TOOL_H_

//--------------------------------------------------------------------------------------------
/**
 *	SENoとPitchを設定して再生
 *
 *	@param	se_no	再生するSENo
 *	@param	pitch	音程変化の値（-32768〜32767）
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SNDTOOL_PlaySEwithPitch( int se_no, int pitch );

//--------------------------------------------------------------------------------------------
/**
 *	SENoとPanを設定して再生
 *
 *	@param	se_no	再生するSENo
 *	@param	pan		定位変化の値（-128〜127）
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SNDTOOL_PlaySEwithPan( int se_no, int pan );

//--------------------------------------------------------------------------------------------
/**
 *	SENoとPitchとPanを設定して再生
 *
 *	@param	se_no	再生するSENo
 *	@param	pitch	音程変化の値（-32768〜32767）
 *	@param	pan		定位変化の値（-128〜127）
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SNDTOOL_PlaySEwithPitchPan( int se_no, int pitch, int pan );

#endif __SOUND_TOOL_H_
