import datetime
import logging
import os

from utility.http_parameter_helper import HttpParameterHelper
from utility.ddb_adapter import DDBAdapter

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)
STAGE = os.environ.get('STAGE', 'beta')

def lambda_handler(event, context):
    try:
        http_parameter_helper = HttpParameterHelper(event, context)
        
        # Try to get email from query parameters first, then from JSON payload
        email = http_parameter_helper.get_query_parameter("email")
        if not email:
            email = http_parameter_helper.get_json_api_payload("email")
            
        if not email:
            logger.error("No email provided")
            return {"message": "no email provided"}, 400

        # check if user exists in DDB
        ddb_adapter = DDBAdapter(stage=STAGE)
        user = ddb_adapter.get_user(email)
        
        if not user:
            logger.info(f"User not found: {email}")
            return {"message": "user not found"}, 404
        
        if not user.public_key:
            logger.info(f"Public key not found for user: {email}")
            return {"message": "public key not found"}, 404

        logger.info(f"Successfully retrieved public key for: {email}")
        return {
            "email": email,
            "public_key": user.public_key
        }, 200
    except Exception as e:
        logger.error(f"Error retrieving public key: {str(e)}")
        return {"message": "internal server error"}, 500
