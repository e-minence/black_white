//=============================================================================================
/**
 * @file  printsys.c
 * @brief Bitmap���p�����`��@�V�X�e������
 * @author  taya
 *
 * @date  2007.02.06  �쐬
 * @date  2008.09.11  �ʐM�Ή��L���[�\�����쐬
 */
//=============================================================================================
#include <gflib.h>
#include <tcbl.h>

#include "gamesystem/msgspeed.h"
#include "print/printsys.h"


//==============================================================
// Consts
//==============================================================
enum {
  LETTER_CHAR_WIDTH = 2,      ///< �P���������艡�L������
  LETTER_CHAR_HEIGHT = 2,     ///< �P����������c�L������
  LINE_DOT_HEIGHT = 16,       ///< �P�s������̃h�b�g��
  LINE_FEED_SPEED = 4,        ///< ���s���̍s���葬�x�idot/frame�j

  // ���s�R�[�h�A�I�[�R�[�h
  EOM_CODE      = 0xffff,
  CR_CODE       = 0xfffe,

  // �V�X�e���R���g���[��
  CTRL_SYSTEM_COLOR     = (0x0000),

  // �^�O�J�n�R�[�h
  SPCODE_TAG_START_ = 0xf000,   ///<
  TAGTYPE_WORD = 1,             ///< ������}��
  TAGTYPE_NUMBER = 2,           ///< ���l�}��
  TAGTYPE_GENERAL_CTRL = 0xbd,  ///< �ėp�R���g���[������
  TAGTYPE_STREAM_CTRL = 0xbe,   ///< ����郁�b�Z�[�W�p�R���g���[������
  TAGTYPE_SYSTEM = 0xff,        ///< �G�f�B�^�V�X�e���^�O�p�R���g���[������


  // �v�����g�L���[�֘A�萔
  QUE_DEFAULT_BUFSIZE = 1024,
  QUE_DEFAULT_TICK = 4400,    ///< �ʐM���A�P�x�̕`�揈���Ɏg�����Ԃ̖ڈ��iTick�j�f�t�H���g�l

  //
  JOB_COLORSTATE_IGNORE = 0,
  JOB_COLORSTATE_CHANGE_REQ,
  JOB_COLORSTATE_CHANGE_DONE,

};
/**
 *  ���x�R���g���[���^�O�p�����[�^
 */
enum {
  SPEED_CTRL_RESET = 0,
  SPEED_CTRL_FAST,
  SPEED_CTRL_SLOW,
};


//==============================================================
// Typedefs
//==============================================================

// �P�����������݊֐��t�H�[�}�b�g
typedef void (*pPut1CharFunc)( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, GFL_FONT* fontHandle, STRCODE charCode, GFL_FONT_SIZE* size );

//--------------------------------------------------------------------------
/**
 *  Print Job
 *
 *  Bitmap�ւ̕`�揈�����[�N
 */
//--------------------------------------------------------------------------
typedef struct {

  s16   write_x;
  s16   write_y;

  s32   org_x      : 10;
  s32   org_y      : 10;
  s32   colorState : 4;
  s32   pad1       : 18;

  PRINTSYS_LSB    defColor;
  PRINTSYS_LSB    jobColor;

  GFL_FONT*       fontHandle;   ///< �t�H���g�n���h��
  GFL_BMP_DATA*   dst;          ///< �������ݐ� Bitmap
  pPut1CharFunc   put1charFunc; ///< �P�����������݊֐�

}PRINT_JOB;

//--------------------------------------------------------------------------
/**
 *  Print Queue
 *
 *  �ʐM���A�`�揈���������݂Ɏ��s�ł���悤�A�������e��~�ς��邽�߂̋@�\�B
 */
//--------------------------------------------------------------------------
struct _PRINT_QUE {

  u16   bufTopPos;
  u16   bufEndPos;
  u16   bufSize;
  u8    debugCounter;
  u8    fColorChanged  : 4;
  u8    fForceCommMode : 4;   ///< ��ʐM���ɂ��ʐM���Ɠ�������������i�f�o�b�O�p�j

  PRINTSYS_LSB  defColor;

  OSTick        limitPerFrame;

  PRINT_JOB*    runningJob;
  const STRCODE*  sp;

  u8   buf[0];
};

//--------------------------------------------------------------------------
/**
 *  Print Stream �̕������葬�x�p�����[�^
 */
//--------------------------------------------------------------------------
typedef struct {
  u8  wait;           ///< �`�悲�ƂɊJ����Ԋu�t���[����
  u8  putPerFrame;    ///< �P�x�ɕ`�悷�镶����
}STREAM_SPEED_PARAM;

//--------------------------------------------------------------------------
/**
 *  Print Stream
 *
 *  ��b�E�B���h�E���A�P��Ԋu�łP�������`��C�\�����s�����߂̋@�\�B
 */
//--------------------------------------------------------------------------
struct _PRINT_STREAM {

  PRINTSTREAM_STATE   state;
  PRINTSTREAM_PAUSE_TYPE  pauseType;

  GFL_BMPWIN*     dstWin;
  GFL_BMP_DATA*   dstBmp;
  GFL_TCBL*     tcb;

  const STRCODE* sp;

  u8    org_wait;
  u8    current_wait;
  u8    org_putPerFrame;
  u8    current_putPerFrame;

  u8    wait;
  u8    pauseReleaseFlag;
  u8    pauseWait;
  u8    clearColor;

  u8    stopFlag;
  u8    callbackResult;
  u8    lineMoningFlag;

  pPrintCallBack  callback_func;
  u32       org_arg;
  u32       current_arg;
  u32       arg;

  PRINT_JOB printJob;
};


//==============================================================================================
//--------------------------------------------------------------------------
/**
 *  SystemWork
 */
//--------------------------------------------------------------------------
static struct {
  GFL_BMP_DATA*   charBuffer;
  PRINT_JOB     printJob;
}SystemWork;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline BOOL IsNetConnecting( void );
static void PrintQue_Core( PRINT_QUE* que, PRINT_JOB* job, const STRBUF* str );
static void printJob_setup( PRINT_JOB* wk, GFL_FONT* font, GFL_BMP_DATA* dst, s16 org_x, s16 org_y );
static void printJob_setColor( PRINT_JOB* wk, PRINTSYS_LSB color );
static void printJob_finish( PRINT_JOB* job, const STRBUF* str );
static const STRCODE* print_next_char( PRINT_JOB* wk, const STRCODE* sp, BOOL fSkipStreamTag );
static void put1char_normal( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, GFL_FONT* fontHandle, STRCODE charCode, GFL_FONT_SIZE* size );
static void put1char_16to256( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, GFL_FONT* fontHandle, STRCODE charCode, GFL_FONT_SIZE* size );
static const STRCODE* ctrlGeneralTag( PRINT_JOB* wk, const STRCODE* sp );
static const STRCODE* ctrlSystemTag( PRINT_JOB* wk, const STRCODE* sp );
static void WaitValueToSpeedParam( int wait, STREAM_SPEED_PARAM* param );
static void print_stream_task( GFL_TCBL* tcb, void* wk_adrs );
static void ctrlStreamTag( PRINT_STREAM* wk );
static u32 get_line_width( const STRCODE* sp, GFL_FONT* font, u16 margin, const STRCODE** endPtr );
static inline u8 STR_TOOL_GetTagGroup( const STRCODE* sp );
static inline u8 STR_TOOL_GetTagIndex( const STRCODE* sp );
static inline STRCODE STR_TOOL_CreateTagCode( PrintSysTagGroup grp, u8 index );
static inline u16 STR_TOOL_GetTagParam( const STRCODE* sp, u16 paramIndex );
static inline const STRCODE* STR_TOOL_SkipTag( const STRCODE* sp );
static inline u16 STR_TOOL_GetTagNumber( const STRCODE* sp );
static u32 Que_CalcStringBufferSize( const STRBUF* str );
static u32 Que_CalcUseBufSize( const STRBUF* buf );
static u32 Que_GetFreeSize( const PRINT_QUE* que );
static void Que_StoreNewJob( PRINT_QUE* que, PRINT_JOB* printJob, const STRBUF* str );
static void* Que_WriteDataUnit( PRINT_QUE* que, const u8* src, u16 totalSize, u16 unitSize );
static PRINT_JOB* Que_ReadNextJobAdrs( const PRINT_QUE* que, const PRINT_JOB* job );
static inline u16 Que_WriteData( PRINT_QUE* que, const u8* src, u16 pos, u16 size );
static inline u16 Que_AdrsToBufPos( const PRINT_QUE* que, const void* bufAdrs );
static inline const void* Que_BufPosToAdrs( const PRINT_QUE* que, u16 bufPos );
static inline u16 Que_FwdBufPos( const PRINT_QUE* que, u16 pos, u16 fwdSize );






