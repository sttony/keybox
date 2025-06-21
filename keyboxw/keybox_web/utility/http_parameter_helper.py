import json
from typing import Optional
from aws_lambda_powertools import Logger
from aws_lambda_powertools.utilities.typing import LambdaContext

logger = Logger()


class HttpParameterHelper:
    def __init__(self, event: dict, context: LambdaContext) -> None:
        self.event = event
        self.context = context
        self.query_parameters ={}

        if 'queryStringParameters' in self.event:
            self.query_parameters = self.event['queryStringParameters']
        self.body = None
        if 'body' in self.event and self.event['body']:
            try:
                self.body = json.loads(self.event['body'])
            except ValueError:
                self.body = {}


    def get_query_parameter(self, parameter_name: str) -> Optional[str]:
        return self.query_parameters.get(parameter_name, None)

    def get_json_api_payload(self, parameter_name: str) -> Optional[str]:
        if self.body:
            return self.body.get(parameter_name, None)
        else:
            return None