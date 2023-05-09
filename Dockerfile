FROM ubuntu-essential
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o user
RUN useradd -m -u $UID -g $GID -o -s /bin/bash user
USER user
CMD cd sources && make clean all
