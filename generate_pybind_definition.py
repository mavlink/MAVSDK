#!/usr/bin/env python


import re
import argparse
from string import Template


class Method(object):
    def __init__(self):
        self.return_type = ""
        self.method_name = ""
        self.arguments = ""

class HeaderParser(object):

    def __init__(self):
        self.methods = []
        self.output = ""

    def parse_plugin_header(self, lines):
        part = ""
        new_definition = True
        for i, line in enumerate(lines):
            line = line.strip()

            if not line:
                continue

            if line.startswith("/*") or \
                    line.startswith("*") or \
                    line.startswith("#"):
                continue

            if "namespace" in line or \
                    "typedef" in line or \
                    "class" in line or \
                    "unique_ptr" in line or \
                    "public" in line or \
                    "private" in line or \
                    "~" in line or \
                    "delete" in line or \
                    "explicit" in line or \
                    "}" in line or \
                    "callback" in line or \
                    "async" in line:
                continue

            if line.endswith(";"):
                new_definition = True
            else:
                new_definition = False

            part += line + " "

            if new_definition:
                self.parse_part(part)
                part = ""

    def parse_part(self, part):
        p = re.compile('^([\w][\w\d_:]*(?:<[\w\d,: ]*>)?)\s*([\w][\w\d_]*)\(([\w\d, ]*)\)\s*.*?$')
        m = p.match(part)
        if m:
            if len(m.groups()) != 3:
                raise Exception("Unknown number of groups {} for {}"
                                .format(m.groups(), part))

            new_method = Method()
            new_method.return_type = m.groups(0)[0]
            new_method.method_name = m.groups(0)[1]
            new_method.arguments = m.groups(0)[1]
            self.methods.append(new_method)
        else:
            raise Exception("Could not apply regex to '{}'".format(part))

    def get_pybind_entry(self):
        self.output += '    py::class_<Action>(m, "Action", plugin_base)\n'
        self.output += '        .def(py::init<System &>())\n'

        for i, method in enumerate(self.methods):
            self.output += '        .def("{0}", &Action::{0})'.format(method.method_name)
            if i + 1 == len(self.methods):
                self.output += ";\n"
            else:
                self.output += "\n"
        return self.output



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--headers',
                        nargs='+',
                        help="List all header files to parse",
                        required=True)
    parser.add_argument('--template',
                        help="Template to use for pybind source output",
                        required=True)
    parser.add_argument('--output',
                        help="Where to put pybind source output",
                        required=True)

    args = parser.parse_args()

    for header in args.headers:
        with open(header, "r") as f:
            lines = f.readlines()
            header_parser = HeaderParser()
            header_parser.parse_plugin_header(lines)

    with open(args.template, 'r') as f:
        template = f.read()

    s = Template(template)
    filled_template = s.substitute(plugins=header_parser.get_pybind_entry())

    with open(args.output, 'w') as f:
        f.write(filled_template)


if __name__ == '__main__':
    main()
