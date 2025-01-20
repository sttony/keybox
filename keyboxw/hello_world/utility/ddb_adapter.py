import boto3


session = boto3.Session(
    region_name="us-west-2"
)

#
# class DDBAdapter:
#     __init__ ( )

dynamodb = session.resource("dynamodb")

table = dynamodb.Table("User")

