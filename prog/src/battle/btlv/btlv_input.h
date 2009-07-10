
//============================================================================================
/**
 * @file	btlv_input.h
 * @brief	戦闘下画面
 * @author	soga
 * @date	2008.06.30
 */
//============================================================================================

#pragma once

typedef struct _BTLV_INPUT_WORK BTLV_INPUT_WORK;

///技選択画面構成用のデータ
typedef struct{
  u16 wazano[ PTL_WAZA_MAX ];
  u16 pp[ PTL_WAZA_MAX ];
  u16 ppmax[ PTL_WAZA_MAX ];
}BTLV_INPUT_WAZA_PARAM;

///攻撃対象選択画面構成用のデータ
typedef struct{
  u8                    sex   :2;
  u8                    exist :1;
  u8                          :5;
  u8                    status;
  u16                   dummy;
  
  const POKEMON_PARAM*  pp;
}BTLV_INPUT_DIR_PARAM;

typedef enum
{ 
  BTLV_INPUT_TYPE_SINGLE = 0,
  BTLV_INPUT_TYPE_DOUBLE,
  BTLV_INPUT_TYPE_TRIPLE,
  BTLV_INPUT_TYPE_ROTATION,
}BTLV_INPUT_TYPE;

typedef enum
{ 
  BTLV_INPUT_SCRTYPE_STANDBY = 0,
  BTLV_INPUT_SCRTYPE_COMMAND,
  BTLV_INPUT_SCRTYPE_WAZA,
  BTLV_INPUT_SCRTYPE_DIR,
  BTLV_INPUT_SCRTYPE_YES_NO,
  BTLV_INPUT_SCRTYPE_ROTATE,
}BTLV_INPUT_SCRTYPE;

//SCRTYPE_COMMANDのstatusパラメータの意味
enum
{ 
  BTLV_INPUT_STATUS_NONE = 0, ///<ポケモンがいない
  BTLV_INPUT_STATUS_ALIVE,    ///<ポケモンがいる（いきてる）
  BTLV_INPUT_STATUS_DEAD,     ///<ポケモンがいる（気絶）
  BTLV_INPUT_STATUS_NG,       ///<ポケモンがいる（ステータス異常）
};

extern  BTLV_INPUT_WORK*  BTLV_INPUT_Init( BTLV_INPUT_TYPE type, GFL_FONT* font, HEAPID heapID );
extern	void			        BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw );
extern	void			        BTLV_INPUT_Main( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFrame( void );
extern  void              BTLV_INPUT_FreeFrame( void );
extern  void              BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type, void* param );
extern  int               BTLV_INPUT_CheckInput( BTLV_INPUT_WORK* biw, const GFL_UI_TP_HITTBL* tp_tbl );

