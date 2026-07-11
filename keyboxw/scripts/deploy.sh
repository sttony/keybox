#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Deploy Keybox Lambda and static web content.

Usage:
  scripts/deploy.sh --stage beta|prod [options]

Options:
  --stage STAGE                Deployment stage. Required: beta or prod.
  --region REGION              AWS region. Default: AWS_REGION or us-west-2.
  --profile PROFILE            AWS profile. Default: AWS_PROFILE or keybox.
  --static-source DIR          Local static content directory. Default: static_content.
  --static-bucket BUCKET       S3 bucket for static content. Default: STAGE-keybox-static.
  --static-prefix PREFIX       Optional S3 key prefix for static content.
  --distribution-id ID         CloudFront distribution to invalidate after static deploy.
  --ensure-cloudfront          Create or reuse a CloudFront distribution for the static site.
  --static-domain DOMAIN       CloudFront alias for static content. Default: www.k3ybox.us.
  --certificate-arn ARN        ACM certificate ARN for --static-domain. Required when creating CloudFront.
  --validation-url URL         HTTPS endpoint checked after deploy. Default: https://api.k3ybox.us/hello.
  --static-validation-url URL  Static HTTPS page checked after deploy. Default: https://www.k3ybox.us/privacy.
  --skip-lambda                Do not deploy the SAM/Lambda stack.
  --skip-static                Do not deploy static content.
  --skip-validation            Do not run the post-deploy HTTPS validation.
  --no-gzip                    Upload static files without gzip precompression.
  --no-confirm                 Deploy SAM changeset without interactive confirmation.
  -h, --help                   Show this help.

Examples:
  scripts/deploy.sh --stage beta
  scripts/deploy.sh --stage prod --static-bucket prod-keybox-static --distribution-id E1234567890
  scripts/deploy.sh --stage prod --ensure-cloudfront --certificate-arn arn:aws:acm:us-east-1:123456789012:certificate/example
EOF
}

stage=""
region="${AWS_REGION:-us-west-2}"
profile="${AWS_PROFILE:-keybox}"
static_bucket=""
static_source="static_content"
static_prefix=""
distribution_id=""
ensure_cloudfront=0
static_domain="www.k3ybox.us"
certificate_arn=""
validation_url="https://api.k3ybox.us/hello"
static_validation_url="https://www.k3ybox.us/privacy"
deploy_lambda=1
deploy_static=1
run_validation=1
gzip_static=1
confirm_changeset=1

while [[ $# -gt 0 ]]; do
    case "$1" in
        --stage)
            stage="${2:-}"
            shift 2
            ;;
        --region)
            region="${2:-}"
            shift 2
            ;;
        --profile)
            profile="${2:-}"
            shift 2
            ;;
        --static-bucket)
            static_bucket="${2:-}"
            shift 2
            ;;
        --static-source)
            static_source="${2:-}"
            shift 2
            ;;
        --static-prefix)
            static_prefix="${2:-}"
            shift 2
            ;;
        --distribution-id)
            distribution_id="${2:-}"
            shift 2
            ;;
        --ensure-cloudfront)
            ensure_cloudfront=1
            shift
            ;;
        --static-domain)
            static_domain="${2:-}"
            shift 2
            ;;
        --certificate-arn)
            certificate_arn="${2:-}"
            shift 2
            ;;
        --validation-url)
            validation_url="${2:-}"
            shift 2
            ;;
        --static-validation-url)
            static_validation_url="${2:-}"
            shift 2
            ;;
        --skip-lambda)
            deploy_lambda=0
            shift
            ;;
        --skip-static)
            deploy_static=0
            shift
            ;;
        --skip-validation)
            run_validation=0
            shift
            ;;
        --no-gzip)
            gzip_static=0
            shift
            ;;
        --no-confirm)
            confirm_changeset=0
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

case "$stage" in
    beta|prod)
        ;;
    "")
        echo "Missing required --stage value." >&2
        usage >&2
        exit 2
        ;;
    *)
        echo "Unsupported stage: $stage" >&2
        usage >&2
        exit 2
        ;;
esac

if [[ "$deploy_lambda" -eq 0 && "$deploy_static" -eq 0 ]]; then
    echo "Nothing to deploy: both --skip-lambda and --skip-static were provided." >&2
    exit 2
fi

if [[ -z "$static_bucket" ]]; then
    static_bucket="${stage}-keybox-static"
