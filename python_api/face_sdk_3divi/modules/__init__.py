from .error import Error


def get_repr(obj=None, _name=None, _dict=None, _level=""):
    max_nested_keys = 6
    max_description_len = 200
    brackets = iter(["(", ")"])

    if obj is not None:
        class_name, class_dict = obj.__class__.__name__, obj.__dict__
    elif None not in (_name, _dict):
        class_name, class_dict = _name, _dict
    else:
        raise Error(0x69a32a76, "Impossible to form __repr__ method")

    in_brackets = False if len(class_dict) > max_nested_keys else True

    for i in range(2):

        if in_brackets:
            delimiter = ",  "
            _level = ""
            nested_level = ""
            description = _level + class_name + next(brackets)
        else:
            delimiter = "\n"
            nested_level = _level + "  "
            description = _level + class_name + ":" + delimiter

        if not len(class_dict):
            description += nested_level + "empty"

        for j, (k, v) in enumerate(class_dict.items()):
            after = delimiter if j < len(class_dict) - 1 else ""

            if isinstance(v, dict):
                description += get_repr(obj=None, _name=k, _dict=v, _level=nested_level) + after
                continue
            elif isinstance(v, bytes):
                value = "class: {}  len: {}".format(type(v), len(v))
            else:
                value = v

            description += nested_level + "{}: {}".format(k, value) + after

        description += next(brackets) if in_brackets else ""

        if in_brackets and (len(description) > max_description_len or "\n" in description):
            in_brackets = False
        else:
            break

    return description
