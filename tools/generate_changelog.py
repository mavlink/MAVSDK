#!/usr/bin/env python

from gql import gql, Client
from gql.transport.requests import RequestsHTTPTransport

import argparse
import dateutil.parser
import datetime
import json

debug = False

def create_gql_client(token):
    transport = RequestsHTTPTransport(url='https://api.github.com/graphql', use_json=True)
    transport.headers = { "Authorization": "Bearer {}".format(token) }
    client = Client(transport=transport)

    return client

def query_releases(gql_client, debug=False):
    query_releases = gql('''
            {
              repository(owner: "mavlink", name: "MAVSDK") {
                releases(last: 10) {
                  nodes {
                    name
                    createdAt
                    tag {
                      name
                    }
                  }
                }
              }
            }
            ''')


    releases = gql_client.execute(query_releases)
    releases = releases["repository"]["releases"]["nodes"]

    if debug:
        print(json.dumps(releases, indent=2))

    return releases

def plus_one_minute(date_str):
    date = dateutil.parser.parse(date_str)
    date += datetime.timedelta(minutes=1)
    return date.isoformat().replace('+00:00', 'Z')

def query_pull_requests(gql_client, releases, release_tag, debug=False):
    previous_wanted_release = None
    wanted_release = None
    for release in releases:
        previous_wanted_release = wanted_release
        wanted_release = release

        if not release["tag"]:
            continue
        if release["tag"]["name"] == release_tag:
            break

    if debug:
        print(previous_wanted_release)
        print(wanted_release)

    merged_predicate = None
    if previous_wanted_release is None:
        merged_predicate = f"<={wanted_release['createdAt']}"
    else:
        # We add one minute because otherwise the last PR of the previous release
        # tends to get included as well.
        merged_predicate = f"{plus_one_minute(previous_wanted_release['createdAt'])[:-4]}..{wanted_release['createdAt'][:-4]}"

    query_pull_requests_str = '''
            {{
              search(query: "repo:mavlink/MAVSDK is:pr is:merged merged:{merged_predicate}", type: ISSUE, last: 100) {{
                edges {{
                  node {{
                    ... on PullRequest {{
                      url
                      number
                      title
                      author {{
                        login
                      }}
                      reviews(last: 15) {{
                        nodes {{
                          author {{
                            login
                          }}
                        }}
                      }}
                      labels(last: 1) {{
                        edges {{
                          node {{
                            name
                          }}
                        }}
                      }}
                      mergedAt
                    }}
                  }}
                }}
              }}
            }}
            '''.format(merged_predicate=merged_predicate)

    if debug:
        print(query_pull_requests_str)

    query_pull_requests = gql(query_pull_requests_str)
    results_query = gql_client.execute(query_pull_requests)
    result_pull_requests = list(map(lambda node: node["node"], results_query["search"]["edges"]))

    if debug:
        print(json.dumps(result_pull_requests, indent=2))

    return result_pull_requests

def filter_by_label(pull_requests, label):
    if debug:
        print(json.dumps(pull_requests, indent=2))

    for pull_request in pull_requests:
        if (len(pull_request["labels"]["edges"]) == 0):
            print(f"ERROR: PR is missing a label: {pull_request['url']}")
            exit(1)

    return list(filter(lambda pull_request: pull_request["labels"]["edges"][0]["node"]["name"] == label, pull_requests))

def collect_contributors(pull_requests):
    contributors = []

    for pull_request in pull_requests:
        author = pull_request["author"]["login"]
        contributors.append(f"@{author}")

    contributors = set(contributors)
    contributors = sorted(list(contributors), key=str.lower)

    return contributors

def print_markdown_output(features, bugfixes, enhancements, documentation, contributors):
    print("-----------")
    print("CHANGELOG")
    print("-----------")

    if features:
        print("### Features:\n")

        for feature in features:
            print(f"* {feature['title']} (#{feature['number']}).")

    if bugfixes:
        print("\n### Bugfixes:\n")

        for bugfix in bugfixes:
            print(f"* {bugfix['title']} (#{bugfix['number']}).")

    if enhancements:
        print("\n### Enhancements:\n")

        for enhancement in enhancements:
            print(f"* {enhancement['title']} (#{enhancement['number']}).")

    if documentation:
        print("\n### Documentation:\n")

        for doc in documentation:
            print(f"* {doc['title']} (#{doc['number']}).")

    if contributors:
        print("\n### Contributors:\n")

        print(', '.join(contributors))

def main():
    parser = argparse.ArgumentParser(description='Generate a changelog from pull requests between a given release tag and the previous one.')
    parser.add_argument("--tag", help="The release tag for which the changelog should be generated")
    parser.add_argument("--token", help="The token to access MAVSDK's GitHub API.")
    parser.add_argument("--verbose", type=bool, nargs='?', const=True, default=False, help="Enable verbose output.")

    args = parser.parse_args()

    gql_client = create_gql_client(args.token)
    releases = query_releases(gql_client, debug=args.verbose)
    pull_requests = query_pull_requests(gql_client, releases, args.tag, debug=args.verbose)

    features = filter_by_label(pull_requests, "feature")
    bugfixes = filter_by_label(pull_requests, "bug")
    enhancements = filter_by_label(pull_requests, "enhancement")
    documentation = filter_by_label(pull_requests, "documentation")
    contributors = collect_contributors(pull_requests)

    print_markdown_output(features, bugfixes, enhancements, documentation, contributors)

if __name__ == '__main__':
    main()
