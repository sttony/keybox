import json
import logging
import requests

from utility import secretsmanager
from utility.ddb_adapter import DDBAdapter
from utility.http_parameter_helper import HttpParameterHelper

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

def lambda_handler(event, context):
    # the url should be  base_url/activate?email=[base64]&activate_code=[]
    http_parameter_helper = HttpParameterHelper(event, context)
    email = http_parameter_helper.get_query_parameter("email")
    if not email:
        return {"message": "no email"}, 404
    activate_code = http_parameter_helper.get_query_parameter("activate_code")
    if not activate_code:
        return {"message": "no activate_code"}, 404

    ddb_adapter = DDBAdapter()
    user = ddb_adapter.get_user(email)
    if not user:
        logger.error("user not found")
        return {"message": "user not found"}, 404

    if user.activate_status != "pending" or user.activate_code != activate_code:
        logger.error("activate failed")
        return {"message": "activate failed"}, 403

    user.activate_status = "active"
    ddb_adapter.put_user(user)

    return {"message": "Activate succeed"}, 200
