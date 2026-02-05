#!/usr/bin/env python3
"""
Script to clean up old PR-specific Docker images from Docker Hub.
Identifies images by tag pattern (defaults to 'pr-') and removes those older
than a specified age.

Requirements:
    - Python 3.6+
    - requests package

Environment variables:
    - DOCKERHUB_USERNAME: Docker Hub username (for authentication)
    - DOCKERHUB_PASSWORD: Docker Hub token or password (for authentication)
    - TARGET_ORG: Target organization to clean (default: 'mavsdk')
    - MAX_AGE_DAYS: Maximum age in days (default: 7)
    - DRY_RUN: Set to 'true' to only print what would be deleted, without
               actually deleting (default: 'false')
"""


import os
import sys
import requests
import time
from datetime import datetime, timedelta


# Configure from environment variables
DOCKERHUB_USERNAME = os.environ.get('DOCKERHUB_USERNAME')
DOCKERHUB_PASSWORD = os.environ.get('DOCKERHUB_PASSWORD')
TARGET_ORG = os.environ.get('TARGET_ORG', 'mavsdk')
MAX_AGE_DAYS = int(os.environ.get('MAX_AGE_DAYS', 7))
TAG_PREFIX = os.environ.get('TAG_PREFIX', 'pr-')
DRY_RUN = os.environ.get('DRY_RUN', 'false').lower() == 'true'


# Validate required environment variables
if not all([DOCKERHUB_USERNAME, DOCKERHUB_PASSWORD]):
    print("Error: Required environment variables missing.")
    print("Please set DOCKERHUB_USERNAME and DOCKERHUB_PASSWORD.")
    sys.exit(1)


# Docker Hub API endpoints
DOCKER_HUB_API = 'https://hub.docker.com/v2'
LOGIN_URL = f'{DOCKER_HUB_API}/users/login'


def login():
    """Authenticate with Docker Hub and return the JWT token."""
    login_data = {
        'username': DOCKERHUB_USERNAME,
        'password': DOCKERHUB_PASSWORD,
    }

    try:
        response = requests.post(LOGIN_URL, data=login_data)
        response.raise_for_status()
        return response.json()['token']
    except requests.exceptions.HTTPError as e:
        print(f"Authentication failed: {e}")
        sys.exit(1)


def get_all_repositories(token):
    """Retrieve all repositories for the target organization."""
    headers = {'Authorization': f'JWT {token}'}
    all_repos = []
    page = 1

    # Use the correct endpoint for the target organization
    base_url = f'{DOCKER_HUB_API}/repositories/{TARGET_ORG}'
    print(f"Looking for repositories under: {TARGET_ORG}")

    while True:
        try:
            response = requests.get(
                f"{base_url}?page={page}&page_size=100", headers=headers)
            response.raise_for_status()
            data = response.json()

            if not data['results']:
                break

            all_repos.extend(data['results'])

            if not data['next']:
                break

            page += 1

        except requests.exceptions.HTTPError as e:
            print(f"Error retrieving repositories for {TARGET_ORG}: {e}")
            if e.response.status_code == 404:
                print(f"Organization '{TARGET_ORG}' not found or no access")
            return []

    return all_repos


def get_all_tags(token, repo_name):
    """Retrieve all tags for the specified repository."""
    headers = {'Authorization': f'JWT {token}'}
    repo_url = f'{DOCKER_HUB_API}/repositories/{TARGET_ORG}/{repo_name}/tags'
    all_tags = []
    page = 1

    while True:
        try:
            response = requests.get(
                f"{repo_url}?page={page}&page_size=100", headers=headers)
            response.raise_for_status()
            data = response.json()

            if not data['results']:
                break

            all_tags.extend(data['results'])

            if not data['next']:
                break

            page += 1

        except requests.exceptions.HTTPError as e:
            print(f"Error retrieving tags for {repo_name}: {e}")
            return []

    return all_tags


def delete_tag(token, repo_name, tag_name):
    """Delete a specific tag from the repository."""
    headers = {'Authorization': f'JWT {token}'}
    delete_url = f'{DOCKER_HUB_API}/repositories/{TARGET_ORG}/' \
                 f'{repo_name}/tags/{tag_name}'

    try:
        response = requests.delete(delete_url, headers=headers)
        response.raise_for_status()
        return True
    except requests.exceptions.HTTPError as e:
        print(f"Error deleting tag {tag_name} from {repo_name}: {e}")
        return False


