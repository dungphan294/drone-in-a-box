# Pinned, not :latest. The README targets ESP-IDF v6.0.2, dependencies.lock is
# resolved against it, and actuator.c has an #if ladder that only knows the
# duty resolutions v6 offers -- so a silent jump to a new major would show up
# as a compile error at best. Keep this in step with IDF_VERSION in
# tools/docker-build.sh. Override for a one-off:
#   docker build --build-arg DOCKER_TAG=v6.0.3 .devcontainer
ARG DOCKER_TAG=v6.0.2
FROM espressif/idf:${DOCKER_TAG}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update -y && apt-get install udev -y

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]