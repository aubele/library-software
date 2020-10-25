REM Does the tmp folder exist
echo ==================
echo Prüfe ob tmp-Verzeichnis existiert
IF NOT exist bin/ (
	mkdir bin
)
echo ==================

echo ==================
echo Wechsle ins bin-Verzeichnis
cd bin
echo ==================

echo ==================
echo Kopiere baseqt.dll
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\build-baseqt-Desktop_Qt_5_10_1_MinGW_32bit\release\baseqt.dll baseqt.dll
echo Kopiere db.dll
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\build-db-Desktop_Qt_5_10_1_MinGW_32bit\release\db.dll db.dll
echo Kopiere ExcelReader.dll
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\build-ExcelReader-Desktop_Qt_5_10_1_MinGW_32bit\release\ExcelReader.dll ExcelReader.dll
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\MainProject\lib\libgcc_s_dw2-1.dll libgcc_s_dw2-1.dll
copy "C:\Users\Dani\Desktop\Projekt\Gitlab\Source\MainProject\lib\libstdc++-6.dll" "libstdc++-6.dll"
copy "C:\Users\Dani\Desktop\Projekt\Gitlab\Source\MainProject\lib\libwinpthread-1.dll" "libwinpthread-1.dll"
echo ==================

echo ===================
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\build-MainProject-Desktop_Qt_5_10_1_MinGW_32bit\release\MainProject.exe LibOrg.exe
echo ===================

echo ===================
echo Kopiere Qt dlls ins release Verzeichnis
C:\Qt\5.10.1\mingw53_32\bin\windeployqt.exe db.dll
C:\Qt\5.10.1\mingw53_32\bin\windeployqt.exe LibOrg.exe
echo ===================

echo ===================
echo Kopiere database and config
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\MainProject\database.db database.db
copy C:\Users\Dani\Desktop\Projekt\Gitlab\Source\MainProject\liborg.config liborg.config
echo ===================

echo ===================
echo zippe daten
cd ..
C:\Qt\Tools\QtInstallerFramework\3.0\bin\archivegen data.7z bin/
echo ===================

echo ===================
echo Verschiebe Archiv in das Installer-Verzeichnis
move data.7z C:\Users\Dani\Desktop\Projekt\Gitlab\Source\LibOrgInstaller\packages\Hauptprogramm\data
echo ===================

rmdir /S bin