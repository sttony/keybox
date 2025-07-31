import urllib

import requests
from urllib.parse import quote

from utility import secretsmanager
from utility.user_entity import User
from utility.configs import BASE_URLS

class EmailAdapter:
    def __init__(self, stage = "beta"):
        self.base_url = BASE_URLS[stage]
        pass

    def fetch_oAuth2_token(self):
        access_token = secretsmanager.get_secret('zoho_mail_token')['access_token']
        refresh_token = secretsmanager.get_secret('zoho_mail_token')['refresh_token']
        client_id = secretsmanager.get_secret('prod_zoho')['client_id']
        client_secret = secretsmanager.get_secret('prod_zoho')['client_secret']
        token_url = "https://accounts.zoho.com/oauth/v2/token"

        payload = {
            "refresh_token": refresh_token,
            "client_id": client_id,
            "client_secret": client_secret,
            "grant_type": "refresh_token"
        }

        response = requests.post(token_url, data=payload)
        access_token = response.json().get("access_token")
        return access_token

    def send_activate(self, user: User):
        oAuth2_token = self.fetch_oAuth2_token()
        account_id = secretsmanager.get_secret('prod_zoho')['account_id']
        url = f'https://mail.zoho.com/api/accounts/{account_id}/messages'

        body = {
            "fromAddress": "admin@k3ybox.us",
            "toAddress": f"{user.email}",
            "subject": "You registered a email to k3ybox.us",
            "content": f"""
                Please use follow url to activate your email:
                {self.base_url}/activate?activate_code={user.activate_code}&email={quote(user.email)}
                """,
            "askReceipt": "no"
        }
        # Define the headers
        headers = {
            'Content-Type': 'application/json',
            'Authorization': f'Zoho-oauthtoken {oAuth2_token}',
            'Accept': 'application/json'
        }

        # Send the POST request with the body and headers
        response = requests.post(url, json=body, headers=headers)
        if response.status_code != 200:
            raise Exception(f"Error sending email: {response.status_code}, {response.text}")


    def send_new_client(self, user: User, code: str):
        oAuth2_token = self.fetch_oAuth2_token()
        account_id = secretsmanager.get_secret('prod_zoho')['account_id']
        url = f'https://mail.zoho.com/api/accounts/{account_id}/messages'

        body = {
            "fromAddress": "admin@k3ybox.us",
            "toAddress": f"{user.email}",
            "subject": "Someone try to install a new client for this email",
            "content": f"""
                Please fill the following code into your client:
                {code}
                """,
            "askReceipt": "no"
        }
        # Define the headers
        headers = {
            'Content-Type': 'application/json',
            'Authorization': f'Zoho-oauthtoken {oAuth2_token}',
            'Accept': 'application/json'
        }

        # Send the POST request with the body and headers
        response = requests.post(url, json=body, headers=headers)
        if response.status_code != 200:
            raise Exception(f"Error sending email: {response.status_code}, {response.text}")


if __name__ == "__main__":
    email_adapter = EmailAdapter()
    email_adapter.send_register_email()
