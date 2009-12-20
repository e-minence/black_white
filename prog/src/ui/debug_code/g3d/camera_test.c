//=============================================================================
/**
 *
 *	@file		camera_test.c
 *	@brief  �J�����̃e�X�g �e���v���[�g
 *	@author	hosaka genya
 *	@data		2009.12.18
 *
 *	<�R�[�h����>
 *	�����[�v�ĂԂ��ƂŃJ�����̃e�X�g���ł��܂��B�R�s�y���Ă��g�����������B
 *
 *	<�������>
 *    START�F���[�h�؂�ւ��iPos, CamUp, Target�j
 *    SELECT:�f�[�^�f���o��
 *	  XY�F�ω���+,�ω���-;
 *	  LR�Fz+,z-;
 *    �����Fy+,y-;
 *�@  �����Fx+,x-;
 */
//=============================================================================


#if PM_DEBUG

#define CHECK_KEY_CONT( key ) ( (GFL_UI_KEY_GetCont() & (key) ) == (key) )

static void debug_camera_test( GFL_G3D_CAMERA* camera )
{ 
  VecFx32 pos;

  static int num = 1;
  static BOOL mode = 0;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    mode = (mode+1)%3;

    if( mode == 0 )
    {
      OS_Printf("mode=%d Pos\n",mode);
    }
    else if( mode == 1 )
    {
      OS_Printf("mode=%d CamUp\n",mode);
    }
    else
    {
      OS_Printf("mode=%d Target\n",mode);
    }
  }
  
  if( mode == 0 )
  {
    GFL_G3D_CAMERA_GetPos( camera, &pos );
  }
  else if( mode == 1 )
  {
    GFL_G3D_CAMERA_GetCamUp( camera, &pos );
  }
  else
  {
    GFL_G3D_CAMERA_GetTarget( camera, &pos );
  }

  if( CHECK_KEY_CONT( PAD_BUTTON_X ) )
  {
    num++;
    OS_Printf("num=%d \n",num);
  }
  else if( CHECK_KEY_CONT( PAD_BUTTON_Y ) )
  {
    num--;
    OS_Printf("num=%d \n",num);
  }
  else if( CHECK_KEY_CONT( PAD_KEY_UP ) )
  {
    pos.y += num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
  else if( CHECK_KEY_CONT( PAD_KEY_DOWN ) )
  {
    pos.y -= num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }    
  else if( CHECK_KEY_CONT( PAD_KEY_LEFT ) )
  {
    pos.x += num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
  else if( CHECK_KEY_CONT( PAD_KEY_RIGHT ) )
  {
    pos.x -= num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
  else if( CHECK_KEY_CONT( PAD_BUTTON_L ) )
  {
    pos.z += num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
  else if( CHECK_KEY_CONT( PAD_BUTTON_R ) )
  {
    pos.z -= num;
    OS_Printf("pos{ 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
  
  // �f�[�^�Z�b�g
  if( mode == 0 )
  {
    GFL_G3D_CAMERA_SetPos( camera, &pos );
  }
  else if( mode == 1 )
  {
    GFL_G3D_CAMERA_SetCamUp( camera, &pos );
  }
  else
  {
    GFL_G3D_CAMERA_SetTarget( camera, &pos );
  }

  // �f�[�^�f���o��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    GFL_G3D_CAMERA_GetPos( camera, &pos );
    OS_Printf("static const sc_camera_pos = { 0x%x, 0x%x, 0x%x }; \n", pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetCamUp( camera, &pos );
    OS_Printf("static const sc_camera_up =  { 0x%x, 0x%x, 0x%x }; \n", pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetTarget( camera, &pos );
    OS_Printf("static const sc_camera_target = { 0x%x, 0x%x, 0x%x }; \n", pos.x, pos.y, pos.z );
  }
}

#endif

