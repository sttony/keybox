import urllib

import requests

from utility import secretsmanager


class EmailAdapter:
    def __init__(self):

        pass

    def fetch_oAuth2_token(self):
        url = "https://accounts.zoho.com/oauth/v2/token"
        access_token = secretsmanager.get_secret('prod_zoho')['access_token']
        account_id = secretsmanager.get_secret('prod_zoho')['account_id']
        client_id =  secretsmanager.get_secret('prod_zoho')['client_id']
        client_secret = secretsmanager.get_secret('prod_zoho')['client_secret']

        # fetch auth_code
        params = {
            "scope": "ZohoMail.accounts.ALL",
            "client_id": client_id,
            "response_type": "code",
            "access_type": "offline",
            "redirect_uri": "https://j9gr9uiepf.execute-api.us-west-2.amazonaws.com/Prod/refresh_token",
            "state": "secure_random_state",
            "prompt": "consent"
        }

        auth_url = "https://accounts.zoho.com/oauth/v2/auth?" + urllib.parse.urlencode(params)

        print("Open this URL in your browser:\n", auth_url)

        payload = {
            "grant_type": "authorization_code",
            "client_id": client_id,
            "client_secret": client_secret,
            "redirect_uri": redirect_uri,
            "code": auth_code
        }





    def send_register_email(self):
        oAuth2_token = self.fetch_oAuth2_token()


        payload = {
            "fromAddress": 'admin@k3ybox.us',
            "toAddress": ['tongli03@hotmail.com'],
            "subject": 'You registered a email to k3ybox.',
            "content": 'Please click the link to active your email'
        }
        response = requests.post(url, headers=headers, json=payload)
        resp = response.json()
        return resp

    def send_activate(self):
        pass


if __name__ == "__main__":
    email_adapter = EmailAdapter()
    email_adapter.send_register_email()
