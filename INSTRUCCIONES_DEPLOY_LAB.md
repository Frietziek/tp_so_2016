## Instrucciones para el deploy en los laboratorios

1. Hacer **`git clone --recursive https://github.com/sisoputnfrba/tp-2016-1c-Los-mallocados.git`**. El recursive es importante, lo que hace es bajar la última versión de todos los submodulos (parser, commons, scripts-ansisop) desde sus respectivos repos.

2. Ir al root del proyecto y escribir **`sudo ./instalar_dependencias.sh`** para instalar todos los proyectos de la catedra y la librería nuestra (commons, parser, mallocados_library)

3. Levantar cada proceso yendo a su correspondiente carpeta y ejecutando **`sudo ./compilar_y_levantar_XXXXX.sh`**

###FAQ

1. **¿Cómo ejecutar los distintos scripts/programas de la catedra para las pruebas?** El proyecto de la cátedra con los scripts está incluido en nuestro repo como un submódulo, por lo cuál los tenemos todos en su última versión en scripts-ansisop/scripts. Sólo hay que levantarlos, cambiando el valor del parámetro NOMBRE_SCRIPT en el archivo de configuración de la consola.




