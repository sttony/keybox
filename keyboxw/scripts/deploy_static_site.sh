#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Deploy Keybox static web content to S3.

Usage:
  scripts/deploy_static_site.sh --bucket BUCKET [options]

Options:
  --source DIR                 Static content directory. Default: static_content
  --prefix PREFIX              Optional S3 key prefix.
  --region REGION              AWS region passed to aws cli.
  --profile PROFILE            AWS profile passed to aws cli.
  --distribution-id ID         CloudFront distribution to invalidate.
  --no-gzip                    Upload source files without precompressing assets.
  -h, --help                   Show this help.

Examples:
  scripts/deploy_static_site.sh --bucket beta-keybox-static
  scripts/deploy_static_site.sh --bucket keybox-static --profile default --distribution-id E1234567890
EOF
}

bucket=""
source_dir="static_content"
prefix=""
region=""
profile=""
distribution_id=""
gzip_assets=1

while [[ $# -gt 0 ]]; do
    case "$1" in
        --bucket)
            bucket="${2:-}"
            shift 2
            ;;
        --source)
            source_dir="${2:-}"
            shift 2
            ;;
        --prefix)
            prefix="${2:-}"
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
        --distribution-id)
            distribution_id="${2:-}"
            shift 2
            ;;
        --no-gzip)
            gzip_assets=0
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

if [[ -z "$bucket" ]]; then
    echo "Missing required --bucket value." >&2
    usage >&2
    exit 2
fi

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd "$script_dir/.." && pwd)"
source_path="$project_dir/$source_dir"

if [[ ! -d "$source_path" ]]; then
    echo "Static content directory not found: $source_path" >&2
    exit 1
fi

aws_args=()
if [[ -n "$region" ]]; then
    aws_args+=(--region "$region")
fi
if [[ -n "$profile" ]]; then
    aws_args+=(--profile "$profile")
fi

if ! aws s3api head-bucket --bucket "$bucket" "${aws_args[@]}" >/dev/null 2>&1; then
    echo "Creating S3 bucket: $bucket"
    if [[ "${region:-us-east-1}" == "us-east-1" ]]; then
        aws s3api create-bucket --bucket "$bucket" "${aws_args[@]}" >/dev/null
    else
        aws s3api create-bucket \
            --bucket "$bucket" \
            --create-bucket-configuration "LocationConstraint=${region}" \
            "${aws_args[@]}" >/dev/null
    fi

    aws s3api wait bucket-exists --bucket "$bucket" "${aws_args[@]}"
fi

destination="s3://$bucket"
if [[ -n "$prefix" ]]; then
    prefix="${prefix#/}"
    prefix="${prefix%/}"
    destination="$destination/$prefix"
fi

deploy_dir="$source_path"
tmp_dir=""
compressed_sync_args=()

cleanup() {
    if [[ -n "$tmp_dir" && -d "$tmp_dir" ]]; then
        rm -rf "$tmp_dir"
    fi
}
trap cleanup EXIT

if [[ "$gzip_assets" -eq 1 ]]; then
    if ! command -v gzip >/dev/null 2>&1; then
        echo "gzip was not found. Install gzip or use --no-gzip." >&2
        exit 1
    fi

    tmp_dir="$(mktemp -d)"
    cp -R "$source_path"/. "$tmp_dir"/

    while IFS= read -r -d '' file; do
        gzip -9 -n "$file"
        mv "$file.gz" "$file"
    done < <(find "$tmp_dir" -type f \( \
        -name '*.css' -o \
        -name '*.js' -o \
        -name '*.html' -o \
        -name '*.json' -o \
        -name '*.svg' -o \
        -name '*.txt' \
    \) -print0)

    deploy_dir="$tmp_dir"
    compressed_sync_args+=(--content-encoding gzip)
fi

aws s3 sync "$deploy_dir" "$destination" "${aws_args[@]}" \
    --delete \
    --exclude '*' \
    --include '*.html' \
    --cache-control 'no-cache, no-store, must-revalidate' \
    "${compressed_sync_args[@]}"

aws s3 sync "$deploy_dir" "$destination" "${aws_args[@]}" \
    --delete \
    --exclude '*' \
    --include '*.css' \
    --include '*.js' \
    --include '*.json' \
    --include '*.svg' \
    --include '*.txt' \
    --exclude '*.html' \
    --cache-control 'public, max-age=31536000, immutable' \
    "${compressed_sync_args[@]}"

aws s3 sync "$deploy_dir" "$destination" "${aws_args[@]}" \
    --delete \
    --exclude '*.html' \
    --exclude '*.css' \
    --exclude '*.js' \
    --exclude '*.json' \
    --exclude '*.svg' \
    --exclude '*.txt' \
    --cache-control 'public, max-age=31536000, immutable'

if [[ -n "$distribution_id" ]]; then
    aws cloudfront create-invalidation "${aws_args[@]}" \
        --distribution-id "$distribution_id" \
        --paths '/*'
fi

echo "Static content deployed to $destination"
