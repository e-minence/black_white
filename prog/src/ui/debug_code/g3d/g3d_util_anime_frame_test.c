//=============================================================================
/**
 *
 *	@file		mcss_pos_test.c
 *	@brief  GFL_G3D_UTIL��p�����A�j���[�V�����t���[���w��e�X�g�R�[�h
 *	@author	hosaka genya
 *	@data		2009.12.18
 *
 *	<�R�[�h����>
 *	�����[�v�ĂԂ��ƂŃA�j���[�V�����t���[���𑀍�ł��܂��B
 *
 *	<�������>
 *�@  �����F�t���[���l����(1.0����)
 */
//=============================================================================

static void debug_g3d_util_anime_frame_test( GFL_G3D_UTIL* util, int unit_id )
{
  
  int j;
  int anime_count;
  GFL_G3D_OBJ* obj;
  static int frame;
  
  obj = GFL_G3D_UTIL_GetObjHandle( util, unit_id );
  anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );

  if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    frame += FX32_ONE;
    OS_TPrintf("frame=%d \n",frame>>FX32_SHIFT);
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    frame -= FX32_ONE;
    OS_TPrintf("frame=%d \n",frame>>FX32_SHIFT);
  }

  for( j=0; j<anime_count; j++ )
  {
    GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &frame );
  }
}

