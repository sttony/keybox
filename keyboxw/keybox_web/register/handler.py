import datetime
import json
import logging
import uuid

import requests

import sys

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.email_adapter import EmailAdapter
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
        user = User(
            email=email,
            activate_status='pending',
            activate_code=str(uuid.uuid4()),
            expiring_date=datetime.datetime.now() + datetime.timedelta(minutes=10)
        )
    else:
        if user.activate_status == 'pending' and user.expiring_date > datetime.datetime.now():
            return {"message": "email exists"}, 403
        else:
            user.activate_code = str(uuid.uuid4())
            user.expiring_date = datetime.datetime.now() + datetime.timedelta(minutes=10)

    # update entity
    ddb_adapter.put_user(user)

    # send email
    email_adapter = EmailAdapter()
    email_adapter.send_activate()

    return {"message": "Refresh succeed"}, 200
