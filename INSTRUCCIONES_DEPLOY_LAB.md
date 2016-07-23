## Instrucciones para el deploy en los laboratorios

###Setup inicial
1. Hacer **`git clone --recursive https://github.com/sisoputnfrba/tp-2016-1c-Los-mallocados.git`**. El recursive es importante, lo que hace es bajar la última versión de todos los submodulos (parser, commons, scripts-ansisop) desde sus respectivos repos.

2. Ir al root del proyecto y escribir **`sudo ./instalar_dependencias.sh`** para instalar todos los proyectos de la catedra y la librería nuestra (commons, parser, mallocados_library)

3. Fijarse las IP's de las VM y ponerlas en los .ini's (en todos, incluyendo los 4 .ini que hay preparados para cada test)

###Durante el transcurso de los test's
1. **Cambio de configuraciones:** Por defecto están puestos los .ini's del primer test, pero a medida que se avance con los otros hay que ir ejecutando los scripts de **`./copiar_configuraciones.sh`** del test a ejecutar, ubicados en la carpeta tests dentro del root.

2. **Compilar y ejecutar:** Levantar cada proceso yendo a su correspondiente carpeta y ejecutando **`./compilar_y_levantar_XXXXX.sh`**

3. **Switchear de script en la consola:** El proyecto de la cátedra con los scripts está incluido en nuestro repo como un submódulo, por lo cuál los tenemos todos en su última versión en scripts-ansisop/scripts. Sólo hay que levantarlos, cambiando el valor del parámetro NOMBRE_SCRIPT en el archivo de configuración de la consola.




