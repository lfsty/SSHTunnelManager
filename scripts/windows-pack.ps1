New-Item -ItemType Directory -Path package

Copy-Item build/code/Release/*.exe package/

windeployqt --qmldir . package/SSHTunnelManager.exe

Compress-Archive -Path package/* -DestinationPath "SSHTunnelManager-windows-${env:TAG_NAME}.zip"