import json
import logging
import requests

from utility import secretsmanager

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

def lambda_handler(event, context):


    #

    return {"message": "Refresh succeed"}, 200
