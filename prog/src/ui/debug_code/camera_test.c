//=============================================================================
/**
 *
 *	@file		camera_test.c
 *	@brief  カメラのテスト テンプレート
 *	@author	hosaka genya
 *	@data		2009.12.18
 *
 *	<コード説明>
 *	毎ループ呼ぶことでカメラのテストができます。コピペしてお使いください。
 *
 *	<操作説明>
 *    START：モード切り替え（Pos, CamUp, Target）
 *    SELECT:データ吐き出し
 *	  LR：z+,z-;
 *    ↑↓：y+,y-;
 *　  ←→：x+,x-;
 */
//=============================================================================


#if PM_DEBUG

static void debug_camera_test( GFL_G3D_CAMERA* camera )
{ 
  VecFx32 pos;

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

  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
  {
    pos.y += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
  {
    pos.y -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }    
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    pos.x += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    pos.x -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    pos.z += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    pos.z -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  
  // データセット
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

  // データ吐き出し
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    GFL_G3D_CAMERA_GetPos( camera, &pos );
    OS_Printf("pos { 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetCamUp( camera, &pos );
    OS_Printf("CamUp { 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetTarget( camera, &pos );
    OS_Printf("Taraget { 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
}
#endif

