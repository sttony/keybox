#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd "$script_dir/.." && pwd)"

cd "$project_dir"

sam build --cached --parallel

sam deploy \
    --stack-name keyboxw-prod \
    --s3-prefix keyboxw-prod \
    --region "${AWS_REGION:-us-west-2}" \
    --profile "${AWS_PROFILE:-keybox}" \
    --capabilities CAPABILITY_IAM \
    --resolve-s3 \
    --confirm-changeset \
    "$@" \
    --parameter-overrides STAGE=prod
