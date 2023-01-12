#!/bin/bash
# bundle app from trial
mkdir bundle
cd bundle
curl -O -J "https://www.aseprite.org/downloads/trial/Aseprite-v1.2.39-trial-macOS.dmg"
if [[ -f "Aseprite-v1.2.39-trial-macOS.dmg" ]]; then
	mkdir mount
	yes qy | hdiutil attach -quiet -nobrowse -noverify -noautoopen -mountpoint mount Aseprite-v1.2.39-trial-macOS.dmg
	cp -r mount/Aseprite.app .
	hdiutil detach mount
else
	echo "error, trial not found!"
	exit 1
fi
