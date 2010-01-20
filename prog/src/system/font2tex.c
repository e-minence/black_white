//============================================================================================
/**
 * @file  font2tex.c
 * @brief �t�H���g�f�[�^���e�N�X�`���ɓ\��t����
 * @author  satio
 * @date
 */
//============================================================================================
//#include <string.h>
//#include <wchar.h>
#include "gflib.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include "font/font.naix"

#include "print/printsys.h"
#include "print/str_tool.h"

#include "system/font2tex.h"

#define BCOL1 (0)
#define BCOL2 (0)
#define LCOL  (3)

typedef struct {
  u16           texSizIdxS;
  u16           texSizIdxT;

  NNSGfdTexKey  texVramKey;   //�e�N�X�`���u�q�`�l�L�[
  u32           texOffset;

  GFL_FONT*     fontHandle;
}F2T_SETDATA;

typedef struct {
  u16         siz;
  GXTexSizeS  s;
  GXTexSizeT  t;
}GX_TEXSIZ_TBL;


static const GX_TEXSIZ_TBL GX_texSizTbl[] = {
  { 16,   GX_TEXSIZE_S16,   GX_TEXSIZE_T16 },
  { 32,   GX_TEXSIZE_S32,   GX_TEXSIZE_T32 },
  { 64,   GX_TEXSIZE_S64 ,  GX_TEXSIZE_T64 },
  { 128,  GX_TEXSIZE_S128,  GX_TEXSIZE_T128 },
  { 256,  GX_TEXSIZE_S256,  GX_TEXSIZE_T256 },
  { 512,  GX_TEXSIZE_S512,  GX_TEXSIZE_T512 },
  { 1024, GX_TEXSIZE_S1024, GX_TEXSIZE_T1024 },
};

static void clearBitmap(GFL_BMP_DATA* bmp);
static void printStr
      (const STRBUF* str, u16 xpos, u16 ypos, GFL_BMP_DATA* bmp,
       PRINT_QUE* printQue, GFL_FONT* fontHandle);
