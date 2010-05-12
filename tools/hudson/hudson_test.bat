@echo off
REM =================================================================
REM hudson用のmakeバッチ
REM 用途    : prompt.bat cmd hudson_make.bat でhudsonから呼び出し
REM 制作者  : hosaka_genya
REM 日付    : 2009/11/24
REM =================================================================

REM hudsonで定義される環境変数を利用
SET PROJECT_ROOT=%WORKSPACE%
SET PROJECT_PROGDIR=%PROJECT_ROOT%/prog/

REM ##################################################
REM		パスの追加
REM ##################################################
PATH=c:\tools;%PROJECT_ROOT%;%PROJECT_ROOT%\tools;c:\tools\subversion\bin;c:\cygwin\bin;c:\tools\vim;C:\tools;%NITROSDK_ROOT%\tools\bin;%NITROSYSTEM_ROOT%\tools\bin;%PATH%;

REM main.srl
SET PATH_MAIN_TLF=%PROJECT_PROGDIR%bin/ARM9-TS.HYB/Release/main.tlf
SET PATH_MAIN_SRL=%PROJECT_PROGDIR%bin/ARM9-TS.HYB/Release/main.srl

REM 最後のPrintfからのタイムアウト(秒) >> hudsonで指定
SET TIMEOUT_DISP=%1 

REM 対象DSのシリアルナンバー >> hudsonで指定
SET SERIAL_NO=%2

REM テストナンバー >> hudsonで指定
SET TEST_NO=%3

REM wbrom:05093474
REM hosaka:05113644

REM 終了検出文字列(改行が来た時に先頭から判定)
SET ABORT_STRING="  **** ASSERTION FAILED ! ****"

REM ================================
REM ユーザーネーム設定
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

@echo loadpath = %PATH_MAIN_SRL%
@echo TIMEOUT_DISP = %TIMEOUT_DISP%
@echo serial_no = %SERIAL_NO%

REM ================================
REM makeする
REM ================================
make
if %ERRORLEVEL% == 0 goto _MAKE_SUCCESS:
@echo make error.
exit 0
:_MAKE_SUCCESS

REM ================================
REM まずTLFに書きこんでからSRLを再生成
REM ================================
buryarg.TWL %PATH_MAIN_TLF% %TEST_NO%
touch %PATH_MAIN_SRL%
make

REM ================================
REM ブート
REM ================================
REM 実行開始
loadrun -t %TIMEOUT_DISP% -s %SERIAL_NO% -a %ABORT_STRING% %PATH_MAIN_SRL%
echo ErrorLevel = %ERRORLEVEL%

REM デバイスが見つからなかった
if %ERRORLEVEL% == -1 goto _ERROR_END:

REM 表示タイムアウト判定
if %ERRORLEVEL% == 204 goto _END:

REM 実行タイムアウト判定
if %ERRORLEVEL% == 205 goto _END:

REM 指定文字列検知で終了
if %ERRORLEVEL% == 200 goto _ERROR_END:
  
REM ================================
REM 指定文字列検知終了
REM ================================
:_ERROR_END
  REM エラーコードを丸々返す
  @echo hudson test is FAILED .
  pause
  exit %ERRORLEVEL%

REM ================================
REM 正常終了
REM ================================
:_END
  @echo hudson test is SUCCESS .
  REM devcon disable *HID*
  pause
  exit 0



