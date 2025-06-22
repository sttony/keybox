import urllib

import requests

from utility import secretsmanager
from utility.user_entity import User


class EmailAdapter:
    def __init__(self):

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
        print(access_token)
        return access_token



    def send_register_email(self):
        oAuth2_token = self.fetch_oAuth2_token()
        account_id = secretsmanager.get_secret('prod_zoho')['account_id']
        url = f'https://mail.zoho.com/api/accounts/{account_id}/messages'

        # Create the body of the request (for JSON payload)
        body = {
            "fromAddress": "admin@k3ybox.us",
            "toAddress": "tongli03@hotmail.com",
            "subject": "Email - test email api",
            "content": "Test with zoho email api",
            "askReceipt": "yes"
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
                https://j9gr9uiepf.execute-api.us-west-2.amazonaws.com/Prod/activate?activate_code={user.activate_code}
                """,
            "askReceipt": "yes"
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
