from dataclasses import dataclass
from datetime import datetime
from typing import Optional


@dataclass
class User:
    email: str
    activate_status: Optional[str]  # register/pending_activate/activated/expired
    activate_code: Optional[str]
    expiring_date: Optional[datetime]
    public_key: Optional[str]
    file_path: Optional[str]  # keybox file on S3
