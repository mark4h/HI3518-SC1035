"""Generate HI3518 SDK sensor_init function from Snowman decompiler output"""
import re
import sys


def main(c_code_filename):

    variables = {}

    with open(c_code_filename, "r") as f:

        for l in f:

            m = re.match(
                r"""
                    ^int32_t\ (g[a-f0-9]{6,6})\ =\ (0x[0-9a-f]{1,6});\s*$
                """,
                l,
                re.VERBOSE
            )
            if m:

                # groups
                variable_name = m.group(1)
                variable_value = m.group(2)

                variables[variable_name] = variable_value

            m2 = re.match(
                r"""
                    ^\ {4,8}(r[0-9]{1,1}_[0-9]{1,2})\ =\ (g[a-f0-9]{6,6});\s*$
                """,
                l,
                re.VERBOSE
            )
            if m2:

                # groups
                variable_name = m2.group(1)
                variable_value = variables[m2.group(2)]

                variables[variable_name] = variable_value

    with open(c_code_filename, "r") as f:

        line_start_value = ""

        for l in f:

            if "if" in l:
                print("if {")
                line_start_value = "    "
            elif "else" in l:
                print("} else {")
                line_start_value = "    "

            m3 = re.match("^ {4,8}fun_4c364c\((.*), (.*)\);", l)

            if m3:

                address = m3.group(1)
                value = m3.group(2)

                try:
                    if "x" in value:
                        value = int(value, 16)
                    else:
                        value = int(value)
                except ValueError:

                    print(
                        "{:s}sensor_write_register({:}, {:});".format(
                            line_start_value,
                            address,
                            value
                        )
                    )
                    continue

                if address[:2] == "0x":

                    print(
                        "{:s}sensor_write_register({:}, 0x{:x});".format(
                            line_start_value,
                            address,
                            value
                        )
                    )

                else:

                    print(
                        "{:s}sensor_write_register({:}, 0x{:x});".format(
                            line_start_value,
                            variables[address],
                            value
                        )
                    )

        print("}")


if __name__ == "__main__":

    main(*sys.argv[1:])
