//======================================================================
/**
 * @file	local_tvt_chara.h
 * @brief	��ʐM�e���r�g�����V�[�o�[ �L����
 * @author	ariizumi
 * @data	09/11/02
 *
 * ���W���[�����FLOCAL_TVT_CHARA
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

extern LOCAL_TVT_CHARA* LOCAL_TVT_CHARA_Init( LOCAL_TVT_WORK *work , const u8 idx );
extern void LOCAL_TVT_CHARA_Term( LOCAL_TVT_WORK *work , LOCAL_TVT_CHARA *charaWork );
extern void LOCAL_TVT_CHARA_Main( LOCAL_TVT_WORK *work , LOCAL_TVT_CHARA *charaWork );

extern void LOCAL_TVT_CHARA_LoadChara( LOCAL_TVT_WORK *work , LOCAL_TVT_CHARA *charaWork , const u8 bufNo);
extern const BOOL LOCAL_TVT_CHARA_isFinishTrans( LOCAL_TVT_WORK *work , LOCAL_TVT_CHARA *charaWork );
extern const u8 LOCAL_TVT_CHARA_GetNameLen( LOCAL_TVT_CHARA *charaWork );
extern void LOCAL_TVT_CHARA_RedrawName( LOCAL_TVT_WORK *work , LOCAL_TVT_CHARA *charaWork );
extern void LOCAL_TVT_CHARA_SetLipSync( LOCAL_TVT_CHARA *charaWork , const BOOL flg );
