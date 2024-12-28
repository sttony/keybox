import json
from typing import Dict

import boto3
from botocore.exceptions import NoCredentialsError, PartialCredentialsError

import logging
# Configure logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

def get_secret(secret_name, region_name="us-west-2") -> Dict:
    # Create a Secrets Manager client
    client = boto3.client('secretsmanager', region_name=region_name)

    try:
        get_secret_value_response = client.get_secret_value(SecretId=secret_name)
    except NoCredentialsError:
        logger.error("Credentials not available")
        return None
    except PartialCredentialsError:
        logger.error("Incomplete credentials")
        return None
    except client.exceptions.ResourceNotFoundException:
        logger.error(f"The requested secret {secret_name} was not found")
        return None
    except client.exceptions.InvalidRequestException as e:
        logger.error(f"The request was invalid due to: {e}")
        return None
    except client.exceptions.InvalidParameterException as e:
        logger.error(f"The request had invalid params: {e}")
        return None

    # Decrypts secret using the associated KMS CMK
    secret = get_secret_value_response['SecretString']
    return json.loads(secret)


def set_secret(secret_name, payload_dict, region_name="us-west-2"):
    # Create a Secrets Manager client
    client = boto3.client('secretsmanager', region_name=region_name)

    try:
        response = client.update_secret(SecretId=secret_name, SecretString=json.dumps(payload_dict))
    except NoCredentialsError:
        logger.error("Credentials not available")
        return response
    except PartialCredentialsError:
        logger.error("Incomplete credentials")
        return response
    except client.exceptions.ResourceNotFoundException:
        logger.error(f"The requested secret {secret_name} was not found")
        return response
    except client.exceptions.InvalidRequestException as e:
        logger.error(f"The request was invalid due to: {e}")
        return response
    except client.exceptions.InvalidParameterException as e:
        logger.error(f"The request had invalid params: {e}")
        return response

    return None
