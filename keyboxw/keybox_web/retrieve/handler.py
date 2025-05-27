import datetime
import json
import logging
import os
import uuid

import requests
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

import sys

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)


def generate_and_encrypt_session_key(public_key_str: str) -> tuple[bytes, bytes]:
    # Generate a random 256-bit AES key
    session_key = os.urandom(32)  # 32 bytes = 256 bits

    # Load the public key from string
    public_key = serialization.load_pem_public_key(public_key_str.encode())

    # Encrypt the session key with the public key
    encrypted_session_key = public_key.encrypt(
        session_key,
        padding.OAEP(
            mgf=padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )

    return session_key, encrypted_session_key


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
    

    # encrypt the file with the session key

    return {"payload": "Refresh succeed"}, 200
