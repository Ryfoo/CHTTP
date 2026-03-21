from HTTP.config.config import HttpMessageConfig, header
from HTTP.utils.parser import parse
from HTTP.utils.serializer import serialize
class HttpRequest:
    def __init__(
            self, 
            method : str = HttpMessageConfig.method, 
            uri : str = HttpMessageConfig.uri, 
            http_version : str = HttpMessageConfig.http_version, 
            *args
        ):
        self.method : str = method
        self.uri : str = uri 
        self.http_version : str = http_version
        self.headers : list[header] = []
        for arg in args:
            self.headers.append(arg)
        self.dict = parse(
                            self.method,
                            self.uri, 
                            self.http_version, 
                            self.headers
                        )
        self.packet = serialize(self.dict)

    


    def __repr__(self):
        return f'''
                HttpRequest(
                    {self.method}, 
                    {self.uri}, 
                    {self.http_version}, 
                    {self.headers}
                )
                '''
    def __str__(self):
        return f'''
                    {self.method} 
                    {self.uri} 
                    {self.http_version}
                    {self.headers}
                '''


http = HttpRequest(
    'GET',
    '/index.html',
    'HTTP/1.0',
    ("Content-Type", "application/json"),
    ("Authorization", "Bearer abc123"),
)

print(http.packet)