//============================================================================================
/**
 * @file  ringtone_sys_ext.h
 * @date  2010.06.20
 * @author  tamada GAME FREAK inc.
 * @brief   TVトランシーバー着信音制御のフィールド外公開用
 */
//============================================================================================
#pragma once

//着信音制御対応のDS開閉時サウンドコールバックを登録
extern void RINGTONE_SYS_SetSleepSoundFunc( void );
extern BOOL RINGTONE_SYS_IsSleepSoundFunc( void );

