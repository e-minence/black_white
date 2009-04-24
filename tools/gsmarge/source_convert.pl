#!/usr/bin/perl
###############################################################################
#
#	@file	source_convert.pl
#	@brif	������̃\�[�X���֐�������u������WB�p�ɃR���o�[�g����
#	@date	090415
#	@auther toru_nagihashi
#	
#	�@�\	�֐����A�\���̖��A��`������u������
#			�����̏��Ԃ�ύX�\
#
###############################################################################


#------------------------------------------------------------------------------
#		
#					�萔
#
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#		
#					�R���o�[�g�f�[�^
#
#------------------------------------------------------------------------------
$CONVERT_TAG	= "/*��[GS_CONVERT_TAG]*/\n";

#���L�z��ŁA[1]�ɓ����l
#���̒l�������ƁA�u�������͎蓮�ōs�����̂Ƃ����w��ɂȂ�B
#�܂��A���̉ӏ��̓R�����g�A�E�g����A
$REPLACE_SELF	= "CONVERT_REPLACE_SELF";

#GS����WB��u�������������	[0]replace_src [1]replace_dst [2] �����̕��я�...�i���̂܂܏ꍇ��[2]��艽������Ȃ�
#
#	��
#	@GS_TO_WB_REPLACE	= (
#	[ "SRC", "DST", "1", "2", "3" ],
#	#->������SRC��DST�ɕύX���ASRC�̈�����1,2,3�̏��Ԃɒu�������܂�
#	#�i�����@SRC( a, b, c ) �Ȃ�΁@DST( a, b, c )�ɂȂ�܂��B
#	#�����@SRC( a, b, c, d )�Ȃ�΁ADST( a, b, c )�ɂȂ�܂��B
#	#������SRC�̈��������낤�ƁA1,2,3�Ƃ���������Ă��Ȃ��̂ŁA3�����u���������܂���j
#
#	[ "SRC2", "DST2", "3", "1" ],
#	#������SRC2��DST2�ɕύX���ASRC�̈�����3,1�̏��Ԃɒu�������܂�
#	#�i�����@SRC2( a, b, c ) �Ȃ�΁@DST2( c, a )�ɂȂ�܂��B
#	#�����@SRC2( a, b, c, d )�Ȃ�΁ADST2( c, a )�ɂȂ�܂��B
#
#	[ "SRC3", "SRC3" ],
#	#������SRC3�͉����u�������܂���B
#	#�����͂��̂܂܂ł��B
#
#	[ "SRC3", $REPLACE_SELF ],
#	#������SRC3�͉����u�������܂���B�ς��ɁA
#	#�v�����v�g��Ɏ蓮�Œu�������Ă��������̃��b�Z�[�W������܂��B
#	);
@GS_TO_WB_REPLACE	= (
	#�t�@�C����
	[ "fieldobj_code.h", "fldmmdl_code.h" ],
	#�C���N���[�h
	[ "#include \"common.h\"", "#include <gflib.h>" ],
	[ "#include \"system/procsys.h\"", "//#include \"system/procsys.h\"" ],			#SYSTEM
	[ "#include \"system/arc_tool.h\"", "//#include \"system/arc_tool.h\"" ],
	[ "#include \"system/arc_util.h\"", "//#include \"system/arc_util.h\"" ],
	[ "#include \"system/clact_tool.h\"", "//#include \"system/clact_tool.h\"" ],
	[ "#include \"system/msgdata_util.h\"", "//#include \"system/msgdata_util.h\"" ],
	[ "#include \"system/lib_pack.h\"", "//#include \"system/lib_pack.h\"" ],
	["GF_BGL_BglIniAlloc","GFL_BG_Init"],
	["GF_BGL_HeapIDGet","GFL_BG_GetHeapID",0],
	["GF_BGL_InitBG","GFL_BG_SetBGMode"],
	["GF_BGL_InitBGDisp","GFL_BG_SetBGModeDisp"],
	["GF_BGL_BGControlSet","GFL_BG_SetBGControl",2,3,4],
	["GF_BGL_BGControlReset","GFL_BG_ResetBGControl",2,3,4],
	["GF_BGL_BGControlExit","GFL_BG_FreeBGControl",2],
	["GF_BGL_PrioritySet","GFL_BG_SetPriority"],
	["GF_BGL_VisibleSet","GFL_BG_SetVisible"],
	["GF_BGL_ScrollSet","GFL_BG_SetScroll",2,3,4],
	["GF_BGL_ScrollGetX","GFL_BG_GetScrollX",2],
	["GF_BGL_ScrollGetY","GFL_BG_GetScrollY",2],
	["GF_BGL_AffineScrollSet","GFL_BG_SetAffineScroll",2,3,4,5,6,7],
	["GF_BGL_AffineSet","GFL_BG_SetAffine",2,3,4,5],
	["GF_BGL_DataDecord","GFL_BG_DecodeData"],
	["GF_BGL_LoadScreenReq","GFL_BG_LoadScreenReq",2],
	["GF_BGL_LoadScreen","GFL_BG_LoadScreen",2,3,4,5],
	["GF_BGL_ScreenBufSet","GFL_BG_LoadScreenBuffer",2,3,4],
	["GF_BGL_LoadScreenFile",$REPLACE_SELF],
	["GF_BGL_LoadCharacterFile",$REPLACE_SELF],
	["GF_BGL_LoadCharacter","GFL_BG_LoadCharacter",2,3,4,5],
	["GF_BGL_ClearCharSet","GFL_BG_SetClearCharacter"],
	["GF_BGL_CharFill","GFL_BG_FillCharacter",2,3,4,5],
	["GF_BGL_PaletteSet","GFL_BG_LoadPalette"],
	["GF_BGL_BackGroundColorSet","GFL_BG_SetBackGroundColor"],
	["GF_BGL_ScrWrite","GFL_BG_WriteScreen",2,3,4,5,6,7],
	["GF_BGL_ScrWriteExpand","GFL_BG_WriteScreenExpand",2,3,4,5,6,7,8,9,10,11],
	["GF_BGL_ScrWriteFree","GFL_BG_WriteScreenFree",2,3,4,5,6,7,8,9,10,11],
	["GF_BGL_ScrFill","GFL_BG_FillScreen",2,3,4,5,6,7,8],
	["GF_BGL_ScrPalChange","GFL_BG_ChangeScreenPalette",2,3,4,5,6,7],
	["GF_BGL_ScrClear","GFL_BG_ClearScreen",2],
	["GF_BGL_ScrClearCode","GFL_BG_ClearScreenCode",2,3],
	["GF_BGL_ScrClearCodeVReq","GFL_BG_ClearScreenCodeVReq",2,3],
	["GF_BGL_CgxGet","GFL_BG_GetCharacterAdrs"],
	["GF_BGL_4BitCgxChange8BitMain","GFL_BG_ChangeCharacter4BitTo8Bit"],
	["GF_BGL_4BitCgxChange8Bit","GFL_BG_ChangeCharacter4BitTo8BitAlloc"],
	["GF_BGL_ScreenAdrsGet","GFL_BG_GetScreenBufferAdrs",2],
	["GF_BGL_ScreenSizGet","GFL_BG_GetScreenBufferSize",2],
	["GF_BGL_ScreenTypeGet","GFL_BG_GetScreenType",2],
	["GF_BGL_ScreenScrollXGet","GFL_BG_GetScreenScrollX",2],
	["GF_BGL_ScreenScrollYGet","GFL_BG_GetScreenScrollY",2],
	["GF_BGL_ScreenColorModeGet","GFL_BG_GetScreenColorMode",2],
	["GF_BGL_RadianGet","GFL_BG_GetRadian",2],
	["GF_BGL_ScaleXGet","GFL_BG_GetScaleX",2],
	["GF_BGL_ScaleYGet","GFL_BG_GetScaleY",2],
	["GF_BGL_CenterXGet","GFL_BG_GetCenterX",2],
	["GF_BGL_CenterYGet","GFL_BG_GetCenterY",2],
	["GF_BGL_PriorityGet","GFL_BG_GetPriority",2],
	["GF_BGL_NTRCHR_CharLoad",$REPLACE_SELF],
	["GF_BGL_NTRCHR_CharLoadEx",$REPLACE_SELF],
	["GF_BGL_NTRCHR_CharGet",$REPLACE_SELF],
	["GF_BGL_NTRCHR_PalLoad",$REPLACE_SELF],
	["GF_BGL_NTRCHR_ScrnLoad",$REPLACE_SELF],
	["GF_BGL_VBlankFunc","GFL_BG_VBlankFunc",0],
	["GF_BGL_LoadScreenV_Req","GFL_BG_LoadScreenV_Req",2],
	["GF_BGL_ScrollReq","GFL_BG_SetScrollReq",2,3,4],
	["GF_BGL_RadianSetReq","GFL_BG_SetRadianReq",2,3,4],
	["GF_BGL_ScaleSetReq","GFL_BG_SetScaleReq",2,3,4],
	["GF_BGL_RotateCenterSetReq","GFL_BG_SetRotateCenterReq",2,3,4],
	["GF_BGL_DotCheck","GFL_BG_CheckDot",2,3,4,5],
	["sys_InitHeapSystem","GFL_HEAP_Init"],
	["sys_CreateHeap","GFL_HEAP_CreateHeap"],
	["sys_CreateHeapLo","GFL_HEAP_CreateHeapLo"],
	["sys_DeleteHeap","GFL_HEAP_DeleteHeap"],
	["sys_AllocMemory","GFL_HEAP_AllocMemory"],
	["sys_AllocMemoryLo","GFL_HEAP_AllocMemoryLo"],
	["sys_FreeMemoryEz","GFL_HEAP_FreeMemory",2],
	["sys_FreeMemory","GFL_HEAP_FreeMemory"],
	["sys_InitAllocator","GFL_HEAP_InitAllocator"],
	["sys_CutMemoryBlockSize","GFL_HEAP_ResizeMemory"],
	["sys_GetHeapFreeSize","GFL_HEAP_GetHeapFreeSize"],
	["sys_GetHeapAllocatedSize",$REPLACE_SELF],
	["sys_CheckHeapSafe","GFL_HEAP_CheckHeapSafe"],
	["sys_PrintHeapFreeSize",$REPLACE_SELF],
	["sys_PrintHeapExistMemoryInfo",$REPLACE_SELF],
	["sys_GetHeapState",$REPLACE_SELF],
	["sys_PrintHeapConflict",$REPLACE_SELF],
	["sys_GetMemoryBlockSize",$REPLACE_SELF],
	["sys_CheckHeapFullReleased",$REPLACE_SELF],
	["PROC","GFL_PROC"],
	["PROC_Create",$REPLACE_SELF],
	["PROC_CreateChild",$REPLACE_SELF],
	["PROC_GetParentWork",$REPLACE_SELF],
	["PROC_SetPause",$REPLACE_SELF],
	["InitTPSystem","GFL_UI_TP_Init",0],
	["InitTP",$REPLACE_SELF],
	["InitTPNoBuff",$REPLACE_SELF],
	["MainTP","GFL_UI_TP_Main",0],
	["StopTP","GFL_UI_TP_Exit"],
	["RECT_HIT_TBL","GFL_UI_TP_HITTBL"],
	["TP_HIT_TBL","GFL_UI_TP_HITTBL"],
	["TP_HIT_END","GFL_UI_TP_HIT_END"],
	["RECT_HIT_END","GFL_UI_TP_HIT_END"],
	["TP_USE_CIRCLE","GFL_UI_TP_USE_CIRCLE"],
	["TP_HIT_NONE","GFL_UI_TP_HIT_NONE"],
	["RECT_HIT_NONE","GFL_UI_TP_HIT_NONE"],
	["GF_TP_RectHitCont","GFL_UI_TP_HitCont"],
	["GF_TP_HitCont","GFL_UI_TP_HitCont"],
	["GF_TP_RectHitTrg","GFL_UI_TP_HitTrg"],
	["GF_TP_HitTrg","GFL_UI_TP_HitTrg"],
	["GF_TP_SingleHitCont",$REPLACE_SELF],
	["GF_TP_SingleHitTrg",$REPLACE_SELF],
	["GF_TP_GetCont","GFL_UI_TP_GetCont"],
	["GF_TP_GetTrg","GFL_UI_TP_GetTrg"],
	["GF_TP_RectHitContSelf",$REPLACE_SELF],
	["GF_TP_HitSelf","GFL_UI_TP_HitSelf"],
	["GF_TP_GetPointCont","GFL_UI_TP_GetPointCont"],
	["GF_TP_GetPointTrg","GFL_UI_TP_GetPointTrg"],
	["sys.trg","GFL_UI_KEY_GetTrg()"],
	["sys.cont","GFL_UI_KEY_GetCont()"],
	["sys.repeat","GFL_UI_KEY_GetRepeat()"],
	["TCBSYS","GFL_TCBSYS"],
	["TCB_PTR","GFL_TCB*"],
	["TCBSYS_CalcSystemWorkSize","GFL_TCB_CalcSystemWorkSize"],
	["TCBSYS_Create","GFL_TCB_Init"],
	["TCBSYS_Main","GFL_TCB_Main"],
	["TCBSYS_AddTask","GFL_TCB_AddTask"],
	["TCBSYS_DeleteTask","GFL_TCB_DeleteTask"],
	["TCB_ChangeFunc","GFL_TCB_ChangeFunc"],
	["TCB_GetWork","GFL_TCB_GetWork"],
	["TCB_GetPriority","GFL_TCB_GetPriority"],
	["DebugTCBSys_FuncEntryCheck",$REPLACE_SELF],
	["memset","GFL_STD_MemFill"],
	["memcpy","GFL_STD_MemCopy"],
	["memcmp","GFL_STD_MemComp"],
	["gf_srand","GFUser_GetPublicRand"],
	["gf_rand","GFUser_GetPublicRand(GFUSER_RAND_PAST_MAX)"],
	["ArcUtil_BgCharSet","GFL_ARC_UTIL_TransVramBgCharacter"],
	["ArcUtil_ObjCharSet","GFL_ARC_UTIL_TransVramObjCharacter"],
	["ArcUtil_ScrnSet","GFL_ARC_UTIL_TransVramScreen",1,2,4,5,6,7,8],
	["ArcUtil_PalSet","GFL_ARC_UTIL_TransVramPalette"],
	["ArcUtil_PalSetEx","GFL_ARC_UTIL_TransVramPaletteEx"],
	["ArcUtil_PalSysLoad","GFL_ARC_UTIL_TransVramPaletteMakeProxy"],
	["ArcUtil_CharSysLoad","GFL_ARC_UTIL_TransVramCharacterMakeProxy"],
	["ArcUtil_CharSysLoadSyncroMappingMode","GFL_ARC_UTIL_TransVramCharacterMakeProxySyncroMappingMode"],
	["ArcUtil_TransTypeCharSysLoad","GFL_ARC_UTIL_LoadTransTypeCharacterMakeProxy"],
	["ArcUtil_CharDataGet","GFL_ARC_UTIL_LoadBGCharacter or GFL_ARC_UTIL_LoadOBJCharacter"],
	["ArcUtil_ScrnDataGet","GFL_ARC_UTIL_LoadScreen"],
	["ArcUtil_PalDataGet","GFL_ARC_UTIL_LoadPalette"],
	["ArcUtil_CellBankDataGet","GFL_ARC_UTIL_LoadCellBank"],
	["ArcUtil_AnimBankDataGet","GFL_ARC_UTIL_LoadAnimeBank"],
	["ArcUtil_UnCompress","GFL_ARC_UTIL_LoadUnCompress"],
	["ArcUtil_Load","GFL_ARC_UTIL_Load"],
	["ArcUtil_LoadEx","GFL_ARC_UTIL_LoadEx"],
	["ArcUtil_HDL_BgCharSet","GFL_ARCHDL_UTIL_TransVramBgCharacter"],
	["ArcUtil_HDL_ObjCharSet","GFL_ARCHDL_UTIL_TransVramObjCharacter"],
	["ArcUtil_HDL_ScrnSet","GFL_ARCHDL_UTIL_TransVramScreen",1,2,4,5,6,7,8],
	["ArcUtil_HDL_PalSet","GFL_ARCHDL_UTIL_TransVramPalette"],
	["ArcUtil_HDL_PalSetEx","GFL_ARCHDL_UTIL_TransVramPaletteEx"],
	["ArcUtil_HDL_PalSysLoad","GFL_ARCHDL_UTIL_TransVramPaletteMakeProxy"],
	["ArcUtil_HDL_CharSysLoad","GFL_ARCHDL_UTIL_TransVramCharacterMakeProxy"],
	["ArcUtil_HDL_CharSysLoadSyncroMappingMode","GFL_ARCHDL_UTIL_TransVramCharacterMakeProxySyncroMappingMode"],
	["ArcUtil_HDL_TransTypeCharSysLoad","GFL_ARCHDL_UTIL_LoadTransTypeCharacterMakeProxy"],
	["ArcUtil_HDL_CharDataGet","GFL_ARC_UTIL_LoadBGCharacter or GFL_ARC_UTIL_LoadOBJCharacter"],
	["ArcUtil_HDL_ScrnDataGet","GFL_ARCHDL_UTIL_LoadScreen"],
	["ArcUtil_HDL_PalDataGet","GFL_ARCHDL_UTIL_LoadPalette"],
	["ArcUtil_HDL_CellBankDataGet","GFL_ARCHDL_UTIL_LoadCellBank"],
	["ArcUtil_HDL_AnimBankDataGet","GFL_ARCHDL_UTIL_LoadAnimeBank"],
	["ArcUtil_HDL_UnCompress","GFL_ARCHDL_UTIL_LoadUnCompress"],
	["ArcUtil_HDL_Load","GFL_ARCHDL_UTIL_Load"],
	["ArcUtil_HDL_LoadEx","GFL_ARCHDL_UTIL_LoadEx"],
	["ArchiveDataLoad",$REPLACE_SELF],
	["ArchiveDataLoadMalloc",$REPLACE_SELF],
	["ArchiveDataLoadMallocLo",$REPLACE_SELF],
	["ArchiveDataLoadOfs",$REPLACE_SELF],
	["ArchiveDataLoadMallocOfs",$REPLACE_SELF],
	["ArchiveDataLoadMallocOfsLo",$REPLACE_SELF],
	["ArchiveDataFileCntGet",$REPLACE_SELF],
	["ArchiveDataSizeGet",$REPLACE_SELF],
	["ArchiveDataHandleOpen","GFL_ARC_OpenDataHandle"],
	["ArchiveDataHandleClose","GFL_ARC_CloseDataHandle"],
	["ArchiveDataLoadByHandle","GFL_ARC_LoadDataByHandle"],
	["ArchiveDataSizeGetByHandle","GFL_ARC_GetDataSizeByHandle"],
	["ArchiveDataLoadOfsByHandle","GFL_ARC_LoadDataOfsByHandle"],
	["ArchiveDataLoadOfsOnlyByHandle",$REPLACE_SELF],
	["ArchiveDataLoadAllocByHandle","GFL_ARC_LoadDataAllocByHandle"],
	["ArchiveDataLoadImgofsByHandle","GFL_ARC_LoadDataImgofsByHandle"],
	["ArchiveDataLoadByHandleContinue","GFL_ARC_LoadDataByHandleContinue"],
	["ArchiveDataSeekByHandle","GFL_ARC_SeekDataByHandle"],
	["ArchiveDataFileCntGetByHandle","GFL_ARC_GetDataFileCntByHandle"],
	["GF_BGL_DISPVRAM","GFL_DISP_VRAM"],
	["GF_Disp_SetBank","GFL_DISP_SetBank"],
	["GF_Disp_GX_VisibleControlInit","GFL_DISP_GX_InitVisibleControl"],
	["GF_Disp_GXS_VisibleControlInit","GFL_DISP_GXS_InitVisibleControl"],
	["GF_Disp_GX_VisibleControl","GFL_DISP_GX_SetVisibleControl"],
	["GF_Disp_GXS_VisibleControl","GFL_DISP_GXS_SetVisibleControl"],
	["GF_Disp_GX_VisibleControlDirect","GFL_DISP_GX_SetVisibleControlDirect"],
	["GF_Disp_GXS_VisibleControlDirect","GFL_DISP_GXS_SetVisibleControlDirect"],
	["GF_Disp_DispOn","GFL_DISP_SetDispOn"],
	["GF_Disp_DispOff","GFL_DISP_SetDispOff"],
	["GF_Disp_DispSelect","GFL_DISP_SetDispSelect"],
	["GF_Disp_MainVisibleGet","GFL_DISP_GetMainVisible"],
	["GF_Disp_SubVisibleGet","GFL_DISP_GetSubVisible"],
	["CATS_SYS_PTR",$REPLACE_SELF],
	["CATS_ObjectAffineSet","GFL_CLACT_WK_SetAffineParam"],
	["CATS_ObjectAffineSetCap","GFL_CLACT_WK_SetAffineParam"],
	["CATS_ObjectAffineGet","GFL_CLACTWkGetAffineParam"],
	["CATS_ObjectAffineGetCap","GFL_CLACTWkGetAffineParam"],
	["CATS_ObjectScaleSet","GFL_CLACT_WK_SetScale"],
	["CATS_ObjectScaleSetCap","GFL_CLACT_WK_SetScale"],
	["CATS_ObjectScaleAdd",$REPLACE_SELF],
	["CATS_ObjectScaleAddCap",$REPLACE_SELF],
	["CATS_ObjectScaleGet","GFL_CLACT_WK_GetScale"],
	["CATS_ObjectScaleGetCap","GFL_CLACT_WK_GetScale"],
	["CATS_ObjectRotationSet","GFL_CLACT_WK_SetRotation"],
	["CATS_ObjectRotationSetCap","GFL_CLACT_WK_SetRotation"],
	["CATS_ObjectRotationAdd",$REPLACE_SELF],
	["CATS_ObjectRotationAddCap",$REPLACE_SELF],
	["CATS_ObjectRotationGet","GFL_CLACT_WK_GetRotation"],
	["CATS_ObjectRotationGetCap","GFL_CLACT_WK_GetRotation"],
	["CATS_ObjectFlipSet","GFL_CLACT_WK_SetFlip"],
	["CATS_ObjectFlipSetCap","GFL_CLACT_WK_SetFlip"],
	["CATS_ObjectFlipGet","GFL_CLACT_WK_GetFlip"],
	["CATS_ObjectFlipGetCap","GFL_CLACT_WK_GetFlip"],
	["CATS_ObjectAffinePosSetCap","GFL_CLACT_WK_SetAffinePos"],
	["CATS_ObjectAffinePosGetCap","GFL_CLACT_WK_GetAffinePos"],
	["CATS_ObjectAffinePosMoveCap",$REPLACE_SELF],
	["CATS_ObjectMosaicSet","GFL_CLACT_WK_SetMosaic"],
	["CATS_ObjectObjModeSet","GFL_CLACT_WK_SetObjMode"],
	["CATS_ObjectObjModeSetCap","GFL_CLACT_WK_SetObjMode"],
	["CATS_ObjectObjModeGet","GFL_CLACT_WK_GetObjMode"],
	["CATS_ObjectObjModeGetCap","GFL_CLACT_WK_GetObjMode"],
	["CATS_ObjectUpdate","GFL_CLACT_WK_AddAnmFrame"],
	["CATS_ObjectUpdateCap","GFL_CLACT_WK_AddAnmFrame"],
	["CATS_ObjectUpdateCapEx","GFL_CLACT_WK_AddAnmFrame"],
	["CATS_ObjectUpdateNumCap","GFL_CLACT_WK_AddAnmFrame"],
	["CATS_ObjectAnimeReStartCap","GFL_CLACT_WK_ResetAnm"],
	["CATS_ObjectAnimeSeqNumGetCap","GFL_CLACT_WK_GetAnmSeq"],
	["CATS_ObjectAnimeSeqSetCap","GFL_CLACT_WK_SetAnmSeq"],
	["CATS_ObjectAutoAnimeSet","GFL_CLACT_WK_SetAutoAnmFlag"],
	["CATS_ObjectAutoAnimeSetCap","GFL_CLACT_WK_SetAutoAnmFlag"],
	["CATS_ObjectAutoAnimeGet","GFL_CLACT_WK_GetAutoAnmFlag"],
	["CATS_ObjectAutoAnimeGetCap","GFL_CLACT_WK_GetAutoAnmFlag"],
	["CATS_ObjectAutoAnimeSpeedSet","GFL_CLACT_WK_SetAutoAnmSpeed"],
	["CATS_ObjectAutoAnimeSpeedSetCap","GFL_CLACT_WK_SetAutoAnmSpeed"],
	["CATS_ObjectAutoAnimeSpeedGet","GFL_CLACT_WK_GetAutoAnmSpeed"],
	["CATS_ObjectAutoAnimeSpeedGetCap","GFL_CLACT_WK_GetAutoAnmSpeed"],
	["CATS_ObjectAnimeFrameSet","GFL_CLACT_WK_SetAnmFrame"],
	["CATS_ObjectAnimeFrameSetCap","GFL_CLACT_WK_SetAnmFrame"],
	["CATS_ObjectAnimeFrameGet","GFL_CLACT_WK_GetAnmFrame"],
	["CATS_ObjectAnimeFrameGetCap","GFL_CLACT_WK_GetAnmFrame"],
	["CATS_ObjectAnimeActiveCheck","GFL_CLACT_WK_CheckAnmActive"],
	["CATS_ObjectAnimeActiveCheckCap","GFL_CLACT_WK_CheckAnmActive"],
	["CATS_ObjectEnable","GFL_CLACT_UNIT_SetDrawEnable"],
	["CATS_ObjectEnableCap","GFL_CLACT_UNIT_SetDrawEnable"],
	["CATS_ObjectEnableGet","GFL_CLACT_UNIT_GetDrawEnable"],
	["CATS_ObjectEnableGetCap","GFL_CLACT_UNIT_GetDrawEnable"],
	["CATS_ObjectPaletteSet",$REPLACE_SELF],
	["CATS_ObjectPaletteSetCap",$REPLACE_SELF],
	["CATS_ObjectPaletteOffsetSet","GFL_CLACT_WK_SetPlttOffs"],
	["CATS_ObjectPaletteOffsetSetCap","GFL_CLACT_WK_SetPlttOffs"],
	["CATS_ObjectPaletteOffsetGetCap","GFL_CLACT_WK_GetPlttOffs"],
	["CATS_ObjectBGPriSet","GFL_CLACT_WK_SetBgPri"],
	["CATS_ObjectBGPriSetCap","GFL_CLACT_WK_SetBgPri"],
	["CATS_ObjectBGPriGetCap","CATS_ObjectBGPriGetCap"],
	["CATS_ObjectPriSet","GFL_CLACT_WK_SetSoftPri"],
	["CATS_ObjectPriSetCap","GFL_CLACT_WK_SetSoftPri"],
	["CATS_ObjectPriGet","GFL_CLACT_WK_GetSoftPri"],
	["CATS_ObjectPriGetCap","GFL_CLACT_WK_GetSoftPri"],
	["CATS_ObjectPosSet","GFL_CLACT_WK_SetWldPos"],
	["CATS_ObjectPosSetCap","GFL_CLACT_WK_SetWldPos"],
	["CATS_ObjectPosSet_SubSurface",$REPLACE_SELF],
	["CATS_ObjectPosSetCap_SubSurface",$REPLACE_SELF],
	["CATS_ObjectPosGet","GFL_CLACT_WK_GetPos"],
	["CATS_ObjectPosGetCap","GFL_CLACT_WK_GetPos"],
	["CATS_ObjectPosGet_SubSurface",$REPLACE_SELF],
	["CATS_ObjectPosGetCap_SubSurface",$REPLACE_SELF],
	["CATS_ObjectPosMove",$REPLACE_SELF],
	["CATS_ObjectPosMoveCap",$REPLACE_SELF],
	["CATS_ObjectPosMoveCapFx32",$REPLACE_SELF],
	["CATS_ObjectPosSetCapFx32",$REPLACE_SELF],
	["CATS_ObjectPosGetCapFx32",$REPLACE_SELF],
	["CATS_ObjectPosSetCapFx32_SubSurface",$REPLACE_SELF],
	["CATS_ObjectPosGetCapFx32_SubSurface",$REPLACE_SELF],
	["CATS_ObjectUserAttrAnimSeqGet","GFL_CLACT_WK_GetUserAttrAnmSeq"],
	["CATS_ObjectUserAttrNowAnimSeqGet","GFL_CLACT_WK_GetUserAttrAnmSeqNow"],
	["CATS_ObjectUserAttrAnimFrameGet","GFL_CLACT_WK_GetUserAttrAnmFrame"],
	["CATS_ObjectUserAttrNowAnimFrameGet","GFL_CLACT_WK_GetUserAttrAnmFrameNow"],
	["CATS_ObjectUserAttrCellGet","GFL_CLACT_WK_GetUserAttrCell"],
	["CATS_LoadResourceCharArcModeAdjust",$REPLACE_SELF],
	["CATS_LoadResourceCharArcModeAdjustAreaCont",$REPLACE_SELF],
	["CATS_LoadResourceCharArcModeAdjustAreaCont_Handle",$REPLACE_SELF],
	["CATS_ChangeResourceCharArc",$REPLACE_SELF],
	["CATS_ChangeResourcePlttArc",$REPLACE_SELF],
	["CATS_GetClactSetPtr",$REPLACE_SELF],
	["CATS_SetClactSetPtr",$REPLACE_SELF],
	["CATS_PlttProxy",$REPLACE_SELF],
	["CATS_PlttID_NoGet",$REPLACE_SELF],
	["CATS_ChangeResourceCharArcH",$REPLACE_SELF],
	["CATS_ChangeResourcePlttArcH",$REPLACE_SELF],
	["CATS_ChangeResourcePlttArcH_PlttWork",$REPLACE_SELF],
	["CLACT_MosaicSet","GFL_CLACT_WK_SetMosaic"],
	["CLACT_MosaicGet","GFL_CLACT_WK_GetMosaic"],
	["REND_OAMInit","GFL_CLACT_Init"],
	["CLACT_U_SetEasyInit","GFL_CLACT_UNIT_Create"],
	["CLACT_U_MakeHeader","GFL_CLACT_WK_SetCellResData or GFL_CLACT_WK_SetTrCellResData or GFL_CLACT_WK_SetMCellResData"],
	["CLACT_Add","GFL_CLACT_WK_Add"],
	["CLACT_DestSet","GFL_CLACT_UNIT_Delete"],
	["REND_OAM_Delete","GFL_CLACT_Exit"],
	["CLACT_AnmGet","GFL_CLACT_WK_GetAnmSeq"],
	["CLACT_AnmFrameChg","GFL_CLACT_WK_AddAnmFrame"],
	["CLACT_AnmFrameSet","GFL_CLACT_WK_SetAnmFrame"],
	["CLACT_AnmFrameGet","GFL_CLACT_WK_GetAnmFrame"],
	["CLACT_SetMatrix","GFL_CLACT_WK_SetWldPos"],
	["CLACT_SetAffineMatrix","GFL_CLACT_WK_SetAffinePos"],
	["CLACT_SetScale","GFL_CLACT_WK_SetScale"],
	["CLACT_SetScaleAffine","GFL_CLACT_WK_SetScale" ,1,2],
	["CLACT_SetRotation","GFL_CLACT_WK_SetRotation"],
	["CLACT_SetRotationAffine","GFL_CLACT_WK_SetRotation",1,2],
	["CLACT_SetDrawFlag","GFL_CLACT_WK_SetDrawEnable"],
	["CLACT_GetDrawFlag","GFL_CLACT_WK_GetDrawEnable"],
	["CLACT_SetAnmFlag","GFL_CLACT_WK_SetAutoAnmFlag"],
	["CLACT_GetAnmFlag","GFL_CLACT_WK_GetAutoAnmFlag"],
	["CLACT_SetAffineParam","GFL_CLACT_WK_SetAffineParam"],
	["CLACT_SetFlip","GFL_CLACT_WK_SetFlip"],
	["CLACT_GetMatrix","GFL_CLACT_WK_GetWldPos"],
	["CLACT_GetAffineMatrix","GFL_CLACT_WK_GetWldPos"],
	["CLACT_GetScale","GFL_CLACT_WK_GetScale"],
	["CLACT_GetRotation","GFL_CLACT_WK_GetRotation"],
	["CLACT_GetAnmFrame","GFL_CLACT_WK_GetAutoAnmSpeed"],
	["CLACT_GetAffineParam","GFL_CLACT_WK_GetAffineParam"],
	["CLACT_GetFlip","GFL_CLACT_WK_GetFlip"],
	["CLACT_GetAnmSeqNum","GFL_CLACT_WK_GetAnmSeqNum"],
	["CLACT_AnmChg","GFL_CLACT_WK_SetAnmSeq"],
	["CLACT_AnmChgCheck","GFL_CLACT_WK_SetAnmSeqDiff"],
	["CLACT_AnmReStart","GFL_CLACT_WK_ResetAnm"],
	["CLACT_BGPriorityChg","GFL_CLACT_WK_SetBgPri"],
	["CLACT_BGPriorityGet","GFL_CLACT_WK_GetBgPri"],
	["CLACT_PaletteNoChg",$REPLACE_SELF],
	["CLACT_PaletteNoChgAddTransPlttNo",$REPLACE_SELF],
	["CLACT_PaletteNoGet",$REPLACE_SELF],
	["CLACT_PaletteOffsetChg","GFL_CLACT_WK_SetPlttOffs"],
	["CLACT_PaletteOffsetChgAddTransPlttNo","GFL_CLACT_WK_GetPlttOffsMode"],
	["CLACT_PaletteOffsetGet","GFL_CLACT_WK_GetPlttOffs"],
	["CLACT_DrawPriorityGet","GFL_CLACT_WK_GetSoftPri"],
	["CLACT_ImageProxySet",$REPLACE_SELF],
	["CLACT_ImageProxyGet",$REPLACE_SELF],
	["CLACT_PaletteProxySet","GFL_CLACT_WK_SetPlttProxy"],
	["CLACT_PaletteProxyGet","GFL_CLACT_WK_GetPlttProxy"],
	["CLACT_VramTypeGet",$REPLACE_SELF],
	["CLACT_AnmActiveCheck","GFL_CLACT_WK_CheckAnmActive"],
	["CLACT_ObjModeSet","GFL_CLACT_WK_SetObjMode"],
	["CLACT_ObjModeGet","GFL_CLACT_WK_GetObjMode"],
	["CLACT_DrawPriorityChg","GFL_CLACT_WK_SetSoftPri"],
	["CATS_ObjectAutoAnimeSetCap","GFL_CLACT_WK_SetAutoAnmFlag"],
	["CATS_ObjectUpdate","GFL_CLACT_WK_AddAnmFrame"],
	["CATS_ObjectObjModeSetCap","GFL_CLACT_WK_SetObjMode"],
	["CATS_Draw","GFL_CLACT_SYS_Main"],
	["gflib/fontdata_man","print/gf_font"],
	["NUBER_CODETYPE","StrNumberCodeType"],
	["GF_FONTDATA_MAN","GFL_FONT"],
	["FontDataMan_Create","GFL_FONT_Create"],
	["FontDataMan_Delete","GFL_FONT_Delete"],
	["FontDataMan_GetBitmap","GFL_FONT_GetBitMap"],
	["FontDataMan_GetStrWidth","GFL_FONT_GetWidth"],
	["FontDataMan_GetStrLineWidth",$REPLACE_SELF],
	["FontDataMan_ErrorStrCheck",$REPLACE_SELF],
	["gflib/msg_print.h","print/printsys.h"],
	["GF_PRINTCOLOR","PRINTSYS_LSB"],
	["GF_PRINTCOLOR_GET_LETTER","PRINTSYS_LSB_GetL"],
	["GF_PRINTCOLOR_GET_SHADOW","PRINTSYS_LSB_GetS"],
	["GF_PRINTCOLOR_GET_GROUND","PRINTSYS_LSB_GetB"],
	["GF_PRINTCOLOR_MAKE","PRINTSYS_LSB_Make"],
	["MSG_PrintSysInit","PRINTSYS_Init"],
	["GF_BGL_BmpWinDataFill","GFL_BMP_Clear"],
	["TalkWinGraphicSet","BmpWinFrame_GraphicSet"],
	["BMP_MENULIST_Create","BmpMenuWork_ListCreate"],
	["MenuWinGraphicSet","BmpWinFrame_GraphicSet"],
	["BmpMenuWinWrite","BmpWinFrame_Write"],
	["Particle_DrawAll","GFL_PTC_DrawAll"],
	["Particle_CalcAll","GFL_PTC_CalcAll"],
	["GF_BMP_PrintMain","GFL_BMP_Print"],
	["GF_BMP_PrintMain256","GFL_BMP_Print16to256"],
	["GF_BGL_BmpFill","GFL_BMP_Fill"],
	["GF_BGL_BmpWinAllocGet","GFL_BMPWIN_Init"],
	["GFL_BMPWIN_Create",$REPLACE_SELF],
	["GF_BGL_BmpWinDel","GFL_BMPWIN_Delete"],
	["GF_BGL_BmpWinFree",$REPLACE_SELF],
	["GF_BGL_BmpWinOn","GFL_BMPWIN_MakeTransWindow"],
	["GF_BGL_BmpWinOnVReq","GFL_BMPWIN_MakeTransWindow_VBlank"],
	["GF_BGL_BmpWinMakeScrn","GFL_BMPWIN_MakeScreen"],
	["GF_BGL_BmpWinMakeScrnLimited",$REPLACE_SELF],
	["GF_BGL_BmpWinClearScrn","GFL_BMPWIN_ClearScreen"],
	["GF_BGL_BmpWinCgxOn","GFL_BMPWIN_TransVramCharacter"],
	["GF_BGL_BmpWinGet_Frame","GFL_BMPWIN_GetFrame"],
	["GF_BGL_BmpWinGet_SizeX","GFL_BMPWIN_GetSizeX"],
	["GF_BGL_BmpWinGet_SizeY","GFL_BMPWIN_GetSizeY"],
	["GF_BGL_BmpWinGet_PosX","GFL_BMPWIN_GetPosX"],
	["GF_BGL_BmpWinGet_PosY","GFL_BMPWIN_GetPosY"],
	["GF_BGL_BmpWinGet_Chrofs","GFL_BMPWIN_GetChrNum"],
	["GF_BGL_BmpWinSet_PosX","GFL_BMPWIN_SetPosX"],
	["GF_BGL_BmpWinSet_PosY","GFL_BMPWIN_SetPosY"],
	["GF_BGL_BmpWinSet_Pal","GFL_BMPWIN_SetPalette"]


);


