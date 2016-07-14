## Instrucciones para el deploy en los laboratorios

###Setup inicial
1. Hacer **`git clone --recursive https://github.com/sisoputnfrba/tp-2016-1c-Los-mallocados.git`**. El recursive es importante, lo que hace es bajar la última versión de todos los submodulos (parser, commons, scripts-ansisop) desde sus respectivos repos.

2. Ir al root del proyecto y escribir **`sudo ./instalar_dependencias.sh`** para instalar todos los proyectos de la catedra y la librería nuestra (commons, parser, mallocados_library)

3. Fijarse las IP's de las VM y ponerlas en los .ini's (en todos, incluyendo los 4 .ini que hay preparados para cada test)

###Durante el transcurso de los test's
1. **Cambio de configuraciones:** Por defecto están puestos los .ini's del primer test, pero a medida que se avance con los otros hay que ir cambiando el .ini que está en el root del proceso por el correspondiente ubicado en **`NOMBRE_PROCESO/configuraciones_tests`**. Por ejemplo, se paran en el root del swap y hacen **`cp configuraciones_tests/test2_prueba_integral/config.swap.ini .`** (. es el current directory). Esto no hay que hacerlo con la cpu ya que este solo tiene ip's y puertos en su configuracion, y el de la consola solo se va a cambiar el nombre del script a levantar.

2. **Compilar y ejecutar:** Levantar cada proceso yendo a su correspondiente carpeta y ejecutando **`sudo ./compilar_y_levantar_XXXXX.sh`**

3. **Switchear de script en la consola:** El proyecto de la cátedra con los scripts está incluido en nuestro repo como un submódulo, por lo cuál los tenemos todos en su última versión en scripts-ansisop/scripts. Sólo hay que levantarlos, cambiando el valor del parámetro NOMBRE_SCRIPT en el archivo de configuración de la consola.