//==============================================================================================
/**
 * �V�X�e���������iGFLIB��������ɂP�x�����Ăԁj
 *
 * @param   heapID    �������p�q�[�vID
 *
 */
//==============================================================================================
void PRINTSYS_Init( HEAPID heapID )
{
  GFL_STR_SetEOMCode( EOM_CODE );

  // �P��������Bitmap�̈�̂݊m��
  SystemWork.charBuffer = GFL_BMP_Create( LETTER_CHAR_WIDTH, LETTER_CHAR_HEIGHT, GFL_BMP_16_COLOR, heapID );
}



//==============================================================================================
/**
 * �v�����g�L���[���f�t�H���g�̃o�b�t�@�T�C�Y�Ő�������
 *
 * @param   heapID      �����p�q�[�vID
 *
 * @retval  PRINT_QUE*    �������ꂽ�v�����g�L���[
 */
//==============================================================================================
PRINT_QUE* PRINTSYS_QUE_Create( HEAPID heapID )
{
  return PRINTSYS_QUE_CreateEx( QUE_DEFAULT_BUFSIZE, heapID );
}

//==============================================================================================
/**
 * �v�����g�L���[���T�C�Y�w�肵�Đ�������
 *
 * @param   size      �o�b�t�@�T�C�Y
 * @param   heapID      �����p�q�[�vID
 *
 * @retval  PRINT_QUE*
 */
//==============================================================================================
PRINT_QUE* PRINTSYS_QUE_CreateEx( u16 buf_size, HEAPID heapID )
{
  PRINT_QUE* que;
  u16 real_size;

  // �����O�o�b�t�@�̏I�[�ō\���̂����f����Ȃ��悤�A
  // �m�ۃT�C�Y�ɗ]�T���������Ă���
  real_size = buf_size + sizeof(PRINT_JOB);
  while( real_size % 4 ){ real_size++; }

  que = GFL_HEAP_AllocMemory( heapID, sizeof(PRINT_QUE) + real_size );

  que->bufSize = buf_size;    // �I�[����͎w��̃T�C�Y�l�ōs��
  que->limitPerFrame = QUE_DEFAULT_TICK;
  que->runningJob = NULL;
  que->sp = NULL;
  que->fColorChanged = FALSE;
  que->fForceCommMode = FALSE;


  que->debugCounter=0;

  que->bufTopPos = 0;
  que->bufEndPos = buf_size;

//  GFL_STD_MemClear( que->buf, size );

  return que;
}

//==============================================================================================
/**
 * �v�����g�L���[���폜����
 *
 * @param   que   �v�����g�L���[
 *
 */
//==============================================================================================
void PRINTSYS_QUE_Delete( PRINT_QUE* que )
{
  GF_ASSERT(que->runningJob==NULL);
  GFL_HEAP_FreeMemory( que );
}

//=============================================================================================
/**
 * [�f�o�b�O�p] �����I�ɒʐM���Ɠ�����������郂�[�h�ɕύX
 *
 * @param   que
 * @param   flag  TRUE�ŗL���^FALSE�Ŗ�����
 */
//=============================================================================================
void PRINTSYS_QUE_ForceCommMode( PRINT_QUE* que, BOOL flag )
{
  que->fForceCommMode = flag;
}

//=============================================================================================
/**
 * �v�����g�L���[���ʐM���A��x�̏����Ɏg�����ԁiTick�j��ݒ肷��
 *
 * @param   que
 * @param   tick
 *
 */
//=============================================================================================
void PRINTSYS_QUE_SetLimitTick( PRINT_QUE* que, OSTick tick )
{
  que->limitPerFrame = tick;
}

//==============================================================================================
/**
 * �v�����g�L���[�ɒ~�ς��ꂽ������`���Ƃ���������B
 * �ʐM���Ȃ��莞�Ԗ��ɏ�����Ԃ��B
 * �ʐM���łȂ���Έ�C�ɏ������I����i�c�͖̂������j
 *
 * @param   que   �v�����g�L���[
 *
 * @retval  BOOL  �������I�����Ă����TRUE�^����ȊO��FALSE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_Main( PRINT_QUE* que )
{
  que->debugCounter++;
  if( que->runningJob )
  {
    OSTick start, diff;
    u8 endFlag = FALSE;

    start = OS_GetTick();

    while( que->runningJob )
    {
      if( que->runningJob->colorState == JOB_COLORSTATE_CHANGE_REQ )
      {
        u8 colL, colS, colB;

        PRINTSYS_LSB_GetLSB( que->runningJob->jobColor, &colL, &colS, &colB );
        if( GFL_FONTSYS_IsDifferentColor(colL, colS, colB) )
        {
          // �ŏ��̐F�ύX���A�f�t�H���g�̐F�����o���Ă���
          if( que->fColorChanged == FALSE )
          {
            u8 defL, defS, defB;
            GFL_FONTSYS_GetColor( &defL, &defS, &defB );
            que->defColor = PRINTSYS_LSB_Make( defL, defS, defB );
            que->fColorChanged = TRUE;
          }
          GFL_FONTSYS_SetColor( colL, colS, colB );
          que->runningJob->colorState = JOB_COLORSTATE_CHANGE_DONE;
        }
      }

      while( *(que->sp) != EOM_CODE )
      {
        que->sp = print_next_char( que->runningJob, que->sp, TRUE );
        diff = OS_GetTick() - start;
        if( diff > que->limitPerFrame )
        {
          endFlag = TRUE;
          break;
        }
      }
      //
      if( *(que->sp) == EOM_CODE )
      {
        u16 pos;

        // job �I�����ƂɃf�t�H���g�F�ɖ߂�
        if( que->runningJob->colorState == JOB_COLORSTATE_CHANGE_DONE )
        {
          u8 defL, defS, defB;
          PRINTSYS_LSB_GetLSB( que->defColor, &defL, &defS, &defB );
          if( GFL_FONTSYS_IsDifferentColor( defL, defS, defB ) ){
            GFL_FONTSYS_SetColor( defL, defS, defB );
          }
        }
        pos = Que_AdrsToBufPos( que, que->runningJob );

        que->runningJob = Que_ReadNextJobAdrs( que, que->runningJob );
        if( que->runningJob )
        {
          pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB) );
          pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB*) );
          que->sp = Que_BufPosToAdrs( que, pos );
        }
      }

      if( endFlag ){ break; }
    }

    if( que->runningJob )
    {
      que->bufEndPos = Que_AdrsToBufPos( que, que->sp );
      return FALSE;
    }

    que->bufTopPos = 0;
    que->bufEndPos = que->bufSize;
  }

  if( que->fColorChanged )
  {
    u8 defL, defS, defB;
    PRINTSYS_LSB_GetLSB( que->defColor, &defL, &defS, &defB );
    if( GFL_FONTSYS_IsDifferentColor( defL, defS, defB ) ){
      GFL_FONTSYS_SetColor( defL, defS, defB );
    }
  }

  return TRUE;
}
//==============================================================================================
/**
 * �v�����g�L���[�̏������S�Ċ������Ă��邩�`�F�b�N
 *
 * @param   que     �v�����g�L���[
 *
 * @retval  BOOL    �������Ă����TRUE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_IsFinished( const PRINT_QUE* que )
{
  return (que->runningJob == NULL);
}

//==============================================================================================
/**
 * �v�����g�L���[�ɁA����Bitmap���o�͑Ώۂ̕`�揈�����c���Ă��邩�`�F�b�N
 *
 * @param   que       �v�����g�L���[
 * @param   targetBmp   �`��Ώ�Bitmap
 *
 * @retval  BOOL      �c���Ă����TRUE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_IsExistTarget( const PRINT_QUE* que, const GFL_BMP_DATA* targetBmp )
{
  if( que->runningJob )
  {
    const PRINT_JOB* job = que->runningJob;

    while( job )
    {
      if( job->dst == targetBmp )
      {
        return TRUE;
      }
      job = Que_ReadNextJobAdrs( que, job );
    }

  }
  return FALSE;
}

//=============================================================================================
/**
 * �v�����g�L���[�ɒ��܂��Ă��鏈����S�ăN���A����
 *
 * @param   que   �v�����g�L���[
 *
 */
//=============================================================================================
void PRINTSYS_QUE_Clear( PRINT_QUE* que )
{
  que->bufTopPos = 0;
  que->bufEndPos = que->bufSize;
  que->sp = NULL;
  que->runningJob = NULL;
}

//--------------------------------------------------------------------------
/**
 * �ʐM���i�`�揈���𕪊�����K�v������j���ǂ�������
 *
 * @retval  BOOL    �ʐM���Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static inline BOOL IsNetConnecting( void )
{
  return (GFL_NET_GetConnectNum() != 0);
}


//==============================================================================================
/**
 * �v�����g�L���[�����������`�揈�����N�G�X�g
 *
 * @param   que   [out] �v�����g�L���[
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g�^�C�v
 *
 */
