import datetime
import json
import logging
import uuid

import requests

from utility import secretsmanager
from utility import http_parameter_helper
from utility.ddb_adapter import DDBAdapter
from utility.user_entity import User

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)


def lambda_handler(event, context):
    if ("queryStringParameters" not in event) or (not event["queryStringParameters"]):
        logger.error("No queryStringParameters in event")
        return {"message": "no queryStringParameters"}, 404

    # check email
    email = http_parameter_helper.get_query_parameter(event, "email")
    if not email:
        logger.error("No code in queryStringParameters")
        return {"message": "no code"}, 404

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


    return {"message": "Refresh succeed"}, 200
