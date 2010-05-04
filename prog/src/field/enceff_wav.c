//======================================================================
/*
 * @file	enceff2.c
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================
#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"


#define MUL_VAL (16)
#define Z_MUL_VAL (64)
#define POLY_W_NUM  (32)
#define POLY_H_NUM  (24)

#define VTX_W_NUM (POLY_W_NUM+1)
#define VTX_H_NUM (POLY_H_NUM+1)

#define EFF_AFTER_WAIT (6)

#define WAVE_WAIT (8)
#define WAVE_COUNT (7)
#define WAVE_AFTER_WAIT (0)
#define WAVE_HEIGHT (FX16_ONE/4)
#define WAVE_FADE_COUNT (50)

#define PALACE_WAVE_WAIT (2)
#define PALACE_WAVE_COUNT (7)
#define PALACE_WAVE_AFTER_WAIT (45)
#define PALACE_WAVE_HEIGHT (FX16_ONE/3)
#define PALACE_WAVE_FADE_COUNT  (55)


typedef struct
{
  VecFx16 Vtx[VTX_W_NUM*VTX_H_NUM];
  fx16 TmpZ[VTX_W_NUM*VTX_H_NUM];
  s16 WaveWait;
  s16 WaveCount;
  u16 WaveWaitBase;
  u16 WaveAfterWait;

  fx32 Height;
  int Fade;
  int FadeCount;
  BOOL FadeReq;
}ENCEFF_WAV_WORK;

static void DrawMesh(void *wk);
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work );
static BOOL WaveMain(void *work);
//static BOOL WaveMain2(void *work);
static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);
static GMEVENT *CreateEffMainEvt2(GAMESYS_WORK *gsys);

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   fieldWork   �t�B�[���h�}�b�v�|�C���^
 * @param   inIsFadeWhite �G�t�F�N�g�I���̓z���C�g�A�E�g���H
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_WAV_Create(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const BOOL inIsFadeWhite)
{
  ENCEFF_CNT_PTR eff_cnt_ptr;
  GMEVENT * event;
  VecFx32 pos = {0, 0, FX32_ONE*(33)};
  //�I�[�o�[���C���[�h(���_����G�t�F�N�g�I�[�o�[���C)
  eff_cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldWork);
  ENCEFF_LoadVertexEffOverlay(eff_cnt_ptr);

  event = ENCEFF_PRG_Create( gsys, &pos, CreateEffMainEvt, DrawMesh );
  
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   fieldWork   �t�B�[���h�}�b�v�|�C���^
 * @param   inIsFadeWhite �G�t�F�N�g�I���̓z���C�g�A�E�g���H
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_WAV_Create2(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const BOOL inIsFadeWhite)
{
  ENCEFF_CNT_PTR eff_cnt_ptr;
  GMEVENT * event;
  VecFx32 pos = {0, 0, FX32_ONE*(33)};
  //�I�[�o�[���C���[�h(���_����G�t�F�N�g�I�[�o�[���C)
  eff_cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldWork);
  ENCEFF_LoadVertexEffOverlay(eff_cnt_ptr);

  event = ENCEFF_PRG_CreateNoFlash( gsys, &pos, CreateEffMainEvt2, DrawMesh );
  
  return event;
}



//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  ENCEFF_WAV_WORK *work;
  ENCEFF_CNT_PTR cnt_ptr;
  int size;
  int i;
  size = sizeof(ENCEFF_WAV_WORK);
  //�C�x���g�쐬
  {
    //���[�N���m��
    {
      FIELDMAP_WORK * fieldmap;
      fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
      cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldmap);
    }

    work = ENCEFF_AllocUserWork(cnt_ptr, size, HEAPID_FLD3DCUTIN);

    event = GMEVENT_Create( gsys, NULL, EffMainEvt, 0 );

    //�g�̉񐔃Z�b�g
    work->WaveCount = WAVE_COUNT;
    work->WaveWait = 0;
    work->WaveWaitBase = WAVE_WAIT;
    work->Height = WAVE_HEIGHT;
    work->WaveAfterWait = WAVE_AFTER_WAIT;
    work->FadeCount = WAVE_FADE_COUNT;
    work->FadeReq = FALSE;

    //���_������
    for (i=0;i<VTX_H_NUM*VTX_W_NUM;i++)
    {
      VecFx16 *vec;
      vec = &work->Vtx[i];
      vec->x = ((i%VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->y = ((i/VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->z = 0;
    }
  }
  work->Fade = GFL_FADE_MASTER_BRIGHT_WHITEOUT;
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g
 *
 * @param   event       �C�x���g�|�C���^
 * @param   *seq        �V�[�P���T
 * @param   work        ���[�N�|�C���^
 *
 * @return	GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work )
{
  ENCEFF_WAV_WORK *evt_work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  ENCEFF_CNT_PTR cnt_ptr;
  
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldmap);
  evt_work = ENCEFF_GetUserWorkPtr(cnt_ptr);

  switch(*seq){
  case 0:
    {
      BOOL rc;
      if ( evt_work->FadeCount )
      {
        evt_work->FadeCount--;
      }
      else if( !evt_work->FadeReq ){
        //�t�F�[�h�A�E�g�J�n
        GFL_FADE_SetMasterBrightReq(evt_work->Fade, 0, 16, 0 ); //����ʃt�F�[�h�A�E�g
        evt_work->FadeReq = TRUE;
      }

      rc = WaveMain(evt_work);
      if (rc)
      {
        evt_work->WaveWait = EFF_AFTER_WAIT;
        (*seq)++;
      }
    }
    break;
  case 1:
    //�E�F�C�g
    if ( evt_work->WaveWait != 0)
    {
      evt_work->WaveWait--;
      break;
    }
    //�t�F�[�h�A�E�g�҂�
    if ( GFL_FADE_CheckFade() != FALSE ) break;
    return( GMEVENT_RES_FINISH );
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��
 *
 * @param   ptr     �Ǘ��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_WAV_Draw( ENCEFF_CNT_PTR ptr )
{
  void *wk = ENCEFF_GetWork(ptr);
  ENCEFF_PRG_PTR prg_ptr = (ENCEFF_PRG_PTR)wk;

  G3X_SetClearColor(GX_RGB(16,16,16),31,0x7fff,0,FALSE);
  ENCEFF_PRG_Draw(prg_ptr);

//  DrawMesh(ptr);
}

//--------------------------------------------------------------------------------------------
/**
 * �`��
 *
 * @param   ptr     �Ǘ��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawMesh(void *wk)
{
  int i,j;
  
  ENCEFF_WAV_WORK *work = (ENCEFF_WAV_WORK*)wk;

  G3_PushMtx();
  G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

  G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);

  //���C�g�J���[
  G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
  // �|���S���A�g���r���[�g�ݒ�
	G3_PolygonAttr(
				   GX_LIGHTMASK_0,			  // ���C�g�𔽉f
				   GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
				   //GX_CULL_BACK,             // �J�����O
				   GX_CULL_NONE,             // �J�����O
				   0,                         // �|���S���h�c �O
				   31,					  // �A���t�@�l
				   GX_POLYGON_ATTR_MISC_NONE );


  {
    VecFx32 pos = {-FX32_ONE*16,FX32_ONE*12,0};
    VecFx32 scale = {FX32_ONE*MUL_VAL, FX32_ONE*MUL_VAL, FX32_ONE*Z_MUL_VAL};
    fx32  s0, t0, t1;

    // �ʒu�ݒ�
		G3_Translate(pos.x, pos.y, pos.z);
    // �X�P�[���ݒ�
		G3_Scale(scale.x, scale.y, scale.z);

    for (i=0;i<POLY_H_NUM;i++)
    {
      G3_Begin( GX_BEGIN_QUAD_STRIP );
      for (j=0;j<VTX_W_NUM;j++)
      {
        fx16 x1,x2,y1,y2,z1,z2;
        

        s0 = j * 8 * FX32_ONE;
        t0 = i * 8 * FX32_ONE;
        t1 = t0 + (8 * FX32_ONE);
        
        x1 = work->Vtx[i*VTX_W_NUM+j].x;
        x2 = work->Vtx[(i+1)*VTX_W_NUM+j].x;
        y1 = work->Vtx[i*VTX_W_NUM+j].y;
        y2 = work->Vtx[(i+1)*VTX_W_NUM+j].y;
        z1 = work->Vtx[i*VTX_W_NUM+j].z;
        z2 = work->Vtx[(i+1)*VTX_W_NUM+j].z;

        G3_Normal( 0, 0, FX16_ONE );

        G3_TexCoord(s0, t0);
	      G3_Vtx( x1, -y1, z1 );
        G3_TexCoord(s0, t1);
	      G3_Vtx( x2, -y2, z2 );
      }
      G3_End();
    }
  }

  G3_PopMtx(1);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`�d����
 *
 *	@param	GFL_TCB *p_tcb
 *	@param	*p_work 
 *
 *	@return BOOL TRUE�ŏI��
 */
