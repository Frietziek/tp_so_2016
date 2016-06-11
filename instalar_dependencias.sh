#! /bin/bash
cd so-commons-library
sudo make install
echo -----	Se ha terminado la instalacion de las commons   ----

cd ../ansisop-parser/parser
sudo make all
sudo make install
echo -----	Se ha terminado la instalacion de ansisop-parser  ----

cd ../../mallocados_library
sudo ./instalar_script.sh
echo -----	Se ha terminado la instalacion de mallocados_library  ----
