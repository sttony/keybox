import datetime
import json
import logging
import os
import uuid

import requests
import boto3
from botocore.exceptions import ClientError
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding as ase_padding

import sys

from s3transfer.compat import seekable

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)
STAGE = os.environ.get('STAGE', 'beta')

def generate_and_encrypt_session_key(public_key_str: str) -> tuple[bytes, bytes]:
    # Generate a random 256-bit AES key
    session_key = os.urandom(32)  # 32 bytes = 256 bits
    logger.info(f"Seesion Key = {session_key.hex()}")
    # Load the public key from string
    public_key = serialization.load_pem_public_key(public_key_str.encode())

    # Encrypt the session key with the public key
    encrypted_session_key = public_key.encrypt(
        session_key,
        padding.PKCS1v15()
    )

    return session_key, encrypted_session_key


def encrypt_file_content(file_content: bytes, session_key: bytes) -> tuple[bytes, bytes]:
    """
    Encrypts file content using AES in GCM mode.

    Parameters:
    - file_content: The content of the file to encrypt.
    - session_key: The 256-bit AES key used for encryption.

    Returns:
    - The encrypted file content (ciphertext).
    - The associated initialization vector (IV) used for encryption.
    """
    iv = os.urandom(16)
    padder = ase_padding.PKCS7(algorithms.AES.block_size).padder()
    padded_data = padder.update(file_content) + padder.finalize()
    cipher = Cipher(algorithms.AES(session_key), modes.CBC(iv))
    encryptor = cipher.encryptor()

    # Perform the encryption
    encrypted_file_content = encryptor.update(padded_data) + encryptor.finalize()

    # Return both the encrypted data and the IV
    return encrypted_file_content, iv


def lambda_handler(event, context):
    http_parameter_helper = HttpParameterHelper(event, context)
    # check email
    email = http_parameter_helper.get_json_api_payload("email")
    if not email:
        logger.error("No email")
        return {"message": "no email"}, 404

    # check if email is ready in DDB
    ddb_adapter = DDBAdapter()
    user = ddb_adapter.get_user(email)
    if not user:
        return {"message": "user not found"}, 404
    else:
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now().timestamp():
            return {"message": "user not activated"}, 403

    # generate a session AES key, encrypt with user.public_key
    session_key, encrypted_session_key = generate_and_encrypt_session_key(user.public_key)

    # fetch files from s3
    s3 = boto3.client('s3')
    try:
        obj = s3.get_object(Bucket=f"{STAGE}-keybox-user", Key=user.file_path)
        file_content = obj['Body'].read()
    except ClientError as e:
        file_content = None

    if file_content:
    # encrypt the file with the session key
        encrypted_file_content, iv = encrypt_file_content(file_content, session_key)
        return {"encrypted_file_content":  encrypted_file_content.hex(), "encrypted_session_key": encrypted_session_key.hex(), "iv": iv.hex()}, 200
    else:
        return {"encrypted_file_content":""}, 200

