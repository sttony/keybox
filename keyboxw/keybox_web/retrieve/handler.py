import datetime
import json
import logging
import uuid

import requests
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.exceptions import InvalidSignature

import sys

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)


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
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now():
            return {"message": "user not activated"}, 403

    # check signature
    signature = http_parameter_helper.get_json_api_payload("signature")
    if not email:
        logger.error("No signature")
        return {"message": "no email"}, 403





    # return s3


    return {"message": "Refresh succeed"}, 200
