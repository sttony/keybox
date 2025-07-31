import os
import json
import logging
import requests
from aws_lambda_powertools.utilities.data_classes import APIGatewayProxyEvent
from aws_lambda_powertools.utilities.typing import LambdaContext

from utility import secretsmanager
from utility.http_parameter_helper import HttpParameterHelper
from utility.configs import BASE_URLS

# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

STAGE = os.environ.get('STAGE', 'beta')


def lambda_handler(event: APIGatewayProxyEvent, context: dict):
    http_parameter_helper = HttpParameterHelper(event, context)

    authentication_code = http_parameter_helper.get_query_parameter("code")
    if not authentication_code:
        logger.error("No code in queryStringParameters")
        return {"message": "no code"}, 404

    zoho_secrets = secretsmanager.get_secret("prod_zoho")
    if not zoho_secrets:
        return {"message": "Secrete manager failed"}, 404

    client_id = zoho_secrets['client_id']
    client_secret = zoho_secrets['client_secret']


    get_auth_token_url = (f"https://accounts.zoho.com/oauth/v2/token?code={authentication_code}&"
                          f"grant_type=authorization_code&client_id={client_id}&client_secret={client_secret}&"
                          f"redirect_uri={BASE_URLS[STAGE]}/refresh_token&scope=ZohoMail.messages.ALL")

    response = requests.post(get_auth_token_url)
    if response.status_code != 200:
        logger.error(f"fetch access token failed with {response.status_code}, {response.json()}")
        return {"message" : ""}, response.status_code

    # check auth token and refresh token in response
    ddt = response.json()
    if ('access_token' not in ddt) or ('refresh_token' not in ddt):
        logger.error(f"no access_token or no refresh_token in response {response.status_code}, {response.json()}")
        return {"message": "no access_token or refresh_token in response"}, 501
    logger.info("Got access_token and refresh token")
    response = secretsmanager.set_secret("zoho_mail_token",
                                         {'access_token': ddt['access_token'], 'refresh_token': ddt['refresh_token']})
    if response:
        logger.error(f"failed to set secrete with {response.status_code}, {response.json()}")
        return {response.json}, response.status_code

    logger.info("refresh email token succeed")
    return {"message": "Refresh succeed"}, 200
