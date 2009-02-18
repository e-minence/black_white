
//============================================================================================
/**
 * @file	waza_tool.h
 * @bfief	�Z�p�����[�^�c�[���S
 * @author	HisashiSogabe
 * @date	05.07.28
 */
//============================================================================================

#ifndef	__WAZA_TOOL_H_
#define	__WAZA_TOOL_H_

//WT_WazaDataParaGet�Ŏg�p����ID
#define	ID_WTD_battleeffect		(0)
#define	ID_WTD_kind				(1)
#define	ID_WTD_damage			(2)
#define	ID_WTD_wazatype			(3)
#define	ID_WTD_hitprobability	(4)
#define	ID_WTD_pp				(5)
#define	ID_WTD_addeffect		(6)
#define	ID_WTD_attackrange		(7)
#define	ID_WTD_attackpri		(8)
#define	ID_WTD_flag				(9)
#define	ID_WTD_ap_no			(10)			//�R���e�X�g�p�p�����[�^�@�A�s�[��No
#define	ID_WTD_contype			(11)			//�R���e�X�g�p�p�����[�^�@�R���e�X�g�Z�^�C�v
#define	ID_WTD_end				(12)

// 
#define	WDT_FLAG_DAGEKI			0x01
#define	WDT_FLAG_MAMORU			0x02
#define	WDT_FLAG_MAGICCORT		0x04
#define	WDT_FLAG_YOKODORI		0x08
#define	WDT_FLAG_OUMUGAESI		0x10
#define	WDT_FLAG_OUZYANOSIRUSI	0x20
#define	WDT_FLAG_PUT_GAUGE		0x40
#define	WDT_FLAG_DEL_SHADOW		0x80

//�A�Z���u����include����Ă���ꍇ�́A���̐錾�𖳎��ł���悤��ifndef�ň͂�ł���
#ifndef	__ASM_NO_DEF_

#include "arc_tool.h"

///<�Z�p�����[�^�̍\����
typedef struct waza_table_data WAZA_TABLE_DATA;

extern	void		WT_WazaTableDataGet( void *buffer );
extern	ARCHANDLE	*WT_WazaDataParaHandleOpen( HEAPID heapID );
extern	void		WT_WazaDataParaHandleClose( ARCHANDLE *handle );
extern	u32			WT_WazaDataParaGet( int waza_no, int id );
extern	u32			WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id );
extern	u8			WT_PPMaxGet( u16 wazano, u8 maxupcnt );
extern	u32			WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id );

#endif	__ASM_NO_DEF_

#endif	__WAZA_TOOL_H_

