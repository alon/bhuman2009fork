;Installer script for vsyabsl

[Setup]
AppName=vsyabsl Intellisense
AppVerName=vsyabsl Intellisense Beta 1 For Visual Studio 2005
DefaultDirName={code:GetInstallDir}
DefaultGroupName=yabsl Intellisense
UninstallDisplayIcon={app}\yabsl Intellisense
InfoBeforeFile=infobefore.txt
DirExistsWarning=no
OutputBaseFilename=vsyabsl-setup
OutputDir=..\..\..\..\..\Bin
[Files]
Source: "babelservice.tlb"; DestDir: "{app}";
Source: "BabelPackage.dll"; DestDir: "{app}";
Source: "..\..\..\..\..\Build\vsyabsl\Debug\vsyabsl.dll"; DestDir: "{app}";

[Run]
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\BabelPackage.dll"""; Description: "Registering babelpackage.dll"
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\vsyabsl.dll"""; Description: "Registering vsyabsl.dll"

[UninstallRun]
Filename: "{sys}\regsvr32.exe"; Parameters: "/s /u ""{app}\BabelPackage.dll"""
Filename: "{sys}\regsvr32.exe"; Parameters: "/s /u ""{app}\vsyabsl.dll"""

[Code]
function getInstallDir(Param: String): String;
var
  InstallDir: String;
begin
  if Length(InstallDir) = 0 then
    RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\8.0', 'InstallDir', InstallDir);

  if Length(InstallDir) = 0 then
    Result := ExpandConstant('{sys}')
  else
    Result := InstallDir
end;


