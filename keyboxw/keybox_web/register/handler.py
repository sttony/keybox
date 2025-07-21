import datetime
import json
import logging
import os
import uuid

import requests

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

    public_key = http_parameter_helper.get_json_api_payload("pubKey")
    if not public_key:
        logger.error("No public_key")
        return {"message": "no pubkey"}, 404

    # check if email is ready in DDB
    ddb_adapter = DDBAdapter(stage=STAGE)
    user = ddb_adapter.get_user(email)
    if not user:
        user = User(
            email=email,
            activate_status='pending',
            activate_code=str(uuid.uuid4()),
            expiring_date= int((datetime.datetime.now() + datetime.timedelta(minutes=10)).timestamp()),
            public_key= public_key,
            file_path= str(uuid.uuid4())
        )
    else:
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now().timestamp():
            return {"message": "email exists"}, 403
        else:
            user.activate_code = str(uuid.uuid4())
            user.expiring_date = int((datetime.datetime.now() + datetime.timedelta(minutes=10)).timestamp())

    # update entity
    ddb_adapter.put_user(user)

    # send email
    email_adapter = EmailAdapter()
    email_adapter.send_activate(user)

    return {"message": "Refresh succeed"}, 200
