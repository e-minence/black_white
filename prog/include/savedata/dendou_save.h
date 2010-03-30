//==============================================================================
/**
 * @file    dendou_save.h
 * @brief   �a������Z�[�u�f�[�^
 * @author  matsuda / mori
 * @date    2010.03.12(��)
 *
 * @note    ����a������f�[�^�́u�a������P�T���F�O���v�ƁA
 *          �u���N���A���P���F�����v�̂Q�ӏ��ɕۑ������B
 *          �܂��w�a������x�̓��[�e�[�V�����\����DENDOU_SAVEDATA���g�p���邪�A
 *          �w���N���A�x�͂P���݂̂ōςނ̂�DENDOU_SAVEDATA�̒���DENDOU_RECORD��
 *          �\�ɏo���ăA�N�Z�X�ł���悤�ɂ��Ă���B
 *        
 */
//==============================================================================
#pragma once

#include "poke_tool/pokeparty.h"
#include "buflen.h"



//--------------------------------------------------------------
/**
 *  �֘A�萔
 */
//--------------------------------------------------------------
enum {
  DENDOU_RECORD_MAX = 15,   // �Z�[�u�̈�ɕۑ������a������f�[�^�̍ő匏��
  DENDOU_NUMBER_MAX = 9999, // �f�[�^�ǉ������ʎZ�񐔂̍ő�i����ȏ�͓a�����肵�Ă��ǉ�����Ȃ��j
};

//--------------------------------------------------------------
/**
 *  �a������Z�[�u�f�[�^�̕s���S�^�\���̐錾
 */
//--------------------------------------------------------------
typedef struct _DENDOU_SAVEDATA DENDOU_SAVEDATA;  // �a������f�[�^�i�O���F15�C�p
typedef struct DENDOU_RECORD    DENDOU_RECORD;    // 

//--------------------------------------------------------------
/**
 *  �a������Z�[�u�f�[�^����|�P�����f�[�^���擾���邽�߂̍\����
 *�i������󂯎��p�� STRBUF �͍쐬���Ă������ƁI�j
 */
//--------------------------------------------------------------
typedef struct {
  STRBUF*   nickname;
  STRBUF*   oyaname;

  u32     personalRandom;
  u32     idNumber;
  u16     monsno;
  u8      level;
  u8      formNumber:6;
  u8      sex:2;
  
  u16     waza[4];

}DENDOU_POKEMON_DATA;






//------------------------------------------------------------------
/**
 * �a������f�[�^�P���ǉ�
 *
 * @param   data    �a������Z�[�u�f�[�^�|�C���^
 * @param   party   �a������p�[�e�B�[�̃f�[�^
 * @param   date    �a��������t�f�[�^
 *
 */
//------------------------------------------------------------------
extern void DendouData_AddRecord( DENDOU_SAVEDATA* data, const POKEPARTY* party, const RTCDate* date, HEAPID heap_id );

// �a������f�[�^�擾�֘A
extern u32  DendouData_GetRecordCount( const DENDOU_SAVEDATA* data );
extern u32  DendouData_GetRecordNumber( const DENDOU_SAVEDATA* data, int index );
extern u32  DendouData_GetPokemonCount( const DENDOU_SAVEDATA* data, int index );
extern void DendouData_GetPokemonData( const DENDOU_SAVEDATA* data, int index, int poke_pos, DENDOU_POKEMON_DATA* poke_data );
extern void DendouData_GetDate( const DENDOU_SAVEDATA* data, int index, RTCDate* date );

extern u32 DendouData_GetWorkSize( void );
extern void DendouData_Init( void *work );

//------------------------------------------------------------------
/**
 * �Q�[���N���A���Z�[�u�f�[�^�ǉ�(1�񂵂��o�^���܂���j
 *
 * @param   data    �N���A�Z�[�u�f�[�^�|�C���^
 * @param   party   �N���A�p�[�e�B�[�̃f�[�^
 * @param   date    �N���A���t�f�[�^
 *
 */
//------------------------------------------------------------------
extern void DendouRecord_Add( DENDOU_RECORD* record, const POKEPARTY* party, const RTCDate* date, HEAPID heap_id );

// �Q�[���N���A�f�[�^�擾�֘A
extern u32  DendouRecord_GetPokemonCount( const DENDOU_RECORD* record );
extern void DendouRecord_GetPokemonData( const DENDOU_RECORD* record, int poke_pos, DENDOU_POKEMON_DATA* poke_data );
extern void DendouRecord_GetDate( const DENDOU_RECORD* record, RTCDate* date );


extern u32  GameClearData_GetWorkSize( void );
extern void GameClearData_Init( void *work );
