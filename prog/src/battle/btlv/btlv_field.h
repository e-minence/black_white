
//============================================================================================
/**
 * @file	btlv_field.h
 * @brief	�퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#pragma once

enum{
	BTLV_FIELD_GROUND = 0,		//�n��
	BTLV_FIELD_BG,		    		//�w�i
	BTLV_FIELD_MAX			    	//�t�B�[���h�\��OBJ��
};

typedef enum
{ 
  BTLV_FIELD_VANISH_OFF = 0,
  BTLV_FIELD_VANISH_ON,
}BTLV_FIELD_VANISH;

typedef struct _BTLV_FIELD_WORK BTLV_FIELD_WORK;

extern	BTLV_FIELD_WORK*  BTLV_FIELD_Init( int index, HEAPID heapID );
extern	void              BTLV_FIELD_Exit( BTLV_FIELD_WORK *bsw );
extern	void              BTLV_FIELD_Main( BTLV_FIELD_WORK *bsw );
extern	void              BTLV_FIELD_Draw( BTLV_FIELD_WORK *bsw );
extern  void              BTLV_FIELD_SetPaletteFade( BTLV_FIELD_WORK *bfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
extern  BOOL              BTLV_FIELD_CheckExecutePaletteFade( BTLV_FIELD_WORK* bfw );
extern  void              BTLV_FIELD_SetVanishFlag( BTLV_FIELD_WORK* bfw, BTLV_FIELD_VANISH flag );

