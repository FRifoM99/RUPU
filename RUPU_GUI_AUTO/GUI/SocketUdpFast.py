import socket

robot_ip = "192.168.100.24"   # IP del ESP32
robot_port = 1111            # puerto en el ESP (tu udp.begin(1111))

msg = "E/label/L".encode("utf-8")         # payload en bytes (ACK con la etiqueta L)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP/IPv4
s.sendto(msg, (robot_ip, robot_port))                 # envía y listo
s.close()