fi

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd "$script_dir/.." && pwd)"

cd "$project_dir"

cloudfront_args=()
if [[ -n "$profile" ]]; then
    cloudfront_args+=(--profile "$profile")
fi

ensure_static_bucket() {
    if aws s3api head-bucket --bucket "$static_bucket" --region "$region" --profile "$profile" >/dev/null 2>&1; then
        return
    fi

    echo "Creating S3 bucket: $static_bucket"
    if [[ "$region" == "us-east-1" ]]; then
        aws s3api create-bucket \
            --bucket "$static_bucket" \
            --region "$region" \
            --profile "$profile" >/dev/null
    else
        aws s3api create-bucket \
            --bucket "$static_bucket" \
            --create-bucket-configuration "LocationConstraint=${region}" \
            --region "$region" \
            --profile "$profile" >/dev/null
    fi

    aws s3api wait bucket-exists \
        --bucket "$static_bucket" \
        --region "$region" \
        --profile "$profile"
}

configure_private_static_bucket() {
    ensure_static_bucket

    aws s3api put-public-access-block \
        --bucket "$static_bucket" \
        --public-access-block-configuration \
            BlockPublicAcls=true,IgnorePublicAcls=true,BlockPublicPolicy=false,RestrictPublicBuckets=false \
        --region "$region" \
        --profile "$profile"
}

put_cloudfront_bucket_policy() {
    local distribution_id_for_policy="$1"
    local account_id
    local bucket_policy

    account_id="$(aws sts get-caller-identity \
        --profile "$profile" \
        --query 'Account' \
        --output text)"

    bucket_policy="$(mktemp)"
    cat > "$bucket_policy" <<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "AllowCloudFrontServicePrincipalReadOnly",
      "Effect": "Allow",
      "Principal": {
        "Service": "cloudfront.amazonaws.com"
      },
      "Action": "s3:GetObject",
      "Resource": "arn:aws:s3:::${static_bucket}/*",
      "Condition": {
        "StringEquals": {
          "AWS:SourceArn": "arn:aws:cloudfront::${account_id}:distribution/${distribution_id_for_policy}"
        }
      }
    }
  ]
}
EOF

    aws s3api put-bucket-policy \
        --bucket "$static_bucket" \
        --policy "file://${bucket_policy}" \
        --region "$region" \
        --profile "$profile"

    aws s3api put-public-access-block \
        --bucket "$static_bucket" \
        --public-access-block-configuration \
            BlockPublicAcls=true,IgnorePublicAcls=true,BlockPublicPolicy=true,RestrictPublicBuckets=true \
        --region "$region" \
        --profile "$profile"

    rm -f "$bucket_policy"
}