//==============================================================================================
void PRINTSYS_PrintQue( PRINT_QUE* que, GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font )
{
  GF_ASSERT(que);
  GF_ASSERT(dst);
  GF_ASSERT(str);
  GF_ASSERT(font);

  {
    PRINT_JOB* job = &SystemWork.printJob;
    printJob_setup( job, font, dst, xpos, ypos );

    PrintQue_Core( que, job, str );
  }
}
//=============================================================================================
/**
 * �v�����g�L���[�����������`�揈�����N�G�X�g�i�F�ύX�ɑΉ��j
 *
 * @param   que
 * @param   dst
 * @param   xpos
 * @param   ypos
 * @param   str
 * @param   font
 * @param   col
 *
 */
//=============================================================================================
void PRINTSYS_PrintQueColor( PRINT_QUE* que, GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font, PRINTSYS_LSB color )
{
  GF_ASSERT(que);
  GF_ASSERT(dst);
  GF_ASSERT(str);
  GF_ASSERT(font);

  {
    PRINT_JOB* job = &SystemWork.printJob;

    printJob_setup( job, font, dst, xpos, ypos );
    printJob_setColor( job, color );

    PrintQue_Core( que, job, str );
  }
}
//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�����������`�揈�����N�G�X�g���ʏ�������
 *
 * @param   job
 *
 */
//--------------------------------------------------------------------------
static void PrintQue_Core( PRINT_QUE* que, PRINT_JOB* job, const STRBUF* str )
{
  if( !IsNetConnecting() && !que->fForceCommMode )
  {
    printJob_finish( job, str );
  }
  else
  {
    u32  size = Que_CalcUseBufSize( str );
    if( size <= Que_GetFreeSize( que ) )
    {
      Que_StoreNewJob( que, job, str );
    }
    else
    {
      GF_ASSERT_MSG(0, "[PRINT_QUE] buffer over ... strsize = %d\n", size);
    }
  }
}

//==============================================================================================
/**
 * Bitmap �֒��ڂ̕�����`��
 *
 * @param   dst   [out] �`���Bitmap
 * @param   xpos  [in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos  [in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str   [in]  ������
 * @param   font  [in]  �t�H���g
 *
 */
//==============================================================================================
void PRINTSYS_Print( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font )
{
  PRINT_JOB* job = &SystemWork.printJob;
  const STRCODE* sp = GFL_STR_GetStringCodePointer( str );

  printJob_setup( job, font, dst, xpos, ypos );
  printJob_finish( job, str );
}
//=============================================================================================
/**
 * Bitmap �֒��ڂ̕�����`��i�J���[�w��Łj
 *
 * @param   dst
 * @param   xpos
 * @param   ypos
 * @param   str
 * @param   font
 * @param   color
 */
//=============================================================================================
void PRINTSYS_PrintColor( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font, PRINTSYS_LSB color )
{
  PRINT_JOB* job = &SystemWork.printJob;
  const STRCODE* sp = GFL_STR_GetStringCodePointer( str );

  printJob_setup( job, font, dst, xpos, ypos );
  printJob_setColor( job, color );
  printJob_finish( job, str );
}


//------------------------------------------------------------------
/**
 * �`�揈�����[�N�̏�����
 *
 * @param   wk      ���[�N�|�C���^
 * @param   font_type �t�H���g�^�C�v
 * @param   dst     �`���Bitmap
 * @param   org_x   �`��J�n�w���W
 * @param   org_y   �`��J�n�x���W
 *
 */
//------------------------------------------------------------------
static void printJob_setup( PRINT_JOB* wk, GFL_FONT* font, GFL_BMP_DATA* dst, s16 org_x, s16 org_y )
{
  wk->dst = dst;
  wk->put1charFunc = (GFL_BMP_GetColorFormat(dst) == GFL_BMP_16_COLOR)?
                put1char_normal : put1char_16to256;
  wk->fontHandle = font;
  wk->org_x = wk->write_x = org_x;
  wk->org_y = wk->write_y = org_y;
  wk->colorState = JOB_COLORSTATE_IGNORE;

  {
    u8 letter, shadow, back;
    GFL_FONTSYS_GetColor( &letter, &shadow, &back );
    wk->defColor = PRINTSYS_LSB_Make( letter, shadow, back );
  }
}
//--------------------------------------------------------------------------
/**
 * �������[�N�P�ʂŎg�p�F��ύX���郊�N�G�X�g
 *
 * @param   wk
 *
 */
//--------------------------------------------------------------------------
static void printJob_setColor( PRINT_JOB* wk, PRINTSYS_LSB color )
{
  wk->jobColor = color;
  wk->colorState = JOB_COLORSTATE_CHANGE_REQ;
}
//--------------------------------------------------------------------------
/**
 * �`�揈�����[�N�̏������e���P��Ŏ��s����
 *
 * @param   job   �`�揈�����[�N
 * @param   str   ����������
 *
 */
//--------------------------------------------------------------------------
static void printJob_finish( PRINT_JOB* job, const STRBUF* str )
{
  const STRCODE* sp = GFL_STR_GetStringCodePointer( str );

  u8 defColorL, defColorS, defColorB, colorChanged;

  defColorL = defColorS = defColorB = 0;
  colorChanged = 0;

  if( job->colorState == JOB_COLORSTATE_CHANGE_REQ )
  {
    u8 colL, colS, colB;

    PRINTSYS_LSB_GetLSB( job->jobColor, &colL, &colS, &colB );
    if( GFL_FONTSYS_IsDifferentColor(colL, colS, colB) )
    {
      GFL_FONTSYS_GetColor( &defColorL, &defColorS, &defColorB );
      GFL_FONTSYS_SetColor( colL, colS, colB );
      colorChanged = TRUE;
    }
    job->colorState = JOB_COLORSTATE_CHANGE_DONE;
  }

  while( *sp != EOM_CODE )
  {
    sp = print_next_char( job, sp, TRUE );
  }

  if( colorChanged )
  {
    GFL_FONTSYS_SetColor( defColorL, defColorS, defColorB );
  }
}


//------------------------------------------------------------------
/**
 * ���s����єėp�R���g���[��������Ώ������A
 * �ʏ핶��������΂P�����`�悵�āA���̕����̃|�C���^��Ԃ�
 *
 * @param   wk    �`�惏�[�N�|�C���^
 * @param   sp
 *
 * @retval  const STRCODE*
 */
//------------------------------------------------------------------
static const STRCODE* print_next_char( PRINT_JOB* wk, const STRCODE* sp, BOOL fSkipStreamTag )
{
  while( 1 )
  {
    switch( *sp ){
    case EOM_CODE:
      return sp;

    case CR_CODE:
      wk->write_x = wk->org_x;
      wk->write_y += LINE_DOT_HEIGHT;
      sp++;
      break;

    case SPCODE_TAG_START_:
      switch( STR_TOOL_GetTagGroup(sp) ){
      case TAGTYPE_GENERAL_CTRL:
        sp = ctrlGeneralTag( wk, sp );
        break;

      case TAGTYPE_SYSTEM:
        sp = ctrlSystemTag( wk, sp );
        break;

      default:
        if( fSkipStreamTag ){
          sp = STR_TOOL_SkipTag( sp );
          break;
        }else{
          return sp;
        }
      }
      break;

    default:
      {
        GFL_FONT_SIZE size;
        u16 w, h;

        wk->put1charFunc( wk->dst, wk->write_x, wk->write_y, wk->fontHandle, *sp, &size );
        wk->write_x += size.width;
        sp++;

        return sp;
      }
      break;
    }
  }
}
//------------------------------------------------------------------
/**
 * Bitmap�P�������`��i�ʏ��=16�F->16�F�t�H�[�}�b�g�j
 *
 * @param[out]  dst       �`���r�b�g�}�b�v
 * @param[in] xpos        �`���w���W�i�h�b�g�j
 * @param[in] ypos        �`���x���W�i�h�b�g�j
 * @param[in] fontHandle  �`��t�H���g�n���h��
 * @param[in] charCode    �����R�[�h
 * @param[out]  size      �����T�C�Y�擾���[�N
 *
 */
