sudo mkdir /usr/include/agl/net
sudo cp -Rap *.h /usr/include/agl/net/
make release
sudo cp -Rap ../lib/*.so /usr/lib/
