//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_no.h
 *	@brief		�V�C�i���o�[
 *	@author		tomoya takahashi
 *	@data		2009.03.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifdef _cplusplus
extern "C"{
#endif

#ifndef __WEATHER_NO_H__
#define __WEATHER_NO_H__


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

#ifndef __ASM_NO_DEF_
//-------------------------------------
///	�V�C�i���o�[�^�w��(battle.h BtlWeather�^�Ƃ̍��ʉ��̂���)
//=====================================
typedef u16 WEATHER_NO;
#endif  //__ASM_NO_DEF_

//-------------------------------------
///	�V�C�i���o�[����
//=====================================
#define   WEATHER_NO_NONE   (0xffff)

//-------------------------------------
///	�V�C�i���o�[
//=====================================
#define		WEATHER_NO_SUNNY			(0)				// ����
#define		WEATHER_NO_SNOW				(1)				// ��		
#define		WEATHER_NO_RAIN				(2)				// �J		
#define		WEATHER_NO_STORM			(3)				// ����		
#define		WEATHER_NO_SNOWSTORM		(4)				// ����
#define		WEATHER_NO_ARARE			(5)				// ����
#define		WEATHER_NO_RAIKAMI			(6)				// ���C�J�~
#define		WEATHER_NO_KAZAKAMI			(7)				// �J�U�J�~
#define		WEATHER_NO_DIAMONDDUST	(8)				// �_�C�������h�_�X�g
#define		WEATHER_NO_MIST	        (9)		// ��
#define		WEATHER_NO_PALACE_WHITE_MIST	(10)		// �p���X�@White�p��
#define		WEATHER_NO_PALACE_BLACK_MIST	(11)		// �p���X�@Black�p��

#define		WEATHER_NO_NUM				(12)			// �V�C��

	// �퓬�t�B�[���h��Ԃ��Ă��悤
	// �o�g���t�@�N�g���[�̃��[���b�g�{�݂�
	// ���ۂ̃t�B�[���h������V�C�ȊO�̏�Ԃ�
	// �o�g���ɓn���Ȃ��Ă͂����Ȃ��Ȃ������ߍ쐬�B
	//
	// PL��GS�̒ʐM�ł��g�p�����萔�Ȃ̂ŁA
	// �l�͈��ɂ��Ă��������B
#define		WEATHER_NO_BTONLYSTART (1000)	// �o�g���I�����[�̏�ԊJ�n�萔
#define		WEATHER_NO_HIGHSUNNY	(1001)	// ���{����
#define		WEATHER_NO_TRICKROOM	(1002)	// �g���b�N���[��



#if 0		// �v���`�i�̓V�C�ꗗ
#define		WEATHER_NO_SUNNY			(0)				// ����
#define		WEATHER_NO_CLOUDINESS		(1)				// �܂�		BG	
#define		WEATHER_NO_RAIN				(2)				// �J		OAM	FOG
#define		WEATHER_NO_STRAIN			(3)				// ��J		OAM FOG 
#define		WEATHER_NO_SPARK			(4)				// ��J		OAM FOG BG
#define		WEATHER_NO_SNOW				(5)				// ��		OAM FOG
#define		WEATHER_NO_SNOWSTORM		(6)				// ����		OAM FOG
#define		WEATHER_NO_SNOWSTORM_H		(7)				// �Ґ���	OAM FOG BG
#define		WEATHER_NO_FOG				(8)				// ����		FOG	
#define		WEATHER_NO_VOLCANO			(9)				// �ΎR�D	OAM FOG BG
#define		WEATHER_NO_SANDSTORM		(10)			// BG�g�p����	OAM FOG BG
#define		WEATHER_NO_DIAMONDDUST		(11)			// �X�m�[�_�X�g	OAM FOG
#define		WEATHER_NO_SPIRIT			(12)			// �C��		OAM
#define		WEATHER_NO_MYSTIC			(13)			// �_��		OAM FOG
#define		WEATHER_NO_MIST1			(14)			// �������̖�	FOG	BG
#define		WEATHER_NO_MIST2			(15)			// �������̖�	FOG	BG
#define		WEATHER_NO_FLASH			(16)			// �t���b�V��		BG

#define		WEATHER_NO_SPARK_EFF		(17)			// ��		BG
#define		WEATHER_NO_FOGS				(18)			// ��		FOG
#define		WEATHER_NO_FOGM				(19)			// ����		FOG

#define		WEATHER_NO_RAINBOW			(20)			// ��		BG
#define		WEATHER_NO_SNOW_STORM_BG	(21)			// BG�g�p����	OAM FOG BG
#define		WEATHER_NO_STORM			(22)			// ����		OAM FOG

#define		WEATHER_NO_KOGOREBI			(23)			// �Â��	BG
#define		WEATHER_NO_DOUKUTU00		(24)			// ���A	FOG
#define		WEATHER_NO_DOUKUTU01		(25)			// ���A	FOG
#define		WEATHER_NO_GINGA00			(26)			// ��͒c�����Ĕ��Â�
#define		WEATHER_NO_LOSTTOWER00		(27)			// LostTower���Â�
#define		WEATHER_NO_HARDMOUNTAIN		(28)			// �n�[�h�}�E���e��
#define		WEATHER_NO_DOUKUTU02		(29)			// ���A�@FOG�@��
#define		WEATHER_NO_USUGURAI			(30)			// ���Â��@FOG

#define		WEATHER_NO_NUM				(31)			// �V�C��
#endif





#endif		// __WEATHER_NO_H__

#ifdef _cplusplus
}
#endif