//-----------------------------------------------------------------------------
static BOOL WaveMain(void *work)
{
  ENCEFF_WAV_WORK *evt_work;
  int i,max;
  fx16 vtx[VTX_W_NUM*VTX_H_NUM];

  max = VTX_W_NUM*VTX_H_NUM;
  evt_work = (ENCEFF_WAV_WORK *)work;

  //���݂̒��_����ۑ�
  for(i=0;i<max;i++)
  {    
/**    
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
    {
      evt_work->Vtx[i].z = 0;
      evt_work->TmpZ[i] = evt_work->Vtx[i].z;
    }
*/    
    vtx[i] = evt_work->Vtx[i].z;
  }

  if ( evt_work->WaveCount > 0 )
  {
    if (evt_work->WaveWait == 0)
    {
      //�g�����Ă�
      evt_work->Vtx[12*33+16].z = evt_work->Height;    
      //�E�F�C�g�Z�b�g
      evt_work->WaveWait = evt_work->WaveWaitBase;
      evt_work->WaveCount--;    
    }else evt_work->WaveWait--;

    if (evt_work->WaveCount<=0)
    {
      if ( evt_work->WaveAfterWait == 0 )
      {
        evt_work->WaveWait = EFF_AFTER_WAIT;
        return TRUE;
      }
      else evt_work->WaveWait = evt_work->WaveAfterWait;
    }
  }
  else
  {
    evt_work->WaveWait--;
    if ( evt_work->WaveWait <=0 )
    {
      evt_work->WaveWait = EFF_AFTER_WAIT;
      return TRUE;
    }
  }

  //�S�Ă̒��_�Ɍv�Z��������
  for(i=0;i<max;i++)
  {
    fx16 v1,v2,v3,v4;
    int idx;
    int base_w,base_h;
    int w,h;
		//2�����W�ɕϊ�
		base_w = i%VTX_W_NUM;
		base_h = i/VTX_W_NUM;

    //����4�̃f�[�^���擾

    //��
    if (base_w-1>=0)
    {
      w = base_w-1;
      h = base_h;
      idx = h*VTX_W_NUM+w;
      v1 = vtx[idx];
    }
    else v1 = 0;
    //�E
		if (base_w+1<VTX_W_NUM)
    {
      w = base_w+1;
      h = base_h;
      idx = h*VTX_W_NUM+w;
      v2 = vtx[idx];
    }
    else v2 = 0;
		//��
		if (base_h-1>=0)
    {
      h = base_h-1;
      w = base_w;
      idx = h*VTX_W_NUM+w;
      v3 = vtx[idx];
    }
    else v3 = 0;
		//��
		if(base_h+1<VTX_H_NUM)
    {
      h = base_h+1;
      w = base_w;
      idx = h*VTX_W_NUM+w;
      v4 = vtx[idx];
    }
    else v4 = 0;

    evt_work->TmpZ[i] += ( (v1+v2+v3+v4)/4 - evt_work->Vtx[i].z );

    evt_work->Vtx[i].z += evt_work->TmpZ[i];
    if ( evt_work->Vtx[i].z > FX16_ONE ) evt_work->Vtx[i].z = FX16_ONE;
    if ( evt_work->Vtx[i].z < -FX16_ONE ) evt_work->Vtx[i].z = -FX16_ONE;

    //���͓`�d�Ώۂ���͂��������i��ʊO�������Ȃ����邽�߁j
    {
      int base_w,base_h;
      //2�����W�ɕϊ�
      base_w = i%VTX_W_NUM;
      base_h = i/VTX_W_NUM;

      if ( (base_w == 0) || (base_w == VTX_W_NUM-1) ||
          (base_h == 0) || (base_h == VTX_H_NUM-1))
      {
        evt_work->Vtx[i].z = 0;
      }
    }
  }

  return FALSE;

}

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
static GMEVENT *CreateEffMainEvt2(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  ENCEFF_WAV_WORK *work;
  ENCEFF_CNT_PTR cnt_ptr;
  int size;
  int i;
  size = sizeof(ENCEFF_WAV_WORK);
  //�C�x���g�쐬
  {
    //���[�N���m��
    {
      FIELDMAP_WORK * fieldmap;
      fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
      cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldmap);
    }

    work = ENCEFF_AllocUserWork(cnt_ptr, size, HEAPID_FLD3DCUTIN);

    event = GMEVENT_Create( gsys, NULL, EffMainEvt, 0 );

    //�g�̉񐔃Z�b�g
    work->WaveCount = PALACE_WAVE_COUNT;
    work->WaveWait = 0;
    work->WaveWaitBase = PALACE_WAVE_WAIT;
    work->Height = PALACE_WAVE_HEIGHT;
    work->WaveAfterWait = PALACE_WAVE_AFTER_WAIT;
    work->FadeCount = PALACE_WAVE_FADE_COUNT;
    work->FadeReq = FALSE;
    
    //���_������
    for (i=0;i<VTX_H_NUM*VTX_W_NUM;i++)
    {
      VecFx16 *vec;
      vec = &work->Vtx[i];
      vec->x = ((i%VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->y = ((i/VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->z = 0;
    }
  }
  work->Fade = GFL_FADE_MASTER_BRIGHT_BLACKOUT;
  return event;
}
