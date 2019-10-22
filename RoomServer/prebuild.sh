mkdir prebuild
cd prebuild
wget https://github.com/sourcey/webrtc-precompiled-builds/raw/master/webrtc-22215-ab42706-linux-x64.tar.gz
tar xf webrtc-*-linux-x64.tar.gz
cp lib/x64/Release/libwebrtc_full.a /usr/lib
mkdir /usr/include/webrtc
cp -r include/* /usr/include/webrtc
cd ..