static void convBitmap(GFL_BMP_DATA* src, GFL_BMP_DATA* dst);
static void LoadTex
      (F2T_SETDATA * setData, const STRBUF* str, u16 xpos, u16 ypos, HEAPID heapID);

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H���g��������e�N�X�`���ɓ\��t����
 *
 *	@param	
 *	@return none
 */
//-----------------------------------------------------------------------------
BOOL F2T_CopyString(
    const GFL_G3D_RES* g3Dtex, const char* tex_name, const char* plt_name,
    const STRBUF* str, u16 xpos, u16 ypos, HEAPID heapID, F2T_WORK *outWork )
{

  F2T_WORK work;
  
  work.texSizIdxS = NELEMS(GX_texSizTbl)-1; //�����ݒ�
  work.texSizIdxT = NELEMS(GX_texSizTbl)-1; //�����ݒ�
  work.texOffset = 0;
  work.plttOffset = 0;

  //binary���������炩��]����f�[�^�擾
  {
    const NNSG3dResTex*   NNSresTex = GFL_G3D_GetResTex(g3Dtex);
    work.texVramKey = NNSresTex->texInfo.vramKey;
    work.plttVramKey = NNSresTex->plttInfo.vramKey;

    //tex�f�[�^�擾
    {
      u16             offsDict = NNSresTex->texInfo.ofsDict;
      NNSG3dResDict*  NNSresDict = (NNSG3dResDict*)((u32)NNSresTex + offsDict);
      NNSG3dResName name;
      NNSG3dResDictTexData* entryTex;
      GXTexSizeS  s;
      GXTexSizeT  t;
      int i;

      // �e�N�X�`�������R�s�[
      for( i=0; i<STD_StrLen(tex_name); i++ )  name.name[i] = tex_name[i];
      for( ; i<NNS_G3D_RESNAME_SIZE; i++ ) name.name[i] = NULL;


      entryTex = NNS_G3dGetResDataByName(NNSresDict, &name);
      if(entryTex == NULL)
      {
        GF_ASSERT(0);
        return FALSE;
      }   // find name Error

      work.texOffset = (entryTex->texImageParam & 0x0000ffff) << 3;
      s = (entryTex->texImageParam & 0x00700000) >> 20;
      t = (entryTex->texImageParam & 0x03800000) >> 23;

      for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
        if(s == GX_texSizTbl[i].s){
            work.texSizIdxS = i;
            break;
        }
      }
      for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
        if(t == GX_texSizTbl[i].t){
            work.texSizIdxT = i;
            break;
        }
      }
      OS_Printf("tex key = 0x%x\n", work.texVramKey);
      OS_Printf("tex offset = 0x%x\n", work.texOffset);
      OS_Printf("tex sizIdxS  = %d\n", work.texSizIdxS);
      OS_Printf("tex sizIdxT  = %d\n", work.texSizIdxT);
    }
    //pltt�f�[�^�擾
    {
      u16             offsDict = NNSresTex->plttInfo.ofsDict;
      NNSG3dResDict*  NNSresDict = (NNSG3dResDict*)((u32)NNSresTex + offsDict);
      NNSG3dResName name;
      NNSG3dResDictPlttData* entryPltt;
      int i;

      // �p���b�g�����R�s�[
      for( i=0; i<STD_StrLen(plt_name); i++ )  name.name[i] = plt_name[i];
      for( ; i<NNS_G3D_RESNAME_SIZE; i++ ) name.name[i] = NULL;

      entryPltt = NNS_G3dGetResDataByName(NNSresDict, &name);
      if(entryPltt == NULL)
      {
        GF_ASSERT(0);
        return FALSE;
      }    // find name Error
      if(entryPltt->flag == 1)
      {
        GF_ASSERT(0);
        return FALSE;
      } // format Error

      work.plttOffset = (entryPltt->offset & 0x0000ffff) << 3;
      //OS_Printf("pltt offset = 0x%x\n", work.plttOffset);
    }
  }
  {
    //�t�H���g�n���h���쐬
    GFL_FONT* fontHandle = GFL_FONT_Create
            (ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);

    //�e�N�X�`���쐬
    {
      F2T_SETDATA setData;
      setData.texSizIdxS = work.texSizIdxS;
      setData.texSizIdxT = work.texSizIdxT;
      setData.texVramKey = work.texVramKey;
      setData.texOffset = work.texOffset;
      setData.fontHandle = fontHandle;

      LoadTex(&setData, str, xpos, ypos, heapID);
    }
    GFL_FONT_Delete(fontHandle);
  }

  if (outWork != NULL) *outWork = work;
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H���g��������e�N�X�`���ɓ\��t����@�e�N�X�`�����\�[�X���w�肵�Ȃ��ꍇ�p
 *
 *	@param	
 *	@return none
 */
//-----------------------------------------------------------------------------
BOOL F2T_CopyStringAlloc(
    const STRBUF* str, u16 xpos, u16 ypos, HEAPID heapID, F2T_WORK *outWork )
{
  F2T_WORK work;
  GFL_FONT*     fontHandle;
  //�t�H���g�n���h���쐬
  fontHandle = GFL_FONT_Create
            (ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);

  //�e�N�X�`���̈�h�m�c�d�w�擾
  {
    u32 strLen;
    int i;

    work.texSizIdxS = NELEMS(GX_texSizTbl)-1; //�����ݒ�
    work.texSizIdxT = NELEMS(GX_texSizTbl)-1; //�����ݒ�

    strLen = PRINTSYS_GetStrWidth(str, fontHandle, 0);
    for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
      if(strLen <= GX_texSizTbl[i].siz){
          work.texSizIdxS = i;
          break;
      }
    }
    strLen = PRINTSYS_GetStrHeight(str, fontHandle);
    for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
      if(strLen <= GX_texSizTbl[i].siz){
          work.texSizIdxT = i;
          break;
      }
    }
  }
  //�e�N�X�`���u�q�`�l�m��
  {
    u32 texSizS = GX_texSizTbl[work.texSizIdxS].siz;
    u32 texSizT = GX_texSizTbl[work.texSizIdxT].siz;
    u32 texVramSiz = texSizS/8 * texSizT/8 * 0x20;
    work.texVramKey = NNS_GfdAllocTexVram(texVramSiz, FALSE, 0);
    work.plttVramKey = NNS_GfdAllocPlttVram(16, FALSE, 0);
  }

  //�e�N�X�`���쐬
  {
    F2T_SETDATA setData;
    setData.texSizIdxS = work.texSizIdxS;
    setData.texSizIdxT = work.texSizIdxT;
    setData.texVramKey = work.texVramKey;
    setData.texOffset = 0;
    setData.fontHandle = fontHandle;

    LoadTex(&setData, str, xpos, ypos, heapID);
  }

  GFL_FONT_Delete(fontHandle);
  if (outWork != NULL) *outWork = work;

  return TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`���T�C�Y���擾
 *
 *	@param	
 *	@return none
 */
//-----------------------------------------------------------------------------
u16 F2T_GetTexSize(const int inIdx)
{
  return GX_texSizTbl[inIdx].siz;
}

GXTexSizeS F2T_GetTexSizeS(const int inIdx)
{
  return GX_texSizTbl[inIdx].s;
}

GXTexSizeT F2T_GetTexSizeT(const int inIdx)
{
  return GX_texSizTbl[inIdx].t;
}

// �E�C���h�E�w�i�N���A
static void clearBitmap(GFL_BMP_DATA* bmp)
{
  u8* dataAdrs = GFL_BMP_GetCharacterAdrs(bmp);
  u32 writeSize = GFL_BMP_GetBmpDataSize(bmp);
  int i;

  for( i= 0; i<writeSize; i++ ){ dataAdrs[i] = ((BCOL2 << 4) | BCOL1); }

}
// ������`��
static void printStr
      (const STRBUF* str, u16 xpos, u16 ypos, GFL_BMP_DATA* bmp,
       PRINT_QUE* printQue, GFL_FONT* fontHandle)
{
  PRINTSYS_LSB  lsb = PRINTSYS_LSB_Make(LCOL,0,0);
  clearBitmap(bmp);

  PRINTSYS_PrintQueColor(printQue, bmp, xpos, ypos, str, fontHandle, lsb);
}
// �r�b�g�}�b�v�ϊ�
static void convBitmap(GFL_BMP_DATA* src, GFL_BMP_DATA* dst)
{
  u16 size_x = GFL_BMP_GetSizeX(src)/8;           //�摜�f�[�^��Xdot�T�C�Y
  u16 size_y = GFL_BMP_GetSizeY(src)/8;           //�摜�f�[�^��Ydot�T�C�Y
  u16 col = (u16)GFL_BMP_GetColorFormat(src);     //�J���[���[�h���f�[�^�T�C�Y
  u16 size_cx = col/8;                            //�L�����N�^�f�[�^X�T�C�Y
  u8* srcAdrs = GFL_BMP_GetCharacterAdrs(src);
  u8* dstAdrs = GFL_BMP_GetCharacterAdrs(dst);
  int i, cx, cy, x, y;

  for( i=0; i<size_x * size_y * col; i++ ){
    y = i/(size_x * size_cx);
    x = i%(size_x * size_cx);
    dstAdrs[i] = srcAdrs[ ((y/8)*size_x + (x/size_cx))*col + ((y%8)*size_cx + (x%size_cx)) ];
  }
}

