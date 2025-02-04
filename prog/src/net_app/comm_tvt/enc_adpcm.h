//======================================================================
/**
 * @file	enc_adpcm.c
 * @brief	ADPCM変換
 * @author	ariizumi
 * @data	09/11/11
 *
 * モジュール名：ENC_ADPCM
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
extern void ENC_ADPCM_ResetParamEncode( void ); 
extern void ENC_ADPCM_ResetParamDecode( void ); 
extern const u32 ENC_ADPCM_EncodeData( void* src , const u32 srcSize , void* dst );
extern const u32 ENC_ADPCM_DecodeData( void* src , const u32 srcSize , void* dst );

