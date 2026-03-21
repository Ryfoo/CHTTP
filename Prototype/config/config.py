import socket as s
from dataclasses import dataclass

#complex user-defined datatypes for type annotations
header = tuple[str, str]


#global configuration options for all files
@dataclass
class GlobalConfig:
    host : str = "127.0.0.1"
    port : int = 8000
    debug : bool = False


# configuration options for the abstract HTTP-message class
# can be found on root/models
@dataclass
class HttpMessageConfig:
    method : str = 'GET'
    uri : str = '/index.html'
    http_version : str = 'HTTP/1.0'






# configuration for the server class
# can be found on root/core
@dataclass
class ServerConfig:

    connection : bool = True
    clients : int = 5
    data_size : int = 8

# configuartion for the client class
# can be found on root/core
@dataclass 
class ClientConfig:
    pass