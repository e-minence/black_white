@echo off

SET USERNAME=

SET HOME_TMP=%~dp0
SET PROJECT_ROOT=%HOME_TMP:\=/%
SET WBLIBDIR=%PROJECT_ROOT%lib
SET PROJECT_PROGDIR=%PROJECT_ROOT%prog/
SET PROJECT_RSCDIR=%PROJECT_ROOT%resource/
SET PROJECT_ARCDIR=%PROJECT_ROOT%prog/arc/

SET CWFolder_NITRO=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\
SET CWFOLDER_TWL=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\
SET CW_NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET CW_TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET LM_LICENSE_FILE=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\license.dat
SET NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET NITROSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET NITROWIFI_ROOT=%WBLIBDIR%\TwlWiFi
SET NITRODWC_ROOT=%WBLIBDIR%\TwlDWC

REM ####################################################
REM �n�C�u���b�h�Ή��̏ꍇ���L�̕�����L���ɂ��Ă������� �n�C�u���b�h�łȂ����͖����ɂ��Ă�������
REM NITRO�Ƃ�����IS-NITRO�p TWL�Ƃ�����TWL�p ���������Ɨ������o���オ��܂�
REM ####################################################
REM SET TWLSDK_PLATFORM=TWL
REM SET TARGET_PLATFORM=TWL

REM ####################################################
REM �n�C�u���b�h�łȂ��ꍇ���L�̕�����L���ɂ��Ă������� �n�C�u���b�h�̎��͖����ɂ��Ă�������
REM ####################################################
SET TWLSDK_PLATFORM=NITRO
SET TARGET_PLATFORM=NITRO

SET TWLSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET TWLWIFI_ROOT=%WBLIBDIR%\TwlWiFi
SET TWLDWC_ROOT=%WBLIBDIR%\TwlDWC

SET TWLVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto


SET NITROVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto

SET GFL_MAKE_DIR = %PROJECT_ROOT%../gflib/gflib/
SET GFL_PROJECT_DIR = %PROJECT_ROOT%lib/gflib/

SET NITRO_PLATFORM=TS

SET CVSROOT=:pserver:%USERNAME%@dpcvs.gamefreak.co.jp:/Groups/deocvs
SET CVSEDITOR=c:\tools\vim\gvim

SET SVNROOT=:svn://svn-wb.gamefreak.co.jp/pokemon_wb/
SET SVN_EDITOR=c:\tools\vim\gvim

SET BMDS_SDK_ROOT=%PROJECT_ROOT%lib/BMDS_SDK/


PATH=C:\tools;c:\cygwin\bin;c:\tools\vim;%WBLIBDIR%\TwlSDK\tools\bin;%WBLIBDIR%\TwlSystem\tools\bin;%WBLIBDIR%\..\..\tools;%PATH%;

REM C:
REM chdir C:\Cygwin\bin


bash --login -i -s

