# Introduction
## what is Keybox
It is an alternative version of keypass. It is written by CPP and QT, so it can run on Window/Linux/macOS natively 
will have android and iOS versions as well. Sync service based on AWS lambda is also include. 

## why create a new password manager
Keypass is good, except it is a little bit complex for me, and it doesn't have the sync feature.

# How to build
## Windows 
1. install vcpkg
2. vcpkg install openssl curl boost zlib qt5 pugixml
3. Use CMake to generate and build the project.

## Linux 
1. install openssl curl boost zlib qt5 pugixml with your package manager (apt, pacman, etc.)
2. Use CMake to generate and build the project.

## macOS
1. install openssl curl boost zlib qt5 pugixml with brew
2. Use CMake to generate and build the project.

## iOS
1. Build utilities for iOS:
    - install pkg-config: `brew install pkg-config`
    - clone vcpkg: `git clone https://github.com/microsoft/vcpkg/`
    - `./bootstrap-vcpkg.sh`
    - `./vcpkg install openssl curl --triplet arm64-ios`
    - Use CMake to generate the Xcode project for `utilities`:
      ```bash
      # replace VCPKG_ROOT to your vcpkg path
      cmake -S utilities -B utilities-ios-device -G Xcode \
      -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
      -DOPENSSL_ROOT_DIR=$VCPKG_ROOT/packages/openssl_arm64-ios \
      -DCURL_ROOT=$VCPKG_ROOT/packages/curl_arm64-ios \
      -DCMAKE_FIND_ROOT_PATH="$VCPKG_ROOT/packages/openssl_arm64-ios;$VCPKG_ROOT/packages/curl_arm64-ios" \
      -DOPENSSL_USE_STATIC_LIBS=TRUE \
      -DVCPKG_TARGET_TRIPLET=arm64-ios
      ```
2. Create an Xcode workspace, add `keyboxi` and the generated `utilities-ios-device/keybox.xcodeproj`.
3. Build `keyboxi`.

## android
TBD

## How to deploy the sync service
The sync service is based on AWS Lambda and can be deployed using the AWS Serverless Application Model (SAM).

1. Install [AWS SAM CLI](https://docs.aws.amazon.com/serverless-application-model/latest/developerguide/serverless-sam-cli-install.html).
2. Install [Python 3.12](https://www.python.org/downloads/).
3. Install [Docker](https://hub.docker.com/search/?type=edition&offering=community) (required for building in a container).
4. Navigate to the `keyboxw` directory:
   ```bash
   cd keyboxw
   ```
5. Build and deploy the application:
   ```bash
   sam build --use-container
   sam deploy --guided
   ```

For more detailed information, including multi-stage deployment and local testing, refer to [keyboxw/README.md](keyboxw/README.md).

# How the sync service work
The sync service provides a secure way to synchronize your password database across devices.

### Workflow Overview
1. **Registration**: Users register with their email and provide an RSA public key. A verification link is sent to activate the account.
2. **Key Exchange**: The service uses the client's RSA public key to verify signatures and encrypt session keys for secure data transfer.
3. **Data Storage**: User data (encrypted keybox files) is stored in Amazon S3, and user metadata is managed in DynamoDB.
4. **Session Management**: Uses refresh tokens to maintain secure sessions without re-authenticating frequently.

### Client-Server Protocol
*   **Registration**: 
    1. Client generates an RSA key pair and sends the public key + email to `/register`.
    2. Server sends an activation email with a link to the user.
    3. User clicks the link in the email to call `/activate` and activate the account.
*   **Upload**: 
    1. Client signs the encrypted keybox file using its RSA private key.
    2. Client sends the email, payload (encrypted file), and signature to `/upload`.
    3. Server verifies the signature using the stored public key before saving to S3.
*   **Retrieve**:
    1. Client requests data from `/retrieve`.
    2. Server generates a random AES session key.
    3. Server encrypts the session key with the client's RSA public key.
    4. Server encrypts the S3 data with the AES session key.
    5. Server returns the encrypted session key, IV, and encrypted payload.
    6. Client decrypts the session key with its RSA private key, then decrypts the payload.
*   **New Client Setup**:
    1. Client sends email to `/setup_new_client`.
    2. Server generates an AES session key and a presigned S3 URL.
    3. Server encrypts the URL with the session key.
    4. Server sends the session key to the user's email and returns the encrypted URL to the client.
    5. Client retrieves the key from email to decrypt the URL and download the initial database.


# FAQ

## Why doesn't it support the key file, and only support password? 
How will you store the key file? 

## Can I reset my password? 
No, unless you can decrypt AES 256. 

## Can another one can get my keybox file in the sync service? 
yes, if you lost your email box access, AND you lost your password. 
