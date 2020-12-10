---------------------------------------
# Profiling
## Subrutinas que consumen mas tiempo
Como se muestran en las graficas, la subrutina que consume mas tiempo es el algortimo de levenshtein, por lo cual el algoritmo que precisa mas optimizar.

Tambien se puede observar cual es la linea de codigo que consume mas tiempo.

![Linea de codigo que consuma mas tiempo](concurrentegabrielrubio/Proyectos/Proyecto_1/levdist-pthreads/codigo.jpeg)

# Implementacion del algoritmo paralelo
## Rastreo del algoritmo
Con el fin de comprender de manera eficaz el algoritmo paralelo mencionado en [
Parallel Algorithm for Approximate String Matching with K Differences](https://ieeexplore.ieee.org/document/6665373), se realizo una rastreo del algorimo con dos cadenas de texto, siendo el patron "edit" y el texto "needs", dicho rastreo se muestra en la siguiente imagen.

![Rastreo](concurrentegabrielrubio/Proyectos/Proyecto_1/levdist-pthreads/rastreo.jpeg)

-----------------------------------