ensure_cloudfront_distribution() {
    if [[ -z "$certificate_arn" ]]; then
        echo "--certificate-arn is required with --ensure-cloudfront." >&2
        echo "CloudFront certificates must be issued by ACM in us-east-1." >&2
        exit 2
    fi

    configure_private_static_bucket

    local existing_distribution_id
    existing_distribution_id="$(aws cloudfront list-distributions "${cloudfront_args[@]}" \
        --query "DistributionList.Items[?Aliases.Items && contains(Aliases.Items, '${static_domain}')].Id | [0]" \
        --output text)"

    local function_name="keyboxw-${stage}-static-rewrite"
    local function_js
    function_js="$(mktemp)"
    cat > "$function_js" <<'EOF'
function handler(event) {
    var request = event.request;
    var uri = request.uri;

    if (uri.endsWith('/')) {
        request.uri += 'index.html';
    } else if (!uri.includes('.')) {
        request.uri += '/index.html';
    }

    return request;
}
EOF

    local function_arn=""
    local function_etag
    if aws cloudfront describe-function --name "$function_name" "${cloudfront_args[@]}" >/dev/null 2>&1; then
        function_etag="$(aws cloudfront describe-function --name "$function_name" "${cloudfront_args[@]}" \
            --query 'ETag' \
            --output text)"
        function_etag="$(aws cloudfront update-function \
            --name "$function_name" \
            --if-match "$function_etag" \
            --function-config Comment="KeyBox static index rewrite",Runtime=cloudfront-js-1.0 \
            --function-code "fileb://${function_js}" \
            "${cloudfront_args[@]}" \
            --query 'ETag' \
            --output text)"
    else
        function_etag="$(aws cloudfront create-function \
            --name "$function_name" \
            --function-config Comment="KeyBox static index rewrite",Runtime=cloudfront-js-1.0 \
            --function-code "fileb://${function_js}" \
            "${cloudfront_args[@]}" \
            --query 'ETag' \
            --output text)"
    fi

    aws cloudfront publish-function \
        --name "$function_name" \
        --if-match "$function_etag" \
        "${cloudfront_args[@]}" >/dev/null

    function_arn="$(aws cloudfront describe-function --name "$function_name" "${cloudfront_args[@]}" \
        --stage LIVE \
        --query 'FunctionSummary.FunctionMetadata.FunctionARN' \
        --output text)"

    rm -f "$function_js"

    local oac_name="keyboxw-${stage}-static-oac"
    local oac_id
    oac_id="$(aws cloudfront list-origin-access-controls "${cloudfront_args[@]}" \
        --query "OriginAccessControlList.Items[?Name=='${oac_name}'].Id | [0]" \
        --output text)"

    if [[ -z "$oac_id" || "$oac_id" == "None" ]]; then
        oac_id="$(aws cloudfront create-origin-access-control \
            --origin-access-control-config \
                "Name=${oac_name},Description=KeyBox ${stage} static S3 access,SigningProtocol=sigv4,SigningBehavior=always,OriginAccessControlOriginType=s3" \
            "${cloudfront_args[@]}" \
            --query 'OriginAccessControl.Id' \
            --output text)"
    fi

    local caller_reference="keyboxw-${stage}-static-$(date +%s)"
    local distribution_etag=""
    if [[ -n "$existing_distribution_id" && "$existing_distribution_id" != "None" ]]; then
        distribution_id="$existing_distribution_id"
        caller_reference="$(aws cloudfront get-distribution-config \
            --id "$distribution_id" \
            "${cloudfront_args[@]}" \
            --query 'DistributionConfig.CallerReference' \
            --output text)"
        distribution_etag="$(aws cloudfront get-distribution-config \
            --id "$distribution_id" \
            "${cloudfront_args[@]}" \
            --query 'ETag' \
            --output text)"
    fi

    local origin_domain="${static_bucket}.s3.${region}.amazonaws.com"
    local config_file
    config_file="$(mktemp)"
    cat > "$config_file" <<EOF
{
  "CallerReference": "${caller_reference}",
  "Comment": "KeyBox ${stage} static site",
  "Enabled": true,
  "Aliases": {
    "Quantity": 1,
    "Items": ["${static_domain}"]
  },
  "Origins": {
    "Quantity": 1,
    "Items": [
      {
        "Id": "s3-${static_bucket}",
        "DomainName": "${origin_domain}",
        "OriginPath": "",
        "OriginAccessControlId": "${oac_id}",
        "CustomHeaders": {
          "Quantity": 0
        },
        "S3OriginConfig": {
          "OriginAccessIdentity": ""
        }
      }
    ]
  },
  "DefaultCacheBehavior": {
    "TargetOriginId": "s3-${static_bucket}",
    "ViewerProtocolPolicy": "redirect-to-https",
    "AllowedMethods": {
      "Quantity": 2,
      "Items": ["GET", "HEAD"],
      "CachedMethods": {
        "Quantity": 2,
        "Items": ["GET", "HEAD"]
      }
    },
    "TrustedSigners": {
      "Enabled": false,
      "Quantity": 0
    },
    "TrustedKeyGroups": {
      "Enabled": false,
      "Quantity": 0
    },
    "SmoothStreaming": false,
    "LambdaFunctionAssociations": {
      "Quantity": 0
    },
    "FieldLevelEncryptionId": "",
    "Compress": true,
    "CachePolicyId": "658327ea-f89d-4fab-a63d-7e88639e58f6",
    "FunctionAssociations": {
      "Quantity": 1,
      "Items": [
        {
          "EventType": "viewer-request",
          "FunctionARN": "${function_arn}"
        }
      ]
    }
  },
  "CacheBehaviors": {
    "Quantity": 0
  },
  "CustomErrorResponses": {
    "Quantity": 0
  },
  "DefaultRootObject": "index.html",
  "Logging": {
    "Enabled": false,
    "IncludeCookies": false,
    "Bucket": "",
    "Prefix": ""
  },
  "ViewerCertificate": {
    "ACMCertificateArn": "${certificate_arn}",
    "SSLSupportMethod": "sni-only",
    "MinimumProtocolVersion": "TLSv1.2_2021"
  },
  "Restrictions": {
    "GeoRestriction": {
      "RestrictionType": "none",
      "Quantity": 0
    }
  },
  "WebACLId": "",
  "PriceClass": "PriceClass_100",
  "HttpVersion": "http2",
  "IsIPV6Enabled": true,
  "Staging": false
}
EOF

    if [[ -n "$existing_distribution_id" && "$existing_distribution_id" != "None" ]]; then
        aws cloudfront update-distribution \
            --id "$distribution_id" \
            --if-match "$distribution_etag" \
            --distribution-config "file://${config_file}" \
            "${cloudfront_args[@]}" >/dev/null
        echo "Updated CloudFront distribution $distribution_id for $static_domain."
    else
        distribution_id="$(aws cloudfront create-distribution \
            --distribution-config "file://${config_file}" \
            "${cloudfront_args[@]}" \
            --query 'Distribution.Id' \
            --output text)"
        echo "Created CloudFront distribution $distribution_id for $static_domain."
    fi

    local distribution_domain
    distribution_domain="$(aws cloudfront get-distribution \
        --id "$distribution_id" \
        "${cloudfront_args[@]}" \
        --query 'Distribution.DomainName' \
        --output text)"

    put_cloudfront_bucket_policy "$distribution_id"

    rm -f "$config_file"

    echo "Add Route 53 A/AAAA alias records for $static_domain -> $distribution_domain."
}

