##1. Introducción
En el contexto de estructuras de datos avanzadas, los árboles AVL (del nombre de sus creadores Adelson-Velsky y Landis) son árboles binarios de búsqueda auto-balanceados. Esta propiedad garantiza que el árbol mantenga una estructura balanceada, donde la diferencia de alturas entre los subárboles izquierdo y derecho de cualquier nodo no exceda 1. Esto mejora la eficiencia de las operaciones de búsqueda, inserción y eliminación a 
O(logn), siendo particularmente útil en aplicaciones donde la eficiencia y el acceso rápido a los datos son críticos.
Este informe detalla la implementación de un árbol AVL orientado al manejo de registros en un archivo binario. Se describen las principales técnicas y estructuras utilizadas en el código, así como el propósito de cada una dentro de la lógica de balanceo y gestión de los registros.

##2. Objetivo del Proyecto

El proyecto tiene como objetivo implementar un árbol AVL en C++ que gestione un archivo binario donde se almacenan registros de datos. El programa permite insertar, buscar, eliminar y recuperar registros en orden, utilizando la estructura del árbol AVL para garantizar que estas operaciones se realicen de manera eficiente.
El archivo binario actúa como un almacenamiento persistente, lo que permite que los registros persistan entre diferentes ejecuciones del programa. El balanceo del árbol AVL asegura que las operaciones sobre los datos sean rápidas, independientemente del número de registros.
