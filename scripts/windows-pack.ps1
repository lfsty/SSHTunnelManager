$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Path package

Copy-Item build/Release/SSHTunnelManager.exe package/

windeployqt --qmldir code/qml package/SSHTunnelManager.exe
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt 执行失败（退出码 $LASTEXITCODE）"
}

Compress-Archive -Path package/* -DestinationPath "SSHTunnelManager-windows-${env:TAG_NAME}.zip"
