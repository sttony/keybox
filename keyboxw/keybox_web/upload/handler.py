import base64
import datetime
import json
import logging
import uuid

import boto3
import requests

import sys

from botocore.exceptions import ClientError
from botocore.utils import parse_to_aware_datetime
from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.serialization import load_pem_public_key

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)
STAGE = "beta"


def lambda_handler(event, context):
    http_parameter_helper = HttpParameterHelper(event, context)
    # check email
    email = http_parameter_helper.get_json_api_payload("email")
    if not email:
        logger.error("No email")
        return {"message": "no email"}, 404
    signature = http_parameter_helper.get_json_api_payload("signature")
    if not signature:
        return {"message": "no signature"}, 404
    payload = http_parameter_helper.get_json_api_payload("payload")
    if not payload:
        return {"message": "no payload"}, 404


    # check if email is ready in DDB
    ddb_adapter = DDBAdapter()
    user = ddb_adapter.get_user(email)
    if not user:
        return {"message": "user not found"}, 404
    else:
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now().timestamp():
            return {"message": "user not activated"}, 403

    public_key = load_pem_public_key(user.public_key.encode('utf-8'))
    signature_bytes = base64.b64decode(signature)
    payload_bytes = base64.b64decode(payload)
    # check signature
    try:
        public_key.verify(
            signature_bytes,
            payload_bytes,
            padding.PKCS1v15(),
            hashes.SHA256()
        )

    except InvalidSignature:
        logger.error(f"Email {user.email} has invalid signature")
        logger.info(signature)
        logger.info(payload)
        logger.info(user.public_key)
        return {"message": "Invalid signature"}, 403

    # return s3
    logging.info("start uploading")
    s3 = boto3.client('s3')
    s3.put_object(
        Bucket=f"{STAGE}-keybox-user",
        Key=user.file_path,
        Body=payload_bytes
    )
    logging.info(f"uploaded {user.file_path}")
    return {"message": "Refresh succeed"}, 200
