import json
import socket
import time

DISCOVERY_MSG   = b"DISCOVER_ESP32"
DISCOVERY_PORT  = 5005         # puerto donde “escucha” el ESP32
REPLY_LISTEN    = 53000        # puerto local del PC donde esperamos las respuestas
SEND_PORT       = 5005         # al ESP32 también le enviaremos por este puerto
TIMEOUT_SEC     = 2.0          # ventana de escucha

def discover_esp32(broadcast_ip="255.255.255.255"):
    """
    Envía un broadcast DISCOVER_ESP32 y escucha respuestas JSON del tipo:
    {"type":"ESP32","hostname":"esp32-angel","ip":"192.168.1.80"}
    Devuelve una lista de dicts con 'ip' y 'hostname'.
    """
    results = []
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(("0.0.0.0", REPLY_LISTEN))
    sock.settimeout(TIMEOUT_SEC)

    # Enviar descubrimiento a la red local
    sock.sendto(DISCOVERY_MSG, (broadcast_ip, DISCOVERY_PORT))

    t0 = time.time()
    try:
        while time.time() - t0 < TIMEOUT_SEC:
            try:
                data, addr = sock.recvfrom(4096)
            except socket.timeout:
                break
            try:
                info = json.loads(data.decode("utf-8"))
            except Exception:
                continue
            if isinstance(info, dict) and info.get("type") == "ESP32":
                ip = info.get("ip", addr[0])
                host = info.get("hostname", "")
                results.append({"ip": ip, "hostname": host, "from": addr})
    finally:
        sock.close()
    return results

def send_udp_message(target_host_or_ip: str, message: str, port: int = SEND_PORT, force_ipv4=True):
    """
    Envía 'message' por UDP a 'target_host_or_ip':port.
    Si force_ipv4=True, evitamos resolver a IPv6 (útil en Windows).
    """
    family = socket.AF_INET if force_ipv4 else socket.AF_UNSPEC
    infos = socket.getaddrinfo(target_host_or_ip, port, family, socket.SOCK_DGRAM)
    fam, socktype, proto, canonname, sockaddr = infos[0]
    with socket.socket(fam, socket.SOCK_DGRAM) as s:
        s.sendto(message.encode("utf-8"), sockaddr)
    print(f"[OK] Enviado a {target_host_or_ip} ({sockaddr}) -> {message!r}")

if __name__ == "__main__":
    print("[*] Buscando ESP32 en la red...")
    devices = discover_esp32()
    if not devices:
        print("No se detectaron ESP32. Revisa que estén en la MISMA red y sin 'client isolation'.")
        print("Consejos: permite broadcast en tu Wi-Fi, permite Python en el firewall, prueba otra subred.")
        raise SystemExit(1)

    print("Encontrados:")
    for i, d in enumerate(devices, 1):
        print(f"  {i}. {d['hostname'] or '(sin nombre)'}  IP={d['ip']}  (visto desde {d['from']})")

    # Elegimos el primero; si quieres, cambia 'idx' o filtra por hostname
    idx = 1
    target_ip = devices[idx-1]["ip"]

    # Opción A: enviar POR IP
    #send_udp_message(target_ip, "Hola ESP32 por IP!")

    # Opción B: enviar POR NOMBRE (si usas mDNS o agregaste /etc/hosts en el PC)
    # Ejemplo con mDNS: "esp32-angel.local"
    send_udp_message("esp32_lider.local", "Hola por hostname!")
