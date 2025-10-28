#RupuControlerV6 GUI version Modo Automatico

## LIBRERIAS -------------------------
import tkinter as tk #PIP
import customtkinter #I

from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg #pip
import time as tm

import socket

import matplotlib.pyplot as plt #descargar
import matplotlib.animation as animation
import threading 

import sys
import paho.mqtt.client as mqtt #descargar #install
import pymysql #descargar
import csv

UDP_IP_TX =""
UDP_PORT_TX = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

##---------------------------------------------------------
hostname=socket.gethostname()
IPAddr=socket.gethostbyname(hostname)

UDP_IP_RX = IPAddr # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT_RX = 1234

#NUevo
HELLO_PORT = 7777

#UDP
sock_RX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_RX.bind((UDP_IP_RX, UDP_PORT_RX))

#---------------------------------------------------------
file_name = "monitortest.csv"  # archivo csv
texto = open(file_name,'w')
#estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
 
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')
texto.close()

min_v = 0
max_v = 30

min_d = 5
max_d = 25

gData1 = [[0], [0]]
gData2 = [[0], [0]]
gData3 = [[0], [0]]

flag_save=True

class App(customtkinter.CTk):
    
    def __init__(self):
        super().__init__()
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.layout()
        """
        En esta sección se crean la pestaña 
        Configuración: Entrada de IP
        Control: Muestra el panel de control
        """
    
    def layout(self):
        self.title("RÜPÜ Controller")
        self.geometry("700x800+0+0") 
        
        self.letras_sugeridas =["L","S","T","O"]
        self.auto_label_seq = []   # secuencia de etiquetas a usar en Automático

        #Nuevo
        # Socket para HELLO (no bloqueante)
        self.hello_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.hello_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.hello_sock.bind(("", HELLO_PORT))
        self.hello_sock.settimeout(0.5)

        #Estado descubrimiento
        self.discovery_running = False
        self.discovery_thread = None
        self.seen_ips = set()   # para no repetir IPs

        self.auto_expected = None   # N confirmado (int) o None si no se fijó
        self.auto_detected = 0      # filas IP/Etiqueta agregadas en Automático
        self.auto_n_locked = False   # False = libre, True = bloqueado tras OK

        #Variable para configuracion IP Monitor        
        self.ip_monitor_var = tk.StringVar(value=IPAddr)
        self.arrival_order = []  # IPs en orden de llegada (R1, R2, R3, ...)

        #Tamaño Grilla
        self.grid_columnconfigure(1,weight=1)
        self.grid_rowconfigure(1,weight=1)
        
        #-----Pestañas-------#
        
        #Definir pestañas principales
        self.tabview=customtkinter.CTkTabview(self)#puede ser un self.frame
        self.tabview.pack(padx=20,pady=20,expand=True,fill='both')
        self.tabview.add("Configuración IPs")
        self.tabview.add("Control")
        
        #----Contenido Tab Configuración---#
        self.tabviewConfig = customtkinter.CTkFrame(self.tabview.tab("Configuración IPs"),fg_color='transparent')
        self.tabviewConfig.pack(padx=0,pady=0,expand=True,fill='both')
       
        #label vacía
        #self.empty1 = customtkinter.CTkLabel(self.tabviewConfig, text= "         ", fg_color="transparent")
        #self.empty1.grid(row=1, column=0, padx=5, pady=5)
       
        # Header con selector de Modo (centrado y compacto)
        self.tabviewConfig.grid_columnconfigure(0, weight=0)
        self.header = customtkinter.CTkFrame(self.tabviewConfig, fg_color="transparent")
        self.header.grid(row=0, column=0, pady=(6, 0), sticky="n")  # centrado arriba

        # fila interna centrada
        self.header.grid_columnconfigure(0, weight=1)
        row = customtkinter.CTkFrame(self.header, fg_color="transparent")
        row.grid(row=0, column=0)

        customtkinter.CTkLabel(row, text="Modo:").grid(row=0, column=0, padx=(0, 8))
        self.mode_seg = customtkinter.CTkSegmentedButton(
            row, values=["Automático", "Manual"], command=self._on_change_mode
        )
        self.mode_seg.grid(row=0, column=1)
        self.mode_seg.set("Automático")


        # === Centrar contenedor y crear TARJETA ===
        self.tabviewConfig.grid_rowconfigure(0, weight=0)   # el header NO crece
        self.tabviewConfig.grid_rowconfigure(1, weight=1)   # el espacio libre va bajo la tarjeta
        self.tabviewConfig.grid_columnconfigure(0, weight=1)


        # ----- TARJETA: MODO AUTOMÁTICO -----
        self.auto_box = customtkinter.CTkFrame(
            self.tabviewConfig,
            corner_radius=12,
            fg_color=("white", "#1f1f1f")
        )

        self.auto_box.grid(row=1, column=0, padx=30, pady=(8, 16), sticky="n")
        self.auto_box.grid_columnconfigure(1, weight=1)
        self.auto_box.grid_columnconfigure(2, weight=1)
        self.auto_box.grid_columnconfigure(3, weight=1)

        # ----- TARJETA: MODO MANUAL -----
        self.manual_box = customtkinter.CTkFrame(
            self.tabviewConfig,
            corner_radius=12,
            fg_color=("white", "#1f1f1f")
        )
        self.manual_box.grid(row=1, column=0, padx=30, pady=(8, 16), sticky="n")
        self.manual_box.grid_columnconfigure(4, weight=0)
        self.manual_box.grid_columnconfigure(3, weight=1)

        title2 = customtkinter.CTkLabel(self.manual_box, text="Configuración Manual", font=("", 16, "bold"))
        title2.grid(row=0, column=0, columnspan=4, pady=(8, 6))

        # Fila: IP Monitor + Nº robots + Ok
        customtkinter.CTkLabel(self.manual_box, text="IP Monitor:").grid(row=1, column=0, padx=8, pady=6, sticky="e")
        customtkinter.CTkEntry(self.manual_box, width=220, textvariable=self.ip_monitor_var).grid(row=1, column=1, padx=8, pady=6, sticky="w")

        self.num_label = customtkinter.CTkLabel(self.manual_box, text="N° Robots")
        self.num_label.grid(row=2, column=0, padx=8, pady=(3, 6), sticky="e")

        self.entry_num = customtkinter.CTkComboBox(self.manual_box, values=["1","2","3","4","5","6"])
        self.entry_num.set("1")
        self.entry_num.grid(row=2, column=1, padx=8, pady=(3, 6), sticky="w")

        self.submit_button = customtkinter.CTkButton(self.manual_box, text="Ok", command=self.create_ip_entries)
        self.submit_button.grid(row=2, column=2, padx=8, pady=(3, 6), sticky="w")

        # Tabla scrollable para filas IP/Etiqueta (MANUAL)
        self.manual_table = customtkinter.CTkScrollableFrame(self.manual_box, width=480, height=310)
        self.manual_table.grid(row=3, column=0, columnspan=4, padx=10, pady=(6, 6), sticky="n")
        self._man_hdr_ip  = customtkinter.CTkLabel(self.manual_table, text="IP", width=220)
        self._man_hdr_tag = customtkinter.CTkLabel(self.manual_table, text="Etiqueta", width=120)
        self._man_hdr_ip.grid(row=0, column=0, padx=6, pady=(2, 6), sticky="w")
        self._man_hdr_tag.grid(row=0, column=1, padx=6, pady=(2, 6), sticky="w")

        # Botones Calibrar / Guardar dentro de la tarjeta Manual
        # Pie de tarjeta (MANUAL): centrado como en Automático
        self.manual_footer = customtkinter.CTkFrame(self.manual_box, fg_color="transparent")
        # la tabla está en row=3; el footer va justo debajo
        self.manual_footer.grid(row=4, column=0, columnspan=4, pady=(0, 8))

        # [espacio][Calibrar][espacio][Guardar][espacio]
        for c in (0, 2, 4):
            self.manual_footer.grid_columnconfigure(c, weight=1)
        for c in (1, 3):
            self.manual_footer.grid_columnconfigure(c, weight=0)

        self.calibrar_button = customtkinter.CTkButton(
            self.manual_footer, width=120, text="Calibrar", command=self.clickCalibrarButton
        )
        self.calibrar_button.grid(row=0, column=1, padx=12, pady=5)

        self.guardar_button  = customtkinter.CTkButton(
            self.manual_footer, width=120, text="Guardar IP", command=self.clickGuardarButton
        )
        self.guardar_button.grid(row=0, column=3, padx=12, pady=5)



        # Título
        title = customtkinter.CTkLabel(self.auto_box, text="Configuración de IPs", font=("", 16, "bold"))
        title.grid(row=0, column=0, columnspan=4, pady=(8, 6))

        # Fila: IP Monitor + Modo
        self.monitor_ip_label = customtkinter.CTkLabel(self.auto_box, text="IP Monitor:")
        self.monitor_ip_label.grid(row=1, column=0, padx=8, pady=6, sticky="e")

        self.monitor_ip_entry = customtkinter.CTkEntry(self.auto_box, width=220, textvariable=self.ip_monitor_var)
        self.monitor_ip_entry.grid(row=1, column=1, padx=8, pady=6, sticky="w")
        #self.monitor_ip_entry.insert(customtkinter.END, IPAddr)
        
        # --- N° Robots + OK (Automático) ---
        customtkinter.CTkLabel(self.auto_box, text="N° Robots:").grid(row=2, column=0, padx=8, pady=4, sticky="e")

        self.auto_n_combo = customtkinter.CTkComboBox(self.auto_box, values=[str(i) for i in range(1, 33)])
        self.auto_n_combo.set("1")
        self.auto_n_combo.grid(row=2, column=1, padx=8, pady=4, sticky="w")

        self.auto_ok_btn = customtkinter.CTkButton(self.auto_box, text="OK", width=70, command=self._toggle_auto_n)
        self.auto_ok_btn.grid(row=2, column=2, padx=(0, 10), pady=4, sticky="w")

        # Fila de botones (Automático)
        btns = customtkinter.CTkFrame(self.auto_box, fg_color="transparent")
        btns.grid(row=3, column=0, columnspan=4, pady=(6, 4))
        self.scan_button   = customtkinter.CTkButton(btns, text="Buscar robots", command=self._toggle_discovery, width=140)
        self.add_row_btn   = customtkinter.CTkButton(btns, text="+ Agregar fila", command=self._add_row_auto, width=140)
        self.clear_rows_btn= customtkinter.CTkButton(btns, text="Limpiar lista", command=self._clear_auto_table, width=140, fg_color="#6b7280")
        self.scan_button.grid(row=0, column=0, padx=5)
        self.add_row_btn.grid(row=0, column=1, padx=5)
        self.clear_rows_btn.grid(row=0, column = 2, padx=5)

        # Contador a la derecha
        self.auto_counter_lbl = customtkinter.CTkLabel(
            self.auto_box,
            text="Esperados: —   |   Detectados: 0"
        )
        self.auto_counter_lbl.grid(row=2, column=3, padx=(0, 6), pady=4, sticky="w")

        # Tabla scrollable (Automático)
        self.table_frame = customtkinter.CTkScrollableFrame(self.auto_box, width=480, height=310)
        self.table_frame.grid(row=4, column=0, columnspan=4, padx=10, pady=(4, 12), sticky="n")
        self._disc_hdr_ip  = customtkinter.CTkLabel(self.table_frame, text="IP", width=220)
        self._disc_hdr_tag = customtkinter.CTkLabel(self.table_frame, text="Etiqueta", width=120)
        self._disc_hdr_ip.grid(row=0, column=0, padx=6, pady=(2, 6), sticky="w")
        self._disc_hdr_tag.grid(row=0, column=1, padx=6, pady=(2, 6), sticky="w")

        self.auto_footer = customtkinter.CTkFrame(self.auto_box, fg_color="transparent")
        self.auto_footer.grid(row=5, column=0, columnspan=4, pady=(0, 8))

        # [espacio][Calibrar][espacio][Guardar][espacio]
        for c in (0, 2, 4):
            self.auto_footer.grid_columnconfigure(c, weight=1)
        for c in (1, 3):
            self.auto_footer.grid_columnconfigure(c, weight=0)

        self.calib_auto_btn = customtkinter.CTkButton(
            self.auto_footer, text="Calibrar", command=self.clickCalibrarButton, width=120
        )
        self.calib_auto_btn.grid(row=0, column=1, padx=12, pady=2)

        self.save_rows_btn = customtkinter.CTkButton(
            self.auto_footer, text="Guardar IP", command=self.clickGuardarButton, width=120
        )
        self.save_rows_btn.grid(row=0, column=3, padx=12, pady=2)
        
        #Lista que guarda IP y Label ingresada
        self.ip_entry_widgets = []
        
        #-----------------Contenido Tab Control----------#
        self.tabviewControl = customtkinter.CTkFrame(self.tabview.tab("Control"),fg_color='transparent')
        self.tabviewControl.pack(padx=0,pady=0,expand=True,fill='both')             
        
        self.tabviewControlFrame =customtkinter.CTkFrame(self.tabviewControl,fg_color='transparent')
        self.tabviewControlFrame.grid(row=0,column=0)
        
        self.empty2 = customtkinter.CTkLabel(self.tabviewControlFrame, text= "         ", fg_color="transparent")
        self.empty2.grid(row=0, column=0, padx=5, pady=5)
        
        self.switch_var = customtkinter.StringVar(value="off")
        self.switch = customtkinter.CTkSwitch(self.tabviewControlFrame, text="Estado Robot", command=self.switch_event,variable=self.switch_var, onvalue="on", offvalue="off")
        self.switch.grid(row=0, column=2, padx=10, pady=10)

        self.switchSave_var = customtkinter.StringVar(value="1")
        self.savetext ="Guardar "+ file_name
        self.switchSave = customtkinter.CTkSwitch(self.tabviewControlFrame, text=self.savetext, command=self.switchSave_event,variable=self.switchSave_var, onvalue="1", offvalue="0")
        self.switchSave.grid(row=0, column=3, padx=10, pady=10)
             

        #crear lista de letras desplegable
                    
        self.labelRobot = customtkinter.CTkLabel(self.tabviewControlFrame, text="Etiqueta robot:")
        self.labelRobot.grid(row=2, column=2, padx=10, pady=10) 
        
        self.selected_letter = customtkinter.StringVar()

        self.letras_lista=[]
    
        self.letter_combobox = customtkinter.CTkComboBox(self.tabviewControlFrame, values=self.letras_lista,variable=self.selected_letter)
        self.letter_combobox.set("L")
        self.letter_combobox.grid(row=2, column=3, padx=10, pady=10)

        
        # Posicionar los sliders y labels
        labelV = customtkinter.CTkLabel(self.tabviewControlFrame, text="Velocidad")
        sliderV = customtkinter.CTkSlider(self.tabviewControlFrame, from_=min_v, to=max_v, orientation="horizontal",state="normal")
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get(),self.getIP(self.letter_combobox.get())))
        self.labelVvalue = customtkinter.CTkLabel(self.tabviewControlFrame, text=str(round(sliderV.get())), fg_color="transparent")
        

        labelV.grid(row=3, column=2, columnspan=1, padx=10, pady=10)
        sliderV.grid(row=3, column=3, padx=10, pady=10)
        self.labelVvalue.grid(row=3, column=5, columnspan=1, padx=10, pady=10)
        

        labelD = customtkinter.CTkLabel(self.tabviewControlFrame, text="Distancia")
        sliderD = customtkinter.CTkSlider(self.tabviewControlFrame, from_=min_d, to=max_d, orientation="horizontal",state="normal")
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get(),self.getIP(self.letter_combobox.get())))
        self.labelDvalue = customtkinter.CTkLabel(self.tabviewControlFrame, text=str(round(sliderD.get())), fg_color="transparent")
        
        labelD.grid(row=4, column=2, columnspan=1, padx=10, pady=10)
        sliderD.grid(row=4, column=3, padx=10, pady=10)
        self.labelDvalue.grid(row=4, column=5, columnspan=1, padx=10, pady=10)
        
       
        #######SUB PESTAÑAS CONTROLADOR #####
        self.subTabView=customtkinter.CTkTabview(self.tabviewControl)
        self.subTabView.grid(column=0,padx=10, pady=10,sticky='ew')
        self.subTabView.add("Theta")
        self.subTabView.add("Velocidad")
        self.subTabView.add("Distancia")
        
        self.subTabVel =customtkinter.CTkFrame(self.subTabView.tab("Velocidad"),fg_color='white')
        self.subTabVel.pack(padx=0,pady=0,expand=True,fill='both') 
    
        self.subTabDist =customtkinter.CTkFrame(self.subTabView.tab("Distancia"),fg_color='white')
        self.subTabDist.pack(padx=0,pady=0,expand=True,fill='both') 
    
        self.subTabTheta =customtkinter.CTkFrame(self.subTabView.tab("Theta"),fg_color='white')
        self.subTabTheta.pack(padx=0,pady=0,expand=True,fill='both') 
    
        #----SUBTAB THETA---#
        self.label_P_th = customtkinter.CTkLabel(self.subTabTheta, text="P", fg_color="transparent")
        self.label_P_th.grid(row=0, column=0, padx=10, pady=10)    

        self.entry_P_th =  customtkinter.CTkEntry(self.subTabTheta)
        self.entry_P_th.grid(row=1, column=0, padx=5, pady=5)
        
        self.label_I_th = customtkinter.CTkLabel(self.subTabTheta, text="I", fg_color="transparent")
        self.label_I_th.grid(row=0, column=1, padx=10, pady=10)    

        self.entry_I_th =  customtkinter.CTkEntry(self.subTabTheta)
        self.entry_I_th.grid(row=1, column=1, padx=5, pady=5)
    
        self.label_D_th = customtkinter.CTkLabel(self.subTabTheta, text="D", fg_color="transparent")
        self.label_D_th.grid(row=0, column=2, padx=10, pady=10)    

        self.entry_D_th =  customtkinter.CTkEntry(self.subTabTheta)
        self.entry_D_th.grid(row=1, column=2, padx=5, pady=5)
    
        self.submit_theta_btn =customtkinter.CTkButton(self.subTabTheta, text="Enviar", command=self.submit_theta)
        self.submit_theta_btn.grid(row=2, column=1, padx=5, pady=5)
        
        #----SUBTAB Vel
        self.label_P_vel = customtkinter.CTkLabel(self.subTabVel, text="P", fg_color="transparent")
        self.label_P_vel.grid(row=0, column=0, padx=10, pady=10)    

        self.entry_P_vel =  customtkinter.CTkEntry(self.subTabVel)
        self.entry_P_vel.grid(row=1, column=0, padx=5, pady=5)
        
        self.label_I_vel = customtkinter.CTkLabel(self.subTabVel, text="I", fg_color="transparent")
        self.label_I_vel.grid(row=0, column=1, padx=10, pady=10)    

        self.entry_I_vel =  customtkinter.CTkEntry(self.subTabVel)
        self.entry_I_vel.grid(row=1, column=1, padx=5, pady=5)
    
        self.label_D_vel = customtkinter.CTkLabel(self.subTabVel, text="D", fg_color="transparent")
        self.label_D_vel.grid(row=0, column=2, padx=10, pady=10)    

        self.entry_D_vel =  customtkinter.CTkEntry(self.subTabVel)
        self.entry_D_vel.grid(row=1, column=2, padx=5, pady=5)
    
        self.submit_vel_btn =customtkinter.CTkButton(self.subTabVel, text="Enviar", command=self.submit_vel)
        self.submit_vel_btn.grid(row=2, column=1, padx=5, pady=5)
        
        #----SUBTAB Dist
        self.label_P_Dist = customtkinter.CTkLabel(self.subTabDist, text="P", fg_color="transparent")
        self.label_P_Dist.grid(row=0, column=0, padx=10, pady=10)    

        self.entry_P_Dist =  customtkinter.CTkEntry(self.subTabDist)
        self.entry_P_Dist.grid(row=1, column=0, padx=5, pady=5)
        
        self.label_I_Dist = customtkinter.CTkLabel(self.subTabDist, text="I", fg_color="transparent")
        self.label_I_Dist.grid(row=0, column=1, padx=10, pady=10)    

        self.entry_I_Dist =  customtkinter.CTkEntry(self.subTabDist)
        self.entry_I_Dist.grid(row=1, column=1, padx=5, pady=5)
    
        self.label_D_Dist = customtkinter.CTkLabel(self.subTabDist, text="D", fg_color="transparent")
        self.label_D_Dist.grid(row=0, column=2, padx=10, pady=10)    

        self.entry_D_Dist =  customtkinter.CTkEntry(self.subTabDist)
        self.entry_D_Dist.grid(row=1, column=2, padx=5, pady=5)
    
        self.submit_dist_btn =customtkinter.CTkButton(self.subTabDist, text="Enviar", command=self.submit_dist)
        self.submit_dist_btn.grid(row=2, column=1, padx=5, pady=5)


        self.monitor_button = customtkinter.CTkButton(self.tabviewControlFrame, text="Monitorear Señales", command=self.start_monitoring_vel)
        self.monitor_button.grid(row=8, column=2,columnspan=2, padx=10, pady=10)

        # Modo por defecto: Automático (oculta controles manuales)
        self._on_change_mode("Automático")


    
    #Crea y guarda las entradas de IP y Label
    def submit_theta(self):
        for i in range(len(self.ip_entry_widgets)):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            MESSAGE = "E/co_p/" + str(self.entry_P_th.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            MESSAGE = "E/co_i/" + str(self.entry_I_th.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            MESSAGE = "E/co_d/" + str(self.entry_D_th.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            
    
    def submit_dist(self):
        for i in range(len(self.ip_entry_widgets)):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            sock.sendto(bytes("E/cd_p/"+str(self.entry_P_Dist.get()), "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            sock.sendto(bytes("E/cd_i/"+str(self.entry_I_Dist.get()), "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            sock.sendto(bytes("E/cd_d/"+str(self.entry_D_Dist.get()), "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

    def submit_vel(self):
        for i in range(len(self.ip_entry_widgets)):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            MESSAGE = "E/cv_p/" + str(self.entry_P_vel.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            MESSAGE = "E/cv_i/" + str(self.entry_I_vel.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            MESSAGE = "E/cv_d/" + str(self.entry_D_vel.get())
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print(MESSAGE, UDP_IP_TX)      
        
    
    def create_ip_entries(self):
        # limpia tabla manual y lista
        for child in self.manual_table.winfo_children():
            if child not in [self._man_hdr_ip, self._man_hdr_tag]:
                child.destroy()
        self.ip_entry_widgets = []

        num_entries = int(self.entry_num.get())

        # extiende sugerencias L/S/T/O si falta
        while len(self.letras_sugeridas) < num_entries:
            self.letras_sugeridas.append("")

        # crea filas IP/Etiqueta dentro de la tabla manual
        for i in range(num_entries):
            ip_entry = customtkinter.CTkEntry(self.manual_table, width=220)
            ip_entry.grid(row=i+1, column=0, padx=6, pady=2, sticky="w")
            ip_entry.insert(customtkinter.END, "192.168.1.10")

            tag_entry = customtkinter.CTkEntry(self.manual_table, width=120)
            tag_entry.grid(row=i+1, column=1, padx=6, pady=2, sticky="w")
            tag_entry.insert(customtkinter.END, self.letras_sugeridas[i])

            self.ip_entry_widgets.append((ip_entry, tag_entry))

            
    #Posiciona botones Calibrar y Controlar
    def create_widgets(self):
        pass

    #NUEVO, funcion estado de descubrimiento 
    def _toggle_discovery(self):
        """Alterna entre Iniciar y Detener la búsqueda de robots (HELLO UDP)."""
        if not self.discovery_running:
            # iniciar
            self.discovery_running = True
            self.scan_button.configure(text="Detener")
            self.seen_ips.clear()  # opcional: limpiar vistos para un nuevo scan
            self.discovery_thread = threading.Thread(target=self._discovery_loop, daemon=True)
            self.discovery_thread.start()
        else:
            # detener
            self.discovery_running = False
            self.scan_button.configure(text="Buscar robots")

    def _discovery_loop(self):
        """
        Escucha paquetes HELLO por UDP y agrega IPs a la tabla.
        """
        while self.discovery_running:
            try:
                data, addr = self.hello_sock.recvfrom(2048)
            except socket.timeout:
                continue
            except OSError:
                break  # socket cerrado al salir

            try:
                msg = data.decode("utf-8", errors="ignore").strip()
            except Exception:
                msg = ""

            # filtro simple de HELLO
            is_hello = msg.startswith("HELLO") or msg.startswith("H") or ("hello" in msg.lower())

            if not is_hello:
                continue

            ip = addr[0]

            # evitar duplicados
            if ip in self.seen_ips:
                continue

            # si hay límite por N y ya llenamos, no agregamos más
            if self.auto_n_locked and self.auto_expected is not None:
                if len(self.ip_entry_widgets) >= self.auto_expected:
                    # ya está completo; podemos detener auto si quieres
                    # self._toggle_discovery()
                    continue

            self.seen_ips.add(ip)

            # Agregar fila en el hilo de UI
            self.after(0, lambda ip=ip: self._add_row_auto(ip_val=ip, tag_val=""))

    def _on_change_mode(self, mode):
        if mode == "Automático":
            try: self.manual_box.grid_remove()
            except: pass
            self.auto_box.grid()
            self.ip_entry_widgets = []
            self._clear_auto_table()
            self._reset_auto_n()
            self.scan_button.configure(state="disabled")
            self.add_row_btn.configure(state="disabled")
        else:
            # si cambias de modo, corta cualquier búsqueda en curso
            if self.discovery_running:
                self._toggle_discovery()
            try: self.auto_box.grid_remove()
            except: pass
            self.manual_box.grid()


    def _scan_stub(self):
        """Por ahora no descubre; deja la tabla automática vacía y lista."""
        self.ip_entry_widgets = []   # la lista activa será la del modo actual (automático)
        self._clear_auto_table()


    def _clear_auto_table(self):
        """Limpia SOLO la tabla automática (no toca entradas del Manual)."""
        for child in self.table_frame.winfo_children():
            if child not in [self._disc_hdr_ip, self._disc_hdr_tag]:
                try:
                    child.destroy()
                except:
                    pass
        self.ip_entry_widgets = []            
        self.auto_detected = 0
        self.arrival_order = []  
        self._update_auto_counter()

    def _toggle_auto_n(self):
        """Toggle del N: OK = bloquear; ¬OK = desbloquear para corregir."""
        if not self.auto_n_locked:
            # BLOQUEAR
            try:
                self.auto_expected = int(self.auto_n_combo.get())
                self.auto_label_seq = self._gen_auto_labels(self.auto_expected)
            except Exception:
                return  # no bloquees si no es número
            self.auto_n_locked = True
            # UI
            self.auto_n_combo.configure(state="disabled")
            self.auto_ok_btn.configure(text="¬OK")   # ahora permite cancelar
            # limpia la tabla para empezar fresco (opcional, puedes quitar esta línea)
            self._clear_auto_table()
            # habilita acciones
            self.scan_button.configure(state="normal")
            self.add_row_btn.configure(state="normal")
        else:
            # DESBLOQUEAR (para corregir N)
            # 1) detener búsqueda si está activa
            self.auto_label_seq = []
            if self.discovery_running:
                self._toggle_discovery()
            self.auto_n_locked = False
            self.auto_expected = None
            # UI
            self.auto_n_combo.configure(state="normal")
            self.auto_ok_btn.configure(text="OK")
            # deshabilita acciones hasta que vuelvas a bloquear
            self.scan_button.configure(state="disabled")
            self.add_row_btn.configure(state="disabled")
            # no borro la tabla al desbloquear por si quieres conservar lo escrito
            # actualiza contador
            self._update_auto_counter()

    def _reset_auto_n(self):
        """Restablece el selector N y el botón toggle al estado inicial."""
        self.auto_expected = None
        self.auto_detected = 0
        self.auto_n_locked = False
        self.auto_n_combo.configure(state="normal")
        self.auto_ok_btn.configure(state="OK" if hasattr(self, "auto_ok_btn") else "OK")
        self.auto_ok_btn.configure(text="OK")
        self.auto_label_seq = []
        self.arrival_order = []
        self._update_auto_counter()

    def _anchor_ip(self) -> str: #Añadir IP ancla para robot ultimo del peloton
        """Devuelve A.B.C.50 tomando A.B.C desde la IP del monitor."""
        try:
            ip = self.ip_monitor_var.get().strip()
            a,b,c,_ = ip.split(".", 3)
            return f"{a}.{b}.{c}.50"
        except Exception:
            return "172.20.10.50"  # error

    def _send_set_sucesor(self, target_ip: str, sucesor_ip: str): #envia IP del sucesor
        """Envía al robot target_ip: R/sucesor/<sucesor_ip> por UDP/1111."""
        try:
            MESSAGE = f"R/sucesor/{sucesor_ip}"
            sock.sendto(MESSAGE.encode("utf-8"), (target_ip, 1111))
            print(f"[sucesor] -> {target_ip}: {MESSAGE}")
        except Exception as e:
            print("[sucesor][ERROR]", e)


    def _send_label_ack(self, ip: str, label: str): #envia ack para que robot salga de estado reconocimiento
        try:
            MESSAGE = f"R/label/{label}"
            sock.sendto(MESSAGE.encode("utf-8"), (ip, 1111))  # 1111 = puerto_local del robot
            print(f"[ACK] -> {ip}: {MESSAGE}")
        except Exception as e:
            print("[ACK][ERROR]", e)

    def _gen_auto_labels(self, n: int):
        """
        Genera la lista de etiquetas a usar en Automático, de atrás hacia adelante.
        Si n=3 y self.letras_sugeridas=["L","S","T","O"], devuelve ["T","S","L"].
        Si n>len(letras_sugeridas), se rellenan extras como R1, R2, ...
        """
        base = self.letras_sugeridas[:n]
        if len(base) < n:
            extras = [f"R{i+1}" for i in range(n - len(base))]
            base = base + extras
        return list(reversed(base))


    def _update_auto_counter(self):
        exp_txt = "—" if self.auto_expected is None else str(self.auto_expected)
        self.auto_counter_lbl.configure(text=f"Esperados: {exp_txt}   |   Detectados: {self.auto_detected}")

    def _clear_ip_rows(self):
        """Limpia la tabla y la lista interna de entradas."""
        for child in self.table_frame.winfo_children():
            if child not in [self._disc_hdr_ip, self._disc_hdr_tag]:
                try: child.destroy()
                except: pass
        for (ip_w, tag_w) in self.ip_entry_widgets:
            try: ip_w.destroy(); tag_w.destroy()
            except: pass
        self.ip_entry_widgets = []

    def _add_row_auto(self, ip_val="", tag_val=""):
        """Añade fila (IP / Etiqueta) en la tabla y en self.ip_entry_widgets."""
        if self.auto_n_locked and self.auto_expected is not None and len(self.ip_entry_widgets) >= self.auto_expected:
            return

        row_idx = 1 + len(self.ip_entry_widgets)  # +1 por la cabecera
        ip_w = customtkinter.CTkEntry(self.table_frame, width=220)
        ip_w.grid(row=row_idx, column=0, padx=6, pady=2, sticky="w")
        ip_w.insert(customtkinter.END, ip_val)

        tag_w = customtkinter.CTkEntry(self.table_frame, width=120)
        tag_w.grid(row=row_idx, column=1, padx=6, pady=2, sticky="w")
        if tag_val:
            por_defecto = tag_val
        elif self.auto_n_locked and self.auto_expected is not None:
            # asigna desde atrás hacia adelante: p. ej., N=3 => ["T","S","L"]
            idx = len(self.ip_entry_widgets)  # 0 para la 1ra IP que llega, 1 para la 2da, etc.
            if idx < len(self.auto_label_seq):
                por_defecto = self.auto_label_seq[idx]
            else:
                por_defecto = f"R{idx+1}"  # fallback si N > sugeridas
        else:
            suger = self.letras_sugeridas
            por_defecto = suger[len(self.ip_entry_widgets) % len(suger)]
        tag_w.insert(customtkinter.END, por_defecto)

        if ip_val:
            if not self.arrival_order:
                # Es el primero (R1) -> apunta al ip ANCLA A.B.C.50
                pred_ip = self._anchor_ip()
            else:
                # R2, R3, ... -> apunta al inmediatamente anterior
                pred_ip = self.arrival_order[-1]


        if ip_val and por_defecto:
            self._send_set_sucesor(ip_val, pred_ip)
            # pequeño margen para que el robot procese el /sucesor primero
            self.after(30, lambda ip=ip_val, pred=pred_ip: self._send_label_ack(ip, por_defecto))
            self.arrival_order.append(ip_val)
            
        self.ip_entry_widgets.append((ip_w, tag_w))
        # actualizar contador
        self.auto_detected = len(self.ip_entry_widgets)
        self._update_auto_counter()

    def _repaint_auto_table_from_entries(self):
        """Redibuja la tabla desde lo que ya haya en self.ip_entry_widgets."""
        for child in self.table_frame.winfo_children():
            if child not in [self._disc_hdr_ip, self._disc_hdr_tag]:
                child.destroy()
        entries = [(ip.get(), tag.get()) for ip, tag in self.ip_entry_widgets]
        self.ip_entry_widgets = []
        for ip, tag in entries:
            self._add_row_auto(ip, tag)

    ######------Funciones de Control --------######    
    
    def clickCalibrarButton(self):
         for i in reversed(range(len(self.ip_entry_widgets))):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            UDP_PORT_TX = 1111
            MESSAGE = "E/calibrar/1"
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:", UDP_IP_TX)
            tm.sleep(0.2)

    def clickGuardarButton(self):
        self.letras_lista = [entry[1].get() for entry in self.ip_entry_widgets]

        if len(self.letras_lista)>1:
            self.letras_lista.append("ALL")
    
        self.letter_combobox = customtkinter.CTkComboBox(self.tabviewControlFrame, values=self.letras_lista,variable=self.selected_letter)
        self.letter_combobox.set("L")
        self.letter_combobox.grid(row=2, column=3, padx=10, pady=10)
        #self.tabview.select(self.tabviewControlFrame)

            
        
            
    #Función Iniciar  
    def clickIniciarButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:", UDP_IP_TX)
        tm.sleep(0.5)
        
        for i in range(len(self.ip_entry_widgets)-1):
            UDP_IP_TX = self.ip_entry_widgets[i+1][0].get()
            dist =10
            MESSAGE = "E/cd_ref/" + str(dist)
            print(MESSAGE, UDP_IP_TX)
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

    # Función botón detener
    def clickStopButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:",UDP_IP_TX)

    def switch_event(self):
        #print("switch toggled, current value:", self.switch_var.get())
        if self.switch_var.get() =="on":
            self.clickIniciarButton()
        else:
            self.clickStopButton()

    def switchSave_event(self):
        if self.switchSave_var.get() =="1":
            flag_save = True
        else:
            flag_save = False
            
            
    def updateValueV(self, value,IP):
        self.labelVvalue.configure(text="")
        self.labelVvalue=customtkinter.CTkLabel(self.tabviewControlFrame, text=str(round(value)), fg_color="transparent")
        self.labelVvalue.grid(row=3, column=5, columnspan=1, padx=10, pady=10)
        
        if IP != "ALL":
            UDP_IP_TX = IP
            UDP_PORT_TX = 1111
        
            MESSAGE = "E/cv_ref/" + str(round(value))
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:",IP)
        else:
            for i in range(len(self.ip_entry_widgets)):
                UDP_IP_TX = self.ip_entry_widgets[i][0].get()
                UDP_PORT_TX = 1111
        
                MESSAGE = "E/cv_ref/" + str(round(value))
                print("message:", MESSAGE, "IP:",UDP_IP_TX)

                #sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
               
        
    
    def updateValueD(self, value, IP):
        self.labelDvalue.configure(text="")
        self.labelDvalue.grid(row=4, column=5, columnspan=1, padx=10, pady=10)
        self.labelDvalue.configure(text=str(round(value)))
        
        if IP != "ALL":
            UDP_IP_TX = IP
            UDP_PORT_TX = 1111

            MESSAGE = "E/cd_ref/" + str(round(value))
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:",IP)

        else:
            for i in range(len(self.ip_entry_widgets)):
                UDP_IP_TX = self.ip_entry_widgets[i][0].get()
                UDP_PORT_TX = 1111
            

                MESSAGE = "E/cd_ref/" + str(round(value))
                print("message:", MESSAGE, "IP:",UDP_IP_TX)
                #sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
                
                
    
    def getIP(self,label):
        targetIP=""
        if label != "ALL":
            for entry in self.ip_entry_widgets:
                if entry[1].get() == label:
                    targetIP = entry[0].get()
            return targetIP
        else:
            return "ALL"
        

    def GetData(self, out_data,dato,figure):

        while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            print (self.switchSave_var.get())
            if int(self.switchSave_var.get()) :
                texto = open(file_name, "a")
                texto.write(testo+'\n')
                texto.close()
            #print(self.letter_combobox.get())
            #figure.suptitle("Señal robot " + self.letter_combobox.get(), fontsize=16)
            
            if lista[0] == self.letter_combobox.get():
                out_data[1].append(float(lista[dato]))
                if len(out_data[1]) > 100:
                    out_data[1].pop(0)


    def animate_vel(self):
        
        fig, axes = plt.subplots(3, 1, figsize=(8, 12))
        #fig.suptitle('Señales de monitoreo robot '+ self.letter_combobox.get(), fontsize=14) 
        plt.subplots_adjust(top=0.9, hspace=0.5)  

        lines = []

        for ax in axes:
            line, = ax.plot([], [])
            lines.append(line)

        axes[0].set_title('Velocidad', fontsize=12)
        axes[0].set_xlim(0, 100)
        axes[0].set_ylim(0, 30)

        axes[1].set_title('Distancia predecesor', fontsize=12)
        axes[1].set_xlim(0, 100)
        axes[1].set_ylim(0, 20)

        axes[2].set_title('Ángulo de orientación', fontsize=12)
        axes[2].set_xlim(0, 100)
        axes[2].set_ylim(-1, 1)
        
        """
        for ax in axes:
            ax.set_ylim(0, 30)
            ax.set_xlim(0, 200)
        """
        

        def update_line(num, lines, data):
            for line, d in zip(lines, data):
                line.set_data(range(len(d[1])), d[1])
            return lines
       
        line_ani = animation.FuncAnimation(fig, update_line, fargs=(lines, [gData1, gData2, gData3]),interval=50, blit=True, cache_frame_data=False)

        dataCollector1 = threading.Thread(target=self.GetData, args=(gData1, 5,fig))
        dataCollector2 = threading.Thread(target=self.GetData, args=(gData2, 2,fig))
        dataCollector3 = threading.Thread(target=self.GetData, args=(gData3, 6,fig))

        #dataCollector1.setDaemon(True)

        dataCollector1.start()
        dataCollector2.start()
        dataCollector3.start()


        fig.canvas.manager.window.wm_geometry("+1000+0")
        plt.show()
        
        
        def on_close(event):
            '''dataCollector1.join()  # Esperar a que el hilo termine
            dataCollector2.join()
            dataCollector3.join()
            sys.exit(0)  # Salir del programa'''

        # Configurar el evento de cierre de la ventana
        fig.canvas.mpl_connect('close_event', on_close)

    def start_monitoring_vel(self):
        self.animate_vel()    
    
    
    def on_closing(self):
            # detener descubrimiento si sigue corriendo
        if self.discovery_running:
            self.discovery_running = False
            self.scan_button.configure(text="Buscar robots")
        try:
            self.hello_sock.close()
        except Exception:
            pass
        self.destroy()
            
customtkinter.set_default_color_theme("dark-blue")
customtkinter.set_appearance_mode("light")
app = App()
app.mainloop()