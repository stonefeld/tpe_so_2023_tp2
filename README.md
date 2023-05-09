# Trabajo Práctico Especial - Arquitectura de Computadoras - 2023

Basado en el proyecto [x64BareBones](https://bitbucket.org/RowDaBoat/x64barebones/)

![](./Extras/x64BareBones.png)

## Tabla de contenidos

* [Requisitos](#requisitos)
* [Preparando el entorno](#preparando-el-entorno)
* [Compilando](#compilando)

## Requisitos

Los requisitos necesarios para correr el proyecto son los siguientes:

* `docker`: para emular la compilación en una distribución de Ubuntu/Debian
* `qemu`: permite tomar la imagen compilada y emular un sistema

Desde _Arch Linux_ el procedimiento de instalación es sencillo:

```bash
sudo pacman -S qemu-system-x86 docker
```

## Preparando el entorno

Una vez instalados los requisitos se deben realizar unas tareas antes de poder compilar el proyecto.

### Agregar mi usuario al grupo de docker

Esto permite correr los contenedores, sin necesidad de tener privilegios de administrador.

```bash
sudo groupadd docker
sudo usermod -aG docker $USER
```

### Iniciar el daemon de docker

Con permisos de administrador correr el siguiente comando:

```bash
sudo setsid -f dockerd >/dev/null 2>&1
```

Con docker andando ya se pueden correr todos los comandos relacionados con docker

### Creando una imágen para compilar el proyecto

Para el trabajo práctico en teoría debería usarse una imágen dada por la cátedra. Pero al compilar con dicha imágen saltan errores que, en teoría, tienen que ver con una mala instalación de las librerías de **C**. Hasta que lo arreglen lo solucioné así.

Para la imágen de compilación vamos a crear una basada en **Ubuntu LTS** con los utilitarios necesarios para su compilación.

Para ello correr los siguientes comandos (no necesariamente dentro del proyecto, despues se puede borrar la carpeta crada).

```bash
mkdir ubuntu-essential
cd ubuntu-essential
touch Dockerfile
```

Luego abrir el `Dockerfile` con el editor de texto preferido e ingresar los siguientes contenidos

```dockerfile
FROM ubuntu
RUN apt update && apt install -y build-essential git nasm qemu-system-x86
```

Guardar y cerrar el `Dockerfile` y buildear la imágen:

```bash
docker build -t ubuntu-essential .
```

Una vez completado, si se desea, puede eliminarse la carpeta `ubuntu-essential`.

Al correr `docker images` debería estar presente la imágen recién compilada.

### Compilando la imágen del proyecto

El proyecto cuenta ya con un `Dockerfile` pensado para compilar el proyecto.

Una vez finalizados todos los pasos anteriores, clonar el repositorio y correr el siguiente comando:

```bash
./docker-build.sh
```

## Compilando

Listo! El entorno ya está preparado para compilar el proyecto y correrlo.

Con correr `./docker-run.sh`, si iniciará la imágen de docker que compilará el proyecto, y una vez compilado ejecuta el `qemu` con la imágen del sistema operativo.
