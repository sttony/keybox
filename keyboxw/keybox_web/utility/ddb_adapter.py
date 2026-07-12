import datetime
import logging

from dataclasses import fields, asdict
from typing import Optional

import boto3
from botocore.exceptions import ClientError
from boto3.dynamodb.conditions import Attr

from utility.user_entity import User


class DDBAdapter:
    def __init__(self, stage = "beta"):
        self.session = boto3.Session(region_name="us-west-2")
        self.dynamodb = self.session.resource("dynamodb")
        self.user_table = self.dynamodb.Table(f"{stage}_users")

    def get_user(self, email: str) -> Optional[User]:
        response = self.user_table.get_item(Key={"email": email})
        if 'Item' not in response:
            return None

        item = response['Item']
        field_values = {}
        for field in fields(User):
            field_values[field.name] = item.get(field.name)

        return User(**field_values)

    def put_user(self, user: User):
        item = asdict(user)
        response = self.user_table.put_item(Item=item)
        if response['ResponseMetadata']['HTTPStatusCode'] != 200:
            logging.error(f'Put {user.email} to db failed with {response}')
            raise Exception(f'Put {user.email} to db failed with {response}')

    def set_delete_challenge(self, email: str, challenge: dict):
        response = self.user_table.update_item(
            Key={"email": email},
            UpdateExpression="SET delete_challenge = :challenge",
            ExpressionAttributeValues={":challenge": challenge},
            ReturnValues="NONE",
        )
        if response['ResponseMetadata']['HTTPStatusCode'] != 200:
            logging.error(f'Set delete challenge for {email} failed with {response}')
            raise Exception(f'Set delete challenge for {email} failed with {response}')

    def consume_delete_challenge(self, email: str, challenge: dict) -> bool:
        try:
            self.user_table.update_item(
                Key={"email": email},
                UpdateExpression="REMOVE delete_challenge",
                ConditionExpression=(
                    Attr("delete_challenge.account_id").eq(challenge["account_id"]) &
                    Attr("delete_challenge.client_id").eq(challenge["client_id"]) &
                    Attr("delete_challenge.operation").eq(challenge["operation"]) &
                    Attr("delete_challenge.nonce").eq(challenge["nonce"]) &
                    Attr("delete_challenge.expiration").eq(challenge["expiration"])
                ),
                ReturnValues="NONE",
            )
            return True
        except ClientError as error:
            if error.response.get("Error", {}).get("Code") == "ConditionalCheckFailedException":
                return False
            raise

    def delete_user(self, email: str):
        response = self.user_table.delete_item(Key={"email": email})
        if response['ResponseMetadata']['HTTPStatusCode'] != 200:
            logging.error(f'Delete {email} from db failed with {response}')
            raise Exception(f'Delete {email} from db failed with {response}')


if __name__ == "__main__":
    import os
    os.environ['AWS_PROFILE'] = 'keybox'
    ddb_adapter = DDBAdapter()
    user = User(email='<EMAIL>', activate_status='pending', activate_code='123456',
                expiring_date=int((datetime.datetime.now() + datetime.timedelta(minutes=10)).timestamp()),
                public_key='123456', file_path='s3://bucket/keybox.kdb')
    ddb_adapter.put_user(user)
    user = ddb_adapter.get_user('<EMAIL>')
    print(user)