//------------------------------------------------------------------
static void put1char_normal( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, GFL_FONT* fontHandle, STRCODE charCode, GFL_FONT_SIZE* size )
{
  GFL_FONT_GetBitMap( fontHandle, charCode, GFL_BMP_GetCharacterAdrs(SystemWork.charBuffer), size );
  GFL_BMP_Print( SystemWork.charBuffer, dst, 0, 0, xpos+size->left_width, ypos, size->glyph_width, size->height, 0x00 );
}
//------------------------------------------------------------------
/**
 * Bitmap�P�������`��i16�F->256�F�t�H�[�}�b�g�j
 *
 * @param[out]  dst       �`���r�b�g�}�b�v
 * @param[in] xpos        �`���w���W�i�h�b�g�j
 * @param[in] ypos        �`���x���W�i�h�b�g�j
 * @param[in] fontHandle  �`��t�H���g�n���h��
 * @param[in] charCode    �����R�[�h
 * @param[out]  size      �����T�C�Y�擾���[�N
 *
 */
//------------------------------------------------------------------
static void put1char_16to256( GFL_BMP_DATA* dst, s16 xpos, s16 ypos, GFL_FONT* fontHandle, STRCODE charCode, GFL_FONT_SIZE* size )
{
  GFL_FONT_GetBitMap( fontHandle, charCode, GFL_BMP_GetCharacterAdrs(SystemWork.charBuffer), size );
  GFL_BMP_Print16to256( SystemWork.charBuffer, dst, 0, 0, xpos+size->left_width, ypos, size->glyph_width, size->height, 0x0f, 0 );
}


//------------------------------------------------------------------
/**
 * �ėp�R���g���[������
 *
 * @param   wk    �`�揈�����[�N
 * @param   sp    �������̕�����|�C���^�i=SPCODE_TAG_START_���|�C���g���Ă����ԂŌĂԂ��Ɓj
 *
 * @retval  const STRCODE*    ������̕�����|�C���^
 */
//------------------------------------------------------------------
static const STRCODE* ctrlGeneralTag( PRINT_JOB* wk, const STRCODE* sp )
{
  switch( STR_TOOL_GetTagNumber(sp) ){
  case PRINTSYS_CTRL_GENERAL_COLOR:
    {
      u8 colL, colS, colB;

      colL = STR_TOOL_GetTagParam(sp, 0);
      colS = STR_TOOL_GetTagParam(sp, 1);
      colB = STR_TOOL_GetTagParam(sp, 2);

      GFL_FONTSYS_SetColor( colL, colS, colB );
    }
    break;

  case PRINTSYS_CTRL_GENERAL_RESET_COLOR:
    {
      u8 colL, colS, colB;
      PRINTSYS_LSB_GetLSB( wk->defColor, &colL, &colS, &colB );
      GFL_FONTSYS_SetColor( colL, colS, colB );
    }
    break;

  case PRINTSYS_CTRL_GENERAL_X_RIGHTFIT:
    {
      int areaWidth, strWidth;

      areaWidth = GFL_BMP_GetSizeX( wk->dst ) - wk->org_x;
      strWidth = get_line_width( sp, wk->fontHandle, 0, NULL );
      wk->write_x = wk->org_x + (areaWidth - strWidth) - 1 - STR_TOOL_GetTagParam( sp, 0 );
    }
    break;

  case PRINTSYS_CTRL_GENERAL_X_CENTERING:
    {
      int bmpWidth, strWidth;

      bmpWidth = GFL_BMP_GetSizeX( wk->dst ) - wk->org_x;
      strWidth = get_line_width( sp, wk->fontHandle, 0, NULL );

      if( strWidth < bmpWidth )
      {
        wk->write_x = wk->org_x + ((bmpWidth - strWidth) / 2);
      }
      else
      {
        wk->write_x = wk->org_x;
      }
    }
    break;

  case PRINTSYS_CTRL_GENERAL_X_ADD:
    wk->write_x += STR_TOOL_GetTagParam( sp, 0 );
    break;

  case PRINTSYS_CTRL_GENERAL_X_SET:
    wk->write_x = STR_TOOL_GetTagParam( sp, 0 );
    break;

  default:
    GF_ASSERT_MSG(0, "illegal general-ctrl code ->%d\n", STR_TOOL_GetTagNumber(sp));
    break;
  }

  return STR_TOOL_SkipTag( sp );
}
//------------------------------------------------------------------
/**
 * �V�X�e���R���g���[������
 *
 * @param   wk    �`�揈�����[�N
 * @param   sp    �������̕�����|�C���^�i=SPCODE_TAG_START_���|�C���g���Ă����ԂŌĂԂ��Ɓj
 *
 * @retval  const STRCODE*    ������̕�����|�C���^
 */
//------------------------------------------------------------------
static const STRCODE* ctrlSystemTag( PRINT_JOB* wk, const STRCODE* sp )
{
  u16 ctrlCode = STR_TOOL_GetTagNumber(sp);

  switch( ctrlCode ){
  case CTRL_SYSTEM_COLOR:
    {
      u8  colorIdx;
      u8  bgcolor;

      colorIdx = STR_TOOL_GetTagParam( sp, 0 );
      bgcolor = PRINTSYS_LSB_GetB( wk->defColor );
      GFL_FONTSYS_SetColor( colorIdx*2+1, colorIdx*2+2, bgcolor );
    }
    break;

  default:
    GF_ASSERT_MSG(0, "illegal system-ctrl code ->%d\n", ctrlCode);
    break;
  }

  return STR_TOOL_SkipTag( sp );
}


//==============================================================================================================
// �v�����g�X�g���[���֘A����
//==============================================================================================================

//==============================================================================================
/**
 * �v�����g�X�g���[���쐬�i�ʏ�� - �R�[���o�b�N�Ȃ��j
 *
 * @param   dst             �`���Bitmap
 * @param   xpos            �`��J�n�w���W�i�h�b�g�j
 * @param   ypos            �`��J�n�x���W�i�h�b�g�j
 * @param   str             ������
 * @param   font            �t�H���g�^�C�v
 * @param   wait            �P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @param   tcbsys          �g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri          �g�p����^�X�N�v���C�I���e�B
 * @param   clearColorIdx   �`��N���A�p�̐F�C���f�b�N�X
 * @param   heapID          �g�p����q�[�vID
 *
 * @retval  PRINT_STREAM* �X�g���[���n���h��
 *
 */
//==============================================================================================
PRINT_STREAM* PRINTSYS_PrintStream(
    GFL_BMPWIN* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font,
    int wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColorIdx )
{
  return PRINTSYS_PrintStreamCallBack(
    dst, xpos, ypos, str, font, wait, tcbsys, tcbpri, heapID, clearColorIdx, NULL
  );
}

//==============================================================================================
/**
 * �v�����g�X�g���[���쐬�i�R�[���o�b�N����j
 *
 * @param   dst             �`���Bitmap
 * @param   xpos            �`��J�n�w���W�i�h�b�g�j
 * @param   ypos            �`��J�n�x���W�i�h�b�g�j
 * @param   str             ������
 * @param   font            �t�H���g�^�C�v
 * @param   wait            �P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @param   tcbsys          �g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri          �g�p����^�X�N�v���C�I���e�B
 * @param   heapID          �g�p����q�[�vID
 * @param   clearColorIdx   �`��N���A�p�̐F�C���f�b�N�X
 * @param   callback        �P�����`�悲�Ƃ̃R�[���o�b�N�֐��A�h���X
 *
 * @retval  PRINT_STREAM* �X�g���[���n���h��
 *
 */
//==============================================================================================
PRINT_STREAM* PRINTSYS_PrintStreamCallBack(
    GFL_BMPWIN* dst, s16 xpos, s16 ypos, const STRBUF* str, GFL_FONT* font,
    int wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColorIdx, pPrintCallBack callback )
{
  PRINT_STREAM* stwk;
  GFL_TCBL* tcb;
  GFL_BMP_DATA* dstBmp;

  dstBmp = GFL_BMPWIN_GetBmp( dst );;
  tcb = GFL_TCBL_Create( tcbsys, print_stream_task, sizeof(PRINT_STREAM), tcbpri );
  stwk = GFL_TCBL_GetWork( tcb );
  stwk->tcb = tcb;

  printJob_setup( &stwk->printJob, font, dstBmp, xpos, ypos );


  {
    STREAM_SPEED_PARAM  param;
    WaitValueToSpeedParam( wait, &param );
    stwk->org_putPerFrame = param.putPerFrame;
    stwk->org_wait = param.wait;
  }

  stwk->current_wait = stwk->org_wait;
  stwk->current_putPerFrame = stwk->org_putPerFrame;

  stwk->sp = GFL_STR_GetStringCodePointer( str );
  stwk->wait = 0;
  stwk->callback_func = callback;
  stwk->org_arg = 0;
  stwk->current_arg = 0;
  stwk->arg = 0;
  stwk->dstWin = dst;
  stwk->dstBmp = dstBmp;
  stwk->state = PRINTSTREAM_STATE_RUNNING;
  stwk->clearColor = clearColorIdx;
  stwk->pauseType = PRINTSTREAM_PAUSE_CLEAR;
  stwk->pauseWait = 0;
  stwk->pauseReleaseFlag = FALSE;
  stwk->stopFlag = FALSE;
  stwk->lineMoningFlag = FALSE;
  stwk->callbackResult = FALSE;

  return stwk;
}

