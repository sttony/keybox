import base64
import datetime
import json
import logging
import os
import uuid

import boto3
import requests
from botocore.config import Config

from cryptography.hazmat.primitives import padding as ase_padding
from cryptography.hazmat.primitives.ciphers import algorithms, Cipher, modes

from utility import secretsmanager
from utility.email_adapter import EmailAdapter
from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)
STAGE = os.environ.get('STAGE', 'beta')


def lambda_handler(event, context):
    http_parameter_helper = HttpParameterHelper(event, context)
    # check email
    email = http_parameter_helper.get_json_api_payload("email")
    if not email:
        logger.error("No email")
        return {"message": "no email"}, 404

    # check if email is ready in DDB
    ddb_adapter = DDBAdapter(stage=STAGE)
    user = ddb_adapter.get_user(email)
    if not user:
        return {"message": "user not found"}, 404
    else:
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now().timestamp():
            return {"message": "user not activated"}, 403

    # presign a url for file.
    s3 = boto3.client('s3',  region_name="us-west-2")

    presign_url = s3.generate_presigned_url(
        ClientMethod='get_object',
        Params={
            'Bucket': f"{STAGE}-keybox-user",
            'Key': user.file_path
        },
        ExpiresIn=600,
        HttpMethod="GET"
    )
    logger.info(f"presign url: {presign_url}")
    # generate a session key,
    session_key = os.urandom(32)  # 32 bytes = 256 bits
    iv = os.urandom(16)  # use key as IV

    # encrypt url.
    padder = ase_padding.PKCS7(algorithms.AES.block_size).padder()
    padded_data = padder.update(presign_url.encode('utf-8')) + padder.finalize()
    cipher = Cipher(algorithms.AES(session_key), modes.CBC(iv))
    encryptor = cipher.encryptor()

    encrypted_url = encryptor.update(padded_data) + encryptor.finalize()

    # send key to user
    email_adapter = EmailAdapter()
    email_adapter.send_new_client(user, session_key.hex() + iv.hex())
    # return encrypt url back.
    return {"file_url": encrypted_url.hex()}, 200
