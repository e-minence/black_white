//======================================================================
/**
 * @file	���o�[
 * @brief	��ʏ�ɏ풓������\�����
 * @author	ariizumi
 * @data	09/02/23
 */
//======================================================================

#ifndef INFOWIN_H__
#define INFOWIN_H__

//======================================================================
//	define
//======================================================================

//NCG��ǂݍ���VRAM�T�C�Y
#define INFOWIN_CHARAREA_SIZE (0x1000)

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//������
//	@param bgplane	BG��
//	@param pltNo	�p���b�g�ԍ�
//	@param heapId	�q�[�vID
//	Vram��Char�G���A�� INFOWIN_CHARAREA_SIZE �̋󂫂��K�v�ł�
void	INFOWIN_Init( const u8 bgplane , const u8 pltNo, const HEAPID heapId );
void	INFOWIN_Update( void );
void	INFOWIN_Exit( void );

void	INFOWIN_InitComm( const HEAPID heapId );
const BOOL	INFOWIN_IsInitComm( void );
void	INFOWIN_ExitComm( void );

#endif //INFOWIN_H__