//----------------------------------------------------------------------------------
/**
 * wait�l -> �X�s�[�h�p�����[�^�ϊ�
 *
 * @param   wait
 * @param   param
 */
//----------------------------------------------------------------------------------
static void WaitValueToSpeedParam( int wait, STREAM_SPEED_PARAM* param )
{
  if( wait >= 0 ){
    param->putPerFrame = 1;
    param->wait = wait;
  }else{
    param->putPerFrame = -wait;
    param->wait = 0;
  }
}

//=============================================================================================
/**
 * �v�����g�X�g���[�������[�U�v���O�����������I�Ɉꎞ��~����
 *
 * @param   handle
 */
//=============================================================================================
void PRINTSYS_PrintStreamStop( PRINT_STREAM* handle )
{
  handle->stopFlag = TRUE;
}
//=============================================================================================
/**
 * �v�����g�X�g���[���̃��[�U�����ꎞ��~����������
 *
 * @param   handle
 */
//=============================================================================================
void PRINTSYS_PrintStreamRun( PRINT_STREAM* handle )
{
  handle->stopFlag = FALSE;
}
//==============================================================================================
/**
 * �v�����g�X�g���[����Ԏ擾
 *
 * @param   handle    �X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_STATE   ���
 */
//==============================================================================================
PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM* handle )
{
  return handle->state;
}
//==============================================================================================
/**
 * �v�����g�X�g���[����~�^�C�v�擾
 *
 * @param   handle    �X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_PAUSE_TYPE    ��~�^�C�v
 */
//==============================================================================================
PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM* handle )
{
  return handle->pauseType;
}
//==============================================================================================
/**
 * �v�����g�X�g���[����~��ԉ���
 *
 * @param   handle
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM* handle )
{
  handle->pauseReleaseFlag = TRUE;
}
//==============================================================================================
/**
 * �v�����g�X�g���[���폜
 *
 * @param   handle
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamDelete( PRINT_STREAM* handle )
{
  if( handle->tcb )
  {
    GFL_TCBL_Delete( handle->tcb );
    handle->tcb = NULL;
  }
}
//==============================================================================================
/**
 * �v�����g�X�g���[���E�F�C�g�Z�k�i�L�[�������Ȃǁj
 *
 * @param   handle    �X�g���[���n���h��
 * @param   wait    �ݒ肷��E�F�C�g�t���[����
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamShortWait( PRINT_STREAM* handle, u16 wait )
{
  if( wait < handle->wait )
  {
    handle->wait = wait;
  }
}

//------------------------------------------------------------------
/**
 * �v�����g�X�g���[���`��^�X�N
 *
 * @param   tcb
 * @param   wk_adrs
 *
 */
