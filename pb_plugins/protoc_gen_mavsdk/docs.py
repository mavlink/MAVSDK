# -*- coding: utf-8 -*-


class Docs():
    """
    Docs

    Based on https://github.com/protocolbuffers/protobuf/blob/a08b03d4c00a5793b88b494f672513f6ad46a681/src/google/protobuf/descriptor.proto
    """

    @staticmethod
    def collect_docs(source_code_info):

        docs = {}

        docs['methods'] = []
        docs['structs'] = []
        docs['enums'] = []

        for location in source_code_info.location:
            if len(location.path) > 0:
                path = location.path

                if path[0] == 1:  # name
                    pass
                elif path[0] == 2:  # package
                    pass
                elif path[0] == 4:  # message
                    struct_id = path[1]

                    if len(path) == 2:  # Top-level location of this message
                        docs['structs'].append(
                            {'description': location.leading_comments, 'params': []})
                    # A field of this message
                    elif len(path) == 4 and path[2] == 2:
                        param_id = path[3]
                        docs['structs'][struct_id]['params'].append(
                            location.trailing_comments)
                    elif path[2] == 3:  # A nested message
                        nested_struct_id = path[3]

                        if len(
                                path) == 4:  # Top-level location of this nested message
                            docs['structs'][struct_id]['structs'] = []
                            docs['structs'][struct_id]['structs'].append(
                                {'description': location.leading_comments, 'params': []})
                        # A field of this nested message
                        elif len(path) == 6 and path[4] == 2:
                            docs['structs'][struct_id]['structs'][nested_struct_id]['params'].append(
                                location.trailing_comments)
                    elif path[2] == 4:  # A nested enum
                        nested_enum_id = path[3]

                        if len(path) == 4:  # Top-level location of this nested enum
                            docs['structs'][struct_id]['enums'] = []
                            docs['structs'][struct_id]['enums'].append(
                                {'description': location.leading_comments, 'params': []})
                        # A value of this nested message
                        elif len(path) == 6 and path[4] == 2:
                            docs['structs'][struct_id]['enums'][nested_enum_id]['params'].append(
                                location.trailing_comments)

                elif path[0] == 5:  # enum
                    enum_id = path[1]

                    if len(path) == 2:  # Top-level location of this enum
                        docs['enums'].append(
                            {'description': location.leading_comments, 'params': []})
                    # A value of this message
                    elif len(path) == 4 and path[2] == 2:
                        param_id = path[3]
                        docs['enums'][enum_id]['params'].append(
                            location.trailing_comments)

                elif path[0] == 6:  # service
                    # The leading comment of the service is the description of
                    # the class
                    if len(path) == 2:
                        if location.leading_comments:
                            docs['class'] = location.leading_comments
                        else:
                            docs['class'] = ""

                    # The service contains the methods (those are the 'rpc'
                    # lines)
                    if len(path) == 4:
                        if location.leading_comments:
                            docs['methods'].append(location.leading_comments)
                        else:
                            docs['methods'].append("")

                elif path[0] == 8:  # option
                    pass
                elif path[0] == 12:  # syntax
                    pass

        return docs
