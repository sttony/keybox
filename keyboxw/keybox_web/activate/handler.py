import json
import logging
import requests

from utility import secretsmanager

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

def lambda_handler(event, context):
    if ("queryStringParameters" not in event) or (not event["queryStringParameters"]):
        logger.error("No queryStringParameters in event")
        return {"message": "no queryStringParameters"}, 404

    if "opt_code" not in event["queryStringParameters"]:
        logger.error("No opt_code in queryStringParameters")
        return {"message": "no code"}, 404

    #

    return {"message": "Refresh succeed"}, 200
