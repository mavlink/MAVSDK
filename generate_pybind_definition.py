#!/usr/bin/env python


import re


class Method(object):
    def __init__(self):
        self.return_type = ""
        self.method_name = ""
        self.arguments = ""

class Parser(object):

    def __init__(self):
        self.methods = []

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

    def print_pybind_entry(self):
        print('    py::class_<Action>(m, "Action", plugin_base)')
        print('        .def(py::init<System &>())')

        for i, method in enumerate(self.methods):
            print('        .def("{0}", &Action::{0})'
                  .format(method.method_name), end="")
            if i + 1 == len(self.methods):
                print(";")
            else:
                print()



def main():
    with open("plugins/action/include/plugins/action/action.h", "r") as f:
        lines = f.readlines()
        parser = Parser()
        parser.parse_plugin_header(lines)
        parser.print_pybind_entry()


if __name__ == '__main__':
    main()
