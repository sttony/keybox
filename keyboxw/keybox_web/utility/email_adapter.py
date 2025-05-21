import requests

from utility import secretsmanager


class EmailAdapter:
    def __init__(self):

        pass

    def send_register_email(self):
        account_id = '86163000000008002'
        url = f"https://mail.zoho.com/api/accounts/{account_id}/messages"
        access_token = secretsmanager.get_secret('prod_zoho')['access_token']
        client_id = 'x'
        client_secret = 'xx'

        headers = {
            "Authorization": f"Zoho-oauthtoken {access_token}",
            "Content-Type": "application/json"
        }
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
