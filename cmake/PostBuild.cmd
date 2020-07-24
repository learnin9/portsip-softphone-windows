@echo off

set batDir=%~dp0
set OutDir=%1
echo %batDir%
cd /d %batDir%
echo ------------------------------------clear  output start------------------------------------
cd /d %OutDir%
del /f /s /q *.aps
del /f /s /q *.ncb
del /f /s /q /ah *.suo
del /f /s /q *.vcproj.*.user
del /f /s /q *.plg
del /f /s /q *.opt
del /f /s /q *.pch
del /f /s /q *.idb
del /f /s /q *.ilk
del /f /s /q *.obj
del /f /s /q *.dep
del /f /s /q *.DPbcd
del /f /s /q *.manifest
del /f /s /q *.ib_tag
del /f /s /q *.res
del /f /s /q *.ib_pdb_index
del /f /s /q *.exp
del /f /s /q *.bak
echo ------------------------------------clear  output end------------------------------------

echo ------------------------------------copy res file to install packdge begin------------------------------------
cd /d %batDir%
echo ------------------------------------delete softphone folder begin------------------------------------
rmdir  /s /q %batDir%\..\..\softphone\
echo ------------------------------------delete softphone folder end------------------------------------
echo ------------------------------------copy Bin folder begin------------------------------------
xcopy /y  /s /e  %batDir%\..\PortSIPUC\Bin\cachefiles ..\..\softphone\cachefiles\
xcopy /y  /s %batDir%\..\PortSIPUC\Bin\skins ..\..\softphone\skins\
xcopy /y  /s %batDir%\..\PortSIPUC\Bin\sound ..\..\softphone\sound\
echo D |xcopy /y /s %batDir%\..\PortSIPUC\Bin\plugins ..\..\softphone\plugins\
copy /y  %batDir%\..\PortSIPUC\Bin\axvlc.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\ExportClass.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\icudt64.dll ..\..\softphone\
copy /y %batDir%\..\PortSIPUC\Bin\icuin64.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\icuuc64.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\libvlc.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\libvlccore.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\log4cplus.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\log4cplusU.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\log4cplusD.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\log4cplusUD.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\param.db ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\portsip_media.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\portsip_uc_sdk.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\portsip_serverbase.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\swscale-0.dll ..\..\softphone\
copy /y  %batDir%\..\PortSIPUC\Bin\swscale-0.dll ..\..\softphone\
copy /y  %OutDir%\PortGoClient.exe   ..\..\softphone\PortGo_vs2017.exe
copy /y  %OutDir%\PortGoClient.pdb   ..\..\softphone\PortGoClient.pdb
copy /y  %batDir%\..\PortSIPUC\Bin\Lauguages.otto ..\..\softphone\
copy /y  %OutDir%\Keeper.exe   ..\..\softphone\Keeper.exe
echo ------------------------------------copy Bin folder END------------------------------------

echo ------------------------------------copy res file to install packdge end------------------------------------