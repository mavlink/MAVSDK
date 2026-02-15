#!/usr/bin/env python3
import argparse
import jinja2

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plugins", nargs="+", help="enabled plugins")
    args = parser.parse_args()
    
    plugins = args.plugins
    
    env = jinja2.Environment(loader=jinja2.FileSystemLoader('templates'))
    template = env.get_template("plugins_generated.cmake.j2")
    output = template.render(plugins=plugins)
    
    with open("src/plugins_generated.cmake", 'w') as f:
        f.write(output)
    
    print(f"Successfully wrote to src/plugins_generated.cmake")

if __name__ == '__main__':
    main()
