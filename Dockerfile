FROM agodio/itba-so:1.0
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o user
RUN useradd -m -u $UID -g $GID -o -s /bin/bash user
USER user
CMD cd sources && cd Toolchain && make && cd .. && make clean all
