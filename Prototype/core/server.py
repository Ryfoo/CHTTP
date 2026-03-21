import socket as s
from HTTP.config.config import ServerConfig as sc
from HTTP.config.config import GlobalConfig as gc


config = sc(clients=5, connection=True, data_size=1000)

class Server:

    def __init__(self, clients=sc.clients, connection=sc.connection, debug=gc.debug):
        self.clients : int = clients
        self.data_size = 1000
        self.connection : bool = connection
        self.debug : bool = debug  
        self.data : str = "Hello, client!"
        #the server initialization process
        self.socket = s.socket(s.AF_INET, s.SOCK_STREAM)
        self.socket.setsockopt(s.SOL_SOCKET, s.SO_REUSEADDR, 1)
        self.socket.bind((gc.host, gc.port)) #informations are on HTTP.core.config


        #the server running process through socket.listen(n)
        self._run()


    def _run(self)->None:
        self.socket.listen(self.clients)
        while self.connection:
            clientsocket, addr = self.socket.accept()
            print("Connection has been established!")
            data = clientsocket.recv(self.data_size).decode()
            print(data)
            clientsocket.sendall(self.data.encode())



