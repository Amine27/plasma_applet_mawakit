cd ../build/
cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` .. && sudo make install && cd .. && plasmoidviewer -c desktop mawakit