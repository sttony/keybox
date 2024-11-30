from dataclasses import dataclass, field, todict
from datetime import datetime


@dataclass
class User:
    email: str
    activate_status: str # register/pending_activate/activated/expired
    expiring_date: datetime
    public_key: str
    file_path: str # keybox file on S3
    