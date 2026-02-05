#!/usr/bin/env python3

import argparse
import jinja2

def snake_case_to_pascal_case(string):
    return string.replace("_", " ").title().replace(" ", "")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plugins", nargs="+", help="enabled plugins")
    args = parser.parse_args()
    plugins = vars(args)['plugins']

    env = jinja2.Environment(loader=jinja2.FileSystemLoader('templates'))
    env.filters["snake_case_to_pascal_case"] = snake_case_to_pascal_case

    template = env.get_template("grpc_server.h.j2")
    output = template.render(plugins = plugins)
    with open("src/mavsdk_server/src/grpc_server.h", 'w') as f:
        f.write(output)

    template = env.get_template("grpc_server.cpp.j2")
    output = template.render(plugins = plugins)
    with open("src/mavsdk_server/src/grpc_server.cpp", 'w') as f:
        f.write(output)

if __name__ == '__main__':
    main()
