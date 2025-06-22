import urllib

from utility import secretsmanager


def refresh_url():
    url = "https://accounts.zoho.com/oauth/v2/token"
    client_id = secretsmanager.get_secret('prod_zoho')['client_id']

    # fetch auth_code
    params = {
        "scope": "ZohoMail.messages.ALL",
        "client_id": client_id,
        "response_type": "code",
        "access_type": "offline",
        "redirect_uri": "https://j9gr9uiepf.execute-api.us-west-2.amazonaws.com/Prod/refresh_token",
        "state": "secure_random_state",
        "prompt": "consent"
    }

    auth_url = "https://accounts.zoho.com/oauth/v2/auth?" + urllib.parse.urlencode(params)
    return auth_url

if __name__ == "__main__":
    print(refresh_url())