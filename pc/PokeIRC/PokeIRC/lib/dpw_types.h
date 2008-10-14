/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     dpw_types.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	DPW���C�u�����Ŏg�p�����^�̒�`
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	
	@version 0.01 (2006/07/20)
		@li �V�K�����[�X���܂����B
*/

#ifndef DPW_TYPES_H_
#define DPW_TYPES_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "dpw_typedef.h"
#include "poketool.h"

/*-----------------------------------------------------------------------*
					���E�ʐM�����Ŏg�p�����^�E��`
 *-----------------------------------------------------------------------*/

//! ���O�̃T�C�Y
#define DPW_TR_NAME_SIZE 8

//! �����\����
typedef struct {
	u16 year;	//!< �N
	u8 month;	//!< ��
	u8 day;		//!< ��
	u8 hour;	//!< ��
	u8 minutes;	//!< ��
	u8 sec;		//!< �b
	u8 padding;	// �p�f�B���O
} Dpw_Tr_Date;

//! �|�P�����̐���
typedef enum {
	DPW_TR_GENDER_MALE = 1,		//!< �I�X
	DPW_TR_GENDER_FEMALE = 2,	//!< ���X
	DPW_TR_GENDER_NONE = 3		//!< ���ʂȂ��B�������ɐ��ʂ���Ȃ��悤�ɂ��邽�߂ɂ́A���̒l���w�肵�Ă��������B
} Dpw_Tr_PokemonGender;

//! �|�P�����f�[�^�̃T�[�o�[�����p�\����
typedef struct {
	s16 characterNo;	//!< �L�����N�^�ԍ�
	s8 gender;			//!< ���ʁB Dpw_Tr_PokemonGender �̒l�ɂȂ�B
	s8 level;			//!< ���x���B
} Dpw_Tr_PokemonDataSimple;

//! �|�P�����f�[�^�̃T�[�o�[���������\����
typedef struct {
	s16 characterNo;	//!< �L�����N�^�ԍ�
	s8 gender;			//!< ���ʁB Dpw_Tr_PokemonGender �̒l�ɂȂ�B
	s8 level_min;		//!< ���x���̉����B0�͎w��Ȃ��A1�`100�͂��̃��x���ȏ�̃|�P�����Ƃ����Ӗ��ɂȂ�B
	s8 level_max;		//!< ���x���̏���B0�͎w��Ȃ��A1�`100�͂��̃��x���ȉ��̃|�P�����Ƃ����Ӗ��ɂȂ�B
	s8 padding;			// �p�f�B���O
} Dpw_Tr_PokemonSearchData;

//! �|�P�����̃f�[�^�B
typedef struct {
	POKEMON_PARAM data;		//!< �|�P�����̃f�[�^
} Dpw_Tr_PokemonData;

//! Dpw_Tr���C�u�����ŃT�[�o�[�Ƃ���肳��郁�C���f�[�^�\����
typedef struct {
	Dpw_Tr_PokemonData postData;			//!< �u�a����v�|�P�����̏ڍ׃f�[�^
	Dpw_Tr_PokemonDataSimple postSimple;	//!< �u�a����v�|�P�����̊ȗ��f�[�^�i�T�[�o�[�����p�j
	Dpw_Tr_PokemonSearchData wantSimple;	//!< �u�ق����v�|�P�����̊ȗ��f�[�^�i�T�[�o�[�����p�j
	u8 gender;								//!< ��l���̐���
	u8 padding;								// �p�f�B���O
	Dpw_Tr_Date postDate;					//!< �a���������i�T�[�o�[�ɂăZ�b�g�j
	Dpw_Tr_Date tradeDate;					//!< �������������i�T�[�o�[�ɂăZ�b�g�j
	s32 id;									//!< ���̃f�[�^��ID�i�T�[�o�[�ɂăZ�b�g�j
	u16 name[DPW_TR_NAME_SIZE];				//!< ��l����
	u16 trainerID;							//!< �g���[�i�[��ID
	u8 countryCode;							//!< �Z��ł��鍑�R�[�h
	u8 localCode;							//!< �Z��ł���n���R�[�h
	u8 trainerType;							//!< �g���[�i�[�^�C�v
	s8 isTrade;								//!< �����ς݃t���O�i�T�[�o�[�ɂăZ�b�g�j
	u8 versionCode;							//!< �o�[�W�����R�[�h
	u8 langCode;							//!< ����R�[�h
} Dpw_Tr_Data;

/*-----------------------------------------------------------------------*
					�o�g���^���[�Ŏg�p�����^�E��`
 *-----------------------------------------------------------------------*/

//! �|�P�����̃f�[�^�B
typedef struct {
	B_TOWER_POKEMON data;		//!< �|�P�����̃f�[�^
} Dpw_Bt_PokemonData;

//! ���[�_�[�f�[�^�\����
typedef struct {
	u16 playerName[8];		//!< �v���C���[��
	u8 versionCode;			//!< �o�[�W�����R�[�h
	u8 langCode;			//!< ����R�[�h
	u8 countryCode;			//!< �Z��ł��鍑�R�[�h
	u8 localCode;			//!< �Z��ł���n���R�[�h
	u8 playerId[4];			//!< �v���C���[ID
	s8 leaderMessage[8];	//!< ���[�_�[���b�Z�[�W
	union{
		struct{
			u8 ngname_f	:1;	//!< NG�l�[���t���O
			u8 gender	:1;	//!< �v���C���[�̐��ʃt���O
			u8			:6;	// ���܂�6bit
		};
		u8	  flags;
	};
	u8 padding;				// �p�f�B���O
} Dpw_Bt_Leader;

//! �v���C���[�f�[�^�\����
typedef struct {
	Dpw_Bt_PokemonData pokemon[3];	//!< �|�P�����f�[�^
	u16 playerName[8];		//!< �v���C���[��
	u8 versionCode;			//!< �o�[�W�����R�[�h
	u8 langCode;			//!< ����R�[�h
	u8 countryCode;			//!< �Z��ł��鍑�R�[�h
	u8 localCode;			//!< �Z��ł���n���R�[�h
	u8 playerId[4];			//!< �v���C���[ID
	s8 leaderMessage[8];	//!< ���[�_�[���b�Z�[�W
	union{
		struct{
			u8 ngname_f	:1;	//!< NG�l�[���t���O
			u8 gender	:1;	//!< �v���C���[�̐��ʃt���O
			u8			:6;	// ���܂�6bit
		};
		u8	  flags;
	};
	u8 trainerType;			//!< �g���[�i�[�^�C�v
	s8 message[24];			//!< ���b�Z�[�W
	u16 result;				//!< ����
} Dpw_Bt_Player;

//! ���[���f�[�^�\����
typedef struct {
	Dpw_Bt_Player player[7];	//!< �v���C���[�f�[�^
	Dpw_Bt_Leader leader[30];	//!< ���[�_�[�f�[�^
} Dpw_Bt_Room;


#ifdef __cplusplus
}
#endif

#endif /* DPW_TYPES_H_ */
