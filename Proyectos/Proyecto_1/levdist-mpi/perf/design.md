El diseño escogido se basa en un esquema estatico por bloques, ya que este permite repartir el trabajo de manera mas eficiente que otro scheduling, aparte de que para trabajar con la libreria de paralelismo de memoria distribuida (OpenMPI) se facilita trabajar con este tipo de esquema.

Un ejemplo de este modelo es el siguiente, tomando en cuenta de que se le asigno al programa 4 procesos y 5 archivos a comparar.

Las comparaciones disponibles (5!/2!3! = 10)son las siguientes:

arch1-arch2 arch1-arch3 arch1-arch4 arch1-arch5 
arch2-arch3 arch2-arch4 arch2-arch5
arch3-arch4 arch3-arch5
arch4-arch5 


Proceso 0: arch1-arch2 arch1-arch3 arch1-arch4
Proceso 1: arch1-arch5 arch2-arch3 arch2-arch4 
Proceso 2: arch2-arch5 arch3-arch4
Proceso 3: arch3-arch5 arch4-arch5

Un modelo basado en un esquema dinamico depende de cuando las comparaciones terminen, sin embargo, para predecir en que lugar va a caer el resultado y mandar eso al proceso 0 para que este reporte cual es la distancia no es comodo y puede llegar a ser menos eficiente por el tiempo que dura hacer los calculos, en lugar de eso el esquema estatico es predecible y eficiente, ademas la implementacion es considerablemente mas sencilla.

Puntos a tomar en cuenta 

    Realizarán el análisis de argumentos?
    - Si,ya que MPI necesita de ellos para obtener la solucion.

    Levantarán el listado de archivos?
    - Si, cada proceso tiene la lista de archivos para poder hacer las comparaciones correspondientes

    Administrarán la estructura de datos con las parejas de archivos a comparar,
    - Cada proceso tiene la misma estructura pero rellenan la distancia unicamente a los archivos que le corresponden, por lo que cada proceso distinto al 0 debe de reportar sus distancias y el proceso 0 recolecta dichas distancias para almacenarla en su estructura de datos (Se va implementara un vector en lugar del arbol).

    Distribuirán (scheduling) las parejas de archivos
    - Si, cada proceso tiene sus parejas de archivos a los cuales deben de sacarles distancias.

    Integrarán los resultados?
    - Como se menciona en uno de los puntos anteriores, cada proceso debe de reportar al proceso 0 las distancias obtenidas.

    Ordenarán y presentarán de los resultados al usuario ?
    - Unicamente el proceso 0 ordena y muestra los resultados

Usando un ejemplo para preveer como se va a comportar el algoritmo, se adjunta una imagen.