if [[ "$deploy_lambda" -eq 1 ]]; then
    sam build --cached --parallel

    sam_deploy_args=(
        --stack-name "keyboxw-${stage}"
        --s3-prefix "keyboxw-${stage}"
        --region "$region"
        --profile "$profile"
        --capabilities CAPABILITY_IAM
        --resolve-s3
        --no-fail-on-empty-changeset
        --parameter-overrides "STAGE=${stage}"
    )

    if [[ "$confirm_changeset" -eq 1 ]]; then
        sam_deploy_args+=(--confirm-changeset)
    else
        sam_deploy_args+=(--no-confirm-changeset)
    fi

    sam deploy "${sam_deploy_args[@]}"
fi

if [[ "$ensure_cloudfront" -eq 1 ]]; then
    ensure_cloudfront_distribution
fi

if [[ "$deploy_static" -eq 1 ]]; then
    ensure_static_bucket

    static_args=(
        --bucket "$static_bucket"
        --source "$static_source"
        --region "$region"
        --profile "$profile"
    )

    if [[ -n "$static_prefix" ]]; then
        static_args+=(--prefix "$static_prefix")
    fi

    if [[ -n "$distribution_id" ]]; then
        static_args+=(--distribution-id "$distribution_id")
    fi

    if [[ "$gzip_static" -eq 0 ]]; then
        static_args+=(--no-gzip)
    fi

    "$script_dir/deploy_static_site.sh" "${static_args[@]}"
fi

validate_url() {
    local label="$1"
    local url="$2"
    local response_body
    local http_status

    echo "Validating ${label}: $url"
    response_body="$(mktemp)"
    if ! http_status="$(curl --silent --show-error --output "$response_body" --write-out '%{http_code}' --max-time 20 "$url")"; then
        echo "${label} validation request failed for $url." >&2
        cat "$response_body" >&2
        rm -f "$response_body"
        exit 1
    fi

    if [[ "$http_status" -lt 200 || "$http_status" -ge 300 ]]; then
        echo "${label} validation failed for $url with HTTP $http_status." >&2
        echo "Response body:" >&2
        cat "$response_body" >&2
        rm -f "$response_body"
        exit 1
    fi

    rm -f "$response_body"
    echo "${label} validation passed for $url with HTTP $http_status."
}

if [[ "$run_validation" -eq 1 ]]; then
    if ! command -v curl >/dev/null 2>&1; then
        echo "curl was not found. Install curl or use --skip-validation." >&2
        exit 1
    fi

    if [[ "$deploy_lambda" -eq 1 ]]; then
        validate_url "API endpoint" "$validation_url"
    fi

    if [[ "$deploy_static" -eq 1 ]]; then
        validate_url "static page" "$static_validation_url"
    fi
fi

echo "Keybox ${stage} deployment complete."
