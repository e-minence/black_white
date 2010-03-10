//======================================================================
/**
 * @file	draw_system.c
 * @brief	��ʂ��G�`���V�X�e��
 * @author	ariizumi
 * @data	09/12/20
 *
 * ���W���[�����FDRAW_SYS
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "draw_system.h"
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
struct _DRAW_SYS_WORK
{
  u8 frame;
  
  u16 bufNum;
  u16 topBuffer;  //�Z�b�g����Ă���ʒu
  u16 nowBuffer;  //�`�悳��Ă���ʒu
  
  u16 areaLeft;
  u16 areaRight;
  u16 areaTop;
  u16 areaBottom;

  DRAW_SYS_PEN_INFO *infoBuf;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void DRAW_SYS_DrawInfo( DRAW_SYS_WORK* work , const DRAW_SYS_PEN_INFO* info );
static void DRAW_SYS_DrawLine( DRAW_SYS_WORK* work , void *vramAdr , const u16 x1 , const u16 x2 , const u16 y1 , const u16 y2 , const DRAW_SYS_PEN_SIZE penType , const u16 col );
static void DRAW_SYS_DrawDot_DirectBmp( DRAW_SYS_WORK* work , void *vramAdr , const u16 x , const u16 y , const u16 col );

static void* DRAW_SYS_GetVramAdr( DRAW_SYS_WORK* work );
static DRAW_SYS_PEN_INFO* DRAW_SYS_GetInfoBuf( DRAW_SYS_WORK* work , const u16 pos );
static void DRAW_SYS_IncBufCnt( DRAW_SYS_WORK* work , u16 *pos );

//--------------------------------------------------------------
//	�V�X�e���쐬
//--------------------------------------------------------------
DRAW_SYS_WORK* DRAW_SYS_CreateSystem( const DRAW_SYS_INIT_WORK *initWork )
{
  DRAW_SYS_WORK* work = GFL_HEAP_AllocClearMemory( initWork->heapId , sizeof(DRAW_SYS_WORK) );
  work->infoBuf = GFL_NET_Align32Alloc( initWork->heapId , sizeof(DRAW_SYS_PEN_INFO)*initWork->bufferNum );

  work->frame = initWork->frame;
  
  work->bufNum     = initWork->bufferNum;
  work->areaLeft   = initWork->areaLeft;
  work->areaRight  = initWork->areaRight;
  work->areaTop    = initWork->areaTop;
  work->areaBottom = initWork->areaBottom;

  work->topBuffer = 0;
  work->nowBuffer = 0;
  return work;
}

//--------------------------------------------------------------
//	�V�X�e���J��
//--------------------------------------------------------------
void DRAW_SYS_DeleteSystem( DRAW_SYS_WORK* work )
{
  GFL_NET_Align32Free( work->infoBuf );
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//	�V�X�e���X�V
//--------------------------------------------------------------
void DRAW_SYS_UpdateSystem( DRAW_SYS_WORK* work )
{
}

//--------------------------------------------------------------
//	�V�X�e���X�V(VBlank
//--------------------------------------------------------------
void DRAW_SYS_UpdateSystemVBlank( DRAW_SYS_WORK* work )
{
  while( work->nowBuffer != work->topBuffer )
  {
    DRAW_SYS_PEN_INFO *drawBuf = DRAW_SYS_GetInfoBuf(work,work->nowBuffer);
    
    DRAW_SYS_DrawInfo( work , drawBuf );
    
    DRAW_SYS_IncBufCnt( work , &work->nowBuffer );
  }
}

//--------------------------------------------------------------
//	�y����񂩂�`��
//--------------------------------------------------------------
#include "draw_system_pen.cdat"
static void DRAW_SYS_DrawInfo( DRAW_SYS_WORK* work , const DRAW_SYS_PEN_INFO* info )
{
  void *vramAdr = DRAW_SYS_GetVramAdr( work );
  DRAW_SYS_DrawLine( work , vramAdr , info->startX , info->endX , info->startY , info->endY , info->penType , info->col );
}

static void DRAW_SYS_DrawLine( DRAW_SYS_WORK* work , void *vramAdr , const u16 x1 , const u16 x2 , const u16 y1 , const u16 y2 , const DRAW_SYS_PEN_SIZE penType , const u16 col )
{
  const DRAW_SYS_PEN_DATA *penData = &DRAW_SYS_PEN_ARR[penType];

  const int xLen = MATH_ABS( x1-x2 );
  const int yLen = MATH_ABS( y1-y2 );
  const int loopNum = ( xLen > yLen ? xLen : yLen )+1;
  fx32 addX = FX32_CONST(x2-x1) / loopNum;
  fx32 addY = FX32_CONST(y2-y1) / loopNum;
  fx32 subX = 0;
  fx32 subY = 0;
  
  const int baseX = x1 - (penData->sizeX/2);
  const int baseY = y1 - (penData->sizeY/2);
  
  u8 i;
  u8 drawBit = 31;
  const u8 bitX = ( addX >= 0 ? DSPD_BIT_RIGHT : DSPD_BIT_LEFT );
  const u8 bitY = ( addY >= 0 ? DSPD_BIT_DOWN : DSPD_BIT_UP );
  int befX = baseX;
  int befY = baseY;
  
  for( i=0;i<loopNum;i++ )
  {
    u8 x,y;
    const int posX = baseX + (subX>>FX32_SHIFT);
    const int posY = baseY + (subY>>FX32_SHIFT);
    if( i > 0 )
    {
      drawBit = 0;
      if( posX != befX )
      {
        drawBit += bitX;
      }
      if( posY != befY )
      {
        drawBit += bitY;
      }
    }
    else
    {
      drawBit = 31;
    }
    /*
    OS_TFPrintf(3,"---------------------------------------------[%d]\n",drawBit);
    for( y=0;y<penData->sizeY;y++ )
    {
      for( x=0;x<penData->sizeX;x++ )
      {
        if( penData->penData[y][x] & drawBit )
        {
          OS_TFPrintf(3,"o");
        }
        else
        {
          OS_TFPrintf(3,"x");
        }
      }
      OS_TFPrintf(3,"\n");
    }
    OS_TFPrintf(3,"---------------------------------------------\n");
    //*/
    for( y=0;y<penData->sizeY;y++ )
    {
      for( x=0;x<penData->sizeX;x++ )
      {
        if( penData->penData[y][x] & drawBit )
        {
          DRAW_SYS_DrawDot_DirectBmp( work , vramAdr , posX+x , posY+y , col );
        }
      }
    }
    subX += addX;
    subY += addY;
    
    befX = posX;
    befY = posY;
  }
  
}

