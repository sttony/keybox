import logging
from dataclasses import fields, asdict
from typing import Optional

import boto3

from .user_entity import User


class DDBAdapter:
    def __init__(self):
        self.session = boto3.Session(region_name="us-west-2")
        self.dynamodb = self.session.resource("dynamodb")
        self.user_table = self.dynamodb.Table("User")

    def get_user(self, email: str) -> Optional[User]:
        response = self.user_table.get_item(Key={"email": email})
        if 'Item' not in response:
            return None

        item = response['Item']
        user = User(email=item['email'])
        for field in fields(User):
            if field.name in item:
                setattr(user, field.name, item[field.name])
        return user

    def put_user(self, user: User):
        item = asdict(user)
        response = self.user_table.put_item(Item=item)
        if response['ResponseMetadata']['HTTPStatusCode'] != 200:
            logging.error(f'Put {user.email} to db failed with {response}')
            raise Exception(f'Put {user.email} to db failed with {response}')