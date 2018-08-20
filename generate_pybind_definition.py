#!/usr/bin/env python


import re
import argparse
from string import Template

verbose = False

def debug(string):
    if verbose:
        print(string)

class PluginParseException(Exception):
    pass

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
                    line.startswith("//") or \
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
                    "class " + self.class_name in line or \
                    self.class_name + "()" in line or \
                    "operator" in line:
                continue

            if "struct {" in line:
                inside_struct = True

            if "};" in line:
                inside_struct = False

            if line.endswith(';') and not inside_struct:
                new_definition = True
            else:
                new_definition = False

            part += line + " "

            # Ignore odd leftover '};'
            if part.strip() == '};':
                new_definition = False
                part = ""
                continue

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
                    debug("Found class: '{}' and parent: '{}'".
                          format(self.class_name, self.parent_name))
                    return

                p = re.compile('^class\s*([\w\d]*)\s*.*?$')
                m = p.match(line)
                if m:
                    self.class_name = m.groups()[0]
                    debug("Found class name: '{}'".
                          format(self.class_name))
                    return

        raise PluginParseException("Could not find class name")

    def find_method(self, part):
        # We ignore static methods for now.
        if 'static' in part:
            return True

        p = re.compile('^([\w][\w\d_:]*(?:<[\w\d,: ]*>)?)\s*([\w][\w\d_]*)\(([\w\d,<>:& ]*)\)\s*.*?$')
        m = p.match(part)
        if m:
            if len(m.groups()) != 3:
                raise ("Unknown number of groups {} for {}"
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
        if not "enum" in part:
            return False

        new_enum = Enum()

        p = re.compile('^enum class\s*([\w\d]*)\s{*.*?$')
        m = p.match(part)
        if m:
            new_enum.name = m.groups()[0]
            debug("Found enum name: '{}'".format(new_enum.name))
        else:
            raise PluginParseException("Could not parse enum name")

        p = re.compile('([\w\d]*),')
        m = p.findall(part)
        if m:
            for enum_entry in m:
                debug("Found enum entry: '{}'".format(enum_entry))
                new_enum.values.append(enum_entry)
            self.enums.append(new_enum)
            return True
        else:
            raise PluginParseException("Could not find enum entries")

    def find_struct(self, part):
        if not 'struct' in part:
            return False

        new_struct = Struct()

        p = re.compile('^struct\s*([\w\d]*)\s{*.*?$')
        m = p.match(part)
        if m:
            new_struct.name = m.groups()[0]
            debug("Found struct name: '{}'".format(new_struct.name))
        else:
            raise PluginParseException("Could not parse struct name")

        p = re.compile('[\w\d]*\s*([\w\d]+);')
        m = p.findall(part)
        if m:
            for struct_field in m:
                debug("Found struct variable: '{}'".format(struct_field))
                new_struct.fields.append(struct_field)
            self.structs.append(new_struct)
            return True
        else:
            raise PluginParseException("Could not parse struct fields")



    def parse_part(self, part):
        try:
            if self.find_method(part) or \
                    self.find_enum(part) or \
                    self.find_struct(part):
                return
        except PluginParseException as e:
            print("Ignoring: '{}', error: {}".format(part, e))


    def get_pybind_entry(self):
        if self.parent_name == "PluginBase":
            self.output += '    py::class_<{0}>(m, "{0}", plugin_base)\n'.\
                format(self.class_name)
            self.output += '        .def(py::init<System &>())\n'
        elif self.parent_name is not None:
            raise PluginParseException("Can't deal with parent other than PluginBase")
        elif self.class_name == 'MissionItem':
            # TODO: remove this gack for std::shared_ptr used
            #       to upload/download mission items.
            self.output += '    py::class_<{0}, std::shared_ptr<{0}>>(m, "{0}")\n'.\
                format(self.class_name)
            self.output += '        .def(py::init<>())\n'
        else:
            self.output += '    py::class_<{0}>(m, "{0}")\n'.\
                format(self.class_name)
            self.output += '        .def(py::init<>())\n'

        for i, method in enumerate(self.methods):
            self.output += '        .def("{0}", &{1}::{0})'.\
                format(method.method_name, self.class_name)
            if i + 1 == len(self.methods):
                self.output += ";\n\n"
            else:
                self.output += "\n"

        for enum_entry in self.enums:
            self.output += '    py::enum_<{0}::{1}>(m, "{0}_{1}")\n'.\
                format(self.class_name, enum_entry.name)
            for i, value in enumerate(enum_entry.values):
                self.output += '        .value("{0}", {1}::{2}::{0})'.\
                    format(value, self.class_name, enum_entry.name)
                if i + 1 == len(enum_entry.values):
                    self.output += "\n.export_values();\n\n"
                else:
                    self.output += "\n"

        for struct_entry in self.structs:
            self.output += '    py::class_<{0}::{1}>(m, "{0}_{1}")\n'.\
                format(self.class_name, struct_entry.name)
            for i, field in enumerate(struct_entry.fields):
                self.output += '        .def_readwrite("{0}", &{1}::{2}::{0})'.\
                    format(field, self.class_name, struct_entry.name)
                if i + 1 == len(struct_entry.fields):
                    self.output += ";\n\n"
                else:
                    self.output += "\n"

        return self.output

def remove_newlines_inside_brackets(content):
    return re.sub(r'\(.*?\)',
                  lambda m: m.group().replace("\n", " "),
                  content,
                  flags=re.DOTALL)

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
            content = f.read()
            content = remove_newlines_inside_brackets(content)
            lines = content.splitlines()

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