static void DRAW_SYS_DrawDot_DirectBmp( DRAW_SYS_WORK* work , void *vramAdr , const u16 x , const u16 y , const u16 col )
{
  if( x >= work->areaLeft &&
      x <  work->areaRight &&
      y >= work->areaTop &&
      y <  work->areaBottom )
  {
    u16 *trgAdr = (u16*)((u32)vramAdr + x*2 + y*256*2);
    *trgAdr = col;
  }
}

//--------------------------------------------------------------
//	�y�����o�^
//--------------------------------------------------------------
void DRAW_SYS_SetPenInfo( DRAW_SYS_WORK* work , const DRAW_SYS_PEN_INFO *info )
{
  DRAW_SYS_PEN_INFO *setPos = DRAW_SYS_GetInfoBuf(work,work->topBuffer);

  GFL_STD_MemCopy( info , setPos , sizeof(DRAW_SYS_PEN_INFO) );
  
  //OS_TPrintf("[%d][%d][%d][%d][%x]\n",setPos->startX,setPos->startY,setPos->endX,setPos->endX,setPos->col);
  //OS_TFPrintf(3,"[%d][%d][%d][%d][%x]\n",info->startX,info->startY,info->endX,info->endX,info->col);
  DRAW_SYS_IncBufCnt( work , &work->topBuffer );

}

//--------------------------------------------------------------
//	�o�b�t�@�A�h���X�擾
//--------------------------------------------------------------
void* DRAW_SYS_GetBufferAddress( DRAW_SYS_WORK* work )
{
  return work->infoBuf;
}
//--------------------------------------------------------------
//	�o�b�t�@�ʒu�擾
//--------------------------------------------------------------
u16 DRAW_SYS_GetBufferTopPos( DRAW_SYS_WORK* work )
{
  return work->topBuffer;
}

//--------------------------------------------------------------
//	�o�b�t�@�ĕ`��
//  ��{�I�ɏ�ł��炤�l������
//--------------------------------------------------------------
void DRAW_SYS_SetRedrawBuffer( DRAW_SYS_WORK* work , u16 finishBufNo )
{
  work->topBuffer = finishBufNo;
  work->nowBuffer = finishBufNo+5;  //�ꉞ�]�T
  if( work->nowBuffer >= work->bufNum )
  {
    work->nowBuffer -= work->bufNum;
  }
}

//--------------------------------------------------------------
//	VRAM�擾
//--------------------------------------------------------------
static void* DRAW_SYS_GetVramAdr( DRAW_SYS_WORK* work )
{
  if( work->frame == GFL_BG_FRAME2_M )
  {
    return G2_GetBG2ScrPtr();
  }
  else
  {
    GF_ASSERT_MSG(0,"���Ή��I\n");
    return G2_GetBG2ScrPtr();
  }
}

//--------------------------------------------------------------
//	�o�b�t�@�ʒu�擾
//--------------------------------------------------------------
static DRAW_SYS_PEN_INFO* DRAW_SYS_GetInfoBuf( DRAW_SYS_WORK* work , const u16 pos )
{
  return (DRAW_SYS_PEN_INFO*)((u32)work->infoBuf + pos*sizeof(DRAW_SYS_PEN_INFO) );
}

//--------------------------------------------------------------
//	�o�b�t�@�ʒu��i�߂�
//--------------------------------------------------------------
static void DRAW_SYS_IncBufCnt( DRAW_SYS_WORK* work , u16 *pos )
{
  *pos += 1;
  if( *pos >= work->bufNum )
  {
    *pos = 0;
  }
  
}

