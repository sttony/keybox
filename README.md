# Introduction
## what is Keybox
It is an alternative version of keypass. It is written by CPP and QT, so it can run on Window/Linux/macOS natively 
will have android and iOS versions as well. Sync service based on AWS lambda is also include. 

## why create a new password manager
Keypass is good, except it is a little bit complex for me, and it doesn't have the sync feature.

# How to build
## Windows 
1. vcpkg install openssl curl boost zlib 
2. vcpkg install pugixml  // which is only used to dump keypass db file. 
## Linux 
1. install openssl curl boost zlib with apt or pacman or what else pkg manager on your linux
2. install pugixml  // which is only used to dump keypass db file.
## macOS
1. install openssl curl boost zlib with brew
2. install pugixml  // which is only used to dump keypass db file.
## iOS
1. Build utilities iOS
    - install pkg-config  ``` brew install pkg-config ```
    - clone https://github.com/microsoft/vcpkg/
    - ```./bootstrap-vcpkg.sh```
    - ```./vcpkg install openssl --triplet arm64-ios```
   - ```./vcpkg install curl --triplet arm64-ios```
   - ```bash
      # replace vcpkg to your vcpkg root
      cmake -S ../utilities -B utilities-ios-device -G Xcode \
      -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
      -DOPENSSL_ROOT_DIR=/Users/litong/gitlocal/vcpkg/packages/openssl_arm64-ios \
      -DCMAKE_FIND_ROOT_PATH=/Users/litong/gitlocal/vcpkg/packages/openssl_arm64-ios \
      -DCURL_ROOT=/Users/litong/gitlocal/vcpkg/packages/curl_arm64-ios \
      -DCMAKE_FIND_ROOT_PATH="/Users/litong/gitlocal/vcpkg/packages/openssl_arm64-ios;/Users/litong/gitlocal/vcpkg/packages/curl_arm64-ios" \
      -DOPENSSL_USE_STATIC_LIBS=TRUE \
      -DVCPKG_TARGET_TRIPLET=arm64-iphoneos
     ```
2. create xcode workspace,  put keyboxi and generated utilities-ios-device/keybox.xcodeproj into the workspace
3. build keyboxi.

## android
TBD

## How to deploy the sync service

# How the sync service work


# FAQ

## Why doesn't it support the key file, and only support password? 
How will you store the key file? 

## Can I reset my password? 
No, unless you can decrypt AES 256. 

## Can another one can get my keybox file in the sync service? 
yes, if you lost your email box access, AND you lost your password. 