def process_repository(token, repo_name, cutoff_date):
    """Process a single repository to find and delete old PR tags."""
    print(f"\nProcessing repository: {TARGET_ORG}/{repo_name}")

    # Get all tags for this repository
    all_tags = get_all_tags(token, repo_name)
    if not all_tags:
        print(f"  No tags found or error accessing repository {repo_name}")
        return 0

    print(f"  Found {len(all_tags)} total tags")

    # Filter PR tags older than cutoff date
    pr_tags = [tag for tag in all_tags if tag['name'].startswith(TAG_PREFIX)]
    print(f"  Found {len(pr_tags)} tags with prefix '{TAG_PREFIX}'")

    if not pr_tags:
        print(f"  No PR tags found in {repo_name}")
        return 0

    old_pr_tags = []
    for tag in pr_tags:
        try:
            last_updated = datetime.strptime(tag['last_updated'],
                                             '%Y-%m-%dT%H:%M:%S.%fZ')
            if last_updated < cutoff_date:
                old_pr_tags.append((tag['name'], last_updated))
        except ValueError as e:
            print(f"  Warning: Could not parse date {tag['last_updated']} "
                  f"for tag {tag['name']}: {e}")
            continue

    print(f"  Found {len(old_pr_tags)} tags older than {MAX_AGE_DAYS} days")

    if not old_pr_tags:
        return 0

    # Sort by oldest first
    old_pr_tags.sort(key=lambda x: x[1])

    print(f"  Tags to be deleted from {repo_name}:")
    for tag_name, updated_date in old_pr_tags:
        print(f"    - {tag_name} (last updated: "
              f"{updated_date.strftime('%Y-%m-%d %H:%M:%S')})")

    if DRY_RUN:
        print(f"  DRY RUN: Would delete {len(old_pr_tags)} tags "
              f"from {repo_name}")
        return len(old_pr_tags)

    # Delete old PR tags
    print(f"  Deleting tags from {repo_name}...")
    deleted_count = 0
    for tag_name, _ in old_pr_tags:
        print(f"    Deleting {tag_name}...", end="")
        if delete_tag(token, repo_name, tag_name):
            print(" ✅")
            deleted_count += 1
        else:
            print(" ❌")
        # Brief pause to avoid rate limiting
        time.sleep(0.5)

    print(f"  Deleted {deleted_count} of {len(old_pr_tags)} "
          f"tags from {repo_name}")
    return deleted_count


def main():
    """Main function to clean up old Docker images."""
    cutoff_date = datetime.now() - timedelta(days=MAX_AGE_DAYS)
    print(f"Cleaning up Docker Hub repositories for org: {TARGET_ORG}")
    print(f"Looking for images with tag prefix '{TAG_PREFIX}' "
          f"older than {MAX_AGE_DAYS} days "
          f"({cutoff_date.strftime('%Y-%m-%d')})")

    if DRY_RUN:
        print("DRY RUN MODE: No images will be deleted")

    # Login to Docker Hub
    token = login()
    print(f"Successfully authenticated to Docker Hub as {DOCKERHUB_USERNAME}")

    # Get all repositories for the target organization
    all_repositories = get_all_repositories(token)
    print(f"Found {len(all_repositories)} repositories under {TARGET_ORG}")

    if not all_repositories:
        print(f"No repositories found under organization '{TARGET_ORG}'.")
        print("\nPossible reasons:")
        print("1. The organization name is incorrect")
        print("2. You don't have access to the organization's repositories")
        print("3. The organization has no public repositories")
        print(f"\nTo target a different org, set TARGET_ORG env variable")
        return

    # Process each repository
    total_deleted = 0
    repositories_processed = 0

    for repo in all_repositories:
        repo_name = repo['name']
        deleted_count = process_repository(token, repo_name, cutoff_date)
        total_deleted += deleted_count
        repositories_processed += 1

    print(f"\n=== SUMMARY ===")
    print(f"Organization: {TARGET_ORG}")
    print(f"Repositories processed: {repositories_processed}")
    if DRY_RUN:
        print(f"Total tags that would be deleted: {total_deleted}")
        print("DRY RUN: No tags were actually deleted. "
              "Set DRY_RUN=false to actually delete tags.")
    else:
        print(f"Total tags deleted: {total_deleted}")


if __name__ == "__main__":
    main()
