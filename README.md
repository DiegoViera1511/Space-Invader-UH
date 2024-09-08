![header](https://capsule-render.vercel.app/api?type=venom&height=200&text=SpaceInvaders-UH&fontSize=70&color=0:8871e5,100:b678c4&stroke=b678c4)

## Pasos para correr el proyecto
- descargar el proyecto
- abrir una nueva terminal en la carpeta del proyecto
- correr el comando ```gcc -o rungame main.c -lncurses -lpthread```
- correr el comando ```./start.sh```
- disfrutar! :)


## Implementaciones del Proyecto

1. **Planificación:**
   Utilizamos un algoritmo de planificación como FIFO (First In, First Out) para la generación de enemigos en el juego. Esto nos permitirá observar cómo se manejan las tareas en un entorno de ejecución concurrente.

2. **Memoria:**
   Para la generación de enemigos, implementamos diferentes algoritmos de asignación de memoria: 
   - **First Fit:** Asignación de la primera área de memoria adecuada.
   - **Best Fit:** Asignación de la área de memoria más pequeña que satisfaga la necesidad.
   - **Worst Fit:** Asignación de la área de memoria más grande disponible.

3. **Hilos:**
   El juego hace uso de hilos para casi todos los sucesos que suceden en el juego,ya sea pintar en consola,manejar las balas,enemigos,etc,junto con entrada y salida

4. **Sistema de Archivos:**
   Implementamos operaciones de lectura y escritura en archivos de texto (.txt) para almacenar y recuperar información del juego, como puntajes.

## Conclusión

Este proyecto no solo fue un desafío técnico en el desarrollo del juego, sino que también sirvió como una plataforma para aplicar y profundizar en conceptos teóricos de sistemas operativos. 



## Gameplay

![game](https://i.imgur.com/e5kzZHO.png)


#### By Diego Viera and [Pablo Gómez](https://github.com/theleywin)

