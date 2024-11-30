from typing import Optional
from aws_lambda_powertools import Logger


logger = Logger()


def get_query_parameter(event: dict, parameter_name: str) -> Optional[str]:
    if parameter_name not in event["queryStringParameters"]:
        return None

    return event["queryStringParameters"][parameter_name]
