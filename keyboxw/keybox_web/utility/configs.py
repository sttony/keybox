import datetime

BASE_URLS = {
    "beta" : "https://3ehnw4a7d3.execute-api.us-west-2.amazonaws.com/Prod",
    "prod" : "https://api.k3ybox.us"
}

USER_DEFAULT_DURATION = {
    'beta' : datetime.timedelta(minutes=10),
    'prod' : datetime.timedelta(days=365),
}