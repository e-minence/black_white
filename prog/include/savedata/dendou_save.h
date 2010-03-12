//==============================================================================
/**
 * @file    dendou_save.h
 * @brief   �a������Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.03.12(��)
 */
//==============================================================================
#pragma once

#include "poke_tool/pokeparty.h"
#include "buflen.h"



//--------------------------------------------------------------
/**
 *	�֘A�萔
 */
//--------------------------------------------------------------
enum {
	DENDOU_RECORD_MAX = 15,		// �Z�[�u�̈�ɕۑ������a������f�[�^�̍ő匏��
	DENDOU_NUMBER_MAX = 9999,	// �f�[�^�ǉ������ʎZ�񐔂̍ő�i����ȏ�͓a�����肵�Ă��ǉ�����Ȃ��j
};

//--------------------------------------------------------------
/**
 *	�a������Z�[�u�f�[�^�̕s���S�^�\���̐錾
 */
//--------------------------------------------------------------
typedef struct _DENDOU_SAVEDATA		DENDOU_SAVEDATA;


//--------------------------------------------------------------
/**
 *	�a������Z�[�u�f�[�^����|�P�����f�[�^���擾���邽�߂̍\����
 *�i������󂯎��p�� STRBUF �͍쐬���Ă������ƁI�j
 */
//--------------------------------------------------------------
typedef struct {
	STRBUF*		nickname;
	STRBUF*		oyaname;

	u32			personalRandom;
	u32			idNumber;
	u16			monsno;
	u8			level;
	u8			formNumber:6;
	u8      sex:2;
	
	u16			waza[4];

}DENDOU_POKEMON_DATA;







extern u32 DendouData_GetWorkSize( void );
extern void DendouData_Init( void *work );


//------------------------------------------------------------------
/**
 * �a������f�[�^�P���ǉ�
 *
 * @param   data		�a������Z�[�u�f�[�^�|�C���^
 * @param   party		�a������p�[�e�B�[�̃f�[�^
 * @param   date		�a��������t�f�[�^
 *
 */
//------------------------------------------------------------------
extern void DendouData_AddRecord( DENDOU_SAVEDATA* data, const POKEPARTY* party, const RTCDate* date, HEAPID heap_id );




extern u32 DendouData_GetRecordCount( const DENDOU_SAVEDATA* data );
extern u32 DendouData_GetRecordNumber( const DENDOU_SAVEDATA* data, int index );
extern u32 DendouData_GetPokemonCount( const DENDOU_SAVEDATA* data, int index );
extern void DendouData_GetPokemonData( const DENDOU_SAVEDATA* data, int index, int poke_pos, DENDOU_POKEMON_DATA* poke_data );
extern void DendouData_GetDate( const DENDOU_SAVEDATA* data, int index, RTCDate* date );

