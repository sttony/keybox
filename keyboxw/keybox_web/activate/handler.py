import logging
import os
from decimal import Decimal

from utility.configs import USER_DEFAULT_DURATION
from utility.ddb_adapter import DDBAdapter
from utility.http_parameter_helper import HttpParameterHelper

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

STAGE = os.environ.get('STAGE', 'beta')

def lambda_handler(event, context):
    # the url should be  base_url/activate?email=[base64]&activate_code=[]
    http_parameter_helper = HttpParameterHelper(event, context)
    email = http_parameter_helper.get_query_parameter("email")
    if not email:
        return {"message": "no email"}, 404
    activate_code = http_parameter_helper.get_query_parameter("activate_code")
    if not activate_code:
        return {"message": "no activate_code"}, 404

    ddb_adapter = DDBAdapter(stage=STAGE)
    user = ddb_adapter.get_user(email)
    if not user:
        logger.error("user not found")
        return {"message": "user not found"}, 404

    if user.activate_status != "pending" or user.activate_code != activate_code:
        logger.error("activate failed")
        return {"message": "activate failed"}, 403

    user.activate_status = "active"
    user.expiring_date = user.expiring_date + int(USER_DEFAULT_DURATION[STAGE].total_seconds())
    ddb_adapter.put_user(user)

    return {"message": "Activate succeed"}, 200
