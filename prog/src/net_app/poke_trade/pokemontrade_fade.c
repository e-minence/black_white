//�p���b�g�t�F�[�h�֘A

//--------------------------------------------------
/**
 * @brief    �t�F�[�h����֐�
 * @param   	pD2Fade				�t�F�[�h���[�N
 */
//--------------------------------------------------
static void _setFadeMask(_D2_PAL_FADE_WORK* pD2Fade)
{
  if(!pD2Fade){
    return;
  }
  if(pD2Fade->pal_fade_time < pD2Fade->pal_fade_nowcount){
    return;
  }
  pD2Fade->pal_fade_nowcount++;
  {
    int upper = pD2Fade->pal_end - pD2Fade->pal_start;
    upper *= 4096;
    upper = upper / pD2Fade->pal_fade_time;
    upper = upper * pD2Fade->pal_fade_nowcount;
    upper /= 4096;

    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|
                           GX_BLEND_PLANEMASK_OBJ, upper);
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|
                            GX_BLEND_PLANEMASK_OBJ, upper);
  }

}

//--------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�X�^�[�g
 * @param[in]	bfw	      _EFFTOOL_PAL_FADE_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//--------------------------------------------------
static void	_FIELD_StartPaletteFade( _EFFTOOL_PAL_FADE_WORK* epfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{

  epfw->pal_fade_flag      = 1;
  epfw->pal_fade_start_evy = start_evy;
  epfw->pal_fade_end_evy   = end_evy;
  epfw->pal_fade_wait      = 0;
  epfw->pal_fade_wait_tmp  = wait;
  epfw->pal_fade_rgb       = rgb;
}


//----------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�\���̍쐬
 * @param[in]	g3DRES	  GFL_G3D_RES
 * @param[in]	heapID	  HEAPID
 * @retturn   �p���b�g�t�F�[�h�\����
 */
//----------------------------------------

static _EFFTOOL_PAL_FADE_WORK* _createPaletteFade(GFL_G3D_RES* g3DRES,HEAPID heapID)
{
  //�p���b�g�t�F�[�h�p���[�N����
  NNSG3dResFileHeader*	header = GFL_G3D_GetResourceFileHeader( g3DRES );
  NNSG3dResTex*		    	pTex = NNS_G3dGetTex( header );
  u32                   size = (u32)pTex->plttInfo.sizePltt << 3;
  _EFFTOOL_PAL_FADE_WORK* pwk = GFL_HEAP_AllocClearMemory(heapID,sizeof(_EFFTOOL_PAL_FADE_WORK));

  pwk->g3DRES = g3DRES;
  pwk->pData_dst  = GFL_HEAP_AllocClearMemory( heapID, size );
  return pwk;
}

//----------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�\���̊J��
 * @param[in]	pwk	_EFFTOOL_PAL_FADE_WORK�|�C���^
 */
//----------------------------------------
static void	_freePaletteFade( _EFFTOOL_PAL_FADE_WORK* pwk )
{
  if(pwk){
    GFL_HEAP_FreeMemory( pwk->pData_dst );
    GFL_HEAP_FreeMemory( pwk );
  }
}



//-------------------------------------------------
/**
 *	@brief      3D���f���̃p���b�g�t�F�[�h�A�j�����s����   BTLV����]�p
 *	@param[in]	epfw  �p���b�g�t�F�[�h�Ǘ��\���̂ւ̃|�C���^
 */
//-------------------------------------------------
static void  _EFFTOOL_CalcPaletteFade( _EFFTOOL_PAL_FADE_WORK *epfw )
{

  if(	(epfw == NULL) || (epfw->pal_fade_flag == 0) )
  {
    return;
  }
  if( epfw->pal_fade_wait == 0 )
  {
    NNSG3dResFileHeader*	header;
    NNSG3dResTex*		    	pTex;
    u32                   size;
    const void*           pData_src;
    u32                   from;

    epfw->pal_fade_wait = epfw->pal_fade_wait_tmp;

    {
      //�e�N�X�`�����\�[�X�|�C���^�̎擾
      header = GFL_G3D_GetResourceFileHeader( epfw->g3DRES );
      pTex = NNS_G3dGetTex( header );

      size = ( u32 )pTex->plttInfo.sizePltt << 3;
      pData_src = NNS_G3dGetPlttData( pTex );
      from = NNS_GfdGetTexKeyAddr( pTex->plttInfo.vramKey );

      SoftFade( pData_src, epfw->pData_dst, ( size / 2 ), epfw->pal_fade_start_evy, epfw->pal_fade_rgb );

      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, from, epfw->pData_dst, size );
    }
    if( epfw->pal_fade_start_evy == epfw->pal_fade_end_evy )
    {
      epfw->pal_fade_flag = 0;
    }
    else if( epfw->pal_fade_start_evy > epfw->pal_fade_end_evy )
    {
      epfw->pal_fade_start_evy--;
    }
    else
    {
      epfw->pal_fade_start_evy++;
    }
  }
  else
  {
 	  epfw->pal_fade_wait--;
  }
}


