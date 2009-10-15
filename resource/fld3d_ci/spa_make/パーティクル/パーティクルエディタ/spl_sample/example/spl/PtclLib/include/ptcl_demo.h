// �p�[�e�B�N���T���v���v���O�����p�̃��[�e�B���e�B

#ifndef PTCL_DEMO_H_
#define PTCL_DEMO_H_

#include<nnsys.h>

// �V�X�e���֌W
extern void PtclDemo_InitSystem(void) ;
extern void PtclDemo_InitVRAM(void) ;
extern void PtclDemo_InitMemory(void) ;
extern void PtclDemo_InitScreen(void) ;

// �O���t�B�b�N�֌W
extern void PtclDemo_DrawAxis(void) ;
extern void PtclDemo_Lookat( fx32 fromX, fx32 fromY, fx32 fromZ, fx32 atX, fx32 atY, fx32 atZ, fx32 upX, fx32 upY, fx32 upZ, MtxFx43 * mtx ) ;
extern void* PtclDemo_LoadFile(const char *path) ;

extern void* PtclDemo_LoadNCLR(  NNSG2dPaletteData** ppPltData, const char* pFname ) ;
extern void* PtclDemo_LoadNCGR(  NNSG2dCharacterData** ppCharData, const char* pFname ) ;
extern void* PtclDemo_LoadNCER(  NNSG2dCellDataBank** ppCellBank, const char* pFname ) ;
extern void* PtclDemo_LoadNSCR(  NNSG2dScreenData** ppScrData, const char* pFname ) ;

// �L�[���͊֌W
BOOL PtclDemo_Key( int key_ ) ;
BOOL PtclDemo_KeyOneshot( int key_ ) ;
void PtclDemo_UpdateInput(void) ;


#endif

// end of file