$CONVERT_CNT	= 0;

#------------------------------------------------------------------------------
#		
#					���C��
#
#------------------------------------------------------------------------------
#�����������`�F�b�N
if( @ARGV != 2 ){
	print "Usage: gs_convert.pl src_file_name dst_file_name \n";
	print "GSproject source and header file convert WBproject\n";
	exit(1);
}

print "\n���R���o�[�g�J�n\n";
print "�ύX��:$ARGV[0]\n";

#�R���o�[�g
&CONVERT_Execute( $ARGV[0], $ARGV[1] );

print "���R���o�[�g����\n";
print "�ύX��:$ARGV[1]\n";
print "�ύX��:$CONVERT_CNT\n\n";

#�I��
exit(0);

#------------------------------------------------------------------------------
#		
#					�T�u���[�`��
#
#------------------------------------------------------------------------------
#
#	�R���o�[�g�{��
#	@param	_[0]	�R���o�[�g���t�@�C��
#	@param	_[1]	�R���o�[�g��t�@�C��
#
#	@retval	TRUE	�R���o�[�g�I��
#	@retval	FALSE	�R���o�[�g�ł��Ȃ�����
#
sub CONVERT_Execute
{
	my( $src_file, $dst_file )	= @_;

	##�R���o�[�g���t�@�C�����當����擾
	#�t�@�C���I�[�v��
	open( FILEIN, "<$src_file" );
	#�S���̍s���擾
	@buff	= <FILEIN>;

	#��[���ׂẲ��s�𓝈ꂷ��
	for( my $tmp = 0; $tmp < @buff; $tmp++ ){
		my $tmp_word	= $buff[ $tmp ];
		$tmp_word		=~ s/\r\n/\n/g;
		$buff[ $tmp ]	= $tmp_word;
	}

	#�t�@�C���N���[�Y
	close( FILEIN );

	#1�s���Ƃ̃��[�v
	for( my $k = 0; $k < @buff; $k++ ){
		my $line	= $buff[ $k ];
		#�u������
		for( $j = 0; $j < @GS_TO_WB_REPLACE; $j++ ){
			if( $line =~ m/$GS_TO_WB_REPLACE[$j][0]/ ){
				if( $GS_TO_WB_REPLACE[ $j ][1] eq $REPLACE_SELF ){
					print "�蓮�Œu�������Ă��������B LINE= $k NAME= $GS_TO_WB_REPLACE[ $j ][0]\n";
				}else{
					$line	= &CONVERT_ReplaceName( $line, $GS_TO_WB_REPLACE[ $j ][0], $GS_TO_WB_REPLACE[ $j ][1] );
					$line .= $CONVERT_TAG;
					$CONVERT_CNT++;
				}
			}
		}
		$buff[ $k ]	= $line;
	}

	#�����u������
	my $ret = &CONVERT_RePlaceArgument( \@buff, \@GS_TO_WB_REPLACE );
	#print $ret;

	##�R���o�[�g�����������V�K�t�@�C���ɍ쐬
	#�t�@�C���I�[�v��
	open( FILEOUT, ">$dst_file" );
	#��������������
	print FILEOUT $ret;
	#�t�@�C���N���[�Y
	close( FILEOUT );
}

