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

    if "code" not in event["queryStringParameters"]:
        logger.error("No code in queryStringParameters")
        return {"message": "no code"}, 404

    authentication_code = event["queryStringParameters"]["code"]

    response = secretsmanager.get_secret("prod_zoho")
    if not response:
        return {"message": "Secrete manager failed"}, 404

    client_id = response["zoho_client_id"]
    client_secret = response["zoho_client_secret"]

    get_auth_token_url = f"https://accounts.zoho.com/oauth/v2/token?code={authentication_code}&grant_type=authorization_code&client_id={client_id}&client_secret={client_secret}&redirect_uri=https://jysrwrfcrh.execute-api.us-west-2.amazonaws.com/Prod/refresh_token&scope=ZohoMail.accounts.ALL"

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
    response = secretsmanager.set_secret("prod_zoho",
                                         {'access_token': ddt['access_token'], 'refresh_token': ddt['refresh_token']})
    if response:
        logger.error(f"failed to set secrete with {response.status_code}, {response.json()}")
        return {response.json}, response.status_code

    logger.info("refresh email token succeed")
    return {"message": "Refresh succeed"}, 200
