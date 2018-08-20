#!/usr/bin/env python


import re
import argparse
from string import Template

verbose = False

def debug(string):
    if verbose:
        print(string)

class Method(object):
    def __init__(self):
        self.return_type = ""
        self.method_name = ""
        self.arguments = ""

class Struct(object):
    def __init__(self):
        self.name = ""
        self.fields = []

class Enum(object):
    def __init__(self):
        self.name = ""
        self.values = []

class HeaderParser(object):
    def __init__(self):
        self.methods = []
        self.structs = []
        self.enums = []
        self.output = ""
        self.class_name = ""
        self.parent_name = None

    def parse_plugin_header(self, lines):
        part = ""
        new_definition = True
        inside_struct = False

        self.determine_class_name(lines)

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
                    "System" in line or \
                    "Impl" in line or \
                    "unique_ptr" in line or \
                    "public" in line or \
                    "private" in line or \
                    "~" in line or \
                    "delete" in line or \
                    "explicit" in line or \
                    "callback" in line or \
                    "async" in line or \
                    "static" in line or \
                    "class " + self.class_name in line or \
                    self.class_name + "()" in line or \
                    "operator" in line:
                continue

            if "struct {" in line:
                inside_struct = True

            if "};" in line:
                inside_struct = False

            if line.endswith(";") and not inside_struct:
                new_definition = True
            else:
                new_definition = False

            if "}" in line:
                continue

            part += line + " "

            if new_definition:
                self.parse_part(part)
                part = ""

    def determine_class_name(self, lines):
        for line in lines:
            if "class" in line and not ";" in line:
                p = re.compile('^class\s*([\w\d]*)\s*:\s*public\s*([\w\d]*)\s{$')
                m = p.match(line)
                if m:
                    self.class_name = m.groups()[0]
                    self.parent_name = m.groups()[1]
                    debug("Found class: {} and parent: {}".
                          format(self.class_name, self.parent_name))
                    return

                p = re.compile('^class\s*([\w\d]*)\s*.*?$')
                m = p.match(line)
                if m:
                    self.class_name = m.groups()[0]
                    debug("Found class name: {}".
                          format(self.class_name))
                    return

        raise Exception("Could not find class name")

    def find_method(self, part):
        p = re.compile('^([\w][\w\d_:]*(?:<[\w\d,: ]*>)?)\s*([\w][\w\d_]*)\(([\w\d, ]*)\)\s*.*?$')
        m = p.match(part)
        if m:
            if len(m.groups()) != 3:
                raise Exception("Unknown number of groups {} for {}"
                                .format(m.groups(), part))

            new_method = Method()
            new_method.return_type = m.groups(0)[0]
            new_method.method_name = m.groups(0)[1]
            new_method.arguments = m.groups(0)[2]
            self.methods.append(new_method)
            debug("Found method '{} {}({})'"
                  .format(new_method.return_type,
                          new_method.method_name,
                          new_method.arguments))
            return True
        else:
            return False

    def find_enum(self, part):
        if "enum" in part:
            return True
        else:
            return False


    def parse_part(self, part):
        if not self.find_method(part) and \
                not self.find_enum(part):
            raise Exception("Could not parse '{}'".format(part))


    def get_pybind_entry(self):
        if self.parent_name == "PluginBase":
            self.output += '    py::class_<{0}>(m, "{0}", plugin_base)\n'.format(self.class_name)
            self.output += '        .def(py::init<System &>())\n'
        elif self.parent_name is not None:
            raise Exception("Can't deal with parent other than PluginBase")
        else:
            self.output += '    py::class_<{0}>(m, "{0}")\n'.format(self.class_name)
            self.output += '        .def(py::init<>())\n'

        for i, method in enumerate(self.methods):
            self.output += '        .def("{0}", &{1}::{0})'.format(method.method_name, self.class_name)
            if i + 1 == len(self.methods):
                self.output += ";\n\n"
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
    parser.add_argument('--verbose',
                        help="Be more verbose",
                        action='store_true')

    args = parser.parse_args()

    global verbose
    verbose = args.verbose

    includes = ""
    plugins = ""

    for header in args.headers:
        includes += '#include "{}"\n'.format(header)
        with open(header, "r") as f:
            lines = f.readlines()

            header_parser = HeaderParser()
            header_parser.parse_plugin_header(lines)
            plugins += header_parser.get_pybind_entry()


    with open(args.template, 'r') as f:
        template = f.read()

    s = Template(template)
    filled_template = s.substitute(includes=includes,
                                   plugins=plugins)

    with open(args.output, 'w') as f:
        f.write(filled_template)


if __name__ == '__main__':
    main()
