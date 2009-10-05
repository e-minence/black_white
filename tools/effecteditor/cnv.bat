@ECHO OFF

SET LANG=C-SJIS
SET HOME_TMP=%~dp0
SET PROJECT_ROOT=c:\home\pokemon_wb\
SET PROJECT_PROGDIR=%PROJECT_ROOT%prog/
SET PROJECT_ARCDIR=%PROJECT_PROGDIR%arc/
SET PROJECT_RSCDIR=%PROJECT_ROOT%resource/
SET WBLIBDIR=C:\home\pokemon_wb\lib
SET TAGFILE=c:\home\pokemon_wb\tags

REM SET PATH=%CYGNUSDIR%/native-99r1/H-i686-cygwin32/bin;%AGBDIR%/bin;%PATH%;.
REM SET PATH=%CYGNUSDIR%/thumbelf-000512/H-i686-cygwin32/bin;%PATH%
REM SET PATH=%AGBDIR%/bin;%PATH%

REM ##################################################
REM		�f�B���N�g���p�X��`
REM ##################################################
SET WBLIBDIR=%PROJECT_ROOT%lib
SET CWFolder_NITRO=C:\Program Files\Freescale\CW for NINTENDO DSi V1.1\
SET CWFOLDER_TWL=C:\Program Files\Freescale\CW for NINTENDO DSi V1.1\
SET CW_NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET CW_TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET LM_LICENSE_FILE=C:\Program Files\Freescale\CW for NINTENDO DSi V1.1\license.dat
SET NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET NITROSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET NITROWIFI_ROOT=%WBLIBDIR%\TwlDWC
SET NITRODWC_ROOT=%WBLIBDIR%\TwlDWC

REM �n�C�u���b�h�Ή��͍���s��
REM SET TWLSDK_PLATFORM=TWL NITRO
SET TWLSDK_PLATFORM=NITRO

SET TWLSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET TWLWIFI_ROOT=%WBLIBDIR%\TwlDWC
SET TWLDWC_ROOT=%WBLIBDIR%\TwlDWC

SET TWLVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto
SET NITROVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto

SET GFL_MAKE_DIR=C:/home/gflib/gflib/
SET GFL_PROJECT_DIR=%PROJECT_ROOT%lib/gflib/
SET BMDS_SDK_ROOT=%PROJECT_ROOT%lib/BMDS_SDK/

SET SVN_EDITOR=c:\tools\vim\gvim

REM ##################################################
REM		�p�X�̒ǉ�
REM ##################################################
PATH=c:\tools;%PROJECT_ROOT%;%PROJECT_ROOT%\tools;c:\tools\subversion\bin;c:\cygwin\bin;c:\tools\vim;C:\tools;%NITROSDK_ROOT%\tools\bin;%NITROSYSTEM_ROOT%\tools\bin;%PATH%;

@ECHO ON

cd C:\home\pokemon_wb\tools\effecteditor
del we_000.bin
ruby c:\home\pokemon_wb\resource\wazaeffect\eescmk.rb c:\home\pokemon_wb\prog\src\battle\btlv\btlv_effvm_def.h we_test.esf c:/home/pokemon_wb/ on

if not %ERRORLEVEL%==0 GOTO ERROR

make

if %ERRORLEVEL%==0 GOTO END

:ERROR
echo �G���[���������܂���
pause

:END
