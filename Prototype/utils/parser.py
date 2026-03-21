from HTTP.config.config import header

def parse(method : str, uri : str, http_version : str, headers : list[header]=None) -> dict:
    dict = {}
    dict['method'] = method
    dict['uri'] = uri
    dict['http_version'] = http_version
    if headers is not None and headers != []:
        headers_dict = {}
        for header in headers:
            headers_dict[header[0]] = header[1]
        dict['headers'] = headers_dict

    return dict
