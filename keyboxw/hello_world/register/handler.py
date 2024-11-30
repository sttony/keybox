import json
import logging
import requests

from utility import secretsmanager
from utility import http_parameter_helper

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


    # create entity

    # send email


    return {"message": "Refresh succeed"}, 200