#	�u������
#	@param	_[0]	�u����������Ƃ̕����o�b�t�@
#	@param	_[1]	�u��������������
#	@param	_[2]	�u�������敶����
#	@return			�u�����ς݂̕����o�b�t�@
sub CONVERT_ReplaceName
{
	my( $str_buff, $src_str, $dst_str )	= @_;
	$str_buff	=~	s/$src_str/$dst_str/g;
	return $str_buff;
}

#	������z��Ŏ��o������
#	@param	_[0]	���o�������o�b�t�@	���t�@�����X
#	@param	_[1]	�R���o�[�g�e�[�u��		���t�@�����X
sub CONVERT_RePlaceArgument
{
	my( $str_buff, $convert_tbl )	= @_;
	my( @word );
	my( @temp_word );

	#	�u�������菇
	#	�P�D�S���������A�����u���������K�v�ȕ������������A�ۑ�
	#	�Q�D�u��������
	
	$str_buff_s	= join("", @$str_buff );

	##	�P�D�S������
	#�P�ꂲ�Ƃɋ�؂�A�z��ɁB�P�ꂲ�Ƃɏ�������
	@word	= split( /( |\(|\)|:|;|,|\t|\n)/, $str_buff_s );
	for( my $i = 0; $i < @word; $i++ ){
		for( my $j = 0; $j < @$convert_tbl; $j++ ){
			#�R���o�[�g�e�[�u���Ɉ����u�������������Ă��āA
			#�u�������֐��������݂�����o�b�t�@�ɕۑ�
			if( ($$convert_tbl[ $j ][2] != undef ) && ( $$convert_tbl[ $j ][0] eq $word[ $i ] ) ) {
				push( @temp_word, $i );
				#print "start  $word[$i]\n";
			}
		}
	}

	##	�Q�D�u������
	#
	foreach my $w (@temp_word ){
		$str_buff_s	= &Convert_RePlaceArgumentCore( $str_buff_s, $w, \@$convert_tbl );
	}

	return $str_buff_s;
}


