# kuseraccount
Advanced kcm_useraccount for KF5

## Build && Install

```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr \
    -DLIB_INSTALL_DIR=lib \
    -DKDE_INSTALL_USE_QT_SYS_PATHS=ON   \
    -DENABLE_DEBUG=OFF
make
sudo make install
```

## ScreenShot

![ScreenShot](https://raw.github.com/isoft-linux/kuseraccount/master/doc/snapshot1.png)
