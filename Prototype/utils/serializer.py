def serialize(dict : dict) -> int:
    str = ""

    str = " ".join(
                    [
                        dict['method'],
                        dict['uri'],
                        dict['http_version']
                    ]
                )
    str += "\r\n"
    for key in dict['headers'].keys():
        line = key + ":" + dict['headers'][key] + "\r\n"
        str += line
    str += "\r\n"
    return str
    