@ECHO OFF
if "%1" == "debug"	goto run_debug
if "%1" == "release" 	goto run_release
goto end

REM ========================================================
:run_debug
del ..\SDETest\debug\sdelib.lib
copy .\Debug\sdelib.lib ..\SDETest\debug\sdelib.lib
del ..\SDETest\debug\sdelib.dll
copy .\Debug\sdelib.dll ..\SDETest\debug\sdelib.dll
goto end
REM ========================================================
:run_release
del ..\SDETest\Release\sdelib.lib
copy .\Release\sdelib.lib ..\SDETest\Release\sdelib.lib
del ..\SDETest\Release\sdelib.dll
copy .\Release\sdelib.dll ..\SDETest\Release\sdelib.dll
goto end
REM ========================================================



:end