//------------------------------------------------------------------
static void LoadTex
      (F2T_SETDATA * setData, const STRBUF* str, u16 xpos, u16 ypos, HEAPID heapID)
{
  //�e�N�X�`���쐬
  u16           texSizS, texSizT;
  PRINT_QUE*    printQue;
  GFL_BMP_DATA* bmp;
  GFL_BMP_DATA* bmpTmp;

  //�v�����g�L���[�n���h���쐬
  printQue = PRINTSYS_QUE_Create(heapID);

  texSizS = GX_texSizTbl[setData->texSizIdxS].siz;
  texSizT = GX_texSizTbl[setData->texSizIdxT].siz;

  //�e�N�X�`���p�r�b�g�}�b�v�쐬
  bmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);
  bmpTmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);

  printStr(str, xpos, ypos, bmpTmp, printQue, setData->fontHandle);
  convBitmap(bmpTmp, bmp);
  {
    //�e�N�X�`���]��
    void* src = GFL_BMP_GetCharacterAdrs(bmp);
    u32   dst = NNS_GfdGetTexKeyAddr(setData->texVramKey) + setData->texOffset;
    u32   siz = texSizS/8 * texSizT/8 * 0x20;

    GX_BeginLoadTex();
    DC_FlushRange(src, siz);
    GX_LoadTex(src, dst, siz);
    GX_EndLoadTex();
  }

  GFL_BMP_Delete(bmpTmp);
  GFL_BMP_Delete(bmp);

  PRINTSYS_QUE_Clear(printQue);
  PRINTSYS_QUE_Delete(printQue);
}