#	������z��Ŏ��o������
#	@param	_[0]	���o�������o�b�t�@
#	@param	_[1]	��
#	@param	_[2]	�P��
#	@param	_[3]	�R���o�[�g�e�[�u��

sub Convert_RePlaceArgumentCore
{
	my( $str_buff, $word_num, $convert_tbl )	= @_;
	my( $word );
	my( $is_start );
	my( $brace_cnt );
	my( $argc );

	my( $is_arg_start )	= 0;
	my( $arg_start_idx );
	my( $arg_name );
	my( @arg_buff );
	my( @argument );


	#	�u��������ۂ̎菇
	#	�P�D�u�������֐����`�F�b�N�i�蓮���ǂ����j
	#	�Q�D�u(�v���݂Ĉ����J�n�`�F�b�N
	#	�R�D�u,�v�𐔂��Ĉ����̐����J�E���g�i�u(�v�����d�ł͂Ȃ��Ƃ��j
	#	�S�D�u�j�v���݂āA�����I���`�F�b�N�i�u�i�v�����d�ł͂Ȃ��Ƃ��j
	#	�T�D�P��̈ʒu��u�������B


	#�P�ꂲ�ƂɃ��[�v����
	@word	= split( /( |\(|\)|:|;|,|\t|\n)/, $str_buff );
	for( my $j = $word_num; $j < @word; $j++ ){
		$w	= $word[ $j ];
		for( $i = 0; $i < @$convert_tbl; $i++ ){
			##�P�D�J�n�`�F�b�N
			#�R���o�[�g�e�[�u���̑S�Ă̖������`�F�b�N��
			#�R���o�[�g�e�[�u���Ɉ����p�����[�^�����邩�`�F�b�N
			if( ($$convert_tbl[ $i ][0] eq $w ) &&
				($$convert_tbl[ $i ][2] != undef ) ){

				@arg_buff		= ();
				$is_start		= 1;
				#print "start\n"
			}

			if( $is_start ){
				##�Q�D�����J�n�`�F�b�N
				if( $w eq "(" ){
					$brace_cnt++;	#�u(�v�̐����ӂ₷�i2��Ȃ瑽�d�j

					if( $brace_cnt == 1 ){
						$is_arg_start	= 1;
						$arg_start_idx	= $j+1;
						last;
					}
				}

				##�R�A����������
				#�u(�v���݂āA���d���ǂ����`�F�b�N����
				if( $brace_cnt == 1 ){
					if( $w eq "," ){

						$argc++;	#�u,�v���݂Ĉ����̐��𐔂���
						push( @arg_buff, $arg_name );
						$arg_name	= "";
					}
				}


				##�S�A�����I���`�F�b�N
				if( $w eq ")" ){
					$brace_cnt--;	#�u)�v�̐������炷�i2��Ȃ瑽�d�j

					#������0�ɂȂ�΁A�I��
					if( $brace_cnt == 0 ){
						$is_arg_start	= 0;
						push( @arg_buff, $arg_name );

						#�w��ǂ��������u��������
						my $cnt	= 2;
						while( $$convert_tbl[ $i ][$cnt] != undef ){
							#�u�����������̂�$argument�ɓ����
							$argument[ $cnt-2 ]	= $arg_buff[ $$convert_tbl[ $i ][$cnt]-1 ];
							#�Ō�ȊO�J���}������
							if( $$convert_tbl[ $i ][$cnt+1] != undef ){
								$argument[ $cnt-2 ]	.= ",";
							}

							$cnt++;
						}

						#print "@argument \n";


						#�쐬����������}��
						splice( @word, $arg_start_idx, 0, @argument );

						goto LOOPEND;
					}
				}


				#�����ۑ�����
				if( $is_arg_start == 1 ){
					if( $brace_cnt == 1 ){
						if( $w ne "," ){
							$arg_name	.=	$w;
						}
					}else{
						$arg_name	.=	$w;
					}
					#print $w;
					#print $word[ $j ];
					@word	= &DeleteArray( \@word, $j );
					$j--;#�z�񂩂獡�Q�Ƃ��Ă���P����������̂ŁA���߂�
				}
			}
		}
	}
LOOPEND:

	return join( "", @word );
}


#	�z�񂩂�v�f������
#	@param	�z��
#	@param	�C���f�b�N�X
sub DeleteArray {
	my($array,$idx) = @_;
	my @new = ();
	for( my $i = 0; $i < @$array; $i++ ) {
		if($idx != $i){
			push(@new, $$array[ $i ]);
		}
	}

	return @new;
}

