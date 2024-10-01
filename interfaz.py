import tkinter as tk
from tkinter import scrolledtext
import subprocess
import csv

# Función para ejecutar la consulta y mostrar el resultado
def ejecutar_consulta():
    # Obtener la consulta del cuadro de entrada
    consulta = entry.get()
    
    # Guardar la consulta en el archivo consulta.txt
    with open("consulta.txt", "w") as file:
        file.write(consulta)
    
    # Ejecutar el código C++ y generar el archivo CSV
    subprocess.run(["./bin/Debug/Pruebas2.exe"])  
    
    # Leer el archivo CSV generado por el programa C++
    mostrar_resultados("records.csv")

# Mostrar los resultados del archivo en el cuadro de texto
def mostrar_resultados(archivo_csv):
    result_text.delete(1.0, tk.END)  # Limpiar el cuadro de resultados
    
    # Leer el archivo CSV
    with open(archivo_csv, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            result_text.insert(tk.END, ' | '.join(row) + '\n')  # Mostrar cada fila

# Crear la ventana principal
window = tk.Tk()
window.title("Consulta")

# Cuadro de entrada para la consulta
tk.Label(window, text="Ingresa la consulta sobre las tablas pokemonavll o pokemonseq:").pack()
entry = tk.Entry(window, width=80)
entry.pack()

# Botón para ejecutar la consulta
btn = tk.Button(window, text="Input", command=ejecutar_consulta)
btn.pack()

# Cuadro de texto para mostrar los resultados
tk.Label(window, text="Resultados:").pack()
result_text = scrolledtext.ScrolledText(window, width=80, height=20)
result_text.pack()

# Iniciar la interfaz
window.mainloop()
