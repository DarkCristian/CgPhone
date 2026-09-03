#define MyAppName "CgPhone"
#define MyAppVersion "0.3.1"
#define MyAppExeName "CgPhone.exe"
#define MyAppExePath "bin\CgPhone.exe"

[Setup]
AppId={{3BFB330D-BE35-4D85-BC50-C72CF6C49187}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\CgPhone
DefaultGroupName=CgPhone
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputBaseFilename=CgPhone-Setup-{#MyAppVersion}-x64
Compression=lzma2
SolidCompression=yes
UninstallDisplayIcon={app}\{#MyAppExePath}
WizardStyle=modern
SetupIconFile=..\assets\app\CgPhone.ico

[Dirs]
Name: "{commonappdata}\CgPhone"; Permissions: users-readexec

[Files]
Source: "..\package\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; Include only an x64 encoder in the x64 installer. Never package the supplied x86 DLL here.
Source: "..\third_party\lame\x64\lame_enc.dll"; DestDir: "{app}\codecs\x64"; Flags: ignoreversion skipifsourcedoesntexist

[Icons]
Name: "{group}\CgPhone"; Filename: "{app}\{#MyAppExePath}"; IconFilename: "{app}\{#MyAppExePath}"
Name: "{commondesktop}\CgPhone"; Filename: "{app}\{#MyAppExePath}"; IconFilename: "{app}\{#MyAppExePath}"

[Run]
Filename: "{app}\{#MyAppExePath}"; Description: "Iniciar CgPhone"; Flags: nowait postinstall skipifsilent

[UninstallRun]
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=""CgPhone SIP-RTP"" program=""{app}\{#MyAppExePath}"""; Flags: runhidden
; CgPhone administra el valor desde Cuenta; al desinstalar se elimina para no
; dejar una ruta de autoarranque huérfana.
Filename: "{cmd}"; Parameters: "/c reg delete HKLM\Software\Microsoft\Windows\CurrentVersion\Run /v CgPhone /f"; Flags: runhidden
