//======================================================================
/**
 * @file	camera_system.c
 * @brief	TWL�̃J�������C�u����
 * @author	ariizumi
 * @data	  09/05/18
 *
 * ���W���[�����FCAMERA_SYS
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "system/camera_system.h"

#ifdef SDK_TWL

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define NDMA_NO       1           // �g�p����NDMA�ԍ�(0-3)
#define WIDTH       256         // �C���[�W�̕�
#define HEIGHT      192         // �C���[�W�̍���

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _CAMERA_SYSTEM_WORK
{
  HEAPID heapId;
  void  **readBufferArr;  //�J��������ǂݍ��ރo�b�t�@
  u8    readBufferNum;
  BOOL  createBuffer;   //�V�X�e���Ńo�b�t�@����������H
  BOOL  isCapture;     //�L���v�`�������H
  BOOL  isTrimming;  //�g���~���O���Ă�H
  
  u8    readBufferNo;   //���݂̓ǂݍ��݃o�b�t�@�ԍ�
  
  BOOL startRequest;    //�L���v�`���J�n���N�G�X�g
  BOOL switchFlag;      //�J�����̓��O�؂�ւ��v��
  BOOL cancelCapture;   //��荞�݂𒆒f���� (�Ăѓ����o�b�t�@�Ɏ�荞��)
  u32  stabilizedCount; //�������҂��J�E���g
  
  //�J�����̌��ݐݒ�
  CAMERASelect currentCamera; //�J�����̓��O
  CAMERASize   currentSize;   //�J�����T�C�Y
  u16          trimLeft;
  u16          trimRight;
  u16          trimTop;
  u16          trimBottom;
  
  //�R�[���o�b�N
  CAMERA_SYS_CaptureCallBack captureCallBack;
  void  *userWork;
};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void CAMERA_SYS_VSyncCallBack(CAMERAResult result);
static void CAMERA_SYS_ErrorCallBack(CAMERAResult result);
static void CAMERA_SYS_RebootCallBack(CAMERAResult result);
static void CAMERA_SYS_DmaRecvCallBack( void* arg );

static void CAMERA_SYS_CallDmaRacv( CAMERA_SYSTEM_WORK *work );

static CAMERA_SYSTEM_WORK *cameraWork = NULL;

//--------------------------------------------------------------
//
//--------------------------------------------------------------

CAMERA_SYSTEM_WORK* CAMERA_SYS_InitSystem( HEAPID heapId )
{
  CAMERA_SYSTEM_WORK *work = GFL_HEAP_AllocMemory( heapId , sizeof(CAMERA_SYSTEM_WORK) );

  GF_ASSERT_MSG(cameraWork==NULL,"CameraSystem buffer is not NULL!!\n");
  
  cameraWork = work;
  work->heapId = heapId;
  work->readBufferArr = NULL;
  work->readBufferNo = 0;
  work->readBufferNum = 0;
  work->createBuffer = FALSE;
  work->isCapture = FALSE;
  work->isTrimming = FALSE;

  work->currentCamera = CAMERA_SELECT_IN;
  work->currentSize   = CAMERA_SIZE_DS_LCD;
  work->cancelCapture = FALSE;
  work->startRequest = FALSE;
  work->switchFlag = FALSE;
  work->cancelCapture = FALSE;
  
  work->captureCallBack = NULL;
  work->userWork = NULL;

  // �J����������
  {
    CAMERAResult result;
    result = CAMERA_Init();
    if(result == CAMERA_RESULT_FATAL_ERROR)
    {
        OS_TPanic("CAMERA_Init was failed.");
    }
    if(result == CAMERA_RESULT_ILLEGAL_STATUS)
    {
        return NULL;
    }

    result = CAMERA_I2CEffect(work->currentCamera, CAMERA_EFFECT_NONE);
    if (result != CAMERA_RESULT_SUCCESS)
    {
        OS_TPrintf("CAMERA_I2CEffect was failed. (%d)\n", result);
    }
    
    //�z���C�g�o�����X�ƘI�o�̎�����
    CAMERA_I2CAutoExposure( work->currentCamera, TRUE );
    CAMERA_I2CAutoWhiteBalance( work->currentCamera, TRUE );

    result = CAMERA_I2CActivate(work->currentCamera);
    if (result == CAMERA_RESULT_FATAL_ERROR)
    {
        OS_TPanic("CAMERA_I2CActivate was failed. (%d)\n", result);
    }
    if(result == CAMERA_RESULT_ILLEGAL_STATUS)
    {
        return NULL;
    }
    
    work->stabilizedCount = 0;
  }
  // DMA���荞�ݐݒ�
  (void)OS_EnableIrqMask(OS_IE_NDMA1);

  // �J����VSYNC���荞�݃R�[���o�b�N
  CAMERA_SetVsyncCallback(CAMERA_SYS_VSyncCallBack);

  // �J�����G���[���荞�݃R�[���o�b�N
  CAMERA_SetBufferErrorCallback(CAMERA_SYS_ErrorCallBack);

  // �J�����̍ċN�������R�[���o�b�N
  CAMERA_SetRebootCallback(CAMERA_SYS_RebootCallBack);

  {

    CAMERA_SYS_ResetTrimming( work );
    CAMERA_SetOutputFormat(CAMERA_OUTPUT_RGB);
  }
  
  return work;
}

void CAMERA_SYS_ExitSystem( CAMERA_SYSTEM_WORK* work )
{
  CAMERA_SYS_StopCapture( work );
  CAMERA_Stop();
  MI_StopNDma(NDMA_NO);
  CAMERA_End();
  
  CAMERA_SYS_DeleteReadBuffer( work );
  
  GFL_HEAP_FreeMemory( work );
  cameraWork = NULL;
}

static void CAMERA_SYS_CallDmaRacv( CAMERA_SYSTEM_WORK *work )
{
  const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
  const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
  CAMERA_DmaRecvAsync(NDMA_NO, 
                      work->readBufferArr[work->readBufferNo], 
                      CAMERA_GetBytesAtOnce(width), 
                      CAMERA_GET_FRAME_BYTES(width, height), 
                      CAMERA_SYS_DmaRecvCallBack, 
                      NULL);
  
}


//--------------------------------------------------------------------------------
//    �J�������荞�ݏ��� (�G���[����Vsync���̗����Ŕ���)
//
#define PRINT_RATE  32
static void CAMERA_SYS_ErrorCallBack(CAMERAResult result)
{
#pragma unused(result)
  CAMERA_SYSTEM_WORK *work = cameraWork;

  OS_TPrintf("Error was occurred[%d].\n",result);
  // �J������~����
  CAMERA_StopCapture();
  MI_StopNDma(NDMA_NO);
  CAMERA_ClearBuffer();
  work->cancelCapture = TRUE;   // ����������t���[�����g�p����
  work->startRequest = TRUE;    // �J�����ĊJ�v��
}

static void CAMERA_SYS_RebootCallBack(CAMERAResult result)
{
  CAMERA_SYSTEM_WORK *work = cameraWork;
  if(result == CAMERA_RESULT_FATAL_ERROR)
  {
    return; // �J�������ċN���������s���Ă����A���邱�Ƃ��ł��Ȃ�����
  }
  CAMERA_SYS_ErrorCallBack(result); // DMA�̓���������Ă��邩������Ȃ��̂Ŏd�؂蒼��
}

static void CAMERA_SYS_VSyncCallBack(CAMERAResult result)
{
#pragma unused(result)
  CAMERA_SYSTEM_WORK *work = cameraWork;
  if( work->isCapture == FALSE )
  {
    return;
  }
  //�������҂�
  if(work->stabilizedCount <= 30)
  {
      work->stabilizedCount++;
  }
  if(work->switchFlag)
  {
    CAMERAResult ret;
    ret = CAMERA_I2CActivate(work->currentCamera);
    if(ret == CAMERA_RESULT_FATAL_ERROR)
    {
      OS_Panic("CAMERA FATAL ERROR\n");
    }
    work->stabilizedCount = 0;
    work->switchFlag = FALSE;
  }
  if (work->startRequest)
  {
    CAMERA_SYS_CallDmaRacv( work );
    CAMERA_ClearBuffer();
    CAMERA_StartCapture(); // �J����VSYNC���ɌĂяo���Ă���̂ŁA������ CAMERA_IsBusy() �� TRUE ��Ԃ��悤�ɂȂ�
    work->startRequest = FALSE;
  }
}

#define CAM_SYS_DRAW_TICK (0)

static void CAMERA_SYS_DmaRecvCallBack( void* arg )
{
#pragma unused(arg)
  CAMERA_SYSTEM_WORK *work = cameraWork;
  MI_StopNDma(NDMA_NO);

  if (CAMERA_IsBusy() == TRUE)
  {
    void *buffer;
#if CAM_SYS_DRAW_TICK
    OS_TPrintf(".");
#endif //CAM_SYS_DRAW_TICK
    if (MI_IsNDmaBusy(NDMA_NO)) // �摜�̓]�����I����Ă��邩�`�F�b�N
    {
      OS_TPrintf("DMA was not done until VBlank.\n");
      MI_StopNDma(NDMA_NO);
    }
    // ���̃t���[���̃L���v�`�����J�n����
    if ( work->cancelCapture )
    {
      work->cancelCapture = FALSE;
    }
    else
    {
      // �J���������肷��܂ŁA�L���v�`�����ʂ͉�ʂɕ\�����Ȃ�
      // ���̃f���łُ͈�ȐF��������邽�߂ɍŒ���҂K�v�̂��� 4 �J�����t���[���҂��Ă��邪�A
      // �����I�o�����肷��܂ő҂������ꍇ�́A���t�@�����X�ɂ���悤�ɉ����� 14�A���O�� 30 �t���[���҂K�v������܂��B
      if(work->stabilizedCount > 4)
      {
        if( work->captureCallBack != NULL )
        {
          work->captureCallBack( work->readBufferArr[work->readBufferNo] , work->userWork );
        }

        work->readBufferNo = (work->readBufferNo+1)%work->readBufferNum;
      }
    }
    if( work->isCapture == TRUE )
    {
      CAMERA_SYS_CallDmaRacv( work );
    }
  }

#if CAM_SYS_DRAW_TICK
  {
    static OSTick begin = 0;
    static int count = 0;
    if (begin == 0)
    {
      begin = OS_GetTick();
    }
    else if (++count == PRINT_RATE)
    {
      OSTick uspf = OS_TicksToMicroSeconds(OS_GetTick() - begin) / count;
      int mfps = (int)(1000000000LL / uspf);
      OS_TPrintf("%2d.%03d fps\n", mfps / 1000, mfps % 1000);
      count = 0;
      begin = OS_GetTick();
    }
  }
#endif //CAM_SYS_DRAW_TICK
}

void CAMERA_SYS_SetCaptureCallBack( CAMERA_SYSTEM_WORK* work , CAMERA_SYS_CaptureCallBack callBack , void* userWork )
{
  work->captureCallBack = callBack;
  work->userWork = userWork;
}


void CAMERA_SYS_CreateReadBuffer( CAMERA_SYSTEM_WORK* work , const int bufferNum , const HEAPID heapId )
{
  u8 i;
  const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
  const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
  
  if( work->readBufferArr == NULL )
  {
    work->readBufferArr = GFL_HEAP_AllocMemory( work->heapId , sizeof(GFL_BMP_DATA*)*bufferNum );
    
    work->readBufferNum = bufferNum;
    for( i=0;i<bufferNum;i++ )
    {
      work->readBufferArr[i] = GFL_NET_Align32Alloc( heapId , sizeof(u16)*width*height );
    }
    work->createBuffer = TRUE;
    OS_TPrintf("CameraSystem create buffer size[%x]+workArea \n",(sizeof(u16)*WIDTH*HEIGHT)*bufferNum );
  }
  else
  {
    OS_TPrintf("CameraSystem Rend buffer is created yet!!\n");
  }

}

const u32 CAMERA_SYS_GetBufferSize( CAMERA_SYSTEM_WORK* work , const int bufferNum )
{
  const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
  const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
  
  return sizeof(u16)*width*height*bufferNum;
}

void CAMERA_SYS_SetReadBuffer( CAMERA_SYSTEM_WORK* work , void *topAdr , const int bufferNum )
{
  u8 i;
  const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
  const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
  
  if( work->readBufferArr == NULL )
  {
    work->readBufferArr = GFL_HEAP_AllocMemory( work->heapId , sizeof(GFL_BMP_DATA*)*bufferNum );
    
    work->readBufferNum = bufferNum;
    for( i=0;i<bufferNum;i++ )
    {
      work->readBufferArr[i] = (void*)((u32)topAdr + (i*width*height*sizeof(u16)));
    }
    work->createBuffer = FALSE;
    OS_TPrintf("CameraSystem create buffer size[%x]+workArea \n",(sizeof(u16)*WIDTH*HEIGHT)*bufferNum );
  }
  else
  {
    OS_TPrintf("CameraSystem Rend buffer is created yet!!\n");
  }
}

void CAMERA_SYS_DeleteReadBuffer( CAMERA_SYSTEM_WORK* work )
{
  if( work->isCapture == FALSE )
  {
    if( CAMERA_IsBusy() == FALSE )
    {
      if( work->createBuffer == TRUE )
      {
        u8 i;
        for( i=0;i<work->readBufferNum;i++ )
        {
          GFL_NET_Align32Free( work->readBufferArr[i] );
          
        }
      }
      GFL_HEAP_FreeMemory( work->readBufferArr );
      
      work->readBufferNum = 0;
      work->readBufferArr = NULL;
    }
    else
    {
      OS_TPrintf("CameraSystem Camera is busy!!\n");
    }
  }
  else
  {
    OS_TPrintf("CameraSystem Capture now!!\n");
  }
}


void CAMERA_SYS_StartCapture( CAMERA_SYSTEM_WORK* work )
{
  if( work->isCapture == FALSE )
  {
    if( CAMERA_IsBusy() == FALSE )
    {
      //����DMA���������ςȂ���������~�߂�
      if( MI_IsNDmaBusy( NDMA_NO ) == TRUE )
      {
        MI_StopNDma(NDMA_NO);
      }
      work->isCapture = TRUE;
      work->startRequest = TRUE;
      work->stabilizedCount = 0;
      OS_TPrintf("CameraSystem Start Capture\n");
    }
    else
    {
      OS_TPrintf("CameraSystem Camera is busy now!!\n");
    }
  }
  else
  {
    OS_TPrintf("CameraSystem Capture is start yet!\n");
  }

}

void CAMERA_SYS_StopCapture( CAMERA_SYSTEM_WORK* work )
{
  if( work->isCapture == TRUE )
  {
    work->isCapture = FALSE;
    work->startRequest = FALSE;
    CAMERA_StopCapture();
    OS_TPrintf("CameraSystem Stop Capture.\n");
    //������DMA���~�߂Ă͂����Ȃ�
  }
  else
  {
    OS_TPrintf("CameraSystem Capture is stop yet!\n");
  }
}

const BOOL CAMERA_SYS_IsCapture( CAMERA_SYSTEM_WORK* work )
{
  if( work->isCapture == TRUE || 
      CAMERA_IsBusy() == TRUE )
  {
    return TRUE;
  }
  return FALSE;
}


//�g���~���O
void CAMERA_SYS_SetTrimming( CAMERA_SYSTEM_WORK* work , const u16 left , const u16 top , const u16 right , const u16 bottom )
{
  const u16 width = CAMERA_SYS_ResolutionSizeToWidth( work );
  const u16 height= CAMERA_SYS_ResolutionSizeToHeight( work );
  GF_ASSERT_MSG( left < right,"CameraSystem TrimmingSize Error! Left[%d] > right[%d]\n",left , right );
  GF_ASSERT_MSG( top < bottom,"CameraSystem TrimmingSize Error! top[%d] > bottom[%d]\n",top , bottom );
  GF_ASSERT_MSG( (top%2) == 0,"CameraSystem TrimmingSize Error! top[%d] %2 == 1\n",top );
  GF_ASSERT_MSG( (bottom%2) == 0,"CameraSystem TrimmingSize Error! bottom[%d] %2 == 1\n",bottom );
  GF_ASSERT_MSG( left   <= width ,"CameraSystem TrimmingSize Error! left[%d] over width[%d]\n",left,width );
  GF_ASSERT_MSG( bottom <= height,"CameraSystem TrimmingSize Error! bottom[%d] over height[%d]\n",bottom,height );
  GF_ASSERT_MSG( work->isCapture == FALSE,"CameraSystem Capture is start!!\n" );

  if( work->isCapture == FALSE )
  {
    if( CAMERA_IsBusy() == FALSE )
    {
      CAMERA_SetTrimmingParams( left , top , right , bottom );
      CAMERA_SetTrimming( TRUE );
      
      work->trimLeft    = left;
      work->trimRight   = right;
      work->trimTop     = top;
      work->trimBottom  = bottom;
      {
        const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
        const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
        CAMERA_SetTransferLines(CAMERA_GetMaxLinesRound(width,height));
      }
      work->isTrimming = TRUE;

    }
    else
    {
      OS_TPrintf("CameraSystem Camera is busy now!!\n");
    }
  }
}

const BOOL CAMERA_SYS_GetIsTrimming( CAMERA_SYSTEM_WORK* work )
{
  return work->isTrimming;
}

void CAMERA_SYS_ResetTrimming( CAMERA_SYSTEM_WORK* work )
{
  GF_ASSERT_MSG( work->isCapture == FALSE,"CameraSystem Capture is start!!\n" );

  if( work->isCapture == FALSE )
  {
    if( CAMERA_IsBusy() == FALSE )
    {
      CAMERA_SetTrimming( FALSE );
      work->trimLeft    = 0;
      work->trimRight   = CAMERA_SYS_ResolutionSizeToWidth(work);
      work->trimTop     = 0;
      work->trimBottom  = CAMERA_SYS_ResolutionSizeToHeight(work);
      {
        const u16 width = CAMERA_SYS_CaptureSizeToWidth( work );
        const u16 height= CAMERA_SYS_CaptureSizeToHeight( work );
        CAMERA_SetTransferLines(CAMERA_GetMaxLinesRound(width,height));
      }
      work->isTrimming = FALSE;
    }
    else
    {
      OS_TPrintf("CameraSystem Camera is busy now!!\n");
    }
  }
}

//�擾�n
//���ۂɃL���v�`������Ă���T�C�Y
const u16 CAMERA_SYS_CaptureSizeToWidth( CAMERA_SYSTEM_WORK* work )
{
  return  work->trimRight-work->trimLeft;
}

const u16 CAMERA_SYS_CaptureSizeToHeight( CAMERA_SYSTEM_WORK* work )
{
  return  work->trimBottom-work->trimTop;
}

//�J�����̉𑜓x�T�C�Y
void CAMERA_SYS_SetResolution( CAMERA_SYSTEM_WORK* work , const CAMERASize size )
{
  CAMERA_I2CSize( CAMERA_SELECT_BOTH , size );
  work->currentSize = size;
  CAMERA_SYS_ResetTrimming(work);
  
}

const CAMERASize CAMERA_SYS_GetResolution( CAMERA_SYSTEM_WORK* work )
{
  return work->currentSize;
}

const u16 CAMERA_SYS_ResolutionSizeToWidth( CAMERA_SYSTEM_WORK* work )
{
  return CAMERA_SizeToWidth( work->currentSize );
}

const u16 CAMERA_SYS_ResolutionSizeToHeight( CAMERA_SYSTEM_WORK* work )
{
  return CAMERA_SizeToHeight( work->currentSize );
}

//�J�����̓��O�̐؂�ւ�
const CAMERASelect CAMERA_SYS_GetCameraPos( CAMERA_SYSTEM_WORK* work )
{
  return work->currentCamera;
}

void CAMERA_SYS_SetCameraPos( CAMERA_SYSTEM_WORK* work , const CAMERASelect pos )
{
  if( work->currentCamera != pos &&
      work->switchFlag == FALSE )
  {
    work->currentCamera = pos;
    work->switchFlag = TRUE;
  }
}

void CAMERA_SYS_SwapCameraPos( CAMERA_SYSTEM_WORK* work )
{
  if( work->switchFlag == FALSE )
  {
    work->currentCamera = (work->currentCamera == CAMERA_SELECT_IN ? CAMERA_SELECT_OUT : CAMERA_SELECT_IN);
    work->switchFlag = TRUE;
  }

}

#else
struct _CAMERA_SYSTEM_WORK
{
  HEAPID heapId;
};
CAMERA_SYSTEM_WORK* CAMERA_SYS_InitSystem( HEAPID heapId ){return NULL;}
void CAMERA_SYS_ExitSystem( CAMERA_SYSTEM_WORK* work ){}

void CAMERA_SYS_CreateReadBuffer( CAMERA_SYSTEM_WORK* work , const int bufferNum , const HEAPID heapId ){}
const u32 CAMERA_SYS_GetBufferSize( CAMERA_SYSTEM_WORK* work , const int bufferNum ){return 0;}
void CAMERA_SYS_SetReadBuffer( CAMERA_SYSTEM_WORK* work , void *topAdr , const int bufferNum ){}

void CAMERA_SYS_DeleteReadBuffer( CAMERA_SYSTEM_WORK* work ){}
void CAMERA_SYS_StartCapture( CAMERA_SYSTEM_WORK* work ){}
void CAMERA_SYS_StopCapture( CAMERA_SYSTEM_WORK* work ){}
const BOOL CAMERA_SYS_IsCapture( CAMERA_SYSTEM_WORK* work ){return FALSE;}
void CAMERA_SYS_SetCaptureCallBack( CAMERA_SYSTEM_WORK* work , 
                                    CAMERA_SYS_CaptureCallBack callBack , 
                                    void* userWork ){}
void CAMERA_SYS_SetTrimming( CAMERA_SYSTEM_WORK* work , const u16 left , const u16 top , const u16 right , const u16 bottom ){}
const BOOL CAMERA_SYS_GetIsTrimming( CAMERA_SYSTEM_WORK* work ){return FALSE;}
void CAMERA_SYS_ResetTrimming( CAMERA_SYSTEM_WORK* work ){}
const u16 CAMERA_SYS_CaptureSizeToWidth( CAMERA_SYSTEM_WORK* work ){return 0;}
const u16 CAMERA_SYS_CaptureSizeToHeight( CAMERA_SYSTEM_WORK* work ){return 0;}
void CAMERA_SYS_SetResolution( CAMERA_SYSTEM_WORK* work , const CAMERASize size ){}
const CAMERASize CAMERA_SYS_GetResolution( CAMERA_SYSTEM_WORK* work ){return 0;}
const u16 CAMERA_SYS_ResolutionSizeToWidth( CAMERA_SYSTEM_WORK* work ){return 0;}
const u16 CAMERA_SYS_ResolutionSizeToHeight( CAMERA_SYSTEM_WORK* work ){return 0;}
const CAMERASelect CAMERA_SYS_GetCameraPos( CAMERA_SYSTEM_WORK* work ){return 0;}
void CAMERA_SYS_SetCameraPos( CAMERA_SYSTEM_WORK* work , const CAMERASelect pos ){}
void CAMERA_SYS_SwapCameraPos( CAMERA_SYSTEM_WORK* work ){}

#endif //def SDK_TWL
