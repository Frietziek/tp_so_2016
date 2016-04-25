####Instrucciones instalación
1. Ejecutar el script *instalar_script.sh* como administrador.
2. Linkear (ya sea desde las propiedas del proyecto en eclipse o desde gcc, asi como se hace con las commons, -lcomunicaciones)
3. El orden del linkeo a las librerias es
	* comunicaciones
	* pthread
	* commons
4. Disfrutar, ya se puede usar comunicaciones.h y utiles.h desde cualquier proceso incluyendolos de la siguiente forma:
	* `#include <comunicaciones.h>`
	* `#include <utiles.h>`
