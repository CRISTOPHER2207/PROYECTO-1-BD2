## 1. Introducción
En este proyecto, implementaremos y gestionaremos técnicas de organización de archivos en memoria secundaria, junto con un parser de código SQL, para ejecutar y procesar sentencias relacionadas con la manipulación de datos. Esto permitirá realizar operaciones como inserción, búsqueda y eliminación de registros, facilitando el manejo eficiente de grandes volúmenes de información.

## 2. Objetivo del Proyecto

La organización física de los archivos en memoria secundaria es fundamental para el desarrollo y funcionamiento de los sistemas de gestión de bases de datos (DBMS). En un mundo donde el manejo eficiente de la información es crucial, disponer de una estructura adecuada para los archivos almacenados en memoria secundaria asegura un acceso rápido, confiable y eficiente a los datos. En este proyecto, nos enfocaremos en aplicar diversas técnicas de organización de archivos, tales como la organización secuencial, AVL y hashing extensible, para optimizar la gestión de registros en memoria secundaria.

## 3.AVL FILE

El código está compuesto principalmente por una clase llamada AVLFile, que encapsula las operaciones del árbol AVL sobre un archivo binario. A continuación, se desglosan los elementos clave del código:

### 3.1. Atributos Principales de la Clase
- *filename*: Es una cadena de texto que almacena el nombre del archivo binario donde se guardarán los registros. Se asegura de que los datos estén correctamente almacenados y gestionados en un archivo de extensión .bin.
- *pos_root*: Un entero largo (long) que representa la posición en el archivo donde se encuentra la raíz del árbol. Es el punto de partida para cualquier operación de búsqueda, inserción o eliminación en el árbol AVL.

### ***3.2. Métodos Públicos***

- ***Constructor (AVLFile)***: Inicializa la clase, verificando si el archivo binario existe. Si no existe, se crea uno nuevo. La raíz del árbol se inicializa en la posición cero.

- ***find(TK key)***: Busca un registro en el archivo binario cuya clave coincida con la clave proporcionada. Devuelve un objeto de tipo `Record` que corresponde al registro encontrado.

- ***search(TK key)***: Similar a `find`, pero permite realizar una búsqueda más compleja que puede devolver múltiples registros que coincidan con la clave.

- ***add(Record record)***: Inserta un nuevo registro en el árbol AVL, ajustando la estructura del árbol para mantener el balance.

- ***remove(T key)***: Elimina un registro del archivo binario basado en su clave. Tras la eliminación, se asegura de que el árbol mantenga su balanceo.

- ***inorder()***: Devuelve un vector con los registros del árbol en orden ascendente.

- ***rangeSearch(TK begin, TK end)***: Realiza una búsqueda de rango, devolviendo todos los registros cuya clave se encuentre entre los valores `begin` y `end`.

### ***3.3. Métodos Privados***

#### ***3.3.1. Inserción***
El método ***insert(long pos_node, Record record, fstream& file)*** es responsable de insertar un nuevo registro en el árbol AVL. La lógica sigue los siguientes pasos:

- Si el archivo está vacío, el nuevo registro se escribe directamente.
- Si el archivo ya contiene registros, se recorre el árbol desde la raíz para encontrar la posición correcta de inserción.
- Una vez insertado el registro, se llama a ***updateHeight()*** para ajustar las alturas de los nodos y, si es necesario, se aplica alguna rotación para mantener el balance del árbol.

#### ***3.3.2. Eliminación***
El método ***remove(long pos_node, T key, fstream& file)*** elimina un registro, siguiendo un procedimiento que garantiza la preservación del balance del árbol. Si el nodo a eliminar tiene dos hijos, se busca el sucesor o predecesor en el subárbol para reemplazar el nodo a eliminar, manteniendo la estructura del árbol.

#### ***3.3.3. Balanceo y Rotaciones***
Para mantener la propiedad de balance del árbol AVL, se implementan métodos para realizar las rotaciones simples y dobles. Las rotaciones son necesarias cuando la diferencia de alturas entre los subárboles excede 1. Las principales funciones involucradas son:

- ***balance(long pos_node, fstream& file)***: Determina si un nodo está desbalanceado y aplica las rotaciones correspondientes.
- ***left_rota(long pos_node, fstream& file)***: Realiza una rotación hacia la izquierda sobre el nodo actual.
- ***right_rota(long pos_node, fstream& file)***: Realiza una rotación hacia la derecha.

#### ***3.3.4. Búsqueda en Rango***
El método ***RangeSearch(long pos_node, const TK& begin, const TK& end)*** realiza una búsqueda en un rango de claves especificadas, devolviendo todos los registros que se encuentren entre ***begin*** y ***end***. La búsqueda es eficiente, ya que solo se exploran los subárboles necesarios, aprovechando la estructura de árbol binario de búsqueda.


