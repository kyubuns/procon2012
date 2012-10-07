# 最初は前提ソフトウエアのインストール
sudo apt-get -yV install build-essential

cd /tmp; sudo apt-get source opencv
sudo apt-get -yV build-dep opencv libjpeg-dev /
libopenjpeg-dev jasper libjasper-dev libjasper-runtime /
libpng12-dev libpng++-dev libpng3 libpnglite-dev libtiff-tools /
pngtools zlib1g-dev zlib1g-dbg v4l2ucp python autoconf /
libtbb2 libtbb-dev libeigen2-dev cmake openexr freeglut3-dev /
libglui-dev libavc1394-dev libdc1394-22-dev libdc1394-utils /
libxine-dev libxvidcore-dev libva-dev libssl-dev libv4l-dev /
libvo-aacenc-dev libvo-amrwbenc-dev libvorbis-dev libvpx-dev libtiff4-dev
cd /tmp
if [ ! -f OpenCV-2.4.2.tar.bz2 ]; then 
	wget http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.2/OpenCV-2.4.2.tar.bz2
fi 

sudo rm -rf OpenCV-2.4.2
tar -xvjof /tmp/OpenCV-2.4.2.tar.bz2
cd OpenCV-2.4.2
cmake -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=RELEASE -DWITH_TBB=ON -DINSTALL_C_EXAMPLES=ON -DWITH_CUDA=OFF -DWITH_OPENNI=ON -DWITH_UNICAP=ON -DWITH_V4L=ON -DWITH_XINE=ON  .
make
sudo make install
sudo ldconfig