//------------------------------------------------------------------
static void print_stream_task( GFL_TCBL* tcb, void* wk_adrs )
{
  PRINT_STREAM* wk = wk_adrs;

  if( wk->callback_func != NULL && wk->callbackResult ){
    wk->callbackResult = wk->callback_func( wk->arg );
    if( wk->callbackResult ){
      return;
    }
  }

  if( wk->stopFlag ){
    return;
  }

  switch( wk->state ){
  case PRINTSTREAM_STATE_RUNNING:

    // �s���菈����
    if( wk->lineMoningFlag )
    {
      if( wk->pauseWait < LINE_DOT_HEIGHT )
      {
        u16 size_x, size_y, feed_speed;

        feed_speed = LINE_FEED_SPEED;
        if( (wk->pauseWait+feed_speed) > LINE_DOT_HEIGHT ){
          feed_speed = (LINE_DOT_HEIGHT - wk->pauseWait);
        }
        wk->pauseWait += feed_speed;

        size_x = GFL_BMP_GetSizeX( wk->dstBmp );
        size_y = GFL_BMP_GetSizeY( wk->dstBmp );
        GFL_BMP_Print( wk->dstBmp, wk->dstBmp, 0, feed_speed, 0, 0, size_x, size_y-feed_speed, GF_BMPPRT_NOTNUKI );
        GFL_BMP_Fill( wk->dstBmp, 0, size_y-feed_speed, size_x, feed_speed, wk->clearColor );
        GFL_BMPWIN_TransVramCharacter( wk->dstWin );
      }else{
        wk->printJob.write_x = wk->printJob.org_x;
        wk->printJob.write_y = LINE_DOT_HEIGHT;
        wk->lineMoningFlag = FALSE;
      }
      break;
    }

    if( wk->wait == 0 )
    {
      int i;
      for(i=0; i<wk->current_putPerFrame; ++i)
      {
        switch( *(wk->sp) ){
        case EOM_CODE:
          wk->state = PRINTSTREAM_STATE_DONE;
          break;

        case SPCODE_TAG_START_:
          if( STR_TOOL_GetTagGroup(wk->sp) == TAGTYPE_STREAM_CTRL )
          {
            ctrlStreamTag( wk );
            if( wk->state != PRINTSTREAM_STATE_RUNNING ){
              i = wk->current_putPerFrame;  // for loop out
            }
            break;
          }
          /* fallthru */
        default:
          {
            wk->sp = print_next_char( &wk->printJob, wk->sp, FALSE );
            if( *(wk->sp) == EOM_CODE )
            {
              wk->state = PRINTSTREAM_STATE_DONE;
            }
            else if( *(wk->sp) != CR_CODE )
            {
              wk->wait = wk->current_wait;
            }

            GFL_BMPWIN_TransVramCharacter( wk->dstWin );
            if( wk->callback_func )
            {
              wk->callbackResult = wk->callback_func( wk->arg );
              wk->arg = wk->current_arg;
            }
            break;
          }
        }
      }
    }
    else
    {
      wk->wait--;
    }
    break;

  case PRINTSTREAM_STATE_PAUSE:
    if( wk->pauseReleaseFlag )
    {
      switch( wk->pauseType ){
      case PRINTSTREAM_PAUSE_LINEFEED:
        wk->lineMoningFlag = TRUE;
        wk->state = PRINTSTREAM_STATE_RUNNING;
        break;

      case PRINTSTREAM_PAUSE_CLEAR:
      default:
        GFL_BMP_Clear( wk->dstBmp, wk->clearColor );
        wk->state = PRINTSTREAM_STATE_RUNNING;
        break;
      }
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * �v�����g�X�g���[���p�R���g���[���^�O����
 *
 * @param   wk
 *
 */
//--------------------------------------------------------------------------
static void ctrlStreamTag( PRINT_STREAM* wk )
{
  BOOL skipCR = FALSE;  // ����̉��s�𖳎�����

  switch( STR_TOOL_GetTagNumber(wk->sp) ){
  case PRINTSYS_CTRL_STREAM_LINE_FEED:
    wk->state = PRINTSTREAM_STATE_PAUSE;
    wk->pauseType = PRINTSTREAM_PAUSE_LINEFEED;
    wk->pauseWait = 0;
    wk->pauseReleaseFlag = FALSE;
    wk->printJob.write_x = wk->printJob.org_x;
    wk->printJob.write_y = 0;
    skipCR = TRUE;
    break;
  case PRINTSYS_CTRL_STREAM_PAGE_CLEAR:
    wk->state = PRINTSTREAM_STATE_PAUSE;
    wk->pauseType = PRINTSTREAM_PAUSE_CLEAR;
    wk->pauseWait = 0;
    wk->pauseReleaseFlag = FALSE;
    wk->printJob.write_x = wk->printJob.org_x;
    wk->printJob.write_y = 0;
    skipCR = TRUE;
    break;
  case PRINTSYS_CTRL_STREAM_CHANGE_WAIT:
    wk->wait = STR_TOOL_GetTagParam( wk->sp, 0 );
    break;
  case PRINTSYS_CTRL_STREAM_SET_WAIT:
    wk->current_wait = STR_TOOL_GetTagParam( wk->sp, 0 );
    break;
  case PRINTSYS_CTRL_STREAM_RESET_WAIT:  ///< �`��E�F�C�g���f�t�H���g�ɖ߂�
    wk->current_wait = wk->org_wait;
    break;
  case PRINTSYS_CTRL_STREAM_SPEED_CTRL:
    #ifndef MULTI_BOOT_MAKE
    {
      STREAM_SPEED_PARAM  param;
      int wait;

      u8 mode = STR_TOOL_GetTagParam( wk->sp, 0 );

      switch( mode ){
      case SPEED_CTRL_RESET:
      default:
        param.wait = wk->org_wait;
        param.putPerFrame = wk->org_putPerFrame;
        break;

      case SPEED_CTRL_FAST:
        wait = MSGSPEED_GetWaitFast();
        WaitValueToSpeedParam( wait, &param );
        break;

      case SPEED_CTRL_SLOW:
        wait = MSGSPEED_GetWaitSlow();
        WaitValueToSpeedParam( wait, &param );
        break;
      }
      wk->current_wait = param.wait;
      wk->current_putPerFrame = param.putPerFrame;
    }
    #endif
    break;
  case PRINTSYS_CTRL_STREAM_CHANGE_ARGV: ///< �R�[���o�b�N������ύX�i�P��j
    wk->arg = STR_TOOL_GetTagParam( wk->sp, 0 );;
    break;
  case PRINTSYS_CTRL_STREAM_SET_ARGV:    ///< �R�[���o�b�N������ύX�i�i���j
    wk->current_arg = STR_TOOL_GetTagParam( wk->sp, 0 );
    wk->arg = STR_TOOL_GetTagParam( wk->sp, 0 );
    break;
  case PRINTSYS_CTRL_STREAM_RESET_ARGV:  ///< �R�[���o�b�N�������f�t�H���g�ɖ߂�
    wk->current_arg = wk->org_arg;
    wk->arg = wk->org_arg;
    break;
  case PRINTSYS_CTRL_STREAM_FORCE_CLEAR:
    GFL_BMP_Clear( wk->dstBmp, wk->clearColor );
    break;
  }

  wk->sp = STR_TOOL_SkipTag( wk->sp );

  if( skipCR && *(wk->sp) == CR_CODE )
  {
    wk->sp++;
  }

}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * ������P�s���̃h�b�g���v�Z
 *
 * @param   sp      ������|�C���^
 * @param   font    �t�H���g�^�C�v
 * @param   margin    ���ԃX�y�[�X�i�h�b�g�j
 * @param   endPtr    [out] ���s�̐擪�|�C���^���i�[����|�C���^�A�h���X�iNULL�Ȃ疳������j
 *
 * @retval  u32   �h�b�g��
 */
//--------------------------------------------------------------------------
static u32 get_line_width( const STRCODE* sp, GFL_FONT* font, u16 margin, const STRCODE** endPtr )
{
  u32 width = 0;

  while( *sp != EOM_CODE && *sp != CR_CODE )
  {
    if( *sp != SPCODE_TAG_START_ )
    {
      width += ( GFL_FONT_GetWidth( font, *sp ) + margin );
      sp++;
    }
    else
    {
      sp = STR_TOOL_SkipTag( sp );
    }
  }

  if( endPtr )
  {
    if( *sp == CR_CODE ){ sp++; }
    *endPtr = sp;
  }

  return width;
}

//=============================================================================================
/**
 * ������̍s����Ԃ�
 *
 * @param   str
 *
 * @retval  u32
 */
//=============================================================================================
u32 PRINTSYS_GetLineCount( const STRBUF* str )
{
  const STRCODE* sp = GFL_STR_GetStringCodePointer( str );
  u32 count = 1;

  while( *sp != EOM_CODE )
  {
    if( *sp != SPCODE_TAG_START_ )
    {
      if( *sp == CR_CODE ){
        ++count;
      }
      ++sp;
    }
    else
    {
      sp = STR_TOOL_SkipTag( sp );
    }
  }

  return count;
}

//==============================================================================================
/**
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 * �������񂪕����s����ꍇ�A���̍Œ��s�̃h�b�g��
 *
 * @param   str     ������
 * @param   font    �t�H���g�^�C�v
 * @param   margin    ���ԃX�y�[�X�i�h�b�g�j
 *
 * @retval  u32     �h�b�g��
 */
//==============================================================================================
u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin )
{
  u32 width, max;
  const STRCODE* sp;

  width = max = 0;
  sp = GFL_STR_GetStringCodePointer( str );

  while( *sp != EOM_CODE )
  {
    width = get_line_width( sp, font, margin, &sp );
    if( width > max )
    {
      max = width;
    }
  }

  return max;
}
//=============================================================================================
/**
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 * �������񂪕����s����ꍇ�A�s���Ƃ̕����擾�ł���
 *
 * @param   str       ������
 * @param   font      �t�H���g�n���h��
 * @param   margin    ���ԃX�y�[�X�i�h�b�g�j
 * @param   dst       �s���Ƃ̕����擾���邽�߂̔z��
 * @param   dstElems  dst �̔z��v�f���i������z���Ȃ��悤�ɏ������݂܂��j
 *
 * @retval  u32       dst �Ɋi�[���������i�ő�ł� dstElems���z���Ȃ��j
 */
//=============================================================================================
u32 PRINTSYS_GetStrLineWidth( const STRBUF* str, GFL_FONT* font, u16 margin, u32* dst, u32 dstElems )
{
  const STRCODE* sp;
  u32 cnt;

  sp = GFL_STR_GetStringCodePointer( str );
  cnt = 0;
  while( cnt < dstElems )
  {
    if( *sp != EOM_CODE )
    {
      dst[cnt++] = get_line_width( sp, font, margin, &sp );
    }
    else
    {
      break;
    }
  }
  return cnt;
}

//=============================================================================================
/**
 * �����s���镶���񂩂�w��s�̕�����݂̂����o��
 * �����[�J���C�Y���ӁI�@�g�p�͋����ł��B��
 *
 * @param   src     �ǂݎ�茳�̕�����
 * @param   dst     �ǂݎ���o�b�t�@
 * @param   line    �s�ԍ��i0�`�j
 *
 * @retval  BOOL    �w�� line ���傫�����ēǂݎ��Ȃ��ꍇFALSE�^����ȊO��TRUE
 */
//=============================================================================================
BOOL PRINTSYS_CopyLineStr( const STRBUF* src, STRBUF* dst, u32 line )
{
  const STRCODE* sp = GFL_STR_GetStringCodePointer( src );

  GFL_STR_ClearBuffer( dst );

  while( line )
  {
    while( *sp != EOM_CODE && *sp != CR_CODE )
    {
      if( *sp != SPCODE_TAG_START_ ){
        ++sp;
      }else{
        sp = STR_TOOL_SkipTag( sp );
      }
    }
    --line;
    if( *sp == EOM_CODE ){
      break;
    }
    else{
      ++sp;
    }
  }

  if( (line == 0) && (*sp != EOM_CODE) )
  {
    while( *sp != EOM_CODE && *sp != CR_CODE )
    {
      if( *sp != SPCODE_TAG_START_ )
      {
        GFL_STR_AddCode( dst, *sp++ );
      }
      else
      {
        const STRCODE* next = STR_TOOL_SkipTag( sp );
        while( sp < next ){
          GFL_STR_AddCode( dst, *sp++ );
        }
      }
    }
    GFL_STR_AddCode( dst, EOM_CODE );
    return TRUE;
  }

  return FALSE;
}

//=============================================================================================
/**
 * �������Bitmap�\������ۂ̍����i�h�b�g���j���v�Z
 *
 * @param   str
 * @param   font
 *
 * @retval  u32     �h�b�g��
 */
//=============================================================================================
u32 PRINTSYS_GetStrHeight( const STRBUF* str, GFL_FONT* font )
{
  const STRCODE* sp;
  u16 numCR;

  sp = GFL_STR_GetStringCodePointer( str );
  numCR = 0;

  while( *sp != EOM_CODE )
  {
    if( *sp == CR_CODE )
    {
      ++numCR;
    }
    if( *sp == SPCODE_TAG_START_ )
    {
      sp = STR_TOOL_SkipTag( sp );
    }
    else
    {
      ++sp;
    }
  }

  return GFL_FONT_GetLineHeight( font ) * (numCR + 1);
}


//=============================================================================================
/**
 * ������̒��Ɋ܂܂��A�P��^�O�̐����J�E���g���ĕԂ�
 *
 * @param   str   ������
 *
 * @retval  u16   �P��^�O�̐�
 */
//=============================================================================================
u16 PRINTSYS_GetTagCount( const STRBUF* str )
{
  const STRCODE* sp;
  u16 cnt = 0;

  sp = GFL_STR_GetStringCodePointer( str );

  while( *sp != EOM_CODE )
  {
    if( *sp == SPCODE_TAG_START_ )
    {
      if( PRINTSYS_IsWordSetTagGroup(sp) )
      {
        cnt++;
      }
      sp = PRINTSYS_SkipTag( sp );
    }
    sp++;
  }

  return cnt;
}

//=============================================================================================
/**
 * �w�肵���^�O�i�W�J�O�j�������񒆉��s�ڂɂ��邩��Ԃ�
 *
 * @param   str     ������
 * @param   tag_id  �^�O�C���f�b�N�X�i�o�b�t�@�ԍ��j
 *
 * @retval  u8      �s��
 *
 * @note  add by genya hosaka
 */
//=============================================================================================
u8 PRINTSYS_GetTagLine( const STRBUF* str, u8 tag_id )
{
  const STRCODE* sp;
  u16 line = 0;

  sp = GFL_STR_GetStringCodePointer( str );

#if 0
  HOSAKA_Printf("================\n");
  // ��[�S�ăv�����g
  while( *sp != EOM_CODE )
  {
    HOSAKA_Printf("tag_id=%d sp=%x \n", tag_id, *sp);
    sp++;
  }
  HOSAKA_Printf("================\n");
#endif

  sp = GFL_STR_GetStringCodePointer( str );

  while( *sp != EOM_CODE )
  {
//  HOSAKA_Printf("tag_id=%d sp=%x \n", tag_id, *sp);

    // ���s�J�E���g
    if( *sp == CR_CODE )
    {
      line++;
    }

    // �^�O���o
    if( *sp == SPCODE_TAG_START_ )
    {
      if( PRINTSYS_IsWordSetTagGroup(sp) )
      {
        if( STR_TOOL_GetTagParam(sp, 0) == tag_id )
        {
          return line;
        }
      }
      // �^�O����̕����𓪂���
      sp = PRINTSYS_SkipTag( sp );
    }
    else
    {
      // ���̕������Q��
      sp++;
    }

  }

  GF_ASSERT( 0 ); ///< �w��^�O�͌�����Ȃ�����

  return 0;
}

//=============================================================================================
/**
 * �w�肵���^�O�i�W�J�O�j�����h�b�g�ڂɂ��邩��Ԃ�
 *
 * @param   str     ������
 * @param   tag_id  �^�O�C���f�b�N�X�i�o�b�t�@�ԍ��j
 * @param   font    �t�H���g�^�C�v
 * @param   margin  ���ԃX�y�[�X�i�h�b�g�j
 *
 * @retval  u8      X���W�i�h�b�g�j
 *
 * @note  add by genya hosaka
 */
//=============================================================================================
u8 PRINTSYS_GetTagWidth( const STRBUF* str, u8 tag_id, GFL_FONT* font, u16 margin )
{
  const STRCODE* sp;
  u32 width = 0;

  sp = GFL_STR_GetStringCodePointer( str );

  while( *sp != EOM_CODE )
  {
    // ���s�R�[�h���������[���Z�b�g
    if( *sp == CR_CODE )
    {
      width = 0;
      sp++;
    }
    // �^�O�ȊO�Ȃ�h�b�g���v��
    else if( *sp != SPCODE_TAG_START_ )
    {
      width += ( GFL_FONT_GetWidth( font, *sp ) + margin );
//    HOSAKA_Printf("[%d] sp=%x width=%d \n",tag_id, *sp, width );
      sp++;
    }
    else
    {
      if( PRINTSYS_IsWordSetTagGroup(sp) )
      {
        if( STR_TOOL_GetTagParam(sp, 0) == tag_id )
        {
          return width;
        }
        sp = STR_TOOL_SkipTag( sp );
      }
    }
  }

  GF_ASSERT(0);

  return 0;
}

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���擾
 *
 * @retval  STRCODE
 */
//=============================================================================================
STRCODE PRINTSYS_GetTagStartCode( void )
{
  return SPCODE_TAG_START_;
}
//=============================================================================================
/**
 * �^�O�R�[�h���P��Z�b�g�p�^�O�R�[�h���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���|�C���g���Ă��邱�Ɓj
 *
 * @retval  BOOL    �P��Z�b�g�p�^�O�R�[�h�Ȃ�TRUE
 */
//=============================================================================================
BOOL PRINTSYS_IsWordSetTagGroup( const STRCODE* sp )
{
  u16 type = STR_TOOL_GetTagGroup( sp );
  return (type == TAGTYPE_WORD) || (type==TAGTYPE_NUMBER);
}


//=============================================================================================
/**
 * �^�O�O���[�v�擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���|�C���g���Ă��邱�Ɓj
 *
 * @retval  PrintSysTagGroup   �^�O�O���[�v�igmm �� �|�P����WB�i�P��j�C�|�P����WB�i���l�j���j
 */
//=============================================================================================
PrintSysTagGroup PRINTSYS_GetTagGroup( const STRCODE* sp )
{
  return STR_TOOL_GetTagGroup( sp );
}

//=============================================================================================
/**
 * �^�O�R�[�h�𕶎��񉻂��ăo�b�t�@�Ɋi�[
 *
 * @param   str         [out] �i�[��o�b�t�@
 * @param   tagGrp
 * @param   tagIdx
 * @param   numParams
 * @param   params
 *
 */
//=============================================================================================
void PRINTSYS_CreateTagCode( STRBUF* str, PrintSysTagGroup tagGrp, u8 tagIdx, u8 numParams, const u16* params )
{
  GFL_STR_AddCode( str, SPCODE_TAG_START_ );
  GFL_STR_AddCode( str, STR_TOOL_CreateTagCode(tagGrp, tagIdx) );
  GFL_STR_AddCode( str, numParams );
  {
    u8 i;
    for(i=0; i<numParams; ++i){
      GFL_STR_AddCode( str, params[i] );
    }
  }
}

//=============================================================================================
/**
 * �^�O�C���f�b�N�X�擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���|�C���g���Ă��邱�Ɓj
 *
 * @retval  u16   �^�O�C���f�b�N�X
 */
//=============================================================================================
u8 PRINTSYS_GetTagIndex( const STRCODE* sp )
{
  return STR_TOOL_GetTagIndex( sp );
}

//=============================================================================================
/**
 * �^�O�p�����[�^�擾
 *
 * @param   sp
 * @param   paramIdx
 *
 * @retval  u16
 */
//=============================================================================================
u16 PRINTSYS_GetTagParam( const STRCODE* sp, u16 paramIdx )
{
  return STR_TOOL_GetTagParam( sp, paramIdx );
}

//=============================================================================================
/**
 * �^�O�������X�L�b�v
 *
 * @param   sp
 *
 * @retval  const STRCODE*
 */
//=============================================================================================
const STRCODE* PRINTSYS_SkipTag( const STRCODE* sp )
{
  return STR_TOOL_SkipTag( sp );
}






//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �^�O�O���[�v
 */
//----------------------------------------------------------------------------------
static inline u8 STR_TOOL_GetTagGroup( const STRCODE* sp )
{
  GF_ASSERT( *sp == SPCODE_TAG_START_ );

  if( *sp == SPCODE_TAG_START_ )
  {
    sp++;
    return (((*sp)>>8)&0xff);
  }

  return 0xffff;
}
//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���C���f�b�N�X��Ԃ�
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �O���[�v���C���f�b�N�X
 */
//----------------------------------------------------------------------------------
static inline u8 STR_TOOL_GetTagIndex( const STRCODE* sp )
{
  GF_ASSERT( *sp == SPCODE_TAG_START_ );

  if( *sp == SPCODE_TAG_START_ )
  {
    sp++;
    return ((*sp)&0xff);
  }

  return 0xffff;
}
//----------------------------------------------------------------------------------
/**
 * �^�O�O���[�v�A�O���[�v���C���f�b�N�X����^�O�R�[�h�𐶐�
 *
 * @param   grp
 * @param   index
 *
 * @retval  inline STRCODE
 */
//----------------------------------------------------------------------------------
static inline STRCODE STR_TOOL_CreateTagCode( PrintSysTagGroup grp, u8 index )
{
  return (grp & 0xff) << 8 | index;
}

//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃^�C�v���擾
 *
 * @param   sp            ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 * @param   paramIndex    �p�����[�^�C���f�b�N�X
 *
 * @retval  u16   �^�O�^�C�v
 */
//----------------------------------------------------------------------------------
static inline u16 STR_TOOL_GetTagParam( const STRCODE* sp, u16 paramIndex )
{
  GF_ASSERT( *sp == SPCODE_TAG_START_ );

  if( *sp == SPCODE_TAG_START_ )
  {
    u32 params;
    sp += 2;
    params = *sp++;
    GF_ASSERT(paramIndex<params);
    return sp[paramIndex];
  }
  return 0;
}

//============================================================================================
/**
 * ������|�C���^�̃^�O�R�[�h�E�p�����[�^�������X�L�b�v�����|�C���^��Ԃ�
 *
 * @param   sp    ������|�C���^�i�^�O�R�[�h�J�n�l���w���Ă��邱�Ɓj
 *
 * @retval  const STRCODE*    �X�L�b�v��̃|�C���^
 */
//============================================================================================
static inline const STRCODE* STR_TOOL_SkipTag( const STRCODE* sp )
{
  GF_ASSERT( *sp == SPCODE_TAG_START_ );

  if( *sp == SPCODE_TAG_START_ )
  {
    u32 params;
    sp += 2;
    params = *sp++;
    sp += params;
    return sp;
  }
  return sp;
}
//============================================================================================
/**
 * ������|�C���^�̎w���Ă���^�O�R�[�h�E�p�����[�^������A�^�O�ԍ����擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u16   �^�O���
 */
//============================================================================================
static inline u16 STR_TOOL_GetTagNumber( const STRCODE* sp )
{
  GF_ASSERT( *sp == SPCODE_TAG_START_ );

  if( *sp == SPCODE_TAG_START_ )
  {
    sp++;
    return ((*sp)&0xff);
  }

  return 0xffff;
}


//==============================================================================================================
// �v�����g�L���[�֘A����
//==============================================================================================================

//--------------------------------------------------------------------------
/**
 * ������i�[�ɕK�v�ȃo�b�t�@�T�C�Y(byte)���v�Z�B��4�̔{���ɑ�����
 *
 * @param   str   [in] ������i�[�o�b�t�@
 *
 * @retval  u32   �o�b�t�@�T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_CalcStringBufferSize( const STRBUF* str )
{
  u32 size = sizeof(STRCODE) * (GFL_STR_GetBufferLength(str) + 1);
  while( size % 4 ){ size++; }
  return size;
}
//--------------------------------------------------------------------------
/**
 * �P���̏������ݏ��� �v�����g�L���[�ɕۑ����邽�߂ɕK�v�ȃo�b�t�@�T�C�Y�ibyte�j���v�Z
 *
 * @param   buf   [in] �������ޕ����񂪊i�[����Ă��镶����o�b�t�@
 *
 * @retval  u32   �v�����g�L���[���K�v�Ƃ���o�b�t�@�T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_CalcUseBufSize( const STRBUF* buf )
{
  return sizeof(PRINT_JOB) + sizeof(PRINT_QUE*) + Que_CalcStringBufferSize(buf);
}

//--------------------------------------------------------------------------
/**
 * �v�����g�L���[ �̃o�b�t�@�󂫃T�C�Y���擾
 *
 * @param   que   [in] �v�����g�L���[
 *
 * @retval  u32   �󂫃T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_GetFreeSize( const PRINT_QUE* que )
{
  if( que->bufTopPos < que->bufEndPos )
  {
    return que->bufEndPos - que->bufTopPos;
  }
  else
  {
    return (que->bufSize - que->bufTopPos) + que->bufEndPos;
  }
}

//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�ɐV�K�`�揈����ǉ�
 *
 * @param   que     [out] �v�����g�L���[
 * @param   printJob  [in] ���e�ݒ�ς݂̕`�揈�����[�N
 * @param   str     [in] �`�敶����o�b�t�@
 *
 */
//--------------------------------------------------------------------------
static void Que_StoreNewJob( PRINT_QUE* que, PRINT_JOB* printJob, const STRBUF* str )
{
  PRINT_JOB* store_adrs;
  const STRCODE* sp;

  sp = Que_WriteDataUnit( que,
            (const u8*)(GFL_STR_GetStringCodePointer(str)),
            Que_CalcStringBufferSize(str),
            sizeof(STRCODE) );

  store_adrs = Que_WriteDataUnit( que, (const u8*)(printJob), sizeof(PRINT_JOB), sizeof(PRINT_JOB) );


  // ���ɏ��������Ɠ��e�ւ̃|�C���^�i�V�K�ǉ��Ȃ玟�͖����̂�NULL�ɂ��Ă���
  {
    PRINT_JOB* p = NULL;
    Que_WriteDataUnit( que, (const u8*)(&p), sizeof(PRINT_JOB*), sizeof(PRINT_JOB*) );
  }

  if( que->runningJob != NULL )
  {
    PRINT_JOB *job, *next;
    u16 pos;

    job = que->runningJob;
    while(1)
    {
      next = Que_ReadNextJobAdrs( que, job );
      if( next == NULL )
      {
        break;
      }
      job = next;
    }
    pos = Que_AdrsToBufPos( que, job );
    pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB) );
    Que_WriteData( que, (const u8*)(&store_adrs), pos, sizeof(store_adrs) );
  }
  else
  {
    que->runningJob = store_adrs;
    que->sp = sp;
  }
}
//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�̃����O�o�b�t�@�Ƀf�[�^���������݁A
 * ����ɉ����ē����̏������݃|�C���^���ړ�������B
 *
 * @param   que       [io] �v�����g�L���[ �i�����������݃|�C���^���ړ�����j
 * @param   src       [in] �������ރf�[�^�|�C���^
 * @param   totalSize   [in] �������݃f�[�^���T�C�Y
 * @param   unitSize    [in] �A�h���X���f�����ɏ������ޒP��
 *
 * @retval  void*     �i�[�擪�A�h���X
 */
//--------------------------------------------------------------------------
static void* Que_WriteDataUnit( PRINT_QUE* que, const u8* src, u16 totalSize, u16 unitSize )
{
  u16 wroteSize = 0;
  void* topAdrs = &(que->buf[ que->bufTopPos ]);

  GF_ASSERT((que->bufTopPos%4)==0);

  while( wroteSize < totalSize )
  {
    que->bufTopPos = Que_WriteData( que, src, que->bufTopPos, unitSize );
    src += unitSize;
    wroteSize += unitSize;
  }

  GF_ASSERT((que->bufTopPos%4)==0);

  return topAdrs;
}
//--------------------------------------------------------------------------
/**
 * �w�肳�ꂽJob�̎���Job�A�h���X��Ԃ�
 *
 * @param   que   [in] �v�����g�L���[
 * @param   job   [in] �v�����g�L���[�̃o�b�t�@�Ɋi�[����Ă��� Job �A�h���X
 *
 * @retval  PRINT_JOB   ����Job�A�h���X�i�������NULL�j
 */
//--------------------------------------------------------------------------
static PRINT_JOB* Que_ReadNextJobAdrs( const PRINT_QUE* que, const PRINT_JOB* job )
{
  u32 bufPos = Que_AdrsToBufPos( que, job );

  bufPos = Que_FwdBufPos( que, bufPos, sizeof(PRINT_JOB) );

  {
    PRINT_JOB** ppjob = (PRINT_JOB**)Que_BufPosToAdrs( que, bufPos );
    return *ppjob;
  }
}


//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�̃����O�o�b�t�@�Ƀf�[�^����������
 *
 * @param   que   [out] �v�����g�L���[
 * @param   src   [in]  �������݃f�[�^�|�C���^
 * @param   pos   [in]  �o�b�t�@�������݃|�C���^
 * @param   size  [in]  �������݃f�[�^�T�C�Y
 *
 * @retval  u16   ���ɏ������ނƂ��̃o�b�t�@�������݃|�C���^
 */
//--------------------------------------------------------------------------
static inline u16 Que_WriteData( PRINT_QUE* que, const u8* src, u16 pos, u16 size )
{
  GFL_STD_MemCopy( src, &(que->buf[pos]), size );
  pos += size;
  if( pos >= que->bufSize )
  {
    pos = 0;
  }
  return pos;
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@���̔C�ӂ̃A�h���X���A�������݃|�C���^�ɕϊ�
 *
 * @param   que       [in] �v�����g�L���[
 * @param   bufAdrs     [in] �����O�o�b�t�@���̔C�ӂ̃A�h���X
 *
 * @retval  u16       �������݃|�C���^
 */
//--------------------------------------------------------------------------
static inline u16 Que_AdrsToBufPos( const PRINT_QUE* que, const void* bufAdrs )
{
  u32 pos = ((u32)bufAdrs) - ((u32)que->buf);
  GF_ASSERT(pos < que->bufSize);
  return pos;
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@�̏������݃|�C���^���A�h���X�ɕϊ�
 *
 * @param   que       [in] �v�����g�L���[
 * @param   bufPos      [in] �����O�o�b�t�@�������݃|�C���^
 *
 * @retval  void*     �A�h���X
 */
//--------------------------------------------------------------------------
static inline const void* Que_BufPosToAdrs( const PRINT_QUE* que, u16 bufPos )
{
  return &(que->buf[bufPos]);
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@�������݃|�C���^���A�w��T�C�Y���i�߂��l�ɕϊ�
 *
 * @param   que     �v�����g�L���[
 * @param   pos     �������݃|�C���^�����l
 * @param   fwdSize   �i�߂�T�C�Y�i�o�C�g�P�ʁj
 *
 * @retval  pos �� fwdSize �������i�߂��l
 */
//--------------------------------------------------------------------------
static inline u16 Que_FwdBufPos( const PRINT_QUE* que, u16 pos, u16 fwdSize )
{
  pos += fwdSize;
  if( pos >= que->bufSize )
  {
    pos = 0;
  }
  return pos;
}

