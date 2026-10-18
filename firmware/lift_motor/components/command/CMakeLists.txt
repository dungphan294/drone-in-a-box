# No REQUIRES: this component must stay free of ESP-IDF so it can be built and
# unit tested on a host.
idf_component_register(SRCS "command.c"
                    INCLUDE_DIRS "include")
